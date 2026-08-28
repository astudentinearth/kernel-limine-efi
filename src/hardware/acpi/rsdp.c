
#include "boot/limine.h"
#include "boot/limine_requests.h"
#include "debug.h"
#include "hardware/acpi.h"
#include "paging.h"
#include "stdint.h"
#include "string.h"
#include <stdbool.h>
#include <stddef.h>

static struct RSDP_t *rsdp;
static struct XSDP_t *xsdp;

bool validate_rsdp(const struct RSDP_t *rsdp)
{
    u8 sum = 0;
    u8 *bytes = (u8 *)rsdp;
    for (unsigned int i = 0; i < sizeof(struct RSDP_t); i++) {
        sum += bytes[i];
    }

    // lowest byte should be set to 0 in the checksum
    return sum == 0;
}

bool validate_xsdp(struct XSDP_t *xsdp)
{
    u8 sum = 0;
    u8 *bytes = (u8 *)xsdp;
    for (unsigned int i = 0; i < sizeof(struct XSDP_t); i++) {
        sum += bytes[i];
    }

    // lowest byte should be set to 0 in the checksum
    return sum == 0;
}

static void setup_acpi_v1()
{
    debug_info("Found RSDP revision %d (ACPI 1.0)\n", rsdp->revision);
    bool is_valid_rsdp = validate_rsdp(rsdp);
    if (!is_valid_rsdp) { panic("RSDP invalid.\n"); }
    char oemid[7];
    memset(oemid, 0, 7);
    memcpy(oemid, rsdp->OEMID, 6);

    debug_info("RSDP is valid. OEM identifier: %s\n", oemid);
}

static void setup_acpi_v2()
{ debug_info("Found XSDP (revision %d) for ACPI 2.0+\n", rsdp->revision); 
    xsdp = (struct XSDP_t*)rsdp;
    if(!validate_xsdp(xsdp)) panic("XSDP invalid.");

    char oemid[7];
    memset(oemid, 0, 7);
    memcpy(oemid, rsdp->OEMID, 6);
    debug_info("XSDP valid. OEM identifier: %s\n", oemid);
}

void limine_init_rsdp()
{
    uptr base = limine_get_rsdp_base();
    void *rsdp_base = get_virtaddr((void *)base);
    map_page((void *)base, rsdp_base, IS_PRESENT);
    rsdp = rsdp_base;

    if (rsdp->revision == REVISION_RSDP) {
        setup_acpi_v1();
    } else if (rsdp->revision == REVISION_XSDP) {
        setup_acpi_v2();
    }
}

/** Returns the **physical** address of the RSDT. */
uptr get_root_system_descriptor_addr() { return rsdp->revision == REVISION_RSDP ? rsdp->rsdt_address : xsdp->xsdt_address; }

u64 get_acpi_revision() {
    return rsdp->revision;
}

