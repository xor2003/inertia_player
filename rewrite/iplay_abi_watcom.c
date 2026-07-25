#include "iplay_rewrite.h"
#include <dos.h>
#include <string.h>

#ifdef __WATCOMC__
#pragma code_seg("ABI_TEXT")

static db video_temp[4000];
static dw int_vector_off[256];
static dw int_vector_seg[256];
static db snd_vector_irq;
static dw snd_vector_old_off;
static dw snd_vector_old_seg;

static void video_copy_in(db far *video, unsigned count) {
    _fmemcpy(video_temp, video, count);
}

static void video_copy_out(db far *video, unsigned count) {
    _fmemcpy(video, video_temp, count);
}

static dw abi_word(const db *mem, unsigned off) {
    return (dw)(mem[off] | ((dw)mem[off + 1u] << 8));
}

static dd abi_dword(const db *mem, unsigned off) {
    return (dd)mem[off]
         | ((dd)mem[off + 1u] << 8)
         | ((dd)mem[off + 2u] << 16)
         | ((dd)mem[off + 3u] << 24);
}

static void iplay_memfill8080_far_fill(db far *dma) {
    _fmemset(dma, 0x80, 0x1000u);
}

static void iplay_volume_prep_far(IplayRegs *r, db *globals, db far *dst, dw word_24610, dw size) {
    globals[0x0070] = (db)word_24610;
    globals[0x0071] = (db)(word_24610 >> 8);
    globals[0x0072] = (db)size;
    globals[0x0073] = (db)(size >> 8);
    _fmemset(dst, 0, size);
    r->ecx &= 0xffff0000UL;
    r->edi = (r->edi & 0xffff0000UL) | (dw)((dw)r->edi + size);
}

static dd abi_far_dword(db far *mem, unsigned off) {
    return (dd)mem[off]
         | ((dd)mem[off + 1u] << 8)
         | ((dd)mem[off + 2u] << 16)
         | ((dd)mem[off + 3u] << 24);
}

static void hex_public(unsigned bits) {
    IplayRegs r;
    db *mem = (db *)0;
    dw si_in;
    dd eax_in;
    dw ax_out;
    dw dx_out;
    dw si_out;
    _asm {
        mov eax_in, eax
        mov si_in, si
    }
    r.eax = eax_in;
    r.esi = si_in;
    if (bits == 4) iplay_u4tox(&r, mem);
    else if (bits == 8) iplay_u8tox(&r, mem);
    else if (bits == 16) iplay_u16tox(&r, mem);
    else iplay_u32tox(&r, mem);
    ax_out = (dw)r.eax;
    dx_out = (dw)(r.eax >> 16);
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov dx, dx_out
        mov si, si_out
    }
}

void u4tox(void) { hex_public(4); }
void u8tox(void) { hex_public(8); }
void u16tox(void) { hex_public(16); }
void u32tox(void) { hex_public(32); }
void my_u4tox(void) { hex_public(4); }
void my_u8tox(void) { hex_public(8); }
void my_u16tox(void) { hex_public(16); }
void my_u32tox(void) { hex_public(32); }

void hex_1BE39(void) {
    IplayRegs r;
    dw ax_in;
    dw di_in;
    dw es_in;
    dw ax_out;
    dw di_out;
    db tmp[2];
    db far *dst;

    _asm {
        mov ax_in, ax
        mov di_in, di
        mov es_in, es
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.edi = di_in;
    iplay_hex_1be39(&r, tmp);
    ax_out = (dw)r.eax;
    di_out = (dw)r.edi;
    dst = (db far *)MK_FP(es_in, di_in);
    _fmemcpy(dst, tmp, 2u);
    _asm {
        mov ax, ax_out
        mov di, di_out
    }
}

void ems_release(void) {}

void ems_realloc(void) {}

void ems_deinit(void) {}

void ems_save_mapctx(void) {}

void ems_restore_mapctx(void) {}

void ems_mapmem(void) {}

void ems_mapmem2(void) {}

void ems_mapmemx(void) {}

void ems_mapmemy(void) {}


void ems_realloc2(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw di_out;

    _asm {
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    iplay_ems_realloc2_fallback(&r, mem, di_in);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
    }
}

void nullsub_5(void) {}

void eff_nullsub(void) {}

void nullsub_2(void) {}

void nullsub_4(void) {}

void nullsub_3(void) {}

static void snd_guard_public(unsigned op) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_snd_guard(&r, mem, op);
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
        mov si, si_in
        mov di, di_in
    }
}

void snd_initialze(void) { snd_guard_public(0); }
void snd_on(void) { snd_guard_public(1); }
void snd_off(void) { snd_guard_public(2); }
void snd_deinit(void) { snd_guard_public(3); }
void snd_offx(void) { snd_guard_public(4); }

