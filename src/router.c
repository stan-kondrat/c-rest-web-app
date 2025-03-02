#include <stdio.h>
#include <string.h>

#include "router.h"

#include "http.h"
#include "logging.h"

RouterFunction router_find(Router* routers, char* method, size_t method_len, char* path,
                           size_t path_len) {

    log_debug(LOG_ROUTER, "--router_find--:\n");
    log_debug(LOG_ROUTER, "method %.*s", (int) method_len, method);
    log_debug(LOG_ROUTER, "path %.*s (%d)", (int) path_len, path, path_len);

    HttpMethod http_method = str_to_http_method(method, method_len);
    if (http_method == HTTP_METHOD_INVALID) {
        log_debug(LOG_ROUTER, "router_find: HTTP_METHOD_INVALID = %.*s", (int) method_len, method);
        return NULL;
    }

    for (int i = 0; !routers[i].end; ++i) {
        char* route_path = (char*) routers[i].path;
        size_t route_path_len = strlen(route_path);
        log_debug(LOG_ROUTER, "check %.*s", (int) route_path_len, route_path);

        if (routers[i].method == http_method && path_len == route_path_len &&
            memcmp(route_path, path, path_len) == 0) {
            log_debug(LOG_ROUTER, "router_find: found direct %zu\n", routers[i].function);
            return routers[i].function;
        }

        // Check for nested routers
        if (routers[i].routers != NULL && path_len >= route_path_len &&
            memcmp(path, route_path, route_path_len) == 0) {
            log_debug(LOG_ROUTER, "router_find: go nested");
            size_t remaining_path_len = path_len - route_path_len;
            RouterFunction nestedFunc = router_find(routers[i].routers, method, method_len,
                                                    path + route_path_len, remaining_path_len);
            if (nestedFunc != NULL) {
                log_debug(LOG_ROUTER, "router_find: found nested %zu\n", nestedFunc);
                return nestedFunc;
            }
        }
    }

    log_debug(LOG_ROUTER, "router_find: not found\n");
    return NULL;
}

void router_print(Router* routers, int level, const char* parent) {
    if (level == 0) {
        printf("--- Routes:\n");
    }
    for (int i = 0; !routers[i].end; ++i) {
        char* method_str = "";
        char* has_middlware = "";
        if (routers[i].method != 0) {
            method_str = http_method_to_str(routers[i].method);
        }
        if (routers[i].routers != NULL && routers[i].function != NULL) {
            has_middlware = "(middlware)";
        }
        printf("%s: %s%s %s\n", method_str, parent, routers[i].path, has_middlware);
        if (routers[i].routers != NULL) {
            router_print(routers[i].routers, level + 1, routers[i].path);
        }
    }
    if (level == 0) {
        printf("---\n");
    }
}
