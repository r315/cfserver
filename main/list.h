#ifndef _list_h_
#define _list_h_

#include <stdlib.h>

typedef struct _node_t{
    struct _node_t *next;
    void *value;
}node_t;

static void insertTail(node_t *head, node_t *node){
	while(head->next != NULL){
		head = head->next;
	}
	head->next = node;
}

static node_t *createNode(void *value){
	node_t *node = (node_t*)malloc(sizeof(node_t));

	if(node != NULL){
		node->value = value;
		node->next = NULL;		
	}
	return node;
}

static void removeNode(node_t *head, node_t *node){
	while(head->next != NULL){
		if(head->next == node){
			head->next = node->next;
			return;
		}
		head = head->next;
	}
    free(node);
}

static uint32_t countNodes(node_t *head){
    uint32_t count = 0;
    while(head->next != NULL){
        count++;
		head = head->next;
	}
    return count;
}

#endif