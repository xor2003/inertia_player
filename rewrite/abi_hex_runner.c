#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

#define ORIG_DST_OFF 0x9100u
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

void iplay_u4tox(IplayRegs *r, db *mem);
void iplay_u8tox(IplayRegs *r, db *mem);
void iplay_u16tox(IplayRegs *r, db *mem);
void iplay_u32tox(IplayRegs *r, db *mem);
void iplay_my_putdigit(IplayRegs *r, db *mem);

static db mem[0x9208];
static dd input_ax;
static db input_attr;
static dw input_cx;
static dw ret_ax;
static dw ret_cx;
static dw ret_si;
static dw ret_di;
static unsigned input_count;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

static db hex_digit(db value) {
    value = (db)(value & 0x0fu);
    value = (db)(value + '0');
    if (value > '9') value = (db)(value + 7);
    return value;
}

static void format_hex_at(dw start, unsigned nibbles) {
    unsigned i;
    dw si = start;
    for (i = 0; i < nibbles; ++i) {
        unsigned shift = (nibbles - 1u - i) * 4u;
        db ch = hex_digit((db)(input_ax >> shift));
        mem[si++] = ch;
        ret_ax = (dw)((ret_ax & 0xff00u) | ch);
    }
    ret_si = si;
}

static void format_hex(unsigned nibbles) {
    format_hex_at(ORIG_DST_OFF, nibbles);
}

