
#include "term.h"
#include "errno.h"
#include "gfx.h"
#include "libk.h"
#include "lock.h"
#include "string.h"

u64 term_init(Terminal_t *term, usize width_px, usize height_px)
{
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

void term_write(Terminal_t *term, u8 ch) {
    // defensive check
    if(term->cursor_pos >= term->total_chars) term->cursor_pos = 0;
    
    _no_interrupts
    
    if(ch == '\n') {
        term->cursor_pos = term->width * (term->cursor_pos / term->width + 1);
        return;
    }

    // we don't care about control sequences yet
    term->chars[term->cursor_pos] = ch;
    term->cursor_pos++;

    if(term->cursor_pos >= term->total_chars) term->cursor_pos = 0;
}


void term_render(Terminal_t *term, framebuffer_t *fb, u32 fg, usize fb_x, usize fb_y) {
    for(usize y = 0; y < term->height; y++) {
        for(usize x = 0; x < term->width; x++) {
            u8 ch = term->chars[y * term->width + x];
            if(ch < 32 || ch > 126) continue;
            gl_draw_char(fb, fg, fb_x + x * TTY_CHAR_WIDTH, fb_y + y * TTY_CHAR_HEIGHT, ch);
        }
    }
}

