#include <stdio.h>
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

void iplay_sub_15577_disabled(IplayRegs *r, db *channel);

static db channel[0x50];
static dw input_ax;
static dw input_bx;
static dw input_cx;
static dw input_dx;
static dw input_si;
static dw input_di;
static dw ret_ax;
static dw ret_bx;
static dw ret_cx;
static dw ret_dx;
static dw ret_si;
static dw ret_di;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_15577(void);
#pragma aux sub_15577 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sub_15577(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.ebx = input_bx;
    r.ecx = input_cx;
    r.edx = input_dx;
    r.esi = input_si;
    r.edi = input_di;
    iplay_sub_15577_disabled(&r, channel);

    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    ret_si = (dw)r.esi;
    ret_di = (dw)r.edi;
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
    unsigned si_after;
    unsigned di_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abisub15577guard")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x17] = 0;
    input_ax = 0x1234;
    input_bx = 0x5678;
    input_cx = 0x9abc;
    input_dx = 0xdef0;
    input_si = 0x9000;
    input_di = 0x2468;

    _asm {
        mov ax, 1234h
        mov bx, 5678h
        mov cx, 9abch
        mov dx, 0def0h
        mov si, 9000h
        mov di, 2468h
        call sub_15577
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov dx_after, dx
        mov si_after, si
        mov di_after, di
    }

    printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x data=%02x\n",
           ax_after, bx_after, cx_after, dx_after, si_after, di_after,
           (unsigned)channel[0x17]);
    return 0;
}
