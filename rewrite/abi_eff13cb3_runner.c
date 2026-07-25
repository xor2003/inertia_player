#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db channel[0x10];
static db input_byte_24668;
static dw input_ax;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void put_word(db *mem, unsigned off, dw value) {
    mem[off] = (db)value;
    mem[off + 1u] = (db)(value >> 8);
}

void eff_13CB3(void);
#pragma aux eff_13CB3 __parm __caller [] __modify __exact [__ax]
void eff_13CB3(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13cb3(&r, channel, input_byte_24668);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    dw period;

    if (argc != 7) return 2;
    if (!streq(argv[1], "abieff13cb3")) return 2;

    memset(channel, 0, sizeof(channel));
    period = (dw)strtoul(argv[2], 0, 0);
    put_word(channel, 0, period);
    channel[0x0a] = (db)strtoul(argv[3], 0, 0);
    channel[0x0b] = (db)strtoul(argv[4], 0, 0);
    input_byte_24668 = (db)strtoul(argv[5], 0, 0);
    input_ax = (dw)strtoul(argv[6], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13CB3
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x\n", ax_after, (unsigned)channel[0x0a], (unsigned)channel[0x0b]);
    return 0;
}
