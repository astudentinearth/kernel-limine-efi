#pragma once
#include <stdbool.h>
#include "stdint.h"

#define REVISION_RSDP 0
#define REVISION_XSDP 2

struct RSDP_t {
    char signature[8];
    u8 checksum;
    char OEMID[6];
    u8 revision;
    u32 rsdt_address;
} __attribute__((packed));

struct XSDP_t {
    char signature[8];
    u8 checksum;
    char OEMID[6];
    u8 revision;
    u32 rsdt_address; // deprecated since version 2.0

    u32 length;
    u64 xsdt_address;
    u8 extended_checksum;
    u8 reserved[3];
} __attribute__((packed));

struct ACPI_SDT {
    char signature[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
} __attribute__((packed));

struct RSDT_t {
    struct ACPI_SDT header;
    u32 other_headers_ptr[];
} __attribute__((packed));

/** virtual addresses to rsdt table entries */
struct system_tables {
    struct ACPI_SDT *facp;
    struct ACPI_SDT *apic;
    struct ACPI_SDT *hpet;
    struct ACPI_SDT *waet;
};

u32 get_rsdt_entry_count();
bool validate_rsdp(const struct RSDP_t *rsdp);
bool validate_xsdp(struct XSDP_t *xsdp);
bool validate_rsdt(const struct ACPI_SDT *rsdt_header);
void limine_init_rsdp();
uptr get_rsdt_address();
void init_rsdt();
struct system_tables *get_acpi_tables();

// MADT entry types
#define MADT_ENTRY_LOCAL_APIC 0
#define MADT_ENTRY_IO_APIC 1
#define MADT_ENTRY_IO_APIC_INT_SRC_OVERRIDE 2
#define MADT_ENTRY_IO_APIC_NMI_SRC 3
#define MADT_ENTRY_LOCAL_APIC_NMI 4
#define MADT_ENTRY_LOCAL_APIC_ADDR_OVERRIDE 5
#define MADT_ENTRY_PROCESSOR_LOCALx2_APIC 9

struct MADT_entry_header {
    u8 entry_type;
    u8 record_length;
} __attribute__((packed));

/** These fields come right after the default ACPI header in MADT */
struct MADT_pic_info {
    u32 local_apic_address;
    u32 flags;
} __attribute__((packed));

#define MADT_FLAG_LEGACY_PIC_INSTALLED 0x1

struct MADT_local_apic_entry {
    u8 entry_type;
    u8 record_length;
    u8 acpi_processor_id;
    u8 apic_id;
    u32 flags;
}__attribute__((packed));

struct MADT_io_apic_entry {
    u8 entry_type;
    u8 record_length;
    u8 io_apic_id;
    u8 reserved;
    u32 io_apic_phys_addr;
    u32 global_system_interrupt_base;
}__attribute__((packed));

