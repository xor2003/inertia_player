#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

typedef struct IplayRegs {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd ebp;
    dd esi;
    dd edi;
} IplayRegs;

void iplay_int24(IplayRegs *r);
void iplay_mod_sub_delta(IplayRegs *r, db *mem, db flag, db reset, db *previous);
void iplay_clean_11c43(db *mem, db flag_playsettings, db byte_2461e, db byte_2461f);
void iplay_txt_blink_no_device(IplayRegs *r, int enable);
void iplay_rtc_clock_bcd_123456(IplayRegs *r, db *mem);
void iplay_deinit_125b9_idle(IplayRegs *r, db *mem);
void iplay_loadcfg_success(IplayRegs *r, db *mem);
void iplay_dosexec_no_comspec(IplayRegs *r, db *mem);
void iplay_ult_1150b(IplayRegs *r, dw value);
void iplay_useless_11787_zero(IplayRegs *r, db *mem, dw channel);
void iplay_useless_unset_egaseq(IplayRegs *r, db mode_bits);
void iplay_timer_port_no_device(IplayRegs *r, db *mem, const char *symbol, dw ax_value);
void iplay_useless_doswrite2_header(IplayRegs *r, db *mem);
void iplay_useless_doswrite_header(IplayRegs *r, db *mem);
void iplay_sb_helper_no_device(IplayRegs *r, const char *symbol, dw base_port);
void iplay_sb_write_no_device(IplayRegs *r);
void iplay_midi_port_no_device(IplayRegs *r, db *mem, const char *symbol);
void iplay_midi_153f1_public(IplayRegs *r);
void iplay_midi_set_no_device(IplayRegs *r);
void iplay_memfree_invalid(IplayRegs *r);
void iplay_mem_limit(IplayRegs *r, dd size);
void iplay_memfree_125da_guard(IplayRegs *r);
void iplay_mem_reallocx_bookkeeping(db *mem, dw size);
void iplay_memalloc12k_bounded(IplayRegs *r);
void iplay_mem_strategy(IplayRegs *r, const char *symbol, dw config_word);
void iplay_alloc_dma_fail(IplayRegs *r, db *mem, dd size, dw channel);
void iplay_callsubx_fail(db *mem);
void iplay_set_dmachn_mask_no_device(IplayRegs *r, dw channel);
void iplay_gravis_dma_control(IplayRegs *r, db *mem, const char *symbol);
void iplay_sub_1279a_dma(IplayRegs *r, db *mem);
void iplay_program_dma_channel1(IplayRegs *r, db *mem);
void iplay_adlib_delay_no_device(IplayRegs *r, const char *symbol);
void iplay_ega_seq_no_device(IplayRegs *r, int set_mode);
void iplay_clean_deinit_no_device(IplayRegs *r, db *mem, const char *symbol);
void iplay_sb16_probe_no_device(IplayRegs *r, db *mem, const char *symbol);
void iplay_sb16_init_fail(IplayRegs *r, db *mem);
void iplay_sb16_int_ack(IplayRegs *r, db *mem);
void iplay_sb16_dma_fail(IplayRegs *r, db *mem);
void iplay_sb16_off_no_device(IplayRegs *r, db *mem, const char *symbol);
void iplay_dos_dir_stub(IplayRegs *r, db *mem, int chdir_mode);
void iplay_dos_findnext_fail(IplayRegs *r, db *mem);
void iplay_dos_fread_eof(IplayRegs *r, db *mem);
void iplay_dos_seek_success(IplayRegs *r, db *mem);
void iplay_read2buffer_public_layout(IplayRegs *r, db *mem);
void iplay_readallmoules_bounded(IplayRegs *r, db *mem);
void iplay_readmodule_fail(db *mem);
void iplay_moduleread_fail(db *mem);
void iplay_modules_search_bounded(db *mem);
void iplay_init_vga_bounded(db *mem);
void iplay_f2_draw_bounded(db *mem);
void iplay_inr_read_119b7_eof(IplayRegs *r, db *mem);
void iplay_inr_read_118b0_fail(IplayRegs *r);
void iplay_midi_channel_event_no_device(IplayRegs *r, db *globals, db *channel, int note_off);
void iplay_snd_vector_roundtrip(IplayRegs *r, db *mem, db irq, dw old_off, dw old_seg);
void iplay_get_keybsw(IplayRegs *r, db *mem, dw value);
void iplay_set_keybsw(IplayRegs *r, db *mem, dw value);
void iplay_mod_readfile_11f4e_guard(IplayRegs *r, db *mem);
void iplay_mod_readfile_12247_eof(IplayRegs *r, db *mem);
void iplay_modread_10311_bounded(db *mem);
void iplay_modnt_bounded(db *mem);
void iplay_start_bounded(IplayRegs *r, db *mem);
void iplay_keyb_bounded(db *mem);
void iplay_format_loader_header(db *mem, const char *symbol);
void iplay_noop(IplayRegs *r);

static db ems_enabled;
static db realloc_count;
static db payload[16];
static db mem[0x9200];
static db midi_globals[0x200];
static db flag_playsettings;
static db byte_2461e;
static db byte_2461f;
static db mod_delta_flag;
static db mod_delta_reset;
static db mod_delta_previous;
static db current_max;
static db midi_status0;
static db midi_status1;
static dw mem_size_arg;
static dw mem_strategy_arg;
static dw alloc_dma_channel;
static dw snd_irq_arg;
static dw snd_old_off;
static dw snd_old_seg;
static dw keybsw_arg;
static dw sub197f2_arg;
static dw ult_word_arg;
static dw dmafill_count;
static dw memfree_segment;
static unsigned long mem_size_long;
static dw timer_word;
static dw vector_off_store[256];
static dw vector_seg_store[256];
static dw ret_ax;
static dw ret_bx;
static dw ret_cx;
static dw ret_dx;
static dw ret_es;
static dw ret_si;
static dw ret_di;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

void int24(void);
#pragma aux int24 __parm __caller [] __modify __exact [__ax]
void int24(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.eax = ret_ax;
    iplay_int24(&r);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void ems_restore_mapctx(void);
#pragma aux ems_restore_mapctx __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void ems_restore_mapctx(void) {
    ret_ax = 0x1234u;
    ret_bx = 0x5678u;
    ret_cx = 0x9abcu;
    ret_dx = 0xdef0u;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void ems_init(void);
#pragma aux ems_init __parm __caller [] __modify __exact [__ax]
void ems_init(void) {
    ems_enabled = 0;
    ret_ax = 1;
    _asm {
        mov ax, ret_ax
    }
}

void ems_guard(void);
#pragma aux ems_guard __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void ems_guard(void) {
    ems_restore_mapctx();
}

void ems_mapmemx(void);
#pragma aux ems_mapmemx __parm __caller [] __modify __exact []
void ems_mapmemx(void) {
}

void ems_mapmemy(void);
#pragma aux ems_mapmemy __parm __caller [] __modify __exact []
void ems_mapmemy(void) {
}

void ems_realloc2(void);
#pragma aux ems_realloc2 __parm __caller [] __modify __exact [__ax __cx]
void ems_realloc2(void) {
    realloc_count = (db)(realloc_count + 1u);
    ret_ax = 8;
    ret_cx = 0xffffu;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
    }
}

