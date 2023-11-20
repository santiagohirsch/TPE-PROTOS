#ifndef _STM_H_
#define _STM_H_

#include "../parser/command_parser.h"

typedef enum state { START, AUTHENTICATION, TRANSACTION, EXIT } state;

typedef struct state_machine * state_machine_ptr;

state_machine_ptr state_machine_init();

void free_state_machine(state_machine_ptr stm);

void state_machine_run(state_machine_ptr stm, struct parser *p);

#endif