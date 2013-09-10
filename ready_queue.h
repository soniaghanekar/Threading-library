#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Thread {
	ucontext_t uctxt;
} Thread;	

typedef struct node {
	Thread *thread;
	struct node *next;
} node;	

typedef struct ReadyQueue {
	node *front;
	node *rear;
} ReadyQueue;

void initializeQueue(ReadyQueue *q);

int isQueueEmpty(ReadyQueue *q);

int sizeOfQueue(ReadyQueue *q);

void insertIntoQueue(ReadyQueue *q, Thread *thread);

Thread *removeFromQueue(ReadyQueue *q);

void printQueue(ReadyQueue *q);
