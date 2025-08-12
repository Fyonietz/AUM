#ifndef DATABASE
#define DATABASE

#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>


int database(){
    sqlite3* db;

    int exit =sqlite3_open("data.db",&db);

    if(exit){
        std::cerr << "Error Open Database:" <<sqlite3_errmsg(db) <<std::endl;
        return -1;
    };

    std::cout << "Opened Database Success" << std::endl;

}

#endif // !DATABASE


