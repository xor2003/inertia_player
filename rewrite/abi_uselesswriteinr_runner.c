#include <stdio.h>
#include <string.h>

static unsigned short ret_ax;
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;
static unsigned short ret_si;
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

void iplay_useless_writeinr_fail(IplayRegs *r);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void __far useless_writeinr(void);
#pragma aux useless_writeinr __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void __far useless_writeinr(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_useless_writeinr_fail(&r);
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_cx = (unsigned short)r.ecx;
    ret_dx = (unsigned short)r.edx;
    ret_si = (unsigned short)r.esi;
    ret_di = (unsigned short)r.edi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned cx_after;
    unsigned dx_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abiuselesswriteinrfail")) return 2;

    _asm {
        mov dx, 0ffffh
        call useless_writeinr
        mov ax_after, ax
        mov cx_after, cx
        mov dx_after, dx
    }

    printf("ax=%04x cx=%04x dx=%04x\n", ax_after, cx_after, dx_after);
    return 0;
}
