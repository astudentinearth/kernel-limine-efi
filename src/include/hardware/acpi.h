#pragma once
#include <stdbool.h>
#include <stdint.h>

#define REVISION_RSDP 0
#define REVISION_XSDP 2

struct RSDP_t {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
} __attribute__ ((packed));

struct XSDP_t {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;      // deprecated since version 2.0

 uint32_t Length;
 uint64_t XsdtAddress;
 uint8_t ExtendedChecksum;
 uint8_t reserved[3];
} __attribute__ ((packed));

bool validate_rsdp(struct RSDP_t *rsdp);
bool validate_xsdp(struct XSDP_t *xsdp);
void limine_init_rsdp();





