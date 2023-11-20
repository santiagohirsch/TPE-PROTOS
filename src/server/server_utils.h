#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H


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

#endif