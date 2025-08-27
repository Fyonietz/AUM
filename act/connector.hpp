#ifndef DATABASE
#define DATABASE

#include <sqlite3.h>
#include <iostream>
#include <string>
#include <json.hpp>
#include "auth.hpp"
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else   
    #define EXPORT __attribute__((visibility("default")))
#endif

using json = nlohmann::json;

extern "C" struct EXPORT JsonCallbackData {
    json jsonArray = json::array();
};

inline int JsonCallback(void* data, int argc, char** argv, char** azColName) {
    JsonCallbackData* jsonData = static_cast<JsonCallbackData*>(data);

    json row;
    for (int i = 0; i < argc; i++) {
        std::string colName = azColName[i];
        std::string value = argv[i] ? argv[i] : "";
        row[colName] = value;
    }

    jsonData->jsonArray.push_back(row);
    return 0;
}
extern "C" struct EXPORT Db {
    std::string info_sys = " > Phoenix[Info]: ";

    sqlite3* db = nullptr;
    std::string GetSingleResult(const std::string& sql) { std::string result; auto callback = [](void* data, int argc, char** argv, char** azColName) -> int { if (argc > 0 && argv[0]) { *static_cast<std::string*>(data) = argv[0]; } return 0; }; char* errorMessage = nullptr; int rc = sqlite3_exec(db, sql.c_str(), callback, &result, &errorMessage); if (rc != SQLITE_OK) { std::cerr << info_sys << "SQL error in GetSingleResult: " << errorMessage << std::endl; sqlite3_free(errorMessage); return ""; } return result; }
    // Open the database
    bool QueryWithResults(const std::string& sql) { char* errorMessage = nullptr; int exit = sqlite3_exec(db, sql.c_str(), Callback, nullptr, &errorMessage); if (exit != SQLITE_OK) { std::cerr <<info_sys<< "SQL error: " << errorMessage << std::endl; sqlite3_free(errorMessage); return false; } return true; }
    bool Open(const std::string& filename = "data.db") {
        int exit = sqlite3_open(filename.c_str(), &db);
        if (exit) {
            std::cerr << info_sys <<"Error opening database: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        std::cout <<info_sys << "Opened database successfully." << std::endl;
        return true;
    }

    // Execute a query (without results)
    bool Query(const std::string& sql) {
        char* errorMessage = nullptr;
        int exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);

        if (exit != SQLITE_OK) {
            std::cerr <<info_sys<< "SQL error: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
            return false;
        }

        std::cout <<info_sys<< "Query executed successfully." << std::endl;
        return true;
    }

   int GetIntResult(const std::string& sql) {
    sqlite3_stmt* stmt;
    int result = 0;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << info_sys << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }

    return result;
}

// Execute a SELECT and return a single string result
std::string GetStringResult(const std::string& sql) {
    sqlite3_stmt* stmt;
    std::string result;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_text(stmt, 0)) {
            result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << info_sys << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }

    return result;
}

// Execute SELECT and return JSON array
json GetJsonArray(const std::string& sql) {
    JsonCallbackData data;
    char* errorMessage = nullptr;

    int rc = sqlite3_exec(db, sql.c_str(), JsonCallback, &data, &errorMessage);
    if (rc != SQLITE_OK) {
        std::cerr << info_sys << "SQL error in GetJsonArray: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return json::array(); // Return empty
    }

    return data.jsonArray;
}

    // Close the database
    void Close() {
        if (db) {
            sqlite3_close(db);
            db = nullptr;
            std::cout <<info_sys<< "Database closed." << std::endl;
        }
    }

    // Static callback for SELECT queries
    static int Callback(void* NotUsed, int argc, char** argv, char** azColName) {
        for (int i = 0; i < argc; i++) {
            std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << "  ";
        }
        std::cout << std::endl;
        return 0;
    }

    bool AuthenticateAsAdmin(Db& db, const std::string& username, const std::string& password) {
    if (username != "Admin") {
        return false;
    }

    std::string query = "SELECT * FROM user WHERE Role = 'Admin' AND password = '" + password + "';";
    
    JsonCallbackData result;
    char* errorMessage = nullptr;

    int exit = sqlite3_exec(db.db, query.c_str(), JsonCallback, &result, &errorMessage);
    if (exit != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }

    return !result.jsonArray.empty(); // If any row matched, login success
}
    bool AuthenticateAsBK(Db& db, const std::string& username, const std::string& password) {
    if (username != "BK_SMK4") {
        return false;
    }

    std::string query = "SELECT * FROM user WHERE Role = 'BK_SMK4' AND password = '" + password + "';";
    
    JsonCallbackData result;
    char* errorMessage = nullptr;

    int exit = sqlite3_exec(db.db, query.c_str(), JsonCallback, &result, &errorMessage);
    if (exit != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }

    return !result.jsonArray.empty();// If any row matched, login success
}

 std::string Escape(const std::string& input) {
    std::string escaped;
    for (char c : input) {
        if (c == '\'') escaped += "''";  // Escape single quote
        else escaped += c;
    }
    return escaped;
}

};

#endif // !DATABASE
