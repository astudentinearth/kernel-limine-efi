
#include "debug.h"
#include "hardware/memory.h"
#include "boot/limine_requests.h"

static struct limine_memmap_entry **memmap_entries;
static uint64_t memmap_entry_count;
static bool memmap_initialized = false;

void init_memory_map() {
    struct limine_memmap_response *memmap_response = get_limine_memmap();
    memmap_entry_count = memmap_response->entry_count;
    memmap_entries = memmap_response->entries;
    memmap_initialized = true;
}

uint64_t get_usable_memory() { 
    uint64_t result = 0;
    for(uint64_t i = 0; i < memmap_entry_count; i++) {
        if(memmap_entries[i]->type != LIMINE_MEMMAP_USABLE) continue;
        result += memmap_entries[i]->length;
    }
    return result;
}

#ifdef TEST_MODE
void dump_memory_info() {
    uint64_t usable_memory = get_usable_memory();

    debug("[[[ BEGIN MEMORY INFO ]]]]");
    debug_puts("Usable memory: ");
    debug_put_uint(usable_memory);
    debug_puts("bytes ");
    debug_puts("\n[[[ END MEMORY INFO ]]]\n");
}
#endif

