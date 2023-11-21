#include "stack_ADT.h"
#include <stdlib.h>

typedef struct node {
    action_type elem;
    struct node * next;
} node;

typedef struct stack_cdt {
    node * first;
} stack_cdt;


stack_adt new_stack() {
    stack_adt stack = malloc(sizeof(stack_cdt));
    // CHECK MALLOC
    stack->first = NULL;
    return stack;
}

// TODO: add wrapper functions for malloc
void push(stack_adt stack, action_type elem) {
    node * new_node = malloc(sizeof(node));
    // CHECK MALLOC
    new_node->elem = elem;
    new_node->next = stack->first;
    stack->first = new_node;
}

int pop(stack_adt stack, action_type * elem) {
    node * first = stack->first;
    if (first == NULL) {
        return -1;
    }
    *elem = first->elem;
    stack->first = first->next;
    free(first);
    return 0;
}

int peek(stack_adt stack, action_type * elem) {
    node * first = stack->first;
    if (first == NULL) {
        return -1;
    }
    *elem = first->elem;
    return 0;
}

void delete_stack(stack_adt stack) {
    node * first = stack->first;
    while (first != NULL) {
        node * next = first->next;
        free(first);
        first = next;
    }
    free(stack);
}
