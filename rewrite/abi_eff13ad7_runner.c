#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;

static db channel[0x10];
static db input_max_volume;
static dw input_ax;
static dw ret_ax;

typedef struct IplayRegs {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebp;
    unsigned long esi;
    unsigned long edi;
} IplayRegs;

void iplay_eff_13ad7(IplayRegs *r, db *channel, db max_volume);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13AD7(void);
#pragma aux eff_13AD7 __parm __caller [] __modify __exact [__ax]
void eff_13AD7(void) {
    IplayRegs r;

    r.eax = input_ax;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_eff_13ad7(&r, channel, input_max_volume);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 5) return 2;
    if (!streq(argv[1], "abieff13ad7")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x08] = (db)strtoul(argv[2], 0, 0);
    input_max_volume = (db)strtoul(argv[3], 0, 0);
    input_ax = (dw)strtoul(argv[4], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13AD7
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x\n", ax_after, (unsigned)channel[0x08]);
    return 0;
}
