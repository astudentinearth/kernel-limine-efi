#pragma once
#include "stdint.h"

extern void outl(u32 port, u32 value);
extern u32 inl(u32 port);
void setup_keyboard();

typedef struct {
    u64 type;
    u64 payload;
} kinterrupt_t;

#define NOOP 0
#define KEYBOARD_INT 1

void process_keyboard_event(u8 scancode);
void queue_interrupt(kinterrupt_t interrupt);
void kern_handle_interrupt();

