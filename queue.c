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

Thread *dequeue(Queue *q) {
	if(isQueueEmpty(q) == 1)
		return NULL;
	Thread *thread = q->front->thread;	
	if(q->front == q->rear)
		initializeQueue(q);
	else
		q->front = q->front->next;
	return thread;
}

int removeFromQueue(Queue *q, Thread *thread) {
	QueueNode *p = q->front;
	QueueNode *r = q->front;
	while(p != NULL) {
		if(p->thread == thread) {
			if(p == q->front && p == q->rear)
				initializeQueue(q);
			else if(p == q->front)	
				q->front = p->next;
			else if(p == q->rear) {
				q->rear = r;
				r->next = NULL;
			}	
			else	
				r->next = p->next;
			free(p);
			return 1;
		}	
		r = p;
		p = p-> next;
	}	
	return 0;
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

int isPresent(Queue *q, Thread *thread) {
	QueueNode *p = q->front;
	while(p != NULL) {
		if(p->thread == thread)
			return 1;
		p = p-> next;	
	}	
	return 0;
}	

int initializeList(SemList *list) {
	list->head = NULL;
}	

int isListEmpty(SemList *list) {
	if(list->head == NULL)
		return 1;
	return 0;	
}	

void insertIntoList(SemList *list, Semaphore *sem) {
	ListNode *n = (ListNode *)malloc(sizeof(ListNode));
	n->sem = sem;
	n->next = (isListEmpty(list)) ? NULL : list->head->next;
	list->head = n;
}	

int removeFromList(SemList *list, Semaphore *sem) {
	ListNode *p = list->head;
	ListNode *q = list->head;
	while(p != NULL) {
		if(p->sem == sem) {
			if(p == list->head)
				list->head = list->head->next;
			else
				q->next = p->next;
			free(p);
			return 1;	
		}	
		q = p;
		p = p->next;
	}	
	return 0;
}	



