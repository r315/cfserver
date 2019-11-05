#include "list.h"

uint32_t insertTail(node_t *head, node_t *node){
uint32_t count = 0;
	while(head->next != NULL){
		head = head->next;
		count ++;
	}
	head->next = node;
	return count;
}

node_t *createNode(void *value){
	node_t *node = (node_t*)malloc(sizeof(node_t));

	if(node != NULL){
		node->value = value;
		node->next = NULL;		
	}
	return node;
}

void removeNode(node_t *head, node_t *node){
	while(head->next != NULL){
		if(head->next == node){
			head->next = node->next;
			return;
		}
		head = head->next;
	}
    free(node);
}

uint32_t countNodes(node_t *head){
    uint32_t count = 0;
    while(head->next != NULL){
        count++;
		head = head->next;
	}
    return count;
}