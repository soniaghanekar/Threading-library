#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Queue Queue; 

typedef struct Thread {
	ucontext_t uctxt;
	struct Thread *parent;
    Queue *children;
	struct Thread *waitingFor;
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

Thread *dequeue(Queue *q);

int removeFromQueue(Queue *q, Thread *thread);

void printQueue(Queue *q);

int isPresent(Queue *l, Thread *thread);

