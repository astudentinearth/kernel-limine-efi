#include "boot/limine.h"
#include "debug.h"
#include "psf.h"
#include "stdint.h"

static struct limine_framebuffer *framebuffer;

#define FB_WIDTH (framebuffer->pitch / 4)

void set_framebuffer(struct limine_framebuffer *buf)
{
    framebuffer = buf;
#ifdef TEST_MODE
    debug_puts("[Framebuffer address: ");
    debug_put_hex((u64)framebuffer->address);
    debug_puts("]\n");
#endif
}

u32 *cur_move(u32 **cur, int delta_x, int delta_y)
{
    *cur += (delta_y * FB_WIDTH + delta_x);
    return *cur;
}

u32 *cur_setpos(u32 **cur, u32 *fb_start, int x, int y)
{
    *cur = fb_start + (y * FB_WIDTH + x);
    return *cur;
}

u32 cursor_get_x(u32 *cur, u32 *fb_start)
{
    u32 diff = cur - fb_start;
    return diff % FB_WIDTH;
}

u32 cursor_get_y(u32 *cur, u32 *fb_start)
{
    u32 diff = cur - fb_start;
    u32 rem = diff % FB_WIDTH;
    if (rem > 0) {
        return diff / FB_WIDTH;
    } else {
        return diff / FB_WIDTH - 1;
    }
}

void draw_line_h(int x, int delta) {}

void draw_line_v(int y, int delta) {}

void draw_char(int x, int y, char c, u32 color)
{
    u32 *cursor = framebuffer->address;
    cur_setpos(&cursor, framebuffer->address, x, y);
    u8 *glyph_ptr = get_char(c);
    for (int i = 0; i < 16; i++) {
        u8 row = *glyph_ptr;
        for (int j = 0; j < 8; j++) {
            if (row & (1 << (7 - j))) { *cursor = color; }
            cur_move(&cursor, 1, 0);
        }
        cur_move(&cursor, -8, 1);
        glyph_ptr++;
    }
}

void gfx_init() { psf_init(); }
