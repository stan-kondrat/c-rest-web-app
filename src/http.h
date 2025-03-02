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

typedef enum {
    HTTP_CONTINUE = 100,
    HTTP_SWITCHING_PROTOCOLS = 101,
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_ACCEPTED = 202,
    HTTP_NO_CONTENT = 204,
    HTTP_MOVED_PERMANENTLY = 301,
    HTTP_FOUND = 302,
    HTTP_NOT_MODIFIED = 304,
    HTTP_BAD_REQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_REQUEST_TIMEOUT = 408,
    HTTP_TOO_MANY_REQUESTS = 429,
    HTTP_INTERNAL_SERVER_ERROR = 500,
    HTTP_BAD_GATEWAY = 502,
    HTTP_SERVICE_UNAVAILABLE = 503,
    HTTP_GATEWAY_TIMEOUT = 504
} HttpStatusCode;

typedef struct {
    uv_tcp_t* tcp_handle;

    // HttpMethod method;
    char* method;
    size_t method_len;
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
    HttpStatusCode status_code;

    char* body;
    size_t body_len;
    struct phr_header* headers;
    size_t headers_cnt;
} HttpResponse;

HttpMethod str_to_http_method(char* str, size_t str_len);
const char* http_method_to_str(HttpMethod method);
const char* http_status_message(HttpStatusCode status_code);

void print_request_headers(const HttpRequest* req);

int headers_content_length(const struct phr_header* headers, const size_t headers_cnt);
int request_content_length(const HttpRequest* req);
int response_content_length(const HttpResponse* res);

void add_header(HttpResponse* res, const char* header_name, const char* header_value);

void http_response_ok(HttpResponse* response, char* body);

void http_response(HttpResponse* response, HttpStatusCode status_code, char* body, size_t body_len);

int create_http_response_buffer(const HttpResponse* res, char** out_buffer, size_t* out_buffer_len);

#endif // HTTP_H
