#pragma once
#include "stdint.h"

#define PSF1_FONT_MAGIC 0x0436
#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    u16 magic;
    u8 fontMode;
    u8 characterSize;
} PSF1_Header;

typedef struct {
    u32 magic;         /* magic bytes to identify PSF */
    u32 version;       /* zero */
    u32 headersize;    /* offset of bitmaps in file, 32 */
    u32 flags;         /* 0 if there's no unicode table */
    u32 numglyph;      /* number of glyphs */
    u32 bytesperglyph; /* size of each glyph */
    u32 height;        /* height in pixels */
    u32 width;         /* width in pixels */
} PSF_font;

PSF_font get_font();
void psf_init();
u8* get_char(unsigned char c);
