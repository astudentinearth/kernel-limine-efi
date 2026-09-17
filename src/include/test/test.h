#pragma once

#ifdef TEST_MODE
#include "stdint.h"
void test_string_h();
void test_memcpy();
void test_allocator();
extern u64 get_cr3();
extern u32 get_cr4();
void test_paging();
void test_malloc();
void run_tests();
void test_vmm();

#endif
