#include <limine.h>
#include <stdint.h>
#include "psf.h"

static struct limine_framebuffer *framebuffer;

#define FB_WIDTH (framebuffer->pitch / 4)

void set_framebuffer(struct limine_framebuffer *buf) { framebuffer = buf; }

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

uint8_t fake_glyph[16] = {
    0b10000001,
    0b01000010,
    0b00111100,
    0b00011000,
    0b00011000,
    0b00111100,
    0b01000010,
    0b10000001,
    0,0,0,0,0,0,0,0
};

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

void draw_test() {
  uint32_t *cursor = framebuffer->address;

  for (int i = 0; i < 32; i++) {
    *cursor = 0xffffff;
    cur_move(&cursor, 0, 1);
  }

  cur_setpos(&cursor, framebuffer->address,
             cursor_get_x(cursor, framebuffer->address),
             cursor_get_y(cursor, framebuffer->address));

  for (int i = 0; i < 32; i++) {
    *cursor = 0xffffff;
    cur_move(&cursor, 1, 0);
  }

  for (int i = 0; i < 16; i++) {
    *cursor = 0xffffff;
    cur_move(&cursor, 1, 1);
  }

  cur_setpos(&cursor, framebuffer->address, 0, 0);

  for (int i = 0; i < 16; i++) {
    *cursor = 0x556cff;
    cur_move(&cursor, 1, 1);
  }

  psf_init();
  unsigned char msg[8] = {'H', 'e', 'l', 'l', 'o', '!', '?', '?'};
  for(int i = 0; i < 8; i++) {
    draw_char((i * 10)+64, 64, msg[i], 0x5555ff);
  }

}


