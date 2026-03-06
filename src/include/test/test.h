#pragma once
#include "debug.h"
#include "hardware/cpu.h"
#include "assert.h"

void test_string_h();
void test_memcpy();
void test_allocator();

static inline void run_tests() {
    #ifdef TEST_MODE
    test_memcpy();
    test_string_h();
    test_allocator();
    print_test_report();
    #endif
}

