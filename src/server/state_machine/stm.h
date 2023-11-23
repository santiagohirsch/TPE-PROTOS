#ifndef _STM_H_
#define _STM_H_

typedef enum state { START, AUTHENTICATION, TRANSACTION, EXIT } state;

#include "../session/session.h"

typedef struct state_machine * state_machine_ptr;

state_machine_ptr state_machine_init();

void free_state_machine(state_machine_ptr stm);

int state_machine_run(state_machine_ptr stm, session_ptr session, char *buffer, int bytes);

state get_state(state_machine_ptr stm);

#endif