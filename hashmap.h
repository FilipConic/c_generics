#ifndef GENERICS_HASHMAP_H
#define GENERICS_HASHMAP_H

#include "utility.h"

#ifndef HASHMAP_BASE_SIZE
#define HASHMAP_BASE_SIZE BASE_SIZE
#endif // HASHMAP_BASE_SIZE

#define __HASHMAP_BITMAP_BASE_SIZE (((HASHMAP_BASE_SIZE - 1) >> 5) + 2)

typedef struct {
	int32_t key;
	int32_t value;
} IntIntKV;

typedef struct {
	uint64_t* bitmap;
	
	IntIntKV* buffer;
	uint32_t capacity;
	uint32_t len;

	uint64_t (*hash)(uint64_t);
	int (*cmp)(int32_t, int32_t);
} IntIntMap;

#define __hashmap_is_filled_pos(map, pos) ((map)->bitmap[(pos) >> 6] & (0b1ULL << ((pos) % 64)))
#define __hashmap_fill_pos(map, pos) ((map)->bitmap[(pos) >> 6] |= (0b1ULL << ((pos) % 64)))
#define __hashmap_empty_pos(map, pos) ((map)->bitmap[(pos) >> 6] &= ~(0b1ULL << ((pos) % 64))) 
#define __hashmap_is_tombstone_pos(map, pos) ((map)->bitmap[(((pos) + (map)->capacity - 1) >> 6) + 1] & (0b1ULL << ((pos) % 64)))
#define __hashmap_bury_pos(map, pos) ((map)->bitmap[(((pos) + (map)->capacity - 1) >> 6) + 1] |= (0b1ULL << ((pos) % 64)))
#define __hashmap_dig_pos(map, pos) ((map)->bitmap[(((pos) + (map)->capacity - 1) >> 6) + 1] &= ~(0b1ULL << ((pos) % 64))) 
#define __hashmap_bit_cmp(a, b) ({ \
		_Static_assert(__builtin_types_compatible_p(typeof(a), typeof(b)), "Trying to compare two variables of different types!\n"); \
		uint32_t len = sizeof(a); \
		typeof(a) _a = (a), _b = (b); \
		uint8_t* a_ptr = (uint8_t*)(&_a); \
		uint8_t* b_ptr = (uint8_t*)(&_b); \
		uint8_t ret = 1; \
		for (uint32_t i = 0; i < len; ++i) { \
			if (*(a_ptr++) != *(b_ptr++)) { \
				ret = 0; \
				break; \
			} \
		} \
		ret; \
	})
#define __hashmap_cmp(map, val, pos) ({ \
		int32_t eq = 0; \
		if (!(map)->cmp) { \
			if (sizeof((val).key) > 8) { \
				fprintf(stderr, ANSI_YELLOW "WARNING (%s, %d): " ANSI_RESET "You should be using a custom compare function for structs!\n", __FILE__, __LINE__); \
			} \
			if (__hashmap_bit_cmp((map)->buffer[pos].key, (val).key)) { \
				eq = 1; \
			} \
		} else { \
			if ((map)->cmp((map)->buffer[pos].key, (val).key)) { \
				eq = 1; \
			} \
		} \
		eq; \
	})
#define __hashmap_cmp_key(map, key_val, pos) ({ \
		int32_t eq = 0; \
		if (!(map)->cmp) { \
			if (sizeof(key_val) > 8) { \
				fprintf(stderr, ANSI_YELLOW "WARNING (%s, %d): " ANSI_RESET "You should be using a custom compare function for structs!\n", __FILE__, __LINE__); \
			} \
			if (__hashmap_bit_cmp((map)->buffer[pos].key, (key_val))) { \
				eq = 1; \
			} \
		} else { \
			if ((map)->cmp((map)->buffer[pos].key, (key_val))) { \
				eq = 1; \
			} \
		} \
		eq; \
	})
#define __hashmap_insert(map, val) do { \
		if (!(map)->hash) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "You didn\'t provide a hash function!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		uint32_t pos = (map)->hash(val.key) % (map)->capacity; \
		int32_t first_tombstone = -1; \
		for (uint32_t i = 0; i < (map)->capacity; ++i) { \
			uint32_t probe = (pos + i) % (map)->capacity; \
	   		if (__hashmap_is_filled_pos(map, probe)) { \
				if (__hashmap_cmp(map, val, probe)) { \
	   				(map)->buffer[probe].value = (val).value; \
	   				break; \
				} \
			} else if (__hashmap_is_tombstone_pos(map, probe)) { \
				first_tombstone = probe; \
			} else { \
				pos = (first_tombstone != -1 ? (uint32_t)first_tombstone : probe); \
	   			(map)->buffer[pos] = (val); \
	   			++(map)->len; \
	   			__hashmap_dig_pos(map, pos); \
	   			__hashmap_fill_pos(map, pos); \
				break; \
			} \
		} \
	} while(0)
