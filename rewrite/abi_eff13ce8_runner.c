#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db globals[0x200];
static dw input_ax;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13CE8(void);
#pragma aux eff_13CE8 __parm __caller [] __modify __exact [__ax]
void eff_13CE8(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13ce8(&r, globals);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 5) return 2;
    if (!streq(argv[1], "abieff13ce8")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x00c7] = (db)strtoul(argv[2], 0, 0);
    globals[0x00c8] = (db)strtoul(argv[3], 0, 0);
    input_ax = (dw)strtoul(argv[4], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13CE8
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x\n", ax_after, (unsigned)globals[0x00c7], (unsigned)globals[0x00c8]);
    return 0;
}
