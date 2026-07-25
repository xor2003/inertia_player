#include "notcurses_presenter.hpp"

#include <notcurses/notcurses.h>

#include <cstdlib>
#include <cstdio>
#include <string>

struct IplayVgaRgb {
    unsigned r;
    unsigned g;
    unsigned b;
};

static struct notcurses *presenter;
static unsigned presenter_mouse_x;
static unsigned presenter_mouse_y;
static db presenter_characters[256u][512u];
static bool presenter_fixed_layout;

static const IplayVgaRgb *vga_rgb(unsigned color) {
    static const IplayVgaRgb palette[16] = {
        {0u, 0u, 0u},       {0u, 0u, 170u},     {0u, 170u, 0u},     {0u, 170u, 170u},
        {170u, 0u, 0u},     {170u, 0u, 170u},   {170u, 85u, 0u},    {170u, 170u, 170u},
        {85u, 85u, 85u},    {85u, 85u, 255u},   {85u, 255u, 85u},   {85u, 255u, 255u},
        {255u, 85u, 85u},   {255u, 85u, 255u},  {255u, 255u, 85u},  {255u, 255u, 255u},
    };
    return &palette[color & 15u];
}

static const char *cp437_egc(db ch) {
    static char ascii[2];
    if (ch >= 0x20u && ch <= 0x7eu) {
        ascii[0] = (char)ch;
        ascii[1] = 0;
        return ascii;
    }
    switch (ch) {
    case 0x16u: return "\u25ac";
    case 0x18u: return "\u2191";
    case 0x19u: return "\u2193";
    case 0x1au: return "\u2192";
    case 0x1bu: return "\u2190";
    case 0x1du: return "\u2194";
    case 0xb3u: return "\u2502";
    case 0xb0u: return "\u2591";
    case 0xbfu: return "\u2510";
    case 0xc0u: return "\u2514";
    case 0xc4u: return "\u2500";
    case 0xd9u: return "\u2518";
    case 0xdau: return "\u250c";
    case 0xdbu: return "\u2588";
    case 0xdcu: return "\u2584";
    case 0xdfu: return "\u2580";
    case 0xf9u: return "\u2219";
    case 0xfeu: return "\u25a0";
    default: return " ";
    }
}

static bool ensure_presenter(void) {
    notcurses_options options = {};
    if (presenter) return true;
    options.flags = NCOPTION_SUPPRESS_BANNERS;
    presenter = notcurses_init(&options, stdout);
    if (presenter) {
        (void)notcurses_cursor_disable(presenter);
        std::atexit(iplay_notcurses_presenter_stop);
    }
    return presenter != 0;
}

static void draw_80x25_closing_row(struct ncplane *plane) {
    const IplayVgaRgb *fg = vga_rgb(15u);
    const IplayVgaRgb *bg = vga_rgb(7u);
    unsigned col;
    (void)ncplane_set_fg_rgb8(plane, fg->r, fg->g, fg->b);
    (void)ncplane_set_bg_rgb8(plane, bg->r, bg->g, bg->b);
    for (col = 0u; col < 80u; ++col) {
        db ch = ' ';
        if (col == 0u || col == 3u || col == 42u) ch = 0xc0u;
        if (col == 37u || col == 76u || col == 79u) ch = 0xd9u;
        if ((col > 3u && col < 37u) || (col > 42u && col < 76u)) ch = 0xc4u;
        (void)ncplane_putegc_yx(plane, 25, (int)col, cp437_egc(ch), 0);
    }
}

static void put_dos_cell(struct ncplane *plane, int y, int x, db ch, db attr) {
    if (y >= 0 && y < 256 && x >= 0 && x < 512) {
        presenter_characters[(unsigned)y][(unsigned)x] = ch;
    }
    const IplayVgaRgb *fg = vga_rgb((unsigned)iplay_text_attr_fg(attr));
    const IplayVgaRgb *bg = vga_rgb((unsigned)iplay_text_attr_bg(attr));
    (void)ncplane_set_fg_rgb8(plane, fg->r, fg->g, fg->b);
    (void)ncplane_set_bg_rgb8(plane, bg->r, bg->g, bg->b);
    (void)ncplane_putegc_yx(plane, y, x, cp437_egc(ch), 0);
}

