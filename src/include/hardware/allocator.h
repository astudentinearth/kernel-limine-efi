#pragma once
#include <stddef.h>
#include "stdint.h"

#define MAX_PAGE_COUNT 32768
#define PAGE_SIZE 4096
typedef u64 pageframe_t;

pageframe_t kalloc_frame();
void kfree_frame(pageframe_t frame);

void init_pmm();

void* malloc(usize size);
void free(void* ptr);

typedef struct {
    u64 mapped_count;
    u64 total_count;
    u64 page_size;
} PMMStats_t;

void get_pmm_stats(PMMStats_t* stats);
u64 get_total_iter();
