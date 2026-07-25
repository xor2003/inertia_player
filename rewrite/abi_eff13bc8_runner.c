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

void iplay_eff_13bc8(IplayRegs *r, db *channel, db byte_2461a);

static db channel[0x40];
static db input_byte_2461a;
static dw input_ax;
static dw input_dx;
static dw ret_ax;
static dw ret_dx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13BC8(void);
#pragma aux eff_13BC8 __parm __caller [] __modify __exact [__ax __dx]
void eff_13BC8(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.edx = input_dx;
    iplay_eff_13bc8(&r, channel, input_byte_2461a);

    ret_ax = (dw)r.eax;
    ret_dx = (dw)r.edx;

    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned dx_after;

    if (argc != 5) return 2;
    if (!streq(argv[1], "abieff13bc8")) return 2;

    memset(channel, 0, sizeof(channel));
    input_byte_2461a = (db)strtoul(argv[2], 0, 0);
    input_dx = (dw)strtoul(argv[3], 0, 0);
    input_ax = (dw)strtoul(argv[4], 0, 0);

    _asm {
        mov dx, input_dx
        mov ax, input_ax
        call eff_13BC8
        mov ax_after, ax
        mov dx_after, dx
    }

    printf("ax=%04x dx=%04x data=%02x%02x%02x%02x\n",
           ax_after,
           dx_after,
           (unsigned)channel[0x14],
           (unsigned)channel[0x15],
           (unsigned)channel[0x38],
           (unsigned)channel[0x39]);
    return 0;
}
