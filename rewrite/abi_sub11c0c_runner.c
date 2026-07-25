#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char mem[0x100];
static unsigned short ret_ax;
static unsigned short ret_bx;
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

void iplay_sub_11c0c(IplayRegs *r, const unsigned char *mem);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static unsigned hex_value(char ch) {
    if (ch >= '0' && ch <= '9') return (unsigned)(ch - '0');
    if (ch >= 'a' && ch <= 'f') return (unsigned)(ch - 'a' + 10);
    if (ch >= 'A' && ch <= 'F') return (unsigned)(ch - 'A' + 10);
    return 0;
}

static void parse_hex(const char *hex, unsigned char *dst, unsigned max_count) {
    unsigned i;
    for (i = 0; i < max_count && hex[i * 2u] && hex[i * 2u + 1u]; ++i) {
        dst[i] = (unsigned char)((hex_value(hex[i * 2u]) << 4) | hex_value(hex[i * 2u + 1u]));
    }
}

void sub_11C0C(void);
#pragma aux sub_11C0C __parm __caller [] __modify __exact [__ax __bx __si]
void sub_11C0C(void) {
    IplayRegs r;
    unsigned short ax_in;

    _asm {
        mov ax_in, ax
    }
    r.eax = ax_in;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_sub_11c0c(&r, mem);
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_si = (unsigned short)r.esi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov si, ret_si
    }
}

int main(int argc, char **argv) {
    unsigned count;
    unsigned ax_after;
    unsigned si_after;

    if (argc != 4) return 2;
    if (!streq(argv[1], "abisub11c0c")) return 2;

    count = (unsigned)strtoul(argv[2], 0, 0);
    memset(mem, 0, sizeof(mem));
    parse_hex(argv[3], mem, sizeof(mem));

    _asm {
        mov ax, count
        call sub_11C0C
        mov ax_after, ax
        mov si_after, si
    }

    printf("ax=%04x si=%04x\n", ax_after, si_after);
    return 0;
}
