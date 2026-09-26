#pragma once
#include "stdint.h"

inline int64_t abs(int64_t val) { return val > 0 ? val : -val; }

inline u64 upow(u64 val, unsigned exp) {
    u64 result = 1;
    for(unsigned i = 0; i < exp; i++) result *= val;
    return result;
}

#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b
#define is_digit(ch) (ch >= '0' && ch <= '9')
