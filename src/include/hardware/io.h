#pragma once
#include <stdint.h>
void setup_keyboard();

typedef struct {
    uint64_t type;
    uint64_t payload;
} kinterrupt_t;

#define NOOP 0
#define KEYBOARD_INT 1

void process_keyboard_event(uint8_t scancode);
void queue_interrupt(kinterrupt_t interrupt);
void kern_handle_interrupt();

