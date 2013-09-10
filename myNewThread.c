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
	ucontext_t *ctxt = (ucontext_t *)malloc(sizeof(ucontext_t));
	
	if(getcontext(ctxt) == -1)
		handle_error("Could not get context");
		
	char *stack = (char *)malloc(16384*sizeof(char));
	(ctxt->uc_stack).ss_sp = stack;
	(ctxt->uc_stack).ss_size = 16384;
	ctxt->uc_link =  NULL;
	
	makecontext(ctxt, (void (*)()) start_funct, 1, args);
				
	thread->uctxt = *ctxt;	
	currentThread = thread;
	//printf("\nInit currentThread = %p\n", currentThread);
	setcontext(&(currentThread->uctxt));
}


void *MyThreadCreate (void (*start_funct)(void *), void *args) {
	
	Thread *thread = (Thread *)malloc(sizeof(Thread));
	ucontext_t *ctxt = (ucontext_t *)malloc(sizeof(ucontext_t));
	
	if(getcontext(ctxt) == -1)
		handle_error("Could not get context");
		
	char *stack = (char *)malloc(16384*sizeof(char));
	(ctxt->uc_stack).ss_sp = stack;
	(ctxt->uc_stack).ss_size = 16384;
	ctxt->uc_link = (!isQueueEmpty(queue)) ? &(queue->front->thread->uctxt) : NULL;
	
	//printf("\nargs = %d", (int)args);
	makecontext(ctxt, (void (*)()) start_funct, 1, args);	
		
	thread->uctxt = *ctxt;
	insertIntoQueue(queue, thread);
	//printf("\nInit currentThread = %p\n", currentThread);

	//printf("\nCreated: %p", thread);
	//printQueue(queue);
	return (void *)thread;
}		


void MyThreadYield(void) {
	Thread *current = currentThread;
	//printf("\nIn Yield: currentThread = %p", currentThread);
	insertIntoQueue(queue, currentThread);
	currentThread = removeFromQueue(queue);
	//printf("\nNext Thread: currentThread = %p", currentThread);
	//printf("\nBefore swapping: ");
	//printQueue(queue);
	//printf("\nswapping %p %p\n", current, currentThread); 
	
	swapcontext(&(current->uctxt), &(currentThread->uctxt));
	//printf("\nBack in yield\n Current thread: %p", currentThread);
	//printQueue(queue);
}	

void MyThreadExit(void) {
	//printf("Coming here");
	Thread *this = currentThread;
	//printf("\nSize of queue %d", sizeOfQueue(queue));
	if(isQueueEmpty(queue))
		currentThread = NULL;
	else 
		currentThread = removeFromQueue(queue);
	free(this);
	this = NULL;
}


int n;

void t1(void * who){
  int i;

  //printf("\n\nwho= %d", (int)who);
  printf("t%d start\n", (int)who);
  for (i = 0; i < n; i++) {
    printf("t%d yield\n", (int)who);
    MyThreadYield();
  }
  printf("\nt%d end\n", (int)who);
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
