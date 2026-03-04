
#include <cpuid.h>
#include "debug.h"
#include "hardware/cpu.h"
#include "hardware/pic.h"
#include "hardware/serial.h"

void disable_legacy_pic(void) {
    outb(PIC1_8259_DATA, 0xff);
    outb(PIC2_8259_DATA, 0xff);
}

bool check_apic() {
    int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    bool result = edx & CPUID_FEAT_EDX_APIC;
#ifdef TEST_MODE
    debug_printf("APIC support %s.\n", result ? "available" : "unavailable");
#endif
    return result;
}

