#ifndef ROUTER_H
#define ROUTER_H

#include "http.h"
#include <stdbool.h>

typedef void (*RouterFunction)(HttpRequest* request, HttpResponse* response);

typedef struct Router {
    HttpMethod method;
    const char* path;
    RouterFunction function;
    struct Router* routers;
    bool end;
} Router;

RouterFunction router_find(Router* routers, char* method, size_t method_len, char* path,
                           size_t path_len);
void router_print(Router* routers, int level, const char* parent);

#endif // ROUTER_H
