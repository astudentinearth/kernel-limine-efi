
#include "debug.h"
#include "hardware/acpi.h"
#include "paging.h"
#include "stdint.h"
#include "string.h"
#include <stdbool.h>

static struct RSDT_t *rsdt;
static XSDT_t *xsdt;

static struct system_tables acpi_table_directory = {
    .apic = NULL, .facp = NULL, .hpet = NULL, .waet = NULL};

u32 get_rsdt_entry_count()
{
    return (rsdt->header.length - sizeof(rsdt->header)) /
           (get_acpi_revision() == REVISION_RSDP ? 4 : 8);
}

bool validate_rsdt(const struct ACPI_SDT *rsdt_header)
{
    u8 sum = 0;
    for (unsigned int i = 0; i < rsdt_header->length; i++) {
        sum += ((u8 *)rsdt_header)[i];
    }
    return sum == 0;
}

void map_sdt(uptr phys_addr)
{
    void *virt_addr = get_virtaddr((void *)phys_addr);
    map_page((void *)phys_addr, virt_addr, IS_PRESENT);
    char signature[5];
    memset(signature, 0, 5);
    memcpy(signature, ((struct ACPI_SDT *)(virt_addr))->signature, 4);

    if (str_equals(signature, "APIC")) {
        acpi_table_directory.apic = virt_addr;
    } else if (str_equals(signature, "FACP")) {
        acpi_table_directory.facp = virt_addr;
    } else if (str_equals(signature, "HPET")) {
        acpi_table_directory.hpet = virt_addr;
    } else if (str_equals(signature, "WAET")) {
        acpi_table_directory.waet = virt_addr;
    }

    debug_info("Mapped %s entry @%p\n", signature, virt_addr);
}

uptr get_rsdt_entry(usize n) {
    if(get_acpi_revision() == REVISION_RSDP) return rsdt->other_headers_ptr[n];
    else return xsdt->other_headers_ptr[n];
}

void memmap_rsdt_entries()
{
    debug_info("Memory mapping RSDT entries\n");
    for (u32 i = 0; i < get_rsdt_entry_count(); i++) {
        uptr phys_addr = get_rsdt_entry(i);
        map_sdt(phys_addr);
    }
}

void init_rsdt()
{
    uptr physical_addr = get_root_system_descriptor_addr();
    void *virtual_addr = get_virtaddr((void *)physical_addr);

    // leave page read-only
    map_page((void *)physical_addr, virtual_addr, IS_PRESENT);

    rsdt = (struct RSDT_t *)virtual_addr;

    if (get_acpi_revision() == REVISION_XSDP) { xsdt = (XSDT_t*)virtual_addr; }

    if (!validate_rsdt(&rsdt->header)) {
        panic("RSDT/XSDT checksum failed.");
        return;
    }

    char oemid[7];
    char oem_table_id[8];
    memset(oemid, 0, 7);
    memset(oem_table_id, 0, 9);
    memcpy(oemid, rsdt->header.oem_id, 6);
    memcpy(oem_table_id, rsdt->header.oem_table_id, 8);

    debug_info(
        "Found RSDT/XSDT @%p | OEM identifier: %s | OEM table identifier: %s "
        "| Entry count: %d | Revision: %d\n",
        rsdt, oemid, oem_table_id, get_rsdt_entry_count(),
        rsdt->header.revision);

    memmap_rsdt_entries();
}

struct system_tables *get_acpi_tables() { return &acpi_table_directory; }
