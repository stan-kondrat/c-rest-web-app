#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include <cmocka.h>

#include "http.h"
#include "server.h"

void test_app_hello_world_handler(HttpRequest* request, HttpResponse* response)
    __attribute__((unused));
void test_app_hello_world_handler(HttpRequest* request, HttpResponse* response) {
    (void) request;
    http_response(response, (HttpStatusCode) 200, "Hello, world!", strlen("Hello, world!"));
}

static void test_app_hello(void** state) __attribute__((unused));

static void test_app_hello(void** state) {
    (void) state;

    // GIVEN
    Router routers[] = {
        {HTTP_METHOD_GET, "/hello", .function = test_app_hello_world_handler},
        {.end = true},
    };
    client_t client = (client_t){0};
    client.routers = routers;
    const char req_raw[] = "GET /hello HTTP/1.1\r\n"
                           "Host: localhost:8080\r\n"
                           "User-Agent: curl/8.7.1\r\n"
                           "Accept: */*\r\n"
                           "\r\n";
    uv_buf_t buf = (uv_buf_t){.base = (char*) req_raw, .len = sizeof(req_raw)};
    ssize_t nread = sizeof(req_raw) - 1;

    // WHEN
    int result = on_tcp_parse(&client, nread, &buf);
    RouterFunction route_function =
        router_find(client.routers, client.request.method, client.request.method_len,
                    client.request.path, client.request.path_len);
    route_function(&client.request, &client.response);
    int content_length = response_content_length(&client.response);

    // THEN
    assert_int_equal(result, 1);
    assert_int_equal(content_length, 13);
    assert_int_equal(client.response.body_len, 13);
    assert_memory_equal(client.response.body, "Hello, world!", 13);
}
