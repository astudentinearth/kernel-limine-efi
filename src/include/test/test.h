#pragma once
#include "debug.h"
#include "hardware/cpu.h"
#include "assert.h"

void test_string_h();
void test_memcpy();
void test_allocator();
extern u64 get_cr3();
extern u32 get_cr4();
void test_paging();

void run_tests();

