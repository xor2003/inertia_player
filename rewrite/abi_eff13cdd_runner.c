#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db globals[0x200];
static db input_playsettings;
static dw input_ax;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13CDD(void);
#pragma aux eff_13CDD __parm __caller [] __modify __exact [__ax]
void eff_13CDD(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13cdd(&r, globals, input_playsettings);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 6) return 2;
    if (!streq(argv[1], "abieff13cdd")) return 2;

    memset(globals, 0, sizeof(globals));
    input_playsettings = (db)strtoul(argv[2], 0, 0);
    globals[0x00c7] = (db)strtoul(argv[3], 0, 0);
    globals[0x00c8] = (db)strtoul(argv[4], 0, 0);
    input_ax = (dw)strtoul(argv[5], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13CDD
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x\n", ax_after, (unsigned)globals[0x00c7], (unsigned)globals[0x00c8]);
    return 0;
}
