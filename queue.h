#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Thread {
	ucontext_t uctxt;
} Thread;	

typedef struct QueueNode {
	Thread *thread;
	struct QueueNode *next;
} QueueNode;	

typedef struct Queue {
	QueueNode *front;
	QueueNode *rear;
} Queue;

void initializeQueue(Queue *q);

int isQueueEmpty(Queue *q);

int sizeOfQueue(Queue *q);

void insertIntoQueue(Queue *q, Thread *thread);

Thread *removeFromQueue(Queue *q);

void printQueue(Queue *q);
