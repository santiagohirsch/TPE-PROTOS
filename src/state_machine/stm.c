#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stm.h"

typedef struct state_machine {
    state state;
} state_machine;

typedef void (*state_handler)(state_machine_ptr stm, struct parser *p);

void start(state_machine_ptr stm, struct parser *p) {
    printf("+OK POP3 server READY\n");
    stm->state = AUTHENTICATION;
}

state_handler auth(state_machine_ptr stm, struct parser *p) {
    struct parser_event *event = malloc(sizeof(struct parser_event));
    event = get_command(event, p);
    if (strcmp(event->command, "USER") == 0) {
        printf("+OK\n");
        stm->state = AUTHENTICATION;
    }
    else if (strcmp(event->command, "PASS") == 0) {
        printf("+OK\n");
        stm->state = TRANSACTION;
    }
    else {
        printf("-ERR Unknown command\n");
        stm->state = AUTHENTICATION;
    }
    parser_reset(p);
}

state_handler transaction(state_machine_ptr stm, struct parser *p) {
    struct parser_event *event = malloc(sizeof(struct parser_event));
    event = get_command(event, p);
    if (strcmp(event->command, "QUIT") == 0) {
        printf("+OK POP3 server signing off\n");
        stm->state = EXIT;
    }
    else {
        printf("-ERR Unknown command\n");
        stm->state = TRANSACTION;
    }
    parser_reset(p);
    free(event);
}

state_handler end(state_machine_ptr stm, struct parser *p) {
    stm->state = EXIT;
    parser_reset(p);    
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

void state_machine_run(state_machine_ptr stm, struct parser *p) {
    (*state_handlers[stm->state])(stm, p);
}