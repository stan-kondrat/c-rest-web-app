// brew install libuv && brew link --force libuv
// curl http://localhost:8080

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include "logging.h"
#include "router.h"

#define DEFAULT_PORT 8080
#define DEFAULT_BACKLOG 128

#define RESPONSE "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nContent-Type: text/plain\r\n\r\nHello, World!"

uv_loop_t *loop;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}

void on_close(uv_handle_t *handle) {
    free(handle);
}

void on_write(uv_write_t *req, int status) {
    if (status) {
        log_message(LOG_ERROR, LOG_NETWORK, "Write error: %s\n", uv_strerror(status));
    }
    uv_close((uv_handle_t*) req->handle, on_close);
    free(req);
}

void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        uv_write_t *write_req = (uv_write_t*) malloc(sizeof(uv_write_t));
        uv_buf_t wrbuf = uv_buf_init(RESPONSE, strlen(RESPONSE));
        uv_write(write_req, client, &wrbuf, 1, on_write);
    } else {
        uv_close((uv_handle_t*) client, on_close);
    }
    free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        log_message(LOG_ERROR, LOG_NETWORK, "New connection error: %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);

    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, on_read);
    } else {
        // uv_close when the socket isn’t required.
        uv_close((uv_handle_t*) client, on_close);
    }
}

int server(Router* routers) {

    loop = uv_default_loop();
    
    uv_tcp_t tcp_server;
    uv_tcp_init(loop, &tcp_server);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);

    uv_tcp_bind(&tcp_server, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*) &tcp_server, DEFAULT_BACKLOG, on_new_connection);
    if (r) {
        log_message(LOG_ERROR, LOG_NETWORK, "Listen error: %s\n", uv_strerror(r));
        return 1;
    }

    log_message(LOG_INFO, LOG_NETWORK, "HTTP server running on port %d...\n", DEFAULT_PORT);
    uv_run(loop, UV_RUN_DEFAULT);
    
    return 0;
}


int server_interfaces_print() {
    char buf[512];
    uv_interface_address_t *info;
    int count, i;

    uv_interface_addresses(&info, &count);
    i = count;

    printf("--- Number of interfaces: %d\n", count);
    while (i--) {
        uv_interface_address_t interface_a = info[i];

        printf("Name: %s ", interface_a.name);
        printf("Internal? %s ", interface_a.is_internal ? "Yes" : "No");
        
        if (interface_a.address.address4.sin_family == AF_INET) {
            uv_ip4_name(&interface_a.address.address4, buf, sizeof(buf));
            printf("IPv4 address: %s ", buf);
        }
        else if (interface_a.address.address4.sin_family == AF_INET6) {
            uv_ip6_name(&interface_a.address.address6, buf, sizeof(buf));
            printf("IPv6 address: %s ", buf);
        }

        printf("\n");
    }
    printf("---\n");

    uv_free_interface_addresses(info, count);
    return 0;
}