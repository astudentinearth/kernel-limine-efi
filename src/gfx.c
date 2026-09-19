#include "gfx.h"
#include "math.h"
#include "psf.h"
#include "stdint.h"



void gfx_init() { psf_init(); }


void gl_draw_line(framebuffer_t *fb, u32 color, Line_t *line)
{
    if (line->x >= fb->width || line->y >= fb->height) { return; }
    if (line->dir == DIRECTION_RIGHT) {
        usize limit = min(fb->width, line->length);
        u32 *current = fb->pixels + (line->y * fb->width) + line->x;
        for (usize i = 0; i < limit; i++, current++) {
            *current = color;
        }
    } else {
        usize limit = min(fb->height, line->length);
        u32 *current = fb->pixels + (line->y * fb->width) + line->x;
        for (usize i = 0; i < limit; i++, current += fb->width) {
            *current = color;
        }
    }
}
void gl_draw_rect(framebuffer_t *fb, u32 color, Rect_t *rect)
{
    if (rect->x >= fb->width || rect->y >= fb->height) { return; }
    if (!rect->fill) {
        Line_t top = {.x = rect->x,
                      .y = rect->y,
                      .length = rect->w,
                      .dir = DIRECTION_RIGHT};
        Line_t bottom = {.x = rect->x,
                         .y = rect->y + rect->h,
                         .length = rect->w,
                         .dir = DIRECTION_RIGHT};
        Line_t left = {.x = rect->x,
                       .y = rect->y,
                       .length = rect->h,
                       .dir = DIRECTION_DOWN};
        Line_t right = {.x = rect->x + rect->w,
                        .y = rect->y,
                        .length = rect->h,
                        .dir = DIRECTION_DOWN};
        gl_draw_line(fb, color, &top);
        gl_draw_line(fb, color, &left);
        gl_draw_line(fb, color, &bottom);
        gl_draw_line(fb, color, &right);
        return;
    }
    usize lim_y = min(rect->y + rect->h, fb->height);
    usize lim_x = min(rect->x + rect->w, fb->width);
    for (usize y = rect->y; y < lim_y; y++) {
        for (usize x = rect->x; x < lim_x; x++) {
            fb->pixels[y * fb->width + x] = color;
        }
    }
}

inline void gl_draw_bitmap(framebuffer_t *fb, u32 color, usize x, usize y, u8 byte)
{
    if (x >= fb->width || y >= fb->height) { return; }
    usize limit = min(8, fb->width - x);
    u32 *px = fb->pixels + y * fb->width + x;
    for (usize i = 0; i < limit; i++, px++) {
        if (byte & (1 << (7 - i))) { *px = color; }
    }
}

void gl_draw_char(framebuffer_t *fb, u32 color, usize x, usize y, u8 ch)
{
    if (x >= fb->width || y >= fb->height) { return; }
    u8 *glyph = get_char(ch);
    usize lim_dy = min(fb->height - y, 16);
    for (usize dy = 0; dy < lim_dy; dy++, glyph++) {
        gl_draw_bitmap(fb, color, x, y + dy, *glyph);
    }
}

void gl_clear(framebuffer_t *fb, u32 color)
{
    for (usize px = 0; px < fb->width * fb->height; px++) {
        fb->pixels[px] = color;
    }
}
