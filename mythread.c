#include "queue.h"
#include "mythread.h"

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)


Queue *readyQueue;
Queue *blockedQueue;
Thread *currentThread;
Thread *initThread;

Queue *createAndInitializeQueue() {
	Queue *q = (Queue *)malloc(sizeof(Queue));
	initializeQueue(q);
	return q;
}	


Thread *initializeThread(void (*start_funct)(void *), void *args) {
	
	Thread *thread = (Thread *)malloc(sizeof(Thread));
	thread->children = createAndInitializeQueue();
	thread->waitingFor = NULL;
	thread->parent = NULL;
	
	ucontext_t *ctxt = (ucontext_t *)malloc(sizeof(ucontext_t));
	
	if(getcontext(ctxt) == -1)
		handle_error("Could not get context");
		
	char *stack = (char *)malloc(16384*sizeof(char));
	(ctxt->uc_stack).ss_sp = stack;
	(ctxt->uc_stack).ss_size = 16384;
	ctxt->uc_link = &(currentThread->uctxt);
	
	makecontext(ctxt, (void (*)()) start_funct, 1, args);	
		
	thread->uctxt = *ctxt;
	return thread;
}	


void MyThreadInit(void(*start_funct)(void *), void *args) {
	
	readyQueue = createAndInitializeQueue();
	blockedQueue = createAndInitializeQueue();
		
	currentThread = initializeThread(start_funct, args);
	initThread = currentThread;
	
	setcontext(&(currentThread->uctxt));
}


void *MyThreadCreate (void (*start_funct)(void *), void *args) {

	Thread *thread = initializeThread(start_funct, args);
	insertIntoQueue(readyQueue, thread);
	
	insertIntoQueue(currentThread->children, thread);
	thread->parent = currentThread;
	
	return (void *)thread;
}		


void MyThreadYield(void) {

	Thread *current = currentThread;
	
	insertIntoQueue(readyQueue, currentThread);
	currentThread = dequeue(readyQueue);
	
	swapcontext(&(current->uctxt), &(currentThread->uctxt));
}


int MyThreadJoin(void *thread) {

	Thread *child = (Thread *)thread;
	
	if(child->parent != currentThread)
		return -1;
	
	if(isPresent(currentThread->children, child)) {
		currentThread->waitingFor = child;
		insertIntoQueue(blockedQueue, currentThread);
		currentThread = dequeue(readyQueue);
		swapcontext(&(child->parent->uctxt), &(currentThread->uctxt));
	}
	return 0;		
}	


void MyThreadJoinAll(void) {

	if(!isQueueEmpty(currentThread->children)) {
		Thread *this = currentThread;
		insertIntoQueue(blockedQueue, currentThread);
		currentThread = dequeue(readyQueue);
		swapcontext(&(this->uctxt), &(currentThread->uctxt));
	}	
}	
	

void MyThreadExit(void) {
	Thread *this = currentThread;
	
	if(!isQueueEmpty(blockedQueue) && isPresent(blockedQueue, this->parent)) {
		Thread *parent = this->parent;
		removeFromQueue(parent->children, this);
		
		if(isQueueEmpty(parent->children) || (parent->waitingFor == this)) {
			removeFromQueue(blockedQueue, parent);
			parent->waitingFor = NULL;
			insertIntoQueue(readyQueue, parent);
		}	
	}	 
	
	QueueNode *p = this->children->front;
	while(p != NULL) {
		p->thread->parent == initThread;
		p = p->next;
	}	
	
	currentThread = dequeue(readyQueue);
	
	free((this->uctxt).uc_stack.ss_sp);	
	free(this->children);
	this->children = NULL;
	free(this);
	this = NULL;
}
