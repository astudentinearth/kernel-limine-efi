
#include "debug.h"
#include "hardware/allocator.h"
#include "hardware/memory.h"
#include "paging.h"
#include "result.h"
#include "stdint.h"
#include "string.h"
#include <stdbool.h>

#define USED true
#define FREE false

static u64 total_allocated_pages = 0;

// bitmap helpers

static inline bool get_bit(u8 *bitmap, usize idx)
{
    u8 *byte = bitmap + (idx / 8);
    return *byte & (1 << (idx % 8));
}

static inline void set_bit(u8 *bitmap, usize idx, bool value)
{
    u8 *byte = bitmap + (idx / 8);
    if (value) {
        *byte |= 1 << (idx % 8);
    } else {
        *byte &= ~(1 << (idx % 8));
    }
}

// physical region helpers

typedef struct {
    u8 *frame_map;
    uptr start;
    uptr end;
    usize size;
    usize total_reserved;
    usize total_allocated;

    /** index of the last allocated page */
    usize last_allocated;
    usize total_pages;
} PhysicalRegion_t;

static PhysicalRegion_t active_region = {0, 0, 0, 0, 0, 0, 0, 0};

static void init_region(PhysicalRegion_t *region, uptr start, uptr end)
{
    usize size = end - start;
    usize page_count = size / PAGE_SIZE;
    usize bitmap_size = page_count / 8;
    usize reserved_for_bitmap = bitmap_size / PAGE_SIZE + 1;

    region->size = size;
    region->start = start;
    region->end = end;
    region->frame_map = (u8 *)get_virtaddr((void *)start);
    region->total_pages = page_count;
    region->total_reserved = reserved_for_bitmap;
    region->total_allocated = reserved_for_bitmap;
    region->last_allocated = reserved_for_bitmap - 1;

    memset(region->frame_map, 0, reserved_for_bitmap * PAGE_SIZE);

    // set reserved pages to allocated
    for (usize idx = 0; idx < reserved_for_bitmap; idx++) {
        set_bit(region->frame_map, idx, true);
    }
}

/** Returns the index of the next free page starting from idx. */
static option_usize next_free_page(PhysicalRegion_t *region, usize idx)
{
    if (get_bit(region->frame_map, idx) == FREE) {
        option_usize result = {idx, option_not_empty};
        return result;
    }

    while (get_bit(region->frame_map, idx) == USED) {
        idx++;
        if (idx >= region->total_pages) {
            option_usize result = {0, option_empty};
            return result;
        }
    }

    option_usize result = {idx, option_not_empty};
    return result;
}

static pageframe_t alloc_frame(PhysicalRegion_t *region)
{
    if (region->total_allocated >= region->total_pages) {
        panic("pmm: out of pages: all allocated.");
    }
    option_usize result = next_free_page(region, region->last_allocated + 1);
    if (result.empty) { result = next_free_page(region, 0); }
    if (result.empty) {
        panic("pmm: out of pages: no empty pages even after secondary "
              "traversal.");
    }

    region->last_allocated = result.val;
    set_bit(region->frame_map, result.val, USED);
    region->total_allocated++;

    return region->start + result.val * PAGE_SIZE;
}

usize get_frame_idx(PhysicalRegion_t *region, pageframe_t pframe)
{ return (pframe - region->start) / PAGE_SIZE; }

static void free_frame(PhysicalRegion_t *region, pageframe_t pframe)
{
    usize idx = get_frame_idx(region, pframe);
    set_bit(region->frame_map, idx, FREE);
    region->total_allocated--;
}

pageframe_t kalloc_frame()
{
    total_allocated_pages++;
    return alloc_frame(&active_region);
}

void *kalloc_vframe() { return get_virtaddr((void *)kalloc_frame()); }

void kfree_frame(pageframe_t pframe)
{
    total_allocated_pages--;
    free_frame(&active_region, pframe);
}

void kfree_vframe(void *vframe)
{
    void *phys_addr = get_physaddr(vframe, get_active_pml4());
    kfree_frame((pageframe_t)phys_addr);
}

void get_pmm_stats(PMMStats_t *stats)
{
    stats->mapped_count = total_allocated_pages;
    stats->total_count = active_region.total_pages;
    stats->page_size = PAGE_SIZE;
}

void debug_print_vm_stats()
{
    PMMStats_t stats;
    memset(&stats, 0, sizeof(PMMStats_t));
    get_pmm_stats(&stats);
    debug_info("== Page stats\nTotal pages: %u\nMapped pages: %u\nPage size: ",
               stats.total_count, stats.mapped_count, stats.page_size);
}

void init_pmm()
{
    u64 pool_start = (u64)get_largest_usable_memory_block();
    u64 size = get_largest_usable_memory_block_size();

    // check alignment
    if (pool_start % PAGE_SIZE != 0) {
#ifdef TEST_MODE
        debug_printf("Largest usable memory block start is misaligned (%p), "
                     "rounding to: ",
                     pool_start);
#endif
        pool_start += PAGE_SIZE - (pool_start % PAGE_SIZE);

#ifdef TEST_MODE
        debug_printf("%p\n", pool_start);
#endif
    }

    u64 pool_end = (uptr)pool_start + size;

    if (pool_end % PAGE_SIZE != 0) {
#ifdef TEST_MODE
        debug_printf(
            "Largest usable memory block end is misaligned (%p), rounding to: ",
            pool_start);
#endif
        pool_end -= pool_end % PAGE_SIZE;
    }

    init_region(&active_region, pool_start, pool_end);
}

#ifdef TEST_MODE
#include "test/assert.h"
#include "test/test.h"
void test_allocator()
{
    debug_printf("Page frame pool start: %p | end: %p\n", active_region.start,
                 active_region.end);

    describe("pool alignment check",
             assert_equals_uint(0, active_region.start % PAGE_SIZE,
                                "is page pool start aligned?"),
             assert_equals_uint(0, active_region.end % PAGE_SIZE,
                                "is page pool end aligned?"));

    pageframe_t p1, p2, p3, p4;
    p1 = kalloc_frame();
    p2 = kalloc_frame();
    p3 = kalloc_frame();
    p4 = kalloc_frame();
    debug_printf("Allocated frames: %X %X %X %X\n", p1, p2, p3, p4);
    u64 i1, i2, i3, i4;
    i1 = get_frame_idx(&active_region, p1);
    i2 = get_frame_idx(&active_region, p2);
    i3 = get_frame_idx(&active_region, p3);
    i4 = get_frame_idx(&active_region, p4);

    u8 *bitmap = active_region.frame_map;

    describe("used frame check",
             assert(get_bit(bitmap, i1) == USED, "frame 1 used"),
             assert(get_bit(bitmap, i2) == USED, "frame 2 used"),
             assert(get_bit(bitmap, i3) == USED, "frame 3 used"),
             assert(get_bit(bitmap, i4) == USED, "frame 4 used"));
    debug("Freeing frames");
    kfree_frame(p1);
    kfree_frame(p2);
    kfree_frame(p3);
    kfree_frame(p4);

    describe("FREE frame check",
             assert(get_bit(bitmap, i1) == FREE, "frame 1 FREE"),
             assert(get_bit(bitmap, i2) == FREE, "frame 2 FREE"),
             assert(get_bit(bitmap, i3) == FREE, "frame 3 FREE"),
             assert(get_bit(bitmap, i4) == FREE, "frame 4 FREE"));
}
#endif
