#include "hashfuncs.h"

uint64_t hash_wang_uint64(uint64_t n) {
	n = (~n) + (n << 21);
    n = n ^ (n >> 24);
    n = (n + (n << 3)) + (n << 8);
    n = n ^ (n >> 14);
    n = (n + (n << 2)) + (n << 4);
    n = n ^ (n >> 28);
    n = n + (n << 31);
    return n;
}
uint64_t hash_murmur3_uint64(uint64_t n) {
    n ^= n >> 33;
    n *= 0xff51afd7ed558ccdULL;
    n ^= n >> 33;
    n *= 0xc4ceb9fe1a85ec53ULL;
    n ^= n >> 33;
    return n;
}
uint64_t hash_string_seeded(const char* str, uint32_t len, uint32_t seed) {
	const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    uint64_t *data = (uint64_t *)str;
    const uint64_t *end = data + (len / 8);

    while (data != end) {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const uint8_t* data2 = (const uint8_t*)data;

    switch (len & 7) {
        case 7:
			h ^= (uint64_t)(data2[6]) << 48; 
			goto case6;
		case 6:
			case6:
			h ^= (uint64_t)(data2[5]) << 40; 
			goto case5;
		case 5:
			case5: 
			h ^= (uint64_t)(data2[4]) << 32; 
			goto case4;
		case 4: 
			case4: 
			h ^= (uint64_t)(data2[3]) << 24; 
			goto case3;
		case 3: 
			case3: 
			h ^= (uint64_t)(data2[2]) << 16; 
			goto case2;
		case 2: 
			case2: 
			h ^= (uint64_t)(data2[1]) << 8; 
			goto case1;
		case 1: 
			case1: 
			h ^= (uint64_t)(data2[0]);
            h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

uint64_t hash_float(float x) {
	uint64_t n = *(uint32_t*)(&x);
	n = (n << 32) | (~n);
	return hash_uint64(n);
}
uint64_t hash_double(double x) {
	return hash_uint64(*(uint64_t*)(&x));
}