static void useless_sprint_numeric_public(unsigned kind) {
    IplayRegs r;
    db *mem = (db *)0;
    db tmp[16];
    db far *dst;
    size_t len;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw es_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
        mov es_in, es
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    len = iplay_useless_sprint_numeric(&r, mem, tmp, kind);
    dst = (db far *)MK_FP(es_in, di_in);
    _fmemcpy(dst, tmp, len);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void useless_sprint_6(void) { useless_sprint_numeric_public(6); }
void useless_sprint_7(void) { useless_sprint_numeric_public(7); }
void useless_sprint_8(void) { useless_sprint_numeric_public(8); }
void useless_sprint_9(void) { useless_sprint_numeric_public(9); }
void useless_sprint_10(void) { useless_sprint_numeric_public(10); }
void useless_sprint_11(void) { useless_sprint_numeric_public(11); }
void useless_sprint_12(void) { useless_sprint_numeric_public(12); }

void useless_12D61(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_useless_12d61_no_device(&r, mem + 0x0132u);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void useless_mysprintf(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ds_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
        mov ds_in, ds
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_useless_mysprintf(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ds_in
        mov es, ax
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void useless_doswrite2(void) {
    IplayRegs r;
    db *mem = (db *)0xbf68u;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_useless_doswrite2_header(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void useless_doswrite(void) {
    IplayRegs r;
    db *mem = (db *)0xbf68u;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_useless_doswrite_header(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void useless_11787(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    iplay_useless_11787_zero_public_layout(&r, mem, di_in);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void useless_writeinr(void) {
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_useless_writeinr_fail(&r);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void useless_writeinr_118(void) {
    IplayRegs r;
    db *mem = (db *)0x12a6u;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_useless_writeinr_118_header(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}


void int9_keyb(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_int9_keyb_no_scancode(&r, mem + 0x30f4u);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void sub_197F2(void) {
    IplayRegs r;
    db *mem = (db *)0;
    db tmp[6];
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    iplay_sub_197f2_labels(&r, tmp, abi_word(mem, 0x1509u));
    memcpy(mem + 0x0e24u, tmp, 3u);
    memcpy(mem + 0x0e79u, tmp + 3u, 3u);
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
        mov si, si_in
        mov di, di_in
    }
}

void ems_init(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    dw bx_out;
    dw dx_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.edx = dx_in;
    iplay_ems_init_config(&r, mem, abi_word(mem, 0x013au));
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov dx, dx_out
    }
}

void my_putdigit(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw cx_out;
    dw dx_out;
    dw si_out;
    _asm {
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
    }
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    iplay_my_putdigit(&r, mem);
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    _asm {
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
    }
}

void myputdigit(void) { my_putdigit(); }

void my_u32toa(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dd eax_in;
    dw bx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_in;
    dw si_out;
    _asm {
        mov eax_in, eax
        mov bx_in, bx
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    r.eax = eax_in;
    r.ebx = bx_in;
    r.esi = si_in;
    iplay_my_u32toa(&r, mem, bx_in);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
    }
}

static void decimal32_public(int signed_value) {
    IplayRegs r;
    db *mem = (db *)0;
    dd eax_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw si_in;
    dw si_out;
    _asm {
        mov eax_in, eax
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    r.eax = eax_in;
    r.esi = si_in;
    if (signed_value) iplay_my_i32toa10(&r, mem);
    else iplay_my_u32toa10(&r, mem);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
    }
}

void my_u32toa10_0(void) { decimal32_public(0); }
void my_u32toa10(void) { decimal32_public(0); }
void my_i32toa10_0(void) { decimal32_public(1); }
void my_i32toa10(void) { decimal32_public(1); }

static void decimal_small_public(int mode) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw si_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    _asm {
        mov ax_in, ax
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.esi = si_in;
    switch (mode) {
    case 0: iplay_my_u8toa_10(&r, mem); break;
    case 1: iplay_my_u16toa_10(&r, mem); break;
    case 2: iplay_my_i8toa10(&r, mem); break;
    default: iplay_my_i16toa10(&r, mem); break;
    }
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
    }
}

void my_u8toa_10(void) { decimal_small_public(0); }
void my_u8toa10(void) { decimal_small_public(0); }
void my_u16toa_10(void) { decimal_small_public(1); }
void my_u16toa10(void) { decimal_small_public(1); }
void my_i8toa10_0(void) { decimal_small_public(2); }
void my_i8toa10(void) { decimal_small_public(2); }
void my_i16toa10_0(void) { decimal_small_public(3); }
void my_i16toa10(void) { decimal_small_public(3); }

static void strlen_public(void) {
    IplayRegs r;
    const db *mem = (const db *)0;
    dw si_in;
    dw ax_out;
    dw si_out;
    _asm {
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    r.esi = si_in;
    iplay_mystrlen(&r, mem);
    ax_out = (dw)r.eax;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov si, si_out
    }
}

void mystrlen_0(void) { strlen_public(); }
void mystrlen(void) { strlen_public(); }

static void u32toa_fill_public(int with_pointer_prefix) {
    IplayRegs r;
    db *mem = (db *)0;
    dd eax_in;
    dw bx_in;
    dw cx_in;
    dw di_in;
    dw ax_out;
    dw di_out;
    _asm {
        mov eax_in, eax
        mov bx_in, bx
        mov cx_in, cx
        mov di_in, di
    }
    r.eax = eax_in;
    r.ebx = bx_in;
    r.edi = di_in;
    iplay_my_u32toa_fill(&r, mem, cx_in, with_pointer_prefix);
    ax_out = (dw)r.eax;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        xor cx, cx
        xor dx, dx
        xor si, si
        mov di, di_out
    }
}

void my_u32toa_fill(void) { u32toa_fill_public(0); }
void my_pnt_u32toa_fill(void) { u32toa_fill_public(1); }

void myasmsprintf(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dd eax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;
    _asm {
        mov eax_in, eax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    r.eax = eax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_myasmsprintf(&r, mem);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void get_comspec(void) {
    IplayRegs r;
    const db *mem = (const db *)0;
    dw di_out;
    r.edi = 0;
    iplay_get_comspec(&r, mem);
    di_out = (dw)r.edi;
    _asm {
        mov di, di_out
    }
}

void getexename(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw si_in;
    dw ax_out;
    dw cx_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.esi = si_in;
    iplay_getexename(&r, mem, mem);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov si, si_out
        mov di, di_out
    }
}

void int2f_checkmyself(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw ax_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_int2f_checkmyself(&r, mem);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
    }
}

static void txt_blink_public(int enable) {
    IplayRegs r;
    dw cx_in;
    dw dx_in;
    dw ax_out;
    dw bx_out;
    _asm {
        mov cx_in, cx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_txt_blink_no_device(&r, enable);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_in
        mov dx, dx_in
    }
}

void txt_blinkingoff(void) { txt_blink_public(0); }
void txt_enableblink(void) { txt_blink_public(1); }

void setvideomode(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw ax_out;
    dw bx_out;
    dw dx_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_setvideomode_no_hw(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_in
        mov dx, dx_out
    }
}

static void text_setup_public(const char *symbol) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_text_setup_small(&r, mem, symbol);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void text_init(void) { text_setup_public("text_init"); }

#ifndef IPLAY_PLAYER_OMIT_RISKY_UI_ABI
static void setup6_public(const char *symbol, int graph) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    if (graph) {
        iplay_graph_setup_bounded(&r, mem, symbol);
    } else {
        iplay_text_setup_small(&r, mem, symbol);
    }
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void text_init2(void) { setup6_public("text_init2", 0); }

void f1_help(void) { setup6_public("f1_help", 0); }

void f3_textmetter(void) { setup6_public("f3_textmetter", 0); }

void f4_patternnae(void) { setup6_public("f4_patternnae", 0); }

void f6_undoc(void) { setup6_public("f6_undoc", 0); }

void f2_waves(void) { setup6_public("f2_waves", 1); }

void f5_graphspectr(void) { setup6_public("f5_graphspectr", 1); }

void init_f5_spectr(void) { setup6_public("init_f5_spectr", 1); }
#endif

void get_playsettings(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw ax_out;
    _asm {
        mov ax_in, ax
    }
    r.eax = ax_in;
    iplay_get_playsettings(&r, mem[0x00d2]);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
    }
}

void set_playsettings(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw di_out;

    _asm {
        mov ax_in, ax
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    iplay_set_playsettings(&r,
                           mem,
                           mem + 0x1368u,
                           abi_word(mem, 0x0034u),
                           0x50u);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
    }
}

void getset_playstate(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw ax_out;
    dw bx_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    iplay_getset_playstate(&r, mem[0x00df]);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
    }
}

void get_12F7C(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    dw bx_out;
    memset(&r, 0, sizeof(r));
    iplay_get_12f7c(&r,
                    abi_word(mem, 0x0050u),
                    abi_word(mem, 0x0056u));
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
    }
}

void read_sndsettings(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw bp_out;
    dw si_out;

    memset(&r, 0, sizeof(r));
    iplay_read_sndsettings(
        &r,
        mem[0x0132u],
        abi_word(mem, 0x0133u),
        mem[0x0135u],
        mem[0x0136u],
        mem[0x0137u],
        mem[0x0138u],
        mem[0x0139u],
        abi_word(mem, 0x00beu),
        abi_word(mem, 0x0098u),
        abi_word(mem, 0x013au),
        mem[0x0082u]);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    bp_out = (dw)r.ebp;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov bp, bp_out
        mov si, si_out
    }
}

void get_keybsw(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw bios_flags;

    memset(&r, 0, sizeof(r));
    bios_flags = *((dw far *)MK_FP(0x0000u, 0x0017u));
    iplay_get_keybsw(&r, mem + 0x30f6u, bios_flags);
}

void set_keybsw(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw keyb_switches;

    memset(&r, 0, sizeof(r));
    keyb_switches = abi_word(mem, 0x30f6u);
    iplay_set_keybsw(&r, mem + 0x30f6u, keyb_switches);
    *((dw far *)MK_FP(0x0000u, 0x0017u)) = abi_word(mem, 0x30f6u);
}

void memclean(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw di_in;
    dw ax_out;
    dw cx_out;
    dw di_out;
    _asm {
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.edi = di_in;
    iplay_memclean(&r, mem, abi_word(mem, 0x0072u));
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov di, di_out
    }
}

void volume_12A66(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    iplay_volume_12a66(&r, abi_word(mem, 0x0034u));
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
    }
}

static void strcpy_count_public(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw cx_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ecx = cx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_strcpy_count(&r, mem, mem);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov si, si_out
        mov di, di_out
    }
}

void strcpy_count_0(void) { strcpy_count_public(); }
void strcpy_count(void) { strcpy_count_public(); }

static void copy_printable_public(int seg1) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw cx_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov si_in, si
        mov di_in, di
    }
    r.eax = ax_in;
    r.ecx = cx_in;
    r.esi = si_in;
    r.edi = di_in;
    if (seg1) iplay_seg1_copy_printable(&r, mem, mem);
    else iplay_copy_printable(&r, mem, mem);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov si, si_out
        mov di, di_out
    }
}

void copy_printable(void) { copy_printable_public(0); }
void cpy_printable(void) { copy_printable_public(1); }

void txt_1ABAE(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw cx_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ecx = cx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_txt_1abae(&r, mem, mem);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov si, si_out
        mov di, di_out
    }
}

static void put_message_public(int initial_ax) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_put_message(&r, mem, mem, initial_ax);
    ax_out = (dw)r.eax;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov si, si_out
        mov di, di_out
    }
}

void put_message(void) { put_message_public(0); }
void put_message2(void) { put_message_public(1); }

void text_1BF69(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_text_1bf69(&r, mem, mem);
    ax_out = (dw)r.eax;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov si, si_out
        mov di, di_out
    }
}

void write_scr(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bp_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_write_scr(&r, mem, mem);
    ax_out = (dw)r.eax;
    bp_out = (dw)r.ebp;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bp, bp_out
        mov si, si_out
        mov di, di_out
    }
}


static void rtc_clock_public(void) {
    IplayRegs r;
    db tmp[4];
    db far *bios_ticks;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    unsigned i;

    memset(&r, 0, sizeof(r));
    iplay_rtc_clock_bcd_123456(&r, tmp);
    bios_ticks = (db far *)MK_FP(0, 0x046c);
    for (i = 0; i < sizeof(tmp); ++i) bios_ticks[i] = tmp[i];
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov es, dx_out
    }
}

void initclockfromrtc(void) { rtc_clock_public(); }
void rereadrtc_settmr(void) { rtc_clock_public(); }

