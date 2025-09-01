
#include "handler.hpp"
#include "../act/auth.hpp"

EXPORT int dashboard_admin(struct mg_connection *connection, void *callback) {
    if (!open_bk) { 
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }
    Server.home("public/layout.html", connection);
    return 302;
}

EXPORT int admin(struct mg_connection *connection, void *callback) {
    const struct mg_request_info *req_info = mg_get_request_info(connection);
    Db db;

    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Database error</h1></body></html>");
        return 500;
    }

    std::string adminHash = db.GetSingleResult("SELECT Password FROM user WHERE Role='Admin';");
    db.Close();

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

    size_t start = tokenPos + tokenKey.length();
    size_t end = cookies.find(";", start);
    std::string token = cookies.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

    if (token != adminHash) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }

    Server.SSR("public/admin/admin.htpp", connection);
    return 200;
}

EXPORT int admin_crud_user(struct mg_connection *connection, void *callback) {
    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Database error</h1></body></html>");
        return 500;
    }

    std::string adminHash = db.GetSingleResult("SELECT Password FROM user WHERE Role='Admin';");
    db.Close();

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

    size_t start = tokenPos + tokenKey.length();
    size_t end = cookies.find(";", start);
    std::string token = cookies.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

    if (token != adminHash) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }

    Server.SSR("public/admin/user.htpp", connection);
    return 200;
}

EXPORT int admin_crud_kategori(struct mg_connection *connection, void *callback) {
    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Database error</h1></body></html>");
        return 500;
    }

    std::string adminHash = db.GetSingleResult("SELECT Password FROM user WHERE Role='Admin';");
    db.Close();

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

    size_t start = tokenPos + tokenKey.length();
    size_t end = cookies.find(";", start);
    std::string token = cookies.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

    if (token != adminHash) {
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }

    Server.SSR("public/admin/kategori.htpp", connection);
    return 200;
}

// Updated handler.cpp functions to match your database schema


// 1. CREATE KATEGORI - Updated to work with your schema
EXPORT int Save_Kategori(struct mg_connection *connection, void *callback) {
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
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Invalid JSON\"}");
        return 400;
    }

    std::string kategori = parsed.value("kategori", "");
    std::string code = parsed.value("code", ""); // 3-digit code like "JDK"
    
    if (kategori.empty() || code.empty()) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Missing kategori name or code\"}");
        return 400;
    }

    // Validate code is 3 characters
    if (code.length() != 3) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Code must be exactly 3 characters\"}");
        return 400;
    }

    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Database error\"}");
        return 500;
    }

    // Check if kategori name already exists
    std::string check_name_query = "SELECT COUNT(*) FROM bidang_masalah WHERE nama_bidang_masalah = '" + kategori + "';";
    std::string name_count = db.GetSingleResult(check_name_query);
    
    if (name_count != "0") {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Kategori name already exists\"}");
        return 400;
    }

    // Check if code already exists (using LIKE to match pattern)
    std::string check_code_query = "SELECT COUNT(*) FROM bidang_masalah WHERE nama_bidang_masalah LIKE '%" + code + "%';";
    std::string code_count = db.GetSingleResult(check_code_query);
    
    if (code_count != "0") {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Code already exists\"}");
        return 400;
    }

    // Get next available ID
    std::string max_id_query = "SELECT COALESCE(MAX(id), 0) + 1 AS next_id FROM bidang_masalah;";
    std::string next_id = db.GetSingleResult(max_id_query);
    
    if (next_id.empty()) {
        next_id = "1";
    }

    // Store kategori name with code prefix: "JDK - Category Name"
    std::string full_kategori_name = code + " - " + kategori;

    // Escape single quotes
    std::string escaped_kategori = full_kategori_name;
    size_t pos = 0;
    while ((pos = escaped_kategori.find("'", pos)) != std::string::npos) {
        escaped_kategori.replace(pos, 1, "''");
        pos += 2;
    }

    std::string insert = "INSERT INTO bidang_masalah (id, nama_bidang_masalah) VALUES (" + next_id + ", '" + escaped_kategori + "');";

    if (!db.Query(insert)) {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Failed to insert kategori\"}");
        return 500;
    }

    db.Close();

    std::string response = "{\"success\": true, \"message\": \"Kategori saved successfully\", \"code\": \"" + code + "\"}";
    
    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n\r\n"
        "%s", response.c_str());

    return 200;
}// 2. CREATE SUB KATEGORI - Updated to work with your schema
EXPORT int Save_SubKategori(struct mg_connection *connection, void *callback) {
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
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Invalid JSON\"}");
        return 400;
    }

    std::string kategori_code = parsed.value("kategori_code", ""); // 3-digit code
    std::string subkategori = parsed.value("subkategori", "");

    if (kategori_code.empty() || subkategori.empty()) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Missing kategori code or subkategori name\"}");
        return 400;
    }

    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Database error\"}");
        return 500;
    }

    // Find kategori by code using LIKE
    std::string kategori_name_query = "SELECT nama_bidang_masalah FROM bidang_masalah WHERE nama_bidang_masalah LIKE '" + kategori_code + "%' LIMIT 1;";
    std::string kategori_name = db.GetSingleResult(kategori_name_query);

    if (kategori_name.empty()) {
        db.Close();
       
mg_printf(connection,
    "HTTP/1.1 400 Bad Request\r\n"
    "Content-Type: application/json\r\n\r\n"
    "{\"success\": false, \"message\": \"Kategori not found for code: %s\"}",
    kategori_code.c_str());
;
        return 400;
    }

    // Escape single quotes in subkategori name
    std::string escaped_subkategori = subkategori;
    size_t pos = 0;
    while ((pos = escaped_subkategori.find("'", pos)) != std::string::npos) {
        escaped_subkategori.replace(pos, 1, "''");
        pos += 2;
    }

    // Check if subkategori already exists in this kategori
    std::string check_query = "SELECT COUNT(*) FROM soal_masalah WHERE nama_bidang_masalah = '" + kategori_name + "' AND nama_soal_masalah = '" + escaped_subkategori + "';";
    std::string count_result = db.GetSingleResult(check_query);
    
    if (count_result != "0") {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Subkategori already exists in this kategori\"}");
        return 400;
    }

    // Get next available ID for subkategori
    std::string max_id_query = "SELECT COALESCE(MAX(id), 0) + 1 AS next_id FROM soal_masalah;";
    std::string next_id = db.GetSingleResult(max_id_query);
    
    if (next_id.empty()) {
        next_id = "1";
    }

    std::string insert = "INSERT INTO soal_masalah (id, nama_bidang_masalah, nama_soal_masalah) VALUES (" + 
                        next_id + ", '" + kategori_name + "', '" + escaped_subkategori + "');";

    if (!db.Query(insert)) {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Failed to insert subkategori\"}");
        return 500;
    }

    db.Close();

    std::string response = "{\"success\": true, \"message\": \"Subkategori saved successfully\"}";
    
    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n\r\n"
        "%s", response.c_str());

    return 200;
}

