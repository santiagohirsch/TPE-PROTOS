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

typedef enum {
    OK = 20,
    CLIENT_ERROR = 40,
    CLIENT_ERROR_UNAUTHORIZED = 41,
    USER_DOES_NOT_EXIST = 42,
    SERVER_ERROR = 50
} udp_resp_code;

typedef struct udp_resp {
    int rqst_id;
    char value[256];
    udp_resp_code code;
} udp_resp;

int udp_parse_request(char *request, struct udp_rqst *rqst);

#endif
