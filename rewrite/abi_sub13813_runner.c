#include <stdio.h>
#include <string.h>

#include "iplay_rewrite.h"

static db channel[0x40];
static dw in_ax;
static dw in_bx;
static dw in_cx;
static dw in_dx;
static dw in_di;
static dw out_ax;
static dw out_bx;
static dw out_cx;
static dw out_dx;
static dw out_di;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_13813(void);
#pragma aux sub_13813 __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void sub_13813(void) {
    IplayRegs r;

    _asm {
        mov in_ax, ax
        mov in_bx, bx
        mov in_cx, cx
        mov in_dx, dx
        mov in_di, di
    }

    memset(&r, 0, sizeof(r));
    r.eax = in_ax;
    r.ebx = in_bx;
    r.ecx = in_cx;
    r.edx = in_dx;
    r.edi = in_di;

    iplay_sub_13813_guard(&r, channel);

    out_ax = (dw)r.eax;
    out_bx = (dw)r.ebx;
    out_cx = (dw)r.ecx;
    out_dx = (dw)r.edx;
    out_di = (dw)r.edi;

    _asm {
        mov ax, out_ax
        mov bx, out_bx
        mov cx, out_cx
        mov dx, out_dx
        mov di, out_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;
    unsigned di_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abisub13813")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x0a] = 33;
    channel[0x0b] = 0x7c;

    _asm {
        mov ax, 1234h
        mov bx, 2800h
        mov cx, 5678h
        mov dx, 9abch
        mov di, 0def0h
        call sub_13813
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov dx_after, dx
        mov di_after, di
    }

    printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=%02x%02x\n",
           ax_after,
           bx_after,
           cx_after,
           dx_after,
           di_after,
           (unsigned)channel[0x0a],
           (unsigned)channel[0x0b]);
    return 0;
}
