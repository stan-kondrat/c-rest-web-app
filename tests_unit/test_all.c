#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

#include "router.h"
#include "test_router.c"

// Run all tests (you can expand this with more tests)
static const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_router_structure),
    cmocka_unit_test(test_router_find),
    cmocka_unit_test(test_router_middleware),
};

// Main function to run the tests
int main(void) {
    return cmocka_run_group_tests(tests, NULL, NULL);
}
