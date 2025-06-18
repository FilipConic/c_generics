#include "regex.h"

#include "array.h"
#include "dynamic_string.h"
#include "hashset.h"
#include "hashmap.h"
#include "hashfuncs.h"
#include "stack.h"
#include "option.h"

typedef struct CharSet {
	uint64_t* bitmap;

	char* buffer;
	uint32_t capacity;
	uint32_t len;

	uint64_t (*hash)(uint64_t);
	uint64_t (*cmp)(char, char);
} CharSet;

typedef struct StateSymbol {
	int32_t state;
	char symbol;
} StateSymbol;
typedef struct StateKV {
	StateSymbol key;
	int32_t value;
} StateKV;
uint64_t state_hash(StateSymbol s) {
	return hash_uint64(s.state ^ ((s.symbol * 0xfab48712bacd834ULL) << 27));
}
int state_cmp(StateSymbol s1, StateSymbol s2) {
	return (s1.symbol == s2.symbol) && (s1.state == s2.state);
}
typedef struct StateMap {
	uint64_t* bitmap;

	StateKV* buffer;
	uint32_t capacity;
	uint32_t len;

	uint64_t (*hash)(StateSymbol);
	int (*cmp)(StateSymbol, StateSymbol);
} StateMap;

struct __DFA {
	Int32Set states;
	CharSet symbols;
	StateMap transition;
	int32_t initial_state;
	Int32Set final;
};

DFA* dfa_create() {
	DFA* dfa = OPTION_CALLOC(1, sizeof(DFA));
	dfa->states.hash = hash_uint64;
	dfa->transition.hash = state_hash;
	dfa->transition.cmp = state_cmp;
	dfa->symbols.hash = hash_uint64;
	dfa->final.hash = hash_uint64;
	hashset_multi_add(&dfa->states, 3, ((int[]){0, 1, 2}));
	hashset_multi_add(&dfa->symbols, 2, ((char[]){'a', 'b'}));
	hashmap_multi_add(&dfa->transition, 6, ((StateKV[]) { 
		{ .key = { .state = 0, .symbol = 'a' }, .value = 0, },
		{ .key = { .state = 0, .symbol = 'b' }, .value = 1, },
		{ .key = { .state = 1, .symbol = 'a' }, .value = 2, },
		{ .key = { .state = 1, .symbol = 'b' }, .value = 1, },
		{ .key = { .state = 2, .symbol = 'a' }, .value = 2, },
		{ .key = { .state = 2, .symbol = 'b' }, .value = 2, },
	}));
	hashset_multi_add(&dfa->final, 2, ((int[]){0, 1}));
	return dfa;
}
int dfa_run(DFA* dfa, String* str) {
	StateSymbol curr = {
		.state = dfa->initial_state,
		.symbol = 0,
	};
	for (uint32_t i = 0; i < str->len; ++i) {
		curr.symbol = str->buffer[i];
		VoidPtrOption ptr = hashmap_get(&dfa->transition, curr);
		curr.state = *(int32_t*)UNWRAP(ptr);
	}
	return hashset_contains(&dfa->final, curr.state);
}
void dfa_free(DFA* dfa) {
	hashmap_free(&dfa->transition);
	hashset_free(&dfa->symbols);
	hashset_free(&dfa->states);
	hashset_free(&dfa->final);
	free(dfa);
}
void dfa_print(DFA* dfa) {
	printf("DFA:\n");
	printf("\tQ = { ");
	hashset_foreach(s, &dfa->states) {
		printf("%d, ", *s);
	}
	printf("}\n\tSigma = { ");
	hashset_foreach(s, &dfa->symbols) {
		printf("%c, ", *s);
	}
	printf("}\n\tdelta = { ");
	hashmap_foreach(s, &dfa->transition) {
		printf("(%d, %c) : %d, ", s->key.state, s->key.symbol, s->value);
	}
	printf("}\n\tq0 = 0\n\tF = { ");
	hashset_foreach(s, &dfa->final) {
		printf("%d, ", *s);
	}
	printf("}\n");
}