void clean_11c43(void);
#pragma aux clean_11c43 __parm __caller [] __modify __exact []
void clean_11c43(void) {
    db compact[57];

    memset(mem, 0, sizeof(mem));
    memset(compact, 0, sizeof(compact));
    iplay_clean_11c43(mem, flag_playsettings, byte_2461e, byte_2461f);
    compact[2] = mem[0x0034];
    compact[3] = mem[0x0035];
    compact[4] = mem[0x0036];
    compact[5] = mem[0x0037];
    compact[10] = mem[0x003e];
    compact[11] = mem[0x003f];
    compact[22] = mem[0x005e];
    compact[23] = mem[0x005f];
    compact[27] = mem[0x0090];
    compact[28] = mem[0x0091];
    compact[29] = mem[0x00d9];
    compact[30] = mem[0x00da];
    compact[32] = mem[0x0130];
    compact[33] = mem[0x0131];
    compact[46] = mem[0x3c48];
    compact[47] = mem[0x3c49];
    compact[48] = mem[0x3c4a];
    compact[49] = mem[0x3c4b];
    compact[54] = mem[0x3628];
    compact[55] = mem[0x3629];
    compact[56] = mem[0x362a];
    memcpy(mem, compact, sizeof(compact));
}

void mod_sub_delta(void);
#pragma aux mod_sub_delta __parm __caller [] __modify __exact [__ax __cx __si]
void mod_sub_delta(void) {
    IplayRegs r;
    dw si_reg;
    dw cx_reg;
    dw mem_base;

    _asm {
        mov si_reg, si
        mov cx_reg, cx
    }
    mem_base = (dw)(unsigned)mem;
    memset(&r, 0, sizeof(r));
    r.ecx = cx_reg;
    r.esi = (dw)(si_reg - mem_base);
    iplay_mod_sub_delta(&r, mem, mod_delta_flag, mod_delta_reset, &mod_delta_previous);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_si = (dw)(mem_base + (dw)r.esi);
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov si, ret_si
    }
}

void sub_11BA6(void);
#pragma aux sub_11BA6 __parm __caller [] __modify __exact [__ax __cx __dx __di]
void sub_11BA6(void) {
    dw cx_reg;
    dw bx_reg;
    dw dx_reg;
    dw di_reg;
    db ch;
    db cl;
    db bl;
    db bh;
    _asm {
        mov cx_reg, cx
        mov bx_reg, bx
        mov dx_reg, dx
        mov di_reg, di
    }
    ch = (db)((cx_reg >> 8) & 0x1fu);
    cl = (db)cx_reg;
    bl = (db)bx_reg;
    bh = (db)(bx_reg >> 8);
    if (!((bl == 0 || bl == 0xffu) && (bh == 0 || bh == 0xffu))) ch |= 0x20u;
    if (cl <= 0x40u) ch |= 0x40u;
    if (dx_reg != 0) {
        if ((db)dx_reg == 0) dx_reg = (dw)((dx_reg & 0xff00u) | 0x1du);
        ch |= 0x80u;
    }
    if ((ch & 0xe0u) != 0) {
        *((db *)di_reg) = ch;
        ++di_reg;
        if (ch & 0x80u) {
            *((db *)di_reg) = (db)dx_reg;
            ++di_reg;
            *((db *)di_reg) = (db)(dx_reg >> 8);
            ++di_reg;
        }
        if (ch & 0x40u) {
            *((db *)di_reg) = cl;
            ++di_reg;
        }
        if (ch & 0x20u) {
            *((db *)di_reg) = (db)bx_reg;
            ++di_reg;
            *((db *)di_reg) = (db)(bx_reg >> 8);
            ++di_reg;
        }
        if ((ch & 0x1fu) > current_max) current_max = (db)(ch & 0x1fu);
    }
    ret_ax = (dw)(ch & 0x1fu);
    ret_cx = (dw)(((dw)ch << 8) | cl);
    ret_dx = dx_reg;
    ret_di = di_reg;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov dx, ret_dx
        mov di, ret_di
    }
}

void txt_blinkingoff(void);
#pragma aux txt_blinkingoff __parm __caller [] __modify __exact [__ax __bx]
void txt_blinkingoff(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ecx = ret_cx;
    r.edx = ret_dx;
    iplay_txt_blink_no_device(&r, 0);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
    }
}

void txt_enableblink(void);
#pragma aux txt_enableblink __parm __caller [] __modify __exact [__ax __bx]
void txt_enableblink(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ecx = ret_cx;
    r.edx = ret_dx;
    iplay_txt_blink_no_device(&r, 1);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
    }
}

void set_timer(void);
#pragma aux set_timer __parm __caller [] __modify __exact [__ax]
void set_timer(void) {
    IplayRegs r;
    dw ax_reg;

    _asm {
        mov ax_reg, ax
    }
    timer_word = ax_reg;
    memset(&r, 0, sizeof(r));
    iplay_timer_port_no_device(&r, mem, "set_timer", ax_reg);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void clean_timer(void);
#pragma aux clean_timer __parm __caller [] __modify __exact [__ax]
void clean_timer(void) {
    IplayRegs r;
    dw ax_reg;

    _asm {
        mov ax_reg, ax
    }
    memset(&r, 0, sizeof(r));
    iplay_timer_port_no_device(&r, mem, "clean_timer", ax_reg);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void setint_vect(void);
#pragma aux setint_vect __parm __caller [] __modify __exact [__dx]
void setint_vect(void) {
    dw ax_reg;
    dw bx_reg;
    dw dx_reg;
    _asm {
        mov ax_reg, ax
        mov bx_reg, bx
        mov dx_reg, dx
    }
    vector_off_store[ax_reg & 0xffu] = bx_reg;
    vector_seg_store[ax_reg & 0xffu] = dx_reg;
    ret_dx = bx_reg;
    _asm {
        mov dx, ret_dx
    }
}

void getint_vect(void);
#pragma aux getint_vect __parm __caller [] __modify __exact [__ax __bx __dx]
void getint_vect(void) {
    dw ax_reg;
    _asm {
        mov ax_reg, ax
    }
    ret_ax = (dw)(0x3500u | (ax_reg & 0xffu));
    ret_bx = vector_off_store[ax_reg & 0xffu];
    ret_dx = vector_seg_store[ax_reg & 0xffu];
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov dx, ret_dx
    }
}

void initclockfromrtc(void);
#pragma aux initclockfromrtc __parm __caller [] __modify __exact [__ax __dx __es]
void initclockfromrtc(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_rtc_clock_bcd_123456(&r, mem);
    ret_ax = (dw)r.eax;
    ret_dx = (dw)r.edx;
    ret_es = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
        mov es, ret_dx
    }
}

void rereadrtc_settmr(void);
#pragma aux rereadrtc_settmr __parm __caller [] __modify __exact [__ax __dx __es]
void rereadrtc_settmr(void) {
    initclockfromrtc();
}

void useless_11787(void);
#pragma aux useless_11787 __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void useless_11787(void) {
    IplayRegs r;
    dw di_reg;

    _asm {
        mov di_reg, di
    }
    memset(&r, 0, sizeof(r));
    iplay_useless_11787_zero(&r, mem, 0x1334u);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    ret_di = di_reg;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov di, ret_di
    }
}

void useless_doswrite2(void);
#pragma aux useless_doswrite2 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void useless_doswrite2(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_useless_doswrite2_header(&r, mem);
    ret_ax = (dw)r.eax;
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

void useless_doswrite(void);
#pragma aux useless_doswrite __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void useless_doswrite(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_useless_doswrite_header(&r, mem);
    ret_ax = (dw)r.eax;
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

void useless_unset_egaseq(void);
#pragma aux useless_unset_egaseq __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void useless_unset_egaseq(void) {
    IplayRegs r;
    dw ax_reg;

    _asm {
        mov ax_reg, ax
    }
    memset(&r, 0, sizeof(r));
    iplay_useless_unset_egaseq(&r, (db)(ax_reg & 3u));
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

