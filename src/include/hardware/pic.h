#pragma once
#include <stdbool.h>
#include <stdint.h>
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

#define IOAPIC_DELIVERY_MODE_EDGE 0
#define IOAPIC_DELIVERY_MODE_LEVEL 1

#define IOAPIC_DESTINATION_MODE_PHYSICAL 0
#define IOAPIC_DESTINATION_MODE_LOGICAL 1

#define IOAPICID 0x00
#define IOAPICVER 0x01
#define IOAPICARB 0x02
#define IOAPICREDTBL(n)                                                        \
    (0x10 + 2 * n) // lower-32bits (add +1 for upper 32-bits)

struct io_apic_redirection_entry {
    uint64_t vector : 8;
    uint64_t delivery_mode : 3;
    uint64_t destination_mode : 1;
    uint64_t delivery_status : 1;
    uint64_t pin_polarity : 1;
    uint64_t remote_irr : 1;
    uint64_t trigger_mode : 1;
    uint64_t mask : 1;
    uint64_t reserved : 39;
    uint64_t destination : 8;
} __attribute__((packed));

typedef struct {
    uint32_t lower;
    uint32_t upper;
} io_apic_redirection_entry_t;

typedef struct {
    uintptr_t phys_addr;
    uintptr_t virt_addr;
    uint8_t id;
    uint64_t global_interrupt_base;
    uint8_t max_redir_entry_count;
} io_apic_t;

void setup_ioapic(io_apic_t *apic, void *phys_addr, uint64_t gsib);
uint32_t read_ioapic_reg(io_apic_t apic, uint8_t offset);
void write_ioapic_reg(io_apic_t apic, uint8_t offset, uint32_t data);

void read_ioapic_redir_entry(io_apic_t apic, uint8_t n,
                             io_apic_redirection_entry_t *out);

void write_ioapic_redir_entry(io_apic_t apic, uint8_t n,
                              io_apic_redirection_entry_t entry);

io_apic_t *get_default_ioapic();
