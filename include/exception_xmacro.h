#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define EXCEPTION_TABLE \
    X(EXCEPTION_FILE_NOT_FOUND, 0, "FILE_NOT_FOUND") \
    X(EXCEPTION_FAIL_MEMORY_ALLOCATION, 1, "FAIL_MEMORY_ALLOCATION") \
    X(EXCEPTION_INVALID_INPUT, 2, "INVALID_INPUT") \
    X(EXCEPTION_INVALID_FILE, 3, "INVALID_FILE") 


#define X(name, code, type) name = code,
typedef enum {
    EXCEPTION_TABLE
} Exception_code;
#undef X

extern const char *exception_messages[];

void handle_exception(Exception_code code, const char *file, int line, const char *func, const char *format, ...) ;

#define THROW_EXCEPTION_AND_EXIT(code, ...) \
    handle_exception(code, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif