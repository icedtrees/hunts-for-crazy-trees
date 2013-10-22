// Queue.c ... implementation of Queue ADT
// Written by John Shepherd, March 2013
// Modified for use with hunter.c

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Queue.h"

struct _queueNode {
	queueData data;
	QueueNode next;
};

struct _queue {
	QueueNode head;  // ptr to first node
	QueueNode tail;  // ptr to last node
};

// create new empty Queue
Queue QueueCreate() {
	Queue q;
	q = malloc(sizeof(queue));
	q->head = NULL;
	q->tail = NULL;
	return q;
}

// free memory used by Queue
void QueueDispose (Queue q)
{
	QueueNode curr, next;
	assert(q != NULL);
	// free list nodes
	curr = q->head;
	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
	// free queue
	free(q);
}

// display as 3 > 5 > 4 > ...
void QueuePrint (Queue q) {
	QueueNode curr;
	assert(q != NULL);
	// free list nodes
	curr = q->head;
	while (curr != NULL) {
		printf("(%d, %d)", curr->data.location, curr->data.from);
		if (curr->next != NULL) {
			printf(">");
		}
		curr = curr->next;
	}
	printf("\n");
}

// add item at end of Queue 
void QueuePush (Queue q, LocationID location, LocationID from, Round round) {
	assert(q != NULL);
	QueueNode new = malloc(sizeof(queueNode));
	assert(new != NULL);
	new->data.location = location;
	new->data.from = from;
	new->data.round = round;
	new->next = NULL;
	if (q->head == NULL) {
		q->head = new;
	}
	if (q->tail != NULL) {
		q->tail->next = new;
	}
	q->tail = new;
}

// remove item from front of Queue
queueData QueuePop (Queue q) {
	assert(q != NULL);
	assert(q->head != NULL);
	QueueNode node = q->head;
	q->head = node->next;
	queueData data = node->data;
	free(node);
	if (q->head == NULL) {
		q->tail = NULL;
	}
	return data;
}

// check for no items
int QueueEmpty (Queue q) {
	return (q->head == NULL);
}

