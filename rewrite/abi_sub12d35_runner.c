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

void iplay_sub_12d35_disable(IplayRegs *r, db *code_byte);

static db code_byte;
static dw ret_ax;
static dw ret_bx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_12D35(void);
#pragma aux sub_12D35 __parm __caller [] __modify __exact [__ax __bx]
void sub_12D35(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sub_12d35_disable(&r, &code_byte);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abisub12d35disable")) return 2;
    (void)strtoul(argv[2], 0, 0);
    code_byte = 0xff;

    _asm {
        xor ax, ax
        xor bx, bx
        call sub_12D35
        mov ax_after, ax
        mov bx_after, bx
    }

    printf("ax=%04x bx=%04x data=%02x\n", ax_after, bx_after, (unsigned)code_byte);
    return 0;
}
