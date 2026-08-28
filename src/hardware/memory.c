
#include "hardware/memory.h"
#include "boot/limine.h"
#include "boot/limine_requests.h"
#include "debug.h"
#include <stddef.h>

static u64 memmap_entry_count;
static bool memmap_initialized = false;
static void *largest_memory_block_start;
static u64 largest_memory_block_size;

static MemoryMapEntry_t memmap_entries[MAX_MEMMAP_ENTRIES];

extern char _kernel_end[];

MemoryMapEntry_t map_limine_entry(struct limine_memmap_entry *entry)
{
    MemmapEntryType type;
    switch (entry->type) {
    case LIMINE_MEMMAP_USABLE:
        type = USABLE;
        break;

    case LIMINE_MEMMAP_ACPI_NVS:
    case LIMINE_MEMMAP_RESERVED:
        type = HW_RESERVED;
        break;

    case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
        type = BOOT_RECLAIMABLE;
        break;

    case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
        type = KERNEL_RESERVED;
        break;

    case LIMINE_MEMMAP_FRAMEBUFFER:
        type = FRAMEBUFFER;
        break;

    case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
        type = ACPI_RECLAIMABLE;
        break;

    case LIMINE_MEMMAP_BAD_MEMORY:
        type = BAD_MEMORY;
        break;
    }

    MemoryMapEntry_t mapped = {
        .base = entry->base, .length = entry->length, .type = type};

    return mapped;
}

void load_limine_memmap_entries()
{
    struct limine_memmap_response *memmap_response = get_limine_memmap();
    memmap_entry_count = memmap_response->entry_count;

    for (u64 i = 0; i < memmap_entry_count; i++) {
        memmap_entries[i] = map_limine_entry(memmap_response->entries[i]);
    }
}

void init_memory_map()
{
    load_limine_memmap_entries();

    MemoryMapEntry_t largest = memmap_entries[0];
    for (u64 i = 1; i < memmap_entry_count; i++) {
        MemoryMapEntry_t current = memmap_entries[i];
        if (current.type != USABLE) { continue; }
        if (current.length > largest.length) { largest = current; }
    }

    if (largest.type != USABLE) { panic("No suitable memory block for data."); }

    largest_memory_block_start = (void *)largest.base;
    largest_memory_block_size = largest.length;

    memmap_initialized = true;
}

u64 get_usable_memory()
{
    if (!memmap_initialized) { init_memory_map(); }
    u64 result = 0;
    for (u64 i = 0; i < memmap_entry_count; i++) {
        if (memmap_entries[i].type != USABLE) { continue; }
        result += memmap_entries[i].length;
    }
    return result;
}

u64 get_memmap_entry_count() { return memmap_entry_count; }

void *get_largest_usable_memory_block()
{
    if (memmap_initialized) { return largest_memory_block_start; }
    init_memory_map();
    return largest_memory_block_start;
}

MemoryMapEntry_t *get_framebuffer(int n)
{
    for (u64 i = 0; i < memmap_entry_count; i++) {
        MemoryMapEntry_t *entry = &memmap_entries[i];
        if (entry->type != FRAMEBUFFER) { continue; }
        if (n == 0) { return entry; }
        n--;
    }
    return NULL;
}

u64 get_largest_usable_memory_block_size()
{
    if (memmap_initialized) { return largest_memory_block_size; }
    init_memory_map();
    return largest_memory_block_size;
}

MemoryMapEntry_t *get_memmap_entries() { return memmap_entries; }

#ifdef TEST_MODE
static void dump_memory_entry(MemoryMapEntry_t *entry)
{
    debug_printf("Memory entry | Start: %p | Length: %u | Type: ", entry->base,
                 entry->length);
    switch (entry->type) {
    case USABLE:
        debug_puts("USABLE");
        break;

    case KERNEL_RESERVED:
        debug_puts("KERNEL_RESERVED");
        break;

    case ACPI_RECLAIMABLE:
        debug_puts("ACPI_RECLAIMABLE");
        break;

    case BAD_MEMORY:
        debug_puts("BAD_MEMORY");
        break;

    case HW_RESERVED:
        debug_puts("HW_RESERVED");
        break;

    case BOOT_RECLAIMABLE:
        debug_puts("BOOT_RECLAIMABLE");
        break;

    case FRAMEBUFFER:
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
    if (!memmap_initialized) { init_memory_map(); }
    u64 usable_memory = get_usable_memory();

    debug("[[[ BEGIN MEMORY INFO ]]]]");
    debug_printf("Usable memory: %u bytes \n", usable_memory);

    for (u64 i = 0; i < memmap_entry_count; i++) {
        dump_memory_entry(&memmap_entries[i]);
    }

    debug_printf("---\n");
    debug_printf("Kernel address | Physical: %p | Virtual: %p\n",
                 get_physical_executable_base(), get_virtual_executable_base());
    debug_printf("Kernel end (per linker symbol): %p\n", (u64)_kernel_end);
    debug_printf("HHDM offset: %p\n", get_hhdm_offset());
    debug_printf("---\n");
    debug_puts("\n[[[ END MEMORY INFO ]]]\n");
}
#endif
