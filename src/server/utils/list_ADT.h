#ifndef _LIST_ADT_H_
#define _LIST_ADT_H_

#include <string.h>

typedef struct list_CDT * list_ADT;

typedef char * elem_t;

// Returns 1 if elem1 < elem2, 0 otherwise
static int compare(elem_t elem1, elem_t elem2) {
    return strcmp(elem1, elem2) < 0;
}

list_ADT new_list();

void free_list(list_ADT list);

int add_elem(list_ADT list, elem_t elem);

int remove_elem(list_ADT list, elem_t elem);

int is_empty(list_ADT list);

int contains(list_ADT list, elem_t elem);

int size(list_ADT list);

void to_begin(list_ADT list);

int has_next(list_ADT list);

elem_t next(list_ADT list);

elem_t get_elem(list_ADT list, int index);

#endif