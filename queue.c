#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>

#include "queue.h"

void initializeQueue(Queue *q) {
	q->front = NULL;
	q->rear = NULL;
}

int isQueueEmpty(Queue *q) {
	if(q->rear == NULL && q->front == NULL)
	   return 1;
	return 0;
}

void insertIntoQueue(Queue *q, Thread *thread) {
	QueueNode *n = (QueueNode *)malloc(sizeof(QueueNode));
	n->thread = thread;
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

Thread *removeFromQueue(Queue *q) {
	if(isQueueEmpty(q) == 1)
		exit(1);
	Thread *thread = q->front->thread;	
	if(q->front == q->rear)
		initializeQueue(q);
	else
		q->front = q->front->next;
	return thread;
}

int sizeOfQueue(Queue *q) {
	int n=0;
	QueueNode *p= q-> front;
	while(p != NULL) {
		n++;
		p = p->next;
	}
	return n;	
}

void printQueue(Queue *q) {
	QueueNode *p = q->front;
	printf("\n.........Queue.......");
	while(p != NULL) {
		printf("\n%p", p->thread);
		p = p->next;
	}
	printf("\n.....................");		
}	
