#pragma once
#include "gfx.h"
#include "hardware/display.h"
#include "stdint.h"

// shorthand string literal macros

#define FG_BLACK "\x1b[30m"
#define FG_RED "\x1b[31m"
#define FG_GREEN "\x1b[32m"
#define FG_YELLOW "\x1b[33m"
#define FG_BLUE "\x1b[34m"
#define FG_MAGENTA "\x1b[35m"
#define FG_CYAN "\x1b[36m"
#define FG_WHITE "\x1b[37m"
#define FG_DEFAULT "\x1b[39m"
#define FG_BOLD "\x1b[1m"
#define FG_RESET_BOLD "\x1b[22m"

#define BG_BLACK "\x1b[40m"
#define BG_RED "\x1b[41m"
#define BG_GREEN "\x1b[42m"
#define BG_YELLOW "\x1b[43m"
#define BG_BLUE "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN "\x1b[46m"
#define BG_WHITE "\x1b[47m"
#define BG_DEFAULT "\x1b[49m"

// color codes

#define tty_color_idx(color) color - 30
typedef u64 TTYColorScheme_t[20];

extern const TTYColorScheme_t TTY_DEFAULT_COLORS;
extern const TTYColorScheme_t TTY_KANAGAWA_COLORS;

typedef enum __TerminalForegroundColor_t {
    TTY_FG_BLACK = 30,
    TTY_FG_RED = 31,
    TTY_FG_GREEN = 32,
    TTY_FG_YELLOW = 33,
    TTY_FG_BLUE = 34,
    TTY_FG_MAGENTA = 35,
    TTY_FG_CYAN = 36,
    TTY_FG_WHITE = 37,
    TTY_FG_DEFAULT = 39
} TerminalFgColor_t;


typedef enum __TerminalBackgroundColor_t {
    TTY_BG_BLACK = 40,
    TTY_BG_RED = 41,
    TTY_BG_GREEN = 42,
    TTY_BG_YELLOW = 43,
    TTY_BG_BLUE = 44,
    TTY_BG_MAGENTA = 45,
    TTY_BG_CYAN = 46,
    TTY_BG_WHITE = 47,
    TTY_BG_DEFAULT = 49
} TerminalBgColor_t;

// control sequences

#define TERM_ESC '\x1b'
#define TERM_ESC_CSI '['
#define ESC_CURSOR_MOVE_UP 'A'
#define ESC_CURSOR_MOVE_DOWN 'B'
#define ESC_CURSOR_RIGHT 'C'
#define ESC_CURSOR_LEFT 'D'
#define ESC_CURSOR_MOVE_DOWN_LINES 'E'
#define ESC_CURSOR_MOVE_UP_LINES 'F'
#define ESC_CURSOR_MOVE_TO_COL 'G'
#define ESC_CURSOR_HOME 'H'
#define ESC_SET_COLOR 'm'
#define ESC_TTY_RESET_COLORS 0

#define TTY_CHAR_WIDTH 8
#define TTY_CHAR_HEIGHT 16

#define TTY_CH_DIRTY 0x1

#define TTY_CSI_MAX_DIGITS 5

typedef enum __TerminalEscState {
    TERM_NO_SEQ, TERM_IN_ESC, TERM_IN_CSI 
} TerminalEscState_t;

#define TTY_CSI_MAX_PARAMETERS 8

typedef struct __CSIState_t {
    TerminalEscState_t state;
    u16 parameters[TTY_CSI_MAX_PARAMETERS];
    char current_number[TTY_CSI_MAX_DIGITS + 1];
    usize current_number_idx;
    usize current_parameter_idx;
} CSIState_t;

typedef struct __TermColorState_t {
    const TTYColorScheme_t *colors;
    TerminalBgColor_t bg_color;
    TerminalFgColor_t fg_color;
} TermColorState_t;

typedef struct __TerminalRenderingContext_t {
    framebuffer_t *fb;
    usize width_px;
    usize height_px;
    usize x;
    usize y;
} TerminalRenderingContext_t;

typedef struct __Terminal_t {
    usize width;
    usize height;
    usize cursor_pos;
    Rect_t last_rendered_cursor_rect;
    usize total_chars;
    u8 *chars;
    u8 *chars_bg;
    u8 *chars_fg;
    TermColorState_t color;
    TerminalRenderingContext_t render;
    CSIState_t csi;
} Terminal_t;

u64 term_init(Terminal_t *term, TerminalRenderingContext_t render, const TTYColorScheme_t *colors);
void term_write(Terminal_t *term, u8 ch);