void loadcfg(void) {
    db *cfg = (db *)0x1500u;
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    r.esi = 0;
    r.edi = 0;
    iplay_loadcfg_success(&r, cfg);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void readallmoules(void) {
    db *mem = (db *)0;
    iplay_readallmoules_public_layout(mem);
    _asm {
        clc
    }
}

void read_module(void) {
    db *mem = (db *)0;
    iplay_readmodule_public_layout(mem);
    _asm {
        stc
    }
}

void moduleread(void) {
    db *mem = (db *)0;
    iplay_moduleread_public_layout(mem);
    _asm {
        stc
    }
}

void mod_read_10311(void) {
    db *patterns = (db *)0x2d00u;
    iplay_modread_10311_bounded(patterns);
}

void mod_n_t_module(void) {
    db *mem = (db *)0;
    iplay_modnt_public_layout(mem);
}

void modules_search(void) {
    db *mem = (db *)0;
    iplay_modules_search_public_layout(mem);
}

void init_vga_waves(void) {
    db *mem = (db *)0;
    iplay_init_vga_public_layout(mem);
}

void f2_draw_waves(void) {
    db *mem = (db *)0;
    dw ds_in;
    _asm {
        mov ds_in, ds
    }
    iplay_f2_draw_public_layout(mem, ds_in);
}

void f2_draw_waves2(void) {
    f2_draw_waves();
}

static void clean_deinit_public_regs(const char *symbol) {
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    r.esi = 0;
    r.edi = 0;
    iplay_clean_deinit_public(&r, symbol);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void clean_int8_mem_timr(void) { clean_deinit_public_regs("clean_int8_mem_timr"); }
void covox_deinit(void) { clean_deinit_public_regs("covox_deinit"); }
void stereo_deinit(void) { clean_deinit_public_regs("stereo_deinit"); }
void adlib_clean(void) { clean_deinit_public_regs("adlib_clean"); }
void pcspeaker_clean(void) { clean_deinit_public_regs("pcspeaker_clean"); }

static void dos_dir_public(int chdir_mode) {
    IplayRegs r;
    dw si_in;
    db *dst;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    _asm {
        mov si_in, si
    }
    dst = (db *)si_in;
    memset(&r, 0, sizeof(r));
    r.esi = si_in;
    iplay_dos_dir_stub(&r, dst, chdir_mode);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
    }
}

void dosgetcurdir(void) { dos_dir_public(0); }
void doschdir(void) { dos_dir_public(1); }

void dosfindnext(void) {
    db *dta = (db *)0x13fcu;
    IplayRegs r;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    memset(&r, 0, sizeof(r));
    iplay_dos_findnext_fail(&r, dta);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void dosfread(void) {
    dw dx_in;
    db *dst;
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov dx_in, dx
    }
    dst = (db *)dx_in;
    memset(&r, 0, sizeof(r));
    iplay_dos_fread_eof(&r, dst);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void dosseek(void) {
    dw dx_in;
    db *dst;
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov dx_in, dx
    }
    dst = (db *)dx_in;
    memset(&r, 0, sizeof(r));
    iplay_dos_seek_success(&r, dst);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void read2buffer(void) {
    db *dst = (db *)0x2800u;
    IplayRegs r;
    dw si_out;
    iplay_read2buffer_public_layout(&r, dst);
    si_out = (dw)r.esi;
    _asm {
        mov si, si_out
    }
}

static void memlimit_public(void) {
    IplayRegs r;
    dd size;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov size, ebx
    }
    r.esi = 0;
    r.edi = 0;
    iplay_mem_limit(&r, size);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void memalloc(void) { memlimit_public(); }
void memrealloc(void) { memlimit_public(); }


void memalloc12k(void) {
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_memalloc12k_bounded(&r);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov di, di_out
        mov es, ax_out
    }
}

void alloc_dma_buf(void) {
    dd size;
    dw channel;
    db *state = (db *)0x00e4u;
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    _asm {
        mov size, eax
        mov channel, cx
    }
    r.esi = 0;
    r.edi = 0;
    iplay_alloc_dma_fail(&r, state, size, channel);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
    }
}

static void gravis_dma_public(int nongravis) {
    db *state = (db *)0x00cfu;
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    r.esi = 0;
    r.edi = 0;
    iplay_gravis_dma_control(&r, state, nongravis ? "nongravis_dma" : "sub_182DB");
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void sub_182DB(void) { gravis_dma_public(0); }
void nongravis_dma(void) { gravis_dma_public(1); }

void sub_1279A(void) {
    db *state = (db *)0x0018u;
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    r.esi = 0;
    r.edi = 0;
    iplay_sub_1279a_dma(&r, state);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void program_dma(void) {
    db *mode = (db *)0x00cfu;
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    r.esi = 0;
    r.edi = 0;
    iplay_program_dma_channel1(&r, mode);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

static void mem_strategy_public(int mode) {
    dw config_word;
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov config_word, bx
    }
    r.esi = 0;
    r.edi = 0;
    iplay_mem_strategy(&r,
                       mode == 2 ? "setmemallocstrat" :
                       mode == 3 ? "getmemallocstrat" :
                       mode == 0 ? "setmemalloc1" : "setmemalloc2",
                       config_word);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void setmemalloc1(void) { mem_strategy_public(0); }
void setmemalloc2(void) { mem_strategy_public(1); }
void setmemallocstrat(void) { mem_strategy_public(2); }
void getmemallocstrat(void) { mem_strategy_public(3); }

void mem_reallocx(void) {
    dw size;
    db *book = (db *)0x0054u;
    _asm {
        mov size, di
    }
    iplay_mem_reallocx_bookkeeping(book, size);
    _asm {
        mov di, size
    }
}

void int24(void) {
    IplayRegs r;
    dw ax_in;
    dw ax_out;
    _asm {
        mov ax_in, ax
    }
    r.eax = ax_in;
    iplay_int24(&r);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
    }
}

void memfree_125DA(void) {
    IplayRegs r;
    dw ax_out;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    _asm {
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    r.eax = 0;
    iplay_memfree_125da_guard(&r);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
        mov si, si_in
        mov di, di_in
    }
}

void memfree_18A28(void) {
    IplayRegs r;
    dw ax_in;
    dw si_in;
    dw ax_out;
    _asm {
        mov ax_in, ax
        mov si_in, si
    }
    r.eax = ax_in;
    r.esi = si_in;
    iplay_memfree_18a28_guard(&r, (db)si_in);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
    }
}

void sub_12D35(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    iplay_sub_12d35_disable(&r, mem + 0x4f71u);
    _asm {
        mov ax, ax_in
        mov bx, bx_in
    }
}

void sub_12B18(void) {
    db *mem = (db *)0;
    dw si_in;
    dw channel_count;
    db *src_copy = mem + 0x3628u;

    _asm {
        mov si_in, si
    }
    memcpy(src_copy, mem + si_in, 32u);
    channel_count = abi_word(mem, 0x0034u);
    iplay_sub_12b18(mem, mem + 0x1368u, src_copy, channel_count, mem[0x0082u]);
}

void sub_12AFD(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw channel_count;
    db channel_index;
    dw bx_out;
    dw ax_out;

    _asm {
        mov ax_in, ax
        mov cx_in, cx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ecx = cx_in;
    channel_count = abi_word(mem, 0x0034u);
    channel_index = (db)(cx_in >> 8);
    iplay_sub_12afd(&r,
                    mem + 0x1368u,
                    channel_count,
                    channel_index,
                    mem[0x0082u]);
    ax_out = (dw)r.eax;
    bx_out = (channel_index >= channel_count)
        ? (dw)channel_index
        : (dw)(0x1368u + ((dw)channel_index * 0x50u));
    _asm {
        mov ax, ax_out
        mov bx, bx_out
    }
}

void sub_12B83(void) {
    IplayRegs r;
    db *mem = (db *)0;
    db types[32];
    dw ax_in;
    dw i;
    dw count;

    _asm {
        mov ax_in, ax
    }
    count = (dw)(ax_in & 0x00ffu);
    if (count >= 0x20u) count = 0x20u;
    if (count <= 2u) count = 2u;
    for (i = 0; i < count; ++i) {
        types[i] = mem[0x1368u + i * 0x50u + 0x1du];
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    iplay_sub_12b83(&r, mem, mem + 0x1368u, 0x50u, types);
    _asm {
        mov ax, ax_in
    }
}

void sub_131DA(void) {
    db *mem = (db *)0;
    dw bx_in;

    _asm {
        mov bx_in, bx
    }
    iplay_sub_131da(mem + bx_in);
}

void sub_131EF(void) {
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw volume;
    db value;
    db old_fine;
    dw product;
    dw ax_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    value = (db)ax_in;
    old_fine = mem[bx_in + 0x23u];
    volume = abi_word(mem, 0x005cu);
    if (mem[bx_in + 0x1du] != 1 && value > mem[0x00ddu]) {
        value = mem[0x00ddu];
    }
    product = (dw)((dw)value * volume);
    iplay_sub_131ef(mem + bx_in, (db)ax_in, volume, mem[0x00ddu]);
    ax_out = (mem[bx_in + 0x1du] == 1)
        ? (dw)(product & 0xff00u)
        : (dw)((product & 0xff00u) | old_fine);
    _asm {
        mov ax, ax_out
        mov bx, bx_in
    }
}

void sub_13177(void) {
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_out;
    dw dx_out;
    dw di_out;
    dd dword_245bc;
    dd dword_245c0;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    dword_245bc = abi_dword(mem, 0x001cu);
    dword_245c0 = abi_dword(mem, 0x0020u);
    iplay_sub_13177(mem + bx_in, ax_in, dword_245bc, dword_245c0, mem[0x007au]);
    cx_out = mem[0x007au];
    dx_out = 0;
    di_out = (dw)(ax_in << (mem[0x007au] & 15u));
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
    }
}

void someplaymode(void) {
    db *mem = (db *)0;
    dw channel_count;
    dw ax_out;
    dw cx_out;
    dw di_out;

    channel_count = abi_word(mem, 0x0034u);
    iplay_someplaymode(mem, mem + 0x1368u, channel_count, 0x50u);
    ax_out = 0;
    cx_out = 0;
    di_out = (dw)(0x1368u + channel_count * 0x50u);
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov di, di_out
    }
}

void sub_13044(void) {
    db *mem = (db *)0;
    iplay_sub_13044(mem, mem + 0x3d68u);
}

void sub_12D05(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw di_in;
    dw ax_out;
    dw cx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.edi = di_in;
    iplay_sub_12d05(&r, mem + di_in, mem[0x00e0u], mem[0x0132u]);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov si, si_out
        mov di, di_out
    }
}

