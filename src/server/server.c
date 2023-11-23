#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "server_utils.h"
#include "server_ADT.h"
#include "../session/session.h"
#include "../selector/selector.h"

#define MAX_CURRENT_CLIENTS 500

static bool received_signal = false;

static void handle_signal(int signal){
    printf("\nsignal %d received\n", signal);
    received_signal = true;
}

void accept_passive_connection(struct selector_key *key);

static fd_selector new_fd_selector(int socket, fd_handler *handler, struct selector_init *conf);
    
int main(int argc, char *argv[]){

    // close stdin, stdout
    close(0);
    close(1);

    // handle signals
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // setup server
    server_t server = init_server(argc, argv);
    int server_sock = get_server_socket();

    set_fd_handler(&accept_passive_connection, NULL);
    struct fd_handler *server_handler = get_fd_handler();

    // setup selector
    struct selector_init conf = {
        .signal = SIGALRM,
        .select_timeout = {
            .tv_sec = 10,
            .tv_nsec = 0,
        },
    };

    fd_selector fd_selector = new_fd_selector(server_sock, server_handler, &conf);

    // main loop
    while(!received_signal){
        selector_select(fd_selector);
    }

    close_server();
}

static fd_selector new_fd_selector(int socket, fd_handler *handler, struct selector_init *conf){
    int ret = selector_init(conf);
    if(ret != 0){
        perror("selector_init");
        exit(1);
    }

    if (selector_fd_set_nio(socket) == -1) {
        perror("selector_fd_set_nio");
        exit(1);
    }

    fd_selector fd_selector = selector_new(1024);
    selector_register(fd_selector, socket, handler, OP_READ, NULL);
    return fd_selector;
}

void accept_passive_connection(struct selector_key *key){
    int socket_fd = accept_connection(key->fd);
    session_ptr session = new_session(socket_fd);
    add_user(session);
    selector_register(key->s, socket_fd, get_session_fd_handler(session), OP_WRITE, session);
}