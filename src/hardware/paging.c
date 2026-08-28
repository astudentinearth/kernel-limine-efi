
#include "paging.h"
#include "boot/limine.h"
#include "boot/limine_requests.h"
#include "debug.h"
#include "hardware/allocator.h"
#include "hardware/memory.h"
#include "stdint.h"
#include "string.h"
#include "test/assert.h"
#include <stdbool.h>
#include <stddef.h>

// our page tables that we'll build
u64 *kernel_pml4;

extern u64 get_cr3();
extern void invalidate_page(void *virt);
extern void set_pml4_addr(void *pml4);
extern u64 get_rsp();
static bool paging_initalized = false;
extern char _kernel_end[];

void *get_virtaddr(void *phys)
{ return (void *)((u64)phys + get_hhdm_offset()); }

/** Get a pointer to the active PML4, from the CR3 register */
u64 *get_active_pml4()
{ return ((u64 *)(get_virtaddr((void *)(get_cr3() & ~0xFFF)))); }

/**
 * Evaluate a virtual address to a physical address for given page table
 */
void *get_physaddr(void *virtual_addr, u64 *pml4)
{

    u64 pml4_index = PML4_IDX((u64)virtual_addr);
    u64 pml4_entry = pml4[pml4_index];

    if (!(pml4_entry & IS_PRESENT)) { return NULL; }

    u64 *pdpt = (u64 *)get_virtaddr((void *)(ENTRY_ADDR(pml4_entry)));
    u64 pdpt_entry = pdpt[PDPT_IDX((u64)virtual_addr)];
    if (!(pdpt_entry & IS_PRESENT)) { return NULL; }

    u64 *pd = (u64 *)get_virtaddr((void *)(ENTRY_ADDR(pdpt_entry)));
    u64 pd_entry = pd[PD_IDX((u64)virtual_addr)];

    // 2mb page
    if (pd_entry & PS) {
        u64 physical_addr = ENTRY_ADDR(pd_entry);
        u64 offset = PS_PAGE_OFFSET((u64)virtual_addr);

        return (void *)(physical_addr + offset);
    }

    if (!(pd_entry & IS_PRESENT)) { return NULL; }

    u64 *pt = (u64 *)get_virtaddr((void *)(ENTRY_ADDR(pd_entry)));
    u64 pt_idx = PT_IDX((u64)virtual_addr);
    u64 pt_entry = pt[pt_idx];
    if (!(pt_entry & IS_PRESENT)) { return NULL; }
    u64 addr = ENTRY_ADDR(pt_entry);
    return (void *)(addr + PAGE_OFFSET((u64)virtual_addr));
}

void dump_pt(u64 *pt)
{
    for (int i = 0; i < 512; i++) {
        u64 entry = pt[i];
        if (entry & IS_PRESENT) {
            debug_printf("      Present PT entry: %X TO %X\n", entry,
                         ENTRY_ADDR(entry));
        }
    }
}

void dump_pd(u64 *pd)
{
    for (int i = 0; i < 512; i++) {
        u64 entry = pd[i];
        if (entry & IS_PRESENT) {
            debug_printf("    Present PD entry: %X\n", entry);
            dump_pt(get_virtaddr((void *)ENTRY_ADDR(entry)));
        }
    }
}

void dump_pdpt(u64 *pdpt)
{
    for (int i = 0; i < 512; i++) {
        u64 entry = pdpt[i];
        if (entry & IS_PRESENT) {
            debug_printf("  Present PDPT entry: %X\n", entry);
            dump_pd(get_virtaddr((void *)ENTRY_ADDR(entry)));
        }
    }
}

void dump_pml4(u64 *pml4)
{
    for (int i = 0; i < 512; i++) {
        u64 entry = pml4[i];
        if (entry & IS_PRESENT) {
            debug_printf("Present PML4 entry: %X\n", entry);
            dump_pdpt(get_virtaddr((void *)ENTRY_ADDR(entry)));
        }
    }
}

u64 *create_pd()
{
    pageframe_t frame = kalloc_frame();
    u64 *ptr = (u64 *)get_virtaddr((void *)frame);

    for (int i = 0; i < 512; i++) {
        ptr[i] = 0;
    }
    return ptr;
}

