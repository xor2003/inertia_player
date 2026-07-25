#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned short memflag_in;
static unsigned short ret_ax;
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;

typedef struct IplayRegs {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebp;
    unsigned long esi;
    unsigned long edi;
} IplayRegs;

void iplay_memfree_18a28_guard(IplayRegs *r, unsigned char memflag);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void memfree_18A28(void);
#pragma aux memfree_18A28 __parm __caller [] __modify __exact []
void memfree_18A28(void) {
    IplayRegs r;

    _asm {
        mov ret_ax, ax
        mov ret_bx, bx
        mov ret_cx, cx
        mov ret_dx, dx
        mov memflag_in, si
    }
    r.eax = ret_ax;
    r.ebx = ret_bx;
    r.ecx = ret_cx;
    r.edx = ret_dx;
    r.ebp = 0;
    r.esi = memflag_in;
    r.edi = 0;
    iplay_memfree_18a28_guard(&r, (unsigned char)memflag_in);
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;
    unsigned memflag;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abimemfree18a28")) return 2;
    memflag = (unsigned)strtoul(argv[2], 0, 0);

    _asm {
        mov ax, 1234h
        mov bx, 5678h
        mov cx, 9abch
        mov dx, 0def0h
        mov si, memflag
        call memfree_18A28
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov dx_after, dx
    }

    printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
           ax_after, bx_after, cx_after, dx_after);
    return 0;
}
