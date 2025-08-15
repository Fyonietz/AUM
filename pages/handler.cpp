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