#pragma once
#include "mem.h"

static inline void run_tests() {
    #ifdef TEST_MODE
    test_memcpy();
    #endif
}

