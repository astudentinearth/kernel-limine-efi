#pragma once
#include "limine.h"
#include "stdint.h"
void draw_line(int x, int y);
void draw_frame(int x, int y, int width, int height);
void draw_rect(int x, int y, int width, int height);
void draw_char(int x, int y, char c, u32 color);
void set_framebuffer(struct limine_framebuffer* buf);
void gfx_init();
