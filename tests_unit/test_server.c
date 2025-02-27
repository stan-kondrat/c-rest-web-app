#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include <cmocka.h>

#include "server.h"

static void test_server_parse_get(void** state) __attribute__((unused));
static void test_server_parse_post(void** state) __attribute__((unused));

static void test_server_parse_get(void** state) {
    (void) state;

    // GIVEN
    client_t client = (client_t) {0};
    const char rawdata[] = "GET / HTTP/1.1\r\n"
                           "Host: localhost:8080\r\n"
                           "User-Agent: curl/8.7.1\r\n"
                           "Accept: */*\r\n"
                           "\r\n";
    uv_buf_t buf = (uv_buf_t) {.base = rawdata, .len = sizeof(rawdata)};
    ssize_t nread = sizeof(rawdata) - 1;

    // WHEN
    int result = on_tcp_parse(&client, nread, &buf);

    // THEN
    assert_int_equal(result, 1);
    assert_int_equal(client.request.method, HTTP_METHOD_GET);
}

static void test_server_parse_post(void** state) {
    (void) state;

    // GIVEN
    client_t client = (client_t) {0};
    const char rawdata[] = "POST / HTTP/1.1\r\n"
                           "Host: localhost:8080\r\n"
                           "User-Agent: curl/8.7.1\r\n"
                           "Accept: */*\r\n"
                           "Content-Length: 35\r\n"
                           "Content-Type: application/x-www-form-urlencoded\r\n"
                           "\r\n"
                           "key1=value1&key2=value2&key3=value3";
    uv_buf_t buf = (uv_buf_t) {.base = rawdata, .len = sizeof(rawdata)};
    ssize_t nread = sizeof(rawdata) - 1;

    // WHEN
    int result = on_tcp_parse(&client, nread, &buf);

    // THEN
    assert_int_equal(result, 1);
    assert_int_equal(client.request.method, HTTP_METHOD_POST);
}
