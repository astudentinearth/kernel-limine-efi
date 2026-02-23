
#include "debug.h"
#include "hardware/serial.h"
#include "mem.h"
#include "string.h"

void debug(const char* msg) {
    debug_puts(msg);
    outb(COM1, '\n');
}

void debug_puts(const char* msg) {
    for(const char* c = msg; *c != 0; c++) {
        outb(COM1, *c);
    }
}

void debug_put_int(int64_t num) {
    char debug_itoa_buffer[DEBUG_ITOA_BUFFER_SIZE];
    memset(debug_itoa_buffer, 0, DEBUG_ITOA_BUFFER_SIZE);
    itoa(num, debug_itoa_buffer);
    debug_puts(debug_itoa_buffer);
}

void debug_put_uint(uint64_t num) {
    char debug_itoa_buffer[DEBUG_ITOA_BUFFER_SIZE];
    memset(debug_itoa_buffer, 0, DEBUG_ITOA_BUFFER_SIZE);
    itoa_u(num, debug_itoa_buffer);
    debug_puts(debug_itoa_buffer);
}

void debug_put_hex(uint64_t num) {
    char debug_itoa_buffer[DEBUG_ITOA_BUFFER_SIZE];
    memset(debug_itoa_buffer, 0, DEBUG_ITOA_BUFFER_SIZE);
    itoa_hex(num, debug_itoa_buffer);
    debug_puts("0x");
    debug_puts(debug_itoa_buffer);
}

void debug_newline() {
    debug_puts("\n");
}

