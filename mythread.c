#include "queue.h"
#include "mythread.h"

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)


Queue *readyQueue;
Queue *blockedQueue;
Thread *currentThread;
Thread *initThread;
ucontext_t *initProcesssContext;

Queue *createAndInitializeQueue() {
	Queue *q = (Queue *)malloc(sizeof(Queue));
	initializeQueue(q);
	return q;
}	

SemList *createAndInitializeList() {
	SemList *list = (SemList *)malloc(sizeof(SemList));
	initializeList(list);
	return list;
}	

Thread *getNextThread() {
	Thread *next = dequeue(readyQueue);
	if(next == NULL) {
		setcontext(initProcesssContext);
		return NULL;
	}	
	else	
		return next;
}

Thread *initializeThread(void (*start_funct)(void *), void *args) {
	
	Thread *thread = (Thread *)malloc(sizeof(Thread));
	thread->children = createAndInitializeQueue();
	thread->waitingFor = NULL;
	thread->parent = NULL;
	thread->semaphores = createAndInitializeList();
	
	ucontext_t *ctxt = (ucontext_t *)malloc(sizeof(ucontext_t));
	
	if(getcontext(ctxt) == -1)
		handle_error("Could not get context");
		
	char *stack = (char *)malloc(8192*sizeof(char));
	(ctxt->uc_stack).ss_sp = stack;
	(ctxt->uc_stack).ss_size = 8192;
	ctxt->uc_link = &(currentThread->uctxt);
	
	makecontext(ctxt, (void (*)()) start_funct, 1, args);	
		
	thread->uctxt = *ctxt;
	return thread;
}	


void MyThreadInit(void(*start_funct)(void *), void *args) {
	initProcesssContext = (ucontext_t *)malloc(sizeof(ucontext_t));
	
	readyQueue = createAndInitializeQueue();
	blockedQueue = createAndInitializeQueue();
		
	currentThread = initializeThread(start_funct, args);
	initThread = currentThread;
	
	swapcontext(initProcesssContext, &(currentThread->uctxt));
	return;
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
	currentThread = getNextThread();

	swapcontext(&(current->uctxt), &(currentThread->uctxt));
}


int MyThreadJoin(void *thread) {

	Thread *child = (Thread *)thread;
	
	if(child->parent != currentThread)
		return -1;
	
	if(isPresent(currentThread->children, child)) {
		currentThread->waitingFor = child;
		insertIntoQueue(blockedQueue, currentThread);
		currentThread = getNextThread();
		swapcontext(&(child->parent->uctxt), &(currentThread->uctxt));
	}
	return 0;		
}	


void MyThreadJoinAll(void) {

	if(!isQueueEmpty(currentThread->children)) {
		Thread *this = currentThread;
		insertIntoQueue(blockedQueue, currentThread);
		currentThread = getNextThread();
		swapcontext(&(this->uctxt), &(currentThread->uctxt));
	}	
}	
	

void MyThreadExit(void) {
	Thread *this = currentThread;
	Thread *parent = this->parent;
		
	if(parent != NULL)
		removeFromQueue(parent->children, this);

	if(!isQueueEmpty(blockedQueue) && isPresent(blockedQueue, parent)) {
		
		if(isQueueEmpty(parent->children) || (parent->waitingFor == this)) {
			removeFromQueue(blockedQueue, parent);
			parent->waitingFor = NULL;
			insertIntoQueue(readyQueue, parent);
		}	
	}	 
	
	QueueNode *p = this->children->front;
	while(p != NULL) {
		if(this == initThread)
			p->thread->parent = NULL;
		else {	
			p->thread->parent = initThread;
			insertIntoQueue(initThread->children, p->thread);
		}	
		p = p->next;
	}		
	
	if(!isListEmpty(this->semaphores)) {
		ListNode *p = this->semaphores->head;
		while(p != NULL) {
			(p->sem->value)++;
			removeFromQueue(p->sem->blockedQueue, this);
			p = p->next;
		}	
	}	
	
	currentThread = getNextThread();
	free((this->uctxt).uc_stack.ss_sp);	
	free(this->children);
	this->children = NULL;
	free(this->semaphores);
	this->semaphores = NULL;
	free(this);
	this = NULL;
	

	setcontext(&(currentThread->uctxt));
}


MySemaphore MySemaphoreInit(int initialValue) {
	Semaphore *sem = (Semaphore *)malloc(sizeof(Semaphore));
	sem->value = initialValue;
	sem->blockedQueue = createAndInitializeQueue();
	
	return (MySemaphore) sem;
}	


void MySemaphoreWait(MySemaphore semaphore) {
	Semaphore *sem = semaphore;
	
	(sem->value)--;
	insertIntoList(currentThread->semaphores, sem);
	
	if(sem->value < 0) {
		insertIntoQueue(sem->blockedQueue, currentThread);
		while(sem->value < 0) {
			Thread *this = currentThread;
			currentThread = getNextThread();
			swapcontext(&(this->uctxt), &(currentThread->uctxt));	
		}
	}	
}	


void MySemaphoreSignal(MySemaphore semaphore) {
	Semaphore *sem = semaphore;
	
	(sem->value)++;
	removeFromList(currentThread->semaphores, sem);
	
	if(sem->value <= 0) {
		Thread *thread = dequeue(sem->blockedQueue);
		insertIntoQueue(readyQueue, thread);
	}	
}	

int MySemaphoreDestroy(MySemaphore semaphore) {
	Semaphore *sem = semaphore;
	
	if(sem->value == 0) {
		free(sem->blockedQueue);
		return 0;
	}	
	return -1;
}	

