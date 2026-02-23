#include "limine.h"
#include <stdint.h>
#include "psf.h"
#include "debug.h"

static struct limine_framebuffer *framebuffer;

#define FB_WIDTH (framebuffer->pitch / 4)

void set_framebuffer(struct limine_framebuffer *buf) { 
    framebuffer = buf; 
#ifdef TEST_MODE
    debug_puts("[Framebuffer address: ");
    debug_put_hex((uint64_t)framebuffer->address);
    debug_puts("]\n");
#endif
}

uint32_t *cur_move(uint32_t **cur, int delta_x, int delta_y) {
  *cur += (delta_y * FB_WIDTH + delta_x);
  return *cur;
}

uint32_t *cur_setpos(uint32_t **cur, uint32_t *fb_start, int x, int y) {
  *cur = fb_start + (y * FB_WIDTH + x);
  return *cur;
}

uint32_t cursor_get_x(uint32_t *cur, uint32_t *fb_start) {
  uint32_t diff = cur - fb_start;
  return diff % FB_WIDTH;
}

uint32_t cursor_get_y(uint32_t *cur, uint32_t *fb_start) {
  uint32_t diff = cur - fb_start;
  uint32_t rem = diff % FB_WIDTH;
  if (rem > 0)
    return diff / FB_WIDTH;
  else
    return diff / FB_WIDTH - 1;
}

void draw_line_h(int x, int delta) {}

void draw_line_v(int y, int delta) {}


void draw_char(int x, int y, char c, uint32_t color) {
    uint32_t *cursor = framebuffer->address;
    cur_setpos(&cursor, framebuffer->address, x, y);
    uint8_t *glyph_ptr = get_char(c);
    for(int i = 0; i < 16; i++) {
        uint8_t row = *glyph_ptr;
        for(int j = 0; j < 8; j++) {
            if(row & (1 << (7 - j))) *cursor = color;
            cur_move(&cursor, 1, 0);
        }
        cur_move(&cursor, -8, 1);
        glyph_ptr++;
    }
}

void gfx_init(){
    psf_init();
}
