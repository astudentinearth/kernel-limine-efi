#pragma once
#define COM1 0x3f8
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

inline void outcom1(uint8_t val) {
    outb(COM1, val);
}
