
#include "hardware/pic.h"
#include "debug.h"
#include "hardware/acpi.h"
#include "hardware/cpu.h"
#include "hardware/msr.h"
#include "hardware/serial.h"
#include "mem.h"
#include "paging.h"
#include <cpuid.h>
#include <stddef.h>
#include <stdint.h>

static uintptr_t apic_base;
static uintptr_t io_apic_base;

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

uintptr_t cpu_get_apic_base()
{
    uint32_t eax, edx;
    cpu_get_msr(IA32_APIC_BASE_MSR, &eax, &edx);
    return (eax & 0xfffff000);
}

void parse_madt()
{
    debug_info("Parsing MADT\n");
    struct system_tables *acpi_tables = get_acpi_tables();
    struct ACPI_SDT *madt = acpi_tables->apic;

    if (madt == NULL) {
        panic("No MADT/APIC table available.");
        return;
    }

    debug_info("MADT length: %d\n", madt->length);

    struct MADT_pic_info *pic_info =
        (struct MADT_pic_info *)((uintptr_t)madt + sizeof(struct ACPI_SDT));

    if (pic_info->flags & MADT_FLAG_LEGACY_PIC_INSTALLED) {
        debug_info("No legacy PIC installed in this system.\n");
    } else {
        debug_info("Legacy PIC is available in this system.\n");
    }

    uint32_t remaining_length = madt->length;
    remaining_length -= sizeof(struct ACPI_SDT);
    remaining_length -= sizeof(struct MADT_pic_info);

    struct MADT_entry_header *current_header =
        (struct MADT_entry_header *)((uintptr_t)pic_info +
                                     sizeof(struct MADT_pic_info));

    while (remaining_length > 0) {
        switch (current_header->entry_type) {
        case MADT_ENTRY_IO_APIC: {
            struct MADT_io_apic_entry *entry =
                (struct MADT_io_apic_entry *)current_header;
            debug_info(
                "acpi: found ioapic id %d @%p with global system int base %p\n",
                entry->io_apic_id, entry->io_apic_phys_addr,
                entry->global_system_interrupt_base);
            break;
        }
        default: {
            debug_info("Skipped MADT entry type %d with length %d\n",
                       current_header->entry_type,
                       current_header->record_length);
        }
        }
        remaining_length -= current_header->record_length;
        current_header =
            (struct MADT_entry_header *)((uintptr_t)current_header +
                                         current_header->record_length);
    }
}

void init_apic()
{
    if (!check_apic()) {
        panic("APIC is not supported. Halting.");
        return;
    }

    uintptr_t base = cpu_get_apic_base();
    debug_info("APIC base: %p\n", base);
    apic_base = (uintptr_t)get_virtaddr((void *)base);
    map_page((void *)base, (void *)apic_base, READ_WRITE | PAGE_CACHE_DISABLE);

    cpu_set_apic_base(base);

    volatile uint32_t *sivr =
        (volatile uint32_t *)(apic_base + APIC_SIVR_OFFSET);
    *sivr = 0x100 | 0xFF;
    debug_success("APIC is now ready.\n");
    parse_madt();
}
