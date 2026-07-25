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

void iplay_setvideomode_no_hw(IplayRegs *r, db *globals);

static db globals[0x1800];
static dw input_ax;
static dw input_bx;
static dw input_cx;
static dw input_dx;
static dw ret_ax;
static dw ret_bx;
static dw ret_cx;
static dw ret_dx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void setvideomode(void);
#pragma aux setvideomode __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void setvideomode(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.ebx = input_bx;
    r.ecx = input_cx;
    r.edx = input_dx;
    iplay_setvideomode_no_hw(&r, globals);

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

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abisetvideomode")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x1680] = (db)strtoul(argv[2], 0, 0);
    input_ax = 0x1234;
    input_bx = 0x5678;
    input_cx = 0x9abc;
    input_dx = 0xdef0;

    _asm {
        mov ax, 1234h
        mov bx, 5678h
        mov cx, 9abch
        mov dx, 0def0h
        call setvideomode
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov dx_after, dx
    }

    printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x\n",
           ax_after, bx_after, cx_after, dx_after, (unsigned)globals[0x1680]);
    return 0;
}
