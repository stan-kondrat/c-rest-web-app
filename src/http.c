#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"
#include "logging.h"

HttpMethod str_to_http_method(char* str, size_t str_len) {
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

const char* http_status_message(HttpStatusCode status_code) {
    switch (status_code) {
    case HTTP_CONTINUE:
        return "100 Continue";
    case HTTP_SWITCHING_PROTOCOLS:
        return "101 Switching Protocols";
    case HTTP_OK:
        return "200 OK";
    case HTTP_CREATED:
        return "201 Created";
    case HTTP_ACCEPTED:
        return "202 Accepted";
    case HTTP_NO_CONTENT:
        return "204 No Content";
    case HTTP_MOVED_PERMANENTLY:
        return "301 Moved Permanently";
    case HTTP_FOUND:
        return "302 Found";
    case HTTP_NOT_MODIFIED:
        return "304 Not Modified";
    case HTTP_BAD_REQUEST:
        return "400 Bad Request";
    case HTTP_UNAUTHORIZED:
        return "401 Unauthorized";
    case HTTP_FORBIDDEN:
        return "403 Forbidden";
    case HTTP_NOT_FOUND:
        return "404 Not Found";
    case HTTP_METHOD_NOT_ALLOWED:
        return "405 Method Not Allowed";
    case HTTP_REQUEST_TIMEOUT:
        return "408 Request Timeout";
    case HTTP_TOO_MANY_REQUESTS:
        return "429 Too Many Requests";
    case HTTP_INTERNAL_SERVER_ERROR:
        return "500 Internal Server Error";
    case HTTP_BAD_GATEWAY:
        return "502 Bad Gateway";
    case HTTP_SERVICE_UNAVAILABLE:
        return "503 Service Unavailable";
    case HTTP_GATEWAY_TIMEOUT:
        return "504 Gateway Timeout";
    default:
        return "Unknown Status Code";
    }
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
        log_warn(LOG_HTTP, "No headers to display.\n");
        return;
    }

    log_debug(LOG_HTTP, "HTTP Request Headers:\n");
    for (size_t i = 0; i < req->headers_cnt; ++i) {
        // Directly access the struct fields
        log_debug(LOG_HTTP, "%.*s: %.*s\n", (int) req->headers[i].name_len, req->headers[i].name,
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
    return headers_content_length(req->headers, req->headers_cnt);
}

int response_content_length(const HttpResponse* res) {
    if (!res || !res->headers || res->headers_cnt == 0) {
        log_warn(LOG_HTTP, "response_content_length: No headers");
        return 0;
    }
    return headers_content_length(res->headers, res->headers_cnt);
}

int headers_content_length(const struct phr_header* headers, const size_t headers_cnt) {
    char name[] = "Content-Length";
    size_t name_len = sizeof(name) - 1;

    for (size_t i = 0; i < headers_cnt; ++i) {
        if (headers[i].name_len == name_len && strncasecmp(headers[i].name, name, name_len) == 0) {
            long content_length = strtol_safe(headers[i].value, headers[i].value_len);
            return content_length;
        }
    }
    return 0;
}

void http_response_ok(HttpResponse* response, char* body) {
    return http_response(response, HTTP_OK, body, strlen(body));
}

void http_response(HttpResponse* res, HttpStatusCode status_code, char* body, size_t body_len) {
    res->status_code = status_code;
    res->body = body;
    res->body_len = body_len;

    char body_len_str[12];
    snprintf(body_len_str, sizeof(body_len_str), "%d", body_len);

    add_header(res, "Content-Type", "text/plain");
    add_header(res, "Content-Length", body_len_str);
}

void add_header(HttpResponse* response, const char* header_name, const char* header_value) {
    if (response->headers == NULL) {
        response->headers = malloc(sizeof(struct phr_header));
        if (response->headers == NULL) {
            perror("Failed to allocate memory for headers");
            return;
        }
    } else {
        struct phr_header* new_headers =
            realloc(response->headers, (response->headers_cnt + 1) * sizeof(struct phr_header));
        if (new_headers == NULL) {
            perror("Failed to reallocate memory for headers");
            return;
        }
        response->headers = new_headers;
    }

    // Set up the new header
    struct phr_header* new_header = &response->headers[response->headers_cnt];
    new_header->name_len = strlen(header_name);
    new_header->value_len = strlen(header_value);

    // Dynamically allocate memory for the name and value of the header
    new_header->name = malloc(new_header->name_len);
    new_header->value = malloc(new_header->value_len);

    if (new_header->name == NULL || new_header->value == NULL) {
        perror("Failed to allocate memory for header name or value");
        return;
    }

    // Copy the header name and value into the allocated memory
    strcpy((char*) new_header->name, header_name);
    strcpy((char*) new_header->value, header_value);

    // Increment the header count
    response->headers_cnt++;
}

void free_response(HttpResponse* response) {
    for (size_t i = 0; i < response->headers_cnt; i++) {
        free((void*) response->headers[i].name);
        free((void*) response->headers[i].value);
    }
    free(response->headers);
    free(response->body);
}

int create_http_response_buffer(const HttpResponse* res, char** out_buffer,
                                size_t* out_buffer_len) {
    size_t required_len = 0;

    const char* status_message = http_status_message(res->status_code);
    required_len += 11 + strlen(status_message) + 2; // "HTTP/1.1 " + [200 OK] + "\r\n"

    for (size_t i = 0; i < res->headers_cnt; i++) {
        required_len += res->headers[i].name_len + res->headers[i].value_len + 4; // ": ", " \r\n"
    }

    required_len += res->body_len + 2; // "\r\n"

    *out_buffer = (char*) malloc(required_len);
    if (*out_buffer == NULL) {
        return -1;
    }

    char* ptr = *out_buffer;

    ptr += sprintf(ptr, "HTTP/1.1 %s\r\n", status_message);

    for (size_t i = 0; i < res->headers_cnt; i++) {
        memcpy(ptr, res->headers[i].name, res->headers[i].name_len);
        ptr += res->headers[i].name_len;
        *ptr++ = ':';
        *ptr++ = ' ';
        memcpy(ptr, res->headers[i].value, res->headers[i].value_len);
        ptr += res->headers[i].value_len;
        *ptr++ = '\r';
        *ptr++ = '\n';
    }

    *ptr++ = '\r';
    *ptr++ = '\n';

    memcpy(ptr, res->body, res->body_len);

    *out_buffer_len = required_len;

    return 0;
}
