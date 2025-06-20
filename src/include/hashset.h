#ifndef GENERICS_HASHSET_H
#define GENERICS_HASHSET_H

#include "utility.h"
#include "option.h"

#ifndef HASHSET_BASE_SIZE
#define HASHSET_BASE_SIZE BASE_SIZE
#endif // HASHSET_BASE_SIZE

#define __HASHSET_BITMAP_BASE_SIZE (((HASHSET_BASE_SIZE - 1) >> 5) + 2)

typedef struct {
	uint64_t* bitmap;

	int32_t* buffer;
	uint32_t capacity;
	uint32_t len;

	uint64_t (*hash)(uint64_t);
	int (*cmp)(int32_t, int32_t);
} Int32Set;

#define __hashset_is_filled_pos(set, pos) ((set)->bitmap[(pos) / 64] & (0b1ULL << ((pos) % 64)))
#define __hashset_fill_pos(set, pos) ((set)->bitmap[(pos) / 64] |= (0b1ULL << ((pos) % 64)))
#define __hashset_empty_pos(set, pos) ((set)->bitmap[(pos) / 64] &= ~(0b1ULL << ((pos) % 64))) 
#define __hashset_is_tombstone_pos(set, pos) ((set)->bitmap[((pos) + (set)->capacity - 1) / 64 + 1] & (0b1ULL << ((pos) % 64)))
#define __hashset_bury_pos(set, pos) ((set)->bitmap[((pos) + (set)->capacity - 1) / 64 + 1] |= (0b1ULL << ((pos) % 64)))
#define __hashset_dig_pos(set, pos) ((set)->bitmap[((pos) + (set)->capacity - 1) / 64 + 1] &= ~(0b1ULL << ((pos) % 64))) 
#define __hashset_bit_cmp(a, b) ({ \
		_Static_assert(__builtin_types_compatible_p(typeof(a), typeof(b)), "Trying to compare two variables of different types!\n"); \
		uint32_t len = sizeof(a); \
		typeof(a) _a = (a), _b = (b); \
		uint8_t* a_ptr = (uint8_t*)(&_a); \
		uint8_t* b_ptr = (uint8_t*)(&_b); \
		uint8_t ret = 1; \
		for (uint32_t j473 = 0; j473 < len; ++j473) { \
			if (*(a_ptr++) != *(b_ptr++)) { \
				ret = 0; \
				break; \
			} \
		} \
		ret; \
	})
#define __hashset_cmp(set, val, pos) ({ \
		int32_t eq = 0; \
		if (!(set)->cmp) { \
			if (sizeof(val) > 8) { \
				fprintf(stderr, ANSI_YELLOW "WARNING (%s, %d): " ANSI_RESET "You should be using a custom compare function for structs you are using!\n", __FILE__, __LINE__); \
			} \
			if (__hashset_bit_cmp((set)->buffer[pos], val)) { \
				eq = 1; \
			} \
		} else { \
			if ((set)->cmp((set)->buffer[pos], val)) { \
				eq = 1; \
			} \
		} \
		eq; \
	})
#define __hashset_insert(set, val) do { \
		if (!(set)->hash) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "You didn\'t provide a hash function!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		uint32_t pos = (set)->hash(val) % (set)->capacity; \
		int32_t first_tombstone = -1; \
		for (uint32_t i1923 = 0; i1923 < (set)->capacity; ++i1923) { \
			uint32_t probe = (pos + i1923) % (set)->capacity; \
	   		if (__hashset_is_filled_pos(set, probe)) { \
				if (__hashset_cmp(set, val, probe)) { \
	   				break; \
				} \
			} else if (__hashset_is_tombstone_pos(set, probe)) { \
				first_tombstone = probe; \
			} else { \
				pos = (first_tombstone != -1 ? (uint32_t)first_tombstone : probe); \
	   			(set)->buffer[pos] = (val); \
	   			++(set)->len; \
	   			__hashset_dig_pos(set, pos); \
	   			__hashset_fill_pos(set, pos); \
				break; \
			}\
		} \
	} while(0)

