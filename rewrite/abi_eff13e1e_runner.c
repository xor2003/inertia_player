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

void iplay_eff_13e1e(IplayRegs *r, db *channel);

static db channel[0x40];
static dw input_ax;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void put_word(db *mem, unsigned off, dw value) {
    mem[off] = (db)value;
    mem[off + 1] = (db)(value >> 8);
}

void eff_13E1E(void);
#pragma aux eff_13E1E __parm __caller [] __modify __exact [__ax]
void eff_13E1E(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13e1e(&r, channel);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    dw value;

    if (argc != 7) return 2;
    if (!streq(argv[1], "abieff13e1e")) return 2;

    memset(channel, 0, sizeof(channel));
    value = (dw)strtoul(argv[2], 0, 0);
    put_word(channel, 0, value);
    value = (dw)strtoul(argv[3], 0, 0);
    put_word(channel, 0x10, value);
    value = (dw)strtoul(argv[4], 0, 0);
    put_word(channel, 0x12, value);
    channel[0x17] = (db)strtoul(argv[5], 0, 0);
    input_ax = (dw)strtoul(argv[6], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13E1E
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x%02x%02x%02x%02x%02x\n",
           ax_after,
           (unsigned)channel[0], (unsigned)channel[1],
           (unsigned)channel[0x10], (unsigned)channel[0x11],
           (unsigned)channel[0x12], (unsigned)channel[0x13],
           (unsigned)channel[0x17]);
    return 0;
}
