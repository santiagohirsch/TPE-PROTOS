#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <sys/socket.h>

#define BUFF_SIZE 1024

typedef struct buffer_t{
    char buffer[BUFF_SIZE];
    int write_pos;
    int read_pos;
} buffer_t;

/* SETUP_SERVER
*   - creates a socket and binds it to the specified port
*   - returns the socket file descriptor
*   - returns -1 on error
*   - params: port 
*/
int setup_server(int port);


/* ACCEPT_CONNECTION
*   - accepts a connection on the specified socket
*   - returns the socket file descriptor
*   - returns -1 on error
*   - params: server socket file descriptor
*/
int accept_connection(int server);

/* HANDLE_CONNECTION
*   - handles a connection on the specified socket
*   - returns 0 on success
*   - returns -1 on error
*   - params: client socket file descriptor
*/
int handle_connection(int client);

/* W_SOCKET
*   - creates a socket
*   - returns the socket file descriptor
*   - exits on error
*   - params: domain, type, protocol
*/
int w_socket(int domain, int type, int protocol);

/* W_BIND
*   - binds a socket to an address
*   - returns 0 on success
*   - exits on error
*   - params: socket file descriptor, address, address length
*/
int w_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/* W_LISTEN
*   - listens on a socket
*   - returns 0 on success
*   - exits on error
*   - params: socket file descriptor, backlog
*/
int w_listen(int sockfd, int backlog);

/* W_ACCEPT
*   - accepts a connection on a socket
*   - returns the socket file descriptor
*   - exits on error
*   - params: socket file descriptor, address, address length
*/
int w_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/* W_RECV
*   - receives data on a socket
*   - returns the number of bytes received
*   - exits on error
*   - params: socket file descriptor, buffer, buffer length, flags
*/
int w_recv(int sockfd, void *buf, size_t len, int flags);

/* W_SEND
*   - sends data on a socket
*   - returns the number of bytes sent
*   - exits on error
*   - params: socket file descriptor, buffer, buffer length, flags
*/
int w_send(int sockfd, const void *buf, size_t len, int flags);

#endif