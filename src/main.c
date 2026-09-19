#include "boot/limine_requests.h"
#include "debug.h"
#include "gdt.h"
#include "gfx.h"
#include "hardware/acpi.h"
#include "hardware/allocator.h"
#include "hardware/cpu.h"
#include "hardware/display.h"
#include "hardware/io.h"
#include "hardware/memory.h"
#include "hardware/pci.h"
#include "hardware/pic.h"
#include "idt.h"
#include "paging.h"
#include "string.h"
#include "test/test.h"
#include "vmm.h"
#include <stdbool.h>
#include <stddef.h>

extern u8 _bss_start[];
extern u8 _bss_end[];

static void hcf(void)
{
    asm("hlt");
    for (;;) {
        kern_handle_interrupt();
        asm("hlt");
    }
}

extern void enable_hardware_interrupts();
extern u64 get_interrupt_flag();

void kmain(void)
{
    memset(_bss_start, 0, (uptr)_bss_end - (uptr)_bss_start);
    // Ensure the bootloader actually understands our base revision (see spec).
    if (!is_base_revision_supported() || !limine_framebuffer_available()) {
        hcf();
    }

    // no interrupts during bootstrap
    __asm__ volatile("cli");

    init_memory_map();
    debug_info("Hello world!\n");

    debug_info("Loading GDT\n");
    setup_gdt();
    setup_idt();
    init_cpuid();
    gfx_init();
    init_pmm();
    init_paging();
    init_vmm(get_hhdm_offset(), get_cpuid()->physical_address_bits);

        display_t display = limine_get_display(0);
    display_init(display);

#ifdef TEST_MODE
    debug("Running in test mode");
    dump_memory_info();
    run_tests();
#endif

    kern_init_tty();
    limine_init_rsdp();
    init_rsdt();
    init_apic();
    setup_keyboard();
    probe_pci();
    cpuid_debug_print_info();
    pci_debug_print_devices();


    enable_hardware_interrupts();
    hcf();
}
