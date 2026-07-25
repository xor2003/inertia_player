#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db data[2];
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void calc_14043(void);
#pragma aux calc_14043 __parm __caller [] __modify __exact [__ax]
void calc_14043(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_calc_14043(&r, data[0], data[1]);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 4) return 2;
    if (!streq(argv[1], "abicalc14043")) return 2;

    data[0] = (db)strtoul(argv[2], 0, 0);
    data[1] = (db)strtoul(argv[3], 0, 0);

    _asm {
        call calc_14043
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x\n", ax_after, (unsigned)data[0], (unsigned)data[1]);
    return 0;
}
