#include <stdio.h>
#include <stdlib.h>
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

void iplay_get_playsettings(IplayRegs *r, db flag_playsettings);
void iplay_set_playsettings(IplayRegs *r, db *globals, db *channels, dw channel_count, dw channel_stride);
void iplay_volume_12a66(IplayRegs *r, dw channel_count);
void iplay_vlm_141df(IplayRegs *r, db *globals, dw channel_count);
void iplay_change_volume(IplayRegs *r, db *globals, db *channels, dw channel_count);
db iplay_getset_playstate(IplayRegs *r, db play_state);
void iplay_get_12f7c(IplayRegs *r, dw word_245f0, dw word_245f6);
void iplay_memclean(IplayRegs *r, db *mem, dw size);
void iplay_sub_12afd(IplayRegs *r, db *channels, dw channel_count, db channel_index, db sndflags);
void iplay_sub_12b18(db *globals, db *channels, const db *src, dw channel_count, db sndflags);
void iplay_sub_12b83(IplayRegs *r, db *globals, db *channels, dw channel_stride, const db *types);
void iplay_someplaymode(db *globals, db *channels, dw channel_count, dw channel_stride);
void iplay_sub_12d05(IplayRegs *r, db *dst, db snd_init, db sndcard_type);

static db globals[0x200];
static db mem[0x2900];
static db channels_buf[0x50 * 32];
static db input_value;
static db input_request;
static dw input_channels;
static db input_channel_volume;
static dw input_size;
static dw input_fill_count;
static dw input_freq;
static db input_config_hi;
static db input_shift;
static db input_channel_index;
static db input_flags;
static db input_src[32];
static db channel_map[32][2];
static db channel_period_map[32][3];
static db channel_period[2];
static db message_buf[64];
static dw ret_ax;
static dw ret_bx;
static dw ret_cx;
static dw ret_dx;
static dw ret_di;
static dw ret_word2;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void set_dword(db *p, unsigned off, unsigned long value) {
    p[off] = (db)value;
    p[off + 1] = (db)(value >> 8);
    p[off + 2] = (db)(value >> 16);
    p[off + 3] = (db)(value >> 24);
}

void get_playsettings(void);
#pragma aux get_playsettings __parm __caller [] __modify __exact [__ax]
void get_playsettings(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = ret_ax;
    iplay_get_playsettings(&r, input_value);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void volume_12A66(void);
#pragma aux volume_12A66 __parm __caller [] __modify __exact [__ax __bx __cx]
void volume_12A66(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = 0x1234;
    r.ebx = 0x5678;
    r.ecx = 0x9abc;
    iplay_volume_12a66(&r, input_channels);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
    }
}

void vlm_141DF(void);
#pragma aux vlm_141DF __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void vlm_141DF(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = 0x1234;
    r.ebx = 0x5678;
    r.ecx = 0x9abc;
    r.edx = 0xdef0;
    iplay_vlm_141df(&r, globals, 1);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void change_volume(void);
#pragma aux change_volume __parm __caller [] __modify __exact [__ax __bx __cx]
void change_volume(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    memset(channels_buf, 0, sizeof(channels_buf));
    r.eax = ret_ax;
    globals[0x005c] = 0;
    globals[0x005d] = 1;
    globals[0x0108] = input_channel_volume;
    channels_buf[0x08] = input_channel_volume;
    iplay_change_volume(&r, globals, channels_buf, input_channels);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
    }
}

void memclean(void);
#pragma aux memclean __parm __caller [] __modify __exact [__ax __cx __di]
void memclean(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.edi = DSEG_SCRATCH;
    iplay_memclean(&r, mem, input_size);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov di, ret_di
    }
}

