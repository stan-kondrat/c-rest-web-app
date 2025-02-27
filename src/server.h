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

typedef struct {
    uv_tcp_t tcp_handle;

    char* request_header_buf;
    size_t request_header_buf_end;
    size_t request_header_buf_size;
    size_t request_header_len;

    char* request_body_buf;
    size_t request_body_buf_end;
    size_t request_body_buf_size;
    size_t request_body_len;

    HttpRequest request;
    HttpResponse response;
} client_t;

int server(int port, Router* routers);

int on_tcp_parse(client_t* client, ssize_t nread, const uv_buf_t* buf);

#endif // SERVER_H
