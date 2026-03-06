#pragma once
#include "debug.h"
#include "hardware/cpu.h"
#include "assert.h"

void test_string_h();
void test_memcpy();
void test_allocator();
extern void out_cr3();
extern void out_cr4();

static inline void run_tests() {
    #ifdef TEST_MODE
    test_memcpy();
    test_string_h();
    test_allocator();
    print_test_report();
    out_cr3();
    out_cr4();
    #endif
}

