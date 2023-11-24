#include "udp_commands.h"
#include "udp_request_parser.h"
#include "../server_ADT.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "udp_ADT.h"

typedef void (*udp_command)(char * arg1, char * arg2, udp_resp * resp);

void udp_get_bytes(char * arg1, char * arg2, udp_resp * resp) {
    unsigned long package_bytes = get_transferred_bytes_count();
    snprintf(resp->value, 256, "Transferred bytes: %lu", package_bytes);
    resp->code = OK;
}

void udp_get_current(char * arg1, char * arg2, udp_resp * resp) {
    int current_users = get_user_session_count();
    snprintf(resp->value, 256, "Current users: %d", current_users);
    resp->code = OK;
}

void udp_get_history(char * arg1, char * arg2, udp_resp * resp) {
    int history = get_total_user_session_count();
    snprintf(resp->value, 256, "Historic user count: %d", history);
    resp->code = OK;
}

void udp_change_password(char * arg1, char * arg2, udp_resp * resp) {
    char * username = arg1;
    char * password = arg2;

    if (username == NULL || password == NULL) {
        resp->code = CLIENT_ERROR;
        return;
    }

    if (strlen(username) > USERNAME_MAX_LEN || strlen(password) > PASSWORD_MAX_LEN) {
        resp->code = CLIENT_ERROR;
        return;
    }

    struct user_dir * user = get_user_dir(username, strlen(username));

    if (user == NULL) {
        resp->code = USER_DOES_NOT_EXIST;
        return;
    }

    strcpy(user->pass, password);
    snprintf(resp->value, 256, "Changed password for user %s to %s", user->username, user->pass);
    resp->code = OK;
}

void udp_delete_user(char * arg1, char * arg2, udp_resp * resp) {
    char * username = arg1;

    if (username == NULL) {
        resp->code = CLIENT_ERROR;
        return;
    }

    struct user_dir * user = get_user_dir(username, strlen(username));
    if (user == NULL) {
        resp->code = USER_DOES_NOT_EXIST;
        return;
    }

    if (user->deleted) {
        resp->code = SERVER_ERROR;
        return;
    }

    if (delete_user_dir(username, strlen(username)) == 0) {
        snprintf(resp->value, 256, "Deleted user: %s", username);
        resp->code = OK;
        user->deleted = true;
        return;
    }

    resp->code = SERVER_ERROR;

}

void udp_set_concurrent(char * arg1, char * arg2, udp_resp * resp) {
    if (arg1 == NULL) {
        resp->code = CLIENT_ERROR;
        return;
    }

    int concurrent = atoi(arg1);
    if (concurrent <= 0) {
        resp->code = CLIENT_ERROR;
        return;
    }

    if (set_max_concurrent_users(concurrent) != 0) {
        resp->code = SERVER_ERROR;
        return;
    }
   
    snprintf(resp->value, 256, "Set max concurrent users to: %d", concurrent);
    resp->code = OK;
}

typedef struct udp_command_elem {
    char * command;
    udp_command func;
} udp_command_elem;

udp_command_elem udp_commands[] = {
    {"bytes",      udp_get_bytes},
    {"current",    udp_get_current},
    {"history",    udp_get_history},
    {"password",   udp_change_password},
    {"delete",     udp_delete_user},
    {"concurrent", udp_set_concurrent}
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

static bool is_auth(udp_rqst * req, udp_resp * resp) {
    
    if (validate_credentials(req->username, req->password)) {
        return true;
    } 
    resp->code = CLIENT_ERROR_UNAUTHORIZED;
    return false;
}

void handle_request(udp_rqst * req, udp_resp * resp) {

    if (is_auth(req, resp)) {
        udp_command command = get_udp_command(req->command);

        if (command == NULL) {
            resp->code = 1;
            snprintf(resp->value, 256, "Command not found");
            return;
        }
        command(req->arg1, req->arg2, resp);
    }
    

    resp->rqst_id = req->id;
    
}

