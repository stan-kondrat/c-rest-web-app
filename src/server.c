// curl -v http://127.0.0.1:8080
// dd if=/dev/urandom bs=4k count=10 | curl -X POST -d @- http://localhost:8080
// curl -d "key1=value1&key2=value2&key3=value3" -X POST http://localhost:8080
// curl -d "text" -X GET http://localhost:8080

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "picohttpparser/picohttpparser.h"
#include <uv.h>

#include "http.h"
#include "logging.h"
#include "router.h"
#include "server.h"

// #define OVERWRITE_SUGGESTED_BUFFER_SIZE 64*1024
// #define OVERWRITE_SUGGESTED_BUFFER_SIZE 10

#define SOCKET_BACKLOG 128

#define MAX_HEADERS_COUNT 100
#define MIN_BUFFER_HEADER_SIZE 1024
#define MAX_BUFFER_HEADER_SIZE 64 * 1024

typedef struct {
    uv_tcp_t tcp_handle;

    char* request_header_buf;
    size_t request_header_buf_end;
    size_t request_header_buf_size;
    size_t request_header_len;

    HttpRequest request;
    HttpResponse response;
} client_t;

uv_loop_t* loop;

void on_close(uv_handle_t* handle) {
    log_debug(LOG_SOCKET, "on_close");

    client_t* client = (client_t*) handle;

    if (client->request_header_buf) {
        free(client->request_header_buf); // alloc and realloc in on_header_read()
    }
    if (client->request.headers) {
        free(client->request.headers); // alloc on_header_parse()
    }

    free(client); // alloc in on_connect()
}

int on_header_parse(client_t* client, size_t last_request_header_buf_size) {
    log_debug(LOG_HTTP, "on_header_parse: last_request_header_buf_size=%d",
              last_request_header_buf_size);

    const char* buf_start = client->request_header_buf;
    size_t buf_len = client->request_header_buf_size;
    size_t last_len = last_request_header_buf_size;

    char* method;
    size_t method_len;

    char* path;
    size_t path_len;

    int minor_version;

    struct phr_header headers[MAX_HEADERS_COUNT];
    size_t num_headers = MAX_HEADERS_COUNT;

    int ret = phr_parse_request(buf_start, buf_len, &method, &method_len, &path, &path_len,
                                &minor_version, headers, &num_headers, last_len);

    log_debug(LOG_HTTP, "on_header_parse: phr_parse_request ret=%d", ret);

    if (ret == -1) {
        log_warn(LOG_HTTP, "on_header_parse: parse error");
        return -1;
    } else if (ret == -2) {
        log_debug(LOG_HTTP, "on_header_parse: request is partial");
        return -2;
    }

    log_debug(LOG_HTTP, "on_header_parse: success, headers length %d", ret);

    client->request_header_len = ret;

    // Fill HttpRequest
    client->request.tcp_handle = &client->tcp_handle;
    client->request.method = str_to_http_method(method, (int) method_len);
    client->request.path = path;
    client->request.path_len = path_len;
    client->request.http_version = minor_version;

    client->request.headers = malloc(num_headers * sizeof(struct phr_header)); // free on_close
    if (client->request.headers == NULL) {
        log_warn(LOG_HTTP, "on_header_parse: Failed to allocate memory for headers ");
        return -1;
    }
    client->request.headers_cnt = num_headers;
    memcpy(client->request.headers, headers, num_headers * sizeof(struct phr_header));

    // if (buflen == sizeof(buf))
    //     return RequestIsTooLongError;

    printf("request is %d bytes long", ret);
    printf("method is %.*s", (int) method_len, method);
    printf("path is %.*s", (int) path_len, path);
    printf("HTTP version is 1.%d", minor_version);
    print_request_headers(&client->request);

    return 0;
}

int on_header_read(client_t* client, const char* data, const size_t data_len) {
    if (!client || !data || data_len == 0) {
        log_error(LOG_SOCKET, "on_header_read: Invalid parameters passed to on_header_read\n");
        return -1;
    }
    log_debug(LOG_SOCKET, "on_header_read: data_len = %zu, \n ", data_len);
    log_debug(LOG_SOCKET, "on_header_read: received:\n%.*s", (int) data_len, data);

    if (!client->request_header_buf) {
        // Allocate initial buffer
        size_t alloc_size = MIN(data_len, MAX_BUFFER_HEADER_SIZE);
        client->request_header_buf = (char*) malloc(alloc_size); // free in on_close()
        if (!client->request_header_buf) {
            log_warn(LOG_SOCKET, "on_header_read: Failed to allocate memory "
                                 "for request header buffer\n");
            return -1;
        }
        log_debug(LOG_SOCKET, "on_header_read: allocate %zu\n", alloc_size);
        memcpy(client->request_header_buf, data, alloc_size);
        client->request_header_buf_size = alloc_size;
        client->request_header_buf_end = alloc_size;
        return 0;
    }

    // Calculate required space and cap it at MAX_BUFFER_HEADER_SIZE
    size_t required_size = client->request_header_buf_end + data_len;
    required_size = MIN(required_size, MAX_BUFFER_HEADER_SIZE);

    if (required_size > client->request_header_buf_size) {
        // Determine new buffer size (double current size but not beyond
        // MAX_BUFFER_HEADER_SIZE)
        size_t new_size = MAX(required_size, client->request_header_buf_size * 2);

        // Ensure we do not exceed MAX_BUFFER_HEADER_SIZE
        new_size = MIN(new_size, MAX_BUFFER_HEADER_SIZE);

        char* new_buf = (char*) realloc(client->request_header_buf,
                                        new_size); // free in on_close()
        if (!new_buf) {
            log_warn(LOG_SOCKET, "on_header_read: Failed to allocate memory "
                                 "for request header buffer\n");
            return -1;
        }

        log_debug(LOG_SOCKET, "on_header_read: realloc %zu\n", new_size);

        client->request_header_buf = new_buf;
        client->request_header_buf_size = new_size;
    }

    size_t remaining_space = client->request_header_buf_size - client->request_header_buf_end;
    size_t append_len = MIN(data_len, remaining_space);

    // Append the data
    memcpy(client->request_header_buf + client->request_header_buf_end, data, append_len);
    client->request_header_buf_end += append_len;

    if (append_len < data_len) {
        log_warn(LOG_SOCKET,
                 "on_header_read: Not all data fit into the buffer. %zu bytes "
                 "remaining.\n",
                 data_len - append_len);
    }

    return 0;
}

