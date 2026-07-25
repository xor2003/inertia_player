#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

#define DSEG_SCRATCH 0x2800u
#define MESSAGE_SRC  0x2d00u

static db mem[0x5000];
static char textsetup_symbol[32];
static unsigned short ax_after;
static unsigned short bx_after;
static unsigned short cx_after;
static unsigned short dx_after;
static unsigned short si_after;
static unsigned short di_after;

static db txtbottom_byte_1de72;
static db txtbottom_byte_1de73;
static db txtbottom_byte_1de74;
static db txtbottom_byte_1de75;
static db txtbottom_byte_1de76;
static db txtbottom_flags;
static dw txtbottom_volume;
static dw txtbottom_amplif;
static db message_y;
static db message_attr;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static unsigned long parse_u32(const char *s) {
    return strtoul(s, 0, 0);
}

static void print_bytes(const db *p, unsigned count) {
    unsigned i;
    for (i = 0; i < count; ++i) printf("%02x", (unsigned)p[i]);
}

static void set_textsetup_initial_state(void) {
    mem[0x164c] = 0xaa; mem[0x164d] = 0xaa;
    mem[0x164e] = 0xbb; mem[0x164f] = 0xbb;
    mem[0x1650] = 0xcc; mem[0x1651] = 0xcc;
    mem[0x1652] = 0xdd; mem[0x1653] = 0xdd;
    mem[0x1654] = 3; mem[0x1655] = 0;
    mem[0x167e] = 0xee; mem[0x167f] = 0xee;
    mem[0x1680] = 0;
    mem[0x1696] = 1;
    mem[0x1502] = 1;
}

static void run_textsetup_symbol(const char *symbol) {
    IplayRegs r;
    memset(&r, 0, sizeof(r));
    r.eax = 0x1234u;
    r.ebx = 0x5678u;
    r.ecx = 0x9abcu;
    r.edx = 0xdef0u;
    iplay_text_setup_small(&r, mem, symbol);
    ax_after = (dw)r.eax;
    bx_after = (dw)r.ebx;
    cx_after = (dw)r.ecx;
    dx_after = (dw)r.edx;
    _asm {
        mov ax, ax_after
        mov bx, bx_after
        mov cx, cx_after
        mov dx, dx_after
    }
}

void text_init(void);
#pragma aux text_init __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void text_init(void) {
    run_textsetup_symbol("text_init");
}

void text_init2(void);
#pragma aux text_init2 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void text_init2(void) {
    run_textsetup_symbol("text_init2");
}

void f1_help(void);
#pragma aux f1_help __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f1_help(void) {
    run_textsetup_symbol("f1_help");
}

void f3_textmetter(void);
#pragma aux f3_textmetter __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f3_textmetter(void) {
    run_textsetup_symbol("f3_textmetter");
}

void f4_patternnae(void);
#pragma aux f4_patternnae __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f4_patternnae(void) {
    run_textsetup_symbol("f4_patternnae");
}

void f6_undoc(void);
#pragma aux f6_undoc __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f6_undoc(void) {
    run_textsetup_symbol("f6_undoc");
}

void draw_frame(void);
#pragma aux draw_frame __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void draw_frame(void) {
    unsigned short ax_in;
    unsigned short bx_in;
    unsigned short cx_in;
    unsigned short dx_in;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    memset(mem + DSEG_SCRATCH, 0, 0x500);
    iplay_draw_frame(mem + DSEG_SCRATCH,
                     (db)ax_in,
                     (db)(ax_in >> 8),
                     (db)bx_in,
                     (db)cx_in,
                     (db)(cx_in >> 8),
                     (db)dx_in,
                     (db)(dx_in >> 8));
}

void txt_draw_top_title(void);
#pragma aux txt_draw_top_title __parm __caller [] __modify __exact []
void txt_draw_top_title(void) {
    memset(mem + DSEG_SCRATCH, 0, 0x500);
    iplay_txt_draw_top_title(mem + DSEG_SCRATCH);
}

void txt_draw_bottom(void);
#pragma aux txt_draw_bottom __parm __caller [] __modify __exact []
void txt_draw_bottom(void) {
    memset(mem + DSEG_SCRATCH, 0xcc, 0x600);
    iplay_txt_draw_bottom(mem + DSEG_SCRATCH,
                          txtbottom_byte_1de72,
                          txtbottom_byte_1de73,
                          txtbottom_byte_1de74,
                          txtbottom_byte_1de75,
                          txtbottom_byte_1de76,
                          txtbottom_flags,
                          txtbottom_volume,
                          txtbottom_amplif);
}

