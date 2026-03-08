
#include "debug.h"
#include "hardware/serial.h"
#include "mem.h"
#include "term.h"
#include "string.h"
#include <stdarg.h>

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

void panic(const char *message) {
    debug_err("KERNEL PANIC");
    debug_err(message);
    asm ("cli; hlt");
}

void debug_printf_out(const char* msg, va_list args) {
    for(;*msg != 0;msg++) {
        char ch = *msg;
        if(ch == '%') {
            char fmt = *(++msg);
            if(fmt == 0) return;
            switch(fmt) {
                case 'i':
                case 'd':
                    debug_put_int(va_arg(args, uint32_t));
                    continue;

                case 'u':
                    debug_put_uint(va_arg(args, uint64_t));
                    continue;

                case 'p':
                case 'x': //TODO: implement lowercase later
                case 'X':
                    debug_put_hex(va_arg(args, uint64_t));
                    continue;

                case 'c':
                    outb(COM1, va_arg(args, int));
                    continue;

                case 's':
                    debug_puts(va_arg(args, char*));
                    continue;

                default:
                case '%': // handle the %% case
                    outb(COM1, '%');
                    continue;

            }
        }
        outb(COM1, ch);
    }
}

#define LOG_INFO BG_BLUE FG_BLACK FG_BOLD "INFO" BG_DEFAULT FG_DEFAULT FG_RESET_BOLD " "
#define LOG_ERR BG_RED FG_BLACK FG_BOLD "ERROR" BG_DEFAULT FG_RED FG_RESET_BOLD " "

void debug_printf(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    debug_printf_out(msg, args);
    va_end(args);
}

void debug_info(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    debug_printf(LOG_INFO);
    debug_printf_out(msg, args);
    debug_puts(FG_DEFAULT);
    va_end(args);
}

void debug_err(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    debug_printf(LOG_ERR);
    debug_printf_out(msg, args);
    debug_puts(FG_DEFAULT);
    va_end(args);
}

void debug_success(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    debug_puts(FG_GREEN);
    debug_printf_out(msg, args);
    debug_puts(FG_DEFAULT);
    va_end(args);
}

