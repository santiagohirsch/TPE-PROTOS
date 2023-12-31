#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "server_utils.h"
#include "server_ADT.h"
#include "./session/session.h"
#include "./selector/selector.h"
#include <string.h>
#include "./udp/udp_ADT.h"
#include "./udp/udp_handler.h"
#include "./utils/logger.h"

#define MAX_CURRENT_CLIENTS 500

static bool received_signal = false;

static void handle_signal(int signal){
    log_msg(LOG_INFO, "received signal: %d", signal);
    received_signal = true;

    // QUIT 
    
}

void accept_passive_connection(struct selector_key *key);

static fd_selector new_fd_selector(int ipv4_socket, int ipv6_socket, fd_handler *handler, struct selector_init *conf);
    
int main(int argc, char *argv[]){

    // close stdin, stdout
    close(0);
    close(1); // open for logs in server console

    // handle signals
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // setup server
    init_udp();
    server_t server = init_server(argc, argv);
    if (server == NULL) {
        log_msg(LOG_FATAL, "server initialization failed");
        exit(1);
    }
    int server_ipv4_sock = get_server_ipv4_socket();
    log_msg(LOG_INFO, "server ipv4 socket: %d", server_ipv4_sock);

    int server_ipv6_sock = get_server_ipv6_socket();
    log_msg(LOG_INFO, "server ipv6 socket: %d", server_ipv6_sock);

    set_fd_handler(&accept_passive_connection, NULL);
    struct fd_handler *server_handler = malloc(sizeof(fd_handler));
    memcpy((void *)server_handler, get_fd_handler(), sizeof(fd_handler));

    log_msg(LOG_INFO, "server initialization complete");

    // setup udp
    /*udp_ADT udp_server = */
    int udp_ipv4_sock = get_udp_ipv4_socket();
    log_msg(LOG_INFO, "got udp ipv4 socket: %d", udp_ipv4_sock);

    int udp_ipv6_sock = get_udp_ipv6_socket();
    log_msg(LOG_INFO, "got udp ipv6 socket: %d", udp_ipv6_sock);

    //set_udp_fd_handler(&handle_udp_read, &handle_udp_write);
    set_udp_fd_handler(&udp_read, NULL);
    struct fd_handler *udp_handler = malloc(sizeof(fd_handler));

    memcpy((void *)udp_handler, get_udp_fd_handler(), sizeof(fd_handler));

    log_msg(LOG_INFO, "udp initialization complete");


    // setup selector
    struct selector_init conf = {
        .signal = SIGALRM,
        .select_timeout = {
            .tv_sec = 10,
            .tv_nsec = 0,
        },
    };

    fd_selector fd_selector = new_fd_selector(server_ipv4_sock, server_ipv6_sock, server_handler, &conf);

    // register selectors
    selector_register(fd_selector, udp_ipv4_sock, udp_handler, OP_READ, NULL);
    selector_register(fd_selector, udp_ipv6_sock, udp_handler, OP_READ, NULL);
    selector_register(fd_selector, server_ipv4_sock, server_handler, OP_READ, NULL);
    selector_register(fd_selector, server_ipv6_sock, server_handler, OP_READ, NULL);

    log_msg(LOG_INFO, "selector initialization complete");

    // main loop
    while(!received_signal){
        log_msg(LOG_INFO, "selector select active: starting iteration");
        selector_select(fd_selector);
    }

    // cleanup
    selector_destroy(fd_selector);
    free(server_handler);
    free(udp_handler);

    log_msg(LOG_INFO, "server shutting down");
    return 0;
}

static fd_selector new_fd_selector(int ipv4_socket, int ipv6_socket, fd_handler *handler, struct selector_init *conf){
    int ret = selector_init(conf);
    if(ret != 0){
        log_msg(LOG_ERROR, "selector_init");
        exit(1);    // TODO: handle this error with logs
    }

    if (selector_fd_set_nio(ipv4_socket) == -1) {
        log_msg(LOG_ERROR, "selector_fd_set_nio");
        exit(1);
    }

    if (selector_fd_set_nio(ipv6_socket) == -1) {
        log_msg(LOG_ERROR, "selector_fd_set_nio");
        exit(1);
    }

    fd_selector fd_selector = selector_new(1024);
    selector_register(fd_selector, ipv4_socket, handler, OP_READ, NULL);
    selector_register(fd_selector, ipv6_socket, handler, OP_READ, NULL);
    return fd_selector;
}

void accept_passive_connection(struct selector_key *key){
    int socket_fd = accept_connection(key->fd);
    if (server_full()) {
        log_msg(LOG_INFO, "Connection with socket %d refused: server full", socket_fd);
        close(socket_fd);
        return;
    }
    session_ptr session = new_session(socket_fd);
    add_user(session);
    selector_register(key->s, socket_fd, get_session_fd_handler(session), OP_WRITE, session);
}