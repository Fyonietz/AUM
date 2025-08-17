#include "handler.hpp"

Pnix Server;



EXPORT int login(struct mg_connection *connection,void *callback){
    Server.home("public/views/login.htpp",connection);
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
EXPORT int auth(struct mg_connection *connection,void *callbackdata){
    char post_data[1024] = {0};
    int post_data_lenght = mg_read(connection,post_data,sizeof(post_data));
    post_data[post_data_lenght] ='\0';

    std::cout << post_data << std::endl;
    std::string str_post(post_data);
    if(str_post.find("Habib")){
     std::cout << "Here" << std::endl;   
     mg_printf(connection,
        "HTTP/1.1 302 Found\r\n"
        "Location: /auth\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n");
     }else{
        std::cout << "Auth Failed" << std::endl;
     }
    
    return 302;
}
EXPORT int aum(struct mg_connection *connection,void *callback){
    Server.SSR("public/views/AUM.htpp",connection);

    return 200;
}

EXPORT int admin(struct mg_connection *connection,void *callback){
    Server.SSR("public/admin.htpp",connection);

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
    std::string select_sql = "SELECT id, nama_bidang_masalah FROM bidang_masalah;";
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
