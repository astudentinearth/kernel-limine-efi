
#include "gdt.h"
#include <stdint.h>

extern void set_gdt(uint16_t limit, uint64_t base);

uint8_t kernel_stack[RSP0_SIZE];
uint8_t ist1_stack[IST1_SIZE];

static struct GDT gdt;
static struct TSS tss;
static struct system_segment_descriptor ssd;

void setup_gdt() {
    gdt.kernel_cs = KERNEL_CS;
    gdt.kernel_data = KERNEL_DATA;
    gdt.user_data = USER_DATA;
    gdt.user_code = USER_CODE;
    gdt.null_descriptor = NULL_DESCRIPTOR;


    tss.iopb = sizeof(struct TSS);

    uint8_t *kernel_stack_start = kernel_stack_start + RSP0_SIZE;
    uint8_t *ist1_stact_start = ist1_stack + IST1_SIZE;
    

    tss.rsp0 = (uint64_t)kernel_stack_start;
    tss.ist1 = (uint64_t)ist1_stact_start;

    uint64_t tss_base = (uint64_t)&tss;

    ssd.access_byte = 0x89;
    ssd.base_15_0 = (uint16_t)(tss_base & 0xFFFF);
    ssd.base_23_16 = (uint8_t)(tss_base >> 16 & 0xFF);
    ssd.base_31_24 = (uint8_t)(tss_base >> 24 & 0xFF);
    ssd.base_63_32 = (uint32_t)(tss_base >> 32 & 0xFFFFFFFF);
    ssd.limit = sizeof(struct TSS) - 1;

    gdt.ssd = ssd;

    set_gdt(sizeof(struct GDT), (uint64_t)&gdt);
}

