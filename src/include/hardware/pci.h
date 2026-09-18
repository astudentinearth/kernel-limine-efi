#pragma once
#include "stdint.h"
u16 pci_config_read_word(u8 bus, u8 slot, u8 func, u8 offset);

u16 pci_check_vendor(u8 bus, u8 slot);

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
void pci_debug_print_devices();

typedef struct {
    u16 vendor;
    u8 bus;
    u8 slot;
    u8 function;
    uptr bar0;
    uptr bar1;
    uptr bar2;
    uptr bar3;
    uptr bar4;
    uptr bar5;
    u8 class;
    u8 subclass;
    u8 prog_if;
} PCIDevice;
