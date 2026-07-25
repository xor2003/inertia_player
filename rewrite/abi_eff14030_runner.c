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

void iplay_eff_14030(IplayRegs *r, db *globals, db byte_2467c, dw freq, dw buffer_size);

static db globals[0x200];
static dw input_ax;
static db input_byte_2467c;
static dw input_freq;
static dw input_buffer_size;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_14030(void);
#pragma aux eff_14030 __parm __caller [] __modify __exact [__ax]
void eff_14030(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_eff_14030(&r, globals, input_byte_2467c, input_freq, input_buffer_size);

    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 6) return 2;
    if (!streq(argv[1], "abieff14030")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x0089] = 0x20;
    input_ax = (dw)strtoul(argv[2], 0, 0);
    input_byte_2467c = (db)strtoul(argv[3], 0, 0);
    input_freq = (dw)strtoul(argv[4], 0, 0);
    input_buffer_size = (dw)strtoul(argv[5], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_14030
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
           ax_after,
           (unsigned)globals[0x00db], (unsigned)globals[0x00dc],
           (unsigned)globals[0x004a], (unsigned)globals[0x004b],
           (unsigned)globals[0x004c], (unsigned)globals[0x004d],
           (unsigned)globals[0x004e], (unsigned)globals[0x004f],
           (unsigned)globals[0x0088], (unsigned)globals[0x0089],
           (unsigned)globals[0x00c6]);
    return 0;
}
