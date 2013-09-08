#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>

#include "ready_queue.h"

void initializeQueue(ReadyQueue *q) {
	q->front = NULL;
	q->rear = NULL;
}

int isQueueEmpty(ReadyQueue *q) {
	if(q->rear == NULL && q->front == NULL)
	   return 1;
	return 0;
}

void insertIntoQueue(ReadyQueue *q, Thread thread) {
	node *n = (node *)malloc(sizeof(node));
	n->thread = &thread;
	n->next = NULL;
	if(isQueueEmpty(q) == 1) {
		q->front = n;
		q->rear = n; 
	}
	else {
		q->rear->next = n;
		q->rear = n;
	}	
}		   	

Thread removeFromQueue(ReadyQueue *q) {
	if(isQueueEmpty(q) == 1)
		exit(1);
	Thread *thread = q->front->thread;	
	if(q->front == q->rear)
		initializeQueue(q);
	else
		q->front = q->front->next;
	return *thread;
}

int sizeOfQueue(ReadyQueue *q) {
	int n=0;
	node *p= q-> front;
	while(p != NULL) {
		n++;
		p = p->next;
	}
	return n;	
}
