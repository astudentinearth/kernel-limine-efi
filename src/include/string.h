#pragma once
#include "result.h"
#include "stdint.h"
#include <stdbool.h>
#include <stddef.h>

void itoa(int64_t num, char *buf);
void itoa_u(u64 num, char *buf);
void itoa_hex(u64 num, char *buf);
option_u64 parse_uint(const char* str);
bool str_equals(const char *a, const char *b);
void *memcpy(void *dest, const void *src, usize n);
void *memset(void *s, int c, usize n);
void *memmove(void *dest, const void *src, usize n);
int memcmp(const void *s1, const void *s2, usize n);
