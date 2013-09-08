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
	ctxt.uc_link =  NULL;
	
	makecontext(&ctxt, (void (*)()) start_funct, 1, args);	
		
	thread->uctxt = ctxt;
	insertIntoQueue(queue, *thread);
	
	Thread dequedThread = removeFromQueue(queue);
	currentThread = &dequedThread;
	printf("\nInit dequed %p\n currentThread = %p\n", &dequedThread, currentThread);
	setcontext(&(currentThread->uctxt));
}


void *MyThreadCreate (void (*start_funct)(void *), void *args) {
	
	Thread *thread = (Thread *)malloc(sizeof(Thread));
	ucontext_t ctxt;
	
	if(getcontext(&ctxt) == -1)
		handle_error("Could not get context");
		
	char stack[16384];
	ctxt.uc_stack.ss_sp = stack;
	ctxt.uc_stack.ss_size = sizeof(stack);
	ctxt.uc_link = (currentThread) ? &(currentThread->uctxt) : NULL;
	
	makecontext(&ctxt, (void (*)()) start_funct, 1, args);	
		
	thread->uctxt = ctxt;
	insertIntoQueue(queue, *thread);

	printf("\nCreated: %p", thread);
	return (void *)thread;
}		


void MyThreadYield(void) {
	Thread *current = currentThread;
	printf("In yield: current thread %p\n", currentThread);

	ucontext_t oldContext = current->uctxt;
	
	insertIntoQueue(queue, *current);
	Thread nextThread = removeFromQueue(queue);
	currentThread = &nextThread;
	printf("swapping %p %p\n", current, currentThread); 
	
	swapcontext(&(current->uctxt), &(currentThread->uctxt));
	printf("\nBack in yield");
	currentThread = current;
	currentThread->uctxt = oldContext;
}	

void MyThreadExit(void) {
	Thread *this = currentThread;
	if(isQueueEmpty(queue))
		currentThread = NULL;
	else {
		Thread nextThread = removeFromQueue(queue);
		currentThread = &nextThread;
	}	
	free(this);
}	

void function2(){
	printf("In funct 2");
	MyThreadExit();
}	

void function1() {
	printf("In Funct 1");
	MyThreadCreate(function2, NULL);
	printf("\nSize of queue: %d", sizeOfQueue(queue));
	MyThreadYield();
}	

int main() {
	MyThreadInit(function1, 0);
	return 0;
}	


/*
int n;

void t1(void * who)
{
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

*/
