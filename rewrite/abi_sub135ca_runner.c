#include <stdio.h>
#include <string.h>

#include "iplay_rewrite.h"

static db mem[0x1400];
static dw out_ax;
static dw out_bx;
static dw out_cx;
static dw out_dx;
static dw out_si;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void sub_135CA(void);
#pragma aux sub_135CA __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void sub_135CA(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sub_135ca_zero_event(&r, mem);

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
    if (argc != 2) return 2;
    if (!streq(argv[1], "abisub135ca")) return 2;

    memset(mem, 0, sizeof(mem));
    mem[0x1368 + 0x0a] = 0xef;
    mem[0x1368 + 0x0b] = 0xbe;
    mem[0x1368 + 0x3d] = 0xaa;

    _asm { call sub_135CA }

    printf("data=");
    print_bytes(mem + 0x0014, 2);
    print_bytes(mem + 0x1368 + 0x0a, 2);
    print_bytes(mem + 0x1368 + 0x17, 1);
    print_bytes(mem + 0x1368 + 0x3d, 1);
    printf("\n");
    return 0;
}
