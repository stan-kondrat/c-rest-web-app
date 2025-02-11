#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/router.h"
#include "test_router.c"

// Run all tests (you can expand this with more tests)
static const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_router_structure)
};

// Main function to run the tests
int main(void) { 
    return cmocka_run_group_tests(tests, NULL, NULL); 
}
