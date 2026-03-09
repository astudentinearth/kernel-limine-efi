
#include "hardware/pic.h"
#include "debug.h"
#include "hardware/cpu.h"
#include "hardware/msr.h"
#include "hardware/serial.h"
#include <cpuid.h>
#include <stdint.h>
#include "paging.h"

static uintptr_t apic_base;

void disable_legacy_pic(void)
{
    outb(PIC1_8259_DATA, 0xff);
    outb(PIC2_8259_DATA, 0xff);
}

bool check_apic()
{
    int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    bool result = edx & CPUID_FEAT_EDX_APIC;
#ifdef TEST_MODE
    debug_printf("APIC support %s.\n", result ? "available" : "unavailable");
#endif
    return result;
}

void cpu_set_apic_base(uintptr_t apic)
{
    uint32_t edx = 0;
    uint32_t eax = (apic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;
    cpu_set_msr(IA32_APIC_BASE_MSR, eax, edx);
}

uintptr_t cpu_get_apic_base() {
   uint32_t eax, edx;
   cpu_get_msr(IA32_APIC_BASE_MSR, &eax, &edx);
   return (eax & 0xfffff000);
}

void init_apic()
{
    if (!check_apic()) {
        panic("APIC is not supported. Halting.");
        return;
    }

    uintptr_t base = cpu_get_apic_base();
    debug_info("APIC base: %p\n", base);
    apic_base = (uintptr_t)get_virtaddr((void*)base);
    map_page((void*)base, (void*)apic_base, READ_WRITE | PAGE_CACHE_DISABLE);

    cpu_set_apic_base(base);

    volatile uint32_t *sivr = (volatile uint32_t*)(apic_base + APIC_SIVR_OFFSET);
    *sivr = 0x100 | 0xFF;
    debug_success("APIC is now ready.\n");
}
