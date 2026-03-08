
#include "hardware/memory.h"
#include "boot/limine_requests.h"
#include "debug.h"
#include "limine.h"
#include <stddef.h>

static struct limine_memmap_entry **memmap_entries;
static uint64_t memmap_entry_count;
static bool memmap_initialized = false;
static void *largest_memory_block_start;
static uint64_t largest_memory_block_size;
extern char _kernel_end[];

void init_memory_map()
{
    struct limine_memmap_response *memmap_response = get_limine_memmap();
    memmap_entry_count = memmap_response->entry_count;
    memmap_entries = memmap_response->entries;

    struct limine_memmap_entry *largest = memmap_entries[0];
    for (uint64_t i = 1; i < memmap_entry_count; i++) {
        struct limine_memmap_entry *current = memmap_entries[i];
        if (current->type != LIMINE_MEMMAP_USABLE)
            continue;
        if (current->length > largest->length) {
            largest = current;
        }
    }

    if (largest->type != LIMINE_MEMMAP_USABLE) {
        panic("No suitable memory block for data.");
    }

    largest_memory_block_start = (void *)largest->base;
    largest_memory_block_size = largest->length;

    memmap_initialized = true;
}

uint64_t get_usable_memory()
{
    if (!memmap_initialized)
        init_memory_map();
    uint64_t result = 0;
    for (uint64_t i = 0; i < memmap_entry_count; i++) {
        if (memmap_entries[i]->type != LIMINE_MEMMAP_USABLE)
            continue;
        result += memmap_entries[i]->length;
    }
    return result;
}

struct limine_memmap_entry **get_memmap_entries()
{
    if (!memmap_initialized)
        return NULL;
    return memmap_entries;
}

uint64_t get_memmap_entry_count() { return memmap_entry_count; }

void *get_largest_usable_memory_block()
{
    if (memmap_initialized)
        return largest_memory_block_start;
    init_memory_map();
    return largest_memory_block_start;
}

struct limine_memmap_entry *get_framebuffer(int n)
{
    for (uint64_t i = 0; i < memmap_entry_count; i++) {
        struct limine_memmap_entry *entry = memmap_entries[i];
        if (entry->type != LIMINE_MEMMAP_FRAMEBUFFER)
            continue;
        if (n == 0)
            return entry;
        n--;
    }
    return NULL;
}

uint64_t get_largest_usable_memory_block_size()
{
    if (memmap_initialized)
        return largest_memory_block_size;
    init_memory_map();
    return largest_memory_block_size;
}

#ifdef TEST_MODE
static void dump_memory_entry(struct limine_memmap_entry *entry)
{
    debug_printf("Memory entry | Start: %p | Length: %u | Type: ", entry->base,
                 entry->length);
    switch (entry->type) {
    case LIMINE_MEMMAP_USABLE:
        debug_puts("USABLE");
        break;

    case LIMINE_MEMMAP_ACPI_NVS:
        debug_puts("ACPI_NVS");
        break;

    case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
        debug_puts("EXECTUABLE_AND_MODULES");
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
    debug_newline();
}

void dump_memory_info()
{
    if (!memmap_initialized)
        init_memory_map();
    uint64_t usable_memory = get_usable_memory();

    debug("[[[ BEGIN MEMORY INFO ]]]]");
    debug_printf("Usable memory: %u bytes \n", usable_memory);

    for (uint64_t i = 0; i < memmap_entry_count; i++) {
        dump_memory_entry(memmap_entries[i]);
    }

    debug_printf("---\n");
    debug_printf("Kernel address | Physical: %p | Virtual: %p\n",
                 get_physical_executable_base(), get_virtual_executable_base());
    debug_printf("Kernel end (per linker symbol): %p\n", (uint64_t)_kernel_end);
    debug_printf("HHDM offset: %p\n", get_hhdm_offset());
    debug_printf("---\n");
    debug_puts("\n[[[ END MEMORY INFO ]]]\n");
}
#endif