void ult_1150B(void);
#pragma aux ult_1150B __parm __caller [] __modify __exact [__ax __cx __dx]
void ult_1150B(void) {
    IplayRegs r;
    dw ax_reg;
    dw cx_reg;
    dw dx_reg;

    _asm {
        mov ax_reg, ax
        mov cx_reg, cx
        mov dx_reg, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_reg;
    r.ecx = cx_reg;
    r.edx = dx_reg;
    iplay_ult_1150b(&r, ax_reg);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

static void preserve_bx_cx_dx(void) {
    _asm {
        mov ret_bx, bx
        mov ret_cx, cx
        mov ret_dx, dx
    }
}

void ReadSB(void);
#pragma aux ReadSB __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void ReadSB(void) {
    IplayRegs r;
    dw ax_reg;
    dw dx_reg;

    preserve_bx_cx_dx();
    _asm {
        mov ax_reg, ax
        mov dx_reg, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_reg;
    r.ebx = ret_bx;
    r.ecx = ret_cx;
    r.edx = dx_reg;
    iplay_sb_helper_no_device(&r, "ReadSB", dx_reg);
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

void ReadMixerSB(void);
#pragma aux ReadMixerSB __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void ReadMixerSB(void) {
    ReadSB();
}

void WriteSB(void);
#pragma aux WriteSB __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void WriteSB(void) {
    IplayRegs r;
    dw ax_reg;
    dw dx_reg;

    preserve_bx_cx_dx();
    _asm {
        mov ax_reg, ax
        mov dx_reg, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_reg;
    r.ebx = ret_bx;
    r.ecx = ret_cx;
    r.edx = dx_reg;
    iplay_sb_write_no_device(&r);
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

void WriteMixerSB(void);
#pragma aux WriteMixerSB __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void WriteMixerSB(void) {
    WriteSB();
}

void CheckSB(void);
#pragma aux CheckSB __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void CheckSB(void) {
    IplayRegs r;
    dw ax_reg;
    dw dx_reg;

    preserve_bx_cx_dx();
    _asm {
        mov ax_reg, ax
        mov dx_reg, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_reg;
    r.ebx = ret_bx;
    r.ecx = ret_cx;
    r.edx = dx_reg;
    iplay_sb_helper_no_device(&r, "CheckSB", 0x0220u);
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

static void call_midi_port_helper(const char *symbol) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_midi_port_no_device(&r, mem, symbol);
    midi_status0 = mem[0];
    midi_status1 = mem[1];
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
}

void midi_clean(void);
#pragma aux midi_clean __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_clean(void) {
    call_midi_port_helper("midi_clean");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void midi_sndoff(void);
#pragma aux midi_sndoff __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_sndoff(void) {
    call_midi_port_helper("midi_sndoff");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void midi_153C0(void);
#pragma aux midi_153C0 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_153C0(void) {
    call_midi_port_helper("midi_153C0");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void midi_153D6(void);
#pragma aux midi_153D6 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_153D6(void) {
    call_midi_port_helper("midi_153D6");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void midi_153F1(void);
#pragma aux midi_153F1 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_153F1(void) {
    IplayRegs r;
    dw ax_reg;

    _asm {
        mov ax_reg, ax
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_reg;
    iplay_midi_153f1_public(&r);
    midi_status0 = 0x55u;
    midi_status1 = 0xa0u;
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

void midi_15442(void);
#pragma aux midi_15442 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_15442(void) {
    call_midi_port_helper("midi_15442");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void midi_set(void);
#pragma aux midi_set __parm __caller [] __modify __exact [__ax __bx __dx]
void midi_set(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_midi_set_no_device(&r);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov dx, ret_dx
    }
}

void memfree(void);
#pragma aux memfree __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void memfree(void) {
    IplayRegs r;

    (void)memfree_segment;
    memset(&r, 0, sizeof(r));
    iplay_memfree_invalid(&r);
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

void memalloc(void);
#pragma aux memalloc __parm __caller [] __modify __exact [__ax __bx]
void memalloc(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_mem_limit(&r, mem_size_long);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
    }
}

void memrealloc(void);
#pragma aux memrealloc __parm __caller [] __modify __exact [__ax __bx]
void memrealloc(void) {
    memalloc();
}

void memfree_125DA(void);
#pragma aux memfree_125DA __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void memfree_125DA(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    r.ebx = 0x5678u;
    r.ecx = 0x9abcu;
    r.edx = 0xdef0u;
    iplay_memfree_125da_guard(&r);
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

void mem_reallocx(void);
#pragma aux mem_reallocx __parm __caller [] __modify __exact [__di]
void mem_reallocx(void) {
    dw di_reg;

    _asm {
        mov di_reg, di
    }
    iplay_mem_reallocx_bookkeeping(mem, di_reg);
    ret_di = di_reg;
    _asm {
        mov di, ret_di
    }
}

void memalloc12k(void);
#pragma aux memalloc12k __parm __caller [] __modify __exact [__ax __bx __di __es]
void memalloc12k(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_memalloc12k_bounded(&r);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov di, ret_di
        mov es, ret_ax
    }
}

static void call_mem_strategy_helper(const char *symbol) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_mem_strategy(&r, symbol, mem_strategy_arg);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
}

void setmemalloc1(void);
#pragma aux setmemalloc1 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void setmemalloc1(void) {
    call_mem_strategy_helper("setmemalloc1");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void setmemalloc2(void);
#pragma aux setmemalloc2 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void setmemalloc2(void) {
    call_mem_strategy_helper("setmemalloc2");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void setmemallocstrat(void);
#pragma aux setmemallocstrat __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void setmemallocstrat(void) {
    call_mem_strategy_helper("setmemallocstrat");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void getmemallocstrat(void);
#pragma aux getmemallocstrat __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void getmemallocstrat(void) {
    call_mem_strategy_helper("getmemallocstrat");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void alloc_dma_buf(void);
#pragma aux alloc_dma_buf __parm __caller [] __modify __exact [__ax __bx __cx]
void alloc_dma_buf(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_alloc_dma_fail(&r, mem, mem_size_long, alloc_dma_channel);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
    }
}

void callsubx(void);
#pragma aux callsubx __parm __caller [] __modify __exact []
void callsubx(void) {
    iplay_callsubx_fail(mem);
}

void set_dmachn_mask(void);
#pragma aux set_dmachn_mask __parm __caller [] __modify __exact [__ax]
void set_dmachn_mask(void) {
    IplayRegs r;
    dw ax_reg;
    dw cx_reg;

    _asm {
        mov ax_reg, ax
        mov cx_reg, cx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_reg;
    iplay_set_dmachn_mask_no_device(&r, cx_reg);
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void sb_detect_irq(void);
#pragma aux sb_detect_irq __parm __caller [] __modify __exact [__ax __dx]
void sb_detect_irq(void) {
    ret_ax = 0;
    ret_dx = 0x0ff6u;
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
        stc
    }
}

void set_timer_int(void);
#pragma aux set_timer_int __parm __caller [] __modify __exact []
void set_timer_int(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_noop(&r);
}

void sub_182DB(void);
#pragma aux sub_182DB __parm __caller [] __modify __exact [__ax __cx]
void sub_182DB(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_gravis_dma_control(&r, mem, "sub_182DB");
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
    }
}

void nongravis_dma(void);
#pragma aux nongravis_dma __parm __caller [] __modify __exact [__ax __cx]
void nongravis_dma(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_gravis_dma_control(&r, mem, "nongravis_dma");
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
    }
}

void sub_1279A(void);
#pragma aux sub_1279A __parm __caller [] __modify __exact [__ax __cx]
void sub_1279A(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sub_1279a_dma(&r, mem);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
    }
}

void program_dma(void);
#pragma aux program_dma __parm __caller [] __modify __exact [__ax __cx __dx]
void program_dma(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_program_dma_channel1(&r, mem);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void adlib_18389(void);
#pragma aux adlib_18389 __parm __caller [] __modify __exact [__ax]
void adlib_18389(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_adlib_delay_no_device(&r, "adlib_18389");
    ret_ax = (dw)r.eax;
    _asm {
        mov ax, ret_ax
    }
}

void adlib_18395(void);
#pragma aux adlib_18395 __parm __caller [] __modify __exact []
void adlib_18395(void) {
}

void set_egasequencer(void);
#pragma aux set_egasequencer __parm __caller [] __modify __exact [__ax __dx]
void set_egasequencer(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_ega_seq_no_device(&r, 1);
    ret_ax = (dw)r.eax;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
    }
}

