#ifndef GENERICS_HASHFUNCS_H
#define GENERICS_HASHFUNCS_H

#include "utility.h"

#ifndef hash_uint64
#define hash_uint64 hash_murmur3_uint64
#endif // hash_uint64

#ifndef HASH_STRING_SEED 
#define HASH_STRING_SEED 0x12ABCE73F29ABULL
#endif // HASH_STRING_SEED

uint64_t hash_wang_uint64(uint64_t n);
uint64_t hash_murmur3_uint64(uint64_t n);
uint64_t hash_string_seeded(const char* str, uint32_t len, uint32_t seed);
#define hash_murmur3_string(str, len) hash_string_seeded(str, len, HASH_STRING_SEED)

uint64_t hash_float(float x);
uint64_t hash_double(double x);

#endif // GENERIC_HASHFUNCS_H
