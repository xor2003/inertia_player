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

void iplay_eff_13bb2(IplayRegs *r, db *channel);
void iplay_eff_13ba3(IplayRegs *r, db *channel);

static db channel[0x18];
static dw input_ax;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13BB2(void);
#pragma aux eff_13BB2 __parm __caller [] __modify __exact [__ax]
void eff_13BB2(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13bb2(&r, channel);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_13BA3(void);
#pragma aux eff_13BA3 __parm __caller [] __modify __exact [__ax]
void eff_13BA3(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13ba3(&r, channel);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 4) return 2;
    if (!streq(argv[1], "abieff13bb2") && !streq(argv[1], "abieff13ba3")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x17] = (db)strtoul(argv[2], 0, 0);
    input_ax = (dw)strtoul(argv[3], 0, 0);

    if (streq(argv[1], "abieff13bb2")) {
        _asm {
            mov ax, input_ax
            call eff_13BB2
            mov ax_after, ax
        }
    } else {
        _asm {
            mov ax, input_ax
            call eff_13BA3
            mov ax_after, ax
        }
    }

    printf("ax=%04x data=%02x\n", ax_after, (unsigned)channel[0x17]);
    return 0;
}
