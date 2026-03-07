
#include "paging.h"
#include "boot/limine_requests.h"
#include "debug.h"
#include "hardware/allocator.h"
#include "test/assert.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

uint64_t pml4[512];

extern uint64_t get_cr3();
static bool pml4_initialized = false;

uint64_t* create_page_table() {
    
}

void* get_virtaddr(void* phys) {
    return (void*)((uint64_t)phys + get_hhdm_offset());
}

uint64_t* create_pd() {
    pageframe_t frame = kalloc_frame();
    uint64_t* ptr = (uint64_t*)frame;
    for(int i = 0; i < 512; i++) {
        ptr[i] = 0;
    }
    return ptr;
}


void* get_physaddr(void* virtual_addr) {
    
    uint64_t cr3 = get_cr3();
    uint64_t* limine_pml4 = (uint64_t*)get_virtaddr((void*)(cr3 & ~0xFFF));

    uint64_t pml4_index = PML4_IDX((uint64_t)virtual_addr);
    uint64_t pml4_entry = limine_pml4[pml4_index];
    
    if(!(pml4_entry & IS_PRESENT)) return NULL;

    uint64_t* pdpt = (uint64_t*)get_virtaddr((void*)(ENTRY_ADDR(pml4_entry)));
    uint64_t pdpt_entry = pdpt[PDPT_IDX((uint64_t)virtual_addr)];
    if(!(pdpt_entry & IS_PRESENT)) return NULL;

    uint64_t* pd = (uint64_t*)get_virtaddr((void*)(ENTRY_ADDR(pdpt_entry)));
    uint64_t pd_entry = pd[PD_IDX((uint64_t)virtual_addr)];
    if(!(pd_entry & IS_PRESENT)) return NULL;

    uint64_t* pt = (uint64_t*)get_virtaddr((void*)(ENTRY_ADDR(pd_entry)));
    uint64_t pt_idx =PT_IDX((uint64_t)virtual_addr);
    uint64_t pt_entry = pt[pt_idx];
    if(!(pt_entry & IS_PRESENT)) return NULL;
    
    uint64_t addr = ENTRY_ADDR(pt_entry);
    return (void*)(addr + PAGE_OFFSET(( uint64_t )virtual_addr));

}

void init_paging() {
}

#ifdef TEST_MODE

void test_paging() {
    uint64_t kernel_phys = get_physical_executable_base();
    uint64_t kernel_virt = get_virtual_executable_base();

    describe("initial kernel mapping",
            assert_equals_ptr((void*)kernel_phys, get_physaddr((void*)kernel_virt), "kernel virt to phys")
            );

}

#endif