void _map_page(void *physical_address, void *virtual_address, u32 flags,
               u64 *pml4)
{
    asm volatile("cli");
    u64 *pml4_entry = &pml4[PML4_IDX((u64)virtual_address)];

    if (!(*pml4_entry & IS_PRESENT)) {
        // no pml4 entry, create pdpt and assign it to pml4 entry
        u64 *new_pdpt = create_pd();
        if (!get_physaddr(new_pdpt, get_active_pml4())) {
            debug_err("pdpt @%p is null", new_pdpt);
            panic("PDPT NULL");
        }
        u64 new_pml4_entry = ((u64)get_physaddr(new_pdpt, get_active_pml4())) |
                             (IS_PRESENT | flags);
        *pml4_entry = new_pml4_entry;
    }

    u64 *pdpt = (u64 *)get_virtaddr((void *)ENTRY_ADDR((*pml4_entry)));
    u64 *pdpt_entry = &pdpt[PDPT_IDX((u64)virtual_address)];
    if (!(*pdpt_entry & IS_PRESENT)) {
        // no pdpt entry, create pd and assign it to new pdpt entry
        u64 *new_pd = create_pd();
        if (!get_physaddr(new_pd, get_active_pml4())) {
            debug_err("pd @%p is null", new_pd);
            panic("pd NULL");
        }
        u64 new_pdpt_entry = ((u64)get_physaddr(new_pd, get_active_pml4())) |
                             (IS_PRESENT | flags);
        *pdpt_entry = new_pdpt_entry;
    }

    u64 *pd = (u64 *)get_virtaddr((void *)ENTRY_ADDR(*pdpt_entry));
    u64 *pd_entry = &pd[PD_IDX((u64)virtual_address)];
    if (!(*pd_entry & IS_PRESENT)) {
        // no pd entry, create pt and assign it to new pd entry
        u64 *new_pt = create_pd();
        if (!get_physaddr(new_pt, get_active_pml4())) {
            debug_err("pt @%p is null", new_pt);
            panic("pt NULL");
        }
        u64 new_pd_entry = ((u64)get_physaddr(new_pt, get_active_pml4())) |
                           (IS_PRESENT | flags);
        *pd_entry = new_pd_entry;
    }

    u64 *pt = (u64 *)get_virtaddr((void *)ENTRY_ADDR(*pd_entry));
    u64 *pt_entry = &pt[PT_IDX((u64)virtual_address)];
    // we reached the end, map the page
    *pt_entry = (u64)physical_address | (IS_PRESENT | flags);
    asm volatile("sti");
}

void map_page(void *physical_address, void *virtual_address, u32 flags)
{
    _map_page(physical_address, virtual_address, flags, get_active_pml4());
    invalidate_page(virtual_address);
}

void map_kernel()
{
    debug_info("Remapping kernel memory\n");
    u64 phys_base = get_physical_executable_base();
    u64 virt_base = get_virtual_executable_base();
    u64 kernel_end = (u64)_kernel_end;

    while (virt_base < kernel_end) {
        _map_page((void *)phys_base, (void *)virt_base, READ_WRITE,
                  kernel_pml4);
        phys_base += PAGE_SIZE;
        virt_base += PAGE_SIZE;
    }
    debug_info("[DEBUG] Remapped kernel memory\n");
}

void map_hhdm()
{
    debug_info("[DEBUG] Mapping all known memory blocks\n");

    u64 entry_count = get_memmap_entry_count();
    MemoryMapEntry_t *memmap_entries = get_memmap_entries();

    for (unsigned int i = 0; i < entry_count; i++) {
        MemoryMapEntry_t current_entry = memmap_entries[i];
        if (current_entry.type == HW_RESERVED) { continue; }

        u64 page_count = current_entry.length / PAGE_SIZE;

        // round up
        if (current_entry.length % PAGE_SIZE != 0) { page_count++; }

        for (u64 i = 0; i < page_count; i++) {
            void *physical_address =
                (void *)(current_entry.base + i * PAGE_SIZE);
            void *virtual_address = get_virtaddr(physical_address);
            _map_page(physical_address, virtual_address, READ_WRITE,
                      kernel_pml4);
        }
    }
    debug_info("[DEBUG] Mapped all known memory blocks\n");
}

void init_paging()
{
    if (paging_initalized) { return; }
    debug_info("Initializing paging\n");
    kernel_pml4 = get_virtaddr((void *)kalloc_frame());
    memset(kernel_pml4, 0, PAGE_SIZE);
    map_kernel();
    map_hhdm();
    void *pml4_addr = get_physaddr(kernel_pml4, get_active_pml4());
    debug_info("Kernel stack pointer address: %p\n", get_rsp());
    debug_info("Swapping CR3 to %p\n", pml4_addr);
    set_pml4_addr(pml4_addr);
    paging_initalized = true;
    debug_info("We own the pages! CR3 at: %p\n", get_cr3());
}

#ifdef TEST_MODE

void test_paging()
{
    u64 kernel_phys = get_physical_executable_base();
    u64 kernel_virt = get_virtual_executable_base();

    describe(
        "initial kernel mapping",
        assert_equals_ptr((void *)kernel_phys,
                          get_physaddr((void *)kernel_virt, get_active_pml4()),
                          "kernel virt to phys"));
}

#endif
