#include "register.hpp"
#include "../pages/handler.hpp"
#include <iostream>

std::string info = " > Phoenix[Info]: ";
void Route::add(struct mg_context *context,const char *url,int (*handler)(struct mg_connection *connection,void *callbackdata),void *data){
    mg_set_request_handler(context,url,handler,data);
};
extern "C" EXPORT void update(struct mg_context *context) {
    if (!context) {
        std::cerr <<info<< "Cannot Register Routes - Server Context Is Missing!" << std::endl;
        return;
    }
    std::cout <<info << "Routes Check" << std::endl <<std::flush;
    // Register all routes herea
    Route.add(context,"/",login_redirect,0);
    Route.add(context,"/login",login,0);
    Route.add(context,"/dashboard",dashboard_admin,0);
    Route.add(context,"/auth",auth,0);
    // Handle static files (CSS, JS, images)
    Route.add(context,"/style.css",static_file,0);
    Route.add(context,"/script.js",static_file,0);
    std::cout <<info << "Routes Safe" << std::flush <<std::endl;
}
