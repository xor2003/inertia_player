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

void iplay_sub_1609f_disabled(IplayRegs *r, db *dst, dw buffer_size);

#define DSEG_SCRATCH 0x2800u
#define CHANNEL_OFF 0x9000u

static db dst[0x400];
static dw input_buffer_size;
static dw ret_ax;
static dw ret_bx;
static dw ret_cx;
static dw ret_si;
static dw ret_di;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

void sub_1609F(void);
#pragma aux sub_1609F __parm __caller [] __modify __exact [__ax __bx __cx __si __di]
void sub_1609F(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.esi = CHANNEL_OFF;
    r.edi = DSEG_SCRATCH;
    iplay_sub_1609f_disabled(&r, dst, input_buffer_size);

    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_si = (dw)r.esi;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov si, ret_si
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned si_after;
    unsigned di_after;
    unsigned count;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abisub1609fdisabled")) return 2;

    input_buffer_size = (dw)strtoul(argv[2], 0, 0);
    count = (unsigned)input_buffer_size * 8u;
    memset(dst, 0xa5, count);

    _asm {
        mov si, 9000h
        mov di, 2800h
        call sub_1609F
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov si_after, si
        mov di_after, di
    }

    printf("ax=%04x bx=%04x cx=%04x si=%04x di=%04x data=", ax_after, bx_after, cx_after, si_after, di_after);
    print_bytes(dst, count);
    printf("\n");
    return 0;
}
