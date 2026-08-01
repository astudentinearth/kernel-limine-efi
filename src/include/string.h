#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void itoa(int64_t num, char* buf);
void itoa_u(uint64_t num, char* buf);
void itoa_hex(uint64_t num, char* buf);
bool str_equals(const char* a, const char* b);
void *memcpy(void *dest, const void *src, size_t n); 
void *memset(void *s, int c, size_t n); 
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n); 
