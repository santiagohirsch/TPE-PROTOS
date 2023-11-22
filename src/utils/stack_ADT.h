#ifndef _STACK_ADT_H_
#define _STACK_ADT_H_

#include "../session/session.h"

typedef struct stack_cdt * stack_adt;

typedef struct {
    action_type elem;
} data_t;

stack_adt new_stack(void);

void push(stack_adt stack, data_t elem);

int pop(stack_adt stack, data_t * elem);

int peek(stack_adt stack, data_t * elem);

void delete_stack(stack_adt stack);

#endif