void set_playsettings(void);
#pragma aux set_playsettings __parm __caller [] __modify __exact [__ax]
void set_playsettings(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    memset(channels_buf, 0, sizeof(channels_buf));
    r.eax = input_value;
    globals[0x00d3] = input_config_hi;
    globals[0x00be] = (db)input_freq;
    globals[0x00bf] = (db)(input_freq >> 8);
    globals[0x007a] = input_shift;
    globals[0x0089] = 0x20;
    channels_buf[0x3e] = 0xaa;
    channels_buf[0x3f] = 0xaa;
    iplay_set_playsettings(&r, globals, channels_buf, input_channels, 0x50);
    channel_period[0] = channels_buf[0x3e];
    channel_period[1] = channels_buf[0x3f];
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void sub_12AFD(void);
#pragma aux sub_12AFD __parm __caller [] __modify __exact []
void sub_12AFD(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    memset(channels_buf, 0, sizeof(channels_buf));
    r.eax = ret_ax;
    if (input_channel_index < 32u) {
        channels_buf[(dw)input_channel_index * 0x50u + 0x17] = input_flags;
    }
    iplay_sub_12afd(&r, channels_buf, input_channels, input_channel_index, 0);
    if (input_channel_index < 32u) {
        input_flags = channels_buf[(dw)input_channel_index * 0x50u + 0x17];
    }
}

static int hex_nibble(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return 0;
}

static void parse_hex_bytes(const char *text, db *out, unsigned max_count) {
    unsigned i;
    for (i = 0; i < max_count && text[i * 2] && text[i * 2 + 1]; ++i) {
        out[i] = (db)((hex_nibble(text[i * 2]) << 4) | hex_nibble(text[i * 2 + 1]));
    }
}

void sub_12B18(void);
#pragma aux sub_12B18 __parm __caller [] __modify __exact []
void sub_12B18(void) {
    dw i;

    memset(channels_buf, 0, sizeof(channels_buf));
    iplay_sub_12b18(globals, channels_buf, input_src, input_channels, 0);
    for (i = 0; i < input_channels; ++i) {
        channel_map[i][0] = channels_buf[i * 0x50u + 0x18];
        channel_map[i][1] = channels_buf[i * 0x50u + 0x3a];
    }
}

void sub_12B83(void);
#pragma aux sub_12B83 __parm __caller [] __modify __exact []
void sub_12B83(void) {
    dw count;
    dw i;
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    memset(channels_buf, 0, sizeof(channels_buf));
    r.eax = input_value;
    globals[0x00de] = input_shift;
    globals[0x00be] = 0x22;
    globals[0x00bf] = 0x56;
    globals[0x005e] = 100;
    globals[0x005f] = 0;
    globals[0x0089] = 0x20;
    count = input_value;
    if (count >= 0x20u) count = 0x20u;
    if (count <= 2u) count = 2u;
    for (i = 0; i < count; ++i) {
        channels_buf[i * 0x50u + 0x3e] = 0xaa;
        channels_buf[i * 0x50u + 0x3f] = 0xaa;
    }
    iplay_sub_12b83(&r, globals, channels_buf, 0x50, input_src);
    for (i = 0; i < count; ++i) {
        channel_period_map[i][0] = channels_buf[i * 0x50u + 0x18];
        channel_period_map[i][1] = channels_buf[i * 0x50u + 0x3e];
        channel_period_map[i][2] = channels_buf[i * 0x50u + 0x3f];
    }
}

void someplaymode(void);
#pragma aux someplaymode __parm __caller [] __modify __exact []
void someplaymode(void) {
    memset(channels_buf, 0, sizeof(channels_buf));
    globals[0x00d2] = input_value;
    globals[0x00be] = (db)input_freq;
    globals[0x00bf] = (db)(input_freq >> 8);
    globals[0x007a] = input_shift;
    globals[0x0082] = input_config_hi;
    globals[0x0089] = 0x20;
    channels_buf[0x3e] = 0xaa;
    channels_buf[0x3f] = 0xaa;
    iplay_someplaymode(globals, channels_buf, input_channels, 0x50);
    channel_period[0] = channels_buf[0x3e];
    channel_period[1] = channels_buf[0x3f];
}

void sub_12D05(void);
#pragma aux sub_12D05 __parm __caller [] __modify __exact [__ax __cx __si __di]
void sub_12D05(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.edi = DSEG_SCRATCH;
    iplay_sub_12d05(&r, message_buf, input_value, input_config_hi);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov si, 1086h
        mov di, ret_di
    }
}

