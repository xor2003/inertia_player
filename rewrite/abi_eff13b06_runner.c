#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;

static db globals[0x200];
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

void iplay_eff_13b06(IplayRegs *r, db *globals, db flag_playsettings);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13B06(void);
#pragma aux eff_13B06 __parm __caller [] __modify __exact [__ax]
void eff_13B06(void) {
    IplayRegs r;

    r.eax = input_ax;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_eff_13b06(&r, globals, 0);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 4) return 2;
    if (!streq(argv[1], "abieff13b06")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x0050] = 0xaa;
    globals[0x0051] = 0xaa;
    input_ax = (dw)strtoul(argv[3], 0, 0);
    (void)strtoul(argv[2], 0, 0);

    _asm {
        mov ax, input_ax
        call eff_13B06
        mov ax_after, ax
    }

    printf("ax=%04x data=%02x%02x\n", ax_after, (unsigned)globals[0x0050], (unsigned)globals[0x0051]);
    return 0;
}
