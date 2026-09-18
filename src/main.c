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
#include "lock.h"
#include "paging.h"
#include "string.h"
#include "term.h"
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

    // fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = get_limine_framebuffer(0);
    init_memory_map();
    debug_info("Hello world!\n");

    debug_info("Loading GDT\n");
    setup_gdt();
    setup_idt();
    init_cpuid();
    set_framebuffer(framebuffer);
    gfx_init();
    init_pmm();
    init_paging();
    init_vmm(get_hhdm_offset(), get_cpuid()->physical_address_bits);

    limine_init_rsdp();
    init_rsdt();
    init_apic();
    setup_keyboard();
    probe_pci();
    debug_info("printing cpuid\n");
    cpuid_debug_print_info();
    pci_debug_print_devices();
    display_t display = limine_get_display(0);
    display_init(display);

    framebuffer_t *fb = NULL;
    display_acquire(&fb, 0);
    gl_clear(fb, GL_COLOR_GRAY);
    Line_t line = {
        .x = 0, .y = 24, .length = fb->width, .dir = DIRECTION_RIGHT};
    gl_draw_line(fb, GL_COLOR_BLACK, &line);
    Rect_t term_window = {.x = 16,
                    .y = 32,
                    .w = fb->width - 32,
                    .h = fb->height - 48,
                    .fill = true};
    gl_draw_rect(fb, GL_COLOR_WHITE, &term_window);
    term_window.fill = false;
    gl_draw_rect(fb, GL_COLOR_BLACK, &term_window);
    Rect_t bar = {.x = 0, .y = 0, .w = fb->width, .h = 24, .fill = true};

    gl_draw_rect(fb, GL_COLOR_WHITE, &bar);
    int i = 0;
    for (u8 ch = '0'; ch < '0' + 3; ch++) {
        gl_draw_char(fb, GL_COLOR_BLACK, 32 + (i * 8), 4, ch);
        i++;
    }

    Terminal_t *term = malloc(sizeof(Terminal_t));
    term_init(term, term_window.w, term_window.h);
    const char* message = "Lorem ipsum dolor sit amet. \nI don't use Arch anymore BTW!";
    u8 *_cur = message;

    while(*_cur != 0) {
        term_write(term, *_cur++);
    }

    term_render(term, fb, GL_COLOR_BLACK, term_window.x, term_window.y);

    display_commit(0);

#ifdef TEST_MODE
    debug("Running in test mode");
    dump_memory_info();
    run_tests();
#endif
    enable_hardware_interrupts();
    hcf();
}
