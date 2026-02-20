#pragma once
#include <stdbool.h>

bool assert_equals_str(const char* expected, const char* actual, const char* label);
bool assert(bool actual, const char* label);


