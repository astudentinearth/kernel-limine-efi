#pragma once
#include "stdint.h"

typedef struct __framebuffer_t {
    usize width;
    usize height;
    usize pitch;
    u32 *pixels;
    u64 display_n;
} framebuffer_t;

typedef struct __display_t {
    void* fb_address;
    usize width;
    usize height;
    usize pitch;
    usize bpp;
} display_t;

u64 display_init(display_t display);
u64 display_acquire(framebuffer_t **fb_out, u64 display_n);
u64 display_commit(u64 display_n);
u64 display_commit_rect(usize display_n, usize x, usize y, usize w, usize h);
u64 display_release(u64 display_n);

