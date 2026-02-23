
#include "debug.h"

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
