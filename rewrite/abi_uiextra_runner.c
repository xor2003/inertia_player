#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

static db attrs[64];
static db mouse_state[7];
static db records[0x400];
static db fullmem[0x3200];
static dw input_ax;
static dw input_bx;
static dw ret_ax;
static dw ret_bx;
static dw ret_cx;
static dw ret_dx;
static dw ret_bp;
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

void iplay_recolor_txt(IplayRegs *r, db *mem);
int iplay_mouse_1c7a9(IplayRegs *r);
int iplay_mouse_1c7cf(IplayRegs *r, const db *mem);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
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

static dw get_word(const db *p, unsigned off) {
    return (dw)p[off] | ((dw)p[off + 1u] << 8);
}

void recolortxt(void);
#pragma aux recolortxt __parm __caller [] __modify __exact [__ax __bx __cx __di]
void recolortxt(void) {
    IplayRegs r;
    unsigned i;
    dw base = (dw)(((dw)input_ax * 160u) + (80u * 2u * 10u) + (8u * 2u) + 1u);

    memset(&r, 0, sizeof(r));
    memset(fullmem, 0, sizeof(fullmem));
    r.eax = input_ax;
    r.ebx = input_bx;
    for (i = 0; i < 64u; ++i) {
        fullmem[base + i * 2u] = attrs[i];
    }
    iplay_recolor_txt(&r, fullmem);
    for (i = 0; i < 64u; ++i) {
        attrs[i] = fullmem[base + i * 2u];
    }
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov di, ret_di
    }
}

