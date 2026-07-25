#include <stdio.h>
#include <string.h>

static unsigned char globals[0x2000];

typedef struct IplayRegs {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebp;
    unsigned long esi;
    unsigned long edi;
} IplayRegs;

static unsigned short ret_ax;
static unsigned short ret_dx;

void iplay_sub_19050_bounded(IplayRegs *r, unsigned char *globals);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_19050(void);
#pragma aux sub_19050 __parm __caller [] __modify __exact [__ax __dx]
void sub_19050(void) {
    IplayRegs r;

    r.eax = 0;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_sub_19050_bounded(&r, globals);
    ret_ax = (unsigned short)r.eax;
    ret_dx = (unsigned short)r.edx;
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned dx_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abisub19050bounded")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x167e] = 7;
    _asm {
        call sub_19050
        mov ax_after, ax
        mov dx_after, dx
    }

    printf("ax=%04x dx=%04x data=%02x\n", ax_after, dx_after, globals[0x167e]);
    return 0;
}
