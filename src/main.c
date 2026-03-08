#include "boot/limine_requests.h"
#include "debug.h"
#include "gdt.h"
#include "gfx.h"
#include "hardware/allocator.h"
#include "hardware/memory.h"
#include "hardware/pic.h"
#include "idt.h"
#include "paging.h"
#include "test/test.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void hcf(void)
{
    for (;;) {
        asm("hlt");
    }
}

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
    check_apic();
    debug("If you didn't triple fault here congrats");

#ifdef TEST_MODE
    debug("Running in test mode");
    dump_memory_info();
    draw_char(16, 16, 'A', 0xffffffff);
    run_tests();
#endif

    init_paging();
    // We're done, just hang...
    hcf();
}