void message_1BE77(void);
#pragma aux message_1BE77 __parm __caller [] __modify __exact [__ax __si __di]
void message_1BE77(void) {
    IplayRegs r;
    memset(mem + DSEG_SCRATCH, 0, 0x500);
    memset(&r, 0, sizeof(r));
    r.eax = ((dw)message_attr << 8) | message_y;
    r.esi = MESSAGE_SRC;
    iplay_message_1be77(&r, mem, DSEG_SCRATCH);
    ax_after = (dw)r.eax;
    si_after = (dw)r.esi;
    di_after = (dw)r.edi;
    _asm {
        mov ax, ax_after
        mov si, si_after
        mov di, di_after
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 2;

    if (streq(argv[1], "abitextsetup")) {
        if (argc != 3) return 2;
        set_textsetup_initial_state();
        strncpy(textsetup_symbol, argv[2], sizeof(textsetup_symbol) - 1u);
        textsetup_symbol[sizeof(textsetup_symbol) - 1u] = 0;
        if (streq(textsetup_symbol, "text_init")) {
            _asm { call text_init }
        } else if (streq(textsetup_symbol, "text_init2")) {
            _asm { call text_init2 }
        } else if (streq(textsetup_symbol, "f1_help")) {
            _asm { call f1_help }
        } else if (streq(textsetup_symbol, "f3_textmetter")) {
            _asm { call f3_textmetter }
        } else if (streq(textsetup_symbol, "f4_patternnae")) {
            _asm { call f4_patternnae }
        } else if (streq(textsetup_symbol, "f6_undoc")) {
            _asm { call f6_undoc }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem + 0x164c, 8);
        print_bytes(mem + 0x167e, 2);
        print_bytes(mem + 0x1680, 1);
        print_bytes(mem + 0x1696, 1);
        print_bytes(mem + 0x162c, 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abidrawframe")) {
        unsigned style;
        unsigned attr;
        unsigned fill_attr;
        unsigned x;
        unsigned y;
        unsigned right;
        unsigned bottom;
        if (argc != 9) return 2;
        style = (unsigned)parse_u32(argv[2]);
        attr = (unsigned)parse_u32(argv[3]);
        fill_attr = (unsigned)parse_u32(argv[4]);
        x = (unsigned)parse_u32(argv[5]);
        y = (unsigned)parse_u32(argv[6]);
        right = (unsigned)parse_u32(argv[7]);
        bottom = (unsigned)parse_u32(argv[8]);
        _asm {
            mov ax, attr
            shl ax, 8
            or ax, style
            mov bx, fill_attr
            mov cx, y
            shl cx, 8
            or cx, x
            mov dx, bottom
            shl dx, 8
            or dx, right
            call draw_frame
        }
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 400);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abitxtdrawtoptitle")) {
        if (argc != 2) return 2;
        _asm { call txt_draw_top_title }
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 0x500);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abitxtdrawbottom")) {
        if (argc != 10) return 2;
        txtbottom_byte_1de72 = (db)parse_u32(argv[2]);
        txtbottom_byte_1de73 = (db)parse_u32(argv[3]);
        txtbottom_byte_1de74 = (db)parse_u32(argv[4]);
        txtbottom_byte_1de75 = (db)parse_u32(argv[5]);
        txtbottom_byte_1de76 = (db)parse_u32(argv[6]);
        txtbottom_flags = (db)parse_u32(argv[7]);
        txtbottom_volume = (dw)parse_u32(argv[8]);
        txtbottom_amplif = (dw)parse_u32(argv[9]);
        _asm { call txt_draw_bottom }
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 0x600);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimessage1be77")) {
        size_t len;
        if (argc != 5) return 2;
        memset(mem + MESSAGE_SRC, 0, 80);
        strncpy((char *)(mem + MESSAGE_SRC), argv[2], 79);
        len = strlen((const char *)(mem + MESSAGE_SRC));
        mem[MESSAGE_SRC + len] = 0;
        message_y = (db)parse_u32(argv[3]);
        message_attr = (db)parse_u32(argv[4]);
        _asm {
            call message_1BE77
            mov si_after, si
            mov di_after, di
        }
        printf("si=%04x di=%04x data=", si_after, di_after);
        print_bytes(mem + DSEG_SCRATCH, 1000);
        printf("\n");
        return 0;
    }

    return 2;
}
