#pragma once
#include "hardware/display.h"
#include "stdint.h"
#include <stdbool.h>


void gfx_init();

typedef enum _gl_direction {
    DIRECTION_RIGHT,
    DIRECTION_DOWN
} gl_direction;

typedef struct __Line_t {
    usize x;
    usize y;
    usize length;
    gl_direction dir;
} Line_t;

typedef struct __Rect_t {
    usize x;
    usize y;
    usize w;
    usize h;
    bool fill;
} Rect_t;

#define GL_COLOR_RED 0xFFFF0000
#define GL_COLOR_GREEN 0xFF00FF00
#define GL_COLOR_YELLOW 0xFFFFFF00
#define GL_COLOR_BLUE 0xFF0000FF
#define GL_COLOR_MAGENTA 0xFFFF00FF
#define GL_COLOR_CYAN 0xFF00FFFF
#define GL_COLOR_BLACK 0xFF000000
#define GL_COLOR_GRAY 0xFF808080
#define GL_COLOR_WHITE 0xFFFFFFFF

void gl_draw_line     (framebuffer_t *fb, u32 color, Line_t *line);
void gl_draw_rect     (framebuffer_t *fb, u32 color, Rect_t *rect);
void gl_draw_bitmap   (framebuffer_t *fb, u32 color, usize x, usize y, u8 byte);
void gl_draw_char     (framebuffer_t *fb, u32 color, usize x, usize y, u8 ch);
void gl_clear(framebuffer_t *fb, u32 color);


