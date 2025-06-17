#ifndef GENERICS_MAYBE_HASKELL_H
#define GENERICS_MAYBE_HASKELL_H

#include "utility.h"

typedef struct {
	int32_t value;
	char empty;
} Int32Option;

typedef struct {
	void* value;
	char empty;
} VoidPtrOption;

#define SOME(op, val) ((op){ .value = val, .empty = 0, })
#define NONE(op) ((op){ .value = 0, .empty = 1, })
#define UNWRAP(op) ({ \
	if (op.empty) { \
		fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "Trying to unwrap a None value!\n", __FILE__, __LINE__); \
		exit(1); \
	} \
	op.value; \
})
#define EXPECT(op, msg) ({ \
	if (op.empty) { \
		fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "%s", __FILE__, __LINE__, msg); \
		exit(1); \
	} \
	op.value; \
})
#define option_if(op) if (!op.empty)

#define OPTION_MALLOC(count, size) ({ \
		void* ptr = malloc((count) * (size)); \
		if (!ptr) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "Buy more RAM! Failed at malloc!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		ptr; \
	})
#define OPTION_CALLOC(count, size) ({ \
		void* ptr = calloc(count, size); \
		if (!ptr) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "Buy more RAM! Failed at calloc!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		ptr; \
	})
#define OPTION_REALLOC_ARRAY(src, count, size) ({ \
		void* ptr = reallocarray(src, count, size); \
		if (!ptr) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "Buy more RAM! Failed at realloc_array!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		ptr; \
	})

#endif // GENERICS_MAYBE_HASKELL_H
