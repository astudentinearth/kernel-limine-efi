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
} __attribute__((packed));

struct XSDP_t {
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision;
    uint32_t rsdt_address; // deprecated since version 2.0

    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed));

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
} __attribute__((packed));

struct RSDT_t {
    struct ACPI_SDT header;
    uint32_t other_headers_ptr[];
} __attribute__((packed));

/** virtual addresses to rsdt table entries */
struct system_tables {
    struct ACPI_SDT *facp;
    struct ACPI_SDT *apic;
    struct ACPI_SDT *hpet;
    struct ACPI_SDT *waet;
};

uint32_t get_rsdt_entry_count();
bool validate_rsdp(const struct RSDP_t *rsdp);
bool validate_xsdp(struct XSDP_t *xsdp);
bool validate_rsdt(const struct ACPI_SDT *rsdt_header);
void limine_init_rsdp();
uintptr_t get_rsdt_address();
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
    uint8_t entry_type;
    uint8_t record_length;
} __attribute__((packed));

/** These fields come right after the default ACPI header in MADT */
struct MADT_pic_info {
    uint32_t local_apic_address;
    uint32_t flags;
} __attribute__((packed));

#define MADT_FLAG_LEGACY_PIC_INSTALLED 0x1

struct MADT_local_apic_entry {
    uint8_t entry_type;
    uint8_t record_length;
    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t flags;
}__attribute__((packed));

struct MADT_io_apic_entry {
    uint8_t entry_type;
    uint8_t record_length;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_phys_addr;
    uint32_t global_system_interrupt_base;
}__attribute__((packed));

