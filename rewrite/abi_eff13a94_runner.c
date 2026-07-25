#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

static db channel[0x50];
static db input_byte_2461a;
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

void iplay_eff_13a94(IplayRegs *r, db *channel, db byte_2461a);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void put_dword(db *mem, unsigned off, dd value) {
    mem[off] = (db)value;
    mem[off + 1u] = (db)(value >> 8);
    mem[off + 2u] = (db)(value >> 16);
    mem[off + 3u] = (db)(value >> 24);
}

void eff_13A94(void);
#pragma aux eff_13A94 __parm __caller [] __modify __exact [__ax]
void eff_13A94(void) {
    IplayRegs r;

    r.eax = input_ax;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_eff_13a94(&r, channel, input_byte_2461a);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    dd sample_end;

    if (argc != 7) return 2;
    if (!streq(argv[1], "abieff13a94")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x16] = (db)strtoul(argv[2], 0, 0);
    sample_end = strtoul(argv[3], 0, 0);
    put_dword(channel, 0x30, sample_end);
    input_byte_2461a = (db)strtoul(argv[4], 0, 0);
    channel[0x17] = (db)strtoul(argv[5], 0, 0);
    channel[0x4c] = 0xaa;
    channel[0x4d] = 0xaa;
    input_ax = (dw)strtoul(argv[6], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13A94
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x%02x%02x\n",
           ax_after,
           (unsigned)channel[0x16],
           (unsigned)channel[0x17],
           (unsigned)channel[0x4c],
           (unsigned)channel[0x4d]);
    return 0;
}
