#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

typedef struct IplayRegs {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd ebp;
    dd esi;
    dd edi;
} IplayRegs;

void iplay_sub_12f56(IplayRegs *r, db *mem, dw index, dw total, db segment_index, db pending, db bh);

static db mem[0x4000];
static dw input_index;
static dw input_total;
static db input_segment_index;
static db input_pending;
static db input_bh;
static dw out_ax;
static dw out_si;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void __far sub_12F56(void);
#pragma aux sub_12F56 __parm __caller [] __modify __exact [__ax __si]
void __far sub_12F56(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sub_12f56(&r, mem, input_index, input_total, input_segment_index, input_pending, input_bh);

    out_ax = (dw)r.eax;
    out_si = (dw)r.esi;

    _asm {
        mov ax, out_ax
        mov si, out_si
    }
}

int main(int argc, char **argv) {
    unsigned si_after;

    if (argc != 7) return 2;
    if (!streq(argv[1], "abisub12f56")) return 2;

    input_index = (dw)strtoul(argv[2], 0, 0);
    input_total = (dw)strtoul(argv[3], 0, 0);
    input_segment_index = (db)strtoul(argv[4], 0, 0);
    input_pending = (db)strtoul(argv[5], 0, 0);
    input_bh = (db)strtoul(argv[6], 0, 0);
    memset(mem, 0, sizeof(mem));

    _asm {
        call sub_12F56
        mov si_after, si
    }

    printf("si=%04x data=", si_after);
    print_bytes(mem + 0x0014, 2);
    print_bytes(mem + 0x0050, 12);
    print_bytes(mem + 0x00c9, 5);
    print_bytes(mem + 0x3d48 + (input_index >> 3), 1);
    printf("\n");
    return 0;
}
