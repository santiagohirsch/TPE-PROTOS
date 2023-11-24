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

typedef struct user_admin {
    char username[MAX_USER_LEN];
    char pass[16];
} user_admin;

struct udp {
    int ipv4_socket;
    int ipv6_socket;
    user_admin admin;
    struct fd_handler *fd_handler;
};



struct udp * udp_server = NULL;

udp_ADT init_udp() {
    if (udp_server == NULL) {
        udp_server = calloc(1, sizeof(struct udp));
        udp_server->ipv4_socket = setup_udp_ipv4(UPD_PORT);
        udp_server->ipv6_socket = setup_udp_ipv6(UPD_PORT);
        udp_server->fd_handler = malloc(sizeof(fd_handler));
        udp_server->fd_handler->handle_close = close_udp_fd_handler;
    }
    return udp_server;
}

int get_udp_ipv4_socket() {
    return udp_server->ipv4_socket;
}

int get_udp_ipv6_socket() {
    return udp_server->ipv6_socket;
}

void set_admin(char * username, char * password) {
    strcpy(udp_server->admin.username, username);
    strcpy(udp_server->admin.pass, password);
}

bool validate_credentials(const char * username, const char * password) {
    return strcmp(udp_server->admin.username, username) == 0 && strcmp(udp_server->admin.pass, password) == 0;
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
        close(udp_server->ipv4_socket);
        close(udp_server->ipv6_socket);
        free(udp_server->fd_handler);
        free(udp_server);
    }
}

void close_udp_fd_handler(struct selector_key * key) {
    close_udp();
}