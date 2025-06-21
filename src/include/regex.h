#ifndef GENERICS_REGEX_H
#define GENERICS_REGEX_H

#include <stdint.h>
#include "dynamic_string.h"

typedef struct __DFA DFA;

typedef struct __NFA NFA;

typedef struct __EpsilonNFA EpsilonNFA;

typedef struct __Regex Regex;

Regex* regex_init_n(const char* pattern, uint32_t count);
#define regex_init(pattern) regex_init_n(pattern, strlen(pattern))
#define regex_from_string(str) regex_init_n((str)->buffer, (str)->len)
StringSliceOption regex_match(Regex* reg, String* str);
void regex_free(Regex* reg);

#endif // GENERICS_REGEX_H
