#ifndef HTTP_H
#define HTTP_H

typedef enum {
    HTTP_METHOD_INVALID,
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_PATCH
} HttpMethod;

typedef struct {
    HttpMethod method;
    const char* body;
} Request;

typedef struct {
    const char* body;
} Response;

HttpMethod str_to_http_method(const char* methodStr);
const char* http_method_to_str(HttpMethod method);

#endif // HTTP_H
