#include "queue.h"

#include <assert.h>
#include <stdbool.h>

static void test_initialize_queue() {
	Queue *queue = (Queue *)malloc(sizeof(Queue));
	initializeQueue(queue);
	assert(queue->front == NULL && queue->rear == NULL);
	free(queue);
}	

static void test_insert_and_deque_for_empty_queue() {
	Queue *queue = (Queue *)malloc(sizeof(Queue));
	initializeQueue(queue);
	Thread thread2;
	Thread *thread1 = malloc(sizeof(Thread));
	ucontext_t ctxt;
	getcontext(&ctxt);
	thread1->uctxt = ctxt;
	insertIntoQueue(queue, thread1);
	assert(queue->front == queue->rear);
	insertIntoQueue(queue, &thread2);
	assert(queue->front->next == queue->rear);
	dequeue(queue);
	dequeue(queue);
	assert(queue->front == NULL && queue->rear == NULL);
}	


static void test_remove_from_queue() {
	Queue *queue = (Queue *)malloc(sizeof(Queue));
	initializeQueue(queue);
	Thread *thread1 = malloc(sizeof(Thread));
	Thread *thread2 = malloc(sizeof(Thread));
	Thread *thread3 = malloc(sizeof(Thread));
	insertIntoQueue(queue, thread1);
	insertIntoQueue(queue, thread2);
	insertIntoQueue(queue, thread3);
	assert(sizeOfQueue(queue) == 3);
	removeFromQueue(queue, thread3);
	printQueue(queue);
	assert(sizeOfQueue(queue) == 2);
}	


int main() {
	test_initialize_queue();
	test_insert_and_deque_for_empty_queue();
	test_remove_from_queue();
	return 0;
}