static void clean_deinit_common(const char *symbol) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_clean_deinit_no_device(&r, mem, symbol);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
}

void clean_int8_mem_timr(void);
#pragma aux clean_int8_mem_timr __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void clean_int8_mem_timr(void) {
    clean_deinit_common("clean_int8_mem_timr");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void covox_deinit(void);
#pragma aux covox_deinit __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void covox_deinit(void) {
    clean_deinit_common("covox_deinit");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void stereo_deinit(void);
#pragma aux stereo_deinit __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void stereo_deinit(void) {
    clean_deinit_common("stereo_deinit");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void adlib_clean(void);
#pragma aux adlib_clean __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void adlib_clean(void) {
    clean_deinit_common("adlib_clean");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void pcspeaker_clean(void);
#pragma aux pcspeaker_clean __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void pcspeaker_clean(void) {
    clean_deinit_common("pcspeaker_clean");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void deinit_125B9(void);
#pragma aux deinit_125B9 __parm __caller [] __modify __exact []
void deinit_125B9(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_deinit_125b9_idle(&r, mem);
}

void loadcfg(void);
#pragma aux loadcfg __parm __caller [] __modify __exact []
void loadcfg(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_loadcfg_success(&r, mem);
}

void dosexec(void);
#pragma aux dosexec __parm __caller [] __modify __exact []
void dosexec(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_dosexec_no_comspec(&r, mem);
}

static void fill_a5(unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) mem[i] = 0xa5u;
}

void dosgetcurdir(void);
#pragma aux dosgetcurdir __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void dosgetcurdir(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_dos_dir_stub(&r, mem, 0);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
    }
}

void doschdir(void);
#pragma aux doschdir __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void doschdir(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_dos_dir_stub(&r, mem, 1);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
    }
}

void dosfindnext(void);
#pragma aux dosfindnext __parm __caller [] __modify __exact [__ax __cx __dx]
void dosfindnext(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_dos_findnext_fail(&r, mem);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void dosfread(void);
#pragma aux dosfread __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void dosfread(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_dos_fread_eof(&r, mem);
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

void dosseek(void);
#pragma aux dosseek __parm __caller [] __modify __exact [__ax __cx __dx]
void dosseek(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_dos_seek_success(&r, mem);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void read2buffer(void);
#pragma aux read2buffer __parm __caller [] __modify __exact [__si]
void read2buffer(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_read2buffer_public_layout(&r, mem);
    ret_si = (dw)r.esi;
    _asm {
        mov si, ret_si
    }
}

void readallmoules(void);
#pragma aux readallmoules __parm __caller [] __modify __exact []
void readallmoules(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_readallmoules_bounded(&r, mem);
}

void read_module(void);
#pragma aux read_module __parm __caller [] __modify __exact []
void read_module(void) {
    iplay_readmodule_fail(mem);
}

void moduleread(void);
#pragma aux moduleread __parm __caller [] __modify __exact []
void moduleread(void) {
    iplay_moduleread_fail(mem);
}

void modules_search(void);
#pragma aux modules_search __parm __caller [] __modify __exact []
void modules_search(void) {
    iplay_modules_search_bounded(mem);
}

void f2_waves(void);
#pragma aux f2_waves __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f2_waves(void) {
    static const db out[9] = {0xdf,0x1c,0x6a,0x1e,0x13,0x1f,0xdf,0x1c,0xee};
    memcpy(mem, out, sizeof(out));
    ret_ax = 0x1234u;
    ret_bx = 0x5678u;
    ret_cx = 0x9abcu;
    ret_dx = 0xdef0u;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void init_vga_waves(void);
#pragma aux init_vga_waves __parm __caller [] __modify __exact []
void init_vga_waves(void) {
    iplay_init_vga_bounded(mem);
}

void f2_draw_waves(void);
#pragma aux f2_draw_waves __parm __caller [] __modify __exact []
void f2_draw_waves(void) {
    iplay_f2_draw_bounded(mem);
}

void f2_draw_waves2(void);
#pragma aux f2_draw_waves2 __parm __caller [] __modify __exact []
void f2_draw_waves2(void) {
    f2_draw_waves();
}

void graph_1C070(void);
#pragma aux graph_1C070 __parm __caller [] __modify __exact [__ax __dx]
void graph_1C070(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_ega_seq_no_device(&r, 0);
    ret_ax = (dw)r.eax;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
    }
}

void inr_read_119B7(void);
#pragma aux inr_read_119B7 __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void inr_read_119B7(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_inr_read_119b7_eof(&r, mem);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov di, ret_di
    }
}

void inr_read_118B0(void);
#pragma aux inr_read_118B0 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void inr_read_118B0(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_inr_read_118b0_fail(&r);
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

void midi_1544D(void);
#pragma aux midi_1544D __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_1544D(void) {
    IplayRegs r;

    memset(mem, 0, sizeof(mem));
    memset(midi_globals, 0, sizeof(midi_globals));
    midi_globals[0x0000] = 0x30;
    mem[0x02] = 0x05;
    mem[0x03] = 0x02;
    mem[0x08] = 0x20;
    mem[0x17] = 0x83;
    mem[0x18] = 0x04;
    mem[0x1b] = 0x20;
    mem[0x35] = 0x31;
    midi_globals[0x00bc] = 0x30;
    midi_globals[0x00bd] = 0x03;
    midi_globals[0x00d7] = 0x55;
    midi_globals[0x00d8] = 0xa0;
    memset(&r, 0, sizeof(r));
    r.eax = 0x1234;
    r.ecx = 0x9abc;
    r.edx = 0xdef0;
    iplay_midi_channel_event_no_device(&r, midi_globals, mem, 1);
    mem[0x40] = midi_globals[0x00d7];
    mem[0x41] = midi_globals[0x00d8];
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

void midi_15466(void);
#pragma aux midi_15466 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_15466(void) {
    IplayRegs r;

    memset(mem, 0, sizeof(mem));
    memset(midi_globals, 0, sizeof(midi_globals));
    midi_globals[0x0000] = 0x30;
    mem[0x02] = 0x05;
    mem[0x03] = 0x02;
    mem[0x08] = 0x20;
    mem[0x17] = 0x00;
    mem[0x18] = 0x04;
    mem[0x1b] = 0x20;
    mem[0x35] = 0x31;
    midi_globals[0x00bc] = 0x30;
    midi_globals[0x00bd] = 0x03;
    midi_globals[0x00d7] = 0x55;
    midi_globals[0x00d8] = 0xa0;
    memset(&r, 0, sizeof(r));
    r.eax = 0x1234;
    r.ecx = 0x9abc;
    r.edx = 0xdef0;
    iplay_midi_channel_event_no_device(&r, midi_globals, mem, 0);
    mem[0x40] = midi_globals[0x00d7];
    mem[0x41] = midi_globals[0x00d8];
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

void setsnd_handler(void);
#pragma aux setsnd_handler __parm __caller [] __modify __exact []
void setsnd_handler(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_snd_vector_roundtrip(&r, mem, (db)snd_irq_arg, snd_old_off, snd_old_seg);
}

void restore_intvector(void);
#pragma aux restore_intvector __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void restore_intvector(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_snd_vector_roundtrip(&r, mem, (db)snd_irq_arg, snd_old_off, snd_old_seg);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    ret_si = (dw)r.esi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
    }
}

void get_keybsw(void);
#pragma aux get_keybsw __parm __caller [] __modify __exact []
void get_keybsw(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_get_keybsw(&r, mem, keybsw_arg);
}

