#ifndef ROUTER_H
#define ROUTER_H

#include "server.h"

typedef void (*RouterFunction)(Request* request, Response* response);

typedef struct Router {
    HttpMethod method; 
    const char* path;  
    union {
        RouterFunction func;  
        struct Router* routes; 
    };
} Router;

int add(int a, int b);

#endif // ROUTER_H
