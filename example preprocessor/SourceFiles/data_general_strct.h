#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

typedef struct node {
    char *name;
    char *content;
    int line;
    struct node *next;
} node;

#endif

