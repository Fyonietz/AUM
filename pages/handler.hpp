#include "../core/starter.hpp"
#include "../act/connector.hpp"
#include "pyro.hpp"
#include "unordered_map"
#include "../act/auth.hpp"
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else   
    #define EXPORT __attribute__((visibility("default")))
#endif

EXPORT int login(struct mg_connection *connection,void *callback);
EXPORT int login_redirect(struct mg_connection *connection,void *callback);
EXPORT int static_file(struct mg_connection *connection,void *callback);
EXPORT int locked_static_file(struct mg_connection *connection, void *callback);
EXPORT int admin_crud_user(struct mg_connection *connection, void *callback);
EXPORT int dashboard_admin(struct mg_connection *connection,void *callback);
EXPORT int auth(struct mg_connection *connection,void *callbackdata);
EXPORT int auth_siswa(struct mg_connection *connection, void *callbackdata);
 EXPORT int me(struct mg_connection *connection, void *callback);
EXPORT int logout(struct mg_connection *connection,void *callbackdata);
EXPORT int login(struct mg_connection *connection,void *callbackdata);
EXPORT int aum(struct mg_connection *connection,void *callback);
EXPORT int admin(struct mg_connection *connection,void *callback);
//API ENDPOINT DATABASED
//KATEGORI AUM
EXPORT int Save_Kategori(struct mg_connection *connection,void *callback);
EXPORT int Read_Kategori(struct mg_connection *connection,void *callback);
//CRUD USER
EXPORT int Save_User(struct mg_connection *connection,void *callback);