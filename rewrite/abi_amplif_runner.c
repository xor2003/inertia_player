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

void iplay_change_amplif(IplayRegs *r, db *globals, db sound_mode);
void iplay_eff_14020(IplayRegs *r, db *globals, db sound_mode);

static db globals[0x200];
static db input_sound_mode;
static dw input_ax;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static dw get_word(const db *mem, unsigned off) {
    return (dw)mem[off] | ((dw)mem[off + 1] << 8);
}

static void put_word(db *mem, unsigned off, dw value) {
    mem[off] = (db)value;
    mem[off + 1] = (db)(value >> 8);
}

void change_amplif(void);
#pragma aux change_amplif __parm __caller [] __modify __exact [__ax]
void change_amplif(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_change_amplif(&r, globals, input_sound_mode);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_14020(void);
#pragma aux eff_14020 __parm __caller [] __modify __exact [__ax]
void eff_14020(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_14020(&r, globals, input_sound_mode);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    dw channels;

    if (argc != 6) return 2;
    if (!streq(argv[1], "abiamplif")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x005e] = 100;
    globals[0x005f] = 0;
    input_ax = (dw)strtoul(argv[3], 0, 0);
    input_sound_mode = (db)strtoul(argv[4], 0, 0);
    globals[0x00de] = input_sound_mode;
    channels = (dw)strtoul(argv[5], 0, 0);
    put_word(globals, 0x0036, channels);

    if (streq(argv[2], "eff_14020")) {
        _asm {
            mov ax, input_ax
            call eff_14020
            mov ax_after, ax
        }
    } else if (streq(argv[2], "change_amplif")) {
        _asm {
            mov ax, input_ax
            call change_amplif
            mov ax_after, ax
        }
    } else {
        return 2;
    }

    printf("ax=%04x data=%02x%02x%02x%02x\n",
           ax_after,
           (unsigned)globals[0x005e], (unsigned)globals[0x005f],
           (unsigned)globals[0x0085], (unsigned)globals[0x00dd]);
    return 0;
}
