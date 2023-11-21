#ifndef _STACK_ADT_H_
#define _STACK_ADT_H_

#include "../session/session.h"

typedef struct stack_cdt * stack_adt;

stack_adt new_stack(void);

void push(stack_adt stack, action_type elem);

int pop(stack_adt stack, action_type * elem);

int peek(stack_adt stack, action_type * elem);

void delete_stack(stack_adt stack);

#endif