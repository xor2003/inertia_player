#include <stdio.h>
#include <string.h>

static unsigned char dma[0x1000];
static unsigned short ret_ax;
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;
static unsigned short ret_di;

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

void iplay_memfill8080(IplayRegs *r, unsigned char *dma);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void memfill8080(void);
#pragma aux memfill8080 __parm __caller [] __modify __exact [__ax __bx __cx __es __di]
void memfill8080(void) {
    IplayRegs r;
    unsigned short ax_reg;
    unsigned short bx_reg;
    unsigned short cx_reg;
    unsigned short dx_reg;

    _asm {
        mov ax_reg, ax
        mov bx_reg, bx
        mov cx_reg, cx
        mov dx_reg, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_reg;
    r.ebx = bx_reg;
    r.ecx = cx_reg;
    r.edx = dx_reg;
    iplay_memfill8080(&r, dma);
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_cx = (unsigned short)r.ecx;
    ret_dx = (unsigned short)r.edx;
    ret_di = 0x1000u;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;
    unsigned di_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abimemfill8080")) return 2;

    memset(dma, 0xa5, sizeof(dma));
    _asm {
        mov ax, 5678h
        mov bx, 0def0h
        mov cx, 1357h
        mov dx, 2468h
        xor di, di
        call memfill8080
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov dx_after, dx
        mov di_after, di
    }

    printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
           ax_after,
           bx_after,
           cx_after,
           dx_after,
           di_after);
    print_bytes(dma, 16);
    printf("\n");
    return 0;
}
