#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parson/parson.h"
#include "picohttpparser/picohttpparser.h"

void foo()
{
    printf("This is foo!\n");

    printf("parson/parson.c:\n");

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "name", "John Smith");
    json_object_set_number(root_object, "age", 25);
    json_object_dotset_string(root_object, "address.city", "Cupertino");
    json_object_dotset_value(root_object, "contact.emails", json_parse_string("[\"email@example.com\",\"email2@example.com\"]"));
    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    printf("\n\npicohttpparser/picohttpparser.c:\n");

    const char *request = "GET /hello HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "User-Agent: curl/7.64.1\r\n"
                          "Accept: */*\r\n\r\n";

    // Variables to store the parsed result
    struct phr_header headers[10]; // Array of headers
    size_t num_headers = sizeof(headers) / sizeof(headers[0]);
    int minor_version;
    const char *method;
    const char *path;
    size_t method_len;
    size_t path_len;

    // Parse the HTTP request
    ssize_t r = phr_parse_request(request, strlen(request), &method, &method_len, &path, &path_len, &minor_version, headers, &num_headers, 0);

    if (r > 0)
    {
        // Successfully parsed request
        printf("Method: %.*s\n", (int)method_len, method);
        printf("Path: %.*s\n", (int)path_len, path);
        printf("HTTP Version: HTTP/1.%d\n", minor_version);

        // Print headers
        for (size_t i = 0; i < num_headers; i++)
        {
            printf("Header: %.*s: %.*s\n",
                   (int)headers[i].name_len, headers[i].name,
                   (int)headers[i].value_len, headers[i].value);
        }
    }
    else
    {
        // Parsing failed
        printf("Failed to parse the HTTP request.\n");
    }
}