void mouse_getpos(void);
#pragma aux mouse_getpos __parm __caller [] __modify __exact [__bx __cx __dx]
void mouse_getpos(void) {
    ret_bx = 0;
    ret_cx = 0;
    ret_dx = 0;
    _asm {
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

static void mouse_preserve_regs(void) {
    ret_bx = 0x1111u;
    ret_cx = 0x2222u;
    ret_dx = 0x3333u;
    _asm {
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void mouse_showcur(void);
#pragma aux mouse_showcur __parm __caller [] __modify __exact [__bx __cx __dx]
void mouse_showcur(void) {
    mouse_state[6] = 0;
    mouse_preserve_regs();
}

void mouse_hide2(void);
#pragma aux mouse_hide2 __parm __caller [] __modify __exact [__bx __cx __dx]
void mouse_hide2(void) {
    mouse_state[6] = 1;
    mouse_preserve_regs();
}

void mouse_show(void);
#pragma aux mouse_show __parm __caller [] __modify __exact [__bx __cx __dx]
void mouse_show(void) {
    mouse_state[6] = 0;
    mouse_preserve_regs();
}

void mouse_hide(void);
#pragma aux mouse_hide __parm __caller [] __modify __exact [__bx __cx __dx]
void mouse_hide(void) {
    mouse_state[6] = 1;
    mouse_preserve_regs();
}

void mouse_deinit(void);
#pragma aux mouse_deinit __parm __caller [] __modify __exact [__bx __cx __dx]
void mouse_deinit(void) {
    mouse_state[6] = 1;
    mouse_preserve_regs();
}

void mouse_init(void);
#pragma aux mouse_init __parm __caller [] __modify __exact [__bx __cx __dx]
void mouse_init(void) {
    mouse_state[6] = 0;
    mouse_preserve_regs();
}

void mouse_1C7A9(void);
#pragma aux mouse_1C7A9 __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void mouse_1C7A9(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = ret_ax;
    r.ebp = ret_bp;
    r.ecx = ret_cx;
    r.edx = ret_dx;
    r.esi = ret_si;
    r.edi = ret_di;
    iplay_mouse_1c7a9(&r);
    ret_ax = (dw)r.eax;
    ret_bp = (dw)r.ebp;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    ret_si = (dw)r.esi;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
        mov di, ret_di
    }
}

static int mouse_hit_test(void) {
    dw ax = ret_ax;
    dw bp = ret_bp;
    dw cx = ret_cx;
    dw dx = ret_dx;
    dw si = ret_si;
    dw di = ret_di;
    dw tmp;

    if (cx > si) {
        tmp = cx;
        cx = si;
        si = tmp;
    }
    if (dx > di) {
        tmp = dx;
        dx = di;
        di = tmp;
    }
    ret_cx = cx;
    ret_dx = dx;
    ret_si = si;
    ret_di = di;
    if (ax < cx || ax > si || bp < dx || bp > di) return 0;
    ret_ax = (dw)(ax - cx);
    ret_bp = (dw)(bp - dx);
    return 1;
}

void mouse_1C7CF(void);
#pragma aux mouse_1C7CF __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void mouse_1C7CF(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    memset(fullmem, 0, sizeof(fullmem));
    memcpy(fullmem + 0x2800u, records, sizeof(records));
    r.eax = ret_ax;
    r.ebp = ret_bp;
    r.ebx = ret_bx;
    iplay_mouse_1c7cf(&r, fullmem);
    ret_ax = (dw)r.eax;
    ret_bp = (dw)r.ebp;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    ret_si = (dw)r.esi;
    ret_di = (dw)r.edi;
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
    unsigned i;
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;

    if (argc < 2) return 2;

    if (streq(argv[1], "abirecolortxt")) {
        if (argc != 4) return 2;
        input_ax = (dw)strtoul(argv[2], 0, 0);
        input_bx = (dw)strtoul(argv[3], 0, 0);
        for (i = 0; i < 64u; ++i) attrs[i] = (db)(0xa0u | (i & 0x0fu));
        _asm {
            mov ax, input_ax
            mov bx, input_bx
            call recolortxt
            mov ax_after, ax
            mov bx_after, bx
        }
        printf("ax=%04x bx=%04x data=", ax_after, bx_after);
        print_bytes(attrs, 64);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimousegetpos")) {
        if (argc != 2) return 2;
        mouse_state[0] = 0xaa;
        mouse_state[1] = 0xaa;
        mouse_state[2] = 0xbb;
        mouse_state[3] = 0xbb;
        mouse_state[4] = 0xcc;
        mouse_state[5] = 0x00;
        mouse_state[6] = 0x01;
        _asm {
            mov bx, 1111h
            mov cx, 2222h
            mov dx, 3333h
            call mouse_getpos
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("bx=%04x cx=%04x dx=%04x data=", bx_after, cx_after, dx_after);
        print_bytes(mouse_state, 7);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimousecursor")) {
        if (argc != 3) return 2;
        mouse_state[0] = 0xaa;
        mouse_state[1] = 0xaa;
        mouse_state[2] = 0xbb;
        mouse_state[3] = 0xbb;
        mouse_state[4] = 0xcc;
        mouse_state[5] = 0x00;
        mouse_state[6] = streq(argv[2], "mouse_showcur") ? 0x00 : 0x01;
        if (streq(argv[2], "mouse_showcur")) {
            _asm {
                mov bx, 1111h
                mov cx, 2222h
                mov dx, 3333h
                call mouse_showcur
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "mouse_hide2")) {
            _asm {
                mov bx, 1111h
                mov cx, 2222h
                mov dx, 3333h
                call mouse_hide2
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("bx=%04x cx=%04x dx=%04x data=", bx_after, cx_after, dx_after);
        print_bytes(mouse_state, 7);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimousewrapper")) {
        if (argc != 3) return 2;
        mouse_state[0] = 0xaa;
        mouse_state[1] = 0xaa;
        mouse_state[2] = 0xbb;
        mouse_state[3] = 0xbb;
        mouse_state[4] = 0xcc;
        mouse_state[5] = 0x00;
        mouse_state[6] = streq(argv[2], "mouse_show") ? 0x00 : 0x01;
        if (streq(argv[2], "mouse_show")) {
            _asm {
                mov bx, 1111h
                mov cx, 2222h
                mov dx, 3333h
                call mouse_show
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "mouse_hide")) {
            _asm {
                mov bx, 1111h
                mov cx, 2222h
                mov dx, 3333h
                call mouse_hide
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("bx=%04x cx=%04x dx=%04x data=", bx_after, cx_after, dx_after);
        print_bytes(mouse_state, 7);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimousedeinit") || streq(argv[1], "abimouseinit")) {
        if (argc != 2) return 2;
        mouse_state[0] = 0xaa;
        mouse_state[1] = 0xaa;
        mouse_state[2] = 0xbb;
        mouse_state[3] = 0xbb;
        mouse_state[4] = 0xcc;
        mouse_state[5] = 0x00;
        mouse_state[6] = 0x01;
        if (streq(argv[1], "abimouseinit")) {
            _asm {
                mov bx, 1111h
                mov cx, 2222h
                mov dx, 3333h
                call mouse_init
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            _asm {
                mov bx, 1111h
                mov cx, 2222h
                mov dx, 3333h
                call mouse_deinit
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        }
        printf("bx=%04x cx=%04x dx=%04x data=", bx_after, cx_after, dx_after);
        print_bytes(mouse_state, 7);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimouse1c7a9")) {
        if (argc != 8) return 2;
        ret_ax = (dw)strtoul(argv[2], 0, 0);
        ret_bp = (dw)strtoul(argv[3], 0, 0);
        ret_cx = (dw)strtoul(argv[4], 0, 0);
        ret_dx = (dw)strtoul(argv[5], 0, 0);
        ret_si = (dw)strtoul(argv[6], 0, 0);
        ret_di = (dw)strtoul(argv[7], 0, 0);
        _asm {
            mov ax, ret_ax
            mov cx, ret_cx
            mov dx, ret_dx
            mov si, ret_si
            mov di, ret_di
            call mouse_1C7A9
            mov ax_after, ax
            mov cx_after, cx
            mov dx_after, dx
            mov ret_si, si
            mov ret_di, di
        }
        printf("ax=%04x bp=%04x cx=%04x dx=%04x si=%04x di=%04x\n",
               ax_after, ret_bp, cx_after, dx_after, ret_si, ret_di);
        return 0;
    }

    if (streq(argv[1], "abimouse1c7cf")) {
        if (argc != 5) return 2;
        memset(records, 0, sizeof(records));
        ret_ax = (dw)strtoul(argv[2], 0, 0);
        ret_bp = (dw)strtoul(argv[3], 0, 0);
        ret_bx = 0x2800u;
        parse_hex_bytes(argv[4], records, sizeof(records));
        _asm {
            call mouse_1C7CF
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov ret_si, si
            mov ret_di, di
        }
        printf("ax=%04x bx=%04x bp=%04x cx=%04x dx=%04x si=%04x di=%04x\n",
               ax_after, bx_after, ret_bp, cx_after, dx_after, ret_si, ret_di);
        return 0;
    }

    return 2;
}
