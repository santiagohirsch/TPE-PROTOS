#include "session.h"
#include "../state_machine/stm.h"
#include "connection.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../parser/command_parser.h"


typedef struct user_session {
    long id;
    state_machine_ptr stm;
    char * username;
    bool is_auth;
    connection c;
} user_session;

static long id = 0;

user_session * new_session(connection c) {
    user_session * session = malloc(sizeof(user_session));
    session->id = id++;
    session->username = NULL;
    session->is_auth = false;
    session->stm = state_machine_init();
    session->c = c;
    return session;
}

void start_session(user_session * session) {
    struct parser * p = command_parser_init();
    printf("Connection %d - STARTED\n", session->c);

    while(1) {
        state_machine_run(session->stm, p);

        if (session->stm == EXIT) {
            break;
        }
    }
    
    printf("Connection %d - ENDED\n", session->c);
}

void delete_user_session(user_session * session) {
    free(session->username);
    free(session);
}

bool session_auth(user_session * session, char * password) {
    if(strcmp(password, "123456") == 0) {
        session->is_auth = true;
        return true;
    }
    return false;
}