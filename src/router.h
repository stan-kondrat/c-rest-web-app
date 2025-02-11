#ifndef ROUTER_H
#define ROUTER_H

#include "http.h"
#include <stdbool.h>

typedef void (*RouterFunction)(Request* request, Response* response);

typedef struct Router {
    HttpMethod method;
    const char* path;
    RouterFunction function;
    struct Router* routers;
    bool end;
} Router;

RouterFunction router_find(Router* routers, const char* methodStr,
                           const char* path);
void router_print(Router* routers, int level, const char* parent);

#endif // ROUTER_H
