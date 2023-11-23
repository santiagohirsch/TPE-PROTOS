#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stm.h"
#include "../parser/command_parser.h"
#include "../server/commands.h"
#include "../utils/stack_ADT.h"

typedef struct state_machine {
    state state;
} state_machine;

typedef int (*state_handler)(state_machine_ptr stm, session_ptr session, char *buffer, int bytes);

//TODO: Agregar mensajes a un .h (+OK POP3 server READY, -ERR Unknown command, etc)

int start(state_machine_ptr stm, session_ptr session, char *buffer, int bytes) {
    pop_action(session);
    int len = strlen("+OK POP3 server READY\n");
    strncpy(buffer, "+OK POP3 server READY\n", len);
    stm->state = AUTHENTICATION;
    return len;
}

int auth(state_machine_ptr stm, session_ptr session, char *buffer, int bytes) {
    int len = 0;
    struct parser_event *event = get_event(session);
    if (strncmp(event->command, "USER", bytes) == 0) {
        user_cmd(session, event->arg1, event->arg1_len, buffer);
        len = strlen("+OK\n");
        strncpy(buffer, "+OK\n", len);
    } else if (strncmp(event->command, "PASS", bytes) == 0) {
        bool is_authenticated = false;
        pass_cmd(session, event->arg1, event->arg1_len, buffer, &is_authenticated);
        if (is_authenticated) {
            len = strlen("+OK Logged in.\n");
            strncpy(buffer, "+OK Logged in.\n", len);
            init_user_dir(session);
            stm->state = TRANSACTION;
        } else {
            len = strlen("-ERR [AUTH] Authentication failed\n");
            strncpy(buffer, "-ERR [AUTH] Authentication failed\n", len);
        }
        
    } else {
        pop_action(session);
        char * response = calloc(256, sizeof(char));
        len = sprintf(response, "-ERR Unknown command%s%s\r\n", strlen(event->command) > 0 ? ": " : ".", strlen(event->command) > 0 ? event->command : "");
        strcpy(buffer, response);
    }
    return len;
}

int transaction(state_machine_ptr stm, session_ptr session, char *buffer, int bytes) {
    //TODO: ver si hay mas comandos transaction
    int len;
    struct parser_event *event = get_event(session);
    char response[256] = {0};
    if (strncmp(event->command, "QUIT", bytes) == 0) {
        pop_action(session);
        len = strlen("+OK POP3 server signing off\r\n");
        strncpy(buffer, "+OK POP3 server signing off\r\n", len);
        stm->state = EXIT;
    }
    else if (strncmp(event->command, "STAT", bytes) == 0) {
        len = strlen("+OK") + 1;
        strncpy(buffer, "+OK", len);
        strcat(buffer, " ");

        len += stat_cmd(session, event->arg1, event->arg1_len, response);
        strcat(buffer, response);
        strcat(buffer, "\r\n");
        len += 2;
    } else if (strncmp(event->command,"DELE", bytes) == 0) {
        int status = dele_cmd(session, event->arg1, event->arg1_len, response);
        if (status == -1) {
            len = strlen(response);
            strncpy(buffer, response, len);
            return len;
        }
        len = strlen("+OK Marked to be deleted.\r\n");
        strncpy(buffer, "+OK Marked to be deleted.\r\n", len);
    } else if (strncmp(event->command,"NOOP", bytes) == 0) {
        pop_action(session);
        len = strlen("+OK\r\n");
        strncpy(buffer, "+OK\r\n", len);
    } else if (strncmp(event->command,"RSET",bytes) == 0) {
        rset_cmd(session, event->arg1, event->arg1_len, response);
        len = strlen("+OK\r\n");
        strncpy(buffer, "+OK\r\n", len);
    } else if (strncmp(event->command, "LIST", bytes) == 0) {
        len = list_cmd(session, event->arg1, event->arg1_len, buffer, bytes);
    } else {
        pop_action(session);
        char * response = calloc(256, sizeof(char));
        len = sprintf(response, "-ERR Unknown command%s%s\r\n", strlen(event->command) > 0 ? ": " : ".", strlen(event->command) > 0 ? event->command : "");
        strcpy(buffer, response);
        stm->state = TRANSACTION;
    }
    return len;
}

int end(state_machine_ptr stm, session_ptr session, char *buffer, int bytes) {
    stm->state = EXIT;
    return 0;
}

state_handler state_handlers[4] = {
    &start,       
    &auth,        
    &transaction, 
    &end         
};

state_machine_ptr state_machine_init() {
    state_machine_ptr stm = malloc(sizeof(struct state_machine));
    stm->state = START;
    return stm;
}

void free_state_machine(state_machine_ptr stm) {
    free(stm);
}

int state_machine_run(state_machine_ptr stm, session_ptr session, char *buffer, int bytes) {
    return (*state_handlers[stm->state])(stm, session, buffer, bytes);
}

state get_state(state_machine_ptr stm) {
    return stm->state;
}