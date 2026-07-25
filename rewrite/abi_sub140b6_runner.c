#include <stdio.h>
#include <string.h>

#include "iplay_rewrite.h"

static db globals[0x100];
static dw in_ax;
static dw in_bx;
static dw in_cx;
static dw out_ax;
static dw out_bx;
static dw out_cx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void sub_140B6(void);
#pragma aux sub_140B6 __parm __caller [] __modify __exact [__ax __bx __cx]
void sub_140B6(void) {
    IplayRegs r;

    _asm {
        mov in_ax, ax
        mov in_bx, bx
        mov in_cx, cx
    }

    memset(&r, 0, sizeof(r));
    r.eax = in_ax;
    r.ebx = in_bx;
    r.ecx = in_cx;

    iplay_sub_140b6_guard(&r, globals);

    out_ax = (dw)r.eax;
    out_bx = (dw)r.ebx;
    out_cx = (dw)r.ecx;

    _asm {
        mov ax, out_ax
        mov bx, out_bx
        mov cx, out_cx
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abisub140b6guard")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x00d1] = 1;
    globals[0x00c8] = 0;

    _asm {
        mov ax, 1234h
        mov bx, 5678h
        mov cx, 9abch
        call sub_140B6
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
    }

    printf("ax=%04x bx=%04x cx=%04x data=", ax_after, bx_after, cx_after);
    print_bytes(globals + 0x00d1, 1);
    print_bytes(globals + 0x00c8, 1);
    printf("\n");
    return 0;
}
