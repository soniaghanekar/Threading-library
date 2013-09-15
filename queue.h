#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Queue Queue; 
typedef struct SemaphoreList SemList;

typedef struct Thread {
	ucontext_t uctxt;
	struct Thread *parent;
    Queue *children;
	struct Thread *waitingFor;
	SemList *semaphores;
} Thread;	


typedef struct Semaphore {
	int value;
	Queue *blockedQueue;
} Semaphore;	


typedef struct QueueNode {
	Thread *thread;
	struct QueueNode *next;
} QueueNode;	


typedef struct Queue {
	QueueNode *front;
	QueueNode *rear;
} Queue;


typedef struct ListNode {
	Semaphore *sem;
	struct ListNode *next;
} ListNode;

typedef struct SemaphoreList {
	ListNode *head;
} SemList;		

void initializeQueue(Queue *q);

int isQueueEmpty(Queue *q);

int sizeOfQueue(Queue *q);

void insertIntoQueue(Queue *q, Thread *thread);

Thread *dequeue(Queue *q);

int removeFromQueue(Queue *q, Thread *thread);

void printQueue(Queue *q);

int isPresent(Queue *l, Thread *thread);

int initializeList(SemList *list);

int isListEmpty(SemList *list);

void insertIntoList(SemList *list, Semaphore *sem);

int removeFromList(SemList *list, Semaphore *sem);
