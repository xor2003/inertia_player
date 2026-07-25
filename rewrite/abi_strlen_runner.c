#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

#define SRC_OFF 0x9000u
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

void iplay_mystrlen(IplayRegs *r, const db *mem);

static db mem[0x9208];
static dw input_si;
static dw ret_ax;
static dw ret_si;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void load_text(dw off, const char *text) {
    size_t n = strlen(text);
    memcpy(mem + off, text, n);
    mem[off + n] = 0;
}

void mystrlen_0(void);
#pragma aux mystrlen_0 __parm __caller [] __modify __exact [__ax __si]
void mystrlen_0(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.esi = input_si;
    iplay_mystrlen(&r, mem);
    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

void mystrlen(void);
#pragma aux mystrlen __parm __caller [] __modify __exact [__ax __si]
void mystrlen(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.esi = input_si;
    iplay_mystrlen(&r, mem);
    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned si_after;
    const char *text;

    if (argc != 3 && argc != 4) return 2;
    if (!streq(argv[1], "abistrlen")) return 2;

    memset(mem, 0, sizeof(mem));
    text = argc == 4 ? argv[3] : "";

    if (streq(argv[2], "mystrlen_0")) {
        input_si = SRC_OFF;
        load_text(SRC_OFF, text);
        _asm {
            mov si, 9000h
            call mystrlen_0
            mov ax_after, ax
            mov si_after, si
        }
    } else if (streq(argv[2], "mystrlen")) {
        input_si = DSEG_SCRATCH;
        load_text(DSEG_SCRATCH, text);
        _asm {
            mov si, 2800h
            call mystrlen
            mov ax_after, ax
            mov si_after, si
        }
    } else {
        return 2;
    }

    printf("ax=%04x si=%04x\n", ax_after, si_after);
    return 0;
}
