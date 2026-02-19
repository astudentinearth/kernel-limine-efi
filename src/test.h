#pragma once
#include "debug.h"
#include "mem_test.h"
#include "hardware/cpu.h"

static inline void run_tests() {
    #ifdef TEST_MODE
    test_memcpy();
    debug_print_cpu_model();
    debug_put_int(-120);
    debug_puts("\n");
    #endif
}

