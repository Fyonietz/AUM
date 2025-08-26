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
EXPORT int bk_laporan(struct mg_connection *connection, void *callback){
    if(!open_bk){
        mg_printf(connection,
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>Unauthorized</h1></body></html>");
        return 401;
    }
    Server.SSR("public/bk/hasilkelas.htpp",connection);
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

EXPORT int hasil_kelas(struct mg_connection *connection, void *callback) {
    if (!open_bk) {
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

    // Contoh query yang kamu butuhkan:
    // SELECT kelas, bidang_masalah, soal_dipilih, ROUND(AVG(nilai), 2) AS rata_rata, 
    //        (COUNT(soal_dipilih) * 100.0 / total_soal) AS persentase
    // FROM hasil JOIN soal_masalah ... (sesuaikan dengan query sebenarnya)
    // Untuk demo ini, pakai select * dulu
std::string select_sql = R"(
SELECT
  kelas,
  soal_masalah_kategori AS bidang_masalah,
  COUNT(soal_masalah_id) AS soal_dipilih,
  ROUND(
    CAST(COUNT(soal_masalah_id) AS FLOAT) / 
    (SELECT COUNT(*) FROM soal_masalah WHERE nama_bidang_masalah = hasil.soal_masalah_kategori) * 100,
    2
  ) AS persentase
FROM hasil
GROUP BY kelas, soal_masalah_kategori
)";


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

EXPORT int stats(struct mg_connection *connection, void *callback) {
    if (!open_bk) {
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

    sqlite3_stmt *stmt;
    std::string mostSelectedCategory;
    int mostSelectedCount = 0;
    int totalAccounts = 0;
    int totalSubmissions = 0;

    // 1. Most selected category
    const char *sql1 =
        "SELECT soal_masalah_kategori, COUNT(*) as total "
        "FROM hasil "
        "GROUP BY soal_masalah_kategori "
        "ORDER BY total DESC LIMIT 1;";
    if (sqlite3_prepare_v2(db.db, sql1, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            mostSelectedCategory = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            mostSelectedCount = sqlite3_column_int(stmt, 1);
        }
        sqlite3_finalize(stmt);
    }

    // 2. Total registered accounts
    const char *sql2 = "SELECT COUNT(*) FROM siswa;";
    if (sqlite3_prepare_v2(db.db, sql2, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            totalAccounts = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    // 3. Total unique submissions
    const char *sql3 = "SELECT COUNT(DISTINCT nama) FROM hasil;";
    if (sqlite3_prepare_v2(db.db, sql3, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            totalSubmissions = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    db.Close();

    // Build JSON
    nlohmann::json jsonResponse = {
        {"most_selected_category", mostSelectedCategory},
        {"most_selected_count", mostSelectedCount},
        {"total_accounts", totalAccounts},
        {"total_submissions", totalSubmissions}
    };

    std::string response = jsonResponse.dump();

    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n\r\n"
        "%s",
        response.length(),
        response.c_str());

    return 200;
}
