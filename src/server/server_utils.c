#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "server_utils.h"
#include <unistd.h>
#include "../state_machine/stm.h"
#include "../parser/command_parser.h"


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
    socklen_t  client_address_len = sizeof(client_address);

    int client = w_accept(server_sock, (struct sockaddr *) &client_address, &client_address_len);

    return client;
}

int handle_connection(int client) {

    // Initialize resources
    struct parser *command_parser = command_parser_init();

    struct parser_event *event = NULL;

    state_machine_ptr state_machine = state_machine_init();

    struct buffer_t read_buffer = {{0}, 0, 0};

    char write_buffer[BUFF_SIZE];

    state_machine_run(state_machine, event, write_buffer, 0);

    int read_pos_aux;

    while(get_state(state_machine) == AUTHENTICATION) {
        event = malloc(sizeof(struct parser_event));
        read_pos_aux = 0; 

        // If there is a possible command in the buffer, parse it
        while(event->type == MAYEQ) {

            // If the buffer is empty, read from the socket
            if (read_buffer.read_pos == read_buffer.write_pos) {
                event->bytes_received = w_recv(client, read_buffer.buffer + read_buffer.write_pos, BUFF_SIZE - read_buffer.write_pos, 0);
                read_buffer.write_pos += event->bytes_received;
            }

            // Parse the command (save a copy of the read position because it will be modified)
            read_pos_aux = read_buffer.read_pos;
            event = get_command(event, command_parser, &read_buffer, read_buffer.write_pos - read_buffer.read_pos);
        }

        // If the command is complete, run the state machine
        parser_reset(command_parser);

        int len = state_machine_run(state_machine, event, write_buffer, read_buffer.write_pos - read_pos_aux);

        int bytes_sent = 0;

        // Send the response
        while(bytes_sent < len) {
            bytes_sent += w_send(client, write_buffer + bytes_sent, len - bytes_sent, 0);
        }
    }

    // Clean resources and close connection
    free(event);
    free_state_machine(state_machine);
    parser_destroy(command_parser);
    close(client);
    return 0;
}