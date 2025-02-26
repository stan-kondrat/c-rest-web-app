#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

#include <uv.h>

#include "picohttpparser/picohttpparser.h"

typedef enum {
    HTTP_METHOD_INVALID,
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_PATCH
} HttpMethod;

typedef struct {
    uv_tcp_t* tcp_handle;

    HttpMethod method;
    char* path;
    size_t path_len;
    char* body;
    size_t body_len;
    int http_version;
    struct phr_header* headers;
    size_t headers_cnt;

    // int (*read_body_chunk)(struct HttpRequest* request, char* buffer, size_t chunk_size,
    //                        size_t* out_len);
    // int (*read_body)(struct HttpRequest* request, char* buffer, size_t* out_len, size_t
    // max_size);
} HttpRequest;

typedef struct {
    uv_tcp_t* tcp_handle;

    char* body;
    size_t body_len;
    struct phr_header* headers;
    size_t headers_cnt;
} HttpResponse;

HttpMethod str_to_http_method(const char* str, const int str_len);
const char* http_method_to_str(HttpMethod method);

void print_request_headers(const HttpRequest* req);
int request_content_length(const HttpRequest* req);

#endif // HTTP_H
