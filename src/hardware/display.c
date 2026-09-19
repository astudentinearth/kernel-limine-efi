#include "hardware/display.h"
#include "errno.h"
#include "hardware/allocator.h"
#include "libk.h"
#include "lock.h"
#include "math.h"
#include "string.h"

#define MAX_DISPLAYS 32
static display_t current_display = {};
static framebuffer_t *current_framebuffer;
static bool display_acquired[MAX_DISPLAYS];

u64 display_init(display_t display)
{
    current_display = display;
    return RESULT_SUCCESS;
}
u64 display_acquire(framebuffer_t **fb_out, u64 display_n)
{
    if (display_n > 0 || display_n > MAX_DISPLAYS) { return ENODEV; }

    {
        _no_interrupts if (display_acquired[display_n]) return EBUSY;
    }

    display_t d = current_display;

    // we always use 4 byte pixels by convention
    usize buffer_size = d.width * d.height * sizeof(u32);
    u32 *pixel_buffer = malloc(buffer_size);
    if (pixel_buffer == NULL) { return ENOMEM; }

    memset(pixel_buffer, 0, buffer_size);

    framebuffer_t *fb = malloc(sizeof(framebuffer_t));

    fb->width = d.width;
    fb->height = d.height;
    fb->pitch = d.width * sizeof(u32);
    fb->pixels = pixel_buffer;
    fb->display_n = display_n;

    _no_interrupts display_acquired[display_n] = true;
    current_framebuffer = fb;
    *fb_out = fb;
    return RESULT_SUCCESS;
}

u64 display_commit(u64 display_n)
{
    if (display_n > 0) { return ENODEV; }
    // TODO: lock acquire/release during commit

    memcpy(current_display.fb_address, current_framebuffer->pixels,
           current_display.pitch * current_display.height);
    return RESULT_SUCCESS;
}

u64 display_commit_rect(usize display_n, usize x, usize y, usize w, usize h)
{
    if (display_n > 0) { return ENODEV; }
    usize lim_y = min(y + h, current_framebuffer->height);

    for (usize row = y; row < lim_y; row++) {
        u8 *start = (u8 *)current_framebuffer->pixels +
                    current_framebuffer->pitch * row + x * sizeof(u32);
        u8 *dest = (u8*)current_display.fb_address + row * current_display.pitch + x * sizeof(u32);
        usize length = min(w, current_framebuffer->width - x);
        memcpy(dest, start, length * sizeof(u32));
    }
    return RESULT_SUCCESS;
}

u64 display_release(u64 display_n)
{
    if (display_n > 0 || display_n > MAX_DISPLAYS) { return ENODEV; }
    {
        _no_interrupts

            if (!display_acquired[display_n]) return RESULT_SUCCESS;
        display_acquired[display_n] = false;
    }
    free(current_framebuffer->pixels);
    free(current_framebuffer);
    return RESULT_SUCCESS;
}
