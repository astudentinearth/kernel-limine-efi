#pragma once
#include "serial.h"
#include "mem.h"
#include "math.h"
#include "ascii.h"

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

// temporary buffer to print out numbers in debug
static char debug_itoa_buffer[DEBUG_ITOA_BUFFER_SIZE];

static inline void debug_reset_itoa_buffer() {
    memset(debug_itoa_buffer, 0, DEBUG_ITOA_BUFFER_SIZE);
}

static inline void debug_itoa(int64_t num) {
    debug_reset_itoa_buffer();
    
    if(num == 0) {
        debug_itoa_buffer[0] = '0';
        return;
    }

    int64_t value = abs(num);
    int digit_count = 0;
    char* cur = &debug_itoa_buffer[DEBUG_ITOA_BUFFER_SIZE - 1];

    // convert to digits in reverse
    while(value > 0) {
        int64_t remainder = value % 10;
        value /= 10;
        *cur = digit_to_ascii(remainder);
        cur--;
        digit_count++;
    }

    if(num > 0) {
        memcpy(debug_itoa_buffer, ++cur, digit_count);
    }
    else {
        memcpy(&debug_itoa_buffer[1], ++cur, digit_count);
        debug_itoa_buffer[0] = '-';
    }
    
} 

static inline void debug_put_int(int64_t num) {
    debug_itoa(num);
    debug_puts(debug_itoa_buffer);
}

