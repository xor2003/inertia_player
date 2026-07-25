#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

#define SRC0_BASE 0x9000u
#define DST0_BASE 0x9100u
#define SRC1_BASE 0x2800u
#define DST1_BASE 0x2840u
#define SCRATCH_BASE 0x2800u

typedef struct IplayRegs {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd ebp;
    dd esi;
    dd edi;
} IplayRegs;

void iplay_strcpy_count(IplayRegs *r, const db *src_mem, db *dst_mem);
void iplay_copy_printable(IplayRegs *r, const db *src_mem, db *dst_mem);
void iplay_seg1_copy_printable(IplayRegs *r, const db *src_mem, db *dst_mem);
void iplay_myasmsprintf(IplayRegs *r, db *mem);
void iplay_get_comspec(IplayRegs *r, const db *env);
void iplay_getexename(IplayRegs *r, const db *env, db *dst);

static db src[0x200];
static db dst[0x200];
static db scratch[0x200];
static db fullmem[0x9300];
static dw ret_ax;
static dw ret_cx;
static dw ret_si;
static dw ret_di;
static unsigned input_count;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static int hex_nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static unsigned parse_hex_bytes(const char *s, db *out, unsigned max) {
    unsigned n = 0;
    while (s[0] != 0 && s[1] != 0 && n < max) {
        int hi = hex_nibble(s[0]);
        int lo = hex_nibble(s[1]);
        if (hi < 0 || lo < 0) break;
        out[n++] = (db)((hi << 4) | lo);
        s += 2;
    }
    return n;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

void strcpy_count_0(void);
#pragma aux strcpy_count_0 __parm __caller [] __modify __exact [__ax __cx __si __di]
void strcpy_count_0(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_strcpy_count(&r, src, dst);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_si = (dw)(SRC0_BASE + (dw)r.esi);
    ret_di = (dw)(DST0_BASE + (dw)r.edi);
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov si, ret_si
        mov di, ret_di
    }
}

void strcpy_count(void);
#pragma aux strcpy_count __parm __caller [] __modify __exact [__ax __cx __si __di]
void strcpy_count(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_strcpy_count(&r, src, dst);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_si = (dw)(SRC1_BASE + (dw)r.esi);
    ret_di = (dw)(DST1_BASE + (dw)r.edi);
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov si, ret_si
        mov di, ret_di
    }
}

void copy_printable(void);
#pragma aux copy_printable __parm __caller [] __modify __exact [__ax __cx __si __di]
void copy_printable(void) {
    IplayRegs r;
    unsigned i;

    memset(&r, 0, sizeof(r));
    r.ecx = input_count;
    iplay_copy_printable(&r, src, dst);
    ret_ax = (dw)r.eax;
    for (i = 0; i < input_count; ++i) {
        if (src[i] < 0x20u) break;
    }
    ret_cx = (dw)(input_count - i);
    ret_si = SRC0_BASE;
    ret_di = DST0_BASE;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov si, ret_si
        mov di, ret_di
    }
}

void cpy_printable(void);
#pragma aux cpy_printable __parm __caller [] __modify __exact [__ax __cx __si __di]
void cpy_printable(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ecx = input_count;
    iplay_seg1_copy_printable(&r, src, dst);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_si = SRC1_BASE;
    ret_di = DST1_BASE;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov si, ret_si
        mov di, ret_di
    }
}

void myasmsprintf(void);
#pragma aux myasmsprintf __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void myasmsprintf(void) {
    static const db fmt[] = {
        'U', '=', 4, 'u', 0x50, 0x28,
        ' ', 'I', '=', 8, 'i', 0x52, 0x28,
        ' ', 'X', '=', 11, 'x', 0x54, 0x28,
        0, 0
    };
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    memset(fullmem, 0, sizeof(fullmem));
    memcpy(fullmem + SCRATCH_BASE, fmt, sizeof(fmt));
    fullmem[SCRATCH_BASE + 0x50u] = 200;
    fullmem[SCRATCH_BASE + 0x52u] = 0x2e;
    fullmem[SCRATCH_BASE + 0x53u] = 0xfb;
    fullmem[SCRATCH_BASE + 0x54u] = 0xcd;
    fullmem[SCRATCH_BASE + 0x55u] = 0xab;
    r.esi = SCRATCH_BASE;
    r.edi = SCRATCH_BASE + 0x40u;
    iplay_myasmsprintf(&r, fullmem);
    memcpy(scratch + 0x40u, fullmem + SCRATCH_BASE + 0x40u, 20);
    ret_si = (dw)r.esi;
    ret_di = (dw)r.edi;
    _asm {
        mov si, ret_si
        mov di, ret_di
    }
}

