#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "server_utils.h"
#include "server_ADT.h"
#include "../session/session.h"
#include "../selector/selector.h"

#define PORT 110
#define MAX_CURRENT_CLIENTS 500

static bool received_signal = false;

static void handle_signal(int signal){
    printf("signal %d received\n", signal);
    received_signal = true;
}

void accept_passive_connection(struct selector_key *key);
    
int main(int argc, char *argv[]){

    // close stdin, stdout
    close(0);
    // close(1);

    // handle signals
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // setup server
    server_t server = init_server("./mail", argc, argv);
    int server_sock = get_server_socket();

    // setup selector
    const struct selector_init conf = {
        .signal = SIGALRM,
        .select_timeout = {
            .tv_sec = 10,
            .tv_nsec = 0,
        },
    };

    // setup selector (TODO check errors -> != 0)
    selector_init(&conf);

    // setup selector fd
    selector_fd_set_nio(server_sock);

    fd_selector fd_selector = selector_new(2 * MAX_CURRENT_CLIENTS);

    const struct fd_handler server_handler = {
        .handle_read = accept_passive_connection,
        .handle_write = NULL,
        .handle_close = NULL,
    };

    // register server socket
    selector_register(fd_selector, server_sock, &server_handler, OP_READ, NULL);

    // main loop
    while(!received_signal){
        selector_select(fd_selector);
    }
}

void accept_passive_connection(struct selector_key *key){
    int socket_fd = accept_connection(key->fd);
    session_ptr session = new_session(socket_fd);
    selector_register(key->s, socket_fd, get_session_fd_handler(session), OP_WRITE, session);
}