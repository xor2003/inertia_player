#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db globals[0x200];
static db channel[0x40];
static dw input_word_245f6;
static dw input_ax;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13C02(void);
#pragma aux eff_13C02 __parm __caller [] __modify __exact [__ax]
void eff_13C02(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13c02(&r, channel, globals, input_word_245f6);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 7) return 2;
    if (!streq(argv[1], "abieff13c02")) return 2;

    memset(globals, 0, sizeof(globals));
    memset(channel, 0, sizeof(channel));
    globals[0x00c8] = (db)strtoul(argv[2], 0, 0);
    input_word_245f6 = (dw)strtoul(argv[3], 0, 0);
    globals[0x00c9] = 0xaa;
    globals[0x00cb] = 0xbb;
    channel[0x3b] = (db)strtoul(argv[4], 0, 0);
    channel[0x3c] = (db)strtoul(argv[5], 0, 0);
    input_ax = (dw)strtoul(argv[6], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13C02
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x globals=%02x%02x%02x%02x\n",
           ax_after,
           (unsigned)channel[0x3b],
           (unsigned)channel[0x3c],
           (unsigned)globals[0x00c8],
           (unsigned)globals[0x00c9],
           (unsigned)globals[0x00ca],
           (unsigned)globals[0x00cb]);
    return 0;
}