static void draw_hline(struct ncplane *plane, int y, int first, int last, db attr) {
    for (int x = first; x <= last; ++x) put_dos_cell(plane, y, x, 0xc4u, attr);
}

static void center_source_title_row(struct ncplane *plane,
                                    const db *cells,
                                    unsigned source_cols,
                                    unsigned terminal_cols,
                                    unsigned row) {
    unsigned first = 4u;
    unsigned last = source_cols > 4u ? source_cols - 5u : 0u;
    while (first <= last &&
           cells[IPLAY_TEXT_OFFSET(source_cols, row, first)] == ' ') ++first;
    while (last >= first &&
           cells[IPLAY_TEXT_OFFSET(source_cols, row, last)] == ' ') --last;
    if (first > last) return;
    const unsigned length = last - first + 1u;
    const int target = ((int)terminal_cols - (int)length) / 2;
    for (int x = 4; x < (int)terminal_cols - 4; ++x) {
        put_dos_cell(plane, (int)row, x, ' ', 0x7fu);
    }
    for (unsigned index = 0u; index < length; ++index) {
        const dw offset = IPLAY_TEXT_OFFSET(source_cols, row, first + index);
        put_dos_cell(plane, (int)row, target + (int)index,
                     cells[offset], cells[offset + 1u]);
    }
}

static bool is_channel_meter_row(const db *cells, unsigned source_cols, unsigned row) {
    unsigned bars = 0u;
    for (unsigned col = 32u; col <= 61u; ++col) {
        dw offset = IPLAY_TEXT_OFFSET(source_cols, row, col);
        if (cells[offset] == 0x16u) ++bars;
    }
    return bars == 30u;
}

static bool is_sample_view(const db *cells, unsigned source_cols, unsigned source_rows) {
    static const char marker[] = "# SampleName";
    if (source_rows <= 6u || source_cols < 15u) return false;
    for (unsigned i = 0u; i + 1u < sizeof(marker); ++i) {
        dw offset = IPLAY_TEXT_OFFSET(source_cols, 6u, (dw)(3u + i));
        if (cells[offset] != (db)marker[i]) return false;
    }
    return true;
}

