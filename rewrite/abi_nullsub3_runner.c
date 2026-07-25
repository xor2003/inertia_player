#include <stdio.h>
#include <string.h>

static unsigned short ret_ax;
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;
static unsigned short ret_si;
static unsigned short ret_di;

typedef struct IplayRegs {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebp;
    unsigned long esi;
    unsigned long edi;
} IplayRegs;

void iplay_noop(IplayRegs *r);

static void call_noop(void) {
    IplayRegs r;

    r.eax = ret_ax;
    r.ebx = ret_bx;
    r.ecx = ret_cx;
    r.edx = ret_dx;
    r.ebp = 0;
    r.esi = ret_si;
    r.edi = ret_di;
    iplay_noop(&r);
}

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void nullsub_5(void);
#pragma aux nullsub_5 __parm __caller [] __modify __exact []
void nullsub_5(void) {
    _asm {
        mov ret_ax, ax
        mov ret_bx, bx
        mov ret_cx, cx
        mov ret_dx, dx
        mov ret_si, si
        mov ret_di, di
    }
    call_noop();
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
        mov di, ret_di
    }
}

void eff_nullsub(void);
#pragma aux eff_nullsub __parm __caller [] __modify __exact []
void eff_nullsub(void) {
    _asm {
        mov ret_ax, ax
        mov ret_bx, bx
        mov ret_cx, cx
        mov ret_dx, dx
        mov ret_si, si
        mov ret_di, di
    }
    call_noop();
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
        mov di, ret_di
    }
}

void nullsub_2(void);
#pragma aux nullsub_2 __parm __caller [] __modify __exact []
void nullsub_2(void) {
    _asm {
        mov ret_ax, ax
        mov ret_bx, bx
        mov ret_cx, cx
        mov ret_dx, dx
        mov ret_si, si
        mov ret_di, di
    }
    call_noop();
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
        mov di, ret_di
    }
}

void nullsub_4(void);
#pragma aux nullsub_4 __parm __caller [] __modify __exact []
void nullsub_4(void) {
    _asm {
        mov ret_ax, ax
        mov ret_bx, bx
        mov ret_cx, cx
        mov ret_dx, dx
        mov ret_si, si
        mov ret_di, di
    }
    call_noop();
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
        mov di, ret_di
    }
}

void nullsub_3(void);
#pragma aux nullsub_3 __parm __caller [] __modify __exact []
void nullsub_3(void) {
    _asm {
        mov ret_ax, ax
        mov ret_bx, bx
        mov ret_cx, cx
        mov ret_dx, dx
        mov ret_si, si
        mov ret_di, di
    }
    call_noop();
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
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abinoop")) return 2;

    if (streq(argv[2], "nullsub_5")) {
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call nullsub_5
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
    } else if (streq(argv[2], "eff_nullsub")) {
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call eff_nullsub
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
    } else if (streq(argv[2], "nullsub_2")) {
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call nullsub_2
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
    } else if (streq(argv[2], "nullsub_4")) {
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call nullsub_4
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
    } else if (streq(argv[2], "nullsub_3")) {
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call nullsub_3
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
    } else {
        return 2;
    }

    printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
           ax_after,
           bx_after,
           cx_after,
           dx_after);
    return 0;
}
