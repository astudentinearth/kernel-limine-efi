
#include "debug.h"
#include "mem.h"
#include "hardware/acpi.h"
#include "paging.h"
#include <stdbool.h>
#include <stdint.h>

const struct RSDT_t* rsdt;

uint32_t get_rsdt_entry_count() {
    return ( rsdt->header.length - sizeof(rsdt->header) ) / 4;
}

bool validate_rsdt(const struct ACPI_SDT *rsdt_header) {
    uint8_t sum = 0;
    for(unsigned int i = 0; i < rsdt_header->length; i++) {
        sum += ( (uint8_t*) rsdt_header )[i];
    }
    return sum == 0;
}

void init_rsdt() {
    uintptr_t physical_addr = get_rsdt_address();
    void* virtual_addr = get_virtaddr((void*)physical_addr);

    // leave page read-only
    map_page((void*)physical_addr, virtual_addr, IS_PRESENT);


    rsdt = (const struct RSDT_t*)virtual_addr;

    if(!validate_rsdt(&rsdt->header)) {
        panic("RSDT checksum failed.");
        return;
    }

    char oemid[7];
    char oem_table_id[8];
    memset(oemid, 0, 7);
    memset(oem_table_id, 0, 9);
    memcpy(oemid, rsdt->header.oem_id, 6);
    memcpy(oem_table_id, rsdt->header.oem_table_id, 8);
    
    debug_info("Found RSDT @%p | OEM identifier: %s | OEM table identifier: %s | Entry count: %d\n", rsdt, oemid, oem_table_id, get_rsdt_entry_count());

}

