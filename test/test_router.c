#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/router.h"

void test_router_func1(Request* request, Response* response) {
    check_expected(request->body);
    check_expected(response->body);
}

void test_router_func2(Request* request, Response* response) {
    check_expected(request->body);
    check_expected(response->body);
}

static void test_router_structure(void **state) {
    (void)state; 

    // GIVEN
    Router main[] = {
        {GET,  "/index", .func = test_router_func1},
        {GET, "/items", .routes = (Router[]) {
            {GET,  "/list", .func = test_router_func2},
            {POST,  "/add", .func = test_router_func2},
        }}
    };

    // WHEN

    // THEN
    assert_string_equal(main[0].path, "/index"); 
    assert_string_equal(main[1].path, "/items"); 
    assert_string_equal(main[1].routes[0].path, "/list"); 
    assert_string_equal(main[1].routes[1].path, "/add"); 
    assert_int_equal(main[0].method, GET); 
    assert_int_equal(main[1].routes[1].method, POST); 
}