static void present_responsive_cells(struct ncplane *plane,
                                     const db *cells,
                                     const IplayTextMode *mode,
                                     unsigned terminal_rows,
                                     unsigned terminal_cols) {
    const unsigned source_rows = iplay_text_mode_rows(mode);
    const unsigned source_cols = iplay_text_mode_cols(mode);
    const int lower_top = (int)terminal_rows - 11;
    const int middle_bottom = lower_top - 1;
    const int lower_bottom = (int)terminal_rows - 2;
    const int center = (int)terminal_cols / 2;
    const int left_right = center - 3;
    const int right_left = center + 2;
    const int right_right = (int)terminal_cols - 4;
    const unsigned overflow_source_first_row = 28u;
    const int overflow_target_first_row = 16;
    const unsigned channel_label_column = 3u;
    unsigned row;
    unsigned col;
    const bool sample_view = is_sample_view(cells, source_cols, source_rows);

    for (row = 0u; row < terminal_rows; ++row) {
        for (col = 0u; col < terminal_cols; ++col) {
            put_dos_cell(plane, (int)row, (int)col, ' ', 0x78u);
        }
    }
    for (row = 0u; row < source_rows; ++row) {
        int target_y;
        if (row == 0u || row == 1u || row == 4u || row == 5u
            || row == 16u || row == 17u || (row >= 26u && row < 28u)) {
            continue;
        }
        if (row >= 28u) target_y = 16 + (int)row - 28;
        else if (row == 16u) target_y = middle_bottom;
        else if (row >= 17u) target_y = lower_top + (int)row - 17;
        else target_y = (int)row;
        if (target_y < 0 || target_y >= (int)terminal_rows) continue;
        if (target_y >= middle_bottom && row >= 28u) continue;
        if (sample_view && (row == 6u || (row >= 7u && row <= 15u) || row >= 28u)) {
            int details_x = (int)terminal_cols - 44;
            if (row == 6u) {
                static const char more[] = "Press F-4 for more";
                static const char details[] = "Size Vol Mode  C-2 Tune LoopPos LoopEnd";
                for (col = 3u; col < 15u; ++col) {
                    dw offset = IPLAY_TEXT_OFFSET(source_cols, row, col);
                    put_dos_cell(plane, target_y, (int)col, cells[offset], cells[offset + 1u]);
                }
                for (col = 0u; col + 1u < sizeof(more); ++col) {
                    put_dos_cell(
                        plane,
                        target_y,
                        (int)terminal_cols / 2 - (int)(sizeof(more) - 1u) / 2 + (int)col,
                        (db)more[col],
                        0x78u);
                }
                for (col = 0u; col + 1u < sizeof(details); ++col) {
                    put_dos_cell(plane, target_y, details_x + (int)col, (db)details[col], 0x7eu);
                }
            } else {
                for (col = 2u; col <= 36u; ++col) {
                    dw offset = IPLAY_TEXT_OFFSET(source_cols, row, col);
                    put_dos_cell(plane, target_y, (int)col, cells[offset], cells[offset + 1u]);
                }
                for (int x = 37; x < details_x; ++x) {
                    put_dos_cell(plane, target_y, x, ' ', 0x7bu);
                }
                for (col = 37u; col <= 77u; ++col) {
                    dw offset = IPLAY_TEXT_OFFSET(source_cols, row, col);
                    put_dos_cell(
                        plane,
                        target_y,
                        details_x + (int)col - 37,
                        cells[offset],
                        cells[offset + 1u]);
                }
            }
            continue;
        }
        if ((row >= 6u && row <= 15u && is_channel_meter_row(cells, source_cols, row))
            || (row >= 28u && is_channel_meter_row(cells, source_cols, row))) {
            int effect_x = (int)terminal_cols - 17;
            int meter_last = effect_x - 2;
            int meter_width = meter_last >= 32 ? meter_last - 32 + 1 : 0;
            for (col = 2u; col <= 31u; ++col) {
                dw offset = IPLAY_TEXT_OFFSET(source_cols, row, col);
                put_dos_cell(plane, target_y, (int)col, cells[offset], cells[offset + 1u]);
            }
            for (int x = 32; x <= meter_last; ++x) {
                unsigned source_x = 32u + (unsigned)((x - 32) * 30 / meter_width);
                dw offset = IPLAY_TEXT_OFFSET(source_cols, row, source_x);
                put_dos_cell(plane, target_y, x, cells[offset], cells[offset + 1u]);
            }
            for (col = 0u; col < 15u; ++col) {
                dw offset = IPLAY_TEXT_OFFSET(source_cols, row, (dw)(63u + col));
                put_dos_cell(plane, target_y, effect_x + (int)col, cells[offset], cells[offset + 1u]);
            }
            continue;
        }
        for (col = 0u; col < source_cols; ++col) {
            int target_x = (int)col;
            if (row >= 18u) {
                if (col >= 4u && col <= 36u) {
                    target_x = (int)col;
                } else if (col >= 43u && col <= 75u) {
                    target_x = right_left + 1 + (int)col - 43;
                } else {
                    continue;
                }
            } else if (row >= 6u) {
                if (col < 2u || col > 77u) continue;
            } else {
                if (col < 4u || col > 75u) continue;
            }
            if (target_x < 0 || target_x >= (int)terminal_cols) continue;
            dw offset = IPLAY_TEXT_OFFSET(source_cols, row, col);
            put_dos_cell(plane, target_y, target_x, cells[offset], cells[offset + 1u]);
        }
    }

    for (row = overflow_source_first_row; row < source_rows; ++row) {
        const int target_y =
            overflow_target_first_row + (int)(row - overflow_source_first_row);
        if (target_y >= middle_bottom) break;
        const dw offset =
            IPLAY_TEXT_OFFSET(source_cols, row, channel_label_column);
        if ((cells[offset] >= 'A' && cells[offset] <= 'W') ||
            (cells[offset] >= '1' && cells[offset] <= '9')) {
            put_dos_cell(plane, target_y, (int)channel_label_column,
                         cells[offset], cells[offset + 1u]);
        }
    }

    put_dos_cell(plane, 0, 0, 0xdau, 0x7fu);
    draw_hline(plane, 0, 1, (int)terminal_cols - 2, 0x7fu);
    put_dos_cell(plane, 0, (int)terminal_cols - 1, 0xbfu, 0x78u);
    for (int y = 1; y <= 4; ++y) {
        put_dos_cell(plane, y, 0, 0xb3u, 0x7fu);
        put_dos_cell(plane, y, (int)terminal_cols - 1, 0xb3u, 0x78u);
    }
    put_dos_cell(plane, 1, 3, 0xdau, 0x7fu);
    draw_hline(plane, 1, 4, (int)terminal_cols - 5, 0x7fu);
    put_dos_cell(plane, 1, (int)terminal_cols - 4, 0xbfu, 0x78u);
    for (int y = 2; y <= 3; ++y) {
        put_dos_cell(plane, y, 3, 0xb3u, 0x7fu);
        put_dos_cell(plane, y, (int)terminal_cols - 4, 0xb3u, 0x78u);
    }
    put_dos_cell(plane, 4, 3, 0xc0u, 0x7fu);
    draw_hline(plane, 4, 4, (int)terminal_cols - 5, 0x78u);
    put_dos_cell(plane, 4, (int)terminal_cols - 4, 0xd9u, 0x78u);
    for (int y = 5; y <= middle_bottom; ++y) {
        put_dos_cell(plane, y, 0, 0xb3u, 0x7fu);
        put_dos_cell(plane, y, (int)terminal_cols - 1, 0xb3u, 0x78u);
    }
    for (int y = 6; y < middle_bottom; ++y) {
        put_dos_cell(plane, y, 1, 0xb3u, 0x78u);
        put_dos_cell(plane, y, (int)terminal_cols - 2, 0xb3u, 0x7fu);
    }
    put_dos_cell(plane, 5, 1, 0xdau, 0x78u);
    draw_hline(plane, 5, 2, (int)terminal_cols - 3, 0x78u);
    put_dos_cell(plane, 5, (int)terminal_cols - 2, 0xbfu, 0x7fu);
    put_dos_cell(plane, middle_bottom, 1, 0xc0u, 0x78u);
    draw_hline(plane, middle_bottom, 2, (int)terminal_cols - 3, 0x7fu);
    put_dos_cell(plane, middle_bottom, (int)terminal_cols - 2, 0xd9u, 0x7fu);
    center_source_title_row(plane, cells, source_cols, terminal_cols, 2u);
    center_source_title_row(plane, cells, source_cols, terminal_cols, 3u);

    put_dos_cell(plane, lower_top, 3, 0xdau, 0x78u);
    draw_hline(plane, lower_top, 4, left_right - 1, 0x78u);
    put_dos_cell(plane, lower_top, left_right, 0xbfu, 0x7fu);
    put_dos_cell(plane, lower_top, right_left, 0xdau, 0x78u);
    draw_hline(plane, lower_top, right_left + 1, right_right - 1, 0x78u);
    put_dos_cell(plane, lower_top, right_right, 0xbfu, 0x7fu);
    put_dos_cell(plane, lower_top, 0, 0xb3u, 0x7fu);
    put_dos_cell(plane, lower_top, (int)terminal_cols - 1, 0xb3u, 0x78u);
    for (int y = lower_top + 1; y <= lower_bottom; ++y) {
        put_dos_cell(plane, y, 0, 0xb3u, 0x7fu);
        put_dos_cell(plane, y, 3, 0xb3u, 0x78u);
        put_dos_cell(plane, y, left_right, 0xb3u, 0x7fu);
        put_dos_cell(plane, y, right_left, 0xb3u, 0x78u);
        put_dos_cell(plane, y, right_right, 0xb3u, 0x7fu);
        put_dos_cell(plane, y, (int)terminal_cols - 1, 0xb3u, 0x78u);
    }
    put_dos_cell(plane, lower_bottom, 3, 0xc0u, 0x7fu);
    draw_hline(plane, lower_bottom, 4, left_right - 1, 0x7fu);
    put_dos_cell(plane, lower_bottom, left_right, 0xd9u, 0x7fu);
    put_dos_cell(plane, lower_bottom, right_left, 0xc0u, 0x7fu);
    draw_hline(plane, lower_bottom, right_left + 1, right_right - 1, 0x7fu);
    put_dos_cell(plane, lower_bottom, right_right, 0xd9u, 0x7fu);
    put_dos_cell(plane, (int)terminal_rows - 1, 0, 0xc0u, 0x7fu);
    draw_hline(plane, (int)terminal_rows - 1, 1, (int)terminal_cols - 2, 0x78u);
    put_dos_cell(plane, (int)terminal_rows - 1, (int)terminal_cols - 1, 0xd9u, 0x78u);
}

