#ifndef DATABASE
#define DATABASE

#include <sqlite3.h>
#include <iostream>
#include <string>
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else   
    #define EXPORT __attribute__((visibility("default")))
#endif
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
};

#endif // !DATABASE