EXPORT int Read_Kategori_Total(struct mg_connection *connection, void *callback) {
    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "[]");
        return 500;
    }

    JsonCallbackData jsonData;
    std::string select_sql = "SELECT * FROM bidang_masalah ORDER BY id;";
    char* errMsg = nullptr;

    int rc = sqlite3_exec(db.db, select_sql.c_str(), JsonCallback, &jsonData, &errMsg);
    db.Close();

    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "[]");
        return 500;
    }

    // Process the JSON to extract codes
    json processedArray = json::array();
    for (auto& item : jsonData.jsonArray) {
        if (item.contains("nama_bidang_masalah")) {
            std::string full_name = item["nama_bidang_masalah"];
            // Extract code (first 3 characters before " - ")
            size_t dash_pos = full_name.find(" - ");
            if (dash_pos != std::string::npos && dash_pos == 3) {
                std::string code = full_name.substr(0, 3);
                std::string name = full_name.substr(5); // Skip "XXX - "
                
                json new_item = item;
                new_item["code"] = code;
                new_item["display_name"] = name;
                processedArray.push_back(new_item);
            } else {
                // Fallback if format doesn't match
                processedArray.push_back(item);
            }
        }
    }

    std::string response = processedArray.dump();

    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n\r\n"
        "%s",
        response.length(),
        response.c_str());

    return 200;
}
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

// 4. READ SUB KATEGORI by kategori code
EXPORT int Read_SubKategori_By_Code(struct mg_connection *connection, void *callback) {
    const struct mg_request_info *req_info = mg_get_request_info(connection);
    std::string query_string = req_info->query_string ? req_info->query_string : "";
    std::cout <<query_string << std::endl; 
    // Parse kategoriCode from query string
    std::string kategori_code;
    size_t pos = query_string.find("kategoriCode=");
    if (pos != std::string::npos) {
        size_t start = pos + 13; // length of "kategoriCode="
        size_t end = query_string.find("&", start);
        kategori_code = query_string.substr(start, (end == std::string::npos) ? std::string::npos : end - start);
        
        // URL decode if necessary
        size_t decode_pos = 0;
        while ((decode_pos = kategori_code.find("%20", decode_pos)) != std::string::npos) {
            kategori_code.replace(decode_pos, 3, " ");
            decode_pos += 1;
        }
    }
    std::cout << kategori_code << std::endl;
    if (kategori_code.empty()) {
        mg_printf(connection,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n\r\n"
            "[]");
        return 200;
    }

    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "[]");
        return 500;
    }

    // Find kategori name using LIKE with the code
    std::string kategori_name_query = "SELECT nama_bidang_masalah FROM bidang_masalah WHERE nama_bidang_masalah LIKE '" + kategori_code + "%';";
    std::string kategori_name = db.GetSingleResult(kategori_name_query);

    if (kategori_name.empty()) {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n\r\n"
            "[]");
        return 200;
    }

    JsonCallbackData jsonData;
    std::string select_sql = "SELECT * FROM soal_masalah WHERE nama_bidang_masalah LIKE '" + kategori_name + "%' ORDER BY id;";
    char* errMsg = nullptr;
    db.QueryWithResults(select_sql);
    int rc = sqlite3_exec(db.db, select_sql.c_str(), JsonCallback, &jsonData, &errMsg);
    db.Close();

    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "[]");
        return 500;
    }

    std::string response = jsonData.jsonArray.dump();

    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n\r\n"
        "%s",
        response.length(),
        response.c_str());

    return 200;
}

