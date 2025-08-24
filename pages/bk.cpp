#include "handler.hpp"
#include "../act/auth.hpp"


EXPORT int bk_hasil(struct mg_connection *connection, void *callback){
    if(!open_bk){
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }
    Server.SSR("public/bk/hasil.htpp",connection);
};

EXPORT int bk_user(struct mg_connection *connection, void *callback){
    if(!open_bk){
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }
    Server.SSR("public/bk/user.htpp",connection);
};

EXPORT int hasil(struct mg_connection *connection, void *callback){
    if(!open_bk){
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }
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
    std::string select_sql = "SELECT * FROM hasil;";
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

EXPORT int akun(struct mg_connection *connection, void *callback){
    if(!open_bk){
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }
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
    std::string select_sql = "SELECT nama,kelas,password FROM siswa;";
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
