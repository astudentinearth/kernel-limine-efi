#pragma once

#include "stdint.h"

typedef unsigned long long int uword_t;

#define IDT_MAX_DESCRIPTORS 256

struct InterruptDescriptor64 {
    u16 offset_15_0;
    u16 segment_selector;
    u8 ist_offset;
    u8 type_attrs;
    u16 offset_31_16;
    u32 offset_63_32;
    u32 reserved_zero;
}__attribute__((packed));

struct interrupt_frame {
    uword_t ip;
    uword_t cs;
    uword_t flags;
    uword_t sp;
    uword_t ss;
};


void setup_idt();

#define INT_DIVIDE_ERROR 0X00
#define INT_DEBUG_EXCEPTION 0X01
#define INT_NMI_INTERRUPT 0X02
#define INT_BREAKPOINT 0X03
#define INT_OVERFLOW 0X04
#define INT_BOUND 0X05
#define INT_INVALID_OPCODE 0X06
#define INT_DEVICE_NOT_AVAILABLE 0X07
#define INT_DOUBLE_FAULT 0X08
#define INT_COPROCESSOR_SEGMENT_OVERRUN 0x09
#define INT_INVALID_TSS 0X0A
#define INT_SEGMENT_NOT_PRESENT 0X0B
#define INT_STACK_SEGMENT_FAULT 0X0C
#define INT_GENERAL_PROTECTION_FAULT 0X0D
#define INT_PAGE_FAULT 0x0E
#define INT_KEYBOARD 0x21


