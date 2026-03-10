
#include "debug.h"
#include "hardware/io.h"
#include "hardware/pic.h"
#include "idt.h"

static struct io_apic_redirection_entry keyboard_redir_entry = {
    .vector = INT_KEYBOARD,
    .delivery_mode = IOAPIC_DELIVERY_MODE_FIXED,
    .destination_mode = IOAPIC_DESTINATION_MODE_PHYSICAL,
    .trigger_mode = 0,
    .pin_polarity = 0,
    .mask = 0,
    .destination = 0
};

void keyboard_interrupt(uint8_t scancode) {
    debug_info("KEYPRESS: %d\n", scancode);
    lapic_eoi();
}

void setup_keyboard() {
    debug_info("Setting up keyboard\n");
    io_apic_t *apic = get_default_ioapic();
    io_apic_redirection_entry_t *entry = (io_apic_redirection_entry_t*)&keyboard_redir_entry;
    write_ioapic_redir_entry(*apic, KEYBOARD_IRQ, *entry);
    io_apic_redirection_entry_t readback;
read_ioapic_redir_entry(*apic, KEYBOARD_IRQ, &readback);
debug_info("redir low: %x high: %x\n", readback.lower, readback.upper);
    debug_info("Set keyboard up!\n");
}