static bool selector_frame_character(db ch) {
    return ch == 0xb3u || ch == 0xbfu || ch == 0xc0u
        || ch == 0xc4u || ch == 0xd9u || ch == 0xdau;
}

static void present_responsive_selector_cells(struct ncplane *plane,
    const db *cells,
    const IplayTextMode *mode,
    unsigned terminal_rows,
    unsigned terminal_cols) {
    const unsigned source_rows = iplay_text_mode_rows(mode);
    const unsigned source_cols = iplay_text_mode_cols(mode);
    for (unsigned y = 0u; y < terminal_rows; ++y) {
        for (unsigned x = 0u; x < terminal_cols; ++x) {
            put_dos_cell(plane, (int)y, (int)x, ' ', 0x7fu);
        }
    }
    for (unsigned source_y = 0u; source_y < source_rows; ++source_y) {
        const int target_y = source_rows > 1u
            ? (int)(source_y * (terminal_rows - 1u) / (source_rows - 1u))
            : 0;
        for (unsigned source_x = 0u; source_x < source_cols; ++source_x) {
            const dw offset = IPLAY_TEXT_OFFSET(source_cols, source_y, source_x);
            const db ch = cells[offset];
            const db attr = cells[offset + 1u];
            if (selector_frame_character(ch)) {
                const int target_x = source_cols > 1u
                    ? (int)(source_x * (terminal_cols - 1u) / (source_cols - 1u))
                    : 0;
                if (ch == 0xc4u && source_x + 1u < source_cols) {
                    const int next_x = (int)((source_x + 1u) * (terminal_cols - 1u) / (source_cols - 1u));
                    for (int x = target_x; x < next_x; ++x) put_dos_cell(plane, target_y, x, ch, attr);
                } else {
                    put_dos_cell(plane, target_y, target_x, ch, attr);
                }
            }
        }
        for (unsigned source_x = 0u; source_x < source_cols;) {
            dw offset = IPLAY_TEXT_OFFSET(source_cols, source_y, source_x);
            if (selector_frame_character(cells[offset])
                || (cells[offset] == ' ' && cells[offset + 1u] == 0x7fu)) {
                ++source_x;
                continue;
            }
            unsigned end = source_x;
            const db run_attr = cells[offset + 1u];
            while (end + 1u < source_cols) {
                const dw next = IPLAY_TEXT_OFFSET(source_cols, source_y, end + 1u);
                if (selector_frame_character(cells[next]) || cells[next + 1u] != run_attr) break;
                ++end;
            }
            const int target_x = source_cols > 1u
                ? (int)(source_x * (terminal_cols - 1u) / (source_cols - 1u))
                : 0;
            if (run_attr == 0x1eu || run_attr == 0x08u) {
                const int target_end = source_cols > 1u
                    ? (int)((end + 1u) * (terminal_cols - 1u) / (source_cols - 1u))
                    : target_x + 1;
                for (int x = target_x; x < target_end; ++x) {
                    put_dos_cell(plane, target_y, x, ' ', run_attr);
                }
            }
            for (unsigned index = source_x; index <= end && target_x + (int)(index - source_x) < (int)terminal_cols; ++index) {
                const dw text_offset = IPLAY_TEXT_OFFSET(source_cols, source_y, index);
                if (cells[text_offset] != ' ') {
                    put_dos_cell(plane,
                        target_y,
                        target_x + (int)(index - source_x),
                        cells[text_offset],
                        cells[text_offset + 1u]);
                }
            }
            source_x = end + 1u;
        }
    }
}

