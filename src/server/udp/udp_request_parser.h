#ifndef _UDP_REQUEST_PARSER_H_
#define _UDP_REQUEST_PARSER_H_

typedef struct udp_rqst {
    char username[32];
    char password[32];
    int id;
    char command[32];
    char arg1[32];
    char arg2[32];
}udp_rqst;

int udp_parse_request(char *request, struct udp_rqst *rqst);

#endif