#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "server_utils.h"

#define BUFF_SIZE 1024



//TODO: Cambiar los checkeos de errores a otras funciones :)

int setup_server(int port) {

    struct sockaddr_in sock_address;
    memset(&addr, 0, sizeof(sock_address));
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_address.sin_port = htons(port);
    
    int server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(server < 0){
        perror("error creating socket");
        goto finish;
    }

    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    if(bind(server,(struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("bind error");
        goto finish;
    }

    if (listen(server, 20) < 0) {
        perror("listen error");
        goto finish;
    }

    int ret = 0;
    return ret;

    finish:
    ret = -1;
    return ret;
}

int accept_connection(int server_sock){

    struct sockaddr_storage client_address;
    socklen_t  client_address_len = sizeof(client_address);

    int client_socket = accept(serverSock,(struct sockaddr *) &client_address, &client_address_len);
    if(client_socket < 0){
        perror("accept error");
        return -1;
    }

    return client_socket;
}

int handle_connection(int client) {
    char buffer[BUFF_SIZE] = {0};
    int bytes_read = 0;

    while((bytes_read = recv(client, buffer, BUFF_SIZE, 0)) > 0){
        
        if (bytes_read < 0){
            perror("recv error");
            return -1;
        }

        int bytes_sent = send(client, buffer, bytes_read, 0);

        if(bytes_sent < 0){
            perror("send error");
            return -1;
        }

        if(bytes_sent != bytes_read){
            perror("send error");
            return -1;
        }
    }

    close(client);
    return 0;
}