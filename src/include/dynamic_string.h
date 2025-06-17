#ifndef GENERICS_DYN_STRING_H
#define GENERICS_DYN_STRING_H

#include <stdint.h>
#include <string.h>

#ifndef DYN_STRING_BASE_SIZE
#define DYN_STRING_BASE_SIZE 256
#endif // DYN_STRING_BASE_SIZE

typedef struct {
	char* buffer;
	uint32_t capacity;
	uint32_t len;
	int32_t __token;
} String;

typedef struct {
	char const* buffer;
	uint32_t len;
} StringSlice;

void string_reserve(String* str, uint32_t size);

void string_append(String* str, char c);
void string_append_c_str_n(String* str, const char* c_str, uint32_t len);
#define string_append_c_str(str, c_str) string_append_c_str_n(str, c_str, strlen(c_str))
#define string_append_str(str1, str2) string_append_c_str_n(str1, (str2)->buffer, (str2)->len)
#define string_append_slice(str, slice) string_append_c_str_n(str, (slice)->buffer, (slice)->len)
void string_append_file(String* str, const char* file_path);

void string_append_int(String* str, int i);
void string_append_double_prec(String* str, double x, uint8_t prec);
#define string_append_double(str, x) string_append_double_prec(str, x, 5);
void string_printf(String* str, const char* format, ...);

void string_copy(String* dst, const String* src);
int string_cmp(String str1, String str2);
void string_remove(String* str, char c);

#define string_get_slice(str, beginning, length) ((StringSlice){ .buffer = (str)->buffer + beginning, .len = length })
StringSlice string_parse_by(String* str, char c);
#define string_parse(slice, str, c) (str)->__token = -1; \
	for (StringSlice slice = string_parse_by(str, c); (str)->__token != -1; slice = string_parse_by(str, c))
	

uint64_t hash_string(String str);

void string_free(String* str);

#endif // GENERICS_DYN_STRING_H
