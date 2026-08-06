
#include "gdt.h"
#include "debug.h"
#include "stdint.h"

extern void set_gdt(u16 limit, u64 base);

u8 kernel_stack[RSP0_SIZE];
u8 ist1_stack[IST1_SIZE];

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

    u8 *kernel_stack_start = kernel_stack + RSP0_SIZE;
    u8 *ist1_stact_start = ist1_stack + IST1_SIZE;
    

    tss.rsp0 = (u64)kernel_stack_start;
    tss.ist1 = (u64)ist1_stact_start;

    u64 tss_base = (u64)&tss;

    ssd.access_byte = 0x89;
    ssd.base_15_0 = (u16)(tss_base & 0xFFFF);
    ssd.base_23_16 = (u8)(tss_base >> 16 & 0xFF);
    ssd.base_31_24 = (u8)(tss_base >> 24 & 0xFF);
    ssd.base_63_32 = (u32)(tss_base >> 32 & 0xFFFFFFFF);
    ssd.limit = sizeof(struct TSS) - 1;

    gdt.ssd = ssd;

    set_gdt(sizeof(struct GDT), (u64)&gdt);
    
#ifdef TEST_MODE
    debug("####\nGDT set.");
    debug_puts("Kernel stack address: 0x");
    debug_put_hex((u64)kernel_stack);
    debug_puts("\nIST1 stack address: 0x");
    debug_put_hex((u64)ist1_stack);
    debug_puts("\nGDT address: 0x");
    debug_put_hex((u64)&gdt);
    debug("\n####");
#endif

}

