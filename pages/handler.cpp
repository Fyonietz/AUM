#include "handler.hpp"
Pnix Server;

EXPORT int home(struct mg_connection *connection,void *callback){
    Server.home("public/layout.html",connection);
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

EXPORT int home_redirect(struct mg_connection *connection, void *callback) {
    mg_printf(connection,
        "HTTP/1.1 302 Found\r\n"
        "Location: /home\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n");
    return 302;
}