void on_tcp_parse(client_t* client, ssize_t nread, const uv_buf_t* buf) {
    if (client->request.headers != NULL) {
        uv_read_stop((uv_stream_t*) &client->tcp_handle);
        return;
    }

    // read headers
    size_t last_request_header_buf_end = client->request_header_buf_end;
    int rret = on_header_read(client, buf->base, nread);
    if (rret == -1) {
        uv_close((uv_handle_t*) &client->tcp_handle, on_close);
        return;
    }

    // parse headers
    int pret = on_header_parse(client, last_request_header_buf_end);
    if (pret == -1) {
        uv_close((uv_handle_t*) &client->tcp_handle, on_close);
        return;
    } else if (pret == -2) {
        // continue header parse on next read
        return;
    }
    // success parse headers
    log_debug(LOG_SOCKET, "on_tcp_parse: success parse headers");
    uv_read_stop((uv_stream_t*) &client->tcp_handle);
}

void on_tcp_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    log_debug(LOG_SOCKET, "on_tcp_read: Callback for when data is read, nread = %ld", nread);

    if (nread < 0) {
        if (nread == UV_EOF) {
            log_debug(LOG_SOCKET, "on_tcp_read: Connection closed");
        } else {
            log_error(LOG_SOCKET, "on_tcp_read: Read error: %s", uv_strerror((int) nread));
        }
        uv_close((uv_handle_t*) stream, on_close);
    } else if (nread > 0) {
        client_t* client = (client_t*) stream;
        on_tcp_parse(client, nread, buf);
    }

    if (buf->base) {
        log_debug(LOG_SOCKET, "on_tcp_read: Free the buffer after reading");
        free(buf->base); // alloc in alloc_buffer_read()
    }
}

void alloc_buffer_read(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {

#ifdef OVERWRITE_SUGGESTED_BUFFER_SIZE
    suggested_size = OVERWRITE_SUGGESTED_BUFFER_SIZE;
#endif

    log_debug(LOG_SOCKET, "alloc_buffer_read: Suggested size: %zu bytes (%.2f KB)\n",
              suggested_size, (double) suggested_size / 1024.0);
    buf->base = (char*) malloc(suggested_size); // free in on_tcp_read()
    if (buf->base == NULL) {
        log_error(LOG_SOCKET, "Failed to allocate memory\n");
        return;
    }
    buf->len = suggested_size;
}

// Callback when a new TCP connection is made
void on_connect(uv_stream_t* server, int status) {
    if (status < 0) {
        fprintf(stderr, "Connection error: %s\n", uv_strerror(status));
        return;
    }

    client_t* client = (client_t*) malloc(sizeof(client_t)); // free in on_close()
    if (client == NULL) {
        log_warn(LOG_SOCKET, "on_connect: Failed to allocate memory for new client");
        return;
    }
    uv_tcp_t* tcp_client = &client->tcp_handle;

    client->request_header_buf = NULL;
    client->request_header_buf_size = 0;
    client->request_header_buf_end = 0;
    client->request_header_len = 0;

    uv_tcp_init(loop, tcp_client);

    if (uv_accept(server, (uv_stream_t*) tcp_client) == 0) {
        // Start reading data from the client
        uv_read_start((uv_stream_t*) tcp_client, alloc_buffer_read, on_tcp_read);
    } else {
        uv_close((uv_handle_t*) tcp_client, on_close);
    }
}

int server(int port, Router* routers) {

    loop = uv_default_loop();

    uv_tcp_t tcp_server;
    uv_tcp_init(loop, &tcp_server);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", port, &addr);

    uv_tcp_bind(&tcp_server, (const struct sockaddr*) &addr, 0);
    int ret = uv_listen((uv_stream_t*) &tcp_server, SOCKET_BACKLOG, on_connect);
    if (ret) {
        log_message(LOG_ERROR, LOG_SOCKET, "Listen error: %s\n", uv_strerror(ret));
        return 1;
    }

    log_message(LOG_INFO, LOG_SOCKET, "HTTP server running on port %d...\n", port);
    uv_run(loop, UV_RUN_DEFAULT);

    return 0;
}
