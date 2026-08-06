#pragma once
#include <stddef.h>
#include "stdint.h"

#define MAX_PAGE_COUNT 32768
#define PAGE_SIZE 4096
typedef u64 pageframe_t;

pageframe_t kalloc_frame();
void kfree_frame(pageframe_t);

void init_pmm();

void* malloc(usize size);
void free(void* ptr);
