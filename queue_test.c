#include "ready_queue.h"

#include <assert.h>
#include <stdbool.h>

static void test_initialize_queue() {
	ReadyQueue *queue = (ReadyQueue *)malloc(sizeof(ReadyQueue));
	initializeQueue(queue);
	assert(queue->front == NULL && queue->rear == NULL);
	free(queue);
}	

static void test_insert_and_remove_for_empty_queue() {
	ReadyQueue *queue = (ReadyQueue *)malloc(sizeof(ReadyQueue));
	initializeQueue(queue);
	Thread thread2;
	Thread *thread1 = malloc(sizeof(Thread));
	ucontext_t ctxt;
	getcontext(&ctxt);
	thread1->uctxt = ctxt;
	insertIntoQueue(queue, *thread1);
	assert(queue->front == queue->rear);
	insertIntoQueue(queue, thread2);
	assert(queue->front->next == queue->rear);
	removeFromQueue(queue);
	removeFromQueue(queue);
	assert(queue->front == NULL && queue->rear == NULL);
}	


int main() {
	test_initialize_queue();
	test_insert_and_remove_for_empty_queue();
	return 0;
}
