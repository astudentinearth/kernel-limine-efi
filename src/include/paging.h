#pragma once
#include "hardware/allocator.h"
#include "stdint.h"

// page tree offsets
#define PML4_IDX(virt) (virt >> 39) & 0x1FF
#define PDPT_IDX(virt) (virt >> 30) & 0x1FF
#define PD_IDX(virt) (virt >> 21) & 0x1FF
#define PT_IDX(virt) (virt >> 12) & 0x1FF
#define PAGE_OFFSET(virt) (virt & 0xFFF)
#define PS_PAGE_OFFSET(virt) (virt & 0x1FFFFF)
#define ENTRY_ADDR(entry) (entry & 0x000FFFFFFFFFF000ULL)

// flags
#define IS_PRESENT 0x1
#define READ_WRITE 0x2
#define PS 0x80
#define PAGE_CACHE_DISABLE 0x10
#define USER_PAGE 0x4

void init_paging();
void *get_virtaddr(void *phys_addr);
void *get_physaddr(void *virt_addr, u64 *pml4);
u64 *get_active_pml4();

void map_page(void *physical_address, void *virtual_address, u32 flags);
pageframe_t unmap_page(void* virtual_addr);
