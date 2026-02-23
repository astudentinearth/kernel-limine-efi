#pragma once
#include <stdint.h>
uint64_t get_usable_memory();
void init_memory_map();

void* get_largest_usable_memory_block();
uint64_t get_largest_usable_memory_block_size();
#ifdef TEST_MODE
void dump_memory_info();
#endif

