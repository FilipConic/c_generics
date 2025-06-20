#ifndef GENERIC_ARRAY_H
#define GENERIC_ARRAY_H

#include "utility.h"
#include "option.h"

#ifndef ARRAY_BASE_SIZE
#define ARRAY_BASE_SIZE BASE_SIZE
#endif // ARRAY_BASE_SIZE

typedef struct {
	int32_t* buffer;
	uint32_t len;
	uint32_t capacity;
} Int32Array;

typedef struct {
	uint32_t* buffer;
	uint32_t len;
	uint32_t capacity;
} Uint32Array;

#define array_append(arr, val) do { \
		if ((arr)->len == (arr)->capacity) { \
			 if ((arr)->buffer) { \
				(arr)->buffer = OPTION_REALLOC_ARRAY((arr)->buffer, (arr)->capacity <<= 1, sizeof(*(arr)->buffer)); \
			} else { \
				(arr)->buffer = OPTION_CALLOC(ARRAY_BASE_SIZE, sizeof(*(arr)->buffer)); \
				(arr)->capacity = ARRAY_BASE_SIZE; \
				(arr)->len = 0; \
			} \
		} \
		(arr)->buffer[(arr)->len++] = (val); \
	} while(0) 
#define array_multi_append(arr, vals, count) do { \
		if (count <= 0) { \
			continue; \
		} \
		while ((count) + (arr)->len > (arr)->capacity) { \
			if ((arr)->buffer) { \
				(arr)->buffer = OPTION_REALLOC_ARRAY((arr)->buffer, (arr)->capacity <<= 1, sizeof(*(arr)->buffer)); \
			} else { \
				(arr)->buffer = OPTION_CALLOC(ARRAY_BASE_SIZE, sizeof(*(arr)->buffer)); \
				(arr)->capacity = ARRAY_BASE_SIZE; \
				(arr)->len = 0; \
			} \
		} \
		for (uint32_t ijk327 = 0; ijk327 < (count); ++ijk327) { \
			(arr)->buffer[(arr)->len++] = (vals)[ijk327]; \
		} \
	} while (0)
#define array_remove(arr, pos) do { \
		if (pos >= (arr)->len) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d):" ANSI_RESET " Index out of range!\n", __FILE__, __LINE__); \
			exit(1); \
		} \
		--(arr)->len; \
		for (uint32_t ikj283 = pos; ikj283 < (arr)->len; ++ikj283) { \
			(arr)->buffer[ikj283] = (arr)->buffer[ikj283 + 1]; \
		} \
	} while(0)
#define array_insert(arr, val, pos) do { \
		if (pos >= (arr)->len) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d):" ANSI_RESET " Index out of range!\n", __FILE__, __LINE__); \
			exit(1); \
		} \
		array_append(arr, val); \
		for (uint32_t iup12365 = (arr)->len - 1; iup12365 > pos; --iup12365) { \
			(arr)->buffer[iup12365] = (arr)->buffer[iup12365 - 1]; \
		} \
		(arr)->buffer[pos] = val; \
	} while(0)
#define array_free(arr) do { \
		if ((arr)->buffer) { \
			free((arr)->buffer); \
		} \
		(arr)->buffer = NULL; \
		(arr)->capacity = 0; \
		(arr)->len = 0; \
	} while(0)
#define array_move(arr1, arr2) do { \
		array_free(arr1); \
		(arr1)->buffer = (arr2)->buffer; \
		(arr1)->capacity = (arr2)->capacity; \
		(arr1)->len = (arr2)->len; \
		(arr2)->buffer = NULL; \
		(arr2)->capacity = 0; \
		(arr2)->len = 0; \
	} while (0)
#define array_filter(arr, func) ({ \
		typeof(*arr) res = { 0 }; \
		for (uint32_t i57 = 0; i57 < (arr)->len; ++i57) { \
			if ((*func)((arr)->buffer[i57])) { \
				array_append(&res, (arr)->buffer[i57]); \
			} \
		} \
		res; \
	})
#define array_map(arr, func, arr_res_type) ({ \
		arr_res_type res = { 0 }; \
		for (uint32_t i8459 = 0; i8459 < (arr)->len; ++i8459) { \
			array_append(&res, func((arr)->buffer[i8459])); \
		} \
		res; \
	})
#define array_find_by_func(arr, val, cmp) ({ \
		int ret = -1; \
		for (uint32_t n982 = 0; n982 < (arr)->len; ++n982) { \
			if ((cmp)(&val, &((arr)->buffer[n982]))) { \
				ret = (int)n982;\
				break; \
			} \
		} \
		ret;\
	})
#define array_find(arr, val) ({ \
		int ret = -1; \
		for (uint32_t i6723 = 0; i6723 < (arr)->len; ++i6723) { \
			if ((arr)->buffer[i6723] == val) { \
				ret = (int)i6723; \
				break; \
			} \
		} \
		ret; \
	})
#define array_foreach(val_name, arr) uint32_t __CONCAT__(__i, __EVAL__(__LINE__)) = 0; \
	for (typeof(*(arr)->buffer)* val_name = (arr)->buffer; __CONCAT__(__i, __EVAL__(__LINE__))++ < (arr)->len; ++val_name)

#endif // GENERIC_ARRAY_H