typedef struct StateSetKV {
	StateSymbol key;
	Int32Array value;
} StateSetKV;
typedef struct StateSetMap {
	uint64_t* bitmap;

	StateSetKV* buffer;
	uint32_t capacity;
	uint32_t len;

	uint64_t (*hash)(StateSymbol);
	int (*cmp)(StateSymbol, StateSymbol);
} StateSetMap;

typedef struct Int32SetStack {
	Int32Set* buffer;
	uint32_t capacity;
	uint32_t len;
} Int32SetStack;

typedef struct SetIntKV {
	Int32Set key;
	int32_t value;
} SetIntKV;
int hashset_cmp(Int32Set set1, Int32Set set2) {
	if (set1.len != set2.len) return 0;
	uint32_t match = 0;
	hashset_foreach(intiger, &set1) {
		if (hashset_contains(&set2, *intiger)) {
			++match;
		}
	}
	return match == set2.len;
}
uint64_t hash_hashset(Int32Set set) {
	uint32_t counter = 0;
	uint64_t hash = 0;
	hashset_foreach(v, &set){
		switch(counter) {
			case 0:
				hash ^= *v * 0x876708901a96346ULL;
				++counter;
				break;
			case 1:
				hash ^= (*v * 0x665236c7a35fcb6ULL) << 17;
				++counter;
				break;
			case 2:
				hash ^= (*v * 0x88bd37e86131fe87ULL) >> 12;
				++counter;
				break;
			case 3:
				hash ^= (*v * 0x500974cc0335e1f8ULL) << 33;
				++counter;
				break;
			default: break;
		}
		if (counter >= 4) break;
	}

	return hash_uint64(hash ^ ((uint64_t)set.len * 0xfab34782bcULL) ^ ((uint64_t)set.capacity * 0x5430923fbda245cULL));
}
typedef struct SetIntMap {
	uint64_t* bitmap;

	SetIntKV* buffer;
	uint32_t capacity;
	uint32_t len;
	
	uint64_t (*hash)(Int32Set);
	int (*cmp)(Int32Set, Int32Set);
} SetIntMap;
typedef struct TripleTuple {
	Int32Set val1;
	char val2;
	Int32Set val3;
} TripleTuple;
typedef struct SetIntSetArray {
	TripleTuple* buffer;
	uint32_t capacity;
	uint32_t len;
} SetIntSetArray;

struct __NFA {
	Int32Set states;
	CharSet symbols;
	StateSetMap transition;
	Int32Set initial_state;
	Int32Set final;
};