void get_comspec(void);
#pragma aux get_comspec __parm __caller [] __modify __exact [__di]
void get_comspec(void) {
    static const db env[] = "COMSPEC=X\0";
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_get_comspec(&r, env);
    ret_di = (dw)r.edi;
    _asm {
        mov di, ret_di
    }
}

void getexename(void);
#pragma aux getexename __parm __caller [] __modify __exact [__ax __cx __di __si]
void getexename(void) {
    static const db env[] = {'A', '=', 'B', 0, 0, 1, 0, 'C', ':', '\\', 'I', 'P', 'L', 'A', 'Y', '.', 'E', 'X', 'E', 0};
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    memset(scratch, 0, sizeof(scratch));
    r.esi = 0;
    iplay_getexename(&r, env, scratch);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_di = (dw)r.edi;
    ret_si = (dw)(SCRATCH_BASE + (dw)r.esi);
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov di, ret_di
        mov si, ret_si
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned cx_after;
    unsigned si_after;
    unsigned di_after;
    unsigned dump_len;
    const char *symbol;
    const char *text;
    size_t len;

    if (argc < 2) return 2;
    memset(src, 0, sizeof(src));
    memset(dst, '.', sizeof(dst));

    if (streq(argv[1], "abistrcpy")) {
        if (argc != 3 && argc != 4) return 2;
        symbol = argv[2];
        text = argc == 4 ? argv[3] : "";
        len = strlen(text);
        if (len > 255u) return 2;
        memcpy(src, text, len + 1u);
        if (streq(symbol, "strcpy_count_0")) {
            _asm {
                call strcpy_count_0
                mov ax_after, ax
                mov cx_after, cx
                mov si_after, si
                mov di_after, di
            }
            dump_len = (unsigned)(len + 1u);
        } else if (streq(symbol, "strcpy_count")) {
            _asm {
                call strcpy_count
                mov ax_after, ax
                mov cx_after, cx
                mov si_after, si
                mov di_after, di
            }
            dump_len = (unsigned)len;
        } else {
            return 2;
        }
        printf("ax=%04x cx=%04x si=%04x di=%04x data=", ax_after, cx_after, si_after, di_after);
        print_bytes(dst, dump_len);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abicopyprint")) {
        if (argc != 5) return 2;
        symbol = argv[2];
        parse_hex_bytes(argv[3], src, sizeof(src));
        input_count = (unsigned)strtoul(argv[4], 0, 0);
        if (input_count > sizeof(dst)) return 2;
        if (streq(symbol, "copy_printable")) {
            _asm {
                call copy_printable
                mov ax_after, ax
                mov cx_after, cx
                mov si_after, si
                mov di_after, di
            }
        } else if (streq(symbol, "cpy_printable")) {
            _asm {
                call cpy_printable
                mov ax_after, ax
                mov cx_after, cx
                mov si_after, si
                mov di_after, di
            }
        } else {
            return 2;
        }
        printf("ax=%04x cx=%04x si=%04x di=%04x data=", ax_after, cx_after, si_after, di_after);
        print_bytes(dst, input_count);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimyasmsprintf")) {
        if (argc != 2) return 2;
        memset(scratch, 0, sizeof(scratch));
        _asm {
            mov si, 2800h
            mov di, 2840h
            call myasmsprintf
            mov si_after, si
            mov di_after, di
        }
        printf("si=%04x di=%04x data=", si_after, di_after);
        print_bytes(scratch + 0x40u, 20);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abigetcomspec")) {
        if (argc != 2) return 2;
        _asm {
            mov di, 0aaaah
            call get_comspec
            mov di_after, di
        }
        printf("di=%04x\n", di_after);
        return 0;
    }

    if (streq(argv[1], "abigetexename")) {
        if (argc != 2) return 2;
        memset(scratch, 0x2e, sizeof(scratch));
        _asm {
            mov si, 2800h
            call getexename
            mov si_after, si
        }
        printf("si=%04x data=", si_after);
        print_bytes(scratch, 13);
        printf("\n");
        return 0;
    }

    return 2;
}
