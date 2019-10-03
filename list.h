#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

#define new_int_element() (struct INT_LIST *)malloc(sizeof(struct INT_LIST))

typedef struct INT_LIST {
    int data;
    struct INT_LIST *prev;
    struct INT_LIST *next;
} INT_LIST;

extern struct INT_LIST *new_int_list(int elements);
extern void int_list_append(struct INT_LIST *list, int num);

#endif