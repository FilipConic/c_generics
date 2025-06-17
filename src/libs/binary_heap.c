#include "binary_heap.h"
#include "array.h"
#include "utility.h"

#define get_left_child(pos) 2 * pos + 1
#define get_right_child(pos) 2 * pos + 2
#define get_parent(pos) (pos - 1) / 2

#define swap(a, b) do { \
		(a) ^= (b); \
		(b) ^= (a); \
		(a) ^= (b); \
	} while(0)

void bubble_up(Int32Array* bh, uint32_t pos) {
	if (!pos) {
		return;
	}
	uint32_t parent_pos = get_parent(pos);
	if (bh->buffer[parent_pos] > bh->buffer[pos]) {
		swap(bh->buffer[parent_pos], bh->buffer[pos]);
		bubble_up(bh, parent_pos);
	}
}
void bubble_down(Int32Array* bh, uint32_t pos) {
	uint32_t l = get_left_child(pos), r = get_right_child(pos);
	if (l < bh->len && r < bh->len) {
		if (bh->buffer[l] < bh->buffer[r]) {
			if (bh->buffer[l] < bh->buffer[pos]) {
				swap(bh->buffer[l], bh->buffer[pos]);
				bubble_down(bh, l);
			}
		} else {
			if (bh->buffer[r] < bh->buffer[pos]) {
				swap(bh->buffer[r], bh->buffer[pos]);
				bubble_down(bh, r);
			}
		}
	} else if (l < bh->len) {
		if (bh->buffer[l] < bh->buffer[pos]) {
			swap(bh->buffer[l], bh->buffer[pos]);
			bubble_down(bh, l);
		}
	} else if (r < bh->len) {
		if (bh->buffer[r] < bh->buffer[pos]) {
			swap(bh->buffer[r], bh->buffer[pos]);
			bubble_down(bh, r);
		}
	}
}

void binary_heap_insert(BinaryHeap* bh, int32_t val) {
	array_append(bh, val);
	bubble_up(bh, bh->len - 1);
}
int32_t binary_heap_extract(BinaryHeap* bh) {
	if (!bh->len) {
		fprintf(stderr, ANSI_RED "ERROR:" ANSI_RESET " Trying to extract an element from an empty binary heap!\n");
		exit(1);
	}
	int32_t ret = bh->buffer[0];
	bh->buffer[0] = bh->buffer[--bh->len];
	bubble_down(bh, 0);
	return ret;
}
