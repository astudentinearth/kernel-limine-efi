
#include "term.h"
#include "errno.h"
#include "gfx.h"
#include "hardware/display.h"
#include "libk.h"
#include "lock.h"
#include "math.h"
#include "string.h"

#define printable(ch) (ch >= 32 && ch <= 126)

const TTYColorScheme_t TTY_DEFAULT_COLORS = {
    // foreground
    GL_COLOR_BLACK,
    GL_COLOR_RED,
    GL_COLOR_GREEN,
    GL_COLOR_YELLOW,
    GL_COLOR_BLUE,
    GL_COLOR_MAGENTA,
    GL_COLOR_CYAN,
    GL_COLOR_WHITE,
    0x00000000, // unused
    GL_COLOR_WHITE,

    // background
    GL_COLOR_BLACK,
    GL_COLOR_RED,
    GL_COLOR_GREEN,
    GL_COLOR_YELLOW,
    GL_COLOR_BLUE,
    GL_COLOR_MAGENTA,
    GL_COLOR_CYAN,
    GL_COLOR_WHITE,
    0x00000000, // unused
    GL_COLOR_BLACK,
};

const TTYColorScheme_t TTY_KANAGAWA_COLORS = {
    0xFF1F1F28, //black
    0xFFC34043 , //red
    0xFF76946A , // green
    0xFFDCA561 , // yellow
    0xFF7E9CD8 , // blue
    0xFF957FB8 , // magenta
    0xFF6A9589 , // cyan
    0xFFDCD7BA , // white
    0x0  , // unused
    0xFFDCD7BA , // white

    0xFF1F1F28, //black
    0xFFC34043 , //red
    0xFF76946A , // green
    0xFFDCA561 , // yellow
    0xFF7E9CD8 , // blue
    0xFF957FB8 , // magenta
    0xFF6A9589 , // cyan
    0xFFDCD7BA , // white
    0x0  , // unused
    0xFF1F1F28, //black
};

static inline u32 get_color(const TTYColorScheme_t *colors, u8 color)
{
    return (*colors)[tty_color_idx(color)];
}

u64 term_init(Terminal_t *term, TerminalRenderingContext_t render,
              const TTYColorScheme_t *colors)
{
    usize width_px = render.width_px;
    usize height_px = render.height_px;
    if (width_px < TTY_CHAR_WIDTH || height_px < TTY_CHAR_HEIGHT) {
        return EINVAL;
    }
    usize width = width_px / TTY_CHAR_WIDTH;
    usize height = height_px / TTY_CHAR_HEIGHT;
    usize total_chars = width * height;

    u8 *chars = malloc(total_chars * sizeof(u8));
    u8 *chars_fg = malloc(total_chars * sizeof(u8));
    u8 *chars_bg = malloc(total_chars * sizeof(u8));

    if (chars == NULL || chars_fg == NULL || chars_bg == NULL) {
        return ENOMEM;
    }

    memset(chars, 0, total_chars * sizeof(u8));
    memset(chars_bg, TTY_BG_DEFAULT, total_chars * sizeof(u8));
    memset(chars_fg, TTY_FG_DEFAULT, total_chars * sizeof(u8));

    Rect_t cur = {.x = 0,
                  .y = 0,
                  .w = TTY_CHAR_WIDTH,
                  .h = TTY_CHAR_HEIGHT,
                  .fill = false};
    term->width = width;
    term->height = height;
    term->chars = chars;
    term->cursor_pos = 0;
    term->total_chars = total_chars;
    term->last_rendered_cursor_rect = cur;
    term->csi = (CSIState_t){.state = TERM_NO_SEQ};
    term->color = (TermColorState_t){.colors = colors,
                                     .bg_color = TTY_BG_DEFAULT,
                                     .fg_color = TTY_FG_DEFAULT};
    term->chars_bg = chars_bg;
    term->chars_fg = chars_fg;
    term->render = render;

    return RESULT_SUCCESS;
}

static Rect_t get_rect_for_pos(Terminal_t *term, usize pos)
{
    return (Rect_t){.w = TTY_CHAR_WIDTH,
                    .h = TTY_CHAR_HEIGHT,
                    .x = (pos % term->width) * TTY_CHAR_WIDTH + term->render.x,
                    .y = (pos / term->width) * TTY_CHAR_HEIGHT + term->render.y,
                    .fill = true};
}

