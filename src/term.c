
#include "term.h"
#include "errno.h"
#include "gfx.h"
#include "hardware/display.h"
#include "libk.h"
#include "lock.h"
#include "string.h"

#define printable(ch) ch < 32 || ch > 126

u64 term_init(Terminal_t *term, usize width_px, usize height_px, u32 bg_color)
{
    if (width_px < TTY_CHAR_WIDTH || height_px < TTY_CHAR_HEIGHT) {
        return EINVAL;
    }
    usize width = width_px / TTY_CHAR_WIDTH;
    usize height = height_px / TTY_CHAR_HEIGHT;
    usize total_chars = width * height;

    u8 *chars = malloc(total_chars * sizeof(u8));

    if (chars == NULL) { return ENOMEM; }

    memset(chars, 0, total_chars * sizeof(u8));

    Rect_t cur = {.x = 0, .y = 0, .w = TTY_CHAR_WIDTH, .h = TTY_CHAR_HEIGHT, .fill = false};
    term->width = width;
    term->height = height;
    term->chars = chars;
    term->cursor_pos = 0;
    term->total_chars = total_chars;
    term->bg_color = bg_color;
    term->last_rendered_cursor_rect = cur;

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
        if (term->cursor_pos > 0) { term->cursor_pos--; }
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

static Rect_t get_cursor_rect(Terminal_t *term, usize fb_x, usize fb_y)
{
    usize cursor_x = term->cursor_pos % term->width;
    usize cursor_y = term->cursor_pos / term->width;
    Rect_t cursor_rect = {.x = fb_x + cursor_x * TTY_CHAR_WIDTH,
                          .y = fb_y + cursor_y * TTY_CHAR_HEIGHT,
                          .w = TTY_CHAR_WIDTH - 1,
                          .h = TTY_CHAR_HEIGHT - 1,
                          .fill = false};
    return cursor_rect;
}

void term_render(Terminal_t *term, framebuffer_t *fb, u32 fg, usize fb_x,
                 usize fb_y)
{
    Rect_t original_cursor = term->last_rendered_cursor_rect;
    gl_draw_rect(fb, term->bg_color, &original_cursor);


    Rect_t char_bg = {.w = TTY_CHAR_WIDTH,
                      .h = TTY_CHAR_HEIGHT,
                      .x = fb_x,
                      .y = fb_y,
                      .fill = true};
    for (usize y = 0; y < term->height; y++) {
        for (usize x = 0; x < term->width; x++) {
            u8 ch = term->chars[y * term->width + x];
            if (printable(ch)) { continue; }
            char_bg.x = fb_x + x * TTY_CHAR_WIDTH;
            char_bg.y = fb_y + y * TTY_CHAR_HEIGHT;
            gl_draw_rect(fb, term->bg_color, &char_bg);
            gl_draw_char(fb, fg, fb_x + x * TTY_CHAR_WIDTH,
                         fb_y + y * TTY_CHAR_HEIGHT, ch);
            display_commit_rect(fb->display_n, char_bg.x, char_bg.y, char_bg.w,
                                char_bg.h);
        }
    }

    Rect_t cursor_rect = get_cursor_rect(term, fb_x, fb_y);
    cursor_rect.fill = true;
    term->last_rendered_cursor_rect = cursor_rect;
    gl_draw_rect(fb, fg, &cursor_rect);
    display_commit_rect(fb->display_n, cursor_rect.x, cursor_rect.y,
                        cursor_rect.w, cursor_rect.h);
    display_commit_rect(fb->display_n, original_cursor.x, original_cursor.y,
                        original_cursor.w, original_cursor.h);
}