NFA* nfa_create() {
	NFA* nfa = OPTION_CALLOC(1, sizeof(NFA));
	nfa->states.hash = hash_uint64;
	nfa->symbols.hash = hash_uint64;
	nfa->transition.hash = state_hash;
	nfa->transition.cmp = state_cmp;
	nfa->final.hash = hash_uint64;
	nfa->initial_state.hash = hash_uint64;

	return nfa;
}
int nfa_run(NFA *nfa, String *str) {
	StateSymbol curr = { 0 };
	Int32Set working_state = { 0 };
	working_state.hash = hash_uint64;
	hashset_foreach(i, &nfa->initial_state) {
		hashset_add(&working_state, *i);
	}
	Int32Array help = { 0 };

	int ret = 0;
	for (uint32_t i = 0; i < str->len; ++i) {
		if (!working_state.len) {
			ret = 0;
			break;
		}
		curr.symbol = str->buffer[i];
		hashset_foreach(s, &working_state) {
			curr.state = *s;
			VoidPtrOption ptr = hashmap_get(&nfa->transition, curr);
			option_if(ptr) {
				Int32Array* arr = (Int32Array*)UNWRAP(ptr);
				array_multi_append(&help, arr->buffer, arr->len);
			}
			hashset_remove(&working_state, *s);
		}
		array_foreach(to_add, &help) {
			hashset_add(&working_state, *to_add);
		}
		help.len = 0;
	}
	if (working_state.len) {
		hashset_foreach(finals, &working_state) {
			if (hashset_contains(&nfa->final, *finals)) {
				ret = 1;
				break;
			}
		}
	}
	
	hashset_free(&working_state);
	array_free(&help);
	return ret;
}
void nfa_free(NFA* nfa) {
	hashmap_foreach(s, &nfa->transition) {
		array_free(&s->value);
	}
	hashmap_free(&nfa->transition);
	hashset_free(&nfa->symbols);
	hashset_free(&nfa->states);
	hashset_free(&nfa->final);
	free(nfa);
} 
void nfa_print(NFA* nfa) {
	printf("NFA:\n");
	printf("\tQ = { ");
	hashset_foreach(s, &nfa->states) {
		printf("%d, ", *s);
	}
	printf("}\n\tSigma = { ");
	hashset_foreach(s, &nfa->symbols) {
		printf("%c, ", *s);
	}
	printf("}\n\tdelta = { ");
	hashmap_foreach(s, &nfa->transition) {
		printf("(%d, %c) : { ", s->key.state, s->key.symbol);
		array_foreach(a, &s->value) {
			printf("%d, ", *a);
		}
		printf("), ");
	}
	printf("}\n\tq0 = { ");
	hashset_foreach(s, &nfa->initial_state) {
		printf("%d, ", *s);
	}
	printf("}\n\tF = { ");
	hashset_foreach(s, &nfa->final) {
		printf("%d, ", *s);
	}
	printf("}\n");
}
DFA* nfa_to_dfa(NFA* nfa) {
	DFA* dfa = OPTION_CALLOC(1, sizeof(DFA));
	dfa->states.hash = hash_uint64;
	dfa->symbols.hash = hash_uint64;
	hashset_foreach(sym, &nfa->symbols) {
		hashset_add(&dfa->symbols, *sym);
	}
	dfa->transition.hash = state_hash;
	dfa->transition.cmp = state_cmp;
	dfa->final.hash = hash_uint64;
	dfa->initial_state = 0;
	
	SetIntMap set_to_int = { 0 };
	set_to_int.hash = hash_hashset;
	set_to_int.cmp = hashset_cmp;
	Int32SetStack stack = { 0 };
	
	Int32Set init_copy = { 0 };
	init_copy.hash = hash_uint64;
	hashset_multi_add(&init_copy, nfa->initial_state.len, nfa->initial_state.buffer);
	stack_push(&stack, init_copy);
	hashmap_add(&set_to_int, init_copy, 0);

	uint32_t curr_state = 1;

	SetIntSetArray temp = { 0 };
	while(stack.len) {
		Int32Set curr_set = stack_pop(&stack);

		hashset_foreach(sym, &nfa->symbols) {
			Int32Set fill_set = { 0 };
			fill_set.hash = hash_uint64;
			hashset_foreach(st, &curr_set) {
				VoidPtrOption ptr = hashmap_get(&nfa->transition, ((StateSymbol){.state = *st, .symbol = *sym}));
				option_if(ptr) {
					Int32Array* arr = (Int32Array*)UNWRAP(ptr);
					hashset_multi_add(&fill_set, arr->len, arr->buffer);
				}
			}
			array_append(&temp, ((TripleTuple){ .val1 = curr_set, .val2 = *sym, .val3 = fill_set }));
			VoidPtrOption ptr = hashmap_get(&set_to_int, fill_set);
			if (ptr.empty) {
				hashmap_add(&set_to_int, fill_set, curr_state);
				stack_push(&stack, fill_set);
				++curr_state;
			}
		}
	}
	array_foreach(vals, &temp) {
		VoidPtrOption ptr1 = hashmap_get(&set_to_int, vals->val1);
		option_if(ptr1) {
			int32_t s1 = *(int32_t*)UNWRAP(ptr1);
			StateSymbol sym = { .state = s1, .symbol = vals->val2};
			VoidPtrOption ptr2 = hashmap_get(&set_to_int, vals->val3);
			option_if(ptr2) {
				int32_t s2 = *(int32_t*)UNWRAP(ptr2);
		 		hashmap_add(&dfa->transition, sym, s2);
			}
		}
	}
	hashmap_foreach(kv, &set_to_int) {
		hashset_add(&dfa->states, kv->value);
		hashset_foreach(s1, &kv->key) {
			hashset_foreach(s2, &nfa->final) {
				if (*s1 == *s2) {
					hashset_add(&dfa->final, kv->value);
				}
			}
		}
	}
	hashset_free(&init_copy);
	array_foreach(vals, &temp) {
		hashset_free(&vals->val3);
	}
	array_free(&temp);
	hashmap_free(&set_to_int);
	stack_free(&stack);
	return dfa;
}

