#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned short word_245fa;
static unsigned short size1_value;
static unsigned short channels_value;
static unsigned char realloc_count;
static unsigned long module_type;

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
static unsigned short ret_si;
static unsigned short ret_di;

void iplay_sub_126a9(IplayRegs *r, unsigned short word_245fa, unsigned short size1,
                     unsigned short channels, unsigned char realloc_count,
                     unsigned long module_type);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void __far sub_126A9(void);
#pragma aux sub_126A9 __parm __caller [] __modify __exact [__ax __bx __cx __si __di __es]
void __far sub_126A9(void) {
    IplayRegs r;

    r.eax = 0;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_sub_126a9(&r, word_245fa, size1_value, channels_value,
                    realloc_count, module_type);
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_cx = (unsigned short)r.ecx;
    ret_si = (unsigned short)r.esi;
    ret_di = (unsigned short)r.edi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov si, ret_si
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned si_after;
    unsigned di_after;

    if (argc != 7) return 2;
    if (!streq(argv[1], "abisub126a9")) return 2;

    word_245fa = (unsigned short)strtoul(argv[2], 0, 0);
    size1_value = (unsigned short)strtoul(argv[3], 0, 0);
    channels_value = (unsigned short)strtoul(argv[4], 0, 0);
    realloc_count = (unsigned char)strtoul(argv[5], 0, 0);
    module_type = strtoul(argv[6], 0, 0);

    _asm {
        call sub_126A9
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov si_after, si
        mov di_after, di
    }

    printf("ax=%04x bx=%04x cx=%04x si=%04x di=%04x\n",
           ax_after, bx_after, cx_after, si_after, di_after);
    return 0;
}
