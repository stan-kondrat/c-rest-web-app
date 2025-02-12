#ifndef SERVER_H
#define SERVER_H

#include "http.h"
#include "router.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

int server(int port, Router* routers);

#endif // SERVER_H
