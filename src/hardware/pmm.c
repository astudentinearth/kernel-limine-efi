
#include "debug.h"
#include "hardware/allocator.h"
#include "hardware/memory.h"
#include <stdbool.h>
#include "paging.h"
#include "stdint.h"

#define USED true
#define FREE false

static bool frame_map[MAX_PAGE_COUNT];
static u64 pool_ptr;
static u64 pool_end_ptr;
static u64 max_available_pages;
static u64 total_allocated_pages = 0;
static u64 total_initially_available = 0;
static u64 total_iter = 0;

static u64 last_allocated = 0;

u64 get_total_iter() { return total_iter; }


pageframe_t kalloc_frame(){
    while(frame_map[last_allocated] != FREE) {
        last_allocated++;
        // return to start to try and find a free page
        if(last_allocated > max_available_pages) last_allocated = 0;
    } 
    while(frame_map[last_allocated] != FREE) {
        last_allocated++;
        if(last_allocated > max_available_pages) {
            panic("Out of physical pages.");
        }
    }
    frame_map[last_allocated] = USED;
    total_allocated_pages ++;
    return pool_ptr + (last_allocated * PAGE_SIZE);
}

void* kalloc_vframe() {
    return get_virtaddr((void*)kalloc_frame());
}

u64 get_frame_idx(pageframe_t pframe) {
    return (pframe - pool_ptr) / PAGE_SIZE;
}

void kfree_frame(pageframe_t pframe) {
    u64 index = get_frame_idx(pframe);
    frame_map[index] = FREE;
    total_allocated_pages--;
}

void kfree_vframe(void* vframe) {
    void* phys_addr = get_physaddr(vframe, get_active_pml4());
    kfree_frame((pageframe_t) phys_addr);
}

void get_pmm_stats(PMMStats_t *stats) {
    stats->mapped_count = total_allocated_pages;
    stats->total_count = max_available_pages;
    stats->page_size = PAGE_SIZE;
}  

void init_pmm() {
    u64 pool_start = (u64) get_largest_usable_memory_block();
    u64 size = get_largest_usable_memory_block_size();

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

    u64 pool_end = (uptr) pool_start + size;

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
    total_initially_available = max_available_pages * PAGE_SIZE;
}

#ifdef TEST_MODE
#include "test/assert.h"
#include "test/test.h"
void test_allocator() {
    debug_printf("Page frame pool start: %p | end: %p\n", get_virtaddr((void*) pool_ptr ), pool_end_ptr);

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
    u64 i1, i2, i3, i4;
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

