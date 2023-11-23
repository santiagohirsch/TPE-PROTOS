#include "udp_commands.h"
#include "udp_request_parser.h"
#include "../server_ADT.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef void (*udp_command)(char * arg1, char * arg2, udp_resp * resp);

void udp_get_bytes(char * arg1, char * arg2, udp_resp * resp) {
    unsigned long package_bytes = get_transferred_bytes_count();
    snprint(resp->value, 256, "%lu", package_bytes);
}

void udp_get_current(char * arg1, char * arg2, udp_resp * resp) {
    int current_users = get_user_session_count();
    snprint(resp->value, 256, "%d", current_clients);
}

void udp_get_history(char * arg1, char * arg2, udp_resp * resp) {
    int history = get_total_user_session_count();
    snprint(resp->value, 256, "%d", history);
}

void udp_change_password(char *buffer) {
    ;
}

void udp_delete_user(char *buffer) {
    ;
}

typedef struct udp_command_elem {
    char * command;
    udp_command func;
} udp_command_elem;

udp_command_elem udp_commands[] = {
    {"GET_BYTES",       &udp_get_bytes},
    {"GET_CURRENT",     &udp_get_current},
    {"GET_HISTORY",     &udp_get_history},
    {"CHANGE_PASSWORD", &udp_change_password},
    {"DELETE_USER",     &udp_delete_user},
    {NULL, NULL}
};

udp_command get_udp_command(char * command) {
    int i = 0;
    while (udp_commands[i].command != NULL) {
        if (strcmp(command, udp_commands[i].command) == 0) {
            return udp_commands[i].func;
        }
        ++i;
    }
    return NULL;
}

void handle_request(udp_rqst * req, udp_resp * resp) {
    udp_command command = get_udp_command(req->command);

    if (command == NULL) {
        resp->status = 1;
        snprint(resp->value, 256, "Command not found");
        return;
    }

    resp->rqst_id = req->rqst_id;
    command(req->arg1, req->arg2, resp);

    resp->status = OK;
}

