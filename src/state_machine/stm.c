#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stm.h"
#include "../parser/command_parser.h"
#include "../server/commands.h"

typedef struct state_machine {
    state state;
} state_machine;

typedef int (*state_handler)(state_machine_ptr stm, session_ptr session, char *buffer, int bytes);

//TODO: Agregar mensajes a un .h (+OK POP3 server READY, -ERR Unknown command, etc)

int start(state_machine_ptr stm, session_ptr session, char *buffer, int bytes) {
    printf("+OK POP3 server READY\n");
    stm->state = AUTHENTICATION;
    return 0;
}

int auth(state_machine_ptr stm, session_ptr session, char *buffer, int bytes) {
    int len = 0;
    struct parser_event *event = get_event(session);
    if (strncmp(event->command, "USER", bytes) == 0) {
        user_cmd(session, event->arg1, event->arg1_len, buffer);
        stm->state = AUTHENTICATION;
    }
    else if (strncmp(event->command, "PASS", bytes) == 0) {
        pass_cmd(session, event->arg1, event->arg1_len, buffer);
        stm->state = TRANSACTION;
        init_client_dir(session);
    }
    else {
        len = strlen("-ERR Unknown command\n");
        strncpy(buffer, "-ERR Unknown command\n", len);
        stm->state = AUTHENTICATION;
    }
    return len;
}

int transaction(state_machine_ptr stm, session_ptr session, char *buffer, int bytes) {
    //TODO: ver si hay mas comandos transaction
    int len;
    struct parser_event *event = get_event(session);
    char response[256] = {0};
    if (strncmp(event->command, "QUIT", bytes) == 0) {
        len = strlen("+OK POP3 server signing off\n");
        strncpy(buffer, "+OK POP3 server signing off\n", len);
        stm->state = EXIT;
    }
    else if (strncmp(event->command, "STAT", bytes) == 0) {
        len = strlen("+OK STAT") + 1
        strncopy(buffer, "+OK STAT", len);
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
        len = strlen("+OK DELE: message deleted\n");
        strncpy(buffer, "+OK DELE: message deleted\n", len);
    } else if (strncmp(event->command,"NOOP", bytes) == 0) {
        len = strlen("+OK NOOP\r\n");
        strncpy(buffer, "+OK NOOP\r\n", len);
    } else if (strncmp(event->command,"RSET",bytes) == 0) {
        rset_cmd(session, event->arg1, event->arg1_len, response);
        len = strlen("+OK RSET\r\n");
        strncpy(buffer, "+OK RSET\r\n", len);
    } else {
        len = strlen("-ERR Unknown command\n");
        strncpy(buffer, "-ERR Unknown command\n", len);
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