static inline void term_render_pos(Terminal_t *term, usize pos)
{
    if (pos >= term->total_chars) { return; }
    u32 bg = get_color(term->color.colors, term->chars_bg[pos]);
    u32 fg = get_color(term->color.colors, term->chars_fg[pos]);
    u8 ch = term->chars[pos];

    Rect_t char_rect = get_rect_for_pos(term, pos);

    // clear background
    gl_draw_rect(term->render.fb, bg, &char_rect);

    if ((printable(ch))) {
        gl_draw_char(term->render.fb, fg, char_rect.x, char_rect.y, ch);
    }
    display_commit_rect(term->render.fb->display_n, char_rect.x, char_rect.y,
                        char_rect.w, char_rect.h);
}

static inline void term_render_cursor(Terminal_t *term)
{
    u32 fg = get_color(term->color.colors, TTY_FG_DEFAULT);
    u32 char_color = get_color(term->color.colors, TTY_BG_DEFAULT);
    Rect_t char_rect = get_rect_for_pos(term, term->cursor_pos);
    u8 char_under_cursor = term->chars[term->cursor_pos];
    gl_draw_rect(term->render.fb, fg, &char_rect);
    if(printable(char_under_cursor)) {
        gl_draw_char(term->render.fb, char_color, char_rect.x, char_rect.y, char_under_cursor);
    }
    display_commit_rect(term->render.fb->display_n, char_rect.x, char_rect.y,
                        char_rect.w, char_rect.h);
}


static inline void term_clear(Terminal_t *term) {
    _no_interrupts
    memset(term->chars, 0, term->total_chars * sizeof(u8));
    memset(term->chars_bg, TTY_BG_DEFAULT, term->total_chars * sizeof(u8));
    memset(term->chars_fg, TTY_FG_DEFAULT, term->total_chars * sizeof(u8));
    Rect_t term_rect = {
        .x = term->render.x,
        .y = term->render.y,
        .w = term->render.width_px,
        .h = term->render.height_px,
        .fill = true
    };
    gl_draw_rect(term->render.fb, get_color(term->color.colors, TTY_BG_DEFAULT), &term_rect);
    display_commit_rect(term->render.fb->display_n, term_rect.x, term_rect.y, term_rect.w, term_rect.h);
}

static inline void term_scroll(Terminal_t *term, usize n) {
    if(n == 0) return;
    if(n >= term->height) {
        term_clear(term);
        return;
    }

    usize start_offset = n * term->width;
    usize keep_chars = term->total_chars - start_offset;

    _no_interrupts
    // move character buffers
    memmove(term->chars, term->chars + start_offset, term->total_chars - start_offset);
    memmove(term->chars_bg, term->chars_bg + start_offset, term->total_chars - start_offset);
    memmove(term->chars_fg, term->chars_fg + start_offset, term->total_chars - start_offset);

    // clear lower lines
    memset(term->chars + keep_chars, 0, term->total_chars - start_offset);
    memset(term->chars_bg + keep_chars, TTY_BG_DEFAULT, start_offset);
    memset(term->chars_fg + keep_chars, TTY_FG_DEFAULT, start_offset);

    // scroll pixels
    usize shift_px = n * TTY_CHAR_HEIGHT;
    usize keep_px = (term->height - n) * TTY_CHAR_HEIGHT;

    for(usize dy = 0; dy < keep_px; dy++) {
        // framebuffer start + (term offset + dy) * y + x offset
        u32 *dest = term->render.fb->pixels + (term->render.y + dy) * term->render.fb->width + term->render.x;
        u32 *src = dest + shift_px * term->render.fb->width;

        // move scanline
        memmove(dest, src, term->render.width_px * sizeof(u32));
    }

    // clear lower pixels
    Rect_t clear_rect = {
        .x = term->render.x,
        .y = term->render.y + keep_px,
        .w = term->render.width_px,
        .h = n * TTY_CHAR_HEIGHT,
        .fill = true
    };

    gl_draw_rect(term->render.fb, get_color(term->color.colors, TTY_BG_DEFAULT), &clear_rect);

    display_commit_rect(term->render.fb->display_n, term->render.x, term->render.y, term->render.width_px, term->render.height_px);
}

