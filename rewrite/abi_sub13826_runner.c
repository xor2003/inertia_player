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

void iplay_sub_13826(IplayRegs *r, db *channel, db byte_2461a);

static db channel[0x40];
static dw input_ax;
static dw input_table_word;
static dw ret_ax;
static dw ret_cx;
static dw ret_di;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_13826(void);
#pragma aux sub_13826 __parm __caller [] __modify __exact [__ax __cx __di]
void sub_13826(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_sub_13826(&r, channel, 1);

    ret_ax = (dw)(input_table_word >> ((db)input_ax >> 4));
    ret_cx = (dw)r.ecx;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned cx_after;
    unsigned di_after;

    if (argc != 4) return 2;
    if (!streq(argv[1], "abisub13826")) return 2;

    input_ax = (dw)strtoul(argv[2], 0, 0);
    input_table_word = (dw)strtoul(argv[3], 0, 0);
    memset(channel, 0, sizeof(channel));

    _asm {
        mov ax, input_ax
        call sub_13826
        mov ax_after, ax
        mov cx_after, cx
        mov di_after, di
    }

    printf("ax=%04x cx=%04x di=%04x\n", ax_after, cx_after, di_after);
    return 0;
}