typedef struct IntSetKV {
	int32_t key;
	Int32Set value;
} IntSetKV;
typedef struct IntSetMap {
	uint64_t* bitmap;

	IntSetKV* buffer;
	uint32_t capacity;
	uint32_t len;

	uint64_t (*hash)(uint64_t);
	int (*cmp)(int32_t, int32_t);
} IntSetMap;
struct __EpsilonNFA {
	NFA nfa;
	IntSetMap epsilon_closure;
};

EpsilonNFA* epsilon_create() {
	EpsilonNFA* ep = OPTION_CALLOC(1, sizeof(EpsilonNFA));
	ep->nfa.states.hash = hash_uint64;
	ep->nfa.symbols.hash = hash_uint64;
	ep->nfa.transition.hash = state_hash;
	ep->nfa.transition.cmp = state_cmp;
	ep->nfa.initial_state.hash = hash_uint64;
	ep->nfa.final.hash = hash_uint64;
	ep->epsilon_closure.hash = hash_uint64;

	hashset_multi_add(&ep->nfa.states, 4, ((int[]){ 0, 1, 2, 3 }));
	hashset_multi_add(&ep->nfa.symbols, 2, ((char[]){ '0', '1' }));
	Int32Array arr0 = { 0 };
	array_append(&arr0, (int)1);
	Int32Array arr1 = { 0 };
	array_append(&arr1, (int)1);
	Int32Array arr2 = { 0 };
	array_append(&arr2, (int)2);
	Int32Array arr3 = { 0 };
	array_append(&arr3, (int)3);
	hashmap_multi_add(&ep->nfa.transition, 4, ((StateSetKV[]){
		{ .key = { .symbol = '0', .state = 0 }, .value = arr0 },
		{ .key = { .symbol = '1', .state = 1 }, .value = arr1 },
		{ .key = { .symbol = '0', .state = 2 }, .value = arr2 },
		{ .key = { .symbol = '1', .state = 2 }, .value = arr3 },
	}));
	hashset_add(&ep->nfa.initial_state, (int)0);
	hashset_add(&ep->nfa.final, (int)3);

	Int32Set set0 = { 0 };
	set0.hash = hash_uint64;
	hashset_add(&set0, (int)0);
	hashmap_add(&ep->epsilon_closure, 0, set0);
	Int32Set set1 = { 0 };
	set1.hash = hash_uint64;
	hashset_multi_add(&set1, 2, ((int[]){ 1, 2 }));
	hashmap_add(&ep->epsilon_closure, 1, set1);
	Int32Set set2 = { 0 };
	set2.hash = hash_uint64;
	hashset_add(&set2, (int)2);
	hashmap_add(&ep->epsilon_closure, 2, set2);
	Int32Set set3 = { 0 };
	set3.hash = hash_uint64;
	hashset_add(&set3, (int)3);
	hashmap_add(&ep->epsilon_closure, 3, set3);
	
	return ep;
}
void epsilon_print(EpsilonNFA* epsilon) {
	printf("EpsilonNFA:\n");
	printf("\tQ = { ");
	hashset_foreach(s, &epsilon->nfa.states) {
		printf("%d, ", *s);
	}
	printf("}\n\tSigma = { ");
	hashset_foreach(s, &epsilon->nfa.symbols) {
		printf("%c, ", *s);
	}
	printf("}\n\tdelta = { ");
	hashmap_foreach(s, &epsilon->nfa.transition) {
		printf("(%d, %c) : { ", s->key.state, s->key.symbol);
		array_foreach(a, &s->value) {
			printf("%d, ", *a);
		}
		printf("), ");
	}
	printf("}\n\tepsilon = { ");
	hashmap_foreach(s, &epsilon->epsilon_closure) {
		printf("%d : { ", s->key);
		hashset_foreach(a, &s->value) {
			printf("%d, ", *a);
		}
		printf("}, ");
	}
	printf("}\n\tq0 = { ");
	hashset_foreach(s, &epsilon->nfa.initial_state) {
		printf("%d, ", *s);
	}
	printf("}\n\tF = { ");
	hashset_foreach(s, &epsilon->nfa.final) {
		printf("%d, ", *s);
	}
	printf("}\n");
}
NFA* epsilon_to_nfa(EpsilonNFA* epsilon) {
	NFA* nfa = nfa_create();
	
	hashset_foreach(n, &epsilon->nfa.initial_state) {
		hashset_add(&nfa->initial_state, *n);
	}
	hashset_foreach(n, &epsilon->nfa.symbols) {
		hashset_add(&nfa->symbols, *n);
	}
	hashset_foreach(n, &epsilon->nfa.final) {
		hashset_add(&nfa->final, *n);
	}

	hashset_foreach(curr, &epsilon->nfa.states) {
		hashset_add(&nfa->states, *curr);
		hashset_foreach(sym, &epsilon->nfa.symbols) {
			Int32Array step1 = { 0 };
			Int32Array step2 = { 0 };
			Int32Array step3 = { 0 };

			VoidPtrOption ptr = hashmap_get(&epsilon->epsilon_closure, *curr);
			option_if(ptr) {
				Int32Set* set = (Int32Set*)UNWRAP(ptr);
				hashset_foreach(next_step, set) {
					// printf("Step1: %d -> %d\n", *curr, *next_step);
					array_append(&step1, *next_step);
				}
			}
			array_foreach(a, &step1) {
				ptr = hashmap_get(&epsilon->nfa.transition, ((StateSymbol){ .symbol = *sym, .state = *a, }));
				option_if(ptr) {
					Int32Array* arr = (Int32Array*)UNWRAP(ptr);
					array_foreach(v, arr) {	
						// printf("Step2: %d\n", *v);
						array_append(&step2, *v);
					}
				}
			}
			array_foreach(a, &step2) {
				ptr = hashmap_get(&epsilon->epsilon_closure, *a);
				option_if(ptr) {
					Int32Set* set = (Int32Set*)UNWRAP(ptr);
					hashset_foreach(next_step, set) {
						if (array_find(&step3, *next_step) == -1) {
							// printf("Step3: %d\n", *next_step);
							array_append(&step3, *next_step);
						}
					}
				}
			}
			if (step3.len) {
				hashmap_add(&nfa->transition, ((StateSymbol){ .symbol = *sym, .state = *curr, }), step3);
			}
			
			array_free(&step2);
			array_free(&step1);
		}
	}
	return nfa;
}
void epsilon_free(EpsilonNFA* epsilon) {
	hashmap_foreach(a, &epsilon->nfa.transition) {
		array_free(&a->value);
	}
	hashset_free(&epsilon->nfa.states);
	hashset_free(&epsilon->nfa.symbols);
	hashmap_free(&epsilon->nfa.transition);
	hashset_free(&epsilon->nfa.initial_state);
	hashset_free(&epsilon->nfa.final);
	free(epsilon);
}



struct __Regex {
	String name;
	DFA* dfa;
};

Regex* regex_init_n(const char* pattern, uint32_t count);
StringSliceOption regex_match(Regex* reg, String* str);
void regex_free(Regex* reg);
