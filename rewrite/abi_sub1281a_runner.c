#include "iplay_rewrite.h"
#include <stdio.h>
#include <string.h>

static db dst[4];
static db samples[8];
static db vlm_table[0x20];
static db channel[0x50];

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static unsigned parse_sample(const char *s) {
    if (streq(s, "0x2") || streq(s, "2")) return 2;
    return 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void sub_1281A(void) {
    IplayRegs r;
    dw ax_in;
    dw di_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.edi = di_in;
    iplay_sub_1281a_small(&r, dst, samples, vlm_table, channel, 1, 4);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned cx_after;
    unsigned dx_after;
    unsigned si_after;
    unsigned di_after;

    if (argc != 2 && argc != 3) return 2;
    if (!streq(argv[1], "abisub1281asmallmix")) return 2;

    memset(dst, 0xa5, sizeof(dst));
    memset(samples, 0, sizeof(samples));
    memset(vlm_table, 0, sizeof(vlm_table));
    memset(channel, 0, sizeof(channel));
    samples[0] = (argc == 3) ? (db)parse_sample(argv[2]) : 0;
    samples[1] = 1;
    samples[2] = 2;
    samples[3] = 3;
    vlm_table[1] = 0x11;
    vlm_table[3] = 0x22;
    vlm_table[5] = 0x33;
    vlm_table[7] = 0x44;
    channel[0x20] = 0;
    channel[0x21] = 1;
    channel[0x23] = 0;

    _asm {
        xor ax, ax
        mov di, 2aa0h
        call sub_1281A
        mov si, 2aa4h
        mov di, 2ac4h
        mov ax_after, ax
        mov cx_after, cx
        mov dx_after, dx
        mov si_after, si
        mov di_after, di
    }

    printf("ax=%04x cx=%04x dx=%04x si=%04x di=%04x data=",
           ax_after, cx_after, dx_after, si_after, di_after);
    print_bytes(dst, sizeof(dst));
    printf("\n");
    return 0;
}
