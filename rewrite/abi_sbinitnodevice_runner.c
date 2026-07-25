#include <stdio.h>
#include <string.h>

static unsigned char globals[0x100];
static unsigned short ret_ax;
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;
static unsigned short ret_si;

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

void iplay_sb_legacy_init_no_device(IplayRegs *r, unsigned char *globals, int sbpro_mode);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

static void call_sb_legacy_init(const char *symbol) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sb_legacy_init_no_device(&r, globals, streq(symbol, "sbpro_init"));
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_cx = (unsigned short)r.ecx;
    ret_dx = (unsigned short)r.edx;
    ret_si = (unsigned short)r.esi;
}

void sbpro_init(void);
#pragma aux sbpro_init __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void sbpro_init(void) {
    call_sb_legacy_init("sbpro_init");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
        stc
    }
}

void sb_init(void);
#pragma aux sb_init __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void sb_init(void) {
    call_sb_legacy_init("sb_init");
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
    unsigned dx_after;
    unsigned flags_after;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abisbinitnodevice")) return 2;

    memset(globals, 0, sizeof(globals));
    if (streq(argv[2], "sbpro_init")) {
        _asm {
            call sbpro_init
            mov ax_after, ax
            mov dx_after, dx
            pushf
            pop ax
            mov flags_after, ax
        }
    } else if (streq(argv[2], "sb_init")) {
        _asm {
            call sb_init
            mov ax_after, ax
            mov dx_after, dx
            pushf
            pop ax
            mov flags_after, ax
        }
    } else {
        return 2;
    }

    printf("ax=%04x dx=%04x flags=%04x data=", ax_after, dx_after, flags_after);
    print_bytes(globals + 0x82, 3);
    printf("\n");
    return 0;
}
