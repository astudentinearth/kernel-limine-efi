
#include "hardware/io.h"
#include "debug.h"
#include "gfx.h"
#include "hardware/allocator.h"
#include "libk.h"
#include "lock.h"
#include "term.h"

#define QUEUE_SIZE 512
#define TTY_DISPLAY 0
static kinterrupt_t queue[QUEUE_SIZE];
static volatile u64 head = 0;
static volatile u64 tail = 0;
kinterrupt_t noop = {.payload = NOOP, .type = NOOP};

framebuffer_t *fb = NULL;
Terminal_t *term = NULL;

void queue_interrupt(kinterrupt_t interrupt)
{
    _no_interrupts u64 next = (head + 1) % QUEUE_SIZE;
    if (next == tail) { return; }
    queue[head] = interrupt;
    head = next;
}

void kern_render_tty()
{
    u32 _my_bg1 = 0xFF16161D;
    u32 _my_bg2 = 0xFF1F1F28;
    u32 _my_border = 0xFF2A2A37;
    u32 _my_fg = 0xFFDCD7BA;
    gl_clear(fb, _my_bg1);
    Line_t line = {
        .x = 0, .y = 24, .length = fb->width, .dir = DIRECTION_RIGHT};
    gl_draw_line(fb, _my_border, &line);
    Rect_t term_window = {.x = 16,
                          .y = 40,
                          .w = fb->width - 32,
                          .h = fb->height - 56,
                          .fill = true};
    gl_draw_rect(fb, _my_bg2, &term_window);
    term_window.fill = false;
    gl_draw_rect(fb, _my_border, &term_window);
    Rect_t bar = {.x = 0, .y = 0, .w = fb->width, .h = 24, .fill = true};
    gl_draw_rect(fb, _my_bg2, &bar);
    term_render(term, fb, _my_fg, term_window.x + 16, term_window.y + 16);
    display_commit(TTY_DISPLAY);
}

void kern_init_tty()
{
    u64 error;
    if ((error = display_acquire(&fb, TTY_DISPLAY))) {
        debug_err("Failed to get display %d (Error %X)", TTY_DISPLAY, error);
        panic("Failed to acquire display");
    };
    term = malloc(sizeof(Terminal_t));
    term_init(term, fb->width - 64, fb->height - 88);
    const char *message =
        "#include <stdio.h>\n\nint main(void) {\n  printf(\"I just riced my "
        "own operating system lmao\\n\");\n  return 0;\n}";
    u8 *_cur = message;

    while (*_cur != 0) {
        term_write(term, *_cur++);
    }

    kern_render_tty();
}

void kern_handle_interrupt()
{
    while (tail != head) {
        _no_interrupts kinterrupt_t i = queue[tail];
        tail = (tail + 1) % QUEUE_SIZE;
        switch (i.type) {
        case KEYBOARD_INT: {
            char ch =  process_keyboard_event(i.payload);
            if(ch) term_write(term, ch);
            break;
        }
        }
    }
    kern_render_tty();
}
