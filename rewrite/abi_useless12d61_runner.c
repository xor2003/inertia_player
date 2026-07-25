#include <stdio.h>
#include <string.h>

static unsigned char useless12d61_data[8];
static unsigned short ret_ax;

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

void iplay_useless_12d61_no_device(IplayRegs *r, unsigned char *mem);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void useless_12D61(void);
#pragma aux useless_12D61 __parm __caller [] __modify __exact [__ax]
void useless_12D61(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_useless_12d61_no_device(&r, useless12d61_data);
    ret_ax = (unsigned short)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abiuseless12d61")) return 2;

    memset(useless12d61_data, 0x7e, sizeof(useless12d61_data));
    useless12d61_data[5] = 0x22;

    _asm {
        mov ax, 1234h
        call useless_12D61
        mov ax_after, ax
    }

    printf("ax=%04x data=", ax_after);
    print_bytes(useless12d61_data, sizeof(useless12d61_data));
    printf("\n");
    return 0;
}
