
#include "idt.h"
#include "string.h"
#ifdef TEST_MODE
#include "debug.h"
#endif

static inline void dump_frame(struct interrupt_frame *frame)
{
#ifdef TEST_MODE
    debug("[[[ BEGIN INTERRUPT FRAME DUMP ]]]");
    debug_puts("Interrupt frame: ");
    debug_put_hex((u64)frame);
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

__attribute__((noreturn)) void handle_interrupt(struct interrupt_frame *frame,
                                                u64 interrupt_code);
void handle_interrupt(struct interrupt_frame *frame, u64 interrupt_code)
{
#ifdef TEST_MODE
    debug_puts("[!] Interrupt - code ");
    debug_put_hex(interrupt_code);
    debug_puts("\n");
#endif
    dump_frame(frame);
    for (;;) {
        __asm__ volatile("cli; hlt" ::: "memory");
    }
}

__attribute__((noreturn)) void
handle_interrupt_with_error_code(struct interrupt_frame *frame,
                                 u64 interrupt_code, u64 error_code);
void handle_interrupt_with_error_code(struct interrupt_frame *frame,
                                      u64 interrupt_code, u64 error_code)
{

#ifdef TEST_MODE
    debug_setup_tty_out(NULL, NULL);
    debug_puts("[!] Interrupt - code ");
    debug_put_hex(interrupt_code);
    debug_puts("\n");
    debug_puts("[!] Error code provided: ");
    debug_put_hex(error_code);
    debug_puts("\n");
    dump_frame(frame);
#endif
    for (;;) {
        __asm__ volatile("cli; hlt" ::: "memory");
    }
}
