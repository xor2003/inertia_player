#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db channel[0x18];
static db input_sndflags;
static dw input_ax;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13A43(void);
#pragma aux eff_13A43 __parm __caller [] __modify __exact [__ax]
void eff_13A43(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13a43(&r, channel, input_sndflags);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 5) return 2;
    if (!streq(argv[1], "abieff13a43")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x17] = (db)strtoul(argv[2], 0, 0);
    input_sndflags = (db)strtoul(argv[3], 0, 0);
    input_ax = (dw)strtoul(argv[4], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13A43
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x\n", ax_after, (unsigned)channel[0x17]);
    return 0;
}
