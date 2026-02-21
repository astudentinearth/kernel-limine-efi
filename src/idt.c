
#include "debug.h"
#include "idt.h"
#include "gdt.h"
#include <stdbool.h>

extern void load_idt(uint16_t size, uint64_t offset);
extern void *isr_stub_table[];

static bool vectors[IDT_MAX_DESCRIPTORS];

__attribute__((aligned(0x10))) static struct InterruptDescriptor64 idt[255];

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
  struct InterruptDescriptor64 *descriptor = &idt[vector];

  descriptor->offset_15_0 = (uint64_t)isr & 0xFFFF;
  descriptor->segment_selector = KERNEL_CODE_SEGMENT_OFFSET;
  descriptor->ist_offset = 0;
  descriptor->type_attrs = flags;
  descriptor->offset_31_16 = ((uint64_t)isr >> 16) & 0xFFFF;
  descriptor->offset_63_32 = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
  descriptor->reserved_zero = 0;
}

void setup_idt() {
#ifdef TEST_MODE
    debug("Setting up IDT");
#endif
  uint16_t limit = sizeof(struct InterruptDescriptor64) * IDT_MAX_DESCRIPTORS;
  uint64_t base = (uint64_t)&idt[0];

  for (uint8_t vector = 0; vector < 32; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    vectors[vector] = true;
  }

  load_idt(limit, base);
#ifdef TEST_MODE
    debug("IDT set");
    debug_put_int(limit / 0);
#endif
}
