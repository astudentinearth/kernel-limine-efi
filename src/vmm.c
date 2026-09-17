#include "vmm.h"
#include "debug.h"
#include "hardware/allocator.h"
#include "lock.h"
#include "math.h"
#include "test/assert.h"

#define KERNEL_RESERVED_START 0xffffffff80000000

static uptr vmm_start, vmm_current;
static bool vmm_initialized = false;

void init_vmm(usize hhdm_offset, usize physical_address_bits)
{
    if (vmm_initialized) { return; }
    usize bits = min(physical_address_bits, VMM_CAP_PHYSICAL_ADDRESS_BITS);
    vmm_start = hhdm_offset + upow(2, bits);

    // align to next page
    vmm_start = vmm_start + (PAGE_SIZE - (vmm_start % PAGE_SIZE));
    vmm_current = vmm_start;

    debug_info("vmm: initialized vmm arena at %p (%d physical address bits, "
               "skipped %u bytes)\n",
               vmm_start, physical_address_bits, vmm_start - hhdm_offset);
    vmm_initialized = true;
}

void *valloc(usize n)
{
    usize pages = pages_for_size(n);
    _no_interrupts void *ret = (void *)(vmm_current);
    vmm_current += pages * PAGE_SIZE;
    // guard against overflow
    if (vmm_current >= KERNEL_RESERVED_START || vmm_current < (uptr)ret) {
        panic("vmm: out of virtual address space in higher half");
    }

    return ret;
}

usize get_vmm_start() { return vmm_start; }

#ifdef TEST_MODE
void test_vmm()
{
    if (!vmm_initialized) { panic("cannot test vmm: not initialized."); }
    uptr initial = vmm_current;
    valloc(4095);
    uptr test_roundup = vmm_current;
    uptr test_4096_start = (uptr)valloc(PAGE_SIZE);
    uptr test_4096_end = vmm_current;
    uptr test_multi_page_start = (uptr)valloc(4 * PAGE_SIZE);
    uptr test_multi_page_end = vmm_current;
    describe("valloc tests", assert_equals_uint(0, vmm_start % PAGE_SIZE,
                                                "vmm start is page aligned")),
        assert_equals_uint(
            0, test_roundup % PAGE_SIZE,
            "vmm is still page aligned after 4095 byte allocation"),
        assert_equals_uint(PAGE_SIZE, test_roundup - initial,
                           "4095 byte valloc allocates 1 page"),
        assert_equals_uint(PAGE_SIZE, test_4096_end - test_4096_start,
                           "1 page allocation is exactly 1 page"),
        assert_equals_uint(4 * PAGE_SIZE,
                           test_multi_page_end - test_multi_page_start,
                           "allocating 4 pages is exactly 4 pages");
}

#endif
