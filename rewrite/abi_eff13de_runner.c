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

void iplay_sub_14087(IplayRegs *r, db *channel, db byte_24668);
void iplay_eff_13de5(IplayRegs *r, db *channel, db byte_24668);
void iplay_eff_13def(IplayRegs *r, db *channel, db byte_24668);

static db channel[0x40];
static db input_byte_24668;
static dw input_ax;
static dw ret_ax;
static dw ret_dx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void put_word(db *mem, unsigned off, dw value) {
    mem[off] = (db)value;
    mem[off + 1] = (db)(value >> 8);
}

void sub_14087(void);
#pragma aux sub_14087 __parm __caller [] __modify __exact [__ax __dx]
void sub_14087(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.edx = input_byte_24668 == 0 ? 0x0100u : 0x0100u;
    iplay_sub_14087(&r, channel, input_byte_24668);

    ret_ax = (dw)r.eax;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
    }
}

void eff_13DE5(void);
#pragma aux eff_13DE5 __parm __caller [] __modify __exact [__ax]
void eff_13DE5(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13de5(&r, channel, input_byte_24668);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_13DEF(void);
#pragma aux eff_13DEF __parm __caller [] __modify __exact [__ax]
void eff_13DEF(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_13def(&r, channel, input_byte_24668);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    dw period;

    if (argc == 5 && streq(argv[1], "abisub14087")) {
        memset(channel, 0, sizeof(channel));
        input_ax = (dw)strtoul(argv[2], 0, 0);
        channel[0x34] = (db)strtoul(argv[3], 0, 0);
        input_byte_24668 = (db)strtoul(argv[4], 0, 0);
        _asm {
            mov ax, input_ax
            mov dx, 0100h
            call sub_14087
            mov ax_after, ax
            mov ret_dx, dx
        }
        printf("ax=%04x dx=%04x data=%02x\n", ax_after, ret_dx, (unsigned)channel[0x34]);
        return 0;
    }

    if (argc != 7) return 2;
    if (!streq(argv[1], "abieff13de")) return 2;

    memset(channel, 0, sizeof(channel));
    period = (dw)strtoul(argv[3], 0, 0);
    put_word(channel, 0, period);
    input_byte_24668 = (db)strtoul(argv[4], 0, 0);
    channel[0x34] = (db)strtoul(argv[5], 0, 0);
    input_ax = (dw)strtoul(argv[6], 0, 0);

    if (streq(argv[2], "eff_13DE5")) {
        _asm {
            mov ax, input_ax
            call eff_13DE5
            mov ax_after, ax
        }
    } else if (streq(argv[2], "eff_13DEF")) {
        _asm {
            mov ax, input_ax
            call eff_13DEF
            mov ax_after, ax
        }
    } else {
        return 2;
    }

    printf("ax=%04x data=%02x%02x%02x\n", ax_after, (unsigned)channel[0], (unsigned)channel[1], (unsigned)channel[0x34]);
    return 0;
}
