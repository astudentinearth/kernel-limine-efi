#pragma once
#include <stdbool.h>
#define PIC1_8259_COMMAND 0x0020
#define PIC1_8259_DATA 0x0021
#define PIC2_8259_COMMAND 0x00A0
#define PIC2_8259_DATA 0x00A1

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

/** Offset of the spurious interrupt vector register from the APIC base */
#define APIC_SIVR_OFFSET 0xF0

void disable_legacy_pic(void);
bool check_apic(void);
void init_apic();
