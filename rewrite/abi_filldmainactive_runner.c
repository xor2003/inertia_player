#include <stdio.h>
#include <string.h>

static unsigned char dma[8];

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

void iplay_fill_dma_inactive_mono(IplayRegs *r, unsigned char *mem, unsigned short dma_off);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void fill_dma(void);
#pragma aux fill_dma __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void fill_dma(void) {
    IplayRegs r;

    r.eax = 0;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_fill_dma_inactive_mono(&r, dma, 0);
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
    unsigned di_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abifilldmainactivemono")) return 2;

    memset(dma, 0xa5, sizeof(dma));
    _asm {
        call fill_dma
        mov di_after, di
    }

    printf("di=%04x data=", di_after);
    print_bytes(dma, 8);
    printf("\n");
    return 0;
}