void getset_playstate(void);
#pragma aux getset_playstate __parm __caller [] __modify __exact [__ax]
void getset_playstate(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = ret_ax;
    iplay_getset_playstate(&r, input_value);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void get_12F7C(void);
#pragma aux get_12F7C __parm __caller [] __modify __exact [__ax __bx]
void get_12F7C(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_get_12f7c(&r, ret_ax, ret_word2);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
    }
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;

    if (argc < 2) return 2;
    memset(globals, 0, sizeof(globals));
    memset(mem, 0, sizeof(mem));

    if (streq(argv[1], "abigetplaysettings")) {
        if (argc != 3) return 2;
        input_value = (db)strtoul(argv[2], 0, 0);
        ret_ax = 0;
        _asm {
            xor ax, ax
            call get_playsettings
            mov ax_after, ax
        }
        printf("ax=%04x\n", ax_after);
        return 0;
    }

    if (streq(argv[1], "abivolume12a66")) {
        if (argc != 3) return 2;
        input_channels = (dw)strtoul(argv[2], 0, 0);
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            call volume_12A66
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
        }
        printf("ax=%04x bx=%04x cx=%04x\n", ax_after, bx_after, cx_after);
        return 0;
    }

    if (streq(argv[1], "abivlm141df")) {
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call vlm_141DF
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x\n",
               ax_after, bx_after, cx_after, dx_after, (unsigned)globals[0x00d1]);
        return 0;
    }

    if (streq(argv[1], "abichangevolume")) {
        if (argc != 5) return 2;
        ret_ax = (dw)strtoul(argv[2], 0, 0);
        input_channels = (dw)strtoul(argv[3], 0, 0);
        input_channel_volume = (db)strtoul(argv[4], 0, 0);
        _asm {
            mov ax, ret_ax
            call change_volume
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x%02x%02x\n",
               ax_after,
               (unsigned)globals[0x005c],
               (unsigned)globals[0x005d],
               (unsigned)globals[0x0108]);
        return 0;
    }

    if (streq(argv[1], "abimemclean")) {
        if (argc != 4) return 2;
        input_size = (dw)strtoul(argv[2], 0, 0);
        input_fill_count = (dw)strtoul(argv[3], 0, 0);
        memset(mem + DSEG_SCRATCH, 0xa5, input_fill_count);
        _asm {
            mov di, 2800h
            call memclean
            mov dx_after, di
        }
        printf("di=%04x data=", dx_after);
        print_bytes(mem + DSEG_SCRATCH, input_fill_count);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisetplaysettings")) {
        if (argc != 7) return 2;
        input_value = (db)strtoul(argv[2], 0, 0);
        input_config_hi = (db)strtoul(argv[3], 0, 0);
        input_freq = (dw)strtoul(argv[4], 0, 0);
        input_channels = (dw)strtoul(argv[5], 0, 0);
        input_shift = (db)strtoul(argv[6], 0, 0);
        (void)input_channels;
        _asm {
            mov ax, word ptr input_value
            call set_playsettings
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(globals + 0x00d2, 2);
        print_bytes(globals + 0x001c, 8);
        print_bytes(channel_period, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisub12afd")) {
        if (argc != 6) return 2;
        ret_ax = (dw)strtoul(argv[2], 0, 0);
        input_channels = (dw)strtoul(argv[3], 0, 0);
        input_channel_index = (db)strtoul(argv[4], 0, 0);
        input_flags = (db)strtoul(argv[5], 0, 0);
        _asm {
            mov ax, ret_ax
            mov cx, word ptr input_channel_index
            call sub_12AFD
        }
        printf("data=%02x\n", (unsigned)input_flags);
        return 0;
    }

    if (streq(argv[1], "abisub12b18")) {
        unsigned i;
        if (argc != 4) return 2;
        input_channels = (dw)strtoul(argv[2], 0, 0);
        parse_hex_bytes(argv[3], input_src, sizeof(input_src));
        _asm {
            call sub_12B18
        }
        printf("data=");
        print_bytes(globals + 0x007c, 2);
        for (i = 0; i < input_channels; ++i) {
            print_bytes(channel_map[i], 2);
        }
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisub12b83")) {
        unsigned i;
        unsigned count;
        if (argc != 5) return 2;
        input_value = (db)strtoul(argv[2], 0, 0);
        parse_hex_bytes(argv[3], input_src, sizeof(input_src));
        input_shift = (db)strtoul(argv[4], 0, 0);
        count = input_value;
        if (count >= 0x20u) count = 0x20u;
        if (count <= 2u) count = 2u;
        _asm {
            call sub_12B83
        }
        printf("data=");
        print_bytes(globals + 0x0034, 8);
        print_bytes(globals + 0x007c, 2);
        print_bytes(globals + 0x001c, 8);
        print_bytes(globals + 0x00dd, 2);
        for (i = 0; i < count; ++i) {
            print_bytes(channel_period_map[i], 3);
        }
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisomeplaymode")) {
        if (argc != 7) return 2;
        input_value = (db)strtoul(argv[2], 0, 0);
        input_freq = (dw)strtoul(argv[3], 0, 0);
        input_channels = (dw)strtoul(argv[4], 0, 0);
        input_shift = (db)strtoul(argv[5], 0, 0);
        input_config_hi = (db)strtoul(argv[6], 0, 0);
        _asm {
            call someplaymode
        }
        printf("data=");
        print_bytes(globals + 0x001c, 8);
        print_bytes(globals + 0x009c, 4);
        print_bytes(channel_period, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisub12d05")) {
        if (argc != 4) return 2;
        input_value = (db)strtoul(argv[2], 0, 0);
        input_config_hi = (db)strtoul(argv[3], 0, 0);
        (void)input_config_hi;
        _asm {
            mov di, 2800h
            call sub_12D05
            mov cx_after, cx
        }
        printf("cx=%04x data=", cx_after);
        print_bytes(message_buf, strlen("Device not initialised!"));
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abigetsetplaystate")) {
        if (argc != 4) return 2;
        input_value = (db)strtoul(argv[2], 0, 0);
        input_request = (db)strtoul(argv[3], 0, 0);
        ret_ax = input_request;
        _asm {
            mov ax, ret_ax
            call getset_playstate
            mov ax_after, ax
        }
        printf("ax=%04x\n", ax_after);
        return 0;
    }

    if (streq(argv[1], "abiget12f7c")) {
        if (argc != 4) return 2;
        ret_ax = (dw)strtoul(argv[2], 0, 0);
        ret_word2 = (dw)strtoul(argv[3], 0, 0);
        _asm {
            call get_12F7C
            mov ax_after, ax
            mov bx_after, bx
        }
        printf("ax=%04x bx=%04x\n", ax_after, bx_after);
        return 0;
    }

    return 2;
}
