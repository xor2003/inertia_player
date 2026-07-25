#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db channel[0x40];
static dw input_ax;
static db input_max_volume;
static dw ret_ax;
static dw ret_di;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void midi_154AC(void);
#pragma aux midi_154AC __parm __caller [] __modify __exact [__ax __di]
void midi_154AC(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.edi = 0x1000u;
    iplay_midi_154ac(&r, channel, input_max_volume);

    ret_ax = (dw)r.eax;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned di_after;
    db printed_data = '.';

    if (argc != 5) return 2;
    if (!streq(argv[1], "abimidi154ac")) return 2;

    memset(channel, 0, sizeof(channel));
    input_ax = (dw)strtoul(argv[2], 0, 0);
    input_max_volume = (db)strtoul(argv[3], 0, 0);
    channel[0x1b] = (db)strtoul(argv[4], 0, 0);

    _asm {
        mov ax, input_ax
        mov di, 1000h
        call midi_154AC
        mov ax_after, ax
        mov di_after, di
    }

    printf("ax=%04x di=%04x data=%02x\n", ax_after, di_after, (unsigned)printed_data);
    return 0;
}