void sub_11C0C(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw ax_out;
    dw bx_out;
    dw si_out;
    _asm {
        mov ax_in, ax
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    iplay_sub_11c0c(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov si, si_out
    }
}



void useless_strange(void) {
    IplayRegs r;
    db *mem = (db *)0;
    db tmp[4];
    db far *dst;
    dd dst_ptr;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw di_out;
    dw bp_out;
    unsigned i;

    memset(&r, 0, sizeof(r));
    memset(tmp, 0, sizeof(tmp));
    iplay_useless_strange_short(&r, tmp);
    dst_ptr = abi_far_dword((db far *)mem, 0x1630u);
    dst = (db far *)MK_FP((dw)(dst_ptr >> 16), (dw)dst_ptr);
    for (i = 0; i < sizeof(tmp); ++i) dst[i] = tmp[i];
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    bp_out = (dw)r.ebp;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
        mov bp, bp_out
    }
}

void snd_on_parnt(void) {
    db *mem = (db *)0;
    iplay_snd_on_parnt_bounded(mem);
}

void memfree(void) {
    IplayRegs r;
    dw ax_out;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    _asm {
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    r.esi = 0;
    r.edi = 0;
    iplay_memfree_invalid(&r);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
        mov si, si_in
        mov di, di_in
    }
}

void ult_1150B(void) {
    IplayRegs r;
    dw ax_in;
    dw ax_out;
    dw cx_out;
    _asm {
        mov ax_in, ax
    }
    r.ebx = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_ult_1150b(&r, ax_in);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
    }
}

static void ega_seq_public(int set_mode) {
    IplayRegs r;
    dw bx_in;
    dw cx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw dx_out;
    _asm {
        mov bx_in, bx
        mov cx_in, cx
        mov si_in, si
        mov di_in, di
    }
    r.esi = 0;
    r.edi = 0;
    iplay_ega_seq_no_device(&r, set_mode);
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov dx, dx_out
        mov bx, bx_in
        mov cx, cx_in
        mov si, si_in
        mov di, di_in
    }
}

void set_egasequencer(void) {
    ega_seq_public(1);
}

void graph_1C070(void) {
    ega_seq_public(0);
}

void useless_unset_egaseq(void) {
    IplayRegs r;
    dw ax_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov ax_in, ax
    }
    r.esi = 0;
    r.edi = 0;
    iplay_useless_unset_egaseq(&r, (db)ax_in);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void eff_13A43(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    iplay_eff_13a43(&r, mem + bx_in, mem[0x0082]);
    _asm {
        mov ax, ax_in
    }
}

static void eff13bc0_axbx_preserve_public(unsigned op) {
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    if (op == 0) iplay_eff_13bc0(mem + bx_in, (db)ax_in);
    else iplay_eff_13c34(mem + bx_in, (db)ax_in);
    _asm {
        mov ax, ax_in
        mov bx, bx_in
    }
}

void eff_13BC0(void) { eff13bc0_axbx_preserve_public(0); }
void eff_13C34(void) { eff13bc0_axbx_preserve_public(1); }

static void channel_index_guard_public(int second_table) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw di_in;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov di_in, di
    }
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.edi = di_in;
    if (second_table) iplay_sub_13813_guard(&r, mem + bx_in);
    else iplay_sub_137d5_guard(&r, mem + bx_in);
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
        mov di, di_out
    }
}

void sub_137D5(void) { channel_index_guard_public(0); }
void sub_13813(void) { channel_index_guard_public(1); }

void sub_13826(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw ax_out;
    dw cx_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    iplay_sub_13826_full(&r, mem, mem + bx_in);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov cx, cx_out
        mov di, di_out
    }
}

static void eff_axbxdxdi_axdi_public(unsigned op) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw di_in;
    dw ax_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
        mov di_in, di
    }
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = 0;
    r.edx = dx_in;
    r.esi = 0;
    r.edi = di_in;
    if (op == 0) iplay_eff_13ba3(&r, mem + bx_in);
    else if (op == 1) iplay_eff_13bc8(&r, mem + bx_in, mem[0x007a]);
    else iplay_eff_13c3f(&r, mem + bx_in, mem[0x00c8], mem[0x0082]);
    ax_out = (dw)r.eax;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov dx, dx_in
        mov di, di_out
    }
}

void eff_13BA3(void) { eff_axbxdxdi_axdi_public(0); }

void vlm_141DF(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = 0;
    r.edi = 0;
    iplay_vlm_141df(&r, mem, abi_word(mem, 0x0034u));
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void change_volume(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    iplay_change_volume(&r, mem, mem + 0x1368u, abi_word(mem, 0x0034u));
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
    }
}

void sub_13CF6(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw dx_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;

    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov dx_in, dx
    }
    r.eax = ax_in;
    r.ebx = 0;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = 0;
    r.edi = 0;
    iplay_sub_13cf6(&r,
                    mem,
                    abi_word(mem, 0x00beu),
                    abi_word(mem, 0x0044u));
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void eff_13BC8(void) { eff_axbxdxdi_axdi_public(1); }
void eff_13C3F(void) { eff_axbxdxdi_axdi_public(2); }

static void eff_slide_axbxdx_ax_public(unsigned op) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.edx = dx_in;
    if (op == 0) iplay_eff_13e1e(&r, mem + bx_in);
    else iplay_eff_138d2(&r, mem + bx_in);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov dx, dx_in
    }
}

void eff_13E1E(void) { eff_slide_axbxdx_ax_public(0); }
void eff_138D2(void) { eff_slide_axbxdx_ax_public(1); }


static void eff138_axbx_ax_public(unsigned op) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw ax_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    if (op == 0) iplay_eff_13886(&r, mem + bx_in);
    else if (op == 1) iplay_eff_138a4(&r, mem + bx_in);
    else if (op == 2) iplay_eff_1387f(&r, mem + bx_in, mem[0x00c8]);
    else if (op == 3) iplay_eff_1389d(&r, mem + bx_in, mem[0x00c8]);
    else if (op == 4) iplay_eff_1392f(&r, mem + bx_in, mem[0x005e]);
    else if (op == 5) iplay_eff_13e2d(&r, mem + bx_in, mem[0x005e]);
    else iplay_eff_139b9(&r, mem + bx_in, mem[0x00dd]);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
    }
}

void eff_13886(void) { eff138_axbx_ax_public(0); }
void eff_138A4(void) { eff138_axbx_ax_public(1); }
void eff_1387F(void) { eff138_axbx_ax_public(2); }
void eff_1389D(void) { eff138_axbx_ax_public(3); }
void eff_1392F(void) { eff138_axbx_ax_public(4); }
void eff_13E2D(void) { eff138_axbx_ax_public(5); }
void eff_139B9(void) { eff138_axbx_ax_public(6); }

static void eff139_axbx_axdx_public(unsigned op) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    dw dx_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.edx = dx_in;
    if (op == 0) iplay_eff_139ac(&r, mem + bx_in, mem[0x00dd]);
    else if (op == 1) iplay_eff_139b2(&r, mem + bx_in, mem[0x00dd], mem[0x005e]);
    else iplay_eff_13ad7(&r, mem + bx_in, mem[0x00dd]);
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov dx, dx_out
    }
}

void eff_139AC(void) { eff139_axbx_axdx_public(0); }
void eff_139B2(void) { eff139_axbx_axdx_public(1); }
void eff_13AD7(void) { eff139_axbx_axdx_public(2); }

void calc_14043(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    memset(&r, 0, sizeof(r));
    iplay_calc_14043(&r, mem[0x00db], mem[0x00dc]);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
    }
}

void eff_14030(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw di_out;
    _asm {
        mov ax_in, ax
    }
    r.eax = ax_in;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.esi = 0;
    r.edi = (dw)(ax_in & 0x000f);
    iplay_eff_14030(&r, mem, mem[0x00dc],
                    abi_word(mem, 0x00beu),
                    abi_word(mem, 0x0048u));
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
    }
}

void eff_14067(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov ax_in, ax
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    iplay_eff_14067(&r, mem, mem[0x00db], mem[0x00dc],
                    abi_word(mem, 0x00beu),
                    abi_word(mem, 0x0048u));
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

static void eff140_axbxdx_axdx_public(unsigned op) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    dw dx_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.edx = dx_in;
    if (op == 0) iplay_sub_14087(&r, mem + bx_in, mem[0x00c8]);
    else if (op == 1) iplay_eff_13de5(&r, mem + bx_in, mem[0x00c8]);
    else if (op == 2) iplay_eff_13def(&r, mem + bx_in, mem[0x00c8]);
    else iplay_eff_13fbe(&r, mem + bx_in, mem[0x00c8]);
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov dx, dx_out
    }
}

void sub_14087(void) { eff140_axbxdx_axdx_public(0); }
void eff_13DE5(void) { eff140_axbxdx_axdx_public(1); }
void eff_13DEF(void) { eff140_axbxdx_axdx_public(2); }

void eff_13E8C(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw di_out;
    _asm {
        mov ax_in, ax
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    iplay_eff_13e8c(&r, mem,
                    abi_word(mem, 0x00beu),
                    abi_word(mem, 0x0048u));
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
    }
}

