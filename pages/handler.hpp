#include "../core/starter.hpp"

#include "pyro.hpp"
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else   
    #define EXPORT __attribute__((visibility("default")))
#endif

EXPORT int login(struct mg_connection *connection,void *callback);
EXPORT int login_redirect(struct mg_connection *connection,void *callback);
EXPORT int static_file(struct mg_connection *connection,void *callback);
EXPORT int dashboard_admin(struct mg_connection *connection,void *callback);
EXPORT int auth(struct mg_connection *connection,void *callbackdata);