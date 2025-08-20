#include "register.hpp"
#include "../pages/handler.hpp"
#include <iostream>
Db db;
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
    Route.add(context,"/auth/siswa",auth_siswa,0);
    Route.add(context,"/logout",logout,0);
    Route.add(context,"/aum",aum,0);
    Route.add(context,"/admin",admin,0);
    Route.add(context,"/admin/user",admin_crud_user,0);
    Route.add(context,"/api/register/user",Save_User,0);
    Route.add(context,"/api/save",Save_Kategori,0);
    Route.add(context,"/api/read",Read_Kategori,0);
    // Handle static files (CSS, JS, images)
    Route.add(context,"/style.css",static_file,0);
    Route.add(context,"/bk.css",static_file,0);
    Route.add(context,"/script.js",static_file,0);
    Route.add(context,"/views/scripts-siswa.js",static_file,0);
    Route.add(context,"/views/siswa.css",static_file,0);
    Route.add(context,"/admin/style-admin.css",locked_static_file,0);
    Route.add(context,"/admin/script-admin.js",locked_static_file,0);
    std::cout <<info << "Routes Safe" << std::flush <<std::endl;
}
