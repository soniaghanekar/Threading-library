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
	
	ucontext_t *ctxt = (ucontext_t *)malloc(sizeof(ucontext_t));
	
	if(getcontext(ctxt) == -1)
		handle_error("Could not get context");
		
	char *stack = (char *)malloc(16384*sizeof(char));
	(ctxt->uc_stack).ss_sp = stack;
	(ctxt->uc_stack).ss_size = 16384;
	ctxt->uc_link = (!isQueueEmpty(readyQueue)) ? &(readyQueue->front->thread->uctxt) : NULL;
	
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

void MyThreadExit(void) {
	Thread *this = currentThread;
	
	if(isQueueEmpty(readyQueue))
		currentThread = NULL;
	else 
		currentThread = dequeue(readyQueue);
	free((this->uctxt).uc_stack.ss_sp);	
	free(this);
	this = NULL;
}


int n;

void t1(void * who){
  int i;

  printf("t%d start\n", (int)who);
  for (i = 0; i < n; i++) {
    printf("t%d yield\n", (int)who);
    MyThreadYield();
  }
  printf("t%d end\n", (int)who);
  MyThreadExit();
}

void t0(void * dummy)
{
  MyThreadCreate(t1, (void *)1);
  t1(0);
}

int main(int argc, char *argv[])
{
  if (argc != 2)
    return -1;
  n = atoi(argv[1]);
  MyThreadInit(t0, 0);
}
