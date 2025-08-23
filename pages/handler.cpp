#include "handler.hpp"
#include "../act/auth.hpp"
Pnix Server;


EXPORT int login(struct mg_connection *connection, void *callback) {
    // Check if user has a valid auth_token cookie
    const char* cookie_header = mg_get_header(connection, "Cookie");
    if (cookie_header) {
        std::string cookies(cookie_header);
        std::string tokenKey = "auth_token=";
        size_t tokenPos = cookies.find(tokenKey);

        if (tokenPos != std::string::npos) {
            size_t start = tokenPos + tokenKey.length();
            size_t end = cookies.find(";", start);
            std::string token = cookies.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

            // Check if the token matches the admin hash from DB
            Db db;
            if (db.Open()) {
                std::string adminHash = db.GetSingleResult("SELECT Password FROM user WHERE Role='Admin';");
                db.Close();

                if (token == adminHash) {
                    // If token matches, redirect to /admin
                    mg_printf(connection,
                        "HTTP/1.1 302 Found\r\n"
                        "Location: /admin\r\n"
                        "Content-Length: 0\r\n"
                        "Connection: close\r\n\r\n");
                    return 302;
                }
            }
        }
    }

    // If no valid token or not admin, show login page
    Server.home("public/views/login.htpp", connection);
    return 200;
}

EXPORT int static_file(struct mg_connection *connection, void *callback) {
    // Get the request URI
    const struct mg_request_info *req_info = mg_get_request_info(connection);
    std::string uri = req_info->local_uri;
    
    // Serve the static file from public directory
    Server.static_serve("public" + uri, connection);
    return 200;
}
EXPORT int locked_static_file(struct mg_connection *connection, void *callback) {
    // Get the request URI
    const struct mg_request_info *req_info = mg_get_request_info(connection);
    std::string uri = req_info->local_uri;
    Db db;

    // Open DB
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Database error</h1></body></html>");
        return 500;
    }

    // Get admin password hash (assuming this is your token)
    std::string adminHash = db.GetSingleResult("SELECT Password FROM user WHERE Role='Admin';");
    db.Close();

    // Get Cookie header
    const char* cookie_header = mg_get_header(connection, "Cookie");
    if (!cookie_header) {
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
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }

    // Extract auth_token value
    size_t start = tokenPos + tokenKey.length();
    size_t end = cookies.find(";", start);
    std::string token = cookies.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

    // Compare cookie token to stored admin password hash
    if (token != adminHash) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }
    // Serve the static file from public directory
    Server.static_serve("public" + uri, connection);
    return 200;
}
EXPORT int login_redirect(struct mg_connection *connection, void *callback) {
    mg_printf(connection,
        "HTTP/1.1 302 Found\r\n"
        "Location: /login\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n");
    return 302;
}
EXPORT int dashboard_admin(struct mg_connection *connection,void *callback){
    Server.home("public/layout.html",connection);

    return 302;
}
EXPORT int auth(struct mg_connection *connection, void *callbackdata) {
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
    std::string hashed_password = hash(password);
    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Database error\"}");
        return 500;
    }
    bool isAdmin = db.AuthenticateAsAdmin(db,nama,hashed_password);
    std::string sql;
    JsonCallbackData jsonData;

    // Admin login
    if (isAdmin) {
        mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Set-Cookie: auth_token=%s; Path=/; HttpOnly\r\n"
        "Connection: close\r\n\r\n"
        "{\"success\": true, \"redirect\": \"/admin\"}",
        hashed_password.c_str());
        return 200;
    } else {
        // Siswa login
        sql = "SELECT nama, kelas FROM siswa WHERE nama='" + nama + "' AND token='" + hashed_password + "';";
    }

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

    // Success
    std::string redirect = (nama == "Admin") ? "/admin" : "/aum";
    json user = jsonData.jsonArray[0];

    std::string response_body = json{
        {"success", true},
        {"redirect", redirect},
        {"user", user}
    }.dump();

    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Set-Cookie: auth_token=%s; Path=/; HttpOnly\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n"
        "%s",
        hashed_password.c_str(),
        response_body.size(),
        response_body.c_str()
    );

    return 200;
}

EXPORT int logout(struct mg_connection *connection, void *callback) {
    // Clear the auth_token cookie
    mg_printf(connection,
        "HTTP/1.1 302 Found\r\n"
        "Location: /login\r\n"
        "Set-Cookie: auth_token=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n\r\n");
    return 302;
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
EXPORT int me(struct mg_connection *connection, void *callback) {
    const char* cookie_header = mg_get_header(connection, "Cookie");
    if (!cookie_header) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"error\": \"Missing auth token\"}");
        return 401;
    }

    std::string cookies(cookie_header);
    std::string tokenKey = "auth_token=";
    size_t tokenPos = cookies.find(tokenKey);
    if (tokenPos == std::string::npos) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"error\": \"Token not found\"}");
        return 401;
    }

    size_t start = tokenPos + tokenKey.length();
    size_t end = cookies.find(";", start);
    std::string token = cookies.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"error\": \"Database error\"}");
        return 500;
    }

    JsonCallbackData jsonData;
    std::string safe_token = db.Escape(token);
    std::string sql = "SELECT nama, kelas FROM siswa WHERE token = '" + safe_token + "';";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db.db, sql.c_str(), JsonCallback, &jsonData, &errMsg);
    db.Close();

    if (rc != SQLITE_OK || jsonData.jsonArray.empty()) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"error\": \"Invalid or expired token\"}");
        return 401;
    }

    std::string response = jsonData.jsonArray[0].dump();
    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n\r\n"
        "%s",
        response.size(),
        response.c_str());

    return 200;
}

