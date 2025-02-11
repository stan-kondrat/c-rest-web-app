#ifndef SERVER_H
#define SERVER_H

#include "http.h"
#include "router.h"

int server(Router* routers);

int server_interfaces_print();

#endif // SERVER_H
