
#include "hardware/pci.h"
#include "debug.h"
#include "hardware/io.h"
#include <stdint.h>

char *pci_classes[0x11] = {
    "Unclassified",
    "Mass Storage Controller",
    "Network Controller",
    "Display Controller",
    "Multimedia Controller",
    "Memory Controller",
    "Bridge",
    "Simple Communication Controller",
    "System Device",
    "Input Device Controller",
    "Docking Station",
    "Processor",
    "Serial Bus Controller",
    "Wireless Controller",
    "Intelligent Controller",
    "Satellite Communication Controller",
    "Encryption Controller",
};

char *get_pci_class_name(uint8_t class)
{
    if (class > 0x10) { return "Unknown"; }
    return pci_classes[class];
}

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func,
                              uint8_t offset)
{
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t loffset = (uint32_t)offset;

    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                         (loffset & 0xFC) | ((uint32_t)0x80000000));

    outl(PCI_CONFIG_ADDRESS, address);
    return ((uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF));
}

uint32_t pci_read_base_address(uint8_t bus, uint8_t slot, uint8_t func,
                               uint8_t offset)
{
    uint16_t low = pci_config_read_word(bus, slot, func, offset);
    uint16_t high = pci_config_read_word(bus, slot, func, offset + 2);
    uint32_t addr = ((0 | high) << 16) | low;
    return addr;
}

uint16_t pci_get_vendor(uint8_t bus, uint8_t slot, uint8_t function)
{
    return pci_config_read_word(bus, slot, function, PCI_VENDOR_ID_OFFSET);
}

uint8_t pci_get_header_type(uint8_t bus, uint8_t slot, uint8_t function)
{
    return pci_config_read_word(bus, slot, function, PCI_HEADER_TYPE_OFFSET) &
           0xFF;
}

/** low 8 bits is subclass, high 6 bits is class */
uint16_t pci_get_class_info(uint8_t bus, uint8_t slot, uint8_t function)
{
    return pci_config_read_word(bus, slot, function, PCI_SUBCLASS_OFFSET);
}

void dump_bars(uint8_t bus, uint8_t slot, uint8_t function)
{
    uint32_t bar0 = pci_read_base_address(bus, slot, function, PCI_BAR0_OFFSET);
    uint32_t bar1 = pci_read_base_address(bus, slot, function, PCI_BAR1_OFFSET);
    uint32_t bar2 = pci_read_base_address(bus, slot, function, PCI_BAR2_OFFSET);
    uint32_t bar3 = pci_read_base_address(bus, slot, function, PCI_BAR3_OFFSET);
    uint32_t bar4 = pci_read_base_address(bus, slot, function, PCI_BAR4_OFFSET);
    uint32_t bar5 = pci_read_base_address(bus, slot, function, PCI_BAR5_OFFSET);
    debug_info("pci: ^Base addresses | %x | %x | %x | %x | %x | %x\n", bar0, bar1,
               bar2, bar3, bar4, bar5);
}

void check_device(uint8_t bus, uint8_t slot)
{
    uint8_t function = 0;
    uint16_t vendor = pci_get_vendor(bus, slot, function);
    if (vendor == PCI_VENDOR_NONEXISTENT_DEVICE) { return; }
    uint8_t header_type = pci_get_header_type(bus, slot, function);
    uint16_t class_info = pci_get_class_info(bus, slot, function);
    debug_info("pci: found device with vendor %x, %s(%x), subclass %x \n",
               vendor, get_pci_class_name(class_info >> 8), class_info >> 8,
               class_info & 0xFF);
    dump_bars(bus, slot, function);
    if ((header_type & 0x80) == 0) { return; }

    for (function = 0; function < 8; function++) {
        vendor = pci_get_vendor(bus, slot, function);
        if (vendor == PCI_VENDOR_NONEXISTENT_DEVICE) { continue; }
        class_info = pci_get_class_info(bus, slot, function);
        debug_info("pci: found device with vendor %x, %s(%x), subclass %x \n",
                   vendor, get_pci_class_name(class_info >> 8), class_info >> 8,
                   class_info & 0xFF);
        dump_bars(bus, slot, function);
    }
}

void probe_pci()
{
    uint16_t bus;
    uint8_t device;

    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            check_device(bus, device);
        }
    }
}
