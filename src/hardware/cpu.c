#include "hardware/cpu.h"
#include <cpuid.h>
#include "debug.h"
#include "string.h"

char cpu_model[13];
bool cpuid_initialized = false;

inline char* get_cpu_model_string() {
    if(cpuid_initialized) return cpu_model;
    int eax, ebx, edx, ecx;
    __cpuid(0, eax, ebx, ecx, edx);

    memcpy(cpu_model, &ebx, 4);
    memcpy(&(cpu_model[4]), &edx, 4);
    memcpy(&(cpu_model[8]), &ecx, 4);
    return cpu_model;
}

inline void debug_print_cpu_model() {
    char* model = get_cpu_model_string();
    debug_puts("CPU model: ");
    debug_puts(model);
    debug_puts("\n");
}