void eff_13F05(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw ax_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    iplay_eff_13f05(&r, mem + bx_in, mem[0x00c8]);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
    }
}

void eff_13F3B(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_out;
    dw dx_out;
    dw ax_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    iplay_eff_13f3b(&r, mem + bx_in, mem[0x00c8], mem[0x00dd]);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov cx, cx_out
        mov dx, dx_out
    }
}

void eff_13FBE(void) { eff140_axbxdx_axdx_public(3); }

static void eff13e_axbxdx_axcxdx_public(unsigned op) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.edx = dx_in;
    if (op == 0) iplay_eff_13e32(&r, mem + bx_in, mem[0x00c8], mem[0x00dd]);
    else iplay_eff_13e7f(&r, mem + bx_in, mem[0x00c8], mem[0x00dd]);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov cx, cx_out
        mov dx, dx_out
    }
}

void eff_13E32(void) { eff13e_axbxdx_axcxdx_public(0); }
void eff_13E7F(void) { eff13e_axbxdx_axcxdx_public(1); }

void eff_13E84(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.edx = dx_in;
    iplay_eff_13e84(&r, mem + bx_in, mem[0x00c8], mem[0x00dd], mem[0x005e]);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
    }
}

void spectr_1C4F8(void) {
    IplayRegs r;
    dd ebx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov ebx_in, ebx
    }
    memset(&r, 0, sizeof(r));
    r.ebx = ebx_in;
    iplay_spectr_1c4f8(&r);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void sub_13D95(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw cx_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov cx_in, cx
    }
    memset(&r, 0, sizeof(r));
    r.ecx = cx_in;
    iplay_sub_13d95(&r, mem);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void sub_13E9B(void) {
    IplayRegs r;
    dw ax_in;
    dw dx_in;
    dw di_in;
    dw ax_out;
    dw dx_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov dx_in, dx
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.edx = dx_in;
    r.edi = di_in;
    iplay_sub_13e9b_public(&r);
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov dx, dx_out
        mov di, di_out
    }
}

/*
 * Some no-device hardware entrypoints below intentionally remain inline ABI
 * shims even though equivalent pure C helpers exist in iplay_rewrite.c.
 * Several of these symbols are reached from the DOS loader startup path; routing
 * them through ordinary C calls changed practical live state and broke smoke
 * tests. Keep behavior in pure helpers for runner/parity coverage, but only wire
 * a public wrapper through C after the full DOS smoke gate proves it safe.
 */
static void midi_public_regs(dw ax_value, dw bx_value, dw cx_value, dw dx_value) {
    _asm {
        mov ax, ax_value
        mov bx, bx_value
        mov cx, cx_value
        mov dx, dx_value
    }
}

void midi_clean(void) { midi_public_regs(0xff00u, 0x5678u, 0, 0x0330u); }
void midi_sndoff(void) { midi_public_regs(0, 0x0010u, 0, 0x0330u); }
void midi_153C0(void) { midi_public_regs(0x3f00u, 0x5678u, 0, 0x0331u); }
void midi_153D6(void) { midi_public_regs(0, 0x5610u, 0, 0x0330u); }

static void timer_ax_public(int clean_mode) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw ax_out;
    _asm {
        mov ax_in, ax
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    if (clean_mode) iplay_clean_timer_no_device(&r, ax_in);
    else iplay_set_timer_no_device(&r, mem, ax_in);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
    }
}

void set_timer(void) { timer_ax_public(0); }
void clean_timer(void) { timer_ax_public(1); }

void set_timer_int(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_set_timer_int_alloc_fail(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
        mov es, ax
    }
}

void midi_153F1(void) {
    IplayRegs r;
    dw ax_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov ax_in, ax
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    iplay_midi_153f1_public(&r);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    midi_public_regs(ax_out, bx_out, cx_out, dx_out);
}

void midi_15442(void) { midi_public_regs(0x1200u, 0x5678u, 0, 0x0331u); }

void midi_15413(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw dx_in;

    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_midi_15413_guard(&r, mem + 0x00d7u);
    _asm {
        mov ax, ax_in
        mov cx, cx_in
        mov dx, dx_in
    }
}

void midi_154DA(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw ax_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    iplay_midi_154da(&r, mem + bx_in);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
    }
}

void midi_154DE(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    dw dx_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.edx = dx_in;
    iplay_midi_154de(&r, mem + bx_in);
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov dx, dx_out
    }
}

void change_amplif(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw di_in;
    dw ax_out;
    dw cx_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ecx = cx_in;
    r.edi = di_in;
    iplay_change_amplif(&r, mem, mem[0x00de]);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov di, di_out
    }
}

void eff_14020(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw si_in;
    dw ax_out;
    dw bx_out;
    dw si_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.esi = si_in;
    iplay_eff_14020(&r, mem, mem[0x00de]);
    ax_out = (dw)r.eax;
    bx_out = bx_in;
    si_out = si_in;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov si, si_out
    }
}

void midi_set(void) {
    IplayRegs r;
    dw cx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw dx_out;
    _asm {
        mov cx_in, cx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    iplay_midi_set_no_device(&r);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov dx, dx_out
        mov cx, cx_in
        mov si, si_in
        mov di, di_in
    }
}

