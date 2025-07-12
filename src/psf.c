#include <stdint.h>
#include "psf.h"

extern char _binary_src_font_psf_start;
extern char _binary_src_font_psf_end;

static PSF_font _font;
static uint8_t* _start;

void psf_init()
{
    uint16_t glyph = 0;
    PSF_font *font = (PSF_font*)&_binary_src_font_psf_start;
    _font = *font;
    _start = (unsigned char*)&_binary_src_font_psf_start + font->headersize;
}

uint8_t* get_char(unsigned char c) {
    return _start + c * _font.bytesperglyph;
}

PSF_font get_font() {
    return _font;
}
