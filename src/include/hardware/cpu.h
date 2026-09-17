#pragma once
#include "stdint.h"
#include <stdbool.h>

#define CPUID_FEAT_EDX_APIC 1 << 9
#define CPUID_LEAF_ADDRESS_SIZE 0x80000008

typedef struct cpuid_t {
    char cpu_model[13];
    usize physical_address_bits;
} cpuid_t;

const char *get_cpu_model_string();
void init_cpuid();
void cpuid_debug_print_info();
const cpuid_t *get_cpuid();