static void sb16_probe_public_regs(const char *symbol) {
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;

    memset(&r, 0, sizeof(r));
    iplay_sb16_probe_public(&r, symbol);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void sb16_detect_port(void) { sb16_probe_public_regs("sb16_detect_port"); }
void sb16_sound_on(void) { sb16_probe_public_regs("sb16_sound_on"); }
void sb16_init(void) { sb16_probe_public_regs("sb16_init"); }

void sb16_handler_int(void) {
    IplayRegs r;
    db scratch[1];
    dw ax_out;
    dw cx_out;
    dw dx_out;

    memset(&r, 0, sizeof(r));
    iplay_sb16_int_ack(&r, scratch);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void int1a_timer(void) {
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;

    memset(&r, 0, sizeof(r));
    iplay_int1a_passthrough(&r);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void timer_int_end(void) {
    IplayRegs r;
    db scratch[3];
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;

    memset(&r, 0, sizeof(r));
    iplay_timer_int_end_disabled(&r, scratch);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void inr_read_118B0(void) {
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;

    memset(&r, 0, sizeof(r));
    iplay_inr_read_118b0_fail(&r);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void deinit_125B9(void) {
    IplayRegs r;
    db scratch[11];

    memset(&r, 0, sizeof(r));
    iplay_deinit_125b9_idle(&r, scratch);
}

void callsubx(void) {
    db scratch[17];

    iplay_callsubx_fail(scratch);
}

void dosexec(void) {
    IplayRegs r;
    db scratch[4];

    memset(&r, 0, sizeof(r));
    iplay_dosexec_no_comspec(&r, scratch);
}

void start(void) {
    IplayRegs r;
    db scratch[2];

    memset(&r, 0, sizeof(r));
    iplay_start_bounded(&r, scratch);
}

void keyb_19EFD(void) {
    db *mem = (db *)0;
    iplay_keyb_bounded(mem);
}


void sb16_18540(void) {
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;

    memset(&r, 0, sizeof(r));
    iplay_sb16_dma_public(&r);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void inr_read_119B7(void) {
    IplayRegs r;
    db *buf = (db *)0xbf68u;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_inr_read_119b7_eof(&r, buf);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
    }
}

void mod_readfile_12247(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_mod_readfile_12247_eof(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void mod_readfile_11F4E(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_mod_readfile_11f4e_public_layout(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void ult_read(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_ult_read_fast(&r, mem);
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
    }
}

static void sb16_public_off_regs(const char *symbol) {
    IplayRegs r;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;

    memset(&r, 0, sizeof(r));
    iplay_sb16_off_public(&r, symbol);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void sb16_sound_off(void) { sb16_public_off_regs("sb16_sound_off"); }
void sb16_deinit(void) { sb16_public_off_regs("sb16_deinit"); }
void sb_clean(void) { sb16_public_off_regs("sb_clean"); }
void sbpro_clean(void) { sb16_public_off_regs("sbpro_clean"); }
void sb16_off(void) { sb16_public_off_regs("sb16_off"); }
void sb_sndoff(void) { sb16_public_off_regs("sb_sndoff"); }
void sbpro_sndoff(void) { sb16_public_off_regs("sbpro_sndoff"); }

static void read_sb_public_regs(const char *symbol) {
    IplayRegs r;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_sb_helper_no_device(&r, symbol, dx_in);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

static void write_sb_public_regs(void) {
    IplayRegs r;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_sb_write_no_device(&r);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_in
        mov di, di_in
    }
}

void ReadSB(void) { read_sb_public_regs("ReadSB"); }
void ReadMixerSB(void) { read_sb_public_regs("ReadMixerSB"); }
void WriteSB(void) { write_sb_public_regs(); }
void WriteMixerSB(void) { write_sb_public_regs(); }

void CheckSB(void) {
    dw bx_in;
    dw cx_in;
    _asm {
        mov bx_in, bx
        mov cx_in, cx
    }
    _asm {
        mov ax, 0000h
        mov bx, bx_in
        mov cx, cx_in
        mov dx, 0226h
    }
}

void set_dmachn_mask(void) {
    IplayRegs r;
    dw ax_in;
    dw cx_in;
    dw ax_out;
    _asm {
        mov ax_in, ax
        mov cx_in, cx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    iplay_set_dmachn_mask_no_device(&r, cx_in);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
    }
}

void sb_test_interrupt(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_sb_test_interrupt_no_device(&r, mem + 0x00d0u);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        stc
    }
}

void adlib_18395(void) {}

void adlib_18389(void) {
    IplayRegs r;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    _asm {
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    iplay_adlib_delay_public(&r);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
        mov si, si_in
        mov di, di_in
    }
}


void sb_detect_irq(void) {
    IplayRegs r;
    dw ax_out;
    dw dx_out;

    memset(&r, 0, sizeof(r));
    iplay_sb_detect_irq_public(&r);
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov dx, dx_out
        stc
    }
}

static void fill_dmabuf_public(const char *symbol) {
    IplayRegs r;
    db *mem = (db *)0;
    dw si_reg;
    dw di_reg;
    dw cx_reg;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
        mov si_reg, si
        mov di_reg, di
        mov cx_reg, cx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_reg;
    r.edx = dx_in;
    r.esi = si_reg;
    r.edi = di_reg;
    iplay_fill_dma_small(&r, mem, symbol, si_reg, di_reg, cx_reg);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void fill_dmabuf8(void) { fill_dmabuf_public("fill_dmabuf8"); }
void fill_dmabuf8stereo(void) { fill_dmabuf_public("fill_dmabuf8stereo"); }
void fill_dmabuf16stereo(void) { fill_dmabuf_public("fill_dmabuf16stereo"); }




void f5_draw_spectr(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;
    dw bp_out;

    memset(&r, 0, sizeof(r));
    iplay_f5_draw_spectr_inactive(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    bp_out = (dw)r.ebp;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
        mov bp, bp_out
    }
}

void fill_dma(void) {
    IplayRegs r;
    db *mem = (db *)0;
    db tmp[8];
    db far *dma;
    dw dma_off;
    dw dma_seg;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;
    dw bp_out;

    dma_off = abi_word(mem, 0x0018u);
    dma_seg = abi_word(mem, 0x001au);
    memset(&r, 0, sizeof(r));
    memset(tmp, 0, sizeof(tmp));
    iplay_fill_dma_inactive_mono(&r, tmp, 0);
    dma = (db far *)MK_FP(dma_seg, dma_off);
    _fmemcpy(dma, tmp, sizeof(tmp));
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    bp_out = (dw)r.ebp;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
        mov bp, bp_out
    }
}

void mod_sub_delta(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw si_in;
    dw ax_out;
    dw cx_out;
    dw si_out;

    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ecx = cx_in;
    r.esi = si_in;
    iplay_mod_sub_delta(&r, mem, mem[0x00d4u], mem[0x00d5u], mem + 0x00d6u);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov si, si_out
    }
}

void sub_11BA6(void) {
    IplayRegs r;
    db *mem = (db *)0;
    db tmp[8];
    db far *dst;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw di_in;
    dw es_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw di_out;
    dw count;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov di_in, di
        mov es_in, es
    }
    memset(tmp, 0, sizeof(tmp));
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.edi = 0;
    iplay_sub_11ba6(&r, tmp, mem + 0x007bu);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    count = (dw)r.edi;
    di_out = (dw)(di_in + count);
    dst = (db far *)MK_FP(es_in, di_in);
    _fmemcpy(dst, tmp, count);
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
    }
}

void mod_102F5(void) {
    db *mem = (db *)0;
    dw value = iplay_mod_102f5(mem + 0x3a48u);
    mem[0x0052u] = (db)value;
    mem[0x0053u] = (db)(value >> 8);
    _asm {
        mov ax, value
        xor cx, cx
        mov si, 3ac8h
    }
}

void sub_126A9(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dd module_type;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw si_out;
    dw di_out;

    module_type = abi_dword(mem, 0x010cu);
    memset(&r, 0, sizeof(r));
    iplay_sub_126a9(&r,
                    abi_word(mem, 0x005au),
                    abi_word(mem, 0x0032u),
                    abi_word(mem, 0x0034u),
                    mem[0x0077u],
                    module_type);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov si, si_out
        mov di, di_out
        mov es, ax
    }
}

void sub_1265D(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw bp_out;
    dw si_out;
    dw di_out;

    memset(&r, 0, sizeof(r));
    iplay_sub_1265d(&r,
                    abi_word(mem, 0x005cu),
                    mem[0x0132u],
                    mem[0x00c6u],
                    mem[0x00c7u],
                    mem[0x0082u],
                    mem[0x0088u],
                    mem[0x0083u],
                    mem[0x00d1u],
                    abi_word(mem, 0x0056u),
                    abi_word(mem, 0x0050u));
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    bp_out = (dw)r.ebp;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov bp, bp_out
        mov si, si_out
        mov di, di_out
    }
}


void mod_1024A(void) {
    db *mem = (db *)0;
    static db tmp[6u + 31u * 0x40u];
    dw sample_count;
    dw freq;
    dw record_bytes;

    sample_count = abi_word(mem, 0x0032u);
    if (sample_count > 31u) sample_count = 31u;
    freq = abi_word(mem, 0x003eu);
    record_bytes = (dw)(sample_count * 0x40u);
    iplay_mod_1024a(tmp, sample_count, mem + 0xbf7cu, freq);
    memcpy(mem + 0x0024u, tmp, 4u);
    memcpy(mem + 0x00c2u, tmp + 4u, 2u);
    memcpy(mem + 0x1d68u, tmp + 6u, record_bytes);
    _asm {
        mov ax, sample_count
        mov bx, record_bytes
        mov cx, 0000h
        mov dx, freq
        mov si, 0bf7ch
        mov di, 1d68h
    }
}

void mod_1021E(void) {
    db *mem = (db *)0;
    db tmp[152];
    dw si_in;

    _asm {
        mov si_in, si
    }
    iplay_mod_1021e(tmp, mem[si_in], mem[si_in + 1u], mem + si_in + 2u, mem + 0xbf68u);
    memcpy(mem + 0x0058u, tmp, 4u);
    memcpy(mem + 0x3a48u, tmp + 4u, 128u);
    memcpy(mem + 0x0110u, tmp + 132u, 20u);
    _asm {
        mov ax, si_in
        mov cx, 0000h
        mov di, 0124h
    }
}

void recolortxt(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw ax_out;
    dw cx_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    iplay_recolor_txt(&r, mem);
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_in
        mov cx, cx_out
        mov di, di_out
    }
}

void mouse_1C7A9(void) {
    IplayRegs r;
    dw ax_in;
    dw bp_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bp_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;
    int miss;

    _asm {
        mov ax_in, ax
        mov bp_in, bp
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebp = bp_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    miss = iplay_mouse_1c7a9(&r);
    ax_out = (dw)r.eax;
    bp_out = (dw)r.ebp;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    if (miss) {
        _asm {
            mov ax, ax_out
            mov bp, bp_out
            mov cx, cx_out
            mov dx, dx_out
            mov si, si_out
            mov di, di_out
            stc
        }
    } else {
        _asm {
            mov ax, ax_out
            mov bp, bp_out
            mov cx, cx_out
            mov dx, dx_out
            mov si, si_out
            mov di, di_out
            clc
        }
    }
}

void mouse_1C7CF(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw bp_in;
    dw ax_out;
    dw bx_out;
    dw bp_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;
    int miss;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov bp_in, bp
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ebp = bp_in;
    miss = iplay_mouse_1c7cf(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    bp_out = (dw)r.ebp;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    if (miss) {
        _asm {
            mov ax, ax_out
            mov bx, bx_out
            mov bp, bp_out
            mov cx, cx_out
            mov dx, dx_out
            mov si, si_out
            mov di, di_out
            stc
        }
    } else {
        _asm {
            mov ax, ax_out
            mov bx, bx_out
            mov bp, bp_out
            mov cx, cx_out
            mov dx, dx_out
            mov si, si_out
            mov di, di_out
            clc
        }
    }
}

void draw_frame(void) {
    db *mem = (db *)0;
    db far *video = (db far *)MK_FP(abi_word(mem, 0x1632u),
                                    abi_word(mem, 0x1630u));
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    video_copy_in(video, 4000u);
    iplay_draw_frame(video_temp,
                     (db)ax_in,
                     (db)(ax_in >> 8),
                     (db)bx_in,
                     (db)cx_in,
                     (db)(cx_in >> 8),
                     (db)dx_in,
                     (db)(dx_in >> 8));
    video_copy_out(video, 4000u);
    _asm {
        mov ax, 0000h
        mov bx, 0000h
        mov cx, 0000h
        mov dx, 0000h
        mov si, 0000h
        mov di, 0000h
        mov bp, 0000h
    }
}

void message_1BE77(void) {
    IplayRegs r;
    db *mem = (db *)0;
    db far *video = (db far *)MK_FP(abi_word(mem, 0x1632u),
                                    abi_word(mem, 0x1630u));
    const dw temp_src = 0x0f00u;
    dw ax_in;
    dw si_in;
    dw ax_out;
    dw si_out;
    dw di_out;
    dw consumed;
    unsigned i;

    _asm {
        mov ax_in, ax
        mov si_in, si
    }
    video_copy_in(video, 4000u);
    for (i = 0; i < 0x80u && temp_src + i < 4000u; ++i) {
        video_temp[temp_src + i] = mem[si_in + i];
        if (mem[si_in + i] == 0) break;
    }
    if (i == 0x80u || temp_src + i >= 4000u) video_temp[temp_src + 0x7fu] = 0;
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.esi = temp_src;
    iplay_message_1be77(&r, video_temp, 0);
    video_copy_out(video, 4000u);
    consumed = (dw)((dw)r.esi - temp_src);
    ax_out = (dw)r.eax;
    si_out = (dw)(si_in + consumed);
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov si, si_out
        mov di, di_out
    }
}

void sub_1AB8C(void) {
    IplayRegs r;
    db channel[0x40];
    db far *src;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw si_in;
    dw fs_in;
    dw ax_out;
    dw si_out;
    unsigned i;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov si_in, si
        mov fs_in, fs
    }
    src = (db far *)MK_FP(fs_in, bx_in);
    _fmemcpy(channel, src, sizeof(channel));
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ecx = cx_in;
    r.esi = si_in;
    iplay_sub_1ab8c(&r, channel);
    ax_out = (dw)r.eax;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov si, si_out
    }
}

