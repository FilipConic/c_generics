#include "array.h"
#include "binary_heap.h"
#include "queue.h"
#include "stack.h"
#include "hashset.h"
#include "hashfuncs.h"
#include "hashmap.h"
#include "dynamic_string.h"
#include "regex.h"

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
void test_hashset() {
	printf(">> Test Hashset <<<\n");
	Int32Set s = { 0 };
	s.hash = hash_uint64;
	for (int i = 0; i < 100; ++i) {
		int k = rand() % 50;
		hashset_add(&s, k);
	}
	printf("%u, %u -> { ", s.capacity, s.len);
	hashset_foreach(el, &s) {
		printf("%d, ", *el);
	}
	printf("}\n");
	hashset_remove(&s, 10);
	hashset_remove(&s, 8);
	hashset_remove(&s, 20);
	hashset_remove(&s, 30);
	hashset_remove(&s, 40);
	printf("%u, %u -> { ", s.capacity, s.len);
	hashset_foreach(el, &s) {
		printf("%d, ", *el);
	}
	printf("}\n");
	hashset_free(&s);
}
void test_hashmap() {
	printf(">>> Test Hashmap <<<\n");
	IntIntMap hm = { 0 };
	hm.hash = hash_uint64;
	hashmap_add(&hm, 2, 3);
	hashmap_add(&hm, 5, 8);
	printf("{ ");
	hashmap_foreach(kv, &hm) {
		printf("(%d: %d), ", kv->key, kv->value);
	}
	printf("}\n");
	int32_t* v1 = hashmap_get(&hm, 2);
	if (v1) printf("%d\n", *v1); else printf("None\n");
	int32_t* v2 = hashmap_get(&hm, 8);
	if (v2) printf("%d\n", *v2); else printf("None\n");
	int32_t* v3 = hashmap_get(&hm, 5);
	if (v3) printf("%d\n", *v3); else printf("None\n");
	hashmap_free(&hm);
}
void test_string() {
	printf(">>> Test Strings <<<\n");
	String s = { 0 };
	printf("Start\n");
	string_append_c_str(&s, "Hello World! How are you doing today?");

	printf("1: \"%s\"\n", s.buffer);
	string_remove(&s, ' ');
	printf("2: \"%s\"\n", s.buffer);

	StringSlice sl = string_get_slice(&s, 5, 11);
	string_append_slice(&s, &sl);
	printf("3: \"%s\"\n", s.buffer);
	string_reserve(&s, 20);
	printf("4: \"%s\"\n", s.buffer);
	
	String s2 = { 0 };
	string_append_file(&s2, "queue.h");

	string_free(&s2);
	string_free(&s);
}
void test_regex() {
	printf(">>> Testing DFA <<<\n");
	DFA* dfa = dfa_create();
	String s = { 0 };
	string_append_c_str(&s, "aabbb");
	printf("1: %s -> %d\n", s.buffer, dfa_run(dfa, &s));
	s.len = 0;
	string_append_c_str(&s, "abbbab");
	printf("2: %s -> %d\n", s.buffer, dfa_run(dfa, &s));
	s.len = 0;
	string_append_c_str(&s, "bbbbbb");
	printf("3: %s -> %d\n", s.buffer, dfa_run(dfa, &s));
	s.len = 0;
	string_append_c_str(&s, "bbbaaaa");
	printf("4: %s -> %d\n", s.buffer, dfa_run(dfa, &s));
	s.len = 0;
	string_free(&s);
	dfa_free(dfa);
}

int main(void) {
	test_arrays();
	test_stacks();
	test_queues();
	test_binary_heap();
	test_hashset();
	test_hashmap();
	test_string();
	test_regex();

	return 0;
}
