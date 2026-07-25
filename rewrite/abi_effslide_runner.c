#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef signed short sw;
typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

static db channel[0x40];
static dw input_ax;
static db input_active;
static db input_max_volume;
static db input_playsettings;
static dw ret_ax;

typedef struct IplayRegs {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd ebp;
    dd esi;
    dd edi;
} IplayRegs;

void iplay_eff_13886(IplayRegs *r, db *channel);
void iplay_eff_138a4(IplayRegs *r, db *channel);
void iplay_eff_1387f(IplayRegs *r, db *channel, db active_channel);
void iplay_eff_1389d(IplayRegs *r, db *channel, db active_channel);
void iplay_eff_138d2(IplayRegs *r, db *channel);
void iplay_eff_1392f(IplayRegs *r, db *channel, db flag_playsettings);
void iplay_eff_139ac(IplayRegs *r, db *channel, db max_volume);
void iplay_eff_139b2(IplayRegs *r, db *channel, db max_volume, db flag_playsettings);
void iplay_eff_139b9(IplayRegs *r, db *channel, db max_volume);

static void call_eff138(IplayRegs *r) {
    memset(r, 0, sizeof(*r));
    r->eax = input_ax;
}

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static dw get_word(unsigned off) {
    return (dw)channel[off] | ((dw)channel[off + 1u] << 8);
}

static void put_word(unsigned off, dw value) {
    channel[off] = (db)value;
    channel[off + 1u] = (db)(value >> 8);
}

static void set_ax(dw value) {
    ret_ax = value;
}

static void eff_13886_body(void) {
    dw step = (dw)((db)input_ax) << 4;
    dw period = get_word(0);
    period = (period > step) ? (dw)(period - step) : 0;
    if (period < 0x00a0u) period = 0x00a0u;
    put_word(0, period);
    set_ax(period);
}

static void eff_138a4_body(void) {
    dw step = (dw)((db)input_ax) << 4;
    dd sum = (dd)get_word(0) + step;
    dw period = sum > 0x3580ul ? 0x3580u : (dw)sum;
    put_word(0, period);
    set_ax(period);
}

static void target_slide_body(void) {
    dw target = get_word(0x10);
    dw current = get_word(0);
    dw step;

    if (target == 0) return;
    step = get_word(0x12);
    if (target >= current) {
        dd next = (dd)current + step;
        if (next >= target) {
            current = target;
            put_word(0x10, 0);
            channel[0x17] &= 0xefu;
        } else {
            current = (dw)next;
        }
    } else {
        current = current > step ? (dw)(current - step) : 0;
        if ((sw)target >= (sw)current) {
            current = target;
            put_word(0x10, 0);
            channel[0x17] &= 0xefu;
        }
    }
    put_word(0, current);
    if ((channel[0x17] & 0x20u) != 0 && get_word(0x10) != 0) set_ax(0x0032);
    else set_ax(current);
}

static void volume_slide_body(void) {
    db al = (db)input_ax;
    db dl = channel[0x08];
    if ((al & 0xf0u) != 0) {
        al >>= 4;
        al = (db)(dl + al);
        if (al > input_max_volume) al = input_max_volume;
    } else {
        al &= 0x0fu;
        al = dl >= al ? (db)(dl - al) : 0;
    }
    channel[0x08] = al;
    set_ax((dw)((input_ax & 0xff00u) | al));
}