void txt_draw_top_title(void) {
    db *mem = (db *)0;
    db far *video = (db far *)MK_FP(abi_word(mem, 0x1632u),
                                    abi_word(mem, 0x1630u));
    video_copy_in(video, 4000u);
    iplay_txt_draw_top_title(video_temp);
    video_copy_out(video, 4000u);
    _asm {
        mov ax, 0000h
        mov bx, 0000h
        mov cx, 0000h
        mov dx, 0000h
        mov si, 0000h
        mov di, 0000h
        mov bp, 0000h
    }
}

void txt_draw_bottom(void) {
    db *mem = (db *)0;
    db far *video = (db far *)MK_FP(abi_word(mem, 0x1636u),
                                    abi_word(mem, 0x1634u));
    video_copy_in(video, 4000u);
    iplay_txt_draw_bottom(video_temp,
                          mem[0x1682u],
                          mem[0x1683u],
                          mem[0x1684u],
                          mem[0x1685u],
                          mem[0x1686u],
                          mem[0x1687u],
                          abi_word(mem, 0x167au),
                          abi_word(mem, 0x167cu));
    video_copy_out(video, 4000u);
    _asm {
        mov ax, 0000h
        mov bx, 0000h
        mov cx, 0000h
        mov dx, 0000h
        mov si, 0000h
        mov di, 0000h
        mov bp, 0000h
    }
}

void filelist_198B8(void) {
    db *mem = (db *)0;
    db far *video;
    db far *entry;
    dw video_off;
    dw video_seg;
    dw entry_seg;
    dw current;
    dw total;
    dw count;
    dw row_off;
    unsigned row;
    char name[13];

    video_off = abi_word(mem, 0x1630u);
    video_seg = abi_word(mem, 0x1632u);
    entry_seg = abi_word(mem, 0x1662u);
    total = abi_word(mem, 0x1664u);
    current = abi_word(mem, 0x166eu);
    if (current >= total) return;

    count = (dw)(total - current);
    if (count > 15u) count = 15u;
    entry_seg = (dw)(entry_seg + (dw)(current * 3u));
    row_off = 0x0654u;

    for (row = 0; row < count; ++row) {
        entry = (db far *)MK_FP(entry_seg, 0);
        video = (db far *)MK_FP(video_seg, (dw)(video_off + row_off));

        _fmemcpy(name, entry + 0x0cu, 12u);
        name[12] = 0;

        iplay_filelist_row(video_temp,
                           entry[2],
                           entry[3],
                           (dw)(entry[4] | ((dw)entry[5] << 8)),
                           (dw)(entry[6] | ((dw)entry[7] << 8)),
                           abi_far_dword(entry, 8u),
                           name);
        video_copy_out(video, 160u);

        entry_seg = (dw)(entry_seg + 3u);
        row_off = (dw)(row_off + 0x00a0u);
    }
}

void video_prp_mtr_positn(void) {
    db *mem = (db *)0;
    static db channels[0x50u * 32u];
    db far *src;
    dw count;
    dw off;
    dw seg;
    unsigned total;
    unsigned i;

    count = abi_word(mem, 0x1654u);
    if (count > 32u) count = 32u;
    off = abi_word(mem, 0x1638u);
    seg = abi_word(mem, 0x163au);
    src = (db far *)MK_FP(seg, off);
    total = (unsigned)count * 0x50u;
    _fmemcpy(channels, src, total);
    iplay_video_prp_mtr_positn(mem, channels, count);
    _asm {
        mov ax, 0000h
        mov bx, 0000h
        mov cx, 0000h
        mov dx, 0000h
        mov si, 0000h
        mov di, 0000h
        mov bp, 0000h
    }
}

void clean_11C43(void) {
    db *mem = (db *)0;
    dw ds_out;

    _asm {
        mov ds_out, ds
    }
    iplay_clean_11c43(mem, mem[0x00d2u], mem[0x007eu], mem[0x007fu]);
    _asm {
        mov ax, ds_out
        mov es, ax
        mov eax, 3f3f3f3fh
        xor cx, cx
        xor dx, dx
        mov di, 3d48h
    }
}

void sub_15577(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_sub_15577_disabled(&r, mem + si_in);
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
        mov si, si_in
        mov di, di_in
    }
}

void sub_154F4(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw bp_in;
    dw si_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw bp_out;
    dw si_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov bp_in, bp
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.ebp = bp_in;
    r.esi = si_in;
    iplay_sub_154f4(&r, mem, mem + si_in);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    bp_out = (dw)r.ebp;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov bp, bp_out
        mov si, si_out
    }
}

void midi_154AC(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw di_in;
    dw ax_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.edi = di_in;
    iplay_midi_154ac(&r, mem + bx_in, mem[0x00ddu]);
    ax_out = (dw)r.eax;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov di, di_out
    }
}

static void midi_channel_event_public(int note_off) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_midi_channel_event_no_device(&r, mem, mem + bx_in, note_off);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void midi_1544D(void) { midi_channel_event_public(1); }
void midi_15466(void) { midi_channel_event_public(0); }

static void int_vector_public(int set_mode) {
    IplayRegs r;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    db int_number;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
    }
    int_number = (db)ax_in;
    memset(&r, 0, sizeof(r));
    if (set_mode) {
        int_vector_off[int_number] = bx_in;
        int_vector_seg[int_number] = dx_in;
        iplay_int_vector_roundtrip(&r, int_number, bx_in, dx_in);
        ax_out = (dw)(0x2500u | int_number);
        bx_out = bx_in;
        cx_out = (dw)r.ecx;
        dx_out = bx_in;
    } else {
        iplay_int_vector_roundtrip(&r, int_number,
                                   int_vector_off[int_number],
                                   int_vector_seg[int_number]);
        ax_out = (dw)r.eax;
        bx_out = (dw)r.ebx;
        cx_out = (dw)r.ecx;
        dx_out = (dw)r.edx;
    }
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
    }
}

void setint_vect(void) { int_vector_public(1); }
void getint_vect(void) { int_vector_public(0); }

static void snd_vector_public(int restore_mode) {
    IplayRegs r;
    db scratch[8];
    dw ax_in;
    dw si_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    if (!restore_mode) {
        snd_vector_irq = (db)ax_in;
        snd_vector_old_off = 0;
        snd_vector_old_seg = 0;
    }
    iplay_snd_vector_roundtrip(&r, scratch, snd_vector_irq,
                                snd_vector_old_off, snd_vector_old_seg);
    if (!restore_mode) {
        ax_out = ax_in;
        bx_out = (dw)r.ebx;
        cx_out = (dw)r.ecx;
        dx_out = (dw)r.edx;
        si_out = si_in;
        di_out = (dw)r.edi;
    } else {
        ax_out = (dw)r.eax;
        bx_out = (dw)r.ebx;
        cx_out = (dw)r.ecx;
        dx_out = (dw)r.edx;
        si_out = (dw)r.esi;
        di_out = (dw)r.edi;
    }
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void setsnd_handler(void) { snd_vector_public(0); }
void restore_intvector(void) { snd_vector_public(1); }


static void eff_axbx_ax_public(unsigned op) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw ax_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    if (op == 0) iplay_eff_13b78(&r, mem + bx_in, mem[0x00ddu]);
    else if (op == 1) iplay_eff_13a94(&r, mem + bx_in, mem[0x007au]);
    else if (op == 2) iplay_eff_13b06(&r, mem, mem[0x00d2u]);
    else if (op == 3) iplay_eff_13ca2(&r, mem, mem[0x00c8u]);
    else if (op == 4) iplay_eff_13cb3(&r, mem + bx_in, mem[0x00c8u]);
    else if (op == 5) iplay_eff_13ce8(&r, mem);
    else if (op == 6) iplay_eff_13cc9(&r, mem, mem[0x00c8], mem[0x00cd]);
    else if (op == 7) iplay_eff_13cdd(&r, mem, mem[0x00d2]);
    else if (op == 8) iplay_eff_13c02(&r, mem + bx_in, mem, abi_word(mem, 0x0056u));
    else iplay_eff_13bb2(&r, mem + bx_in);
    ax_out = (dw)r.eax;
    _asm {
        mov ax, ax_out
    }
}

