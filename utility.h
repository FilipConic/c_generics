#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"

#define ANSI_BOLD "\x1b[1m"
#define ANSI_ITALIC "\x1b[3m"
#define ANSI_UNDERLINE "\x1b[4m"

#define ANSI_RESET "\x1b[0m"

#define __EVAL__(x) x
#define __CONCAT_BASE__(a, b) a##b
#define __CONCAT__(a, b) __CONCAT_BASE__(a, b)

#ifndef BASE_SIZE
#define BASE_SIZE 128
#endif // BASE_SIZE

#endif // UTILITY_H
