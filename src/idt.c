
#include "idt.h"
#include "debug.h"
#include "gdt.h"
#include <stdbool.h>

extern void load_idt(u16 size, u64 offset);
extern void *isr_stub_table[];
extern void trigger_gp();

static bool vectors[IDT_MAX_DESCRIPTORS];

__attribute__((aligned(
    0x10))) static struct InterruptDescriptor64 idt[IDT_MAX_DESCRIPTORS];

void idt_set_descriptor(u8 vector, void *isr, u8 flags)
{
    struct InterruptDescriptor64 *descriptor = &idt[vector];

    descriptor->offset_15_0 = (u64)isr & 0xFFFF;
    descriptor->segment_selector = KERNEL_CODE_SEGMENT_OFFSET;
    descriptor->ist_offset = 0;
    descriptor->type_attrs = flags;
    descriptor->offset_31_16 = ((u64)isr >> 16) & 0xFFFF;
    descriptor->offset_63_32 = ((u64)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved_zero = 0;
}

void setup_idt()
{
#ifdef TEST_MODE
    debug_info("Setting up IDT");
#endif
    u16 limit = sizeof(struct InterruptDescriptor64) * IDT_MAX_DESCRIPTORS - 1;
    u64 base = (u64)&idt[0];

    for (u16 vector = 0; vector < IDT_MAX_DESCRIPTORS; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    load_idt(limit, base);
#ifdef TEST_MODE
    debug_info("IDT set");
#endif
}
