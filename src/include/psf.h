#pragma once
#include "stdint.h"

#define PSF1_FONT_MAGIC 0x0436
#define PSF1_MODE_512 0x1
#define PSF1_MODE_HAS_UNICODE_TABLE 0x2
#define PSF1_MODE_SEQUENCE 0x4

#define PSF2_FONT_MAGIC 0x864ab572

typedef enum __PSF_Version {
    PSF1, PSF2, PSF_Invalid
} PSF_Version;

typedef struct {
    u16 magic;
    u8 fontMode;
    u8 characterSize;
} __attribute__((packed)) PSF1_Header;

typedef struct {
    u32 magic;         /* magic bytes to identify PSF */
    u32 version;       /* zero */
    u32 headersize;    /* offset of bitmaps in file, 32 */
    u32 flags;         /* 0 if there's no unicode table */
    u32 numglyph;      /* number of glyphs */
    u32 bytesperglyph; /* size of each glyph */
    u32 height;        /* height in pixels */
    u32 width;         /* width in pixels */
} PSF2_Header;

PSF1_Header get_font();
void psf_init();
u8 *get_char(unsigned char c);
