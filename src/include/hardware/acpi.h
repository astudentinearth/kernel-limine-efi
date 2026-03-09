#pragma once
#include <stdbool.h>
#include <stdint.h>

#define REVISION_RSDP 0
#define REVISION_XSDP 2

struct RSDP_t {
 char signature[8];
 uint8_t checksum;
 char OEMID[6];
 uint8_t revision;
 uint32_t rsdt_address;
} __attribute__ ((packed));

struct XSDP_t {
 char signature[8];
 uint8_t checksum;
 char OEMID[6];
 uint8_t revision;
 uint32_t rsdt_address;      // deprecated since version 2.0

 uint32_t length;
 uint64_t xsdt_address;
 uint8_t extended_checksum;
 uint8_t reserved[3];
} __attribute__ ((packed));

struct ACPI_SDT {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oem_id[6];
  char oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
};

struct RSDT_t {
    struct ACPI_SDT header;
    uint32_t *other_headers_ptr;
};

uint32_t get_rsdt_entry_count();
bool validate_rsdp(const struct RSDP_t *rsdp);
bool validate_xsdp(struct XSDP_t *xsdp);
bool validate_rsdt(const struct ACPI_SDT *rsdt_header);
void limine_init_rsdp();
uintptr_t get_rsdt_address();
void init_rsdt();





