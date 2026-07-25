#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

#define SRC_BASE 0x2800u
#define DST_BASE 0x2840u

static db src[0x200];
static db dst[0x200];
static dw input_ax;
static dw ret_ax;
static dw ret_si;
static dw ret_di;

typedef struct IplayRegs {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd ebp;
    dd esi;
    dd edi;
} IplayRegs;

void iplay_put_message(IplayRegs *r, const db *src_mem, db *dst_mem, int initial_ax);
void iplay_text_1bf69(IplayRegs *r, const db *src_mem, db *dst_mem);

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

static void put_message_body(unsigned initial_ax) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_put_message(&r, src, dst, initial_ax != 0);
    ret_ax = (dw)r.eax;
    ret_si = (dw)(SRC_BASE + (dw)r.esi);
    ret_di = (dw)(DST_BASE + (dw)r.edi);
}

void put_message(void);
#pragma aux put_message __parm __caller [] __modify __exact [__ax __si __di]
void put_message(void) {
    put_message_body(0);
    _asm {
        mov ax, ret_ax
        mov si, ret_si
        mov di, ret_di
    }
}

void put_message2(void);
#pragma aux put_message2 __parm __caller [] __modify __exact [__ax __si __di]
void put_message2(void) {
    put_message_body(1);
    _asm {
        mov ax, ret_ax
        mov si, ret_si
        mov di, ret_di
    }
}

void text_1BF69(void);
#pragma aux text_1BF69 __parm __caller [] __modify __exact [__ax __si __di]
void text_1BF69(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = input_ax;
    iplay_text_1bf69(&r, src, dst);
    ret_ax = (dw)r.eax;
    ret_si = (dw)(SRC_BASE + (dw)r.esi);
    ret_di = (dw)(DST_BASE + (dw)r.edi);
    _asm {
        mov ax, ret_ax
        mov si, ret_si
        mov di, ret_di
    }
}

void write_scr(void);
#pragma aux write_scr __parm __caller [] __modify __exact [__ax __bp __si __di]
void write_scr(void) {
    unsigned si = 0;
    unsigned bp = 0;
    unsigned di;
    db ah;
    db al;
    dw delta;

    delta = (dw)src[0] | ((dw)src[1] << 8);
    di = bp + delta;
    si = 2;
    ah = src[si++];
    for (;;) {
        al = src[si++];
        if (al == 0) break;
        if (al == 1) {
            delta = (dw)src[si] | ((dw)src[si + 1u] << 8);
            di = bp + delta;
            si += 2;
            continue;
        }
        if (al == 2) {
            ah = src[si++];
            continue;
        }
        dst[di++] = al;
        dst[di++] = ah;
        ret_ax = (dw)(((dw)ah << 8) | al);
    }
    ret_ax = (dw)((dw)ah << 8);
    ret_si = (dw)(SRC_BASE + si);
    ret_di = (dw)(DST_BASE + di);
    _asm {
        mov ax, ret_ax
        mov si, ret_si
        mov di, ret_di
    }
}

int main(int argc, char **argv) {
    const char *op;
    const char *symbol;
    const char *text;
    unsigned attr;
    unsigned count;
    unsigned ax_after;
    unsigned si_after;
    unsigned di_after;
    size_t len;

    if (argc < 2) return 2;
    op = argv[1];
    memset(src, 0, sizeof(src));
    memset(dst, 0, sizeof(dst));

    if (streq(op, "abiputmessage")) {
        if (argc != 5) return 2;
        symbol = argv[2];
        text = argv[3];
        attr = (unsigned)strtoul(argv[4], 0, 0) & 0xffu;
        len = strlen(text);
        if (len == 0 || len > 64u) return 2;
        input_ax = (dw)(attr << 8);
        if (streq(symbol, "put_message")) {
            memcpy(src, text, len + 1u);
            _asm {
                call put_message
                mov ax_after, ax
                mov si_after, si
                mov di_after, di
            }
        } else if (streq(symbol, "put_message2")) {
            memcpy(src, text + 1u, len);
            input_ax = (dw)(input_ax | (db)text[0]);
            _asm {
                call put_message2
                mov ax_after, ax
                mov si_after, si
                mov di_after, di
            }
        } else {
            return 2;
        }
        printf("ax=%04x si=%04x di=%04x data=", ax_after, si_after, di_after);
        print_bytes(dst, (unsigned)len * 2u);
        printf("\n");
        return 0;
    }

    if (streq(op, "abitext1bf69")) {
        if (argc != 5) return 2;
        count = parse_hex_bytes(argv[2], src, sizeof(src));
        src[count] = 0;
        input_ax = (dw)(((unsigned)strtoul(argv[3], 0, 0) & 0xffu) << 8);
        _asm {
            call text_1BF69
            mov ax_after, ax
            mov si_after, si
            mov di_after, di
        }
        printf("ax=%04x si=%04x di=%04x data=", ax_after, si_after, di_after);
        print_bytes(dst, (unsigned)strtoul(argv[4], 0, 0));
        printf("\n");
        return 0;
    }

    if (streq(op, "abiwritescr")) {
        unsigned delta;
        if (argc != 5) return 2;
        text = argv[2];
        attr = (unsigned)strtoul(argv[3], 0, 0) & 0xffu;
        delta = (unsigned)strtoul(argv[4], 0, 0) & 0xffffu;
        len = strlen(text);
        if (len == 0 || len > 64u) return 2;
        src[0] = (db)delta;
        src[1] = (db)(delta >> 8);
        src[2] = (db)attr;
        memcpy(src + 3u, text, len + 1u);
        _asm {
            call write_scr
            mov ax_after, ax
            mov si_after, si
            mov di_after, di
        }
        printf("ax=%04x si=%04x di=%04x data=", ax_after, si_after, di_after);
        print_bytes(dst + delta, (unsigned)len * 2u);
        printf("\n");
        return 0;
    }

    return 2;
}
