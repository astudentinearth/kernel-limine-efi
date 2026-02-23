#pragma once
#include "math.h"
#include "ascii.h"

/**
 * Pushes a debug message into COM1 port. Ends it with a newline.
 */
void debug(const char* msg); 


/**
 * Pushes a debug message into COM1 port, does not end it with a newline.
 */
void debug_puts(const char* msg);

#define DEBUG_ITOA_BUFFER_SIZE 256
void debug_put_int(int64_t num);
void debug_put_uint(uint64_t num);
void debug_put_hex(uint64_t num);
void debug_newline();
void panic(const char* message);

