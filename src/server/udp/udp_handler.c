#include "udp_handler.h"
#include "udp_request_parser.h"
#include "udp_commands.h"
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>

#define MAX_BYTES_TO_READ 256

void udp_read(struct selector_key *key) {

    char read_buffer[MAX_BYTES_TO_READ];

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    ssize_t read_bytes = recvfrom(key->fd, read_buffer, MAX_BYTES_TO_READ, 0, (struct sockaddr *) &client_addr, &client_addr_len);

    if (read_bytes < 0) {
        perror("recvfrom");
        exit(1);
    }

    udp_rqst *udp_request = malloc(sizeof(udp_rqst));

    if (udp_parse_request(read_buffer, udp_request) < 0) {
        perror("udp_parse_request");
        exit(1);
    }

    printf("UDP request received:\n");
    printf("username: %s\n", udp_request->username);
    printf("password: %s\n", udp_request->password);
    printf("id: %d\n", udp_request->id);
    printf("command: %s\n", udp_request->command);
    printf("arg1: %s\n", udp_request->arg1);
    printf("arg2: %s\n", udp_request->arg2);

    udp_resp *udp_response = malloc(sizeof(udp_resp));

    if (udp_response == NULL) {
        perror("malloc"); // TODO: LOGGER
        exit(1);
    }

    handle_request(udp_request, udp_response);

    char aux[MAX_BYTES_TO_READ];
    int aux_bytes = snprintf(aux, MAX_BYTES_TO_READ, "protocol\r\nrequest_id: %d\r\nstatus_code: %d\r\nresponse: %s\r\n", udp_request->id, udp_response->code, udp_response->value);

    sendto(key->fd, aux, aux_bytes, 0, (struct sockaddr *) &client_addr, client_addr_len);
    
    free(udp_request);
    free(udp_response);
}