// 5. DELETE KATEGORI - Updated to handle foreign key constraints
EXPORT int Del_Kategori(struct mg_connection *connection, void *callback) {
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
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Invalid JSON\"}");
        return 400;
    }

    std::string kategori_code = parsed.value("kategori_code", "");
    if (kategori_code.empty()) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Missing kategori code\"}");
        return 400;
    }

    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Database error\"}");
        return 500;
    }

    // Find kategori name using LIKE with the code
    std::string kategori_name_query = "SELECT nama_bidang_masalah FROM bidang_masalah WHERE nama_bidang_masalah LIKE '" + kategori_code + "%' LIMIT 1;";
    std::string kategori_name = db.GetSingleResult(kategori_name_query);

    if (kategori_name.empty()) {
        db.Close();
       
mg_printf(connection,
    "HTTP/1.1 400 Bad Request\r\n"
    "Content-Type: application/json\r\n\r\n"
    "{\"success\": false, \"message\": \"Kategori not found for code: %s\"}",
    kategori_code.c_str());
;
        return 400;
    }

    // Check if kategori is referenced in hasil table
    std::string check_hasil_query = "SELECT COUNT(*) FROM hasil WHERE soal_masalah_kategori = '" + kategori_name + "';";
    std::string hasil_count = db.GetSingleResult(check_hasil_query);
    
    if (hasil_count != "0") {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Cannot delete kategori: it has associated results data\"}");
        return 400;
    }

    // Delete related subkategori first (cascade delete)
    std::string delete_sub_sql = "DELETE FROM soal_masalah WHERE nama_bidang_masalah = '" + kategori_name + "';";
    if (!db.Query(delete_sub_sql)) {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Failed to delete related subkategori\"}");
        return 500;
    }

    // Delete the kategori using LIKE
    std::string delete_sql = "DELETE FROM bidang_masalah WHERE nama_bidang_masalah LIKE '" + kategori_code + "%';";
    if (!db.Query(delete_sql)) {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Failed to delete kategori\"}");
        return 500;
    }

    db.Close();

    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n\r\n"
        "{\"success\": true, \"message\": \"Kategori and related subkategori deleted successfully\"}");

    return 200;
}

// 6. DELETE SUB KATEGORI - Updated to handle foreign key constraints
EXPORT int Del_SubKategori(struct mg_connection *connection, void *callback) {
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
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Invalid JSON\"}");
        return 400;
    }

    std::string subkategori_name = parsed.value("subkategori_name", "");
    if (subkategori_name.empty()) {
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Missing subkategori name\"}");
        return 400;
    }

    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Database error\"}");
        return 500;
    }

    // Check if subkategori exists
    std::string check_query = "SELECT id FROM soal_masalah WHERE nama_soal_masalah = '" + subkategori_name + "' LIMIT 1;";
    std::string subkategori_id = db.GetSingleResult(check_query);

    if (subkategori_id.empty()) {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Subkategori not found\"}");
        return 400;
    }

    // Check if subkategori is referenced in hasil table
    std::string check_hasil_query = "SELECT COUNT(*) FROM hasil WHERE soal_masalah_id = " + subkategori_id + ";";
    std::string hasil_count = db.GetSingleResult(check_hasil_query);
    
    if (hasil_count != "0") {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Cannot delete subkategori: it has associated results data\"}");
        return 400;
    }

    // Delete the subkategori by name
    std::string delete_sql = "DELETE FROM soal_masalah WHERE nama_soal_masalah = '" + subkategori_name + "';";
    if (!db.Query(delete_sql)) {
        db.Close();
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"success\": false, \"message\": \"Failed to delete subkategori\"}");
        return 500;
    }

    db.Close();

    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n\r\n"
        "{\"success\": true, \"message\": \"Subkategori deleted successfully\"}");

    return 200;
}EXPORT int Save_User(struct mg_connection *connection, void *callback) {
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
EXPORT int Del_User(struct mg_connection *connection, void *callback) {
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
    std::string query = "DELETE FROM siswa WHERE nama='"+name+"';";
    Db db;
    if (!db.Open()) {
        mg_printf(connection,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n"
            "{\"success\": false, \"message\": \"Database error\"}");
        return 500;
    }
    db.QueryWithResults(query);
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
