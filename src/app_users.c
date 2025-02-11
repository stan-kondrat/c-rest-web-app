#include "app_users.h"

#include "http.h"
#include "router.h"

void app_users_add(Request* request, Response* response) {
}

void app_users_list(Request* request, Response* response) {
}

void app_users_get(Request* request, Response* response) {
}

void app_users_update(Request* request, Response* response) {
}

void app_users_delete(Request* request, Response* response) {
}


Router app_users_routes[] = {
    {HTTP_METHOD_POST,  "/add", .function = app_users_add},
    {HTTP_METHOD_GET,  "/", .function = app_users_list},
    {HTTP_METHOD_GET,  "/:id", .function = app_users_get},
    {HTTP_METHOD_POST,  "/:id", .function = app_users_update},
    {HTTP_METHOD_DELETE,  "/:id", .function = app_users_list},
    {.end = true},
};
