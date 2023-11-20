#ifndef _STM_H_
#define _STM_H_

#include "../parser/command_parser.h"

typedef enum state { START, AUTHENTICATION, TRANSACTION, EXIT } state;

typedef struct state_machine * state_machine_ptr;

state_machine_ptr state_machine_init();

void free_state_machine(state_machine_ptr stm);

int state_machine_run(state_machine_ptr stm, struct parser_event *event, char *buffer, int bytes);

state get_state(state_machine_ptr stm);

#endif