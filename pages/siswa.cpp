#include "handler.hpp"
#include "../act/auth.hpp"


EXPORT int auth_siswa(struct mg_connection *connection, void *callbackdata) {
    char post_data[1024] = {0};
    int post_data_length = mg_read(connection, post_data, sizeof(post_data) - 1);
    post_data[post_data_length] = '\0';

    std::string body(post_data);
    json parsed;

    try {
        parsed = json::parse(body);
    } catch (...) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Invalid JSON\"}");
        return 400;
    }

    std::string nama = parsed.value("nama", "");
    std::string password = parsed.value("password", "");

    if (nama.empty() || password.empty()) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Missing credentials\"}");
        return 400;
    }

    std::string hashed_password = hash(password);  // already available in your code

    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Database error\"}");
        return 500;
    }

    // Query siswa table
    std::string sql = "SELECT nama, kelas FROM siswa WHERE nama='" + db.Escape(nama) + "' AND token='" + db.Escape(hashed_password) + "';";
    JsonCallbackData jsonData;
    char* errMsg = nullptr;

    int rc = sqlite3_exec(db.db, sql.c_str(), JsonCallback, &jsonData, &errMsg);
    db.Close();

    if (rc != SQLITE_OK || jsonData.jsonArray.empty()) {
        if (errMsg) sqlite3_free(errMsg);
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Invalid credentials\"}");
        return 401;
    }

    // Success – set cookie and return success JSON
    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Set-Cookie: auth_token=%s; Path=/; HttpOnly\r\n"
        "Connection: close\r\n\r\n"
        "{\"success\": true, \"redirect\": \"/aum\"}",
        hashed_password.c_str());

    return 200;
}
EXPORT int submit(struct mg_connection *connection, void *callback) {
    std::string body;
    char buffer[2048];
    int bytes_read;

    // Read the full POST body
    while ((bytes_read = mg_read(connection, buffer, sizeof(buffer))) > 0) {
        body.append(buffer, bytes_read);
    }

    std::cout << "Received POST data: " << body << std::endl;

    json parsed;
    try {
        parsed = json::parse(body);
    } catch (...) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Invalid JSON\"}");
        return 400;
    }

    if (!parsed.is_array()) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Expected an array\"}");
        return 400;
    }

    Db db;

    for (const auto& item : parsed) {
        db.Open();
        std::string nama = item.value("nama", "");
        std::string kelas = item.value("kelas", "");
        std::string id = item.value("id", "");
        std::string nama_bidang_masalah = item.value("nama_bidang_masalah", "");

        std::string query = "INSERT INTO hasil (nama, kelas, soal_masalah_id, soal_masalah_kategori) VALUES ('" +
                            nama + "','" + kelas + "','" + id + "','" + nama_bidang_masalah + "')";
        db.QueryWithResults(query);
        db.Close();
    }

    std::string response_body = R"({"success": true, "message": "Data processed successfully"})";

    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n"
        "%s",
        response_body.length(),
        response_body.c_str()
    );

    return 200;
}

EXPORT int aum(struct mg_connection *connection, void *callback) {
    const char* cookie_header = mg_get_header(connection, "Cookie");
    if (!cookie_header) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized - No cookie</h1></body></html>");
        return 401;
    }
    if(open_bk==0){
         mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }
    std::string cookies(cookie_header);
    std::string tokenKey = "auth_token=";
    size_t tokenPos = cookies.find(tokenKey);
    if (tokenPos == std::string::npos) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized - Token not found</h1></body></html>");
        return 401;
    }

    // Extract token
    size_t start = tokenPos + tokenKey.length();
    size_t end = cookies.find(";", start);
    std::string token = cookies.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Database error</h1></body></html>");
        return 500;
    }

    // Get student info from siswa table using token
    std::string sql = "SELECT nama, kelas FROM siswa WHERE token = '" + db.Escape(token) + "';";
    JsonCallbackData jsonData;

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db.db, sql.c_str(), JsonCallback, &jsonData, &errMsg);
    db.Close();

    if (rc != SQLITE_OK || jsonData.jsonArray.empty()) {
        if (errMsg) sqlite3_free(errMsg);
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized - Invalid token</h1></body></html>");
        return 401;
    }

    // Extract values
    std::string nama = jsonData.jsonArray[0]["nama"];
    std::string kelas = jsonData.jsonArray[0]["kelas"];

    // Pass data to template
    json context;
    context["nama"] = nama;
    context["kelas"] = kelas;

    Server.SSR("public/views/AUM.htpp", connection);
    return 200;
}