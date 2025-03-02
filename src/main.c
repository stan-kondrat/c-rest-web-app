#include <stdio.h>

#include "app_users.h"
#include "http.h"
#include "logging.h"
#include "router.h"
#include "server.h"

void index_page(HttpRequest* request, HttpResponse* response) {
    (void) request;
    http_response_ok(response, "Hello, world!");
}

void echo_page(HttpRequest* request, HttpResponse* response) {
    http_response(response, (HttpStatusCode) 200, request->body, request->body_len);
}

int main() {

    // LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL
    log_set_level(LOG_DEBUG);

    // LOG_APP | LOG_SOCKET | LOG_HTTP | LOG_ROUTER
    log_set_enabled_features(LOG_APP | LOG_SOCKET | LOG_HTTP | LOG_ROUTER);

    log_info(LOG_APP, "C REST Web Application!");

    Router routers[] = {
        {HTTP_METHOD_GET, "/", .function = index_page},
        {HTTP_METHOD_POST, "/echo", .function = echo_page},
        {HTTP_METHOD_GET, "/users", .routers = app_users_routes},
        {.end = true},
    };

    // router_print(routers, 0, "");

    int port = 8080;
    server(port, routers);

    return 0;
}
