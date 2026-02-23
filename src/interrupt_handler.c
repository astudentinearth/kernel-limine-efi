
#include "idt.h"
#ifdef TEST_MODE
#include "debug.h"
#endif

static inline void dump_frame(struct interrupt_frame *frame) {
    #ifdef TEST_MODE
    debug("[[[ BEGIN INTERRUPT FRAME DUMP ]]]");
    debug_puts("Interrupt frame: ");
    debug_put_hex((uint64_t)frame);
    debug_puts("\nStack segment: ");
    debug_put_hex(frame->ss);
    debug_puts("\nCode segment: ");
    debug_put_hex(frame->cs);
    debug_puts("\nFlags: ");
    debug_put_hex(frame->flags);
    debug_puts("\nInstruction pointer: ");
    debug_put_hex(frame->ip);
    debug_puts("\nStack pointer: ");
    debug_put_hex(frame->sp);
    debug_puts("\n");
    debug("[[[ END INTERRUPT FRAME DUMP ]]]");
    #endif
}

__attribute__((noreturn))
void handle_interrupt(struct interrupt_frame *frame, uint64_t interrupt_code);
void handle_interrupt(struct interrupt_frame *frame, uint64_t interrupt_code) {
#ifdef TEST_MODE
    debug_puts("[!] Interrupt - code ");
    debug_put_hex(interrupt_code);
    debug_puts("\n");
#endif
    dump_frame(frame);
    __asm__ volatile ("cli; hlt");
}

__attribute__((noreturn))
void handle_interrupt_with_error_code(struct interrupt_frame *frame, uint64_t interrupt_code, uint64_t error_code);
void handle_interrupt_with_error_code(struct interrupt_frame *frame, uint64_t interrupt_code, uint64_t error_code) {

#ifdef TEST_MODE
    debug_puts("[!] Interrupt - code ");
    debug_put_hex(interrupt_code);
    debug_puts("\n");
    debug_puts("[!] Error code provided: ");
    debug_put_hex(error_code);
    debug_puts("\n");
#endif
    dump_frame(frame);
    __asm__ volatile ("cli; hlt");
}


