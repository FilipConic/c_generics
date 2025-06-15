#include "array.h"
#include "binary_heap.h"
#include "queue.h"
#include "stack.h"

int func(uint32_t x) {
	return x % 2;
}
uint32_t foo(uint32_t x) {
	return x * x;
}

void test_arrays() {
	printf(">>> Testing Arrays <<<\n");
	Uint32Array arr = { 0 };
	uint32_t vals[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	array_multi_append(&arr, vals, 10);
	printf("[ ");
	array_foreach(i, &arr) {
		printf("%d, ", *i);
	}
	printf("]\n");

	Uint32Array arr2 = array_filter(&arr, func);
	printf("[ ");
	array_foreach(i, &arr2) {
		printf("%d, ", *i);
	}
	printf("]\n");

	Uint32Array arr3 = array_map(&arr, foo, Uint32Array);
	printf("[ ");
	array_foreach(i, &arr3) {
		printf("%d, ", *i);
	}
	printf("]\n");

	array_free(&arr3);
	array_free(&arr2);
	array_free(&arr);
}
void test_stacks() {
	printf(">>> Testing Stacks <<<\n");
	Int32Stack s = { 0 };
	stack_push(&s, 10);
	stack_push(&s, 7);
	stack_push(&s, 41);
	stack_push(&s, 23);
	stack_push(&s, 153);
	printf("[ ");
	for (uint32_t i = 0; i < 4; ++i) {
		printf("%d, ", stack_pop(&s));
	}
	printf("%d ]\n", stack_pop(&s));
	stack_free(&s);
}
void test_queues() {
	printf(">>> Testing Queues <<<\n");
	Int32Queue q = { 0 };
	queue_push(&q, 10);
	queue_push(&q, 7);
	queue_push(&q, 41);
	queue_push(&q, 23);
	queue_push(&q, 153);
	printf("[ ");
	for (uint32_t i = 0; i < 4; ++i) {
		printf("%d, ", queue_pop(&q));
	}
	printf("%d ]\n", queue_pop(&q));
	queue_free(&q);
}
void test_binary_heap() {
	printf(">>> Testing Binary Heaps <<<\n");
	BinaryHeap bh = { 0 };
	for (uint32_t i = 0; i < 30; ++i) {
		binary_heap_insert(&bh, rand() % 50);
	}
	int32_t counter = 0;
	uint32_t level = 0;
	array_foreach(i, &bh) {
		if (!counter) {
			printf("[ ");
		}
		printf("%d, ", *i);
		++counter;
		if (counter == (0b1 << level)) {
			++level;
			counter = 0;
			printf("]\n");
		}
	}
	if (counter != 0) printf("]");
	printf("\n");
	
	for (uint32_t i = 0; i < 10; ++i) {
		printf("%d, ", binary_heap_extract(&bh));
	}
	printf("\n");

	counter = 0;
	level = 0;
	array_foreach(i, &bh) {
		if (!counter) {
			printf("[ ");
		}
		printf("%d, ", *i);
		++counter;
		if (counter == (0b1 << level)) {
			++level;
			counter = 0;
			printf(" ]\n");
		}
	}
	if (counter != 0) printf("]");
	printf("\n");
	array_free(&bh);
}

int main(void) {
	test_arrays();
	test_stacks();
	test_queues();
	test_binary_heap();
	return 0;
}
