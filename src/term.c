
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

static inline void term_move_cursor(Terminal_t *term, usize new_pos)
{
    if (new_pos >= term->total_chars) { new_pos = 0; }
    term_render_pos(term, term->cursor_pos);
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

void term_render(Terminal_t *term, framebuffer_t *fb, u32 fg, usize fb_x,
                 usize fb_y)
{
    /*Rect_t original_cursor = term->las
    gl_draw_rect(fb, (*term->color.colors)[tty_color_idx(term->color.bg_color)],
                 &original_cursor);

    Rect_t char_bg = {.w = TTY_CHAR_WIDTH,
                      .h = TTY_CHAR_HEIGHT,
                      .x = fb_x,
                      .y = fb_y,
                      .fill = true};
    for (usize y = 0; y < term->height; y++) {
        for (usize x = 0; x < term->width; x++) {
            u8 ch = term->chars[y * term->width + x];
            if (printable(ch)) { continue; }
            char_bg.x = fb_x + x * TTY_CHAR_WIDTH;
            char_bg.y = fb_y + y * TTY_CHAR_HEIGHT;
            gl_draw_rect(fb, term->default_bg_argb, &char_bg);
            gl_draw_char(fb, fg, fb_x + x * TTY_CHAR_WIDTH,
                         fb_y + y * TTY_CHAR_HEIGHT, ch);
            display_commit_rect(fb->display_n, char_bg.x, char_bg.y, char_bg.w,
                                char_bg.h);
        }
    }

    Rect_t cursor_rect = get_cursor_rect(term, fb_x, fb_y);
    cursor_rect.fill = true;
    term->last_rendered_cursor_rect = cursor_rect;
    gl_draw_rect(fb, fg, &cursor_rect);
    display_commit_rect(fb->display_n, cursor_rect.x, cursor_rect.y,
                        cursor_rect.w, cursor_rect.h);
    display_commit_rect(fb->display_n, original_cursor.x, original_cursor.y,
                        original_cursor.w, original_cursor.h);*/
}
