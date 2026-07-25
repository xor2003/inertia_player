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

void iplay_midi_15413_guard(IplayRegs *r, db *last_status);

static db last_status;
static dw input_ax;
static dw input_dx;
static dw ret_ax;
static dw ret_dx;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void midi_15413(void);
#pragma aux midi_15413 __parm __caller [] __modify __exact [__ax __dx]
void midi_15413(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.edx = input_dx;
    iplay_midi_15413_guard(&r, &last_status);

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

    if (argc != 3) return 2;
    if (!streq(argv[1], "abimidi15413guard")) return 2;

    last_status = (db)strtoul(argv[2], 0, 0);
    input_ax = (dw)(((dw)last_status << 8) | 0x34u);
    input_dx = 0x5678;

    _asm {
        mov ax, input_ax
        mov dx, input_dx
        call midi_15413
        mov ax_after, ax
        mov dx_after, dx
    }

    printf("ax=%04x dx=%04x data=%02x\n", ax_after, dx_after, (unsigned)last_status);
    return 0;
}
