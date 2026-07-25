#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static dw input_ax;
static dw ret_ax;
static dw ret_dx;
static dw ret_di;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_13E9B(void);
#pragma aux sub_13E9B __parm __caller [] __modify __exact [__ax __dx __di]
void sub_13E9B(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_sub_13e9b_public(&r);

    ret_ax = (dw)r.eax;
    ret_dx = (dw)r.edx;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned dx_after;
    unsigned di_after;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abisub13e9b")) return 2;

    input_ax = (dw)strtoul(argv[2], 0, 0);

    _asm {
        mov ax, input_ax
        call sub_13E9B
        mov ax_after, ax
        mov dx_after, dx
        mov di_after, di
    }

    printf("ax=%04x dx=%04x di=%04x\n", ax_after, dx_after, di_after);
    return 0;
}
