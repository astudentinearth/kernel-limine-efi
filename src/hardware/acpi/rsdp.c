
#include <stddef.h>
#include "boot/limine_requests.h"
#include "debug.h"
#include "hardware/acpi.h"
#include "limine.h"
#include "paging.h"
#include <stdbool.h>
#include <stdint.h>
#include "mem.h"

static const struct RSDP_t *rsdp;
static const struct XSDP_t *xsdp;
static uint32_t rsdp_revision;

bool validate_rsdp(const struct RSDP_t *rsdp) {
    uint8_t sum = 0;
    uint8_t *bytes = (uint8_t*)rsdp;
    for(unsigned int i = 0; i < sizeof(struct RSDP_t); i++) {
        sum += bytes[i];
    }

    // lowest byte should be set to 0 in the checksum
    return sum == 0;
}

bool validate_xsdp(struct XSDP_t *xsdp) {
    // not implemented
    return false;
}

void limine_init_rsdp() {
    struct limine_rsdp_response* rsdp_response = get_limine_rsdp();
    if(rsdp_response == NULL) {
        panic("Could not retrieve the RSDP address from the bootloader.\n");
        return;
    }

    rsdp_revision = rsdp_response->revision;
    void* rsdp_base = get_virtaddr((void*)rsdp_response->address);
    map_page((void*)rsdp_response->address, rsdp_base, IS_PRESENT);

    if(rsdp_revision == REVISION_RSDP) {
        // ACPI 1.0
        rsdp = rsdp_base;
        debug_info("Found RSDP revision %d (ACPI 1.0)\n", rsdp_revision);
    }  else if (rsdp_revision == REVISION_XSDP) {
        debug_info("Found XSDP (revision %d) for ACPI 2.0+\n", rsdp_revision);
        panic("ACPI v2 support not implemented yet.");
        return;
    }

    bool is_valid_rsdp = validate_rsdp(rsdp);
    if(!is_valid_rsdp) {
        panic("RSDP invalid.\n");
    }
    char oemid[7];
    memset(oemid, 0, 7);
    memcpy(oemid, rsdp->OEMID, 6);

    debug_info("RSDP is valid. OEM identifier: %s\n", oemid);
}

/** Returns the **physical** address of the RSDT. */
uintptr_t get_rsdt_address() {
    return rsdp->rsdt_address;   
}

