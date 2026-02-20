#pragma once
#include <stdint.h>
#include <stdbool.h>

void itoa(int64_t num, char* buf);
void itoa_hex(uint64_t num, char* buf);
bool str_equals(const char* a, const char* b);

