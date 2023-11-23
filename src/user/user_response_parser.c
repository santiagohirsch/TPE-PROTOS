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
    if (strcmp(key, "id") == 0) {
        response->request_id = atoi(value);
        *state = STATUS;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_status(char *key, char *value, struct response *response, parser_state *state) {
    if (strcmp(key, "status") == 0) {
        response->status = atoi(value);
        *state = MESSAGE;
    }
    else {
        *state = ERROR;
    }
    return 0;
}

int parse_message(char *key, char *value, struct response *response, parser_state *state) {
    if (strcmp(key, "message") == 0) {
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
    parse_message,
    parse_error
};

int parse_response(char *response_str, struct response *response) {
    parser_state state = HEADER;
    char *line = strtok(response_str, "\r\n");

    while (line != NULL && state != FINISHED && state != ERROR) {
        char *key_value = strtok(line, ":");
        
        if (key_value != NULL) {
            char *key = key_value;
            char *value = strtok(NULL, ":");
            
            if (value == NULL) {
                state = ERROR;
            } 
            handlers[state](key, value, response, &state);
        }

        line = strtok(NULL, "\r\n");
    }
    return state == FINISHED ? 0 : -1;
}