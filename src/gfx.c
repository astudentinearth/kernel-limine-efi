#include <limine.h>
#include <stdint.h>

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
}
