#include "ready_queue.h"
#include "mythread.h"

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)


ReadyQueue *queue;
Thread *currentThread;

void MyThreadInit(void(*start_funct)(void *), void *args) {
	queue = (ReadyQueue *)malloc(sizeof(ReadyQueue));
	initializeQueue(queue);
	
	Thread *thread = (Thread *)malloc(sizeof(Thread));
	ucontext_t ctxt;
	
	if(getcontext(&ctxt) == -1)
		handle_error("Could not get context");
		
	char stack[16384];
	ctxt.uc_stack.ss_sp = stack;
	ctxt.uc_stack.ss_size = sizeof(stack);
	ctxt.uc_link = NULL;
	
	makecontext(&ctxt, (void (*)()) start_funct, 1, args);	
		
	thread->uctxt = ctxt;
	insertIntoQueue(queue, *thread);	
	
	Thread dequedThread = removeFromQueue(queue);
	currentThread = &dequedThread;
	setcontext(&(dequedThread.uctxt));
}


void *MyThreadCreate (void (*start_funct)(void *), void *args) {
	
	Thread *thread = (Thread *)malloc(sizeof(Thread));
	ucontext_t ctxt;
	
	if(getcontext(&ctxt) == -1)
		handle_error("Could not get context");
		
	char stack[16384];
	ctxt.uc_stack.ss_sp = stack;
	ctxt.uc_stack.ss_size = sizeof(stack);
	ctxt.uc_link = NULL;
	
	makecontext(&ctxt, (void (*)()) start_funct, 1, args);	
		
	thread->uctxt = ctxt;
	insertIntoQueue(queue, *thread);
	
	return (void *)thread;
}		


void MyThreadYield(void) {
	Thread *this = currentThread;
	//getcontext(&(this->thread));
	insertIntoQueue(queue, *this);
	*currentThread = removeFromQueue(queue);
	swapcontext(&(this->uctxt), &(currentThread->uctxt));
}	


void function1() {
	printf("In Funct 1");
}	

int main() {
	MyThreadInit(function1, 0);
	return 0;
}	
