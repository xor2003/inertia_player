#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned short channel_count;

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
static unsigned short ret_si;
static unsigned short ret_di;

void iplay_sub_13623_guard(IplayRegs *r, unsigned short channel_count);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_13623(void);
#pragma aux sub_13623 __parm __caller [] __modify __exact [__ax __dx __si __di]
void sub_13623(void) {
    IplayRegs r;
    unsigned short ax_in;
    unsigned short dx_in;
    unsigned short si_in;

    _asm {
        mov ax_in, ax
        mov dx_in, dx
        mov si_in, si
    }
    r.eax = ax_in;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = dx_in;
    r.ebp = 0;
    r.esi = si_in;
    r.edi = 0;
    iplay_sub_13623_guard(&r, channel_count);
    ret_ax = (unsigned short)r.eax;
    ret_dx = (unsigned short)r.edx;
    ret_si = (unsigned short)r.esi;
    ret_di = (unsigned short)r.edi;
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
        mov si, ret_si
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned dx_after;
    unsigned si_after;
    unsigned value;
    unsigned dx_value;

    if (argc != 5) return 2;
    if (!streq(argv[1], "abisub13623guard")) return 2;

    value = (unsigned)strtoul(argv[2], 0, 0);
    dx_value = (unsigned)strtoul(argv[3], 0, 0);
    channel_count = (unsigned short)strtoul(argv[4], 0, 0);

    _asm {
        mov ax, value
        mov dx, dx_value
        mov si, 2800h
        call sub_13623
        mov ax_after, ax
        mov dx_after, dx
        mov si_after, si
    }

    printf("ax=%04x dx=%04x si=%04x\n", ax_after, dx_after, si_after);
    return 0;
}
