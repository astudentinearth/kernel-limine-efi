#include "psf.h"
#include "debug.h"
#include "stdint.h"

extern char _binary_src_font_psf_start;
extern char _binary_src_font_psf_end;

static PSF1_Header _font_header;
static u8 *_start;

void psf_init()
{
    PSF1_Header *font = (PSF1_Header *)&_binary_src_font_psf_start;
    if(font->magic != PSF1_FONT_MAGIC) panic("A valid PSF1 font was not embedded during compilation.");
    _font_header = *font;
    _start = (unsigned char *)&_binary_src_font_psf_start + sizeof(_font_header);
    debug_err("PSF _binary_src_font_psf_start: %p | end: %p | glyph start: %p | header size: %x\n", &_binary_src_font_psf_start, &_binary_src_font_psf_end, _start, sizeof(_font_header));
}

u8 *get_char(unsigned char c) { return _start + c * _font_header.characterSize; }

PSF1_Header get_font() { return _font_header; }