void eff_13B78(void) { eff_axbx_ax_public(0); }
void eff_13A94(void) { eff_axbx_ax_public(1); }
void eff_13B06(void) { eff_axbx_ax_public(2); }
void eff_13CA2(void) { eff_axbx_ax_public(3); }
void eff_13CB3(void) { eff_axbx_ax_public(4); }
void eff_13CE8(void) { eff_axbx_ax_public(5); }
void eff_13CC9(void) { eff_axbx_ax_public(6); }
void eff_13CDD(void) { eff_axbx_ax_public(7); }
void eff_13C02(void) { eff_axbx_ax_public(8); }
void eff_13BB2(void) { eff_axbx_ax_public(9); }


static void eff_axbxdx_axdx_public(unsigned op) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw dx_in;
    dw ax_out;
    dw dx_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.edx = dx_in;
    if (op == 0) iplay_eff_13c64(&r, mem + bx_in, mem[0x00c8u]);
    else if (op == 1) iplay_eff_13c88(&r, mem + bx_in, mem[0x00c8u], mem[0x00ddu]);
    else if (op == 2) iplay_eff_13c95(&r, mem + bx_in, mem[0x00c8u]);
    else iplay_eff_13b88(&r, mem);
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov dx, dx_out
    }
}

void eff_13C64(void) { eff_axbxdx_axdx_public(0); }
void eff_13C88(void) { eff_axbxdx_axdx_public(1); }
void eff_13C95(void) { eff_axbxdx_axdx_public(2); }
void eff_13B88(void) { eff_axbxdx_axdx_public(3); }

void sub_13429(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    iplay_sub_13429_guard(&r, mem + bx_in);
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
    }
}


void sub_1415E(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw si_in;
    dw ax_out;
    dw bx_out;
    dw si_out;
    dw index;
    dw total;
    db segment_index;
    db pending;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov si_in, si
    }
    index = abi_word(mem, 0x0050u);
    total = abi_word(mem, 0x005au);
    segment_index = mem[0x0054u];
    pending = mem[0x0056u];
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.esi = si_in;
    iplay_sub_1415e(&r, mem, index, total, segment_index, pending);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov si, si_out
    }
}

void sub_12F56(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw si_in;
    dw ax_out;
    dw cx_out;
    dw si_out;
    dw index;
    dw total;
    db segment_index;
    db pending;
    db bh;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov si_in, si
    }
    index = ax_in;
    total = abi_word(mem, 0x005au);
    segment_index = mem[0x0054u];
    pending = (db)bx_in;
    bh = (db)(bx_in >> 8);
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.esi = si_in;
    iplay_sub_12f56(&r, mem, index, total, segment_index, pending, bh);
    ax_out = (dw)r.eax;
    cx_out = cx_in;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov si, si_out
    }
}

void sub_135CA(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw si_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw si_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov si_in, si
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.esi = si_in;
    iplay_sub_135ca_zero_event(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    si_out = (dw)r.esi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov si, si_out
    }
}

void spectr_1B084(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;
    dw bp_out;

    _asm {
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.edi = di_in;
    iplay_spectr_1b084_len2(&r, mem, di_in);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    bp_out = (dw)r.ebp;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
        mov bp, bp_out
    }
}

void sub_140B6(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    iplay_sub_140b6_guard(&r, mem);
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
    }
}


void sub_13017(void) {
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw di_in;

    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov di_in, di
    }
    iplay_sub_13017_bounded(mem, mem + 0x1d68u, abi_word(mem, 0x0032u));
    _asm {
        mov ax, ax_in
        mov cx, cx_in
        mov di, di_in
    }
}

void configure_timer(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw dx_in;
    dw di_in;
    dw ax_out;
    dw cx_out;
    dw dx_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov dx_in, dx
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.edi = di_in;
    iplay_configure_timer_bounded(&r, mem, mem + 0x1d68u, abi_word(mem, 0x0032u));
    ax_out = (dw)r.eax;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
    }
}

void sub_13623(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_sub_13623_guard(&r, abi_word(mem, 0x0034u));
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void sub_12CAD(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_sub_12cad_guard(&r, mem + 0x0106u, abi_word(mem, 0x0034u));
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void sub_12DA8(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw bp_in;
    dw si_in;
    dw di_in;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov bp_in, bp
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.ebp = bp_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_sub_12da8_guard(&r, mem);
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
        mov bp, bp_in
        mov si, si_in
        mov di, di_in
    }
}

void sub_1281A(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw bp_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov bp_in, bp
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.ebp = bp_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_sub_1281a_small(&r, mem + di_in, mem, mem + 0x3d68u, mem + 0x1368u, abi_word(mem, 0x0070u), abi_word(mem, 0x0072u));
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov bp, bp_in
        mov si, si_out
        mov di, di_out
    }
}

void sub_1609F(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw buffer_size;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    buffer_size = abi_word(mem, 0x0044u);
    iplay_sub_1609f_disabled(&r, mem + di_in, buffer_size);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov si, si_out
        mov di, di_out
    }
}

void volume_prep(void) {
    IplayRegs r;
    db *mem = (db *)0;
    db far *dst;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw es_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
        mov es_in, es
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    dst = (db far *)MK_FP(es_in, di_in);
    iplay_volume_prep_far(&r, mem, dst, ax_in, cx_in);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

void memfill8080(void) {
    db *mem = (db *)0;
    db far *dma;
    dw dma_off;
    dw dma_seg;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw di_out;

    _asm {
        mov dx_out, dx
    }
    dma_off = abi_word(mem, 0x0018u);
    dma_seg = abi_word(mem, 0x001au);
    dma = (db far *)MK_FP(dma_seg, dma_off);
    iplay_memfill8080_far_fill(dma);

    ax_out = 0x8080u;
    bx_out = 0xdef0u;
    cx_out = 0;
    di_out = 0x1000u;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
        mov ax, dma_seg
        mov es, ax
        mov ax, ax_out
    }
}



static void sb_on_public(const char *symbol) {
    db *mem = (db *)0;
    dw ax_in;
    dw cx_in;
    dw dx_in;
    dw si_in;

    _asm {
        mov ax_in, ax
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
    }
    iplay_sb_on_bounded(mem, symbol);
    _asm {
        mov ax, ax_in
        mov cx, cx_in
        mov dx, dx_in
        mov si, si_in
    }
}

void sb_on(void) { sb_on_public("sb_on"); }
void sb16_on(void) { sb_on_public("sb16_on"); }

void sb_handler_int(void) {
    db *mem = (db *)0;
    dw ax_in;
    dw dx_in;

    _asm {
        mov ax_in, ax
        mov dx_in, dx
    }
    iplay_sb_handler_int_bounded(0, mem);
    _asm {
        mov ax, ax_in
        mov dx, dx_in
    }
}

void sub_19050(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw dx_in;
    dw ax_out;
    dw dx_out;

    _asm {
        mov ax_in, ax
        mov dx_in, dx
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.edx = dx_in;
    iplay_sub_19050_bounded(&r, mem);
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    _asm {
        mov ax, ax_out
        mov dx, dx_out
    }
}



#ifndef IPLAY_PLAYER_OMIT_RISKY_UI_ABI
void spectr_1B406(void) {
    db *mem = (db *)0;
    dw di_in;

    _asm {
        mov di_in, di
    }
    iplay_spectr_1b406_small(mem, di_in);
}
#endif

void spectr_1BCE9(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw dx_in;
    dw di_in;
    dw ax_out;
    dw dx_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov dx_in, dx
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.edx = dx_in;
    r.edi = di_in;
    iplay_spectr_1bce9_equal(&r, mem);
    ax_out = (dw)r.eax;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov dx, dx_out
        mov di, di_out
    }
}

void spectr_1BC2D(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw di_in;
    dw bp_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw di_out;
    dw bp_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov di_in, di
        mov bp_in, bp
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.edi = di_in;
    r.ebp = bp_in;
    iplay_spectr_1bc2d_equal(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    di_out = (dw)r.edi;
    bp_out = (dw)r.ebp;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov di, di_out
        mov bp, bp_out
    }
}

void spectr_1BBC1(void) {
    IplayRegs r;
    db *mem = (db *)0;
    dw ax_in;
    dw bx_in;
    dw cx_in;
    dw dx_in;
    dw si_in;
    dw di_in;
    dw ax_out;
    dw bx_out;
    dw cx_out;
    dw dx_out;
    dw si_out;
    dw di_out;

    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
        mov di_in, di
    }
    memset(&r, 0, sizeof(r));
    r.eax = ax_in;
    r.ebx = bx_in;
    r.ecx = cx_in;
    r.edx = dx_in;
    r.esi = si_in;
    r.edi = di_in;
    iplay_spectr_1bbc1_zero(&r, mem);
    ax_out = (dw)r.eax;
    bx_out = (dw)r.ebx;
    cx_out = (dw)r.ecx;
    dx_out = (dw)r.edx;
    si_out = (dw)r.esi;
    di_out = (dw)r.edi;
    _asm {
        mov ax, ax_out
        mov bx, bx_out
        mov cx, cx_out
        mov dx, dx_out
        mov si, si_out
        mov di, di_out
    }
}

#pragma code_seg()
#endif
