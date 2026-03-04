#pragma once
#include <stdbool.h>
#include <stdint.h>

bool assert_equals_str(const char* expected, const char* actual, const char* label);
bool assert(bool actual, const char* label);
bool assert_equals_uint(uint64_t expected, uint64_t actual, const char* label);


