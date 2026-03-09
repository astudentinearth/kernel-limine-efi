
#include "debug.h"
#include "hardware/acpi.h"
#include "mem.h"
#include "paging.h"
#include "string.h"
#include <stdbool.h>
#include <stdint.h>

const struct RSDT_t *rsdt;

static struct system_tables acpi_table_directory = {
    .apic = NULL, .facp = NULL, .hpet = NULL, .waet = NULL};

uint32_t get_rsdt_entry_count()
{
    return (rsdt->header.length - sizeof(rsdt->header)) / 4;
}

bool validate_rsdt(const struct ACPI_SDT *rsdt_header)
{
    uint8_t sum = 0;
    for (unsigned int i = 0; i < rsdt_header->length; i++) {
        sum += ((uint8_t *)rsdt_header)[i];
    }
    return sum == 0;
}

void memmap_rsdt_entries()
{
    debug_info("Memory mapping RSDT entries\n");
    for (uint32_t i = 0; i < get_rsdt_entry_count(); i++) {
        void *phys_addr = (void *)(uint64_t)(rsdt->other_headers_ptr[i]);
        void *virt_addr = get_virtaddr((void *)phys_addr);
        map_page(phys_addr, virt_addr, IS_PRESENT);
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
}

void init_rsdt()
{
    uintptr_t physical_addr = get_rsdt_address();
    void *virtual_addr = get_virtaddr((void *)physical_addr);

    // leave page read-only
    map_page((void *)physical_addr, virtual_addr, IS_PRESENT);

    rsdt = (const struct RSDT_t *)virtual_addr;

    if (!validate_rsdt(&rsdt->header)) {
        panic("RSDT checksum failed.");
        return;
    }

    char oemid[7];
    char oem_table_id[8];
    memset(oemid, 0, 7);
    memset(oem_table_id, 0, 9);
    memcpy(oemid, rsdt->header.oem_id, 6);
    memcpy(oem_table_id, rsdt->header.oem_table_id, 8);

    debug_info("Found RSDT @%p | OEM identifier: %s | OEM table identifier: %s "
               "| Entry count: %d\n",
               rsdt, oemid, oem_table_id, get_rsdt_entry_count());

    memmap_rsdt_entries();
}

struct system_tables *get_acpi_tables() {
    return &acpi_table_directory;
}
