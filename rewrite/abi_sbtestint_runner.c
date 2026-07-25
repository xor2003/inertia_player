#include <stdio.h>
#include <string.h>

static unsigned char counter;
static unsigned short ret_ax;
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;
static unsigned short ret_si;

typedef struct IplayRegs {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebp;
    unsigned long esi;
    unsigned long edi;
} IplayRegs;

void iplay_sb_test_interrupt_no_device(IplayRegs *r, unsigned char *counter);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sb_test_interrupt(void);
#pragma aux sb_test_interrupt __parm __caller [] __modify __exact [__ax __bx __cx __si]
void sb_test_interrupt(void) {
    IplayRegs r;
    unsigned short ax_in;
    unsigned short bx_in;
    unsigned short cx_in;
    unsigned short dx_in;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_sb_test_interrupt_no_device(&r, &counter);
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_cx = (unsigned short)r.ecx;
    ret_dx = (unsigned short)r.edx;
    ret_si = (unsigned short)r.esi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
        stc
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;
    unsigned si_after;
    unsigned flags_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abisbtestinterruptnodevice")) return 2;

    counter = 0xaa;
    _asm {
        mov ax, 1234h
        mov bx, 5678h
        mov cx, 9abch
        mov dx, 0def0h
        call sb_test_interrupt
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov dx_after, dx
        mov si_after, si
        pushf
        pop ax
        mov flags_after, ax
    }

    printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x flags=%04x data=%02x\n",
           ax_after,
           bx_after,
           cx_after,
           dx_after,
           si_after,
           flags_after,
           counter);
    return 0;
}
