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

    // Open the database
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

    // Execute a query and print results (e.g., SELECT)
    bool QueryWithResults(const std::string& sql) {
        char* errorMessage = nullptr;
        int exit = sqlite3_exec(db, sql.c_str(), Callback, nullptr, &errorMessage);

        if (exit != SQLITE_OK) {
            std::cerr <<info_sys<< "SQL error: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
            return false;
        }

        return true;
    }
    // Execute a SELECT and return the first column of the first row
    std::string GetSingleResult(const std::string& sql) {
    std::string result;
    
    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        if (argc > 0 && argv[0]) {
            *static_cast<std::string*>(data) = argv[0];
        }
        return 0; // Stop after first row
    };

    char* errorMessage = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), callback, &result, &errorMessage);
    if (rc != SQLITE_OK) {
        std::cerr << info_sys << "SQL error in GetSingleResult: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return "";
    }

    return result;
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
