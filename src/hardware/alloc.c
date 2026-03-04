
#include "debug.h"
#include "hardware/allocator.h"
#include "hardware/memory.h"
#include <stdbool.h>

#define USED true
#define FREE false

static bool frame_map[MAX_PAGE_COUNT];
static uint64_t pool_ptr;
static uint64_t max_available_pages;
static uint64_t total_allocated = 0;
static uint64_t total = 0;

pageframe_t kalloc_frame(){
    uint64_t i = 0;
    while(frame_map[i] != FREE) {
        i++;
        if(i > max_available_pages) {
            panic("Out of memory");
        }
    }
    frame_map[i] = USED;
    total_allocated += PAGE_SIZE;
    return pool_ptr + (i * PAGE_SIZE);
}

uint64_t get_frame_idx(pageframe_t pframe) {
    return (pframe - pool_ptr) / PAGE_SIZE;
}

void kfree_frame(pageframe_t pframe) {
    uint64_t index = get_frame_idx(pframe);
    frame_map[index] = FREE;
}

void init_pmm() {
    void* pool_start = get_largest_usable_memory_block();
    uint64_t size = get_largest_usable_memory_block_size();
    max_available_pages = size / PAGE_SIZE;
    if(max_available_pages > MAX_PAGE_COUNT) max_available_pages = MAX_PAGE_COUNT;
    pool_ptr = (uint64_t)pool_start;
    total = max_available_pages * PAGE_SIZE;
}

#ifdef TEST_MODE
#include "test/assert.h"
void test_allocator() {
    debug("[TEST] page frame allocator test begin");
    bool pass = true;
    pageframe_t p1, p2, p3, p4;
    p1 = kalloc_frame();
    p2 = kalloc_frame();
    p3 = kalloc_frame();
    p4 = kalloc_frame();
    debug_printf("Allocated frames: %X %X %X %X\n", p1, p2, p3, p4);
    uint64_t i1, i2, i3, i4;
    i1 = get_frame_idx(p1);
    i2 = get_frame_idx(p2);
    i3 = get_frame_idx(p3);
    i4 = get_frame_idx(p4);

    pass &= assert(frame_map[i1] == USED, "frame 1 used");
    pass &= assert(frame_map[i2] == USED, "frame 2 used");
    pass &=assert(frame_map[i3] == USED, "frame 3 used");
    pass &=assert(frame_map[i4] == USED, "frame 4 used");

    debug("Freeing frames");
    kfree_frame(p1);
    kfree_frame(p2);
    kfree_frame(p3);
    kfree_frame(p4);

    pass &= assert(frame_map[i1] == FREE, "frame 1 free");
    pass &= assert(frame_map[i2] == FREE, "frame 2 free");
    pass &= assert(frame_map[i3] == FREE, "frame 3 free");
    pass &= assert(frame_map[i4] == FREE, "frame 4 free");
    debug_printf("[TEST] page frame allocator test %s\n", pass ? "pass" : "fail");
}
#endif

