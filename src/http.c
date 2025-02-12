#include <stdio.h>
#include <string.h>

#include "http.h"

HttpMethod str_to_http_method(const char* str, const int str_len) {
    HttpMethod http_method = HTTP_METHOD_INVALID;
    size_t len = str_len > 0 ? str_len : strlen(str);
    if (len >= 3 && strncmp(str, "GET", 3) == 0) {
        http_method = HTTP_METHOD_GET;
    } else if (len >= 4 && strncmp(str, "POST", 4) == 0) {
        http_method = HTTP_METHOD_POST;
    } else if (len >= 3 && strncmp(str, "PUT", 3) == 0) {
        http_method = HTTP_METHOD_PUT;
    } else if (len >= 6 && strncmp(str, "DELETE", 6) == 0) {
        http_method = HTTP_METHOD_DELETE;
    } else if (len >= 5 && strncmp(str, "PATCH", 5) == 0) {
        http_method = HTTP_METHOD_PATCH;
    }
    return http_method;
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

void print_request_headers(const HttpRequest* req) {
    if (!req || !req->headers || req->headers_cnt == 0) {
        printf("No headers to display.\n");
        return;
    }

    printf("HTTP Request Headers:\n");
    for (size_t i = 0; i < req->headers_cnt; ++i) {
        // Directly access the struct fields
        printf("%.*s: %.*s\n", (int) req->headers[i].name_len, req->headers[i].name,
               (int) req->headers[i].value_len, req->headers[i].value);
    }
}