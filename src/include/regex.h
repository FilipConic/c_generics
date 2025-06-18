#ifndef GENERICS_REGEX_H
#define GENERICS_REGEX_H

#include <stdint.h>
#include "dynamic_string.h"

typedef struct __DFA DFA;
DFA* dfa_create();
int dfa_run(DFA* dfa, String* str);
void dfa_free(DFA* dfa);
void dfa_print(DFA* dfa);

typedef struct __NFA NFA;
NFA* nfa_create();
int nfa_run(NFA* nfa, String* str);
void nfa_free(NFA* nfa);
DFA* nfa_to_dfa(NFA* nfa);
void nfa_print(NFA* nfa);

typedef struct __EpsilonNFA EpsilonNFA;
EpsilonNFA* epsilon_create();
void epsilon_print(EpsilonNFA* epsilon);
void epsilon_free(EpsilonNFA* epsilon);
NFA* epsilon_to_nfa(EpsilonNFA* epsilon);

typedef struct __Regex Regex;

Regex* regex_init_n(const char* pattern, uint32_t count);
#define regex_init(pattern) regex_init_n(pattern, strlen(pattern))
#define regex_from_string(str) regex_init_n((str)->buffer, (str)->len)
StringSliceOption regex_match(Regex* reg, String* str);
void regex_free(Regex* reg);

#endif // GENERICS_REGEX_H
