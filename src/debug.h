#pragma once
#include "serial.h"

/**
 * Pushes a debug message into COM1 port. Ends it with a newline.
 */
static inline void debug(const char* msg) {
    for(const char* c = msg; *c != 0; c++) {
        outb(COM1, *c);
    }
    outb(COM1, '\n');
}


/**
 * Pushes a debug message into COM1 port, does not end it with a newline.
 */
static inline void debug_puts(const char* msg) {
    for(const char* c = msg; *c != 0; c++) {
        outb(COM1, *c);
    }
}

