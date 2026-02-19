#pragma once
#include "mem.h"
#include "hardware/cpu.h"

static inline void run_tests() {
    #ifdef TEST_MODE
    test_memcpy();
    debug_print_cpu_model();
    #endif
}

