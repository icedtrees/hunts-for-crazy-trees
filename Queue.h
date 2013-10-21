//Queue.h; // interface to Queue ADT
// Written by John Shepherd, March 2013

#ifndef QUEUE_H
#define QUEUE_H
#include "cities.h"
typedef struct _queueNode queueNode;
typedef queueNode *QueueNode;
typedef struct _queue queue;
typedef queue *Queue;

Queue QueueCreate(); // create new empty queue
void QueueDispose(Queue q); // free memory used by queue
void QueuePrint(Queue q); // display as 3 > 5 > 4 > ...
void QueuePush(Queue q, LocationID location, LocationID from); // add item on queue
QueueNode QueuePop(Queue q); // remove item from queue
int QueueEmpty(Queue q); // check for no items

#endif
