#include <stdio.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

#define DSEG_SCRATCH 0x2800u

typedef struct IplayRegs {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd ebp;
    dd esi;
    dd edi;
} IplayRegs;

void iplay_txt_1abae(IplayRegs *r, const db *src_mem, db *dst_mem);

static db mem[0x3000];
static dw ret_si;
static dw ret_di;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

static void join_args(char *out, unsigned out_size, int argc, char **argv, int start) {
    unsigned pos = 0;
    int i;
    if (out_size == 0) return;
    out[0] = 0;
    for (i = start; i < argc; ++i) {
        const char *s = argv[i];
        if (i != start && pos + 1 < out_size) out[pos++] = ' ';
        while (*s && pos + 1 < out_size) out[pos++] = *s++;
    }
    out[pos] = 0;
}

void txt_1ABAE(void);
#pragma aux txt_1ABAE __parm __caller [] __modify __exact [__si __di]
void txt_1ABAE(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.esi = DSEG_SCRATCH;
    r.edi = DSEG_SCRATCH + 0x40u;
    iplay_txt_1abae(&r, mem, mem);

    ret_si = (dw)r.esi;
    ret_di = (dw)r.edi;
    _asm {
        mov si, ret_si
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    char text[80];
    unsigned si_after;
    unsigned di_after;

    if (argc < 3) return 2;
    if (!streq(argv[1], "abitxt1abae")) return 2;

    memset(mem, 0, sizeof(mem));
    join_args(text, sizeof(text), argc, argv, 2);
    memcpy(mem + DSEG_SCRATCH, text, strlen(text));

    _asm {
        mov si, 2800h
        mov di, 2840h
        call txt_1ABAE
        mov si_after, si
        mov di_after, di
    }

    printf("si=%04x di=%04x data=", si_after, di_after);
    print_bytes(mem + DSEG_SCRATCH + 0x40u, 0x16u * 2u);
    printf("\n");
    return 0;
}
