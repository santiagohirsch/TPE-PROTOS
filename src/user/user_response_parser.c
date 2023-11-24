#include "user_response_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum parser_state {
    HEADER,
    ID,
    STATUS,
    MESSAGE,
    FINISHED,
    ERROR
} parser_state;

typedef int (*parser_handler)(char *key, char *value, struct response *response, parser_state *state);

int parse_header(char *key, char *value, struct response *response, parser_state *state) {
    if (strcmp(key, "user protocol") == 0) {
        *state = ID;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_id(char *key, char *value, struct response *response, parser_state *state) {
    if (strcmp(key, "request_id") == 0) {
        response->request_id = atoi(value);
        *state = STATUS;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_status(char *key, char *value, struct response *response, parser_state *state) {
    if (strcmp(key, "status_code") == 0) {
        response->status = atoi(value);
        *state = MESSAGE;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_value(char *key, char *value, struct response *response, parser_state *state) {
    if (strcmp(key, "value") == 0) {
        strcpy(response->message, value);
        *state = FINISHED;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_error(char *key, char *value, struct response *response, parser_state *state) {
    *state = ERROR;
    return 0;
}

parser_handler handlers[] = {
    parse_header,
    parse_id,
    parse_status,
    parse_value,
    parse_error
};

static int parse_line(char *line, char *key, char *value) {
    char *svptr = line;
    char *aux = strtok_r(line, ":\r\n", &svptr);
    strcpy(key, aux);

    while(svptr != NULL && *svptr == ' ') {
        svptr++;
    }
    aux = strtok_r(NULL, "\r\n", &svptr);
    if (aux != NULL) {
        strcpy(value, aux);
    }
    return 1;
}

int parse_response(char *response_str, struct response *response) {
    parser_state state = HEADER;
    char *delim = "\r\n";
    char *svptr = response_str;
    char *line = strtok_r(response_str, delim, &svptr);
    char key_value[32];
    char value[32];

    while (line != NULL && state != FINISHED) {
        if (parse_line(line, key_value, value)) {
            handlers[state](key_value, value, response, &state);
            line = strtok_r(NULL, delim, &svptr);
            key_value[0] = '\0';
            value[0] = '\0';
        }
    }

    return state == FINISHED ? 0 : -1;
}
