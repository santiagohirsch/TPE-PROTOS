#include "list_ADT.h"
#include <stdlib.h>

typedef struct node {
    elem_t elem;
    struct node * next;
} node_t;

typedef node_t * list_t;

struct list_CDT {
    list_t first;
    list_t curr;
    int size;
};

list_ADT new_list() {
    list_ADT list = calloc(1, sizeof(struct list_CDT));
    return list;
}

static void free_list_rec(list_t node) {
    if(node == NULL) {
        return;
    }
    free_list_rec(node->next);
    free(node);
}

void free_list(list_ADT list) {
    free_list_rec(list->first);
    free(list);
}

int is_empty(list_ADT list) {
    return list->size == 0;
}

int size(list_ADT list) {
    return list->size;
}

void to_begin(list_ADT list) {
    list->curr = list->first;
}

int has_next(list_ADT list) {
    return list->curr != NULL;
}

elem_t next(list_ADT list) {
    if (!has_next(list)) {
        return NULL;
    }
    elem_t elem = list->curr->elem;
    list->curr = list->curr->next;
    return elem;
}

static int contains_rec(list_t node, elem_t elem) {
    int c;
    if(node == NULL || (c = compare(node->elem, elem)) > 0) {
        return 0;
    }
    if(c == 0) {
        return 1;
    }
    return contains_rec(node->next, elem);
}

int contains(list_ADT list, elem_t elem) {
    return contains_rec(list->first, elem);
}

static list_t add_elem_rec(list_t node, elem_t elem, int * added) {
    int c;
    if(node == NULL || (c = compare(node->elem, elem)) > 0) {
        list_t aux = malloc(sizeof(struct node));
        aux->elem = elem;
        aux->next = node;
        *added = 1;
        return aux;
    }
    if(c < 0) {
        node->next = add_elem_rec(node->next, elem, added);
    }
    return node;
}

int add_elem(list_ADT list, elem_t elem) {
    int added = 0;
    list->first = add_elem_rec(list->first, elem, &added);
    list->size += added;
    return added;
}

static list_t remove_elem_rec(list_t node, elem_t elem, int * removed) {
    int c;
    if(node == NULL || (c = compare(node->elem, elem)) > 0) {
        return node;
    }
    if(c == 0) {
        list_t aux = node->next;
        free(node);
        *removed = 1;
        return aux;
    }
    node->next = remove_elem_rec(node->next, elem, removed);
    return node;
}

int remove_elem(list_ADT list, elem_t elem) {
    int removed = 0;
    list->first = remove_elem_rec(list->first, elem, &removed);
    list->size -= removed;
    return removed;
}

static elem_t get_rec(list_t node, int index) {
    if(index == 0) {
        return node->elem;
    }
    return get_rec(node->next, index - 1);
}

elem_t get(list_ADT list, int index) {
    if(index >= list->size) {
        return NULL;
    }
    return get_rec(list->first, index);
}