#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool _assert_equals_str(const char* file, int line, const char* expected, const char* actual, const char* label);
bool _assert(const char* file, int line, bool actual, const char* msg);
bool _assert_equals_uint(const char* file, int line, uint64_t expected, uint64_t actual, const char* label);
bool _assert_equals_ptr(const char* file, int line, void* expected, void* actual, const char* label);
void _test_pass(const char* file, int line, const char* msg);

bool _describe(const char* file, int line, const char* name, ...);
void print_test_report();

#define test_pass(label) _test_pass ( __FILE__, __LINE__, label )
#define assert(actual, msg) _assert(__FILE__, __LINE__, actual, msg)
#define assert_equals_str(expected, actual, msg) _assert_equals_str(__FILE__, __LINE__, expected, actual, msg)
#define assert_equals_uint(expected, actual, msg) _assert_equals_uint(__FILE__, __LINE__, expected, actual, msg)
#define assert_equals_ptr(expected, actual, msg) _assert_equals_ptr(__FILE__, __LINE__, expected, actual, msg)
#define END_DESCRIBE 0xABCD
#define describe(name, ...) _describe(__FILE__, __LINE__, name, __VA_ARGS__, END_DESCRIBE)

