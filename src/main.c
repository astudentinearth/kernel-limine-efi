#include "boot/limine_requests.h"
#include "debug.h"
#include "gdt.h"
#include "gfx.h"
#include "hardware/acpi.h"
#include "hardware/allocator.h"
#include "hardware/memory.h"
#include "hardware/pci.h"
#include "hardware/pic.h"
#include "idt.h"
#include "paging.h"
#include "test/test.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "hardware/io.h"

static void hcf(void)
{
    asm("hlt");
    for (;;) {
        kern_handle_interrupt();
        asm("hlt");
    }
}

extern void enable_hardware_interrupts();
void kmain(void)
{
    // Ensure the bootloader actually understands our base revision (see spec).
    if (!is_base_revision_supported() || !limine_framebuffer_available()) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = get_limine_framebuffer(0);
    init_memory_map();
    debug("Hello world!");

    debug("!!! Loading GDT");
    setup_gdt();
    setup_idt();
    set_framebuffer(framebuffer);
    gfx_init();
    init_pmm();
    debug("If you didn't triple fault here congrats");

#ifdef TEST_MODE
    debug("Running in test mode");
    dump_memory_info();
    draw_char(16, 16, 'A', 0xffffffff);
    run_tests();
#endif

    init_paging();
    limine_init_rsdp();
    init_rsdt();
    init_apic();
    setup_keyboard();
    enable_hardware_interrupts();
    probe_pci();
    // We're done, just hang...
    hcf();
}
