#pragma once
#include <stdint.h>
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func,
                              uint8_t offset);

uint16_t pci_check_vendor(uint8_t bus, uint8_t slot);

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC
#define PCI_VENDOR_ID_OFFSET 0x0
#define PCI_DEVICE_ID_OFFSET 0x2
#define PCI_COMMAND_OFFSET 0x4
#define PCI_STATUS_OFFSET 0x6
#define PCI_REV_ID_OFFSET 0x8
#define PCI_PROG_IF_OFFSET 0x9
#define PCI_SUBCLASS_OFFSET 0xA
#define PCI_CLASS_OFFSET 0xB
#define PCI_CACHE_LINE_OFFSET 0xC
#define PCI_LATENCY_TIMER_OFFSET 0xD
#define PCI_HEADER_TYPE_OFFSET 0xE

// header 0x0
#define PCI_BAR0_OFFSET 0x10
#define PCI_BAR1_OFFSET 0x14
#define PCI_BAR2_OFFSET 0x18
#define PCI_BAR3_OFFSET 0x1C
#define PCI_BAR4_OFFSET 0x20
#define PCI_BAR5_OFFSET 0x24


// vendors
#define PCI_VENDOR_NONEXISTENT_DEVICE 0xFFFF

void probe_pci();
