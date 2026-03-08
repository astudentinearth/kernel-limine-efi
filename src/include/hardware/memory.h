#pragma once
#include "limine.h"
#include <stdint.h>
uint64_t get_usable_memory();
void init_memory_map();

void *get_largest_usable_memory_block();
uint64_t get_largest_usable_memory_block_size();
struct limine_memmap_entry *get_framebuffer(int i);
struct limine_memmap_entry **get_memmap_entries();
uint64_t get_memmap_entry_count();
#ifdef TEST_MODE
void dump_memory_info();
#endif
