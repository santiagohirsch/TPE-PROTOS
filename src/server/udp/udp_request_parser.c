#include "udp_request_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum parser_state {
    HEADER,
    USERNAME,
    PASSWORD,
    ID,
    COMMAND,
    ARG1,
    ARG2,
    FINISHED,
    ERROR
} parser_state;

typedef int (*parser_handler)(char *key, char *value, struct udp_rqst *rqst, parser_state *state);

int parse_header(char *key, char *value, struct udp_rqst *rqst, parser_state *state) {
    if (strcmp(key, "user protocol") == 0) {
        *state = USERNAME;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_username(char *key, char *value, struct udp_rqst *rqst, parser_state *state) {
    if (strcmp(key, "username") == 0) {
        strcpy(rqst->username, value);
        *state = PASSWORD;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_password(char *key, char *value, struct udp_rqst *rqst, parser_state *state) {
    if (strcmp(key, "password") == 0) {
        strcpy(rqst->password, value);
        *state = ID;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_id(char *key, char *value, struct udp_rqst *rqst, parser_state *state) {
    if (strcmp(key, "id") == 0) {
        rqst->id = atoi(value);
        *state = COMMAND;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_command(char *key, char *value, struct udp_rqst *rqst, parser_state *state) {
    if (strcmp(key, "command") == 0) {
        strcpy(rqst->command, value);
        *state = ARG1;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_arg1(char *key, char *value, struct udp_rqst *rqst, parser_state *state) {
    if (strcmp(key, "arg1") == 0) {
        strcpy(rqst->arg1, value);
        *state = ARG2;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_arg2(char *key, char *value, struct udp_rqst *rqst, parser_state *state) {
    if (strcmp(key, "arg2") == 0) {
        strcpy(rqst->arg2, value);
        *state = FINISHED;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_error(char *key, char *value, struct udp_rqst *rqst, parser_state *state) {
    fprintf(stderr, "Error parsing request: no value for key %s\n", key);
    exit(1);    
}

parser_handler handlers[] = {
    parse_header,
    parse_username,
    parse_password,
    parse_id,
    parse_command,
    parse_arg1,
    parse_arg2,
    parse_error
};

static int parse_line(char *line, char *key, char *value) {
    char *svptr;
    char *aux = strtok_r(line, ":\r\n", &svptr);
    strcpy(key, aux);

    while(*svptr == ' ') {
        svptr++;
    }
    aux = strtok_r(NULL, "\r\n", &svptr);
    if (aux != NULL) {
        strcpy(value, aux);
    }
    return 1;
}

int udp_parse_request(char *request, struct udp_rqst *rqst) {
    parser_state state = HEADER;
    char *delim = "\r\n";
    char *svptr;
    char *line = strtok_r(request, delim, &svptr);
    char key_value[32];
    char value[32];

    while (line != NULL) {
        if (parse_line(line, key_value, value)) {
            handlers[state](key_value, value, rqst, &state);
            line = strtok_r(NULL, delim, &svptr);
            key_value[0] = '\0';
            value[0] = '\0';
        }
    }

    return state == FINISHED ? 0 : -1;
}
