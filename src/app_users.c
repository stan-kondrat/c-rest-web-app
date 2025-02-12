#include "app_users.h"

#include "http.h"
#include "router.h"

void app_users_add(HttpRequest* request, HttpResponse* response) {
}

void app_users_list(HttpRequest* request, HttpResponse* response) {
}

void app_users_get(HttpRequest* request, HttpResponse* response) {
}

void app_users_update(HttpRequest* request, HttpResponse* response) {
}

void app_users_delete(HttpRequest* request, HttpResponse* response) {
}

Router app_users_routes[] = {
    {HTTP_METHOD_POST, "/add", .function = app_users_add},
    {HTTP_METHOD_GET, "/", .function = app_users_list},
    {HTTP_METHOD_GET, "/:id", .function = app_users_get},
    {HTTP_METHOD_POST, "/:id", .function = app_users_update},
    {HTTP_METHOD_DELETE, "/:id", .function = app_users_list},
    {.end = true},
};
