#pragma once
#include "debug.h"
#include "mem_test.h"
#include "hardware/cpu.h"
#include "string_test.h"

static inline void run_tests() {
    #ifdef TEST_MODE
    test_memcpy();
    debug_print_cpu_model();
//     debug_put_int(12345);
//     debug_puts("\n");
//     debug_put_int(-120);
//     debug_puts("\n");
//     debug_put_int(-14);
//     debug_puts("\n");
//     debug_put_int(12);
//     debug_puts("\n");
//     debug_put_int(1234);
//     debug_puts("\n");
    test_string_h();
    #endif
}

