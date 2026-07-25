#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db globals[0x200];
static db channel[0x40];
static dw in_ax;
static dw in_bx;
static dw in_cx;
static dw in_dx;
static dw in_si;
static dw out_ax;
static dw out_bx;
static dw out_cx;
static dw out_dx;
static dw out_si;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void put_word(db *mem, unsigned off, dw value) {
    mem[off] = (db)value;
    mem[off + 1u] = (db)(value >> 8);
}

static dw get_word(const db *mem, unsigned off) {
    return (dw)(mem[off] | ((dw)mem[off + 1u] << 8));
}

static void put_dword(db *mem, unsigned off, dd value) {
    mem[off] = (db)value;
    mem[off + 1u] = (db)(value >> 8);
    mem[off + 2u] = (db)(value >> 16);
    mem[off + 3u] = (db)(value >> 24);
}

static dd get_dword(const db *mem, unsigned off) {
    return (dd)mem[off] | ((dd)mem[off + 1u] << 8) | ((dd)mem[off + 2u] << 16) | ((dd)mem[off + 3u] << 24);
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void sub_154F4(void);
#pragma aux sub_154F4 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void sub_154F4(void) {
    IplayRegs r;

    _asm {
        mov in_ax, ax
        mov in_bx, bx
        mov in_cx, cx
        mov in_dx, dx
        mov in_si, si
    }

    memset(&r, 0, sizeof(r));
    r.eax = in_ax;
    r.ebx = in_bx;
    r.ecx = in_cx;
    r.edx = in_dx;
    r.esi = in_si;

    iplay_sub_154f4(&r, globals, channel);

    out_ax = (dw)r.eax;
    out_bx = (dw)r.ebx;
    out_cx = (dw)r.ecx;
    out_dx = (dw)r.edx;
    out_si = (dw)r.esi;

    _asm {
        mov ax, out_ax
        mov bx, out_bx
        mov cx, out_cx
        mov dx, out_dx
        mov si, out_si
    }
}

int main(int argc, char **argv) {
    dd sample_ptr;
    dw period;
    dw seg_base;
    dw interp_word;

    if (argc != 9) return 2;
    if (!streq(argv[1], "abisub154f4")) return 2;

    memset(globals, 0, sizeof(globals));
    memset(channel, 0, sizeof(channel));
    put_word(globals, 0x0044, (dw)strtoul(argv[2], 0, 0));
    globals[0x00d2] = (db)strtoul(argv[3], 0, 0);
    sample_ptr = strtoul(argv[4], 0, 0);
    period = (dw)strtoul(argv[5], 0, 0);
    channel[0x23] = (db)strtoul(argv[6], 0, 0);
    seg_base = (dw)strtoul(argv[7], 0, 0);
    interp_word = (dw)strtoul(argv[8], 0, 0);
    put_dword(channel, 0x04, sample_ptr);
    put_word(channel, 0x20, period);
    put_word(channel, 0x24, seg_base);
    put_word(channel, 0x36, interp_word);

    _asm { call sub_154F4 }

    printf("bx=%04x cx=%04x si=%04x data=",
           (unsigned)out_bx,
           (unsigned)out_cx,
           (unsigned)out_si);
    print_bytes(globals + 0x0074, 3);
    print_bytes(globals + 0x00e3, 1);
    printf("\n");
    return 0;
}
