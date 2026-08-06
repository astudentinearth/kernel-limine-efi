
#include "hardware/msr.h"
#include <cpuid.h>
#include "stdint.h"

const u32 CPUID_FLAG_MSR = 1 << 5;

bool cpu_has_msr()
{
    static u32 eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    return edx & CPUID_FLAG_MSR;
}

void cpu_get_msr(u32 msr, u32 *lo, u32 *hi)
{
    asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void cpu_set_msr(u32 msr, u32 lo, u32 hi)
{
    asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}
