#include <stdio.h>
#include <string.h>

static unsigned char state[8];

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
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;

void iplay_stereo_timer_int_snapshot(IplayRegs *r, unsigned char *mem);
void iplay_timer_int_end_disabled(IplayRegs *r, unsigned char *mem);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void stereo_timer_int(void);
#pragma aux stereo_timer_int __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void stereo_timer_int(void) {
    IplayRegs r;

    r.eax = 0;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_stereo_timer_int_snapshot(&r, state);
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_cx = (unsigned short)r.ecx;
    ret_dx = (unsigned short)r.edx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void timer_int_end(void);
#pragma aux timer_int_end __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void timer_int_end(void) {
    IplayRegs r;

    r.eax = 0;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_timer_int_end_disabled(&r, state);
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_cx = (unsigned short)r.ecx;
    ret_dx = (unsigned short)r.edx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void int1a_timer(void);
#pragma aux int1a_timer __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void int1a_timer(void) {
    _asm {
        mov ax, 0100h
        mov bx, 8031h
        mov cx, 9abch
        mov dx, 0def0h
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abitimerint")) return 2;

    memset(state, 0xa5, sizeof(state));
    if (streq(argv[2], "stereo_timer_int")) {
        _asm {
            mov ax, 5678h
            mov bx, 9abch
            mov cx, 0def0h
            mov dx, 037ah
            call stereo_timer_int
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x data=",
               ax_after, bx_after, cx_after, dx_after, 0x156a);
        print_bytes(state, 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[2], "int1a_timer")) {
        _asm {
            mov ax, 0100h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call int1a_timer
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x es=%04x\n",
               ax_after, bx_after, cx_after, dx_after, 0x0a15, 0x0d8f);
        return 0;
    }

    if (streq(argv[2], "timer_int_end")) {
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call timer_int_end
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x data=",
               ax_after, bx_after, cx_after, dx_after, 0x156a);
        print_bytes(state, 3);
        printf("\n");
        return 0;
    }

    return 2;
}
