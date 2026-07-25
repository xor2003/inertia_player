#include <stdio.h>
#include <string.h>

#include "iplay_rewrite.h"

static db channel[0x40];
static dw in_ax;
static dw in_bx;
static dw in_cx;
static dw in_dx;
static dw out_ax;
static dw out_bx;
static dw out_cx;
static dw out_dx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_13429(void);
#pragma aux sub_13429 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sub_13429(void) {
    IplayRegs r;

    _asm {
        mov in_ax, ax
        mov in_bx, bx
        mov in_cx, cx
        mov in_dx, dx
    }

    memset(&r, 0, sizeof(r));
    r.eax = in_ax;
    r.ebx = in_bx;
    r.ecx = in_cx;
    r.edx = in_dx;

    iplay_sub_13429_guard(&r, channel);

    out_ax = (dw)r.eax;
    out_bx = (dw)r.ebx;
    out_cx = (dw)r.ecx;
    out_dx = (dw)r.edx;

    _asm {
        mov ax, out_ax
        mov bx, out_bx
        mov cx, out_cx
        mov dx, out_dx
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abisub13429guard")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x03] = 0x55;
    channel[0x17] = 0;

    _asm {
        mov ax, 1234h
        mov bx, 9000h
        mov cx, 9abch
        mov dx, 0def0h
        call sub_13429
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov dx_after, dx
    }

    printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x%02x\n",
           ax_after,
           bx_after,
           cx_after,
           dx_after,
           (unsigned)channel[0x03],
           (unsigned)channel[0x17]);
    return 0;
}
