#pragma once
#include "debug.h"
#include "mem_test.h"
#include "hardware/cpu.h"

void test_string_h();

static inline void run_tests() {
    #ifdef TEST_MODE
    test_memcpy();
    debug_print_cpu_model();
    test_string_h();
    #endif
}

