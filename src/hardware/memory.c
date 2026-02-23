
#include "debug.h"
#include "hardware/memory.h"
#include "boot/limine_requests.h"
#include "limine.h"

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
static void dump_memory_entry(struct limine_memmap_entry* entry) {
    debug("Memory entry: ");
    debug_puts("    Start address: ");
    debug_put_hex(entry->base);
    debug_puts("\n    Length: ");
    debug_put_uint(entry->length);
    debug_puts("\n    Type: ");
    switch(entry->type) {
        case LIMINE_MEMMAP_USABLE:
            debug_puts("USABLE");
            break;

        case LIMINE_MEMMAP_ACPI_NVS:
            debug_puts("ACPI_NVS");
            break;

        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            debug_puts("ACPI_RECLAIMABLE");
            break;

        case LIMINE_MEMMAP_BAD_MEMORY:
            debug_puts("BAD_MEMORY");
            break;

        case LIMINE_MEMMAP_RESERVED:
            debug_puts("RESERVED");
            break;

        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            debug_puts("MEMMAP_BOOTLOADER_RECLAIMABLE");
            break;

        case LIMINE_MEMMAP_FRAMEBUFFER:
            debug_puts("FRAMEBUFFER");
            break;

        default:
            debug_put_hex(entry->type);
            break;
    }
    debug_puts("\n\n");
}

void dump_memory_info() {
    uint64_t usable_memory = get_usable_memory();

    debug("[[[ BEGIN MEMORY INFO ]]]]");
    debug_puts("Usable memory: ");
    debug_put_uint(usable_memory);
    debug_puts(" bytes \n");
    
    for(uint64_t i = 0; i < memmap_entry_count; i++) {
        dump_memory_entry(memmap_entries[i]);
    }

    debug_puts("\n[[[ END MEMORY INFO ]]]\n");
}
#endif

