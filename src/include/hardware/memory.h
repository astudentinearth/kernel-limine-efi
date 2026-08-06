#pragma once
#include "stdint.h"

typedef enum {
    USABLE = 0,
    HW_RESERVED = 1,
    KERNEL_RESERVED = 2,
    ACPI_RECLAIMABLE = 3,
    BOOT_RECLAIMABLE = 4,
    FRAMEBUFFER = 5,
    BAD_MEMORY = 6
} MemmapEntryType;

typedef struct {
    uptr base;
    u64 length;
    MemmapEntryType type;
} MemoryMapEntry_t;

#define MAX_MEMMAP_ENTRIES 256

u64 get_usable_memory();
void init_memory_map();

void *get_largest_usable_memory_block();
u64 get_largest_usable_memory_block_size();
MemoryMapEntry_t *get_framebuffer(int i);
MemoryMapEntry_t *get_memmap_entries();
u64 get_memmap_entry_count();
#ifdef TEST_MODE
void dump_memory_info();
#endif
