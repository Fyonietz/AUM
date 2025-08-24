#include "handler.hpp"
#include "../act/auth.hpp"
Pnix Server;
bool open_bk = false;

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
    bool isBK = db.AuthenticateAsBK(db,nama,hashed_password);
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
    } else if(isBK){
         mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Set-Cookie: auth_token=%s; Path=/; HttpOnly\r\n"
        "Connection: close\r\n\r\n"
        "{\"success\": true, \"redirect\": \"/dashboard\"}",
        hashed_password.c_str());
        open_bk=true;
        return 200;
    }else {
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
    std::string redirect;
    if (nama == "Admin") {
    redirect = "/admin";
    } else if(nama=="BK_SMK4") {
    redirect = "/dashboard";
    }else{
        redirect = "/aum";
    }

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
