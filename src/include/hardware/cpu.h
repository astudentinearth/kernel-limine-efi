#pragma once
#include <stdbool.h>

extern char cpu_model[13];
extern bool cpuid_initialized;

#define CPUID_FEAT_EDX_APIC 1 << 9

char* get_cpu_model_string();

void debug_print_cpu_model(); 

