#pragma once
#include <stdint.h>


inline int64_t abs(int64_t val) {
    return val > 0 ? val : -val;
}
