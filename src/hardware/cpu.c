#include "hardware/cpu.h"
#include "debug.h"
#include "string.h"
#include <cpuid.h>

cpuid_t cpuid = {};
bool cpuid_initialized = false;

void cpuid_init_cpu_model() {
    int eax, ebx, edx, ecx;
    __cpuid(0, eax, ebx, ecx, edx);

    memcpy(cpuid.cpu_model, &ebx, 4);
    memcpy(&(cpuid.cpu_model[4]), &edx, 4);
    memcpy(&(cpuid.cpu_model[8]), &ecx, 4);
}

void cpuid_init_address_bits() {
    int eax, ebx, ecx, edx;
    __cpuid(CPUID_LEAF_ADDRESS_SIZE, eax, ebx, ecx, edx);
    cpuid.physical_address_bits = eax & 0xFF;
}

void init_cpuid() {
    if(cpuid_initialized) return;
    cpuid_init_cpu_model();
    cpuid_init_address_bits();
    cpuid_initialized = true;
}

const char *get_cpu_model_string()
{
    return cpuid.cpu_model;
}

const cpuid_t *get_cpuid() { return &cpuid; }

inline void cpuid_debug_print_info()
{
    debug_info("CPU Information:\n     -- Model name: %s\n     -- Physical address bits: %d\n", cpuid.cpu_model, cpuid.physical_address_bits);
}
