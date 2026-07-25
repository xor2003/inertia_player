#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char event_store[5];
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
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;
static unsigned short ret_si;
static unsigned short ret_di;

void iplay_sub_12cad_guard(IplayRegs *r, unsigned char *event_store, unsigned short channel_count);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void __far sub_12CAD(void);
#pragma aux sub_12CAD __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void __far sub_12CAD(void) {
    IplayRegs r;
    unsigned short bx_in;
    unsigned short cx_in;
    unsigned short dx_in;

    _asm {
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    r.eax = 0;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_sub_12cad_guard(&r, event_store, channel_count);
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_cx = (unsigned short)r.ecx;
    ret_dx = (unsigned short)r.edx;
    ret_si = (unsigned short)r.esi;
    ret_di = (unsigned short)r.edi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;
    unsigned si_after;
    unsigned ch;
    unsigned cl;
    unsigned bx_value;
    unsigned dx_value;
    unsigned short cx_word;
    unsigned short bx_word;
    unsigned short dx_word_in;

    if (argc != 7) return 2;
    if (!streq(argv[1], "abisub12cadguard")) return 2;

    ch = (unsigned)strtoul(argv[2], 0, 0);
    cl = (unsigned)strtoul(argv[3], 0, 0);
    bx_value = (unsigned)strtoul(argv[4], 0, 0);
    dx_value = (unsigned)strtoul(argv[5], 0, 0);
    channel_count = (unsigned short)strtoul(argv[6], 0, 0);
    cx_word = (unsigned short)(((ch & 0xffu) << 8) | (cl & 0xffu));
    bx_word = (unsigned short)bx_value;
    dx_word_in = (unsigned short)dx_value;
    memset(event_store, 0, sizeof(event_store));

    _asm {
        mov cx, cx_word
        mov bx, bx_word
        mov dx, dx_word_in
        call sub_12CAD
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov dx_after, dx
        mov si_after, si
    }

    printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
           ax_after, bx_after, cx_after, dx_after, si_after);
    print_bytes(event_store, sizeof(event_store));
    printf("\n");
    return 0;
}
