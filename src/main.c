#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "debug.h"
#include "gfx.h"
#include "test/test.h"
#include "gdt.h"

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

inline struct limine_framebuffer* get_framebuffer(int i) {
    return framebuffer_request.response->framebuffers[i];
}

static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = get_framebuffer(0);
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
    debug("If you didn't triple fault here congrats");

    // We're done, just hang...
    hcf();
}
