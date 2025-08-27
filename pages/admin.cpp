#include "handler.hpp"
#include "../act/auth.hpp"



EXPORT int dashboard_admin(struct mg_connection *connection,void *callback){
    if(!open_bk){ 
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }
      Server.home("public/layout.html",connection);
        return 302;
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
EXPORT int admin_crud_kategori(struct mg_connection *connection, void *callback) {
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
    Server.SSR("public/admin/kategori.htpp", connection);
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