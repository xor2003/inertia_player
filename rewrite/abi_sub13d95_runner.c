#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db globals[0x200];
static dw input_cx;
static dw ret_ax;
static dw ret_cx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_13D95(void);
#pragma aux sub_13D95 __parm __caller [] __modify __exact [__ax __cx __dx]
void sub_13D95(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ecx = input_cx;
    iplay_sub_13d95(&r, globals);

    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov dx, 0
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abisub13d95")) return 2;

    memset(globals, 0, sizeof(globals));
    input_cx = (dw)strtoul(argv[2], 0, 0);

    _asm {
        mov cx, input_cx
        call sub_13D95
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x\n", ax_after, (unsigned)globals[0x0078], (unsigned)globals[0x0079]);
    return 0;
}
