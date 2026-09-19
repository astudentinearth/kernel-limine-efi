
#include "term.h"
#include "errno.h"
#include "gfx.h"
#include "libk.h"
#include "lock.h"
#include "string.h"

#define printable(ch) ch < 32 || ch > 126

u64 term_init(Terminal_t *term, usize width_px, usize height_px)
{
    if(width_px < TTY_CHAR_WIDTH || height_px < TTY_CHAR_HEIGHT) return EINVAL;
    usize width = width_px / TTY_CHAR_WIDTH;
    usize height = height_px / TTY_CHAR_HEIGHT;
    usize total_chars = width * height;

    u8 *chars = malloc(total_chars * sizeof(u8));

    if (chars == NULL) { return ENOMEM; }

    memset(chars, 0, total_chars * sizeof(u8));

    term->width = width;
    term->height = height;
    term->chars = chars;
    term->cursor_pos = 0;
    term->total_chars = total_chars;

    return RESULT_SUCCESS;
}

void term_write(Terminal_t *term, u8 ch)
{
    // defensive check
    if (term->cursor_pos >= term->total_chars) { term->cursor_pos = 0; }

    _no_interrupts

        switch (ch)
    {
    case '\n':
        term->cursor_pos = term->width * (term->cursor_pos / term->width + 1);
        break;

    case '\t':
        term->cursor_pos += 4;
        break;

    case '\b':
        if(term->cursor_pos > 0) term->cursor_pos--;
        break;

    case '\r':
        term->cursor_pos = term->width * (term->cursor_pos / term->width);
        break;

    default:
        // we don't care about all control sequences yet
        term->chars[term->cursor_pos] = ch;
        term->cursor_pos++;
        break;
    }

    if (term->cursor_pos >= term->total_chars) { term->cursor_pos = 0; }
}

void term_render(Terminal_t *term, framebuffer_t *fb, u32 fg, usize fb_x,
                 usize fb_y)
{
    for (usize y = 0; y < term->height; y++) {
        for (usize x = 0; x < term->width; x++) {
            u8 ch = term->chars[y * term->width + x];
            if (printable(ch)) { continue; }
            gl_draw_char(fb, fg, fb_x + x * TTY_CHAR_WIDTH,
                         fb_y + y * TTY_CHAR_HEIGHT, ch);
        }
    }
    usize cursor_x = term->cursor_pos % term->width;
    usize cursor_y = term->cursor_pos / term->width;
    u8 char_under_cursor = term->chars[cursor_y * term->width + cursor_x];
    Rect_t cursor_rect = {.x = fb_x + cursor_x * TTY_CHAR_WIDTH,
                          .y = fb_y + cursor_y * TTY_CHAR_HEIGHT,
                          .w = TTY_CHAR_WIDTH,
                          .h = TTY_CHAR_HEIGHT,
                          .fill = (printable(char_under_cursor))};
    gl_draw_rect(fb, fg, &cursor_rect);
}