static void present_adaptive_selector_cells(struct ncplane *plane,
    const db *cells,
    const IplayTextMode *mode,
    unsigned terminal_rows,
    unsigned terminal_cols) {
    const unsigned source_rows = iplay_text_mode_rows(mode);
    const unsigned source_cols = iplay_text_mode_cols(mode);
    const bool large = terminal_rows > 30u;
    const int title_left = large ? 3 : 2;
    const int title_right = (int)terminal_cols - (large ? 5 : 3);
    const int title_top = large ? 1 : 0;
    const int title_bottom = large ? 4 : 3;
    const int hardware_row = large ? 6 : 4;
    const int path_left = large ? 5 : 4;
    const int path_right = (int)terminal_cols - (large ? 7 : 5);
    const int path_top = large ? 7 : 5;
    const int path_bottom = large ? 9 : 7;
    const int list_left = large ? 7 : 6;
    const int list_right = (int)terminal_cols - (large ? 9 : 7);
    const int list_top = large ? 11 : 8;
    const int list_bottom = (int)terminal_rows - (large ? 5 : 3);
    const int footer_row = large ? (int)terminal_rows - 3 : (int)terminal_rows - 1;
    const auto hline = [plane](int y, int left, int right, db attr) {
        for (int x = left + 1; x < right; ++x) put_dos_cell(plane, y, x, 0xc4u, attr);
    };
    const auto box = [plane, &hline](int top, int left, int bottom, int right) {
        hline(top, left, right, 0x7fu);
        hline(bottom, left, right, 0x78u);
        put_dos_cell(plane, top, left, 0xdau, 0x7fu);
        put_dos_cell(plane, top, right, 0xbfu, 0x78u);
        put_dos_cell(plane, bottom, left, 0xc0u, 0x78u);
        put_dos_cell(plane, bottom, right, 0xd9u, 0x78u);
        for (int y = top + 1; y < bottom; ++y) {
            put_dos_cell(plane, y, left, 0xb3u, 0x7fu);
            put_dos_cell(plane, y, right, 0xb3u, 0x78u);
        }
    };
    const auto source_text = [cells, source_cols](unsigned row) {
        std::string text;
        unsigned first = source_cols;
        unsigned last = 0u;
        for (unsigned x = 0u; x < source_cols; ++x) {
            db ch = cells[IPLAY_TEXT_OFFSET(source_cols, row, x)];
            if (ch != ' ' && !selector_frame_character(ch)) {
                if (first == source_cols) first = x;
                last = x;
            }
        }
        if (first == source_cols) return text;
        for (unsigned x = first; x <= last; ++x) {
            db ch = cells[IPLAY_TEXT_OFFSET(source_cols, row, x)];
            text += selector_frame_character(ch) ? ' ' : (char)ch;
        }
        return text;
    };
    const auto centered = [plane, terminal_cols](int y, const std::string &text, db attr) {
        int x = ((int)terminal_cols - (int)text.size()) / 2;
        for (char ch : text) put_dos_cell(plane, y, x++, (db)ch, attr);
    };

    for (unsigned y = 0u; y < terminal_rows; ++y) {
        for (unsigned x = 0u; x < terminal_cols; ++x) put_dos_cell(plane, (int)y, (int)x, ' ', 0x7fu);
    }
    if (large) box(0, 0, (int)terminal_rows - 2, (int)terminal_cols - 2);
    box(title_top, title_left, title_bottom, title_right);
    box(path_top, path_left, path_bottom, path_right);
    box(list_top, list_left, list_bottom, list_right);
    centered(title_top + 1, source_text(1u), 0x7fu);
    centered(title_top + 2, source_text(2u), 0x7fu);
    centered(hardware_row, source_text(4u), 0x78u);
    centered(path_top + 1, source_text(6u), 0x7bu);
    for (unsigned source_row = 9u; source_row + 3u < source_rows; ++source_row) {
        const int target_y = list_top + 1 + (int)(source_row - 9u);
        std::string text = source_text(source_row);
        db attr = 0x7bu;
        if (target_y >= list_bottom || text.empty()) continue;
        for (unsigned x = 0u; x < source_cols; ++x) {
            if (cells[IPLAY_TEXT_OFFSET(source_cols, source_row, x) + 1u] == 0x1eu) {
                attr = 0x1eu;
                break;
            }
        }
        if (attr == 0x1eu) {
            for (int x = list_left + 1; x < list_right; ++x) put_dos_cell(plane, target_y, x, ' ', attr);
        }
        int x = list_left + 2;
        for (char ch : text) put_dos_cell(plane, target_y, x++, (db)ch, attr);
    }
    centered(footer_row, source_text(source_rows - 1u), 0x78u);
}

