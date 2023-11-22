#include "udp_ADT.h"
#include <string.h>
#include "../server_utils.h"
#include <stdlib.h>

#define UPD_PORT 5000
#define CREDENTIALS 2
#define USERNAME 0
#define PASSWORD 1
#define ADMIN "admin"

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
        udp_server->credentials[USERNAME] = calloc(strlen(ADMIN) + 1, sizeof(char));
        udp_server->credentials[PASSWORD] = calloc(strlen(ADMIN) + 1, sizeof(char));
        udp_server->fd_handler = malloc(sizeof(fd_handler));
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

void set_fd_handler(void (*handle_read)(struct selector_key * key), void (*handle_write)(struct selector_key * key)) {
    udp_server->fd_handler->handle_read = handle_read;
    udp_server->fd_handler->handle_write = handle_write;
}