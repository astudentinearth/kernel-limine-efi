#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "debug.h"
#include "gfx.h"
#include "test/test.h"
#include "gdt.h"
#include "idt.h"
#include "boot/limine_requests.h"
#include "hardware/memory.h"

static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

void kmain(void) {
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
    debug("If you didn't triple fault here congrats");


    #ifdef TEST_MODE
    debug("Running in test mode");
    run_tests();
    dump_memory_info();
    draw_char(0, 0, 'A', 0xffffffff);
    #endif


    // We're done, just hang...
    hcf();
}
