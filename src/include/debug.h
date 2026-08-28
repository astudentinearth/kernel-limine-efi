#pragma once
#include "ascii.h"
#include "math.h"

/**
 * Pushes a debug message into COM1 port. Ends it with a newline.
 */
void debug(const char *msg);

/**
 * Pushes a debug message into COM1 port, does not end it with a newline.
 */
void debug_puts(const char *msg);

#define DEBUG_ITOA_BUFFER_SIZE 256
void debug_put_int(int64_t num);
void debug_put_uint(u64 num);
void debug_put_hex(u64 num);
void debug_newline();
void panic(const char *message);
void debug_printf(const char *msg, ...);
void debug_err(const char *msg, ...);
void debug_info(const char *msg, ...);
void debug_success(const char *msg, ...);
