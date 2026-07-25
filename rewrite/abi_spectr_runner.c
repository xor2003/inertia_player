#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

#define DSEG_SCRATCH 0x2800u

typedef struct IplayRegs {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd ebp;
    dd esi;
    dd edi;
} IplayRegs;

void iplay_spectr_1bce9_equal(IplayRegs *r, db *frame);
void iplay_spectr_1bc2d_equal(IplayRegs *r, db *frame);
void iplay_spectr_1bbc1_zero(IplayRegs *r, db *bins);
void iplay_spectr_1b406_small(db *mem, dw di);
void iplay_spectr_1c4f8(IplayRegs *r);

static db mem[0x4200];
static dw ret_bx;
static dw ret_bp;
static dw ret_cx;
static dw ret_dx;
static dw ret_si;
static dw ret_di;
static dw ret_ax;
static unsigned long input_ebx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

void spectr_1BCE9(void);
#pragma aux spectr_1BCE9 __parm __caller [] __modify __exact [__bx]
void spectr_1BCE9(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ebx = DSEG_SCRATCH;
    r.ebp = DSEG_SCRATCH + 0x1000u;
    iplay_spectr_1bce9_equal(&r, mem);
    ret_bx = (dw)r.ebx;
    ret_bp = (dw)r.ebp;
    _asm {
        mov bx, ret_bx
    }
}

void spectr_1BC2D(void);
#pragma aux spectr_1BC2D __parm __caller [] __modify __exact [__bx __cx]
void spectr_1BC2D(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ebx = DSEG_SCRATCH;
    r.ebp = DSEG_SCRATCH + 0x1000u;
    iplay_spectr_1bc2d_equal(&r, mem);
    ret_bx = (dw)r.ebx;
    ret_bp = (dw)r.ebp;
    ret_cx = (dw)r.ecx;
    _asm {
        mov bx, ret_bx
        mov cx, ret_cx
    }
}

void spectr_1BBC1(void);
#pragma aux spectr_1BBC1 __parm __caller [] __modify __exact [__ax __cx __si __di]
void spectr_1BBC1(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ecx = 1;
    r.esi = DSEG_SCRATCH;
    r.edi = DSEG_SCRATCH + 0x100u;
    iplay_spectr_1bbc1_zero(&r, mem);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_si = (dw)r.esi;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov si, ret_si
        mov di, ret_di
    }
}

void spectr_1C4F8(void);
#pragma aux spectr_1C4F8 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void spectr_1C4F8(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ebx = input_ebx;
    iplay_spectr_1c4f8(&r);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void spectr_1B406(void);
#pragma aux spectr_1B406 __parm __caller [] __modify __exact [__ax __cx __si __di]
void spectr_1B406(void) {
    iplay_spectr_1b406_small(mem, DSEG_SCRATCH);
}

int main(int argc, char **argv) {
    unsigned bx_after;
    unsigned bp_after;
    unsigned cx_after;
    unsigned si_after;
    unsigned di_after;

    if (argc < 2) return 2;
    memset(mem, 0, sizeof(mem));

    if (streq(argv[1], "abispectr1bce9equal")) {
        if (argc != 3) return 2;
        mem[DSEG_SCRATCH] = (db)strtoul(argv[2], 0, 0);
        mem[DSEG_SCRATCH + 0x64u] = (db)strtoul(argv[2], 0, 0);
        _asm {
            mov bx, 2800h
            call spectr_1BCE9
            mov bx_after, bx
        }
        bp_after = ret_bp;
        printf("bx=%04x bp=%04x data=", bx_after, bp_after);
        print_bytes(mem + DSEG_SCRATCH + 0x1000u, 8);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abispectr1bc2dequal")) {
        _asm {
            mov bx, 2800h
            call spectr_1BC2D
            mov bx_after, bx
        }
        bp_after = ret_bp;
        printf("bx=%04x bp=%04x data=", bx_after, bp_after);
        print_bytes(mem + DSEG_SCRATCH + 0x1000u, 8);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abispectr1bbc1zero")) {
        _asm {
            mov cx, 1
            mov si, 2800h
            mov di, 2900h
            call spectr_1BBC1
            mov cx_after, cx
            mov si_after, si
            mov di_after, di
        }
        printf("cx=%04x si=%04x di=%04x data=%02x%02x%02x\n",
               cx_after, si_after, di_after,
               (unsigned)mem[DSEG_SCRATCH + 0x100u],
               (unsigned)mem[DSEG_SCRATCH + 0x100u + 0x0c8u],
               (unsigned)mem[DSEG_SCRATCH + 0x100u + 0x12cu]);
        return 0;
    }

    if (streq(argv[1], "abispectrsqrt")) {
        unsigned long value;
        unsigned ax_after;
        unsigned bx_after;
        if (argc != 3) return 2;
        value = strtoul(argv[2], 0, 0);
        input_ebx = value;
        _asm {
            call spectr_1C4F8
            mov ax_after, ax
            mov bx_after, bx
        }
        printf("ax=%04x bx=%04x\n", ax_after, bx_after);
        return 0;
    }

    if (streq(argv[1], "abispectr1b406small")) {
        unsigned i;
        const char *hex;
        if (argc != 3) return 2;
        hex = argv[2];
        for (i = 0; i < 8u && hex[0] != 0 && hex[1] != 0; ++i) {
            char tmp[3];
            tmp[0] = hex[0];
            tmp[1] = hex[1];
            tmp[2] = 0;
            mem[DSEG_SCRATCH + i] = (db)strtoul(tmp, 0, 16);
            hex += 2;
        }
        mem[0x7d30u] = 1;
        mem[0x7d31u] = 0;
        _asm {
            mov di, 2800h
            call spectr_1B406
        }
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 8);
        print_bytes(mem + 0x7d1cu, 0x18);
        printf("\n");
        return 0;
    }

    return 2;
}
