#pragma once
#define COM1 0x3f8
#include "stdint.h"

static inline void outb(u16 port, u8 val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline void outcom1(u8 val) {
    outb(COM1, val);
}
