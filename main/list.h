#ifndef _list_h_
#define _list_h_

#include <stdlib.h>
#include <stdint.h>

typedef struct _node_t{
    struct _node_t *next;
    void *value;
}node_t;

uint32_t insertTail(node_t *head, node_t *node);
node_t *createNode(void *value);
void removeNode(node_t *head, node_t *node);
uint32_t countNodes(node_t *head);

#endif