bool iplay_notcurses_present_cells(const db *cells, const IplayTextMode *mode) {
    struct ncplane *plane;
    const char *dump_path;
    unsigned terminal_rows;
    unsigned terminal_cols;
    dw row;
    dw col;
    if (!cells || !mode) return false;
    dump_path = std::getenv("IPLAY_DUMP_TEXT_CELLS");
    if (dump_path && dump_path[0]) {
        FILE *dump = std::fopen(dump_path, "wb");
        if (dump) {
            (void)std::fwrite(cells, 1u, iplay_text_mode_screen_bytes(mode), dump);
            (void)std::fclose(dump);
        }
    }
    if (!ensure_presenter()) return false;
    plane = notcurses_stddim_yx(presenter, &terminal_rows, &terminal_cols);
    ncplane_erase(plane);
    if (presenter_fixed_layout
        && terminal_cols >= 80u && terminal_rows >= 25u) {
        present_adaptive_selector_cells(plane, cells, mode, terminal_rows, terminal_cols);
    } else if (!presenter_fixed_layout
        && terminal_cols >= 80u && terminal_rows >= 28u
        && iplay_text_mode_cols(mode) >= 80u) {
        present_responsive_cells(plane, cells, mode, terminal_rows, terminal_cols);
    } else {
        for (row = 0u; row < iplay_text_mode_rows(mode) && row < terminal_rows; ++row) {
            for (col = 0u; col < iplay_text_mode_cols(mode) && col < terminal_cols; ++col) {
                dw offset = IPLAY_TEXT_OFFSET(iplay_text_mode_cols(mode), row, col);
                db attr = cells[(dw)(offset + 1u)];
                put_dos_cell(plane, (int)row, (int)col, cells[offset], attr);
            }
        }
    }
    if (!presenter_fixed_layout
        && iplay_text_mode_cols(mode) == 80u && iplay_text_mode_rows(mode) == 25u
        && terminal_cols >= 80u && terminal_rows >= 26u) {
        draw_80x25_closing_row(plane);
    }
    (void)notcurses_cursor_disable(presenter);
    return notcurses_render(presenter) == 0;
}