static inline void term_move_cursor(Terminal_t *term, usize new_pos)
{
    term_render_pos(term, term->cursor_pos);
    if (new_pos >= term->total_chars) { 
        term_scroll(term, (new_pos - term->total_chars) / term->width + 1);
        new_pos -= ((new_pos - term->total_chars) / term->width + 1) * term->width;
    }
    {
        _no_interrupts term->cursor_pos = new_pos;
    }
    term_render_cursor(term);
}

static inline void term_write_normal(Terminal_t *term, u8 ch)
{
    term->chars[term->cursor_pos] = ch;
    term->chars_bg[term->cursor_pos] = term->color.bg_color;
    term->chars_fg[term->cursor_pos] = term->color.fg_color;
    term_move_cursor(term, term->cursor_pos + 1);
}

static inline void term_handle_esc_introducer(Terminal_t *term, u8 ch)
{
    if (ch == TERM_ESC_CSI) {
        term->csi = (CSIState_t){.state = TERM_IN_CSI};
        return;
    }
    // invalid introducer, exit escape sequence
    term->csi = (CSIState_t){.state = TERM_NO_SEQ};
}

static inline void term_csi_flush_param(Terminal_t *term)
{
    if (term->csi.current_parameter_idx >= TTY_CSI_MAX_PARAMETERS) { return; }
    option_u64 value = parse_uint(term->csi.current_number);
    if (is_some(value)) {
        term->csi.parameters[term->csi.current_parameter_idx++] = value.val;
    }
    memset(term->csi.current_number, 0, TTY_CSI_MAX_DIGITS);
    term->csi.current_number_idx = 0;
}


static inline void term_reset_colors(Terminal_t *term) {
    term->color.bg_color = TTY_BG_DEFAULT;
    term->color.fg_color = TTY_FG_DEFAULT;
}

static inline void term_write_csi(Terminal_t *term, u8 ch)
{
    _no_interrupts
        // we are still taking in a number
        if (is_digit(ch))
    {
        if(term->csi.current_number_idx < TTY_CSI_MAX_DIGITS) term->csi.current_number[term->csi.current_number_idx++] = ch;
        return;
    }

    switch (ch) {

    // we are done with the current number, start new parameter
    case ';': {
        term_csi_flush_param(term);
        return;
    }

    case ESC_SET_COLOR: {
        term_csi_flush_param(term);
        if(term->csi.current_parameter_idx == 0) term_reset_colors(term); 
        for (usize p = 0; p < term->csi.current_parameter_idx; p++) {
            u16 param = term->csi.parameters[p];
            if(param == ESC_TTY_RESET_COLORS) term_reset_colors(term);
            // fg color switch, 38 is invalid
            else if (param >= 30 && param <= 39 && param != 38) {
                term->color.fg_color = param;
            }
            // bg color switch, 48 is invalid
            else if (param >= 40 && param <= 49 && param != 48) {
                term->color.bg_color = param;
            }
        }
        term->csi = (CSIState_t){.state = TERM_NO_SEQ};
        return;
    }
    }

    term->csi = (CSIState_t){.state = TERM_NO_SEQ};
}

void term_write(Terminal_t *term, u8 ch)
{
    _no_interrupts switch (ch)
    {
    case '\n':
        term_move_cursor(term,
                         term->width * (term->cursor_pos / term->width + 1));
        break;

    case '\t':
        term_move_cursor(term, term->cursor_pos + 4);
        break;

    case '\b':
        if (term->cursor_pos > 0) {
            term_move_cursor(term, term->cursor_pos - 1);
        }
        break;

    case '\r':
        term_move_cursor(term, term->width * (term->cursor_pos / term->width));
        break;

    case TERM_ESC: // entering control sequence, discard existing csi state
        term->csi = (CSIState_t){.state = TERM_IN_ESC};
        break;

    default:
        // we don't care about all control sequences yet
        if (term->csi.state == TERM_NO_SEQ) {
            term_write_normal(term, ch);
        } else if (term->csi.state == TERM_IN_ESC) {
            term_handle_esc_introducer(term, ch);
        } else if (term->csi.state == TERM_IN_CSI) {
            term_write_csi(term, ch);
        }
        break;
    }
}

