
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

#define MOD "\0"
#define US_QWERTY MOD MOD "!@#$%^&*()_+\b\tQWERTYUIOP{}\n" MOD "ASDFGHJKL;'`" MOD "|ZXCVBNM,./" MOD "*" MOD " " 
#define US_QWERTY_LOWERCASE MOD MOD "1234567890-=\b\tqwertyuiop[]\n" MOD "asdfghjkl:\"`" MOD "\\zxcvbnm<>?" MOD "*" MOD " " 
char us_qwerty[256] = US_QWERTY;
char us_qwerty_low[256] = US_QWERTY_LOWERCASE;

bool shift_down = false;
extern uint8_t ps2_kbd_scancode_cmd(uint8_t subcommand);

void process_keyboard_event(uint8_t scancode) {
    char ch = shift_down ? us_qwerty[scancode] : us_qwerty_low[scancode];
    if(scancode == 0x36 || scancode == 0x2A) {
        shift_down = true;
    } 
    else if(scancode == 0xAA || scancode == 0xB6) {
        shift_down = false;
    }
    else if(ch != '\0') {
        if(ch == '\b') {
            debug_printf("\b \b");
            return;
        }
        debug_printf("%c", ch);
    }
}

void keyboard_interrupt(uint8_t scancode) {
    kinterrupt_t i = {
        .type = KEYBOARD_INT,
        .payload = scancode
    };
    queue_interrupt(i);
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
    uint8_t set_response = ps2_kbd_scancode_cmd(1);
    uint8_t b = ps2_kbd_scancode_cmd(0);
    debug_info("%x %x\n", set_response,b);
}

