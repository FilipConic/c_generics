#ifndef GENERIC_STACK_H
#define GENERIC_STACK_H

#include "utility.h"
#include "option.h"

#ifndef STACK_BASE_SIZE
#define STACK_BASE_SIZE BASE_SIZE
#endif // STACK_BASE_SIZE

typedef struct {
	int32_t* buffer;
	uint32_t len;
	uint32_t capacity;
} Int32Stack;

typedef struct {
	uint32_t* buffer;
	uint32_t len;
	uint32_t capacity;
} Uint32Stack;

#define stack_push(stk, val) do { \
		if ((stk)->len == (stk)->capacity) { \
			if ((stk)->buffer) { \
				(stk)->buffer = OPTION_REALLOC_ARRAY((stk)->buffer, (stk)->capacity <<= 1, sizeof(*(stk)->buffer)); \
			} else { \
				(stk)->buffer = OPTION_CALLOC(STACK_BASE_SIZE, sizeof(*(stk)->buffer)); \
				(stk)->capacity = STACK_BASE_SIZE; \
				(stk)->len = 0; \
			}\
		} \
		(stk)->buffer[(stk)->len++] = val; \
	} while (0)
#define stack_multi_push(stk, n, vals) do { \
		uint32_t res = (stk)->capacity ? (stk)->capacity : STACK_BASE_SIZE; \
		while ((stk)->len + (n) < res) { \
			res <<= 1; \
		} \
		(stk)->buffer = OPTION_REALLOC_ARRAY((stk)->buffer, res, sizeof(*(stk)->buffer)); \
		for (uint32_t i3912 = 0; i3912 < n; ++i3912) { \
			stack_push(stk, vals[i3912]); \
		} \
	} while(0)
#define stack_pop(stk) ({ \
		if (!(stk)->len) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d):" ANSI_RESET " Trying to pop an empty stack!\n", __FILE__, __LINE__); \
			exit(1); \
		} \
		(stk)->buffer[--(stk)->len]; \
	})
#define stack_peek(stk) ({ \
		if (!(stk)->len) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d):" ANSI_RESET " Trying to peek at an empty stack!\n", __FILE__, __LINE__); \
			exit(1); \
		} \
		&(stk)->buffer[(stk)->len - 1]; \
	})
#define stack_reverse(stk) ({ \
		typeof(*(stk)) ret = { 0 }; \
		for (uint32_t i72634 = 0; i72634 < (stk)->len; ++i72634) { \
			stack_push(&ret, (stk)->buffer[(stk)->len - 1 - i72634]); \
		} \
		ret; \
	})
#define stack_free(stk) do { \
		if ((stk)->buffer) { \
			free((stk)->buffer); \
		} \
		(stk)->buffer = NULL; \
		(stk)->len = 0; \
		(stk)->capacity = 0; \
	} while(0)

#endif // GENERIC_STACK_H
