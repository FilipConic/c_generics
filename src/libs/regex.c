#include "regex.h"

#include "hashset.h"
#include "hashmap.h"
#include "hashfuncs.h"
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

struct __DFA* dfa_create() {
	struct __DFA* dfa = OPTION_CALLOC(1, sizeof(struct __DFA));
	dfa->states.hash = hash_uint64;
	dfa->transition.hash = state_hash;
	dfa->transition.cmp = state_cmp;
	dfa->symbols.hash = hash_uint64;
	dfa->final.hash = hash_uint64;
	hashset_add(&dfa->states, (int)0);
	hashset_add(&dfa->states, (int)1);
	hashset_add(&dfa->states, (int)2);
	hashset_add(&dfa->symbols, (char)'a');
	hashset_add(&dfa->symbols, (char)'b');
	hashmap_add(&dfa->transition, ((StateSymbol){ .state = 0, .symbol = 'a' }), 0);
	hashmap_add(&dfa->transition, ((StateSymbol){ .state = 0, .symbol = 'b' }), 1);
	hashmap_add(&dfa->transition, ((StateSymbol){ .state = 1, .symbol = 'a' }), 2);
	hashmap_add(&dfa->transition, ((StateSymbol){ .state = 1, .symbol = 'b' }), 1);
	hashmap_add(&dfa->transition, ((StateSymbol){ .state = 2, .symbol = 'a' }), 2);
	hashmap_add(&dfa->transition, ((StateSymbol){ .state = 2, .symbol = 'b' }), 2);
	hashset_add(&dfa->final, (int)0);
	hashset_add(&dfa->final, (int)1);
	return dfa;
}
int dfa_run(struct __DFA* dfa, String* str) {
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
void dfa_free(struct __DFA* dfa) {
	hashmap_free(&dfa->transition);
	hashset_free(&dfa->symbols);
	hashset_free(&dfa->states);
	hashset_free(&dfa->final);
	free(dfa);
}

struct __Regex {
	uint32_t buffer;
};

Regex regex_init(const char* pattern);
StringSlice regex_match(Regex* reg, String* str);
void regex_free(Regex* reg);
