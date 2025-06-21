#include "regex.h"

#include "array.h"
#include "queue.h"
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

#define INVALID_CHAR 0x80
DFA dfa_create() {
	DFA dfa = { 0 };
	dfa.states.hash = hash_uint64;
	dfa.transition.hash = state_hash;
	dfa.transition.cmp = state_cmp;
	dfa.symbols.hash = hash_uint64;
	dfa.final.hash = hash_uint64;
	return dfa;
}
int dfa_run(DFA* dfa, String* str) {
	StateSymbol curr = {
		.state = dfa->initial_state,
		.symbol = 0,
	};
	for (uint32_t i = 0; i < str->len; ++i) {
		curr.symbol = str->buffer[i];
		if (!hashset_contains(&dfa->symbols, curr.symbol)) {
			curr.symbol = INVALID_CHAR;
		}
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
}
void dfa_print(DFA* dfa) {
	printf("DFA:\n");
	printf("\tQ = { ");
	hashset_foreach(s, &dfa->states) {
		printf("%d, ", *s);
	}
	printf("},\n\tSigma = { ");
	hashset_foreach(s, &dfa->symbols) {
		printf("\'%c\', ", *s);
	}
	printf("},\n\tdelta = { ");
	hashmap_foreach(s, &dfa->transition) {
		printf("(%d, \'%c\') : %d, ", s->key.state, s->key.symbol, s->value);
	}
	printf("},\n\tq0 = 0,\n\tF = { ");
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

NFA nfa_create() {
	NFA nfa = { 0 };
	nfa.states.hash = hash_uint64;
	nfa.symbols.hash = hash_uint64;
	nfa.transition.hash = state_hash;
	nfa.transition.cmp = state_cmp;
	nfa.final.hash = hash_uint64;
	nfa.initial_state.hash = hash_uint64;

	return nfa;
}
void nfa_free(NFA* nfa) {
	hashmap_foreach(s, &nfa->transition) {
		array_free(&s->value);
	}
	hashmap_free(&nfa->transition);
	hashset_free(&nfa->symbols);
	hashset_free(&nfa->states);
	hashset_free(&nfa->final);
} 
void nfa_print(NFA* nfa) {
	printf("NFA:\n");
	printf("\tQ = { ");
	hashset_foreach(s, &nfa->states) {
		printf("%d, ", *s);
	}
	printf("}\n\tSigma = { ");
	hashset_foreach(s, &nfa->symbols) {
		printf("\'%c\', ", *s);
	}
	printf("}\n\tdelta = { ");
	hashmap_foreach(s, &nfa->transition) {
		printf("(%d, \'%c\') : { ", s->key.state, s->key.symbol);
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
DFA nfa_to_dfa(NFA* nfa) {
	DFA dfa = dfa_create();
	hashset_foreach(sym, &nfa->symbols) {
		hashset_add(&dfa.symbols, *sym);
	}
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
		 		hashmap_add(&dfa.transition, sym, s2);
			}
		}
	}
	hashmap_foreach(kv, &set_to_int) {
		hashset_add(&dfa.states, kv->value);
		hashset_foreach(s1, &kv->key) {
			hashset_foreach(s2, &nfa->final) {
				if (*s1 == *s2) {
					hashset_add(&dfa.final, kv->value);
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
	char is_or;
};

EpsilonNFA epsilon_create() {
	EpsilonNFA ep = { 0 };
	ep.nfa = nfa_create();
	ep.epsilon_closure.hash = hash_uint64;

	hashset_add(&ep.nfa.states, (int)0);
	hashset_add(&ep.nfa.initial_state, (int)0);
	hashset_add(&ep.nfa.final, (int)0);

	// hashset_add(&ep.nfa.symbols, (char)INVALID_CHAR);

	Int32Set set = { 0 };
	set.hash = hash_uint64;
	hashset_add(&set, (int)0);
	hashmap_add(&ep.epsilon_closure, (int)0, set);

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
		printf("\'%c\', ", *s);
	}
	printf("}\n\tdelta = { ");
	hashmap_foreach(s, &epsilon->nfa.transition) {
		printf("(%d, \'%c\') : { ", s->key.state, s->key.symbol);
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

void epsilon_reachable_from(EpsilonNFA* epsilon, int32_t state, Int32Set* set) {
	hashset_add(set, state);

	Int32Set visited = { 0 };
	visited.hash = hash_uint64;
	Int32Stack stack = { 0 };
	stack_push(&stack, state);
	
	while (stack.len) {
		int32_t curr = stack_pop(&stack);
		hashset_add(&visited, curr);
		VoidPtrOption ptr = hashmap_get(&epsilon->epsilon_closure, curr);
		option_if(ptr) {
			Int32Set* s = (Int32Set*)UNWRAP(ptr);
			hashset_foreach(v, s) {
				hashset_add(set, *v);
			}
		}
		hashset_foreach(v, set) {	
			if (!hashset_contains(&visited, *v)) {
				stack_push(&stack, *v);
			}
		}
	}
	hashset_free(&visited);
	stack_free(&stack);
}
Int32Set epsilon_reachable_from_states(EpsilonNFA* epsilon, Int32Set* set) {
	Int32Set res = { 0 };
	res.hash = hash_uint64;

	hashset_foreach(s, set) {
		epsilon_reachable_from(epsilon, *s, &res);
	}
	
	hashset_free(set);
	return res;
}

NFA epsilon_to_nfa(EpsilonNFA* epsilon) {
	NFA nfa = nfa_create();
	
	hashset_foreach(n, &epsilon->nfa.initial_state) {
		hashset_add(&nfa.initial_state, *n);
	}
	hashset_foreach(n, &epsilon->nfa.symbols) {
		hashset_add(&nfa.symbols, *n);
	}
	hashset_foreach(n, &epsilon->nfa.final) {
		hashset_add(&nfa.final, *n);
	}

	hashset_foreach(curr, &epsilon->nfa.states) {
		hashset_add(&nfa.states, *curr);

		Int32Set step1 = { 0 };
		step1.hash = hash_uint64;
		Int32Set step2 = { 0 };
		step2.hash = hash_uint64;
		Int32Set step3 = { 0 };
		step3.hash = hash_uint64;
		hashset_foreach(sym, &epsilon->nfa.symbols) {
			epsilon_reachable_from(epsilon, *curr, &step1);
			
			hashset_foreach(a, &step1) {
				VoidPtrOption ptr = hashmap_get(&epsilon->nfa.transition, ((StateSymbol){ .symbol = *sym, .state = *a, }));
				option_if(ptr) {
					Int32Array* arr = (Int32Array*)UNWRAP(ptr);
					array_foreach(v, arr) {	
						hashset_add(&step2, *v);
					}
				}
			}
			step3 = epsilon_reachable_from_states(epsilon, &step2);
			if (step3.len) {
				Int32Array arr = { 0 };
				hashset_foreach(s, &step3) {
					array_append(&arr, *s);
				}
				hashmap_add(&nfa.transition, ((StateSymbol){ .symbol = *sym, .state = *curr, }), arr);
			}
			
			hashset_free(&step3);
			hashset_free(&step2);
			hashset_free(&step1);
		}
	}
	return nfa;
}
void epsilon_free(EpsilonNFA* epsilon) {
	nfa_free(&epsilon->nfa);
	hashmap_free(&epsilon->epsilon_closure);
}

typedef struct EpsilonQueue {
	EpsilonNFA* buffer;
	uint32_t capacity;
	uint32_t len;
	uint32_t tail;
	uint32_t head;
} EpsilonQueue;

void epsilon_or(EpsilonNFA* e1, EpsilonNFA* e2) {
	int32_t e2_init = e1->nfa.states.len;

	int32_t e1_init = 0;
	hashset_foreach(s, &e1->nfa.initial_state) {
		e1_init = *s;
		break;
	}

	hashset_foreach(c, &e2->nfa.symbols) {
		hashset_add(&e1->nfa.symbols, *c);
	}
	hashset_foreach(s, &e2->nfa.states) {
		hashset_add(&e1->nfa.states, (*s + e2_init));
		Int32Set set = { 0 };
		set.hash = hash_uint64;
		VoidPtrOption ptr = hashmap_get(&e2->epsilon_closure, *s);
		option_if(ptr) {
			Int32Set* set_from = (Int32Set*)UNWRAP(ptr);
			hashset_foreach(s2, set_from) {
				hashset_add(&set, (*s2 + e2_init));
			}
		}
		hashmap_add(&e1->epsilon_closure, (*s + e2_init), set);
	}

	hashmap_foreach(kv, &e2->nfa.transition) {
		Int32Array arr = { 0 };
		array_multi_append(&arr, kv->value.buffer, kv->value.len);
		array_foreach(a, &arr) *a += e2_init;
		hashmap_add(&e1->nfa.transition, ((StateSymbol){ .state = kv->key.state + e2_init, .symbol = kv->key.symbol, }), arr);
	}
	VoidPtrOption ptr = hashmap_get(&e1->epsilon_closure, e1_init);
	option_if(ptr) {
		Int32Set* set = (Int32Set*)UNWRAP(ptr);
		hashset_add(set, e2_init);
	}

	int32_t e1_final = 0;
	hashset_foreach(s, &e1->nfa.final) {
		e1_final = *s;
		break;
	}
	int32_t e2_final = 0;
	hashset_foreach(s, &e2->nfa.final) {
		e2_final = *s + e2_init;
		break;
	}

	int32_t new_state = e1->nfa.states.len;
	ptr = hashmap_get(&e1->epsilon_closure, e1_final);
	option_if(ptr) {
		Int32Set* set = (Int32Set*)UNWRAP(ptr);
		hashset_add(set, new_state);
	}
	ptr = hashmap_get(&e1->epsilon_closure, e2_final);
	option_if(ptr) {
		Int32Set* set = (Int32Set*)UNWRAP(ptr);
		hashset_add(set, new_state);
	}
	hashset_add(&e1->nfa.states, new_state);
	Int32Set set = { 0 };
	set.hash = hash_uint64;
	hashset_add(&set, new_state);
	hashmap_add(&e1->epsilon_closure, new_state, set);
	
	hashset_free(&e1->nfa.final);
	hashset_add(&e1->nfa.final, new_state);
}
void epsilon_append(EpsilonNFA* e1, EpsilonNFA* e2) {
	if (e1->is_or) {
		epsilon_or(e1, e2);
		e1->is_or = 0;
		epsilon_free(e2);
		return;
	}

	int32_t e2_init = e1->nfa.states.len;
	int32_t e1_final = 0;
	hashset_foreach(s, &e1->nfa.final) {
		e1_final = *s;
		break;
	}

	hashset_foreach(c, &e2->nfa.symbols) {
		hashset_add(&e1->nfa.symbols, *c);
	}

	hashset_foreach(s, &e2->nfa.states) {
		hashset_add(&e1->nfa.states, (*s + e2_init));
		Int32Set set = { 0 };
		set.hash = hash_uint64;
		VoidPtrOption ptr = hashmap_get(&e2->epsilon_closure, *s);
		option_if(ptr) {
			Int32Set* set_from = (Int32Set*)UNWRAP(ptr);
			hashset_foreach(s2, set_from) {
				hashset_add(&set, (*s2 + e2_init));
			}
		}
		hashmap_add(&e1->epsilon_closure, (*s + e2_init), set);
	}
	
	hashset_free(&e1->nfa.final);
	hashset_foreach(s, &e2->nfa.final) {
		hashset_add(&e1->nfa.final, (*s + e2_init));
	}

	hashmap_foreach(kv, &e2->nfa.transition) {
		Int32Array arr = { 0 };
		array_multi_append(&arr, kv->value.buffer, kv->value.len);
		array_foreach(a, &arr) {
			*a += e2_init;
		}
		hashmap_add(&e1->nfa.transition, ((StateSymbol){ .state = kv->key.state + e2_init, .symbol = kv->key.symbol, }), arr);
	}
	VoidPtrOption ptr = hashmap_get(&e1->epsilon_closure, e1_final);
	option_if(ptr) {
		Int32Set* set = (Int32Set*)UNWRAP(ptr);
		hashset_add(set, e2_init);
		hashmap_add(&e1->epsilon_closure, e1_final, *set);
	}
	
	if (e2->is_or) e1->is_or = 1;
	epsilon_free(e2);
}
EpsilonNFA epsilon_symbol(char c) {
	EpsilonNFA epsilon = epsilon_create();
	hashset_add(&epsilon.nfa.states, (int)1);
	hashset_add(&epsilon.nfa.symbols, c);
	Int32Array arr = { 0 };
	array_append(&arr, (int)1);
	hashmap_add(&epsilon.nfa.transition, ((StateSymbol){ .state = 0, .symbol = c }), arr);
	
	hashset_remove(&epsilon.nfa.final, (int)0);
	hashset_add(&epsilon.nfa.final, (int)1);
	
	Int32Set set = { 0 };
	set.hash = hash_uint64;
	hashset_add(&set, (int)1);
	hashmap_add(&epsilon.epsilon_closure, (int)1, set);

	return epsilon;
}
void epsilon_star(EpsilonNFA* epsilon) {
	int32_t initial_state = 0;
	hashset_foreach(s, &epsilon->nfa.initial_state) {
		initial_state = *s;
		break;
	}
	int32_t final_state = 0;
	hashset_foreach(s, &epsilon->nfa.final) {
		final_state = *s;
		break;
	}
	VoidPtrOption ptr = hashmap_get(&epsilon->epsilon_closure, final_state);
	option_if(ptr) {
		Int32Set* set = (Int32Set*)UNWRAP(ptr);
		hashset_add(set, initial_state);
		hashmap_add(&epsilon->epsilon_closure, final_state, *set);
	}
	ptr = hashmap_get(&epsilon->epsilon_closure, initial_state);
	option_if(ptr) {
		Int32Set* set = (Int32Set*)UNWRAP(ptr);
		hashset_add(set, final_state);
		hashmap_add(&epsilon->epsilon_closure, initial_state, *set);
	}
}
void epsilon_plus(EpsilonNFA* epsilon) {
	int32_t initial_state = 0;
	hashset_foreach(s, &epsilon->nfa.initial_state) {
		initial_state = *s;
		break;
	}
	int32_t final_state = 0;
	hashset_foreach(s, &epsilon->nfa.final) {
		final_state = *s;
		break;
	}
	VoidPtrOption ptr = hashmap_get(&epsilon->epsilon_closure, final_state);
	option_if(ptr) {
		Int32Set* set = (Int32Set*)UNWRAP(ptr);
		hashset_add(set, initial_state);
		hashmap_add(&epsilon->epsilon_closure, final_state, *set);
	}
}

EpsilonNFA epsilon_compress(EpsilonQueue* q) {
	printf("=====-----=====\n");
	EpsilonNFA res = queue_pop(q);
	while (q->len) {
		EpsilonNFA ep = queue_pop(q);
		epsilon_print(&ep);
		epsilon_append(&res, &ep);
	}
	printf("=====-----=====\n");
	return res;
}

struct __Regex {
	String name;
	DFA dfa;
};

Regex* regex_init_n(const char* pattern, uint32_t count) {
	Regex* reg = OPTION_CALLOC(1, sizeof(Regex));
	string_append_c_str_n(&reg->name, pattern, count);
	
	EpsilonQueue eq = { 0 };
	EpsilonQueue prep = { 0 };
	for (uint32_t i = 0; i < count; ++i) {
		switch (pattern[i]) {
			case '(': case ')': {
				EpsilonNFA p = epsilon_compress(&eq);
				queue_push(&prep, p);
				break;
			}
			case '+': {
				epsilon_plus(queue_head(&eq));
				break;
			}
			case '*': {
				epsilon_star(queue_head(&eq));
				break;
			}
			case '|': {
				EpsilonNFA e = epsilon_create();
				e.is_or = 1;
				queue_push(&eq, e);
				break;
			}
			default: {
				EpsilonNFA nfa = epsilon_symbol(pattern[i]);
				queue_push(&eq, nfa);
			}
		}
	}
	if (eq.len) {
		EpsilonNFA p = epsilon_compress(&eq);
		queue_push(&prep, p);
	}

	EpsilonNFA ep = epsilon_compress(&prep);
	epsilon_print(&ep);
	NFA nfa = epsilon_to_nfa(&ep);
	nfa_print(&nfa);
	reg->dfa = nfa_to_dfa(&nfa);
	dfa_print(&reg->dfa);

	nfa_free(&nfa);
	epsilon_free(&ep);

	queue_free(&eq);
	queue_free(&prep);

	return reg;
}
StringSliceOption regex_match(Regex* reg, String* str);
void regex_free(Regex* reg) {
	dfa_free(&reg->dfa);
	string_free(&reg->name);
	free(reg);
}
