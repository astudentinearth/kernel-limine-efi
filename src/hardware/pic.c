
#include "hardware/pic.h"
#include "debug.h"
#include "hardware/acpi.h"
#include "hardware/cpu.h"
#include "hardware/msr.h"
#include "hardware/serial.h"
#include "paging.h"
#include "stdint.h"
#include "string.h"
#include <cpuid.h>
#include <stddef.h>

uptr apic_base;
static io_apic_t default_apic;

void disable_legacy_pic(void)
{
    outb(PIC1_8259_DATA, 0xff);
    outb(PIC2_8259_DATA, 0xff);
}

__attribute__((used)) void lapic_eoi();
void lapic_eoi() { *(volatile u32 *)(apic_base + 0xB0) = 0; }

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

void cpu_set_apic_base(uptr apic)
{
    u32 edx = 0;
    u32 eax = (apic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;
    cpu_set_msr(IA32_APIC_BASE_MSR, eax, edx);
}

uptr cpu_get_apic_base()
{
    u32 eax, edx;
    cpu_get_msr(IA32_APIC_BASE_MSR, &eax, &edx);
    return (eax & 0xfffff000);
}

u32 read_ioapic_reg(io_apic_t apic, u8 offset)
{
    *(u32 volatile *)(apic.virt_addr) = offset; // select register
    return *(volatile u32 *)(apic.virt_addr + 0x10);
}

void write_ioapic_reg(io_apic_t apic, u8 offset, u32 data)
{
    *(u32 volatile *)(apic.virt_addr) = offset;
    *(u32 volatile *)(apic.virt_addr + 0x10) = data;
}

void read_ioapic_redir_entry(io_apic_t apic, u8 n,
                             io_apic_redirection_entry_t *out)
{
    out->lower = read_ioapic_reg(apic, IOAPICREDTBL(n));
    out->upper = read_ioapic_reg(apic, IOAPICREDTBL(n) + 1);
}

void write_ioapic_redir_entry(io_apic_t apic, u8 n,
                              io_apic_redirection_entry_t entry)
{
    write_ioapic_reg(apic, IOAPICREDTBL(n), entry.lower);
    write_ioapic_reg(apic, IOAPICREDTBL(n) + 1, entry.upper);
}

io_apic_t *get_default_ioapic() { return &default_apic; }

void setup_ioapic(io_apic_t *apic, void *phys_addr, u64 gsib)
{
    apic->global_interrupt_base = gsib;
    apic->phys_addr = (uptr)phys_addr;
    apic->virt_addr = (uptr)get_virtaddr(phys_addr);
    apic->id = (read_ioapic_reg(*apic, IOAPICID) >> 24) & 0x0F;
    apic->max_redir_entry_count = (read_ioapic_reg(*apic, IOAPICVER) >> 16) + 1;
    debug_info("IOAPIC %d initialized with %d max entries\n", apic->id,
               apic->max_redir_entry_count);
};

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
        (struct MADT_pic_info *)((uptr)madt + sizeof(struct ACPI_SDT));

    if (pic_info->flags & MADT_FLAG_LEGACY_PIC_INSTALLED) {
        debug_info("No legacy PIC installed in this system.\n");
    } else {
        debug_info("Legacy PIC is available in this system.\n");
    }

    u32 remaining_length = madt->length;
    remaining_length -= sizeof(struct ACPI_SDT);
    remaining_length -= sizeof(struct MADT_pic_info);

    struct MADT_entry_header *current_header =
        (struct MADT_entry_header *)((uptr)pic_info +
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

            void *ioapic_virt =
                get_virtaddr((void *)(uptr)entry->io_apic_phys_addr);
            map_page((void *)(uptr)entry->io_apic_phys_addr, ioapic_virt,
                     READ_WRITE | PAGE_CACHE_DISABLE);
            debug_info("acpi: mapped ioapic to virtual address @%p\n",
                       ioapic_virt);
            setup_ioapic(&default_apic, (void *)(uptr)entry->io_apic_phys_addr,
                         entry->global_system_interrupt_base);
            break;
        }
        default: {
            debug_info("Skipped MADT entry type %d with length %d\n",
                       current_header->entry_type,
                       current_header->record_length);
            break;
        }
        }
        remaining_length -= current_header->record_length;
        current_header =
            (struct MADT_entry_header *)((uptr)current_header +
                                         current_header->record_length);
    }
}

void init_apic()
{
    if (!check_apic()) {
        panic("APIC is not supported. Halting.");
        return;
    }

    uptr base = cpu_get_apic_base();
    debug_info("APIC base: %p\n", base);
    apic_base = (uptr)get_virtaddr((void *)base);
    map_page((void *)base, (void *)apic_base, READ_WRITE | PAGE_CACHE_DISABLE);

    cpu_set_apic_base(base);

    volatile u32 *sivr = (volatile u32 *)(apic_base + APIC_SIVR_OFFSET);
    *sivr = 0x100 | 0xFF;
    debug_success("APIC is now ready.\n");
    parse_madt();
}
