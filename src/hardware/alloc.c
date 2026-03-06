
#include "debug.h"
#include "hardware/allocator.h"
#include "hardware/memory.h"
#include <stdbool.h>
#include <stdint.h>

#define USED true
#define FREE false

static bool frame_map[MAX_PAGE_COUNT];
static uint64_t pool_ptr;
static uint64_t pool_end_ptr;
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
    uint64_t pool_start = (uint64_t) get_largest_usable_memory_block();
    uint64_t size = get_largest_usable_memory_block_size();

    // check alignment
    if(pool_start % PAGE_SIZE != 0) {
#ifdef TEST_MODE
        debug_printf("Largest usable memory block start is misaligned (%p), rounding to: ", pool_start);
#endif
        pool_start += PAGE_SIZE - (pool_start % PAGE_SIZE);

#ifdef TEST_MODE 
        debug_printf("%p\n", pool_start);
#endif
    }

    uint64_t pool_end = (uintptr_t) pool_start + size;

    if(pool_end % PAGE_SIZE != 0) {
#ifdef TEST_MODE
        debug_printf("Largest usable memory block end is misaligned (%p), rounding to: ", pool_start);
#endif
        pool_end -= pool_end % PAGE_SIZE;
    }

    size = pool_end - pool_start;
    max_available_pages = size / PAGE_SIZE;
    if(max_available_pages > MAX_PAGE_COUNT) max_available_pages = MAX_PAGE_COUNT;
    pool_ptr = pool_start;
    pool_end_ptr = pool_end;
    total = max_available_pages * PAGE_SIZE;
}

#ifdef TEST_MODE
#include "test/assert.h"
#include "test/test.h"
void test_allocator() {
    debug_printf("Page frame pool start: %p | end: %p\n", pool_ptr, pool_end_ptr);
    bool pass = true;

    describe(
        "pool alignment check",
        assert_equals_uint(0, pool_ptr % PAGE_SIZE, "is page pool start aligned?"),
        assert_equals_uint(0, pool_end_ptr % PAGE_SIZE, "is page pool end aligned?")
    );

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

    describe(
        "used frame check",
        assert(frame_map[i1] == USED, "frame 1 used"),
        assert(frame_map[i2] == USED, "frame 2 used"),
        assert(frame_map[i3] == USED, "frame 3 used"),
        assert(frame_map[i4] == USED, "frame 4 used")
    );
    debug("Freeing frames");
    kfree_frame(p1);
    kfree_frame(p2);
    kfree_frame(p3);
    kfree_frame(p4);

    describe(
        "free frame check",
        assert(frame_map[i1] == FREE, "frame 1 free"),
        assert(frame_map[i2] == FREE, "frame 2 free"),
        assert(frame_map[i3] == FREE, "frame 3 free"),
        assert(frame_map[i4] == FREE, "frame 4 free")
    );
}
#endif

