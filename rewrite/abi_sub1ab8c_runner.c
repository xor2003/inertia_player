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

void iplay_sub_1ab8c(IplayRegs *r, const db *channel);

static db channel[0x40];
static dw input_cx;
static dw ret_ax;
static dw ret_si;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_1AB8C(void);
#pragma aux sub_1AB8C __parm __caller [] __modify __exact [__ax __si]
void sub_1AB8C(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ecx = input_cx;
    r.esi = 0x2222;
    iplay_sub_1ab8c(&r, channel);

    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned si_after;

    if (argc != 4) return 2;
    if (!streq(argv[1], "abisub1ab8c")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x35] = (db)strtoul(argv[2], 0, 0);
    input_cx = (dw)strtoul(argv[3], 0, 0);

    _asm {
        mov cx, input_cx
        mov si, 2222h
        call sub_1AB8C
        mov ax_after, ax
        mov si_after, si
    }

    printf("ax=%04x si=%04x\n", ax_after, si_after);
    return 0;
}
