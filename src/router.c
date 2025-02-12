#include <stdio.h>
#include <string.h>

#include "router.h"

#include "http.h"

RouterFunction router_find(Router* routers, const char* methodStr, const char* path) {
    HttpMethod method = str_to_http_method(methodStr);
    if (method == HTTP_METHOD_INVALID) {
        return NULL;
    }

    for (int i = 0; !routers[i].end; ++i) {
        if (routers[i].method == method && strcmp(routers[i].path, path) == 0) {
            return routers[i].function;
        }

        if (routers[i].routers != NULL &&
            strncmp(path, routers[i].path, strlen(routers[i].path)) == 0) {
            RouterFunction nestedFunc =
                router_find(routers[i].routers, methodStr, path + strlen(routers[i].path));
            if (nestedFunc != NULL) {
                return nestedFunc;
            }
        }
    }

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
