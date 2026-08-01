#include "kernio.h"
#include "gfx.h"
#include <stdint.h>

uint32_t *fb_cursor;

void kernio_init(uint32_t *framebuffer) {

}

void newline() {
    // check if we are at the bottom
        // yes -> clear screen and start from scratch
        // no  -> move the cursor down and to the start
}

void kputc(char c) {
    if(c == '\n') {
        // newline() -> return
    }
    // check if there's enough room on the x axis to display c
        // yes -> continue
        // no  -> newline() -> continue
    // check if there's enough room on the y axis to display c
        // yes -> continue
        // no  -> newline() -> continue
    // draw_char()
    // end
}