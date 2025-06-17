#ifndef GENERICS_REGEX_H
#define GENERICS_REGEX_H

#include <stdint.h>
#include "dynamic_string.h"

typedef struct __DFA DFA;
struct __DFA* dfa_create();
int dfa_run(struct __DFA* dfa, String* str);
void dfa_free(struct __DFA* dfa);
typedef struct __Regex Regex;

Regex regex_init(const char* pattern);
StringSlice regex_match(Regex* reg, String* str);
void regex_free(Regex* reg);

#endif // GENERICS_REGEX_H
