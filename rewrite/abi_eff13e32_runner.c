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

void iplay_eff_13e32(IplayRegs *r, db *channel, db byte_24668, db max_volume);

static db channel[0x40];
static db input_byte_24668;
static db input_max_volume;
static dw input_ax;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13E32(void);
#pragma aux eff_13E32 __parm __caller [] __modify __exact [__ax]
void eff_13E32(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13e32(&r, channel, input_byte_24668, input_max_volume);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 7) return 2;
    if (!streq(argv[1], "abieff13e32")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x08] = (db)strtoul(argv[2], 0, 0);
    input_byte_24668 = (db)strtoul(argv[3], 0, 0);
    input_max_volume = (db)strtoul(argv[4], 0, 0);
    channel[0x34] = (db)strtoul(argv[5], 0, 0);
    input_ax = (dw)strtoul(argv[6], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13E32
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x\n", ax_after, (unsigned)channel[0x08], (unsigned)channel[0x34]);
    return 0;
}