void u4tox(void);
#pragma aux u4tox __parm __caller [] __modify __exact [__ax __si]
void u4tox(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.esi = ORIG_DST_OFF;
    iplay_u4tox(&r, mem);
    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

void u8tox(void);
#pragma aux u8tox __parm __caller [] __modify __exact [__ax __si]
void u8tox(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.esi = ORIG_DST_OFF;
    iplay_u8tox(&r, mem);
    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

void u16tox(void);
#pragma aux u16tox __parm __caller [] __modify __exact [__ax __si]
void u16tox(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.esi = ORIG_DST_OFF;
    iplay_u16tox(&r, mem);
    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

void u32tox(void);
#pragma aux u32tox __parm __caller [] __modify __exact [__si]
void u32tox(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.esi = ORIG_DST_OFF;
    iplay_u32tox(&r, mem);
    ret_si = (dw)r.esi;
    _asm {
        mov si, ret_si
    }
}

void hex_1BE39(void);
#pragma aux hex_1BE39 __parm __caller [] __modify __exact [__ax __di]
void hex_1BE39(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.esi = DSEG_SCRATCH;
    iplay_u4tox(&r, mem);
    mem[DSEG_SCRATCH + 1] = input_attr;
    ret_ax = (dw)(r.eax & 0xffu) | ((dw)input_attr << 8);
    ret_di = DSEG_SCRATCH + 2u;
    _asm {
        mov ax, ret_ax
        mov di, ret_di
    }
}

void my_u4tox(void);
#pragma aux my_u4tox __parm __caller [] __modify __exact [__ax __si]
void my_u4tox(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.esi = DSEG_SCRATCH;
    iplay_u4tox(&r, mem);
    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

void my_u8tox(void);
#pragma aux my_u8tox __parm __caller [] __modify __exact [__ax __si]
void my_u8tox(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.esi = DSEG_SCRATCH;
    iplay_u8tox(&r, mem);
    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

void my_u16tox(void);
#pragma aux my_u16tox __parm __caller [] __modify __exact [__ax __si]
void my_u16tox(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.esi = DSEG_SCRATCH;
    iplay_u16tox(&r, mem);
    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

void my_u32tox(void);
#pragma aux my_u32tox __parm __caller [] __modify __exact [__ax __si]
void my_u32tox(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    r.esi = DSEG_SCRATCH;
    iplay_u32tox(&r, mem);
    ret_ax = (dw)r.eax;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov si, ret_si
    }
}

void myputdigit(void);
#pragma aux myputdigit __parm __caller [] __modify __exact [__cx __si]
void myputdigit(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ecx = input_cx;
    r.edx = input_ax;
    r.esi = DSEG_SCRATCH;
    iplay_my_putdigit(&r, mem);
    ret_cx = (dw)r.ecx;
    ret_si = (dw)r.esi;
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_putdigit(void);
#pragma aux my_putdigit __parm __caller [] __modify __exact [__cx __si]
void my_putdigit(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ecx = input_cx;
    r.edx = input_ax;
    r.esi = ORIG_DST_OFF;
    iplay_my_putdigit(&r, mem);
    ret_cx = (dw)r.ecx;
    ret_si = (dw)r.esi;
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned cx_after;
    unsigned si_after;
    unsigned di_after;

    if (argc < 3) return 2;
    memset(mem, 0, sizeof(mem));

    if (streq(argv[1], "abihex4")) {
        input_ax = strtoul(argv[2], 0, 0);
        input_count = 1;
        _asm {
            mov ax, word ptr input_ax
            mov si, 9100h
            call u4tox
            mov ax_after, ax
            mov si_after, si
        }
    } else if (streq(argv[1], "abihex8")) {
        input_ax = strtoul(argv[2], 0, 0);
        input_count = 2;
        _asm {
            mov ax, word ptr input_ax
            mov si, 9100h
            call u8tox
            mov ax_after, ax
            mov si_after, si
        }
    } else if (streq(argv[1], "abihex16")) {
        input_ax = strtoul(argv[2], 0, 0);
        input_count = 4;
        _asm {
            mov ax, word ptr input_ax
            mov si, 9100h
            call u16tox
            mov ax_after, ax
            mov si_after, si
        }
    } else if (streq(argv[1], "abihex32")) {
        input_ax = strtoul(argv[2], 0, 0);
        input_count = 8;
        _asm {
            mov eax, dword ptr input_ax
            mov si, 9100h
            call u32tox
            mov si_after, si
        }
        printf("si=%04x data=", si_after);
        print_bytes(mem + ORIG_DST_OFF, input_count);
        printf("\n");
        return 0;
    } else if (streq(argv[1], "abihex1be39")) {
        if (argc != 4) return 2;
        input_ax = strtoul(argv[2], 0, 0);
        input_attr = (db)strtoul(argv[3], 0, 0);
        _asm {
            mov ax, word ptr input_ax
            mov di, 2800h
            call hex_1BE39
            mov ax_after, ax
            mov di_after, di
        }
        printf("ax=%04x di=%04x data=", ax_after, di_after);
        print_bytes(mem + DSEG_SCRATCH, 2);
        printf("\n");
        return 0;
    } else if (streq(argv[1], "abimyhex")) {
        if (argc != 4) return 2;
        input_ax = strtoul(argv[3], 0, 0);
        if (streq(argv[2], "my_u4tox")) {
            input_count = 1;
            _asm {
                mov ax, word ptr input_ax
                mov si, 2800h
                call my_u4tox
                mov ax_after, ax
                mov si_after, si
            }
        } else if (streq(argv[2], "my_u8tox")) {
            input_count = 2;
            _asm {
                mov ax, word ptr input_ax
                mov si, 2800h
                call my_u8tox
                mov ax_after, ax
                mov si_after, si
            }
        } else if (streq(argv[2], "my_u16tox")) {
            input_count = 4;
            _asm {
                mov ax, word ptr input_ax
                mov si, 2800h
                call my_u16tox
                mov ax_after, ax
                mov si_after, si
            }
        } else if (streq(argv[2], "my_u32tox")) {
            input_count = 8;
            _asm {
                mov ax, word ptr input_ax
                mov si, 2800h
                call my_u32tox
                mov ax_after, ax
                mov si_after, si
            }
        } else {
            return 2;
        }
        printf("ax=%04x si=%04x data=", ax_after, si_after);
        print_bytes(mem + DSEG_SCRATCH, input_count);
        printf("\n");
        return 0;
    } else if (streq(argv[1], "abimyputdigit")) {
        if (argc != 4) return 2;
        input_cx = (dw)strtoul(argv[2], 0, 0);
        input_ax = strtoul(argv[3], 0, 0);
        _asm {
            mov dx, word ptr input_ax
            mov cx, input_cx
            mov si, 2800h
            call myputdigit
            mov cx_after, cx
            mov si_after, si
        }
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + DSEG_SCRATCH, 1);
        printf("\n");
        return 0;
    } else if (streq(argv[1], "abiputdigit")) {
        if (argc != 4) return 2;
        input_cx = (dw)strtoul(argv[2], 0, 0);
        input_ax = strtoul(argv[3], 0, 0);
        _asm {
            mov dx, word ptr input_ax
            mov cx, input_cx
            mov si, 9100h
            call my_putdigit
            mov cx_after, cx
            mov si_after, si
        }
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, 1);
        printf("\n");
        return 0;
    } else {
        return 2;
    }

    printf("ax=%04x si=%04x data=", ax_after, si_after);
    print_bytes(mem + ORIG_DST_OFF, input_count);
    printf("\n");
    return 0;
}
