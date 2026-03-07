#pragma once
#include "debug.h"
#include "hardware/cpu.h"
#include "assert.h"

void test_string_h();
void test_memcpy();
void test_allocator();
extern uint64_t get_cr3();
extern uint32_t get_cr4();
void test_paging();

static inline void run_tests() {
    #ifdef TEST_MODE
    test_memcpy();
    test_string_h();
    test_allocator();
    test_paging();
    print_test_report();
    debug_printf("[DEBUG] CR3=%p | CR4=%p\n", get_cr3(), get_cr4());
    #endif
}

