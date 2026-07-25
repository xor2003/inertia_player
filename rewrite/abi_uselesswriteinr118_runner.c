#include <stdio.h>
#include <string.h>

static unsigned char header_data[96];
static unsigned short ret_ax;
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;
static unsigned short ret_si;
static unsigned short ret_di;

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

void iplay_useless_writeinr_118_header(IplayRegs *r, unsigned char *mem);

static const unsigned char expected_header[96] = {
    0x49,0x6e,0x65,0x72,0x74,0x69,0x61,0x20,0x53,0x61,0x6d,0x70,0x6c,0x65,0x3a,0x20,
    0x53,0x48,0x4f,0x52,0x54,0x20,0x53,0x41,0x4d,0x50,0x4c,0x45,0x20,0x4e,0x41,0x4d,
    0x45,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x0d,0x0a,0x1a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x78,0x56,0x34,0x12,0x10,0x40,0x7f,0xa5,0x21,0x43,0x00,0x00,0x11,0x11,0x11,0x11,
    0x22,0x22,0x22,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void __far useless_writeinr_118(void);
#pragma aux useless_writeinr_118 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void __far useless_writeinr_118(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_useless_writeinr_118_header(&r, header_data);
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
    unsigned dx_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abiuselesswriteinr118")) return 2;

    memset(header_data, 0, sizeof(header_data));
    _asm {
        mov dx, 0001h
        call useless_writeinr_118
        mov dx_after, dx
    }

    printf("dx=%04x data=", dx_after);
    print_bytes(header_data, sizeof(header_data));
    printf("\n");
    return 0;
}