static void vibrato_body(db base_shift, db update_memory) {
    static const db wave[32] = {
        0x00, 0x18, 0x31, 0x4a, 0x61, 0x78, 0x8d, 0xa1,
        0xb4, 0xc5, 0xd4, 0xe0, 0xeb, 0xf4, 0xfa, 0xfd,
        0xff, 0xfd, 0xfa, 0xf4, 0xeb, 0xe0, 0xd4, 0xc5,
        0xb4, 0xa1, 0x8d, 0x78, 0x61, 0x4a, 0x31, 0x18
    };
    db al = (db)input_ax;
    db dl;
    dw ax;
    db dh;

    if (update_memory && al != 0) {
        db ch = al;
        dl = channel[0x0c];
        if ((al & 0x0fu) != 0) dl = (db)((dl & 0xf0u) | (al & 0x0fu));
        if ((ch & 0xf0u) != 0) dl = (db)((dl & 0x0fu) | (ch & 0xf0u));
        channel[0x0c] = dl;
    }
    al = (db)((channel[0x0d] >> 2) & 0x1fu);
    dl = (db)(channel[0x09] & 3u);
    if (dl != 0) {
        al <<= 3;
        if (dl == 1) {
            dl = al;
            if (channel[0x0d] & 0x80u) dl = (db)(0xffu - al);
        } else {
            dl = 0xffu;
        }
    } else {
        dl = wave[al];
    }
    ax = (dw)((dw)(channel[0x0c] & 0x0fu) * dl);
    ax >>= (db)(base_shift + (input_playsettings & 1u));
    if (channel[0x0d] & 0x80u) ax = (dw)(-((sw)ax));
    ax = (dw)(ax + get_word(0));
    dh = (db)((channel[0x0c] >> 2) & 0x3cu);
    channel[0x0d] = (db)(channel[0x0d] + dh);
    set_ax(ax);
}

