
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

void kfree_frame(pageframe_t pframe) {

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
void run_out_of_memory() {
    debug_puts(">> Pool start: ");
    debug_put_hex(pool_ptr);
    debug_puts("\nExpected pool end: ");
    debug_put_hex(pool_ptr + total);
    debug_puts("\nMax available pages:: ");
    debug_put_uint(max_available_pages);
    for(uint64_t i = 0; i < max_available_pages; i++) {
        pageframe_t ptr = kalloc_frame();
        if(i % 1000 == 0) {
            debug_puts("\n>> Total allocated bytes: ");
            debug_put_uint(total_allocated);
            debug_puts("\n>> Last frame address: ");
            debug_put_hex(ptr);
        }
    }
    debug("\n[!] Allocated all pages. We should now crash.");
    kalloc_frame();
    kalloc_frame();
    kalloc_frame();
    kalloc_frame();
}
#endif

