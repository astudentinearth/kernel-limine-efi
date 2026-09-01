#pragma once
#include "stdint.h"
#include <stddef.h>

#define MAX_PAGE_COUNT 32768
#define PAGE_SIZE 4096
typedef u64 pageframe_t;

pageframe_t kalloc_frame();
void *kalloc_vframe();
void kfree_frame(pageframe_t frame);
void kfree_vframe(void *vframe);

void init_pmm();

void *malloc(usize size);
void free(void *ptr);

typedef struct {
    u64 mapped_count;
    u64 total_count;
    u64 page_size;
} PMMStats_t;

void get_pmm_stats(PMMStats_t *stats);
void debug_print_vm_stats();
