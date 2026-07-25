#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db channel[0x40];
static dw ret_ax;
static dw ret_dx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void midi_154DA(void);
#pragma aux midi_154DA __parm __caller [] __modify __exact [__ax]
void midi_154DA(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_midi_154da(&r, channel);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void midi_154DE(void);
#pragma aux midi_154DE __parm __caller [] __modify __exact [__ax __dx]
void midi_154DE(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_midi_154de(&r, channel);

    ret_ax = (dw)r.eax;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned dx_after;

    if (argc != 3) return 2;
    memset(channel, 0, sizeof(channel));

    if (streq(argv[1], "abimidi154da")) {
        channel[0x18] = (db)strtoul(argv[2], 0, 0);
        _asm {
            call midi_154DA
            mov ax_after, ax
        }
        printf("ax=%04x\n", ax_after);
        return 0;
    }

    if (streq(argv[1], "abimidi154de")) {
        channel[0x35] = (db)strtoul(argv[2], 0, 0);
        _asm {
            call midi_154DE
            mov ax_after, ax
            mov dx_after, dx
        }
        printf("ax=%04x dx=%04x\n", ax_after, dx_after);
        return 0;
    }

    return 2;
}
