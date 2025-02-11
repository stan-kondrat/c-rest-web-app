#ifndef SERVER_H
#define SERVER_H

typedef enum {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH
} HttpMethod;

typedef struct {
    HttpMethod method;
    const char* body;
} Request;

typedef struct {
    const char* body;
} Response;


int server();

#endif // SERVER_H
