#include <stdio.h>
#include <string.h>

static unsigned char mem[0x1800];

typedef struct IplayRegs {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebp;
    unsigned long esi;
    unsigned long edi;
} IplayRegs;

void iplay_find_mods_no_nul_guard(IplayRegs *r, unsigned char *mem, unsigned short dseg);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void find_mods(void);
#pragma aux find_mods __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void find_mods(void) {
    IplayRegs r;

    r.eax = 0;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_find_mods_no_nul_guard(&r, mem, 0x0d8f);
    _asm {
        mov ax, 0d00h
        mov di, 2b05h
    }
}

int main(int argc, char **argv) {
    unsigned i;
    unsigned ax_after;
    unsigned di_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abifindmodsguard")) return 2;

    memset(mem, 0, sizeof(mem));
    for (i = 0; i < 120; ++i) mem[0x137c + i] = 'X';
    mem[0x168e] = 0xaa;
    mem[0x1640] = 0xbb;
    mem[0x1641] = 0xbb;
    mem[0x1642] = 0xcc;
    mem[0x1643] = 0xcc;

    _asm {
        call find_mods
        mov ax_after, ax
        mov di_after, di
    }

    printf("ax=%04x di=%04x data=", ax_after, di_after);
    print_bytes(mem + 0x168e, 1);
    print_bytes(mem + 0x1640, 4);
    printf("\n");
    return 0;
}
