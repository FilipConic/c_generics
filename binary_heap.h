#ifndef GENERIC_BINARY_HEAP_H
#define GENERIC_BINARY_HEAP_H

#include "array.h"

typedef Int32Array BinaryHeap;

void binary_heap_insert(Int32Array* bh, int32_t val); 
int32_t binary_heap_extract(Int32Array* bh);

#endif // GENERIC_BINARY_HEAP_H
