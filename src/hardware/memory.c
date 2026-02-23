
#include "hardware/memory.h"
#include "limine.h"

static struct limine_memmap_entry **memmap_entries;

void init_memory_map() {}

uint64_t get_usable_memory() { return 0; }
