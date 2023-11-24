#include "user_request.h"
#include "user_response_parser.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define UDP_PORT 5000

int main(int argc, char *argv[])
{
    struct request * req = get_request(argc - 1, argv + 1);
    char rq_buffer[1024];
    char rp_buffer[1024];

    build_request(req, rq_buffer);
    printf("Request: \n%s\n", rq_buffer);

    int rq_buffer_len = strlen(rq_buffer);
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(sock < 0) {
        free(req);
        perror("[ERROR] socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        free(req);
        close(sock);
        perror("[ERROR] setsockopt");
        exit(1);
    }

    ssize_t sent = sendto(sock, rq_buffer, rq_buffer_len, 0, (struct sockaddr *)&server_addr, server_addr_len);
    if (sent < 0) {
        free(req);
        close(sock);
        perror("[ERROR] sendto");
        exit(1);
    } else if (sent != rq_buffer_len) {
        free(req);
        close(sock);
        fprintf(stderr, "sendto: sent %ld bytes, expected %d\n", sent, rq_buffer_len);
        exit(1);
    }

    ssize_t received = recvfrom(sock, rp_buffer, sizeof(rp_buffer), 0, (struct sockaddr *)&server_addr, &server_addr_len);
    if (received < 0) {
        free(req);
        close(sock);
        perror("[ERROR] recvfrom");
        exit(1);
    } else {
        rp_buffer[received] = '\0';
        printf("Response: %s\n", rp_buffer);
    }

    struct response * resp = malloc(sizeof(struct response));
    if (parse_response(rp_buffer, resp) < 0) {
        free(req);
        free(resp);
        close(sock);
        fprintf(stderr, "parse_response: error\n");
        exit(1);
    }

    switch(resp->status) {
        case 20:
            printf("%s\n", resp->message);
            break;
        case 40:
            printf("Client error\n");
            break;
        case 41:
            printf("Client error: unauthorized\n");
            break;
        case 42:
            printf("Client error: user does not exist\n");
            break;
        case 50:
            printf("Server error\n");
            break;
        default:
            printf("Unknown error\n");
            break;
    }

    free(req);
    free(resp);
    close(sock);
    return 0;
}