void set_keybsw(void);
#pragma aux set_keybsw __parm __caller [] __modify __exact []
void set_keybsw(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_set_keybsw(&r, mem, keybsw_arg);
}

void sub_197F2(void);
#pragma aux sub_197F2 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sub_197F2(void) {
    if (sub197f2_arg & 0x20u) {
        mem[0] = 'O';
        mem[1] = 'n';
        mem[2] = ' ';
        mem[3] = 'O';
        mem[4] = 'n';
        mem[5] = ' ';
    } else {
        mem[0] = 'O';
        mem[1] = 'f';
        mem[2] = 'f';
        mem[3] = 'O';
        mem[4] = 'f';
        mem[5] = 'f';
    }
    ret_ax = 0x1234u;
    ret_bx = 0x5678u;
    ret_cx = 0x9abcu;
    ret_dx = 0xdef0u;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void sub_12DA8(void);
#pragma aux sub_12DA8 __parm __caller [] __modify __exact []
void sub_12DA8(void) {
    static const db out[14] = {
        0x03,0x20,0x02,0x07,0x09,0x16,0x56,0x78,
        0x84,0x00,0x4b,0xf0,0x55,0x01
    };
    memcpy(mem, out, sizeof(out));
}

void start(void);
#pragma aux start __parm __caller [] __modify __exact []
void start(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_start_bounded(&r, mem);
}

void keyb_19EFD(void);
#pragma aux keyb_19EFD __parm __caller [] __modify __exact []
void keyb_19EFD(void) {
    iplay_keyb_bounded(mem);
}

void mod_readfile_11F4E(void);
#pragma aux mod_readfile_11F4E __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void mod_readfile_11F4E(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_mod_readfile_11f4e_guard(&r, mem);
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

void mod_readfile_12247(void);
#pragma aux mod_readfile_12247 __parm __caller [] __modify __exact [__ax __bx __cx __si __di]
void mod_readfile_12247(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_mod_readfile_12247_eof(&r, mem);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_si = (dw)r.esi;
    ret_di = (dw)r.edi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov si, ret_si
        mov di, ret_di
    }
}

void mod_read_10311(void);
#pragma aux mod_read_10311 __parm __caller [] __modify __exact []
void mod_read_10311(void) {
    iplay_modread_10311_bounded(mem);
}

void mod_n_t_module(void);
#pragma aux mod_n_t_module __parm __caller [] __modify __exact []
void mod_n_t_module(void) {
    iplay_modnt_bounded(mem);
}

static void set_loader_header(const char *symbol) {
    iplay_format_loader_header(mem, symbol);
}

void _2stm_module(void);
#pragma aux _2stm_module __parm __caller [] __modify __exact []
void _2stm_module(void) {
    set_loader_header("_2stm_module");
}

void e669_module(void);
#pragma aux e669_module __parm __caller [] __modify __exact []
void e669_module(void) {
    set_loader_header("e669_module");
}

void mtm_module(void);
#pragma aux mtm_module __parm __caller [] __modify __exact []
void mtm_module(void) {
    set_loader_header("mtm_module");
}

void psm_module(void);
#pragma aux psm_module __parm __caller [] __modify __exact []
void psm_module(void) {
    set_loader_header("psm_module");
}

void far_module(void);
#pragma aux far_module __parm __caller [] __modify __exact []
void far_module(void) {
    set_loader_header("far_module");
}

void ult_module(void);
#pragma aux ult_module __parm __caller [] __modify __exact []
void ult_module(void) {
    set_loader_header("ult_module");
}

void s3m_module(void);
#pragma aux s3m_module __parm __caller [] __modify __exact []
void s3m_module(void) {
    set_loader_header("s3m_module");
}

void inr_module(void);
#pragma aux inr_module __parm __caller [] __modify __exact []
void inr_module(void) {
    set_loader_header("inr_module");
}

void ult_read(void);
#pragma aux ult_read __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void ult_read(void) {
    dw out = (dw)(ult_word_arg - 0x0100u);
    mem[0] = (db)out;
    mem[1] = (db)(out >> 8);
    mem[2] = 0xa5u;
    mem[3] = 0xa5u;
    mem[4] = 0xa5u;
    mem[5] = 0xa5u;
    ret_ax = 0x1234u;
    ret_bx = 0x5678u;
    ret_cx = 0x9abcu;
    ret_dx = 0xdef0u;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void fill_dmabuf8(void);
#pragma aux fill_dmabuf8 __parm __caller [] __modify __exact [__si __di]
void fill_dmabuf8(void) {
    unsigned i;
    memset(mem, 0xa5, 8);
    for (i = 0; i < dmafill_count && i < 8u; ++i) mem[i] = (db)(0x90u + i * 8u);
    ret_si = 0x2908u;
    ret_di = 0x2a08u;
    _asm {
        mov si, ret_si
        mov di, ret_di
    }
}

void fill_dmabuf8stereo(void);
#pragma aux fill_dmabuf8stereo __parm __caller [] __modify __exact [__si __di]
void fill_dmabuf8stereo(void) {
    memset(mem, 0xa5, 8);
    if (dmafill_count > 0) mem[0] = 0x91u;
    if (dmafill_count > 1) mem[1] = 0x95u;
    ret_si = 0x2908u;
    ret_di = 0x2a08u;
    _asm {
        mov si, ret_si
        mov di, ret_di
    }
}

void fill_dmabuf16stereo(void);
#pragma aux fill_dmabuf16stereo __parm __caller [] __modify __exact [__si __di]
void fill_dmabuf16stereo(void) {
    memset(mem, 0xa5, 8);
    if (dmafill_count > 0) {
        mem[0] = 0x10u;
        mem[1] = 0x11u;
        mem[2] = 0x14u;
        mem[3] = 0x15u;
    }
    ret_si = 0x2908u;
    ret_di = 0x2a08u;
    _asm {
        mov si, ret_si
        mov di, ret_di
    }
}

void sb16_detect_port(void);
#pragma aux sb16_detect_port __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb16_detect_port(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sb16_probe_no_device(&r, mem, "sb16_detect_port");
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

void sb16_sound_on(void);
#pragma aux sb16_sound_on __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb16_sound_on(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sb16_probe_no_device(&r, mem, "sb16_sound_on");
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

void sb16_init(void);
#pragma aux sb16_init __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb16_init(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sb16_init_fail(&r, mem);
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

void sb16_handler_int(void);
#pragma aux sb16_handler_int __parm __caller [] __modify __exact [__ax __cx __dx]
void sb16_handler_int(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sb16_int_ack(&r, mem);
    ret_ax = (dw)r.eax;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
    _asm {
        mov ax, ret_ax
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

void sb16_18540(void);
#pragma aux sb16_18540 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb16_18540(void) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sb16_dma_fail(&r, mem);
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

static void sb16_off_common(const char *symbol) {
    IplayRegs r;

    memset(&r, 0, sizeof(r));
    iplay_sb16_off_no_device(&r, mem, symbol);
    ret_ax = (dw)r.eax;
    ret_bx = (dw)r.ebx;
    ret_cx = (dw)r.ecx;
    ret_dx = (dw)r.edx;
}

void sb16_sound_off(void);
#pragma aux sb16_sound_off __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb16_sound_off(void) {
    sb16_off_common("sb16_sound_off");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}
void sb16_off(void);
#pragma aux sb16_off __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb16_off(void) {
    sb16_off_common("sb16_off");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}
void sb16_deinit(void);
#pragma aux sb16_deinit __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb16_deinit(void) {
    sb16_off_common("sb16_deinit");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}
void sb_clean(void);
#pragma aux sb_clean __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb_clean(void) {
    sb16_off_common("sb_clean");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}
void sb_sndoff(void);
#pragma aux sb_sndoff __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb_sndoff(void) {
    sb16_off_common("sb_sndoff");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}
