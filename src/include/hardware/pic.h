#pragma once
#include "stdint.h"
#include <stdbool.h>
#define PIC1_8259_COMMAND 0x0020
#define PIC1_8259_DATA 0x0021
#define PIC2_8259_COMMAND 0x00A0
#define PIC2_8259_DATA 0x00A1

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

/** Offset of the spurious interrupt vector register from the APIC base */
#define APIC_SIVR_OFFSET 0xF0

void disable_legacy_pic(void);
bool check_apic(void);
void init_apic();

#define IOAPIC_DELIVERY_MODE_FIXED 0
#define IOAPIC_DELIVERY_MODE_LOW 1
#define IOAPIC_DELIVERY_MODE_SMI 2
#define IOAPIC_DELIVERY_MODE_NMI 4
#define IOAPIC_DELIVERY_MODE_INIT 5
#define IOAPIC_DELIVERY_MODE_EXTINIT 7

#define IOAPIC_DESTINATION_MODE_PHYSICAL 0
#define IOAPIC_DESTINATION_MODE_LOGICAL 1

#define IOAPICID 0x00
#define IOAPICVER 0x01
#define IOAPICARB 0x02
#define IOAPICREDTBL(n)                                                        \
    (0x10 + 2 * n) // lower-32bits (add +1 for upper 32-bits)

struct io_apic_redirection_entry {
    u64 vector : 8;
    u64 delivery_mode : 3;
    u64 destination_mode : 1;
    u64 delivery_status : 1;
    u64 pin_polarity : 1;
    u64 remote_irr : 1;
    u64 trigger_mode : 1;
    u64 mask : 1;
    u64 reserved : 39;
    u64 destination : 8;
} __attribute__((packed));

typedef struct {
    u32 lower;
    u32 upper;
} io_apic_redirection_entry_t;

typedef struct {
    uptr phys_addr;
    uptr virt_addr;
    u8 id;
    u64 global_interrupt_base;
    u8 max_redir_entry_count;
} io_apic_t;

void setup_ioapic(io_apic_t *apic, void *phys_addr, u64 gsib);
u32 read_ioapic_reg(io_apic_t apic, u8 offset);
void write_ioapic_reg(io_apic_t apic, u8 offset, u32 data);

void read_ioapic_redir_entry(io_apic_t apic, u8 n,
                             io_apic_redirection_entry_t *out);

void write_ioapic_redir_entry(io_apic_t apic, u8 n,
                              io_apic_redirection_entry_t entry);

io_apic_t *get_default_ioapic();
void lapic_eoi();

#define KEYBOARD_IRQ 1
