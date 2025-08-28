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
extern Pnix Server;
extern bool open_bk;
extern bool open_admin;
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
EXPORT int auth_bk(struct mg_connection *connection,void *callbackdata);
EXPORT int aum(struct mg_connection *connection,void *callback);
EXPORT int submit(struct mg_connection *connection,void *callback);
EXPORT int admin(struct mg_connection *connection,void *callback);
//API ENDPOINT DATABASED
//KATEGORI AUM
EXPORT int admin_crud_kategori(struct mg_connection *connection, void *callback);
EXPORT int Save_Kategori(struct mg_connection *connection,void *callback);
EXPORT int Read_Kategori(struct mg_connection *connection,void *callback);
//CRUD USER
EXPORT int Save_User(struct mg_connection *connection,void *callback);

//BK
EXPORT int bk_hasil(struct mg_connection *connection, void *callback); 
EXPORT int bk_user(struct mg_connection *connection, void *callback);
EXPORT int bk_cat(struct mg_connection *connection, void *callback);
EXPORT int hasil(struct mg_connection *connection, void *callback);
EXPORT int akun(struct mg_connection *connection, void *callback);
EXPORT int hasil_kelas(struct mg_connection *connection, void *callback);
EXPORT int bk_laporan(struct mg_connection *connection, void *callback);
EXPORT int stats(struct mg_connection *connection, void *callback);