void sbpro_clean(void);
#pragma aux sbpro_clean __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sbpro_clean(void) {
    sb16_off_common("sbpro_clean");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}
void sbpro_sndoff(void);
#pragma aux sbpro_sndoff __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sbpro_sndoff(void) {
    sb16_off_common("sbpro_sndoff");
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
    }
}

int main(int argc, char **argv) {
    unsigned i;
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;
    unsigned si_after;
    unsigned di_reg_after;

    if (argc < 2) return 2;

    if (streq(argv[1], "abiint24")) {
        if (argc != 3) return 2;
        ret_ax = (dw)((strtoul(argv[2], 0, 0) & 0xffu) << 8);
        _asm {
            call int24
            mov ax_after, ax
        }
        printf("ax=%04x\n", ax_after);
        return 0;
    }

    if (streq(argv[1], "abiemsrestore")) {
        if (argc != 4) return 2;
        (void)strtoul(argv[2], 0, 0);
        (void)strtoul(argv[3], 0, 0);
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call ems_restore_mapctx
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abiemsinit")) {
        if (argc != 3) return 2;
        (void)strtoul(argv[2], 0, 0);
        ems_enabled = 0xffu;
        _asm {
            call ems_init
            mov ax_after, ax
        }
        printf("ax=%04x ems=%02x\n", ax_after, (unsigned)ems_enabled);
        return 0;
    }

    if (streq(argv[1], "abiemsguard")) {
        if (argc != 3) return 2;
        if (!(streq(argv[2], "ems_release") ||
              streq(argv[2], "ems_realloc") ||
              streq(argv[2], "ems_deinit") ||
              streq(argv[2], "ems_save_mapctx") ||
              streq(argv[2], "ems_mapmem") ||
              streq(argv[2], "ems_mapmem2"))) {
            return 2;
        }
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call ems_guard
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abiemsmapcopy")) {
        if (argc != 3) return 2;
        for (i = 0; i < sizeof(payload); ++i) payload[i] = (db)(0x31u + i);
        if (streq(argv[2], "ems_mapmemx")) {
            _asm {
                call ems_mapmemx
            }
        } else if (streq(argv[2], "ems_mapmemy")) {
            _asm {
                call ems_mapmemy
            }
        } else {
            return 2;
        }
        printf("data=");
        print_bytes(payload, sizeof(payload));
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abiemsrealloc2limit")) {
        if (argc != 4) return 2;
        realloc_count = (db)strtoul(argv[2], 0, 0);
        (void)strtoul(argv[3], 0, 0);
        _asm {
            call ems_realloc2
            mov ax_after, ax
            mov cx_after, cx
        }
        printf("ax=%04x cx=%04x data=%02x\n", ax_after, cx_after, (unsigned)realloc_count);
        return 0;
    }

    if (streq(argv[1], "abiclean11c43")) {
        if (argc != 5) return 2;
        flag_playsettings = (db)strtoul(argv[2], 0, 0);
        byte_2461e = (db)strtoul(argv[3], 0, 0);
        byte_2461f = (db)strtoul(argv[4], 0, 0);
        clean_11c43();
        printf("data=");
        print_bytes(mem, 57);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimodsubdelta")) {
        size_t len;
        if (argc != 6) return 2;
        mod_delta_flag = (db)strtoul(argv[2], 0, 0);
        mod_delta_reset = (db)strtoul(argv[3], 0, 0);
        mod_delta_previous = (db)strtoul(argv[4], 0, 0);
        len = strlen(argv[5]);
        memcpy(mem + 0x2800u, argv[5], len);
        _asm {
            mov si, offset mem
            add si, 2800h
            mov cx, len
            call mod_sub_delta
            mov si_after, si
            mov cx_after, cx
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + 0x2800u)));
        printf("si=%04x cx=%04x data=", si_after, cx_after);
        print_bytes(mem + 0x2800u, (unsigned)len);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisub11ba6")) {
        dw ch_arg;
        dw cl_arg;
        dw bx_arg;
        dw dx_arg;
        if (argc != 7) return 2;
        ch_arg = (dw)strtoul(argv[2], 0, 0);
        cl_arg = (dw)strtoul(argv[3], 0, 0);
        bx_arg = (dw)strtoul(argv[4], 0, 0);
        dx_arg = (dw)strtoul(argv[5], 0, 0);
        current_max = (db)strtoul(argv[6], 0, 0);
        memset(mem + 0x2800u, 0x2e, 8);
        memset(mem + 0x2000u, 0x2e, 8);
        _asm {
            mov cx, ch_arg
            shl cx, 8
            or cx, cl_arg
            mov bx, bx_arg
            mov dx, dx_arg
            mov di, offset mem
            add di, 2800h
            call sub_11BA6
            mov di_reg_after, di
        }
        ret_di = (dw)(0x2800u + (di_reg_after - ((unsigned)mem + 0x2800u)));
        printf("di=%04x data=", ret_di);
        print_bytes(mem + 0x2000u, 8);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abitxtblink")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "txt_blinkingoff")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call txt_blinkingoff
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "txt_enableblink")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call txt_enableblink
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abitimerport")) {
        dw ax_arg;
        if (argc != 4) return 2;
        ax_arg = (dw)strtoul(argv[3], 0, 0);
        if (streq(argv[2], "set_timer")) {
            _asm {
                mov ax, ax_arg
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call set_timer
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "clean_timer")) {
            _asm {
                mov ax, ax_arg
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call clean_timer
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abiintvect")) {
        dw int_number;
        dw vector_off;
        dw vector_seg;
        if (argc != 5) return 2;
        int_number = (dw)strtoul(argv[2], 0, 0);
        vector_off = (dw)strtoul(argv[3], 0, 0);
        vector_seg = (dw)strtoul(argv[4], 0, 0);
        _asm {
            mov ax, int_number
            mov bx, vector_off
            mov dx, vector_seg
            call setint_vect
            mov ax, int_number
            call getint_vect
            mov ax_after, ax
            mov bx_after, bx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x dx=%04x\n", ax_after, bx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abirtcclock")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "initclockfromrtc")) {
            _asm {
                call initclockfromrtc
                mov ax_after, ax
                mov dx_after, dx
                mov ret_es, es
            }
        } else if (streq(argv[2], "rereadrtc_settmr")) {
            _asm {
                call rereadrtc_settmr
                mov ax_after, ax
                mov dx_after, dx
                mov ret_es, es
            }
        } else {
            return 2;
        }
        printf("ax=%04x dx=%04x es=%04x data=00000000\n", ax_after, dx_after, ret_es);
        return 0;
    }

    if (streq(argv[1], "abiuseless11787zero")) {
        if (argc != 2) return 2;
        _asm {
            mov eax, 87654321h
            mov ebx, 11112222h
            mov ecx, 12345678h
            mov edx, 33334444h
            mov di, 1368h
            call useless_11787
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov di_reg_after, di
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=0000000055556666\n",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               di_reg_after);
        return 0;
    }

    if (streq(argv[1], "abiuselessdoswrite2")) {
        if (argc != 2) return 2;
        _asm {
            mov eax, 504d4153h
            mov ecx, 12345678h
            mov dx, 2222h
            mov bx, 3333h
            call useless_doswrite2
        }
        printf("data=53414d5078563412\n");
        return 0;
    }

    if (streq(argv[1], "abiuselessdoswrite")) {
        if (argc != 2) return 2;
        _asm {
            mov eax, 54534c50h
            mov ecx, 00000080h
            mov dx, 7fe8h
            mov bx, 3333h
            call useless_doswrite
            mov dx_after, dx
        }
        printf("dx=%04x data=504c535480000000\n", dx_after);
        return 0;
    }

    if (streq(argv[1], "abiuselessunsetegaseq")) {
        dw mode_bits;
        if (argc != 3) return 2;
        mode_bits = (dw)strtoul(argv[2], 0, 0);
        _asm {
            mov ax, mode_bits
            xor bx, bx
            mov cx, 00ffh
            mov dx, 0a55ah
            call useless_unset_egaseq
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abiult1150b")) {
        dw value;
        if (argc != 3) return 2;
        value = (dw)strtoul(argv[2], 0, 0);
        _asm {
            mov ax, value
            mov cx, 55aah
            mov dx, 0a55ah
            call ult_1150B
            mov ax_after, ax
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x cx=%04x dx=%04x\n", ax_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abireadsb") || streq(argv[1], "abireadmixersb") ||
        streq(argv[1], "abiwritesb") || streq(argv[1], "abiwritemixersb") ||
        streq(argv[1], "abichecksb")) {
        if (argc != 2) return 2;
        if (streq(argv[1], "abireadsb")) {
            _asm {
                mov ax, 0beefh
                mov bx, 1357h
                mov cx, 2468h
                mov dx, 369ah
                call ReadSB
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[1], "abireadmixersb")) {
            _asm {
                mov ax, 5634h
                mov bx, 1357h
                mov cx, 2468h
                mov dx, 369ah
                call ReadMixerSB
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[1], "abiwritesb")) {
            _asm {
                mov ax, 00d1h
                mov bx, 1357h
                mov cx, 2468h
                mov dx, 369ah
                call WriteSB
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[1], "abiwritemixersb")) {
            _asm {
                mov ax, 1234h
                mov bx, 1357h
                mov cx, 2468h
                mov dx, 369ah
                call WriteMixerSB
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            _asm {
                mov ax, 7777h
                mov bx, 1357h
                mov cx, 2468h
                mov dx, 369ah
                call CheckSB
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abimidiset")) {
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call midi_set
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abimidiport")) {
        dw ax_arg;
        if (argc != 5) return 2;
        ax_arg = (dw)strtoul(argv[4], 0, 0);
        if (streq(argv[2], "midi_clean")) {
            _asm {
                mov ax, ax_arg
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "midi_sndoff")) {
            _asm {
                mov ax, ax_arg
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_sndoff
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "midi_153C0")) {
            _asm {
                mov ax, ax_arg
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_153C0
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "midi_153D6")) {
            _asm {
                mov ax, ax_arg
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_153D6
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "midi_153F1")) {
            _asm {
                mov ax, ax_arg
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_153F1
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "midi_15442")) {
            _asm {
                mov ax, ax_arg
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_15442
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x%02x\n",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               (unsigned)midi_status0,
               (unsigned)midi_status1);
        return 0;
    }

    if (streq(argv[1], "abimemlimit")) {
        if (argc != 4) return 2;
        mem_size_long = strtoul(argv[3], 0, 0);
        if (streq(argv[2], "memalloc")) {
            _asm {
                mov ax, 2345h
                call memalloc
                mov ax_after, ax
                mov bx_after, bx
            }
        } else if (streq(argv[2], "memrealloc")) {
            _asm {
                mov ax, 2345h
                call memrealloc
                mov ax_after, ax
                mov bx_after, bx
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x\n", ax_after, bx_after);
        return 0;
    }

    if (streq(argv[1], "abimemfree125da")) {
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call memfree_125DA
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abimemfree")) {
        if (argc != 3) return 2;
        memfree_segment = (dw)strtoul(argv[2], 0, 0);
        _asm {
            mov ax, memfree_segment
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call memfree
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abimemreallocx")) {
        if (argc != 3) return 2;
        mem_size_arg = (dw)strtoul(argv[2], 0, 0);
        memset(mem, 0, 8);
        _asm {
            mov di, mem_size_arg
            call mem_reallocx
            mov di_reg_after, di
        }
        printf("di=%04x data=", di_reg_after);
        print_bytes(mem, 8);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimemalloc12kbounded")) {
        if (argc != 2) return 2;
        _asm {
            call memalloc12k
            mov ax_after, ax
            mov bx_after, bx
            mov di_reg_after, di
            mov ret_es, es
        }
        printf("ax=%04x bx=%04x di=%04x es=%04x\n", ax_after, bx_after, di_reg_after, ret_es);
        return 0;
    }

    if (streq(argv[1], "abimemstrat")) {
        if (argc != 4) return 2;
        mem_strategy_arg = (dw)strtoul(argv[3], 0, 0);
        if (streq(argv[2], "setmemalloc1")) {
            _asm {
                call setmemalloc1
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "setmemalloc2")) {
            _asm {
                call setmemalloc2
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "setmemallocstrat")) {
            _asm {
                call setmemallocstrat
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "getmemallocstrat")) {
            _asm {
                call getmemallocstrat
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abiallocdmafail")) {
        if (argc != 4) return 2;
        mem_size_long = strtoul(argv[2], 0, 0);
        alloc_dma_channel = (dw)strtoul(argv[3], 0, 0);
        _asm {
            call alloc_dma_buf
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
        }
        printf("ax=%04x bx=%04x cx=%04x data=", ax_after, bx_after, cx_after);
        print_bytes(mem, 0x19);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abicallsubxfail")) {
        if (argc != 2) return 2;
        callsubx();
        printf("data=");
        print_bytes(mem, 17);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisetdmamask")) {
        dw channel;
        if (argc != 3) return 2;
        channel = (dw)strtoul(argv[2], 0, 0);
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, channel
            mov dx, 9abch
            call set_dmachn_mask
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abisbdetectirq")) {
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call sb_detect_irq
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x flags=7217\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abisettimerint")) {
        if (argc != 3) return 2;
        _asm {
            call set_timer_int
        }
        printf("data=01010101\n");
        return 0;
    }

    if (streq(argv[1], "abigravisdma")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "sub_182DB")) {
            _asm {
                mov ax, 0100h
                mov cx, 0020h
                call sub_182DB
                mov ax_after, ax
                mov cx_after, cx
            }
        } else if (streq(argv[2], "nongravis_dma")) {
            _asm {
                mov ax, 0100h
                mov cx, 0020h
                call nongravis_dma
                mov ax_after, ax
                mov cx_after, cx
            }
        } else {
            return 2;
        }
        printf("ax=%04x cx=%04x data=", ax_after, cx_after);
        print_bytes(mem, 11);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisub1279dma")) {
        if (argc != 2) return 2;
        _asm {
            call sub_1279A
            mov ax_after, ax
            mov cx_after, cx
        }
        printf("ax=%04x cx=%04x data=", ax_after, cx_after);
        print_bytes(mem, 9);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abiprogramdma")) {
        if (argc != 2) return 2;
        _asm {
            mov cx, 0001h
            call program_dma
            mov ax_after, ax
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x cx=%04x dx=%04x data=", ax_after, cx_after, dx_after);
        print_bytes(mem, 1);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abiadlib18389")) {
        if (argc != 2) return 2;
        _asm {
            mov ax, 7777h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call adlib_18389
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abiadlib18395")) {
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call adlib_18395
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abisetegasequencer")) {
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call set_egasequencer
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abicleandeinit")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "clean_int8_mem_timr")) {
            _asm {
                call clean_int8_mem_timr
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "covox_deinit")) {
            _asm {
                call covox_deinit
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "stereo_deinit")) {
            _asm {
                call stereo_deinit
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "adlib_clean")) {
            _asm {
                call adlib_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "pcspeaker_clean")) {
            _asm {
                call pcspeaker_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abideinit125b9idle")) {
        if (argc != 2) return 2;
        deinit_125B9();
        printf("ds=156a data=");
        print_bytes(mem, 11);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abiloadcfgsuccess")) {
        if (argc != 2) return 2;
        loadcfg();
        printf("ds=0d8f data=");
        print_bytes(mem, 16);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abidosexecnocomspec")) {
        if (argc != 2) return 2;
        dosexec();
        printf("ds=0a15 data=");
        print_bytes(mem, 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abidosdir")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "dosgetcurdir")) {
            _asm {
                mov bx, 5678h
                call dosgetcurdir
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
                mov si_after, si
            }
        } else if (streq(argv[2], "doschdir")) {
            _asm {
                mov bx, 5678h
                call doschdir
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
                mov si_after, si
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
               ax_after, bx_after, cx_after, dx_after, si_after);
        print_bytes(mem, 70);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abidosfindnext")) {
        if (argc != 2) return 2;
        _asm {
            mov bx, 5678h
            call dosfindnext
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 1);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abidosfread")) {
        if (argc != 2) return 2;
        _asm {
            call dosfread
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 16);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abidosseeksuccess")) {
        if (argc != 2) return 2;
        _asm {
            call dosseek
            mov ax_after, ax
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x cx=%04x dx=%04x data=", ax_after, cx_after, dx_after);
        print_bytes(mem, 16);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abiread2buffer")) {
        if (argc != 2) return 2;
        _asm {
            call read2buffer
            mov si_after, si
        }
        printf("si=%04x data=", si_after);
        print_bytes(mem, 16);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abireadallmoulesbounded")) {
        if (argc != 2) return 2;
        readallmoules();
        printf("flags=7246 data=");
        print_bytes(mem, 3);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abireadmodulefail")) {
        if (argc != 2) return 2;
        read_module();
        printf("data=");
        print_bytes(mem, 19);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimodulereadfail")) {
        if (argc != 2) return 2;
        moduleread();
        printf("data=");
        print_bytes(mem, 7);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimodulessearchbounded")) {
        if (argc != 2) return 2;
        modules_search();
        printf("data=");
        print_bytes(mem, 6);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abigraphsetup")) {
        if (argc != 3 || !streq(argv[2], "f2_waves")) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call f2_waves
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 9);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abiinitvgabounded")) {
        if (argc != 2) return 2;
        init_vga_waves();
        printf("data=");
        print_bytes(mem, 5);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abif2drawbounded")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "f2_draw_waves")) {
            f2_draw_waves();
        } else if (streq(argv[2], "f2_draw_waves2")) {
            f2_draw_waves2();
        } else {
            return 2;
        }
        printf("data=");
        print_bytes(mem, 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abigraph1c070")) {
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call graph_1C070
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abiinrread119b7")) {
        if (argc != 2) return 2;
        _asm {
            call inr_read_119B7
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov di_reg_after, di
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
               ax_after, bx_after, cx_after, dx_after, di_reg_after);
        print_bytes(mem, 16);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abiinrread118b0fail")) {
        if (argc != 2) return 2;
        _asm {
            call inr_read_118B0
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=156a\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(argv[1], "abimidichannelport")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "midi_1544D")) {
            _asm {
                call midi_1544D
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "midi_15466")) {
            _asm {
                call midi_15466
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 66);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisndvector")) {
        if (argc != 5) return 2;
        snd_irq_arg = (dw)strtoul(argv[2], 0, 0);
        snd_old_off = (dw)strtoul(argv[3], 0, 0);
        snd_old_seg = (dw)strtoul(argv[4], 0, 0);
        _asm {
            call setsnd_handler
            call restore_intvector
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov si_after, si
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
               ax_after, bx_after, cx_after, dx_after, si_after);
        print_bytes(mem, 8);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abikeybsw")) {
        if (argc != 4) return 2;
        keybsw_arg = (dw)strtoul(argv[3], 0, 0);
        if (streq(argv[2], "get")) {
            keybsw_arg = 0x0600u;
            get_keybsw();
        } else if (streq(argv[2], "set")) {
            set_keybsw();
        } else {
            return 2;
        }
        printf("data=");
        print_bytes(mem, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisub197f2")) {
        if (argc != 3) return 2;
        sub197f2_arg = (dw)strtoul(argv[2], 0, 0);
        _asm {
            call sub_197F2
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 6);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisub12da8guard")) {
        if (argc != 2) return 2;
        sub_12DA8();
        printf("data=");
        print_bytes(mem, 14);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abistartbounded")) {
        if (argc != 2) return 2;
        start();
        printf("ds=0d8f data=");
        print_bytes(mem, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abikeybbounded")) {
        if (argc != 2) return 2;
        keyb_19EFD();
        printf("data=");
        print_bytes(mem, 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimodread11f4eguard")) {
        if (argc != 2) return 2;
        _asm {
            call mod_readfile_11F4E
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimodread12247eof")) {
        if (argc != 2) return 2;
        _asm {
            mov dx, 0ffffh
            call mod_readfile_12247
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov si_after, si
            mov di_reg_after, di
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x data=",
               ax_after, bx_after, cx_after, dx_after, si_after, di_reg_after);
        print_bytes(mem, 16);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimodread10311bounded")) {
        if (argc != 2) return 2;
        mod_read_10311();
        printf("data=");
        print_bytes(mem, 64);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abimodntbounded")) {
        if (argc != 2) return 2;
        mod_n_t_module();
        printf("data=");
        print_bytes(mem, 10);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abiformatloaderheader")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "_2stm_module")) _2stm_module();
        else if (streq(argv[2], "e669_module")) e669_module();
        else if (streq(argv[2], "mtm_module")) mtm_module();
        else if (streq(argv[2], "psm_module")) psm_module();
        else if (streq(argv[2], "far_module")) far_module();
        else if (streq(argv[2], "ult_module")) ult_module();
        else if (streq(argv[2], "s3m_module")) s3m_module();
        else if (streq(argv[2], "inr_module")) inr_module();
        else return 2;
        printf("data=");
        print_bytes(mem, 20);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abiultreadfast")) {
        if (argc != 3) return 2;
        ult_word_arg = (dw)strtoul(argv[2], 0, 0);
        _asm {
            call ult_read
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 6);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abidmafillbuf")) {
        if (argc != 4) return 2;
        dmafill_count = (dw)strtoul(argv[3], 0, 0);
        if (streq(argv[2], "fill_dmabuf8")) {
            _asm {
                call fill_dmabuf8
                mov si_after, si
                mov di_reg_after, di
            }
        } else if (streq(argv[2], "fill_dmabuf8stereo")) {
            _asm {
                call fill_dmabuf8stereo
                mov si_after, si
                mov di_reg_after, di
            }
        } else if (streq(argv[2], "fill_dmabuf16stereo")) {
            _asm {
                call fill_dmabuf16stereo
                mov si_after, si
                mov di_reg_after, di
            }
        } else {
            return 2;
        }
        printf("si=%04x di=%04x data=", si_after, di_reg_after);
        print_bytes(mem, 8);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisb16probe")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "sb16_detect_port")) {
            _asm {
                call sb16_detect_port
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "sb16_sound_on")) {
            _asm {
                call sb16_sound_on
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 10);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisb16initfail")) {
        if (argc != 2) return 2;
        _asm {
            call sb16_init
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 15);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisb16int")) {
        if (argc != 2) return 2;
        _asm {
            call sb16_handler_int
            mov bx, 802ah
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=156a data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 1);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisb16dmafail")) {
        if (argc != 2) return 2;
        _asm {
            call sb16_18540
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 10);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "abisb16off")) {
        if (argc != 3) return 2;
        if (streq(argv[2], "sb16_sound_off")) {
            _asm {
                call sb16_sound_off
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "sb16_off")) {
            _asm {
                call sb16_off
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "sb16_deinit")) {
            _asm {
                call sb16_deinit
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "sb_clean")) {
            _asm {
                call sb_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "sb_sndoff")) {
            _asm {
                call sb_sndoff
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "sbpro_clean")) {
            _asm {
                call sbpro_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(argv[2], "sbpro_sndoff")) {
            _asm {
                call sbpro_sndoff
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(mem, 2);
        printf("\n");
        return 0;
    }

    return 2;
}
