#ifndef _USER_RESPONSE_PARSER_H_
#define _USER_RESPONSE_PARSER_H_

typedef struct response {
    int request_id;
    int status;
    char message[256];
} response;

int parse_response(char *response_str, response *response);

#endif
