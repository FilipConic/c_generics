#include "dynamic_string.h"

#include "option.h"
#include "utility.h"
#include "hashfuncs.h"
#include <stdio.h>

void string_reserve(String* str, uint32_t size) {
	if (str->buffer) {
		char* buf = calloc(size, sizeof(char));
		if (!buf) {
			fprintf(stderr, ANSI_RED "ERROR: " ANSI_RESET "Not enought RAM to allocate a string!\n");
			exit(1);
		}
		int32_t smin = (size < str->len ? size : str->len);
		str->capacity = size;
		str->__token = -1;
		str->len = 0;
		for (int32_t i = 0; i < smin - 1; ++i) {
			buf[i] = str->buffer[str->len++];
		}
		buf[str->len] = '\0';
		free(str->buffer);
		str->buffer = buf;
	} else {
		if (!size) {
			str->buffer = calloc(DYN_STRING_BASE_SIZE, sizeof(char));
			str->capacity = DYN_STRING_BASE_SIZE;
		} else {
			str->buffer = calloc(size, sizeof(char));
			str->capacity = size;
		}
		if (!str->buffer) {
			fprintf(stderr, ANSI_RED "ERROR: " ANSI_RESET "Not enought RAM to allocate a string!\n");
			exit(1);
		}
		str->__token = -1;
		str->len = 0;
	}
}

void string_append(String* str, char c) {
	if (str->len == str->capacity + 1) {
		string_reserve(str, str->capacity << 1);
	}
	str->buffer[str->len++] = c;
	str->buffer[str->len] = '\0';
}
void string_append_c_str_n(String* str, const char* c_str, uint32_t len) {
	uint32_t to_reserve = (str->capacity ? str->capacity : DYN_STRING_BASE_SIZE);
	while (str->len + len >= to_reserve + 1) {
		to_reserve <<= 1;
	}
	string_reserve(str, to_reserve);
	for (uint32_t i = 0; i < len; ++i) {
		str->buffer[str->len++] = c_str[i];
	}
	str->buffer[str->len] = '\0';
}

void string_append_int(String* str, int i);
void string_append_double_prec(String* str, double x, uint8_t prec);
void string_printf(String* str, const char* format, ...);
void string_append_file(String *str, const char *file_path) {
	FILE* file = fopen(file_path, "r");
	if (!file) {
		fprintf(stderr, ANSI_RED "ERROR: " ANSI_RESET "Couldn\'t open file!\n");
		return;
	}
	uint32_t start = ftell(file);
	fseek(file, 0, SEEK_END);
	uint32_t len = ftell(file) - start;
	fseek(file, 0, SEEK_SET);
	uint32_t to_reserve = (str->capacity ? str->capacity : DYN_STRING_BASE_SIZE);
	while (str->len + len >= to_reserve + 1) {
		to_reserve <<= 1;
	}
	string_reserve(str, to_reserve);
	char c = 0;
	for (uint32_t i = 0; i < len; ++i) {
		fread(&c, 1, sizeof(char), file);
		str->buffer[str->len++] = c;
	}
	str->buffer[str->len] = '\0';

	fclose(file);
}

void string_copy(String* dst, const String* src) {
	string_reserve(dst, src->capacity);
	dst->len = 0;
	string_append_c_str_n(dst, src->buffer, src->len);
}
int string_cmp(String str1, String str2) {
	if (str1.len != str2.len) {
		return 0;
	}
	for (uint32_t i = 0; i < str1.len; ++i) {
		if (str1.buffer[i] != str2.buffer[i]) {
			return 0;
		}
	}
	return 1;
}
void string_remove(String* str, char c) {
	for (uint32_t i = 0; i < str->len; ++i) {
		if (c == str->buffer[i]) {
			for (uint32_t j = i; j < str->len; ++j) {
				str->buffer[j] = str->buffer[j + 1];
			}
			--i;
			--str->len;
		}
	}
	str->buffer[str->len] = '\0';
}

StringSliceOption string_parse_by(String* str, char c) {
	if (str->__token == -2) {
		str->__token = -1;
		return string_get_slice(str, 0, 0);
	}
	uint32_t start = ++str->__token;
	for (; str->__token < (int32_t)str->len; ++str->__token) {
		if (str->buffer[str->__token] == c) {
			break;
		}
	}
	StringSliceOption ret = string_get_slice(str, start, str->__token - start);
	if (str->__token == (int32_t)str->len) {
		str->__token = -2;
	}
	return ret;
}

uint64_t hash_string(String str) {
	return hash_murmur3_string(str.buffer, str.len);
}

void string_free(String* str) {
	if (str->buffer) {
		free(str->buffer);
	}
	str->buffer = NULL;
	str->len = 0;
	str->capacity = 0;
	str->__token = -1;
}


