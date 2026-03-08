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

void run_tests();

