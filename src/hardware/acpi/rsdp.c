
#include <stddef.h>
#include "boot/limine_requests.h"
#include "debug.h"
#include "hardware/acpi.h"
#include "boot/limine.h"
#include "paging.h"
#include <stdbool.h>
#include "stdint.h"
#include "string.h"

static const struct RSDP_t *rsdp;
static const struct XSDP_t *xsdp;

bool validate_rsdp(const struct RSDP_t *rsdp) {
    u8 sum = 0;
    u8 *bytes = (u8*)rsdp;
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
    uptr base = limine_get_rsdp_base();
    void* rsdp_base = get_virtaddr((void*)base);
    map_page((void*)base, rsdp_base, IS_PRESENT);
    rsdp = rsdp_base;

    if(rsdp->revision == REVISION_RSDP) {
        // ACPI 1.0
        debug_info("Found RSDP revision %d (ACPI 1.0)\n", rsdp->revision);
    }  else if (rsdp->revision == REVISION_XSDP) {
        debug_info("Found XSDP (revision %d) for ACPI 2.0+\n", rsdp->revision);
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
uptr get_rsdt_address() {
    return rsdp->rsdt_address;   
}