bool iplay_notcurses_present_cells_fixed(const db *cells, const IplayTextMode *mode) {
    bool result;
    presenter_fixed_layout = true;
    result = iplay_notcurses_present_cells(cells, mode);
    presenter_fixed_layout = false;
    return result;
}

uint32_t iplay_notcurses_poll_key(void) {
    ncinput input = {};
    uint32_t key;
    if (!presenter) return IPLAY_NOTCURSES_KEY_NONE;
    key = notcurses_get_nblock(presenter, &input);
    if (key == NCKEY_BUTTON1) {
        if (input.evtype != NCTYPE_PRESS) return IPLAY_NOTCURSES_KEY_NONE;
        presenter_mouse_x = input.x;
        presenter_mouse_y = input.y;
        return IPLAY_NOTCURSES_KEY_MOUSE_LEFT;
    }
    switch (key) {
    case NCKEY_F01: return IPLAY_NOTCURSES_KEY_F1;
    case NCKEY_F02: return IPLAY_NOTCURSES_KEY_F2;
    case NCKEY_F03: return IPLAY_NOTCURSES_KEY_F3;
    case NCKEY_F04: return IPLAY_NOTCURSES_KEY_F4;
    case NCKEY_F05: return IPLAY_NOTCURSES_KEY_F5;
    case NCKEY_F06: return IPLAY_NOTCURSES_KEY_F6;
    case NCKEY_F07: return IPLAY_NOTCURSES_KEY_F7;
    case NCKEY_F08: return IPLAY_NOTCURSES_KEY_F8;
    case NCKEY_F09: return IPLAY_NOTCURSES_KEY_F9;
    case NCKEY_F10: return IPLAY_NOTCURSES_KEY_F10;
    case NCKEY_F11: return IPLAY_NOTCURSES_KEY_F11;
    case NCKEY_F12: return IPLAY_NOTCURSES_KEY_F12;
    case NCKEY_LEFT: return IPLAY_NOTCURSES_KEY_LEFT;
    case NCKEY_RIGHT: return IPLAY_NOTCURSES_KEY_RIGHT;
    case NCKEY_UP: return IPLAY_NOTCURSES_KEY_UP;
    case NCKEY_DOWN: return IPLAY_NOTCURSES_KEY_DOWN;
    case NCKEY_ENTER: return '\n';
    default: return key == (uint32_t)-1 ? IPLAY_NOTCURSES_KEY_NONE : key;
    }
}