#define __hashset_reserve(set, n) do { \
		if ((set)->buffer) { \
			uint64_t* bits = (set)->bitmap; \
			typeof(*(set)->buffer)* buf = (set)->buffer; \
			(set)->capacity = (n); \
			(set)->bitmap = OPTION_CALLOC((((n) - 1) >> 5) + 2, sizeof(uint64_t)); \
			(set)->buffer = OPTION_CALLOC(n, sizeof(*(set)->buffer)); \
			(set)->len = 0; \
			for (uint32_t j823742 = 0; j823742 < ((set)->capacity >> 1); ++j823742) { \
				if (bits[j823742 / 64] & (0b1ULL << (j823742 % 64))) { \
					__hashset_insert(set, buf[j823742]); \
				} \
			} \
			free(bits); \
		} else { \
			(set)->bitmap = OPTION_CALLOC((((n) - 1) >> 5) + 2, sizeof(uint64_t)); \
			(set)->buffer = OPTION_CALLOC((n), sizeof(*(set)->buffer)); \
			(set)->capacity = HASHSET_BASE_SIZE; \
			(set)->len = 0; \
		}\
	} while (0)
#define hashset_add(set, val) do { \
		uint32_t res = (set)->capacity ? (set)->capacity : HASHSET_BASE_SIZE; \
		if ((set)->len * 1000 >= res * 675) { \
			res <<= 1; \
		} \
		if (res != (set)->capacity) __hashset_reserve(set, res); \
		__hashset_insert(set, val); \
	} while(0)
#define hashset_multi_add(set, n, vals) do { \
		uint32_t res = (set)->capacity ? (set)->capacity : HASHSET_BASE_SIZE; \
		while (((set)->len + (n)) * 1000 >= res * 675) { \
			res <<= 1; \
		} \
		if (res != (set)->capacity) __hashset_reserve(set, res); \
		for (uint32_t i328 = 0; i328 < (n); ++i328) { \
			__hashset_insert(set, vals[i328]); \
		} \
	} while (0)
#define hashset_foreach(val_name, set) uint32_t __CONCAT__(i, __LINE__) = 0; \
	for (typeof(*(set)->buffer)* val_name = &(set)->buffer[0]; __CONCAT__(i, __LINE__)++ < (set)->capacity; ++val_name) \
		if (__hashset_is_filled_pos(set, __CONCAT__(i, __LINE__) - 1))
#define hashset_remove(set, val) do { \
		if (!(set)->hash) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "You didn\'t provide a hash function!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		uint32_t pos = (set)->hash(val) % (set)->capacity; \
		for (uint32_t ij738 = 0; ij738 < (set)->capacity; ++ij738) { \
			uint32_t probe = (pos + ij738) % (set)->capacity; \
	   		if (__hashset_is_filled_pos(set, probe)) { \
				if (__hashset_cmp(set, val, probe)) { \
	   				__hashset_bury_pos(set, probe); \
	   				__hashset_empty_pos(set, probe); \
	   				(set)->buffer[probe] = 0; \
	   				--(set)->len; \
					break; \
				}\
			} else if (!__hashset_is_tombstone_pos(set, probe)) { \
				break; \
			}\
		} \
	} while(0)
#define hashset_contains(set, val) ({ \
		if (!(set)->hash) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "You didn\'t provide a hash function!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		uint32_t pos = (set)->hash(val) % (set)->capacity; \
		uint32_t found = 0; \
		for (uint32_t i9483 = 0; i9483 < (set)->capacity; ++i9483) { \
			uint32_t probe = (pos + i9483) % (set)->capacity; \
			if (__hashset_is_filled_pos(set, probe)) { \
				if (__hashset_cmp(set, val, probe)) { \
					found = 1; \
					break; \
				} \
			} else if (!__hashset_is_tombstone_pos(set, probe)) { \
				break; \
			} \
		} \
		found; \
	})
#define hashset_free(set) do { \
		if ((set)->bitmap) { \
			free((set)->bitmap); \
		} \
		if ((set)->buffer) { \
			free((set)->buffer); \
		} \
		(set)->bitmap = NULL; \
		(set)->buffer = NULL; \
		(set)->capacity = 0; \
		(set)->len = 0; \
	} while(0)

#endif // GENERICS_HASHSET_H