EXPORT int admin(struct mg_connection *connection, void *callback) {
    const struct mg_request_info *req_info = mg_get_request_info(connection);
    Db db;

    // Open DB
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Database error</h1></body></html>");
        return 500;
    }

    // Get admin password hash (assuming this is your token)
    std::string adminHash = db.GetSingleResult("SELECT Password FROM user WHERE Role='Admin';");
    db.Close();

    // Get Cookie header
    const char* cookie_header = mg_get_header(connection, "Cookie");
    if (!cookie_header) {
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
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }

    // Extract auth_token value
    size_t start = tokenPos + tokenKey.length();
    size_t end = cookies.find(";", start);
    std::string token = cookies.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

    // Compare cookie token to stored admin password hash
    if (token != adminHash) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }

    // Serve the admin page
    Server.SSR("public/admin/admin.htpp", connection);
    return 200;
}

EXPORT int admin_crud_user(struct mg_connection *connection, void *callback) {
    const struct mg_request_info *req_info = mg_get_request_info(connection);
    Db db;

    // Open DB
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Database error</h1></body></html>");
        return 500;
    }

    // Get admin password hash (assuming this is your token)
    std::string adminHash = db.GetSingleResult("SELECT Password FROM user WHERE Role='Admin';");
    db.Close();

    // Get Cookie header
    const char* cookie_header = mg_get_header(connection, "Cookie");
    if (!cookie_header) {
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
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }

    // Extract auth_token value
    size_t start = tokenPos + tokenKey.length();
    size_t end = cookies.find(";", start);
    std::string token = cookies.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

    // Compare cookie token to stored admin password hash
    if (token != adminHash) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }

    // Serve the admin page
    Server.SSR("public/admin/user.htpp", connection);
    return 200;
}

EXPORT int Save_Kategori(struct mg_connection *connection,void *callback){
    char post_data[1024] = {0};
    int post_data_lenght = mg_read(connection,post_data,sizeof(post_data));
    post_data[post_data_lenght] ='\0'; 
    std::string str_post_data(post_data);
    size_t pos = str_post_data.find(',');
    if(pos == std::string::npos){
       return 400;
    }
    std::string nama_kategory = str_post_data.substr(0,pos);
    std::string id_kategory = str_post_data.substr(pos+1);

    std::cout << nama_kategory << std::endl;
    std::cout << std::stoi(id_kategory) << std::endl;
    std::string insert = "INSERT INTO bidang_masalah (id,nama_bidang_masalah) "
                        "VALUES (" + id_kategory + ", '" + nama_kategory + "');";
    Db db;
    if(!db.Open()) return 1;
    if(!db.Query(insert)){
        db.Close();
        return 500;
    }
    db.Close();
    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Lenght: %zu\r\n"
    );
    return 302;
};


EXPORT int Read_Kategori(struct mg_connection *connection, void *callback) {
    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 21\r\n\r\n"
            "Failed to open DB.\n");
        return 500;
    }

    JsonCallbackData jsonData;
    std::string select_sql = "SELECT * FROM soal_masalah;";
    char* errMsg = nullptr;

    int rc = sqlite3_exec(db.db, select_sql.c_str(), JsonCallback, &jsonData, &errMsg);
    db.Close();

    if (rc != SQLITE_OK) {
        std::cerr << "Select error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 23\r\n\r\n"
            "Failed to query table.\n");
        return 500;
    }

    std::string response = jsonData.jsonArray.dump();

    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        response.length(),
        response.c_str()
    );

    return 200;
}

EXPORT int Save_User(struct mg_connection *connection, void *callback) {
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

    std::string name = parsed.value("nama", "");
    std::string password = parsed.value("password", "");
    std::string hashed_password = hash(password);
    std::string kelas = parsed.value("kelas", "");
    std::string query = "INSERT INTO siswa (nama,password,token,kelas) VALUES ('"+name+"','"+password+"','"+hashed_password+"','"+kelas+"');";
    std::cout << query << std::endl;

    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Database error\"}");
        return 500;
    }
    db.Query(query);
    db.Close();

    // Create a response JSON
    std::string response_body = R"({"success": true, "message": "User saved successfully"})";

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
EXPORT int submit(struct mg_connection *connection, void *callback) {
    char post_data[1024] = {0};
    int post_data_length = mg_read(connection, post_data, sizeof(post_data) - 1);
    post_data[post_data_length] = '\0'; 
    std::string body(post_data);
    Db db;
    // Print raw data for debugging
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

    // The input is an array, so we'll iterate through it
    if (!parsed.is_array()) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Expected an array\"}");
        return 400;
    }

    // Iterate through each element of the array
    for (const auto& item : parsed) {
        db.Open();
        // Extract relevant data
        std::string nama = item.value("nama", "");
        std::string kelas = item.value("kelas", "");
        std::string id = item.value("id", "");
        std::string nama_bidang_masalah = item.value("nama_bidang_masalah", "");
        

        // You can process the data here, e.g., save it to the database, etc.
        // For example, you might want to insert into a table based on this data.
        // Example (pseudo-code):
        std::string query = "INSERT INTO hasil (nama, kelas, soal_masalah_id, soal_masalah_kategori) VALUES ('"+nama+"','"+kelas+"','"+id+"','"+nama_bidang_masalah+"')";
        db.QueryWithResults(query);
        db.Close();
    }

    // Create a success response
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
