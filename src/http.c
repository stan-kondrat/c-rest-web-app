#include <string.h>

#include "http.h"

HttpMethod str_to_http_method(const char* methodStr) {
    HttpMethod method;
    if (strcmp(methodStr, "GET") == 0) {
        method = HTTP_METHOD_GET;
    } else if (strcmp(methodStr, "POST") == 0) {
        method = HTTP_METHOD_POST;
    } else if (strcmp(methodStr, "PUT") == 0) {
        method = HTTP_METHOD_PUT;
    } else if (strcmp(methodStr, "DELETE") == 0) {
        method = HTTP_METHOD_DELETE;
    } else if (strcmp(methodStr, "PATCH") == 0) {
        method = HTTP_METHOD_PATCH;
    } else {
        method = HTTP_METHOD_INVALID;
    }
    return method;
}

const char* http_method_to_str(HttpMethod method) {
    switch (method) {
    case HTTP_METHOD_GET:
        return "GET";
    case HTTP_METHOD_POST:
        return "POST";
    case HTTP_METHOD_PUT:
        return "PUT";
    case HTTP_METHOD_DELETE:
        return "DELETE";
    case HTTP_METHOD_PATCH:
        return "PATCH";
    default:
        return "INVALID";
    }
}