#define hashmap_add_kv(map, val) do { \
		if ((map)->len * 1000 >= (map)->capacity * 675) { \
			if ((map)->buffer) { \
				uint64_t* bits = (map)->bitmap; \
				typeof(*(map)->buffer)* buf = (map)->buffer; \
				(map)->capacity <<= 1; \
				(map)->bitmap = calloc((((map)->capacity - 1) >> 5) + 2, sizeof(uint64_t)); \
				(map)->buffer = calloc((map)->capacity, sizeof(*(map)->buffer)); \
				(map)->len = 0; \
				for (uint32_t j = 0; j < ((map)->capacity >> 1); ++j) { \
					if (bits[j >> 6] & (0b1ULL << (j % 64))) { \
						__hashmap_insert(map, buf[j]); \
					} \
				} \
				free(bits); \
			} else { \
				(map)->bitmap = calloc(__HASHMAP_BITMAP_BASE_SIZE, sizeof(uint64_t)); \
				(map)->buffer = calloc(HASHMAP_BASE_SIZE, sizeof(*(map)->buffer)); \
				(map)->capacity = HASHMAP_BASE_SIZE; \
				(map)->len = 0; \
			}\
		} \
		__hashmap_insert(map, val); \
	} while (0)
#define hashmap_add(map, key_val, val_val) hashmap_add_kv(map, ((typeof(*(map)->buffer)){ .key = key_val, .value = val_val, }))
#define hashmap_foreach(val_name, map) uint32_t __CONCAT__(i, __LINE__) = 0; \
	for (typeof(*(map)->buffer)* val_name = &(map)->buffer[0]; __CONCAT__(i, __LINE__)++ < (map)->capacity; ++val_name) \
		if (__hashmap_is_filled_pos(map, (__CONCAT__(i, __LINE__) - 1)))
#define hashmap_remove(map, key_val) do { \
		if (!(map)->hash) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "You didn\'t provide a hash function!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		uint32_t pos = (map)->hash(key_val) % (map)->capacity; \
		for (uint32_t i = 0; i < (map)->capacity; ++i) { \
			uint32_t probe = (pos + i) % (map)->capacity; \
	   		if (__hashmap_is_filled_pos(map, probe)) { \
				if (__hashmap_cmp_key(map, key_val, probe)) { \
	   				__hashmap_bury_pos(map, probe); \
	   				__hashmap_empty_pos(map, probe); \
	   				(map)->buffer[probe].value = 0; \
	   				(map)->buffer[probe].key = 0; \
	   				--(map)->len; \
					break; \
				}\
			} else if (!__hashmap_is_tombstone_pos(map, probe)) { \
				break; \
			}\
		} \
	} while(0)
#define hashmap_contains(map, key_val) ({ \
		if (!(map)->hash) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "You didn\'t provide a hash function!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		uint32_t pos = (map)->hash(key_val) % (map)->capacity; \
		uint32_t found = 0; \
		for (uint32_t i = 0; i < (map)->capacity; ++i) { \
			uint32_t probe = (pos + i) % (map)->capacity; \
			if (__hashmap_is_filled_pos(map, probe)) { \
				if (__hashmap_cmp_key(map, key_val, probe)) { \
					found = 1; \
					break; \
				} \
			} else if (!__hashmap_is_tombstone_pos(map, probe)) { \
				break; \
			} \
		} \
		found; \
	})
#define hashmap_get(map, key_val) ({ \
		if (!(map)->hash) { \
			fprintf(stderr, ANSI_RED "ERROR (%s, %d): " ANSI_RESET "You didn\'t provide a hash function!\n", __FILE__, __LINE__); \
			assert(0); \
		} \
		uint32_t pos = (map)->hash(key_val) % (map)->capacity; \
		int32_t found = -1; \
		for (uint32_t i = 0; i < (map)->capacity; ++i) { \
			uint32_t probe = (pos + i) % (map)->capacity; \
			if (__hashmap_is_filled_pos(map, probe)) { \
				if (__hashmap_cmp_key(map, key_val, probe)) { \
					found = (int32_t)probe; \
					break; \
				} \
			} else if (!__hashmap_is_tombstone_pos(map, probe)) { \
				break; \
			} \
		} \
		typeof((map)->buffer[0].value)* ret = (found != -1 ? &(map)->buffer[found].value : NULL); \
		ret; \
	})
#define hashmap_free(map) do { \
		if ((map)->buffer) { \
			free((map)->buffer); \
		} \
		if ((map)->bitmap) { \
			free((map)->bitmap); \
		} \
		(map)->bitmap = NULL; \
		(map)->buffer = NULL; \
		(map)->len = 0; \
		(map)->capacity = 0; \
	} while(0)

#endif // GENERICS_HASHMAP_H