unsigned iplay_notcurses_mouse_x(void) {
    return presenter_mouse_x;
}

unsigned iplay_notcurses_mouse_y(void) {
    return presenter_mouse_y;
}

uint32_t iplay_notcurses_mouse_character(void) {
    if (presenter_mouse_y >= 256u || presenter_mouse_x >= 512u) return 0u;
    return presenter_characters[presenter_mouse_y][presenter_mouse_x];
}

uint32_t iplay_notcurses_get_key(void) {
    ncinput input = {};
    uint32_t key;
    if (!presenter) return IPLAY_NOTCURSES_KEY_NONE;
    key = notcurses_get_blocking(presenter, &input);
    if (key == NCKEY_BUTTON1) {
        if (input.evtype != NCTYPE_PRESS) return IPLAY_NOTCURSES_KEY_NONE;
        presenter_mouse_x = input.x;
        presenter_mouse_y = input.y;
        return IPLAY_NOTCURSES_KEY_MOUSE_LEFT;
    }
    switch (key) {
    case NCKEY_UP: return IPLAY_NOTCURSES_KEY_UP;
    case NCKEY_DOWN: return IPLAY_NOTCURSES_KEY_DOWN;
    case NCKEY_ENTER: return '\n';
    default: return key == (uint32_t)-1 ? IPLAY_NOTCURSES_KEY_NONE : key;
    }
}

bool iplay_notcurses_presenter_active(void) {
    return presenter != 0;
}

unsigned iplay_notcurses_presenter_rows(void) {
    unsigned rows = 0u;
    unsigned cols = 0u;
    if (presenter) (void)notcurses_stddim_yx(presenter, &rows, &cols);
    return rows;
}

void iplay_notcurses_presenter_stop(void) {
    if (!presenter) return;
    (void)notcurses_stop(presenter);
    presenter = 0;
}
