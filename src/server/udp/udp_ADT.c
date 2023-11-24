#include "udp_ADT.h"
#include <string.h>
#include "../server_utils.h"
#include <stdlib.h>
#include <unistd.h>

#define UPD_PORT 5000
#define CREDENTIALS 2
#define USERNAME 0
#define PASSWORD 1
#define MAX_USER_LEN 32
#define MAX_PASS_LEN 32

struct udp {
    int socket;
    char *credentials[CREDENTIALS];
    struct fd_handler *fd_handler;
};

struct udp * udp_server = NULL;

udp_ADT init_udp() {
    if (udp_server == NULL) {
        udp_server = malloc(sizeof(struct udp));
        udp_server->socket = setup_udp_ipv4(UPD_PORT);
        udp_server->credentials[USERNAME] = calloc(MAX_USER_LEN, sizeof(char));
        udp_server->credentials[PASSWORD] = calloc(MAX_PASS_LEN, sizeof(char));
        udp_server->fd_handler = malloc(sizeof(fd_handler));
        udp_server->fd_handler->handle_close = close_udp_fd_handler;
    }
    return udp_server;
}

int get_udp_socket() {
    return udp_server->socket;
}

int validate_credentials(const char * username, const char * password) {
    return strcmp(udp_server->credentials[USERNAME], username) == 0 && strcmp(udp_server->credentials[PASSWORD], password) == 0;
}

fd_handler * get_udp_fd_handler() {
    return udp_server->fd_handler;
}

void set_udp_fd_handler(void (*handle_read)(struct selector_key * key), void (*handle_write)(struct selector_key * key)) {
    udp_server->fd_handler->handle_read = handle_read;
    udp_server->fd_handler->handle_write = handle_write;
}

void close_udp() {
    if (udp_server != NULL) {
        close(udp_server->socket);
        free(udp_server->fd_handler);
        free(udp_server);
    }
}

void close_udp_fd_handler(struct selector_key * key) {
    close_udp();
}