void eff_1387F(void);
#pragma aux eff_1387F __parm __caller [] __modify __exact [__ax]
void eff_1387F(void) {
    IplayRegs r;

    call_eff138(&r);
    iplay_eff_1387f(&r, channel, input_active);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_13886(void);
#pragma aux eff_13886 __parm __caller [] __modify __exact [__ax]
void eff_13886(void) {
    IplayRegs r;

    call_eff138(&r);
    iplay_eff_13886(&r, channel);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_1389D(void);
#pragma aux eff_1389D __parm __caller [] __modify __exact [__ax]
void eff_1389D(void) {
    IplayRegs r;

    call_eff138(&r);
    iplay_eff_1389d(&r, channel, input_active);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_138A4(void);
#pragma aux eff_138A4 __parm __caller [] __modify __exact [__ax]
void eff_138A4(void) {
    IplayRegs r;

    call_eff138(&r);
    iplay_eff_138a4(&r, channel);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_138D2(void);
#pragma aux eff_138D2 __parm __caller [] __modify __exact [__ax]
void eff_138D2(void) {
    IplayRegs r;

    call_eff138(&r);
    iplay_eff_138d2(&r, channel);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_1392F(void);
#pragma aux eff_1392F __parm __caller [] __modify __exact [__ax]
void eff_1392F(void) {
    IplayRegs r;

    call_eff138(&r);
    iplay_eff_1392f(&r, channel, input_playsettings);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_139AC(void);
#pragma aux eff_139AC __parm __caller [] __modify __exact [__ax]
void eff_139AC(void) {
    IplayRegs r;

    call_eff138(&r);
    iplay_eff_139ac(&r, channel, input_max_volume);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_139B2(void);
#pragma aux eff_139B2 __parm __caller [] __modify __exact [__ax]
void eff_139B2(void) {
    IplayRegs r;

    call_eff138(&r);
    iplay_eff_139b2(&r, channel, input_max_volume, input_playsettings);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void eff_139B9(void);
#pragma aux eff_139B9 __parm __caller [] __modify __exact [__ax]
void eff_139B9(void) {
    IplayRegs r;

    call_eff138(&r);
    iplay_eff_139b9(&r, channel, input_max_volume);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

static void print_period(void) {
    printf("ax=%04x data=", ret_ax);
    print_bytes(channel, 2);
    printf("\n");
}

int main(int argc, char **argv) {
    const char *op;
    unsigned ax_after;
    dw period;
    dw target;
    dw step;

    if (argc < 2) return 2;
    op = argv[1];
    memset(channel, 0, sizeof(channel));

    if (streq(op, "abieffslide")) {
        const char *symbol;
        if (argc != 5 && argc != 6) return 2;
        symbol = argv[2];
        period = (dw)strtoul(argv[3], 0, 0);
        put_word(0, period);
        input_ax = (dw)strtoul(argv[4], 0, 0);
        input_active = argc == 6 ? (db)strtoul(argv[5], 0, 0) : 0;
        if (streq(symbol, "eff_1387F")) {
            _asm {
                call eff_1387F
                mov ax_after, ax
            }
        } else if (streq(symbol, "eff_13886")) {
            _asm {
                call eff_13886
                mov ax_after, ax
            }
        } else if (streq(symbol, "eff_1389D")) {
            _asm {
                call eff_1389D
                mov ax_after, ax
            }
        } else if (streq(symbol, "eff_138A4")) {
            _asm {
                call eff_138A4
                mov ax_after, ax
            }
        } else {
            return 2;
        }
        ret_ax = (dw)ax_after;
        print_period();
        return 0;
    }

    if (streq(op, "abieff138d2")) {
        if (argc != 7) return 2;
        period = (dw)strtoul(argv[2], 0, 0);
        target = (dw)strtoul(argv[3], 0, 0);
        step = (dw)strtoul(argv[4], 0, 0);
        put_word(0, period);
        put_word(0x10, target);
        put_word(0x12, step);
        channel[0x17] = (db)strtoul(argv[5], 0, 0);
        input_ax = (dw)strtoul(argv[6], 0, 0);
        _asm {
            call eff_138D2
            mov ax_after, ax
        }
        ret_ax = (dw)ax_after;
        printf("ax=%04x data=", ret_ax);
        print_bytes(channel, 2);
        print_bytes(channel + 0x10, 4);
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff1392f")) {
        if (argc != 8) return 2;
        period = (dw)strtoul(argv[2], 0, 0);
        put_word(0, period);
        channel[0x09] = (db)strtoul(argv[3], 0, 0);
        channel[0x0c] = (db)strtoul(argv[4], 0, 0);
        channel[0x0d] = (db)strtoul(argv[5], 0, 0);
        (void)strtoul(argv[6], 0, 0);
        input_playsettings = 0;
        input_ax = (dw)strtoul(argv[7], 0, 0);
        _asm {
            call eff_1392F
            mov ax_after, ax
        }
        ret_ax = (dw)ax_after;
        printf("ax=%04x data=", ret_ax);
        print_bytes(channel, 2);
        print_bytes(channel + 0x0c, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff139ac")) {
        if (argc != 9) return 2;
        period = (dw)strtoul(argv[2], 0, 0);
        target = (dw)strtoul(argv[3], 0, 0);
        step = (dw)strtoul(argv[4], 0, 0);
        put_word(0, period);
        put_word(0x10, target);
        put_word(0x12, step);
        channel[0x17] = (db)strtoul(argv[5], 0, 0);
        channel[0x08] = (db)strtoul(argv[6], 0, 0);
        input_max_volume = (db)strtoul(argv[7], 0, 0);
        input_ax = (dw)strtoul(argv[8], 0, 0);
        _asm {
            call eff_139AC
            mov ax_after, ax
        }
        ret_ax = (dw)ax_after;
        printf("ax=%04x data=", ret_ax);
        print_bytes(channel, 2);
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x10, 4);
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff139b2")) {
        if (argc != 10) return 2;
        period = (dw)strtoul(argv[2], 0, 0);
        put_word(0, period);
        channel[0x09] = (db)strtoul(argv[3], 0, 0);
        channel[0x0c] = (db)strtoul(argv[4], 0, 0);
        channel[0x0d] = (db)strtoul(argv[5], 0, 0);
        input_playsettings = (db)strtoul(argv[6], 0, 0);
        channel[0x08] = (db)strtoul(argv[7], 0, 0);
        input_max_volume = (db)strtoul(argv[8], 0, 0);
        input_ax = (dw)strtoul(argv[9], 0, 0);
        _asm {
            call eff_139B2
            mov ax_after, ax
        }
        ret_ax = (dw)ax_after;
        printf("ax=%04x data=", ret_ax);
        print_bytes(channel, 2);
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x0c, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff139b9")) {
        if (argc != 8) return 2;
        channel[0x08] = (db)strtoul(argv[2], 0, 0);
        channel[0x09] = (db)strtoul(argv[3], 0, 0);
        channel[0x0e] = (db)strtoul(argv[4], 0, 0);
        channel[0x0f] = (db)strtoul(argv[5], 0, 0);
        input_max_volume = (db)strtoul(argv[6], 0, 0);
        input_ax = (dw)strtoul(argv[7], 0, 0);
        _asm {
            call eff_139B9
            mov ax_after, ax
        }
        ret_ax = (dw)ax_after;
        printf("ax=%04x data=", ret_ax);
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x0e, 2);
        printf("\n");
        return 0;
    }

    return 2;
}
