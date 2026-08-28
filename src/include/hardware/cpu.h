#pragma once
#include <stdbool.h>

#define CPUID_FEAT_EDX_APIC 1 << 9

char *get_cpu_model_string();

void debug_print_cpu_model();
