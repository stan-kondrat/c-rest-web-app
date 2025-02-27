#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include <cmocka.h>

#include "router.h"

static void test_router_structure(void** state) __attribute__((unused));
static void test_router_find(void** state) __attribute__((unused));
static void test_router_find_nested(void** state) __attribute__((unused));
static void test_router_not_found(void** state) __attribute__((unused));

void test_router_func1(HttpRequest* request, HttpResponse* response) {
    check_expected(request->body);
    check_expected(response->body);
}

void test_router_func2(HttpRequest* request, HttpResponse* response) {
    check_expected(request->body);
    check_expected(response->body);
}

void test_router_func3(HttpRequest* request, HttpResponse* response) {
    check_expected(request->body);
    check_expected(response->body);
}

static void test_router_structure(void** state) {
    (void) state;

    // GIVEN
    Router routers[] = {
        {HTTP_METHOD_GET, "/index", .function = test_router_func1},
        {HTTP_METHOD_GET, "/items",
         .routers =
             (Router[]) {
                 {HTTP_METHOD_GET, "/list", .function = test_router_func2},
                 {HTTP_METHOD_POST, "/add", .function = test_router_func2},
                 {.end = true},
             }},
        {.end = true},
    };

    // WHEN

    // THEN
    assert_string_equal(routers[0].path, "/index");
    assert_string_equal(routers[1].path, "/items");
    assert_string_equal(routers[1].routers[0].path, "/list");
    assert_string_equal(routers[1].routers[1].path, "/add");
    assert_int_equal(routers[0].method, HTTP_METHOD_GET);
    assert_int_equal(routers[1].routers[1].method, HTTP_METHOD_POST);
}

static void test_router_find(void** state) {
    (void) state;

    // GIVEN
    Router routers[] = {
        {HTTP_METHOD_GET, "/index", .function = test_router_func1},
        {HTTP_METHOD_GET, "/items",
         .routers =
             (Router[]) {
                 {HTTP_METHOD_GET, "/list", .function = test_router_func2},
                 {HTTP_METHOD_POST, "/add", .function = test_router_func2},
                 {.end = true},
             }},
        {.end = true},
    };

    // WHEN
    RouterFunction not_found_path = router_find(routers, "GET", "/x");
    RouterFunction wrong_method = router_find(routers, "POST", "/index");
    RouterFunction found = router_find(routers, "GET", "/index");
    RouterFunction found_nested = router_find(routers, "POST", "/items/add");

    // THEN
    assert_null(not_found_path);
    assert_null(wrong_method);

    assert_non_null(found);
    assert_ptr_equal(found, test_router_func1);

    assert_non_null(found_nested);
    assert_ptr_equal(found_nested, test_router_func2);
}

static void test_router_find_nested(void** state) {
    (void) state;

    // GIVEN
    Router routers[] = {
        {HTTP_METHOD_GET, "/parent", .function = test_router_func1,
         .routers =
             (Router[]) {
                 {HTTP_METHOD_GET, "/child", .function = test_router_func2},
                 {.end = true},
             }},
        {HTTP_METHOD_GET, "/a", .function = test_router_func1,
         .routers =
             (Router[]) {
                 {HTTP_METHOD_GET, "/b", .function = test_router_func2},
                 {HTTP_METHOD_GET, "/c", .function = test_router_func3},
                 {.end = true},
             }},
        {.end = true},
    };

    // WHEN
    RouterFunction func = router_find(routers, "GET", "/a/c");

    // THEN
    assert_ptr_equal(func, test_router_func3);
}

static void test_router_not_found(void** state) {
    (void) state;

    // GIVEN
    Router routers[] = {
        {HTTP_METHOD_GET, "/", .function = test_router_func1},
        {.end = true},
    };

    // WHEN
    RouterFunction func = router_find(routers, "GET", "/not-exist");

    // THEN
    assert_ptr_equal(func, NULL);
}
