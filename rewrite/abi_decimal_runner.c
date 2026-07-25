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

void iplay_my_u32toa(IplayRegs *r, db *mem, unsigned base);
void iplay_my_u8toa_10(IplayRegs *r, db *mem);
void iplay_my_u16toa_10(IplayRegs *r, db *mem);
void iplay_my_u32toa10(IplayRegs *r, db *mem);
void iplay_my_i8toa10(IplayRegs *r, db *mem);
void iplay_my_i16toa10(IplayRegs *r, db *mem);
void iplay_my_i32toa10(IplayRegs *r, db *mem);
void iplay_my_u32toa_fill(IplayRegs *r, db *mem, dw count, int with_pointer_prefix);

static db mem[0x9208];
static dw input_ax;
static dd input_eax;
static dd input_base;
static dw input_count;
static dw output_base;
static dw ret_cx;
static dw ret_di;
static dw ret_si;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

static void init_si_regs(IplayRegs *r, dd eax, dw si) {
    memset(r, 0, sizeof(*r));
    r->eax = eax;
    r->esi = si;
}

static void save_cx_si(const IplayRegs *r) {
    ret_cx = (dw)r->ecx;
    ret_si = (dw)r->esi;
}

static unsigned emit_unsigned_at(unsigned offset, unsigned long value) {
    char tmp[10];
    unsigned n = 0;
    unsigned i;

    do {
        tmp[n++] = (char)('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u);

    for (i = 0; i < n; ++i) {
        mem[output_base + offset + i] = (db)tmp[n - 1u - i];
    }
    return n;
}

static unsigned emit_unsigned_base_at(unsigned offset, unsigned long value, unsigned long base) {
    char tmp[33];
    unsigned n = 0;
    unsigned i;

    if (base < 2u || base > 16u) base = 10u;
    do {
        unsigned digit = (unsigned)(value % base);
        tmp[n++] = (char)(digit < 10u ? ('0' + digit) : ('A' + digit - 10u));
        value /= base;
    } while (value != 0u);

    for (i = 0; i < n; ++i) {
        mem[output_base + offset + i] = (db)tmp[n - 1u - i];
    }
    return n;
}

static void emit_unsigned(unsigned long value) {
    unsigned n = emit_unsigned_at(0, value);
    ret_cx = (dw)n;
    ret_si = (dw)(output_base + n);
}

static void emit_unsigned_base(unsigned long value, unsigned long base) {
    unsigned n = emit_unsigned_base_at(0, value, base);
    ret_cx = (dw)n;
    ret_si = (dw)(output_base + n);
}

static unsigned make_unsigned_decimal(char *out, unsigned long value) {
    char tmp[10];
    unsigned n = 0;
    unsigned i;

    do {
        tmp[n++] = (char)('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u);

    for (i = 0; i < n; ++i) {
        out[i] = tmp[n - 1u - i];
    }
    return n;
}

static void emit_u32_fill(unsigned prefix) {
    char digits[10];
    unsigned digits_n;
    unsigned count = input_count;
    unsigned start;
    unsigned spaces;
    unsigned i;
    unsigned dst = output_base;

    if (prefix) {
        mem[dst++] = 0x02;
        mem[dst++] = 0x7f;
    }

    digits_n = make_unsigned_decimal(digits, (unsigned long)input_eax);
    start = digits_n > count ? digits_n - count : 0;
    spaces = digits_n < count ? count - digits_n : 0;

    for (i = 0; i < spaces; ++i) {
        mem[dst++] = ' ';
    }
    for (i = start; i < digits_n; ++i) {
        if ((dst - output_base) >= count + (prefix ? 2u : 0u)) break;
        mem[dst++] = (db)digits[i];
    }
    ret_di = (dw)(output_base + count + (prefix ? 2u : 0u));
}

static void emit_signed(long value) {
    if (value < 0) {
        unsigned n;
        mem[output_base] = '-';
        n = emit_unsigned_at(1, (unsigned long)(-value));
        ret_cx = (dw)(n + 1u);
        ret_si = (dw)(output_base + n + 1u);
    } else {
        emit_unsigned((unsigned long)value);
    }
}

void my_u8toa_10(void);
#pragma aux my_u8toa_10 __parm __caller [] __modify __exact [__cx __si]
void my_u8toa_10(void) {
    IplayRegs r;

    init_si_regs(&r, input_ax & 0x00ffu, output_base);
    iplay_my_u8toa_10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_u16toa_10(void);
#pragma aux my_u16toa_10 __parm __caller [] __modify __exact [__cx __si]
void my_u16toa_10(void) {
    IplayRegs r;

    init_si_regs(&r, input_ax, output_base);
    iplay_my_u16toa_10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_i8toa10_0(void);
#pragma aux my_i8toa10_0 __parm __caller [] __modify __exact [__cx __si]
void my_i8toa10_0(void) {
    IplayRegs r;

    init_si_regs(&r, input_ax & 0x00ffu, output_base);
    iplay_my_i8toa10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_i16toa10_0(void);
#pragma aux my_i16toa10_0 __parm __caller [] __modify __exact [__cx __si]
void my_i16toa10_0(void) {
    IplayRegs r;

    init_si_regs(&r, input_ax, output_base);
    iplay_my_i16toa10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_u8toa10(void);
#pragma aux my_u8toa10 __parm __caller [] __modify __exact [__cx __si]
void my_u8toa10(void) {
    IplayRegs r;

    init_si_regs(&r, input_eax & 0x000000fful, output_base);
    iplay_my_u8toa_10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_u16toa10(void);
#pragma aux my_u16toa10 __parm __caller [] __modify __exact [__cx __si]
void my_u16toa10(void) {
    IplayRegs r;

    init_si_regs(&r, input_eax & 0x0000fffful, output_base);
    iplay_my_u16toa_10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_u32toa10(void);
#pragma aux my_u32toa10 __parm __caller [] __modify __exact [__cx __si]
void my_u32toa10(void) {
    IplayRegs r;

    init_si_regs(&r, input_eax, output_base);
    iplay_my_u32toa10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_i8toa10(void);
#pragma aux my_i8toa10 __parm __caller [] __modify __exact [__cx __si]
void my_i8toa10(void) {
    IplayRegs r;

    init_si_regs(&r, input_eax & 0x000000fful, output_base);
    iplay_my_i8toa10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_i16toa10(void);
#pragma aux my_i16toa10 __parm __caller [] __modify __exact [__cx __si]
void my_i16toa10(void) {
    IplayRegs r;

    init_si_regs(&r, input_eax & 0x0000fffful, output_base);
    iplay_my_i16toa10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_i32toa10(void);
#pragma aux my_i32toa10 __parm __caller [] __modify __exact [__cx __si]
void my_i32toa10(void) {
    IplayRegs r;

    init_si_regs(&r, input_eax, output_base);
    iplay_my_i32toa10(&r, mem);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_u32toa(void);
#pragma aux my_u32toa __parm __caller [] __modify __exact [__cx __si]
void my_u32toa(void) {
    IplayRegs r;
    unsigned base = (unsigned)input_base;

    if (base < 2u || base > 16u) base = 10u;
    init_si_regs(&r, input_eax, output_base);
    iplay_my_u32toa(&r, mem, base);
    save_cx_si(&r);
    _asm {
        mov cx, ret_cx
        mov si, ret_si
    }
}

void my_u32toa_fill(void);
#pragma aux my_u32toa_fill __parm __caller [] __modify __exact [__di]
void my_u32toa_fill(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_eax;
    r.edi = output_base;
    iplay_my_u32toa_fill(&r, mem, input_count, 0);
    ret_di = (dw)r.edi;
    _asm {
        mov di, ret_di
    }
}

void my_pnt_u32toa_fill(void);
#pragma aux my_pnt_u32toa_fill __parm __caller [] __modify __exact [__di]
void my_pnt_u32toa_fill(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_eax;
    r.edi = output_base;
    iplay_my_u32toa_fill(&r, mem, input_count, 1);
    ret_di = (dw)r.edi;
    _asm {
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    unsigned cx_after;
    unsigned di_after;
    unsigned si_after;

    if (argc == 3 && (streq(argv[1], "abiu32toa10") || streq(argv[1], "abii32toa10"))) {
        memset(mem, 0, sizeof(mem));
        input_eax = strtoul(argv[2], 0, 0);
        output_base = ORIG_DST_OFF;
        if (streq(argv[1], "abiu32toa10")) {
            _asm {
                mov eax, dword ptr input_eax
                mov si, 9100h
                call my_u32toa10
                mov cx_after, cx
                mov si_after, si
            }
        } else {
            _asm {
                mov eax, dword ptr input_eax
                mov si, 9100h
                call my_i32toa10
                mov cx_after, cx
                mov si_after, si
            }
        }
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, cx_after);
        printf("\n");
        return 0;
    }

    if (argc != 4 && argc != 5) return 2;
    if (!streq(argv[1], "abidecimal16") && !streq(argv[1], "abimyutoa10") && !streq(argv[1], "abimyitoa10") && !streq(argv[1], "abimyu32toa") && !streq(argv[1], "abimyu32toa0") && !streq(argv[1], "abifill")) return 2;

    memset(mem, 0, sizeof(mem));
    input_eax = strtoul(argv[3], 0, 0);
    input_ax = (dw)input_eax;
    input_base = strtoul(argv[3], 0, 0);
    input_count = argc == 5 ? (dw)strtoul(argv[4], 0, 0) : 0;
    if (streq(argv[1], "abimyu32toa") || streq(argv[1], "abimyu32toa0")) {
        input_eax = strtoul(argv[2], 0, 0);
        input_ax = (dw)input_eax;
        input_base = strtoul(argv[3], 0, 0);
    }
    output_base = (streq(argv[1], "abidecimal16") || streq(argv[1], "abimyu32toa0")) ? ORIG_DST_OFF : DSEG_SCRATCH;

    if (streq(argv[1], "abidecimal16") && streq(argv[2], "my_u8toa_10")) {
        _asm {
            mov ax, input_ax
            mov si, 9100h
            call my_u8toa_10
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abidecimal16") && streq(argv[2], "my_u16toa_10")) {
        _asm {
            mov ax, input_ax
            mov si, 9100h
            call my_u16toa_10
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abidecimal16") && streq(argv[2], "my_i8toa10_0")) {
        _asm {
            mov ax, input_ax
            mov si, 9100h
            call my_i8toa10_0
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abidecimal16") && streq(argv[2], "my_i16toa10_0")) {
        _asm {
            mov ax, input_ax
            mov si, 9100h
            call my_i16toa10_0
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abimyutoa10") && streq(argv[2], "my_u8toa10")) {
        _asm {
            mov eax, dword ptr input_eax
            mov si, 2800h
            call my_u8toa10
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abimyutoa10") && streq(argv[2], "my_u16toa10")) {
        _asm {
            mov eax, dword ptr input_eax
            mov si, 2800h
            call my_u16toa10
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abimyutoa10") && streq(argv[2], "my_u32toa10")) {
        _asm {
            mov eax, dword ptr input_eax
            mov si, 2800h
            call my_u32toa10
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abimyitoa10") && streq(argv[2], "my_i8toa10")) {
        _asm {
            mov eax, dword ptr input_eax
            mov si, 2800h
            call my_i8toa10
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abimyitoa10") && streq(argv[2], "my_i16toa10")) {
        _asm {
            mov eax, dword ptr input_eax
            mov si, 2800h
            call my_i16toa10
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abimyitoa10") && streq(argv[2], "my_i32toa10")) {
        _asm {
            mov eax, dword ptr input_eax
            mov si, 2800h
            call my_i32toa10
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abimyu32toa")) {
        _asm {
            mov eax, dword ptr input_eax
            mov ebx, dword ptr input_base
            xor cx, cx
            mov si, 2800h
            call my_u32toa
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abimyu32toa0")) {
        _asm {
            mov eax, dword ptr input_eax
            mov ebx, dword ptr input_base
            xor cx, cx
            mov si, 9100h
            call my_u32toa
            mov cx_after, cx
            mov si_after, si
        }
    } else if (streq(argv[1], "abifill") && streq(argv[2], "my_u32toa_fill")) {
        _asm {
            mov eax, dword ptr input_eax
            mov di, 2800h
            call my_u32toa_fill
            mov di_after, di
        }
        printf("di=%04x data=", di_after);
        print_bytes(mem + DSEG_SCRATCH, input_count);
        printf("\n");
        return 0;
    } else if (streq(argv[1], "abifill") && streq(argv[2], "my_pnt_u32toa_fill")) {
        _asm {
            mov eax, dword ptr input_eax
            mov di, 2800h
            call my_pnt_u32toa_fill
            mov di_after, di
        }
        printf("di=%04x data=", di_after);
        print_bytes(mem + DSEG_SCRATCH, input_count + 2u);
        printf("\n");
        return 0;
    } else {
        return 2;
    }

    printf("cx=%04x si=%04x data=", cx_after, si_after);
    print_bytes(mem + output_base, cx_after);
    printf("\n");
    return 0;
}
