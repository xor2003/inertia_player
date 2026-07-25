#ifndef IPLAY_NOTCURSES_PRESENTER_HPP
#define IPLAY_NOTCURSES_PRESENTER_HPP

#include <stdint.h>

#include "iplay_rewrite.h"

enum IplayNotcursesKey {
    IPLAY_NOTCURSES_KEY_NONE = 0,
    IPLAY_NOTCURSES_KEY_F1 = 0x110001,
    IPLAY_NOTCURSES_KEY_F2,
    IPLAY_NOTCURSES_KEY_F3,
    IPLAY_NOTCURSES_KEY_F4,
    IPLAY_NOTCURSES_KEY_F5,
    IPLAY_NOTCURSES_KEY_F6,
    IPLAY_NOTCURSES_KEY_F7,
    IPLAY_NOTCURSES_KEY_F8,
    IPLAY_NOTCURSES_KEY_F9,
    IPLAY_NOTCURSES_KEY_F10,
    IPLAY_NOTCURSES_KEY_F11,
    IPLAY_NOTCURSES_KEY_F12,
    IPLAY_NOTCURSES_KEY_LEFT,
    IPLAY_NOTCURSES_KEY_RIGHT,
    IPLAY_NOTCURSES_KEY_UP,
    IPLAY_NOTCURSES_KEY_DOWN,
    IPLAY_NOTCURSES_KEY_MOUSE_LEFT
};

bool iplay_notcurses_present_cells(const db *cells, const IplayTextMode *mode);
bool iplay_notcurses_present_cells_fixed(const db *cells, const IplayTextMode *mode);
bool iplay_notcurses_presenter_active(void);
unsigned iplay_notcurses_presenter_rows(void);
uint32_t iplay_notcurses_poll_key(void);
uint32_t iplay_notcurses_get_key(void);
unsigned iplay_notcurses_mouse_x(void);
unsigned iplay_notcurses_mouse_y(void);
uint32_t iplay_notcurses_mouse_character(void);
void iplay_notcurses_presenter_stop(void);

#endif
