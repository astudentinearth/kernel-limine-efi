
#include "hardware/pci.h"
#include "debug.h"
#include "hardware/io.h"
#include "stdint.h"

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

char *get_pci_class_name(u8 class)
{
    if (class > 0x10) { return "Unknown"; }
    return pci_classes[class];
}

u16 pci_config_read_word(u8 bus, u8 slot, u8 func,
                              u8 offset)
{
    u32 address;
    u32 lbus = (u32)bus;
    u32 lslot = (u32)slot;
    u32 lfunc = (u32)func;
    u32 loffset = (u32)offset;

    address = (u32)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                         (loffset & 0xFC) | ((u32)0x80000000));

    outl(PCI_CONFIG_ADDRESS, address);
    return ((u16)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF));
}

u32 pci_read_base_address(u8 bus, u8 slot, u8 func,
                               u8 offset)
{
    u16 low = pci_config_read_word(bus, slot, func, offset);
    u16 high = pci_config_read_word(bus, slot, func, offset + 2);
    u32 addr = ((0 | high) << 16) | low;
    return addr;
}

u16 pci_get_vendor(u8 bus, u8 slot, u8 function)
{
    return pci_config_read_word(bus, slot, function, PCI_VENDOR_ID_OFFSET);
}

u8 pci_get_header_type(u8 bus, u8 slot, u8 function)
{
    return pci_config_read_word(bus, slot, function, PCI_HEADER_TYPE_OFFSET) &
           0xFF;
}

/** low 8 bits is subclass, high 6 bits is class */
u16 pci_get_class_info(u8 bus, u8 slot, u8 function)
{
    return pci_config_read_word(bus, slot, function, PCI_SUBCLASS_OFFSET);
}

void dump_bars(u8 bus, u8 slot, u8 function)
{
    u32 bar0 = pci_read_base_address(bus, slot, function, PCI_BAR0_OFFSET);
    u32 bar1 = pci_read_base_address(bus, slot, function, PCI_BAR1_OFFSET);
    u32 bar2 = pci_read_base_address(bus, slot, function, PCI_BAR2_OFFSET);
    u32 bar3 = pci_read_base_address(bus, slot, function, PCI_BAR3_OFFSET);
    u32 bar4 = pci_read_base_address(bus, slot, function, PCI_BAR4_OFFSET);
    u32 bar5 = pci_read_base_address(bus, slot, function, PCI_BAR5_OFFSET);
    debug_info("pci: ^Base addresses | %x | %x | %x | %x | %x | %x\n", bar0, bar1,
               bar2, bar3, bar4, bar5);
}

void check_device(u8 bus, u8 slot)
{
    u8 function = 0;
    u16 vendor = pci_get_vendor(bus, slot, function);
    if (vendor == PCI_VENDOR_NONEXISTENT_DEVICE) { return; }
    u8 header_type = pci_get_header_type(bus, slot, function);
    u16 class_info = pci_get_class_info(bus, slot, function);
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
    u16 bus;
    u8 device;

    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            check_device(bus, device);
        }
    }
}
