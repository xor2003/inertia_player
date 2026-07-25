#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;

static db globals[0x200];
static db dst[16];
static dw input_word_24610;
static dw input_size;
static dw ret_cx;

typedef struct IplayRegs {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebp;
    unsigned long esi;
    unsigned long edi;
} IplayRegs;

void iplay_volume_prep_inactive(IplayRegs *r, db *globals, db *dst, dw word_24610, dw size);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void __far volume_prep(void);
#pragma aux volume_prep __parm __caller [] __modify __exact [__cx]
void __far volume_prep(void) {
    IplayRegs r;

    r.eax = input_word_24610;
    r.ebx = 0;
    r.ecx = input_size;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0x2900u;
    iplay_volume_prep_inactive(&r, globals, dst, input_word_24610, input_size);
    ret_cx = (dw)r.ecx;
    _asm {
        mov cx, ret_cx
    }
}

int main(int argc, char **argv) {
    unsigned cx_after;

    if (argc != 4) return 2;
    if (!streq(argv[1], "abivolumeprepinactive")) return 2;

    input_word_24610 = (dw)strtoul(argv[2], 0, 0);
    input_size = (dw)strtoul(argv[3], 0, 0);
    memset(globals, 0, sizeof(globals));
    memset(dst, 0xa5, sizeof(dst));

    _asm {
        mov ax, input_word_24610
        mov cx, input_size
        mov di, 2900h
        call volume_prep
        mov cx_after, cx
    }

    printf("cx=%04x data=", cx_after);
    print_bytes(globals + 0x0070, 4);
    print_bytes(dst, 8);
    printf("\n");
    return 0;
}
