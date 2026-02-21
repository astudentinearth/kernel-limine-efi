#pragma once

#include <stdint.h>

typedef unsigned long long int uword_t;

#define IDT_MAX_DESCRIPTORS 32

struct InterruptDescriptor64 {
    uint16_t offset_15_0;
    uint16_t segment_selector;
    uint8_t ist_offset;
    uint8_t type_attrs;
    uint16_t offset_31_16;
    uint32_t offset_63_32;
    uint32_t reserved_zero;
}__attribute__((packed));

struct interrupt_frame {
    uword_t ip;
    uword_t cs;
    uword_t flags;
    uword_t sp;
    uword_t ss;
};

struct IDT {
    struct InterruptDescriptor64 divide_error;
    struct InterruptDescriptor64 debug_exception;
    struct InterruptDescriptor64 nmi_interrupt;
    struct InterruptDescriptor64 breakpoint;
    struct InterruptDescriptor64 overflow;
    struct InterruptDescriptor64 bound_range_exceeded;
    struct InterruptDescriptor64 invalid_opcode;
    struct InterruptDescriptor64 device_not_available;
    struct InterruptDescriptor64 double_fault;
    struct InterruptDescriptor64 coprocessor_segment_overrun;
    struct InterruptDescriptor64 invalid_tss;
    struct InterruptDescriptor64 segment_not_present;
    struct InterruptDescriptor64 stack_segment_fault;
    struct InterruptDescriptor64 general_protection_fault;
    struct InterruptDescriptor64 page_fault;
}__attribute__((packed));

void setup_idt();


