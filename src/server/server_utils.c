#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "server_utils.h"
#include <unistd.h>
#include "../state_machine/stm.h"
#include "../parser/command_parser.h"
#include "../session/session.h"


//TODO: Agregar codigos de error al .h

int w_socket(int domain, int type, int protocol){
    int ret = socket(domain, type, protocol);
    if(ret < 0){
        perror("error creating socket");
        exit(1);
    }
    return ret;
}

int w_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    int ret = bind(sockfd, addr, addrlen);
    if(ret < 0){
        perror("error binding socket");
        exit(1);
    }
    return ret;
}

int w_listen(int sockfd, int backlog){
    int ret = listen(sockfd, backlog);
    if(ret < 0){
        perror("error listening socket");
        exit(1);
    }
    return ret;
}

int w_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){

    int ret = accept(sockfd, addr, addrlen);

    if(ret < 0){
        perror("error accepting socket");
        exit(1);
    }
    return ret;
}

int w_recv(int sockfd, void *buf, size_t len, int flags){
    int ret = recv(sockfd, buf, len, flags);
    if(ret < 0){
        perror("error receiving socket");
        exit(1);
    }
    return ret;
}

int w_send(int sockfd, const void *buf, size_t len, int flags){
    int ret = send(sockfd, buf, len, flags);
    if(ret < 0){
        perror("error sending socket");
        exit(1);
    }
    return ret;
}

int setup_server(int port) {

    struct sockaddr_in sock_address;
    memset(&sock_address, 0, sizeof(sock_address));
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_address.sin_port = htons(port);
    
    int server = w_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    w_bind(server, (struct sockaddr *) &sock_address, sizeof(sock_address));

    w_listen(server, 50);

    return 0;
}

int accept_connection(int server_sock){

    struct sockaddr_storage client_address;
    socklen_t client_address_len = sizeof(client_address);

    int client = w_accept(server_sock, (struct sockaddr *) &client_address, &client_address_len);

    return client;
}

int handle_connection(int client) {

    // Initialize user session
    session_ptr session = new_session(client);

    while(get_session_state(session) != EXIT) {

        // Read from session
        struct parser_event * event = read_session(session);

        // Continue session
        int bytes_to_write = continue_session(session);

        // Write to session
        send_session_response(session, bytes_to_write);
    }

    // Close connection
    close(client);
    return 0;
}