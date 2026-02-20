#pragma once
#include "serial.h"
#include "mem.h"
#include "math.h"
#include "ascii.h"
#include "string.h"

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

#define DEBUG_ITOA_BUFFER_SIZE 256
void debug_put_int(int64_t num);
void debug_put_hex(uint64_t num);

