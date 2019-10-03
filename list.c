#include <stdlib.h>
#include "list.h"


struct INT_LIST *new_int_list(int elements) {
    int i;
    struct INT_LIST *list = (struct INT_LIST *)malloc(sizeof(struct INT_LIST) * elements);
    list[0].data = 0;
    list[0].prev = NULL;
    list[0].next = NULL;

    struct INT_LIST *element;

    for (i = 1; i < elements;i++) {
        element = new_int_element();
        element -> data = 0;
        element -> prev = &list[i-1];
        list[i-1].next = element;
        element -> next = NULL;
    }

    list[0].prev = element;
    element -> next = &list[0];

    return list;
}

void int_list_append(struct INT_LIST *list, int num) {
    struct INT_LIST *tail = list[0].prev;
    struct INT_LIST *element = new_int_element();
    element -> data = num;
    element -> prev = tail;
    element -> next = &list[0];
    tail -> next = element;
}