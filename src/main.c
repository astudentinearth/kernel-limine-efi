#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "debug.h"
#include "gfx.h"
#include "test/test.h"
#include "gdt.h"
#include "idt.h"
#include "boot/limine_requests.h"

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
    set_framebuffer(framebuffer);
    gfx_init();
    debug("Hello world!");
    draw_char(0, 0, 'A', 0xffffffff);

    #ifdef TEST_MODE
    debug("Running in test mode");
    run_tests();
    #endif

    debug("!!! Loading GDT");
    setup_gdt();
    setup_idt();
    debug("If you didn't triple fault here congrats");

    // We're done, just hang...
    hcf();
}
