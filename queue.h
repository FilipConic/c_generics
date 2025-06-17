#ifndef GENERIC_QUEUE_H
#define GENERIC_QUEUE_H

#include "utility.h"
#include "option.h"

#ifndef QUEUE_BASE_SIZE
#define QUEUE_BASE_SIZE BASE_SIZE
#endif // QUEUE_BASE_SIZE

typedef struct {
	int32_t* buffer;
	uint32_t capacity;
	uint32_t len;
	uint32_t head;
	uint32_t tail;
} Int32Queue;

#define queue_push(que, val) do { \
		if ((que)->len == (que)->capacity) { \
			if ((que)->buffer) { \
				typeof(*(que)->buffer)* buf = OPTION_MALLOC(((que)->capacity <<= 1), sizeof(*(que)->buffer)); \
				for (uint32_t i = (que)->head, j = 0; j < (que)->len; i = (i + 1) % (que)->len) { \
					buf[j++] = (que)->buffer[i]; \
				} \
				if (!(que)->buffer) { \
					free((que)->buffer); \
				} \
				(que)->buffer = buf; \
			} else { \
				(que)->buffer = OPTION_MALLOC(QUEUE_BASE_SIZE * sizeof(*(que)->buffer)); \
				(que)->capacity = QUEUE_BASE_SIZE; \
				(que)->len = 0; \
			}\
		} \
		++(que)->len; \
		(que)->buffer[(que)->head++] = val; \
	} while(0)
#define queue_pop(que) ({ \
		if (!(que)->len) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d):" ANSI_RESET " Trying to pop an empty queue!\n", __FILE__, __LINE__); \
			exit(1); \
		}\
		typeof(*(que)->buffer) ret = (que)->buffer[(que)->tail]; \
		(que)->tail = ((que)->tail + 1) % (que)->capacity; \
		--(que)->len; \
		ret; \
	})
#define queue_peek(que) ({ \
		if (!(que)->len) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d):" ANSI_RESET " Trying to pop an empty queue!\n", __FILE__, __LINE__); \
			exit(1); \
		}\
		(que)->buffer[(que)->tail]; \
	})
#define queue_free(que) do { \
		if ((que)->buffer) { \
			free((que)->buffer); \
		}\
		(que)->buffer = NULL; \
		(que)->len = 0; \
		(que)->capacity = 0; \
	} while(0)

#endif // GENERIC_QUEUE_H
