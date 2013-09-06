#include "ready_queue.h"
#include "mythread.h"

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)


ReadyQueue *queue;
Thread *currentThread;


Thread *newThread() {
	return malloc(sizeof(Thread));
}	


void initializeStack(ucontext_t *uctxt) {
	char *stack = (char *)malloc(384*sizeof(char));
	(uctxt->uc_stack).ss_sp = stack;
	(uctxt->uc_stack).ss_size = sizeof(stack);
}


Thread *createThreadAndAddToQueue(void(*start_funct)(void *), void *args, ucontext_t *next_context) {
	Thread *thread = newThread();
	ucontext_t *uctxt = malloc(sizeof(ucontext_t));
	
	if(getcontext(uctxt) == -1)
		handle_error("Could not get context");
	
	char *stack = (char *)malloc(384*sizeof(char));
	(uctxt->uc_stack).ss_sp = stack;
	(uctxt->uc_stack).ss_size = sizeof(stack);	
	//initializeStack(uctxt);
	
	uctxt->uc_link = next_context;	
	makecontext(uctxt, (void (*)())start_funct, 1, args);
	thread->uctxt= uctxt;
	
	//Thread *thread1 = newThread();
	//ucontext_t *p;
	//thread1->uctxt = uctxt;
	
	insertIntoQueue(queue, *thread);
	printf("After insert in queue");
	return thread;
}	


ucontext_t *getNextContext() {
	if(isQueueEmpty(queue))
		return NULL;
	return (queue->front->thread).uctxt;	
}	


void MyThreadInit(void(*start_funct)(void *), void *args) {
	queue = (ReadyQueue *)malloc(sizeof(ReadyQueue));
	initializeQueue(queue);
	
	createThreadAndAddToQueue(start_funct, args, NULL);
	Thread thread = removeFromQueue(queue);
	currentThread = &thread;
	setcontext(thread.uctxt);
}


void *MyThreadCreate (void (*start_funct)(void *), void *args) {
	Thread *thread = createThreadAndAddToQueue(start_funct, args, getNextContext());
	return (void *)thread;
}


void MyThreadYield(void) {
	/*Thread *this = currentThread;
	insertIntoQueue(queue, *this);
	*currentThread = removeFromQueue(queue);
	swapcontext(this->uctxt, currentThread->uctxt);
	currentThread = this;
	printf("HERE");
	setcontext(this->uctxt);*/
}	


void function1() {
	printf("I am here");
}	


int main() {
	MyThreadInit(function1, 0);
	return 0;
}
