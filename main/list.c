#include "list.h"
#include "dal.h"

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
			free(node->value);
    		free(node);
			return;
		}
		head = head->next;
	}	
}

uint32_t countNodes(node_t *head){
    uint32_t count = 0;
    while(head->next != NULL){
        count++;
		head = head->next;
	}
    return count;
}

/**
 * Remove node at the specified index
 * \param head		list head
 * \param index		index of the node to be removed
 * \return index of removed node, -1 for not removed
 * */
int32_t removeNodeByIndex(node_t *head, uint32_t index){
node_t *node;
uint32_t count = 0;

	while(head->next != NULL){
		if(count == index){
			node = head->next;
			head->next = node->next;
			free(node->value);
    		free(node);
			return count;
		}
		count++;		
		head = head->next;
	}	
	return -1;
}

/**
 * List sorting functions
 * */
 
/* See https:// www.geeksforgeeks.org/?p=3622 for details of this  
function */
static node_t* SortedMerge(node_t *a, node_t *b, int(*cmp)(void*, void*)) 
{ 
    node_t *result = NULL;

  
    /* Base cases */
    if (a == NULL) 
        return (b); 
    else if (b == NULL) 
        return (a); 
  
    /* Pick either a or b, and recur */
    if (cmp(a->value, b->value) <= 0) { 
        result = a; 
        result->next = SortedMerge(a->next, b, cmp); 
    } 
    else { 
        result = b; 
        result->next = SortedMerge(a, b->next, cmp); 
    } 
    return (result); 
}

/* UTILITY FUNCTIONS */
/* Split the nodes of the given list into front and back halves, 
    and return the two lists using the reference parameters. 
    If the length is odd, the extra node should go in the front list. 
    Uses the fast/slow pointer strategy. */
static void FrontBackSplit(node_t *source, 
                    node_t **frontRef, 
					node_t **backRef) 
{ 
    node_t *fast; 
    node_t *slow; 
    slow = source; 
    fast = source->next; 
  
    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL) { 
        fast = fast->next; 
        if (fast != NULL) { 
            slow = slow->next; 
            fast = fast->next; 
        } 
    } 
  
    /* 'slow' is before the midpoint in the list, so split it in two 
    at that point. */
    *frontRef = source; 
    *backRef = slow->next; 
    slow->next = NULL; 
}

/**
 * Comparator used for the list sorting
 * */ 
int scheduleComparator(void *a, void *b){
	return(((schedule_t*)a)->time - ((schedule_t*)b)->time);
} 

/* sorts the linked list by changing next pointers (not data) */
void sortList(node_t **headRef, int(*cmp)(void*, void*)) 
{ 
    node_t *head = *headRef; 
    node_t *a; 
    node_t *b; 
  
    /* Base case -- length 0 or 1 */
    if ((head == NULL) || (head->next == NULL)) { 
        return; 
    } 
  
    /* Split head into 'a' and 'b' sublists */
    FrontBackSplit(head, &a, &b); 
  
    /* Recursively sort the sublists */
    sortList(&a, cmp); 
    sortList(&b, cmp); 
  
    /* answer = merge the two sorted lists together */
    *headRef = SortedMerge(a, b, cmp); 
} 
 
