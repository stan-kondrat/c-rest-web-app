#include <stdio.h>

#include "router.h"
#include "server.h"
#include "app_users.h"


void index_page(Request* request, Response* response) {
    // check_expected(request->body);
    // check_expected(response->body);
}

int main() {
    printf("Hello, World!\n");

    Router routers[] = {
        {HTTP_METHOD_GET,  "/", .function = index_page},
        {HTTP_METHOD_GET,  "/users", .routers = app_users_routes},
        {.end = true},
    };

    router_print(routers, 0, "");

    server(routers);

    return 0;
}
