#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"
#include "logging.h"

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

long strtol_safe(const char* value, size_t value_len) {
    if (value == NULL || value_len == 0) {
        return 0;
    }

    char* temp = (char*) malloc(value_len + 1);
    if (temp == NULL) {
        return 0;
    }

    memcpy(temp, value, value_len);
    temp[value_len] = '\0';

    char* endptr;
    long result = strtol(temp, &endptr, 10);

    free(temp);

    return result;
}

int request_content_length(const HttpRequest* req) {
    if (!req || !req->headers || req->headers_cnt == 0) {
        log_warn(LOG_HTTP, "request_content_length: No headers");
        return 0;
    }

    char name[] = "Content-Length";
    int name_len = sizeof(name) - 1;

    for (size_t i = 0; i < req->headers_cnt; ++i) {
        if (req->headers[i].name_len == name_len &&
            strncasecmp(req->headers[i].name, name, name_len) == 0) {
            long content_length = strtol_safe(req->headers[i].value, req->headers[i].value_len);
            return content_length;
        }
    }
    return 0;
}
