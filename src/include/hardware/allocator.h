#pragma once
#include <stdint.h>

#define MAX_PAGE_COUNT 32768
#define PAGE_SIZE 4096
typedef uint64_t pageframe_t;

pageframe_t kalloc_frame();
void kfree_frame(pageframe_t);

void init_pmm();

#ifdef TEST_MODE
void run_out_of_memory();
#endif

