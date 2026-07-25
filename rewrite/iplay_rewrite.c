#include "iplay_rewrite.h"
#include <stdio.h>
#include <string.h>

static void apply_full_regs6(IplayRegs *r, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
static void apply_eax_reg(IplayRegs *r, dd eax);
static void apply_ebp_reg(IplayRegs *r, dd ebp);
static void apply_esi_reg(IplayRegs *r, dd esi);
static void apply_edi_reg(IplayRegs *r, dd edi);
static void apply_eax_edi_regs(IplayRegs *r, dd eax, dd edi);
static void apply_eax_esi_regs(IplayRegs *r, dd eax, dd esi);
static void apply_ecx_esi_regs(IplayRegs *r, dd ecx, dd esi);
static void apply_eax_edx_regs(IplayRegs *r, dd eax, dd edx);
static void apply_eax_ecx_edx_regs(IplayRegs *r, dd eax, dd ecx, dd edx);
static void apply_mix_setup_regs(IplayRegs *r, dd ebx, dd ebp, dd ecx, dd esi);
static void apply_sndsettings_regs(IplayRegs *r, dd eax, dd ebx, dd ecx, dd edx, dd ebp, dd esi);
static dd abi_eax(const IplayRegs *r);
static dd abi_ebx(const IplayRegs *r);
static dd abi_ebp(const IplayRegs *r);
static dd abi_ecx(const IplayRegs *r);
static dd abi_edx(const IplayRegs *r);
static dd abi_esi(const IplayRegs *r);
static dd abi_edi(const IplayRegs *r);
static void put_word(db *mem, unsigned off, dw value);
static dw get_word(const db *mem, unsigned off);
static dd get_dword(const db *mem, unsigned off);
static dw eff_13de5_def_period_core(dw current, dw step, db up);

const IplayTextMode IPLAY_TEXT_MODE_40X25 = { IPLAY_TEXT_COLS_40, IPLAY_TEXT_ROWS_25 };
const IplayTextMode IPLAY_TEXT_MODE_80X25 = { IPLAY_TEXT_COLS_80, IPLAY_TEXT_ROWS_25 };
const IplayTextMode IPLAY_TEXT_MODE_80X28 = { IPLAY_TEXT_COLS_80, IPLAY_TEXT_ROWS_28 };
const IplayTextMode IPLAY_TEXT_MODE_80X50 = { IPLAY_TEXT_COLS_80, IPLAY_TEXT_ROWS_50 };
const IplayTextMode IPLAY_TEXT_DEFAULT_MODE = { IPLAY_TEXT_DEFAULT_COLS, IPLAY_TEXT_DEFAULT_ROWS };
static const IplayTextMode *const iplay_supported_text_modes[IPLAY_TEXT_SUPPORTED_MODE_COUNT] = {
    &IPLAY_TEXT_MODE_40X25,
    &IPLAY_TEXT_MODE_80X25,
    &IPLAY_TEXT_MODE_80X28,
    &IPLAY_TEXT_MODE_80X50
};
static db iplay_runtime_fallback_cells[IPLAY_TEXT_FALLBACK_SCREEN_BYTES];
const IplayAudioFormat IPLAY_AUDIO_SB16_STEREO_16 = { 44100u, 16u, 2u, 1u };
const IplayAudioFormat IPLAY_AUDIO_U8_MONO = { 44100u, 8u, 1u, 0u };
const IplayAudioFormat IPLAY_AUDIO_U8_STEREO = { 44100u, 8u, 2u, 0u };
const IplayAudioFormat IPLAY_AUDIO_S16_MONO = { 44100u, 16u, 1u, 1u };
const IplayAudioFormat IPLAY_AUDIO_S16_STEREO = { 44100u, 16u, 2u, 1u };
static const IplayTextMode *iplay_current_text_mode = &IPLAY_TEXT_DEFAULT_MODE;

static dd abi_eax(const IplayRegs *r) { return r->eax; }
static dd abi_ebx(const IplayRegs *r) { return r->ebx; }
static dd abi_ebp(const IplayRegs *r) { return r->ebp; }
static dd abi_ecx(const IplayRegs *r) { return r->ecx; }
static dd abi_edx(const IplayRegs *r) { return r->edx; }
static dd abi_esi(const IplayRegs *r) { return r->esi; }
static dd abi_edi(const IplayRegs *r) { return r->edi; }

static db hex_digit(db value) {
    value &= 0x0f;
    value = (db)(value + '0');
    if (value > '9') value = (db)(value + 7);
    return value;
}

static db write_hex_nibble(db *mem, dw *offset, db value) {
    db digit = hex_digit(value);
    mem[*offset] = digit;
    *offset = (dw)(*offset + 1u);
    return digit;
}

static db write_hex_byte(db *mem, dw *offset, db value) {
    write_hex_nibble(mem, offset, (db)(value >> 4));
    return write_hex_nibble(mem, offset, value);
}

static db write_hex_word(db *mem, dw *offset, dw value) {
    write_hex_byte(mem, offset, (db)(value >> 8));
    return write_hex_byte(mem, offset, (db)value);
}

static db write_hex_dword(db *mem, dw *offset, dd value) {
    write_hex_word(mem, offset, (dw)(value >> 16));
    return write_hex_word(mem, offset, (dw)value);
}

db iplay_hex4_to_buffer(db *mem, dw *offset, db value) {
    return write_hex_nibble(mem, offset, value);
}

db iplay_hex8_to_buffer(db *mem, dw *offset, db value) {
    return write_hex_byte(mem, offset, value);
}

db iplay_hex16_to_buffer(db *mem, dw *offset, dw value) {
    return write_hex_word(mem, offset, value);
}

db iplay_hex32_to_buffer(db *mem, dw *offset, dd value) {
    return write_hex_dword(mem, offset, value);
}

static db write_u32_base(db *mem, dw *offset, dd value, unsigned base, dw *count) {
    db digit;
    if (value >= base) {
        write_u32_base(mem, offset, value / base, base, count);
    }
    digit = (db)('0' + (value % base));
    mem[*offset] = digit;
    *offset = (dw)(*offset + 1u);
    *count = (dw)(*count + 1u);
    return digit;
}

static void write_counted_char(db *mem, dw *offset, dw *count, db value) {
    mem[*offset] = value;
    *offset = (dw)(*offset + 1u);
    *count = (dw)(*count + 1u);
}

dw iplay_put_counted_char_to_buffer(db *mem, dw *offset, dw count, db value) {
    write_counted_char(mem, offset, &count, value);
    return count;
}

static void apply_u32toa_regs(IplayRegs *r, dd eax, dw cx, dw si, db digit) {
    dd edx = abi_edx(r);
    apply_full_regs6(r, eax, abi_ebx(r), cx, (edx & 0xffffff00UL) | digit, si, abi_edi(r));
}

static IplayDecimalResult decimal_result(dd magnitude, dw count, dw offset, db last_digit) {
    IplayDecimalResult result;
    result.magnitude = magnitude;
    result.count = count;
    result.offset = offset;
    result.last_digit = last_digit;
    return result;
}

static IplayDecimalResult write_unsigned_decimal(db *mem, dw *offset, dd value) {
    dw count = 0;
    db digit = write_u32_base(mem, offset, value, 10, &count);
    return decimal_result(value, count, *offset, digit);
}

static db write_i32_decimal(db *mem, dw *offset, int32_t value, dw *count, dd *magnitude) {
    if (value < 0) {
        write_counted_char(mem, offset, count, '-');
        *magnitude = (dd)(-(value + 1)) + 1u;
    } else {
        *magnitude = (dd)value;
    }
    return write_u32_base(mem, offset, *magnitude, 10, count);
}

static IplayDecimalResult write_signed_decimal(db *mem, dw *offset, int32_t value) {
    dw count = 0;
    dd magnitude = 0;
    db digit = write_i32_decimal(mem, offset, value, &count, &magnitude);
    return decimal_result(magnitude, count, *offset, digit);
}

IplayDecimalResult iplay_u8_decimal_to_buffer(db *mem, dw *offset, db value) {
    return write_unsigned_decimal(mem, offset, value);
}

IplayDecimalResult iplay_u16_decimal_to_buffer(db *mem, dw *offset, dw value) {
    return write_unsigned_decimal(mem, offset, value);
}

IplayDecimalResult iplay_u32_decimal_to_buffer(db *mem, dw *offset, dd value) {
    return write_unsigned_decimal(mem, offset, value);
}

IplayDecimalResult iplay_i8_decimal_to_buffer(db *mem, dw *offset, db value) {
    return write_signed_decimal(mem, offset, (int8_t)value);
}

IplayDecimalResult iplay_i16_decimal_to_buffer(db *mem, dw *offset, dw value) {
    return write_signed_decimal(mem, offset, (int16_t)value);
}

IplayDecimalResult iplay_i32_decimal_to_buffer(db *mem, dw *offset, dd value) {
    return write_signed_decimal(mem, offset, (int32_t)value);
}

static dw string_len_at(const db *mem, dw offset) {
    dw len = 0;
    while (mem[(dw)(offset + len)] != 0) ++len;
    return len;
}

dw iplay_string_length_at(const db *mem, dw offset) {
    return string_len_at(mem, offset);
}

static db copy_count_until_nul(const db *src_mem, db *dst_mem, dw *src, dw *dst, dw *count) {
    db value;
    *count = 0;
    for (;;) {
        value = src_mem[*src];
        *count = (dw)(*count + 1u);
        if (value == 0) return value;
        dst_mem[*dst] = value;
        *src = (dw)(*src + 1u);
        *dst = (dw)(*dst + 1u);
    }
}

static int copy_printable_counted(const db *src_mem, db *dst_mem, dw *src, dw *dst, dw *remaining, db *last) {
    int seen = 0;
    while (*remaining != 0) {
        db value = src_mem[*src];
        *src = (dw)(*src + 1u);
        *last = value;
        seen = 1;
        if (value < 0x20u) return 1;
        dst_mem[*dst] = value;
        *dst = (dw)(*dst + 1u);
        *remaining = (dw)(*remaining - 1u);
    }
    return seen;
}

IplayStringCopyResult iplay_strcpy_count_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset) {
    IplayStringCopyResult result;
    dw si = src_offset;
    dw di = dst_offset;
    dw count;
    db last = copy_count_until_nul(src_mem, dst_mem, &si, &di, &count);
    result.src_offset = si;
    result.dst_offset = di;
    result.count = count;
    result.last_byte = last;
    result.copied_any = 1;
    return result;
}

IplayStringCopyResult iplay_copy_printable_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, dw count) {
    IplayStringCopyResult result;
    dw si = src_offset;
    dw di = dst_offset;
    dw remaining = count;
    db last = 0;
    int copied_any = copy_printable_counted(src_mem, dst_mem, &si, &di, &remaining, &last);
    result.src_offset = si;
    result.dst_offset = di;
    result.count = remaining;
    result.last_byte = last;
    result.copied_any = (db)(copied_any != 0);
    return result;
}

IplayStringCopyResult iplay_copy_printable_padded_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, dw count) {
    IplayStringCopyResult result = iplay_copy_printable_to_buffer(src_mem, dst_mem, src_offset, dst_offset, count);
    dw dst = result.dst_offset;
    dw i;
    for (i = 0; i < result.count; ++i) {
        dst_mem[dst] = ' ';
        dst = (dw)(dst + 1u);
    }
    result.dst_offset = dst;
    result.count = 0;
    return result;
}

static db copy_attributed_fixed(const db *src_mem, db *dst_mem, dw *src, dw *dst, dw count, db attr) {
    db value = 0;
    dw i;
    for (i = 0; i < count; ++i) {
        value = src_mem[*src];
        *src = (dw)(*src + 1u);
        dst_mem[*dst] = value;
        *dst = (dw)(*dst + 1u);
        dst_mem[*dst] = attr;
        *dst = (dw)(*dst + 1u);
    }
    return value;
}

IplayAttributedTextResult iplay_copy_attributed_fixed_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, dw count, db attr) {
    IplayAttributedTextResult result;
    dw si = src_offset;
    dw di = dst_offset;
    db al = copy_attributed_fixed(src_mem, dst_mem, &si, &di, count, attr);
    result.src_offset = si;
    result.dst_offset = di;
    result.ax = (dw)(((dw)attr << 8) | al);
    return result;
}

static dw put_attributed_message(const db *src_mem, db *dst_mem, dw *src, dw *dst, db attr, int initial, db initial_ch) {
    db value;
    if (initial) {
        dst_mem[*dst] = initial_ch;
        *dst = (dw)(*dst + 1u);
        dst_mem[*dst] = attr;
        *dst = (dw)(*dst + 1u);
    }
    for (;;) {
        value = src_mem[*src];
        *src = (dw)(*src + 1u);
        if (value == 0) break;
        dst_mem[*dst] = value;
        *dst = (dw)(*dst + 1u);
        dst_mem[*dst] = attr;
        *dst = (dw)(*dst + 1u);
    }
    return (dw)attr << 8;
}

IplayAttributedTextResult iplay_put_attributed_message_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, db attr, int initial, db initial_ch) {
    IplayAttributedTextResult result;
    dw si = src_offset;
    dw di = dst_offset;
    dw ax = put_attributed_message(src_mem, dst_mem, &si, &di, attr, initial, initial_ch);
    result.src_offset = si;
    result.dst_offset = di;
    result.ax = ax;
    return result;
}

static dw put_controlled_attributed_text(const db *src_mem, db *dst_mem, dw *src, dw *dst, db attr) {
    for (;;) {
        db value = src_mem[*src];
        *src = (dw)(*src + 1u);
        if (value == 0) break;
        if (value == 2) {
            attr = src_mem[*src];
            *src = (dw)(*src + 1u);
            continue;
        }
        if (value == 1) {
            *dst = (dw)(dst_mem[*src] | ((dw)dst_mem[(dw)(*src + 1u)] << 8));
            *src = (dw)(*src + 2u);
            continue;
        }
        dst_mem[*dst] = value;
        *dst = (dw)(*dst + 1u);
        dst_mem[*dst] = attr;
        *dst = (dw)(*dst + 1u);
    }
    return (dw)attr << 8;
}

IplayAttributedTextResult iplay_put_controlled_attributed_text_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, db attr) {
    IplayAttributedTextResult result;
    dw si = src_offset;
    dw di = dst_offset;
    dw ax = put_controlled_attributed_text(src_mem, dst_mem, &si, &di, attr);
    result.src_offset = si;
    result.dst_offset = di;
    result.ax = ax;
    return result;
}

static dw put_screen_stream(const db *src_mem, db *dst_mem, dw *src, dw base, dw *dst) {
    db attr;
    *dst = (dw)(base + ((dw)src_mem[*src] | ((dw)src_mem[(dw)(*src + 1u)] << 8)));
    *src = (dw)(*src + 2u);
    attr = src_mem[*src];
    *src = (dw)(*src + 1u);
    for (;;) {
        db value = src_mem[*src];
        *src = (dw)(*src + 1u);
        if (value == 0) break;
        if (value == 1) {
            *dst = (dw)(base + ((dw)src_mem[*src] | ((dw)src_mem[(dw)(*src + 1u)] << 8)));
            *src = (dw)(*src + 2u);
            continue;
        }
        if (value == 2) {
            attr = src_mem[*src];
            *src = (dw)(*src + 1u);
            continue;
        }
        dst_mem[*dst] = value;
        *dst = (dw)(*dst + 1u);
        dst_mem[*dst] = attr;
        *dst = (dw)(*dst + 1u);
    }
    return (dw)attr << 8;
}

IplayScreenStreamResult iplay_write_screen_stream_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw base_offset) {
    IplayScreenStreamResult result;
    dw si = src_offset;
    dw di;
    dw ax = put_screen_stream(src_mem, dst_mem, &si, base_offset, &di);
    result.src_offset = si;
    result.dst_offset = di;
    result.ax = ax;
    return result;
}

void iplay_u4tox(IplayRegs *r, db *mem) {
    dd eax = abi_eax(r);
    dd esi = abi_esi(r);
    dw si = (dw)esi;
    db al = write_hex_nibble(mem, &si, (db)eax);
    apply_eax_esi_regs(r, (eax & 0xffffff00UL) | al,
                       (esi & 0xffff0000UL) | si);
}

void iplay_u8tox(IplayRegs *r, db *mem) {
    dd saved = abi_eax(r);
    dd esi = abi_esi(r);
    dw si = (dw)esi;
    db al = write_hex_byte(mem, &si, (db)saved);
    apply_eax_esi_regs(r, (saved & 0xffffff00UL) | al,
                       (esi & 0xffff0000UL) | si);
}

void iplay_u16tox(IplayRegs *r, db *mem) {
    dd saved = abi_eax(r);
    dd esi = abi_esi(r);
    dw si = (dw)esi;
    db al = write_hex_word(mem, &si, (dw)saved);
    apply_eax_esi_regs(r, (saved & 0xffff0000UL) | ((saved & 0xffu) << 8) | al,
                       (esi & 0xffff0000UL) | si);
}

void iplay_u32tox(IplayRegs *r, db *mem) {
    dd saved = abi_eax(r);
    dd esi = abi_esi(r);
    dw si = (dw)esi;
    db al = write_hex_dword(mem, &si, saved);
    apply_eax_esi_regs(r, (saved & 0xffff0000UL) | ((saved & 0xffu) << 8) | al,
                       (esi & 0xffff0000UL) | si);
}

void iplay_hex_1be39(IplayRegs *r, db *dst) {
    dd eax = abi_eax(r);
    dd edi = abi_edi(r);
    db al = hex_digit((db)eax);
    dw ax = (dw)((eax & 0xff00u) | al);
    dst[0] = (db)ax;
    dst[1] = (db)(ax >> 8);
    apply_eax_edi_regs(r,
                       (eax & 0xffff0000UL) | ax,
                       (edi & 0xffff0000UL) | (dw)((dw)edi + 2u));
}

void iplay_my_putdigit(IplayRegs *r, db *mem) {
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dw si = (dw)esi;
    dw cx = (dw)ecx;
    write_counted_char(mem, &si, &cx, (db)edx);
    apply_ecx_esi_regs(r, (ecx & 0xffff0000UL) | cx,
                       (esi & 0xffff0000UL) | si);
}

IplayDecimalResult iplay_u32_base_to_buffer(db *mem, dw *offset, dd value, unsigned base, dw initial_count) {
    IplayDecimalResult result;
    dw count = initial_count;
    db digit = write_u32_base(mem, offset, value, base, &count);
    result.magnitude = value;
    result.count = count;
    result.offset = *offset;
    result.last_digit = digit;
    return result;
}

void iplay_my_u32toa(IplayRegs *r, db *mem, unsigned base) {
    dd value = abi_eax(r);
    dw si = (dw)abi_esi(r);
    IplayDecimalResult result = iplay_u32_base_to_buffer(mem, &si, value, base, (dw)abi_ecx(r));
    apply_u32toa_regs(r, result.magnitude, result.count, result.offset, result.last_digit);
}

void iplay_my_u8toa_10(IplayRegs *r, db *mem) {
    dd value = abi_eax(r) & 0xffu;
    dw si = (dw)abi_esi(r);
    dw cx = 0;
    db digit = write_u32_base(mem, &si, value, 10, &cx);
    apply_u32toa_regs(r, value, cx, si, digit);
}

void iplay_my_u16toa_10(IplayRegs *r, db *mem) {
    dd value = abi_eax(r) & 0xffffu;
    dw si = (dw)abi_esi(r);
    dw cx = 0;
    db digit = write_u32_base(mem, &si, value, 10, &cx);
    apply_u32toa_regs(r, value, cx, si, digit);
}

void iplay_my_u32toa10(IplayRegs *r, db *mem) {
    dd value = abi_eax(r);
    dw si = (dw)abi_esi(r);
    dw cx = 0;
    db digit = write_u32_base(mem, &si, value, 10, &cx);
    apply_u32toa_regs(r, value, cx, si, digit);
}

void iplay_my_i8toa10(IplayRegs *r, db *mem) {
    int8_t value = (int8_t)(abi_eax(r) & 0xffu);
    dd magnitude;
    dw si = (dw)abi_esi(r);
    dw cx = 0;
    db digit;
    if (value < 0) {
        write_counted_char(mem, &si, &cx, '-');
        magnitude = (dd)(uint8_t)(-value);
    } else {
        magnitude = (dd)(uint8_t)value;
    }
    digit = write_u32_base(mem, &si, magnitude, 10, &cx);
    apply_u32toa_regs(r, magnitude, cx, si, digit);
}

void iplay_my_i16toa10(IplayRegs *r, db *mem) {
    int16_t value = (int16_t)(abi_eax(r) & 0xffffu);
    dd magnitude;
    dw si = (dw)abi_esi(r);
    dw cx = 0;
    db digit;
    if (value < 0) {
        write_counted_char(mem, &si, &cx, '-');
        magnitude = (dd)(uint16_t)(-value);
    } else {
        magnitude = (dd)(uint16_t)value;
    }
    digit = write_u32_base(mem, &si, magnitude, 10, &cx);
    apply_u32toa_regs(r, magnitude, cx, si, digit);
}

void iplay_my_i32toa10(IplayRegs *r, db *mem) {
    dd eax = abi_eax(r);
    int32_t value = (int32_t)eax;
    dd magnitude = eax;
    dw si = (dw)abi_esi(r);
    dw cx = 0;
    db digit;
    if (value < 0) {
        write_counted_char(mem, &si, &cx, '-');
        magnitude = (dd)(-(value + 1)) + 1u;
    }
    digit = write_u32_base(mem, &si, magnitude, 10, &cx);
    apply_u32toa_regs(r, magnitude, cx, si, digit);
}

dw iplay_u32_decimal_fill_to_buffer(db *mem, dw *offset, dd value, dw count, int with_pointer_prefix) {
    db tmp[16];
    dw tmp_off = 0;
    dw digits = 0;
    dw dst = *offset;
    dw copy;
    dw spaces;
    dw i;
    write_u32_base(tmp, &tmp_off, value, 10, &digits);
    copy = digits < count ? digits : count;
    spaces = (dw)(count - copy);
    if (with_pointer_prefix) {
        mem[dst] = 0x02;
        mem[(dw)(dst + 1u)] = 0x7f;
        dst = (dw)(dst + 2u);
    }
    for (i = 0; i < spaces; ++i) {
        mem[dst] = ' ';
        dst = (dw)(dst + 1u);
    }
    for (i = 0; i < copy; ++i) {
        mem[dst] = tmp[(dw)(digits - copy + i)];
        dst = (dw)(dst + 1u);
    }
    *offset = dst;
    return dst;
}

void iplay_my_u32toa_fill(IplayRegs *r, db *mem, dw count, int with_pointer_prefix) {
    dd eax = abi_eax(r);
    dw di = (dw)abi_edi(r);
    iplay_u32_decimal_fill_to_buffer(mem, &di, eax, count, with_pointer_prefix);
    apply_full_regs6(r, eax, abi_ebx(r), 0, 0, 0, di);
}

IplayAsmSprintfResult iplay_myasmsprintf_to_buffer(db *mem, dw src_offset, dw dst_offset, dd eax, dd ecx, dd edx) {
    IplayAsmSprintfResult result;
    dw si = src_offset;
    dw di = dst_offset;
    for (;;) {
        db al = mem[si++];
        db code;
        dw ptr;
        dw fmt_off;
        dw fmt_count;
        dd fmt_value;
        db fmt_digit;
        if (al >= 0x20u) {
            mem[di++] = al;
            eax = (eax & 0xffffff00UL) | al;
            continue;
        }
        if (al > 0x0cu) break;
        ++si; /* format-character byte; original dispatch ignores it */
        code = al;
        if (code == 0) break;
        ptr = (dw)mem[si] | ((dw)mem[(dw)(si + 1u)] << 8);
        si = (dw)(si + 2u);
        fmt_off = di;
        fmt_count = 0;
        switch (code) {
        case 4:
            fmt_value = mem[ptr];
            fmt_digit = write_u32_base(mem, &fmt_off, fmt_value, 10, &fmt_count);
            di = fmt_off;
            eax = fmt_value;
            ecx = fmt_count;
            edx = (edx & 0xffffff00UL) | fmt_digit;
            break;
        case 5:
            fmt_value = get_word(mem, ptr);
            fmt_digit = write_u32_base(mem, &fmt_off, fmt_value, 10, &fmt_count);
            di = fmt_off;
            eax = fmt_value;
            ecx = fmt_count;
            edx = (edx & 0xffffff00UL) | fmt_digit;
            break;
        case 6:
            fmt_value = (dd)get_word(mem, ptr) | ((dd)get_word(mem, (unsigned)(ptr + 2u)) << 16);
            fmt_digit = write_u32_base(mem, &fmt_off, fmt_value, 10, &fmt_count);
            di = fmt_off;
            eax = fmt_value;
            ecx = fmt_count;
            edx = (edx & 0xffffff00UL) | fmt_digit;
            break;
        case 7:
            fmt_digit = write_i32_decimal(mem, &fmt_off, (int8_t)mem[ptr], &fmt_count, &fmt_value);
            di = fmt_off;
            eax = fmt_value;
            ecx = fmt_count;
            edx = (edx & 0xffffff00UL) | fmt_digit;
            break;
        case 8:
            fmt_digit = write_i32_decimal(mem, &fmt_off, (int16_t)get_word(mem, ptr), &fmt_count, &fmt_value);
            di = fmt_off;
            eax = fmt_value;
            ecx = fmt_count;
            edx = (edx & 0xffffff00UL) | fmt_digit;
            break;
        case 9:
            fmt_digit = write_i32_decimal(mem, &fmt_off, (int32_t)((dd)get_word(mem, ptr) | ((dd)get_word(mem, (unsigned)(ptr + 2u)) << 16)), &fmt_count, &fmt_value);
            di = fmt_off;
            eax = fmt_value;
            ecx = fmt_count;
            edx = (edx & 0xffffff00UL) | fmt_digit;
            break;
        case 10:
            fmt_value = mem[ptr];
            fmt_digit = write_hex_byte(mem, &fmt_off, (db)fmt_value);
            di = fmt_off;
            eax = fmt_digit;
            break;
        case 11:
            fmt_value = get_word(mem, ptr);
            fmt_digit = write_hex_word(mem, &fmt_off, (dw)fmt_value);
            di = fmt_off;
            eax = ((fmt_value & 0xffu) << 8) | fmt_digit;
            break;
        case 12:
            fmt_value = (dd)get_word(mem, ptr) | ((dd)get_word(mem, (unsigned)(ptr + 2u)) << 16);
            fmt_digit = write_hex_dword(mem, &fmt_off, fmt_value);
            di = fmt_off;
            eax = (fmt_value & 0xffff0000UL) | ((fmt_value & 0xffu) << 8) | fmt_digit;
            break;
        default:
            break;
        }
    }
    result.src_offset = si;
    result.dst_offset = di;
    result.eax = eax;
    result.ecx = ecx;
    result.edx = edx;
    return result;
}

void iplay_myasmsprintf(IplayRegs *r, db *mem) {
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    IplayAsmSprintfResult result = iplay_myasmsprintf_to_buffer(mem, (dw)esi, (dw)edi, eax, ecx, edx);
    apply_full_regs6(r, result.eax, ebx, result.ecx, result.edx,
                     (esi & 0xffff0000UL) | result.src_offset,
                     (edi & 0xffff0000UL) | result.dst_offset);
}

void iplay_interpolation_patch(db *code, db value) {
    static const dw offsets[] = {
        0x58b4, 0x58e3, 0x5912, 0x5941, 0x5970, 0x599f, 0x59ce, 0x59fd,
        0x5a2c, 0x5a5b, 0x5a8a, 0x5ab9, 0x5ae8, 0x5b17, 0x5b46, 0x5b81,
        0x5bad, 0x5bda, 0x5c07, 0x5c34, 0x5c61, 0x5c8e, 0x5cbb, 0x5ce8,
        0x5d15, 0x5d42, 0x5d6f, 0x5d9c, 0x5dc9, 0x5df6, 0x5e23
    };
    unsigned i;
    for (i = 0; i < sizeof(offsets) / sizeof(offsets[0]); ++i) {
        code[offsets[i]] = value;
    }
}

void iplay_mystrlen(IplayRegs *r, const db *mem) {
    dd eax = abi_eax(r);
    dw start = (dw)abi_esi(r);
    dw len = iplay_string_length_at(mem, start);
    apply_eax_esi_regs(r, (eax & 0xffff0000UL) | len, start);
}

void iplay_strcpy_count(IplayRegs *r, const db *src_mem, db *dst_mem) {
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    IplayStringCopyResult result = iplay_strcpy_count_to_buffer(src_mem, dst_mem, (dw)esi, (dw)edi);
    apply_full_regs6(r, (eax & 0xffffff00UL) | result.last_byte, ebx,
                     (ecx & 0xffff0000UL) | result.count, edx,
                     (esi & 0xffff0000UL) | result.src_offset,
                     (edi & 0xffff0000UL) | result.dst_offset);
}

void iplay_copy_printable(IplayRegs *r, const db *src_mem, db *dst_mem) {
    IplayStringCopyResult result;
    dd eax = abi_eax(r);
    result = iplay_copy_printable_to_buffer(src_mem, dst_mem, (dw)abi_esi(r), (dw)abi_edi(r), (dw)abi_ecx(r));
    if (result.copied_any) {
        eax = (eax & 0xffffff00UL) | result.last_byte;
    }
    apply_eax_reg(r, eax);
    /* Original preserves SI/DI/CX via push/pop. */
}

void iplay_seg1_copy_printable(IplayRegs *r, const db *src_mem, db *dst_mem) {
    IplayStringCopyResult result;
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    result = iplay_copy_printable_padded_to_buffer(src_mem, dst_mem, (dw)esi, (dw)edi, (dw)ecx);
    if (result.copied_any) {
        eax = (eax & 0xffffff00UL) | result.last_byte;
    }
    apply_full_regs6(r, eax, ebx, ecx & 0xffff0000UL,
                     edx, esi, edi);
}

void iplay_txt_1abae(IplayRegs *r, const db *src_mem, db *dst_mem) {
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    IplayAttributedTextResult result = iplay_copy_attributed_fixed_to_buffer(src_mem, dst_mem, (dw)esi, (dw)edi, 0x16u, 0x7b);
    apply_full_regs6(r, (eax & 0xffff0000UL) | result.ax,
                     ebx, ecx & 0xffff0000UL, edx,
                     (esi & 0xffff0000UL) | result.src_offset,
                     (edi & 0xffff0000UL) | result.dst_offset);
}

void iplay_put_message(IplayRegs *r, const db *src_mem, db *dst_mem, int initial_ax) {
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    db ah = (db)(eax >> 8);
    IplayAttributedTextResult result = iplay_put_attributed_message_to_buffer(src_mem, dst_mem, (dw)esi, (dw)edi, ah, initial_ax, (db)eax);
    apply_full_regs6(r, (eax & 0xffff0000UL) | result.ax,
                     ebx, ecx, edx,
                     (esi & 0xffff0000UL) | result.src_offset,
                     (edi & 0xffff0000UL) | result.dst_offset);
}

void iplay_text_1bf69(IplayRegs *r, const db *src_mem, db *dst_mem) {
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    db ah = (db)(eax >> 8);
    IplayAttributedTextResult result = iplay_put_controlled_attributed_text_to_buffer(src_mem, dst_mem, (dw)esi, (dw)edi, ah);
    apply_full_regs6(r, (eax & 0xffff0000UL) | result.ax,
                     ebx, ecx, edx,
                     (esi & 0xffff0000UL) | result.src_offset,
                     (edi & 0xffff0000UL) | result.dst_offset);
}

void iplay_ncplane_init(IplayNcPlane *plane, db *cells, dw rows, dw cols) {
    iplay_ncplane_init_at(plane, cells, rows, cols, 0, 0, cols);
}

void iplay_ncplane_init_mode(IplayNcPlane *plane, db *cells, const IplayTextMode *mode) {
    iplay_ncplane_init(plane, cells, iplay_text_mode_rows(mode), iplay_text_mode_cols(mode));
}

void iplay_text_screen_init(IplayTextScreen *screen, db *cells, const IplayTextMode *mode) {
    iplay_text_screen_init_capacity(screen, cells, iplay_text_mode_screen_bytes(mode), mode);
}

#define iplay_text_screen_set_cells_field(state, value) ((state)->cells = (value))

#define iplay_text_screen_set_capacity_field(state, value) ((state)->capacity_bytes = (value))

#define iplay_text_screen_set_mode_field(state, value) ((state)->mode = *(value))

#define iplay_text_screen_capacity_field(state) ((state)->capacity_bytes)
#define iplay_text_screen_cells_field(state) ((state)->cells)
#define iplay_text_screen_cells_const_field(state) ((state)->cells)
#define iplay_text_screen_root_field(state) (&(state)->root)
#define iplay_text_screen_mode_field(state) (&(state)->mode)

void iplay_text_screen_init_capacity(IplayTextScreen *screen, db *cells, dw capacity_bytes, const IplayTextMode *mode) {
    iplay_text_screen_set_cells(screen, cells);
    iplay_text_screen_set_capacity(screen, capacity_bytes);
    iplay_text_screen_set_mode(screen, mode);
    iplay_text_screen_reinit_root(screen);
}

void iplay_text_screen_set_cells(IplayTextScreen *screen, db *cells) {
    iplay_text_screen_set_cells_field(screen, cells);
}

void iplay_text_screen_set_capacity(IplayTextScreen *screen, dw capacity_bytes) {
    iplay_text_screen_set_capacity_field(screen, capacity_bytes);
}

void iplay_text_screen_set_mode(IplayTextScreen *screen, const IplayTextMode *mode) {
    iplay_text_screen_set_mode_field(screen, mode);
}

void iplay_text_screen_reinit_root(IplayTextScreen *screen) {
    iplay_ncplane_init_mode(iplay_text_screen_root(screen), iplay_text_screen_cells(screen), iplay_text_screen_mode(screen));
}

void iplay_text_screen_resize(IplayTextScreen *screen, const IplayTextMode *mode) {
    (void)iplay_text_screen_resize_checked(screen, mode);
}

int iplay_text_screen_resize_checked(IplayTextScreen *screen, const IplayTextMode *mode) {
    if (!iplay_text_screen_can_resize(screen, mode)) return 0;
    iplay_text_screen_set_mode(screen, mode);
    iplay_text_screen_reinit_root(screen);
    return 1;
}

void iplay_text_screen_resize_to_size(IplayTextScreen *screen, dw cols, dw rows) {
    (void)iplay_text_screen_resize_to_size_checked(screen, cols, rows);
}

int iplay_text_screen_resize_to_size_checked(IplayTextScreen *screen, dw cols, dw rows) {
    const IplayTextMode *mode = iplay_text_mode_for_size(cols, rows);
    return iplay_text_screen_resize_checked(screen, mode);
}

int iplay_text_screen_can_resize(const IplayTextScreen *screen, const IplayTextMode *mode) {
    return iplay_text_mode_fits_capacity(mode, iplay_text_screen_capacity(screen));
}

const IplayTextMode *iplay_text_screen_set_video_mode(IplayTextScreen *screen, db video_mode) {
    iplay_text_screen_set_video_mode_checked(screen, video_mode);
    return iplay_text_screen_mode(screen);
}

int iplay_text_screen_set_video_mode_checked(IplayTextScreen *screen, db video_mode) {
    const IplayTextMode *mode = iplay_text_mode_for_video_mode(video_mode);
    return iplay_text_screen_resize_checked(screen, mode);
}

dw iplay_text_screen_capacity(const IplayTextScreen *screen) {
    return iplay_text_screen_capacity_field(screen);
}

db *iplay_text_screen_cells(IplayTextScreen *screen) {
    return iplay_text_screen_cells_field(screen);
}

const db *iplay_text_screen_cells_const(const IplayTextScreen *screen) {
    return iplay_text_screen_cells_const_field(screen);
}

dd iplay_text_cells_checksum(const db *cells, dw byte_count) {
    dd checksum = 2166136261ul;
    dw i;
    if (!cells) return 0;
    for (i = 0; i < byte_count; ++i) {
        checksum ^= (dd)cells[i];
        checksum *= 16777619ul;
    }
    return checksum;
}

dw iplay_text_cells_nonblank_count(const db *cells, dw byte_count) {
    dw count = 0;
    dw i;
    if (!cells) return 0;
    for (i = 0; (dw)(i + 1u) < byte_count; i = (dw)(i + 2u)) {
        if (cells[i] != 0 && cells[i] != ' ') ++count;
    }
    return count;
}

dd iplay_text_screen_checksum(const IplayTextScreen *screen) {
    return iplay_text_cells_checksum(iplay_text_screen_cells_const(screen), iplay_text_screen_bytes(screen));
}

dw iplay_text_screen_nonblank_count(const IplayTextScreen *screen) {
    return iplay_text_cells_nonblank_count(iplay_text_screen_cells_const(screen), iplay_text_screen_bytes(screen));
}

dw iplay_text_screen_bytes(const IplayTextScreen *screen) {
    return iplay_text_mode_screen_bytes(iplay_text_screen_mode(screen));
}

IplayNcPlane *iplay_text_screen_root(IplayTextScreen *screen) {
    return iplay_text_screen_root_field(screen);
}

const IplayTextMode *iplay_text_screen_mode(const IplayTextScreen *screen) {
    return iplay_text_screen_mode_field(screen);
}

const IplayBottomLayout *iplay_text_screen_bottom_layout(const IplayTextScreen *screen) {
    return iplay_bottom_layout_for_mode(iplay_text_screen_mode(screen));
}

int iplay_text_screen_bottom_layout_fits(const IplayTextScreen *screen) {
    return iplay_bottom_layout_fits(iplay_text_screen_bottom_layout(screen), iplay_text_screen_mode(screen));
}

void iplay_terminal_init_vga_memory(IplayTerminal *terminal, db *cells, const IplayTextMode *mode) {
    iplay_terminal_init_vga_memory_capacity(terminal, cells, iplay_text_mode_screen_bytes(mode), mode);
}

void iplay_terminal_init_vga_memory_capacity(IplayTerminal *terminal, db *cells, dw capacity_bytes, const IplayTextMode *mode) {
    iplay_text_screen_init_capacity(iplay_terminal_screen(terminal), cells, capacity_bytes, mode);
    iplay_terminal_set_backend(terminal, IPLAY_TERMINAL_BACKEND_VGA_MEMORY);
    iplay_terminal_clear_present_callback(terminal);
}

#define iplay_terminal_set_backend_field(state, value) ((state)->backend = (value))
#define iplay_terminal_set_present_field(state, value) ((state)->present = (value))
#define iplay_terminal_set_present_user_field(state, value) ((state)->present_user = (value))

#define iplay_terminal_backend_field(state) ((state)->backend)
#define iplay_terminal_screen_field(state) (&(state)->screen)
#define iplay_terminal_screen_const_field(state) (&(state)->screen)
#define iplay_terminal_present_field(state) ((state)->present)
#define iplay_terminal_present_user_field(state) ((state)->present_user)

void iplay_terminal_set_backend(IplayTerminal *terminal, IplayTerminalBackend backend) {
    iplay_terminal_set_backend_field(terminal, backend);
}

void iplay_terminal_set_present_fn(IplayTerminal *terminal, IplayVideoPresentFn present) {
    iplay_terminal_set_present_field(terminal, present);
}

void iplay_terminal_set_present_user(IplayTerminal *terminal, void *user) {
    iplay_terminal_set_present_user_field(terminal, user);
}

void iplay_terminal_set_present_callback(IplayTerminal *terminal, IplayVideoPresentFn present, void *user) {
    iplay_terminal_set_present_fn(terminal, present);
    iplay_terminal_set_present_user(terminal, user);
}

void iplay_terminal_clear_present_callback(IplayTerminal *terminal) {
    iplay_terminal_set_present_callback(terminal, 0, 0);
}

IplayTerminalBackend iplay_terminal_backend(const IplayTerminal *terminal) {
    return iplay_terminal_backend_field(terminal);
}

int iplay_terminal_has_present(const IplayTerminal *terminal) {
    return iplay_terminal_present_callback(terminal) != 0;
}

IplayTextScreen *iplay_terminal_screen(IplayTerminal *terminal) {
    return iplay_terminal_screen_field(terminal);
}

const IplayTextScreen *iplay_terminal_screen_const(const IplayTerminal *terminal) {
    return iplay_terminal_screen_const_field(terminal);
}

db *iplay_terminal_cells(IplayTerminal *terminal) {
    return iplay_text_screen_cells(iplay_terminal_screen(terminal));
}

const db *iplay_terminal_cells_const(const IplayTerminal *terminal) {
    return iplay_text_screen_cells_const(iplay_terminal_screen_const(terminal));
}

IplayVideoPresentFn iplay_terminal_present_callback(const IplayTerminal *terminal) {
    return iplay_terminal_present_field(terminal);
}

void *iplay_terminal_present_user(const IplayTerminal *terminal) {
    return iplay_terminal_present_user_field(terminal);
}

dw iplay_terminal_capacity(const IplayTerminal *terminal) {
    return iplay_text_screen_capacity(iplay_terminal_screen_const(terminal));
}

int iplay_terminal_bottom_layout_fits(const IplayTerminal *terminal) {
    return iplay_text_screen_bottom_layout_fits(iplay_terminal_screen_const(terminal));
}

IplayNcPlane *iplay_terminal_root(IplayTerminal *terminal) {
    return iplay_text_screen_root(iplay_terminal_screen(terminal));
}

const IplayTextMode *iplay_terminal_mode(const IplayTerminal *terminal) {
    return iplay_text_screen_mode(iplay_terminal_screen_const(terminal));
}

const IplayTextMode *iplay_terminal_resize(IplayTerminal *terminal, const IplayTextMode *mode) {
    (void)iplay_terminal_resize_checked(terminal, mode);
    return iplay_terminal_mode(terminal);
}

int iplay_terminal_resize_checked(IplayTerminal *terminal, const IplayTextMode *mode) {
    return iplay_text_screen_resize_checked(iplay_terminal_screen(terminal), mode);
}

const IplayTextMode *iplay_terminal_resize_to_size(IplayTerminal *terminal, dw cols, dw rows) {
    (void)iplay_terminal_resize_to_size_checked(terminal, cols, rows);
    return iplay_terminal_mode(terminal);
}

int iplay_terminal_resize_to_size_checked(IplayTerminal *terminal, dw cols, dw rows) {
    const IplayTextMode *mode = iplay_text_mode_for_size(cols, rows);
    return iplay_terminal_resize_checked(terminal, mode);
}

const IplayTextMode *iplay_terminal_set_video_mode(IplayTerminal *terminal, db video_mode) {
    return iplay_text_screen_set_video_mode(iplay_terminal_screen(terminal), video_mode);
}

int iplay_terminal_set_video_mode_checked(IplayTerminal *terminal, db video_mode) {
    return iplay_text_screen_set_video_mode_checked(iplay_terminal_screen(terminal), video_mode);
}

dw iplay_terminal_present(IplayTerminal *terminal) {
    IplayVideoPresentFn present = iplay_terminal_present_callback(terminal);
    const IplayTextMode *mode = iplay_terminal_mode(terminal);
    dw bytes = iplay_text_screen_bytes(iplay_terminal_screen_const(terminal));
    if (present) {
        present(iplay_terminal_present_user(terminal), iplay_terminal_cells_const(terminal), mode, bytes);
    }
    return bytes;
}

void iplay_terminal_erase(IplayTerminal *terminal, db attr) {
    iplay_ncplane_erase(iplay_terminal_root(terminal), attr);
}

void iplay_terminal_draw_top_title(IplayTerminal *terminal) {
    iplay_text_screen_draw_top_title(iplay_terminal_screen(terminal));
}

void iplay_terminal_draw_bottom(IplayTerminal *terminal, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif) {
    iplay_text_screen_draw_bottom(iplay_terminal_screen(terminal), byte_1de72, byte_1de73, byte_1de74, byte_1de75, byte_1de76, flags, volume, amplif);
}

void iplay_terminal_draw_audio_output_levels(IplayTerminal *terminal, dw y, dw x, const IplayAudioOutput *output, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr) {
    iplay_text_screen_draw_audio_output_levels(iplay_terminal_screen(terminal), y, x, output, width, fill_ch, empty_ch, left_attr, right_attr, empty_attr);
}

void iplay_notcurses_init_vga_memory(IplayNotcurses *nc, db *cells, const IplayTextMode *mode) {
    iplay_terminal_init_vga_memory(iplay_notcurses_terminal(nc), cells, mode);
}

void iplay_notcurses_init_vga_memory_capacity(IplayNotcurses *nc, db *cells, dw capacity_bytes, const IplayTextMode *mode) {
    iplay_terminal_init_vga_memory_capacity(iplay_notcurses_terminal(nc), cells, capacity_bytes, mode);
}

#define iplay_notcurses_terminal_field(state) (&(state)->terminal)
#define iplay_notcurses_terminal_const_field(state) (&(state)->terminal)

IplayTerminal *iplay_notcurses_terminal(IplayNotcurses *nc) {
    return iplay_notcurses_terminal_field(nc);
}

const IplayTerminal *iplay_notcurses_terminal_const(const IplayNotcurses *nc) {
    return iplay_notcurses_terminal_const_field(nc);
}

IplayNcPlane *iplay_notcurses_stdplane(IplayNotcurses *nc) {
    return iplay_terminal_root(iplay_notcurses_terminal(nc));
}

const IplayTextMode *iplay_notcurses_mode(const IplayNotcurses *nc) {
    return iplay_terminal_mode(iplay_notcurses_terminal_const(nc));
}

dw iplay_notcurses_capacity(const IplayNotcurses *nc) {
    return iplay_terminal_capacity(iplay_notcurses_terminal_const(nc));
}

dw iplay_notcurses_cols(const IplayNotcurses *nc) {
    return iplay_text_mode_cols(iplay_notcurses_mode(nc));
}

dw iplay_notcurses_rows(const IplayNotcurses *nc) {
    return iplay_text_mode_rows(iplay_notcurses_mode(nc));
}

dw iplay_notcurses_row_bytes(const IplayNotcurses *nc) {
    return iplay_text_mode_row_bytes(iplay_notcurses_mode(nc));
}

dw iplay_notcurses_screen_bytes(const IplayNotcurses *nc) {
    return iplay_text_mode_screen_bytes(iplay_notcurses_mode(nc));
}

int iplay_notcurses_bottom_layout_fits(const IplayNotcurses *nc) {
    return iplay_terminal_bottom_layout_fits(iplay_notcurses_terminal_const(nc));
}

#define iplay_video_spec_backend_field(state) ((state)->backend)

#define iplay_video_spec_set_backend_field(state, value) ((state)->backend = (value))

#define iplay_video_spec_mode_field(state) (&(state)->mode)

#define iplay_video_spec_set_mode_field(state, value) ((state)->mode = *(value))

#define iplay_video_spec_present_enabled_field(state) ((state)->present_enabled)

#define iplay_video_spec_set_present_enabled_field(state, value) ((state)->present_enabled = (value))

IplayTerminalBackend iplay_video_spec_backend(const IplayVideoSpec *spec) {
    return iplay_video_spec_backend_field(spec);
}

const IplayTextMode *iplay_video_spec_mode(const IplayVideoSpec *spec) {
    return iplay_video_spec_mode_field(spec);
}

dw iplay_video_spec_cols(const IplayVideoSpec *spec) {
    return iplay_text_mode_cols(iplay_video_spec_mode(spec));
}

dw iplay_video_spec_rows(const IplayVideoSpec *spec) {
    return iplay_text_mode_rows(iplay_video_spec_mode(spec));
}

int iplay_video_spec_present_enabled(const IplayVideoSpec *spec) {
    return iplay_video_spec_present_enabled_field(spec) != 0;
}

IplayVideoSpec iplay_notcurses_video_spec(const IplayNotcurses *nc) {
    IplayVideoSpec spec;
    const IplayTerminal *terminal = iplay_notcurses_terminal_const(nc);
    iplay_video_spec_set_backend_field(&spec, iplay_terminal_backend(terminal));
    iplay_video_spec_set_mode_field(&spec, iplay_terminal_mode(terminal));
    iplay_video_spec_set_present_enabled_field(&spec, (db)iplay_terminal_has_present(terminal));
    return spec;
}

IplayTerminalBackend iplay_notcurses_backend(const IplayNotcurses *nc) {
    IplayVideoSpec spec = iplay_notcurses_video_spec(nc);
    return iplay_video_spec_backend(&spec);
}

int iplay_notcurses_present_enabled(const IplayNotcurses *nc) {
    IplayVideoSpec spec = iplay_notcurses_video_spec(nc);
    return iplay_video_spec_present_enabled(&spec);
}

int iplay_notcurses_has_present(const IplayNotcurses *nc) {
    return iplay_terminal_has_present(iplay_notcurses_terminal_const(nc));
}

IplayVideoPresentFn iplay_notcurses_present_callback(const IplayNotcurses *nc) {
    return iplay_terminal_present_callback(iplay_notcurses_terminal_const(nc));
}

void *iplay_notcurses_present_user(const IplayNotcurses *nc) {
    return iplay_terminal_present_user(iplay_notcurses_terminal_const(nc));
}

void iplay_notcurses_set_present_fn(IplayNotcurses *nc, IplayVideoPresentFn present) {
    iplay_terminal_set_present_fn(iplay_notcurses_terminal(nc), present);
}

void iplay_notcurses_set_present_user(IplayNotcurses *nc, void *user) {
    iplay_terminal_set_present_user(iplay_notcurses_terminal(nc), user);
}

void iplay_notcurses_set_present_callback(IplayNotcurses *nc, IplayVideoPresentFn present, void *user) {
    iplay_notcurses_set_present_fn(nc, present);
    iplay_notcurses_set_present_user(nc, user);
}

void iplay_notcurses_clear_present_callback(IplayNotcurses *nc) {
    iplay_notcurses_set_present_callback(nc, 0, 0);
}

const IplayTextMode *iplay_notcurses_resize(IplayNotcurses *nc, const IplayTextMode *mode) {
    (void)iplay_notcurses_resize_checked(nc, mode);
    return iplay_notcurses_mode(nc);
}

int iplay_notcurses_resize_checked(IplayNotcurses *nc, const IplayTextMode *mode) {
    return iplay_terminal_resize_checked(iplay_notcurses_terminal(nc), mode);
}

const IplayTextMode *iplay_notcurses_resize_to_size(IplayNotcurses *nc, dw cols, dw rows) {
    (void)iplay_notcurses_resize_to_size_checked(nc, cols, rows);
    return iplay_notcurses_mode(nc);
}

int iplay_notcurses_resize_to_size_checked(IplayNotcurses *nc, dw cols, dw rows) {
    const IplayTextMode *mode = iplay_text_mode_for_size(cols, rows);
    return iplay_notcurses_resize_checked(nc, mode);
}

const IplayTextMode *iplay_notcurses_set_video_mode(IplayNotcurses *nc, db video_mode) {
    return iplay_terminal_set_video_mode(iplay_notcurses_terminal(nc), video_mode);
}

int iplay_notcurses_set_video_mode_checked(IplayNotcurses *nc, db video_mode) {
    return iplay_terminal_set_video_mode_checked(iplay_notcurses_terminal(nc), video_mode);
}

void iplay_notcurses_render_static(IplayNotcurses *nc, db erase_attr) {
    iplay_terminal_erase(iplay_notcurses_terminal(nc), erase_attr);
    iplay_terminal_draw_top_title(iplay_notcurses_terminal(nc));
}

void iplay_notcurses_render_bottom(IplayNotcurses *nc, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif) {
    iplay_terminal_draw_bottom(iplay_notcurses_terminal(nc), byte_1de72, byte_1de73, byte_1de74, byte_1de75, byte_1de76, flags, volume, amplif);
}

void iplay_notcurses_draw_audio_output_levels(IplayNotcurses *nc, dw y, dw x, const IplayAudioOutput *output, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr) {
    iplay_terminal_draw_audio_output_levels(iplay_notcurses_terminal(nc), y, x, output, width, fill_ch, empty_ch, left_attr, right_attr, empty_attr);
}

dw iplay_notcurses_present(IplayNotcurses *nc) {
    return iplay_terminal_present(iplay_notcurses_terminal(nc));
}

const IplayTextMode *iplay_text_mode_for_video_mode(db mode) {
    switch (mode & 0x7fu) {
    case IPLAY_VIDEO_MODE_40X25_BW:
    case IPLAY_VIDEO_MODE_40X25_COLOR:
        return &IPLAY_TEXT_MODE_40X25;
    case IPLAY_VIDEO_MODE_80X25_BW:
    case IPLAY_VIDEO_MODE_80X25_COLOR:
        return &IPLAY_TEXT_MODE_80X25;
    case IPLAY_VIDEO_MODE_80X28_PROJECT:
        return &IPLAY_TEXT_MODE_80X28;
    case IPLAY_VIDEO_MODE_80X50_PROJECT:
        return &IPLAY_TEXT_MODE_80X50;
    default:
        return &IPLAY_TEXT_DEFAULT_MODE;
    }
}

const IplayTextMode *iplay_text_mode_for_size(dw cols, dw rows) {
    dw i;
    for (i = 0; i < IPLAY_TEXT_SUPPORTED_MODE_COUNT; ++i) {
        const IplayTextMode *mode = iplay_text_supported_mode(i);
        if (iplay_text_mode_cols(mode) == cols && iplay_text_mode_rows(mode) == rows) return mode;
    }
    return 0;
}

const IplayTextMode *iplay_text_current_mode(void) {
    return iplay_current_text_mode;
}

const IplayTextMode *iplay_set_current_text_video_mode(db video_mode) {
    iplay_current_text_mode = iplay_text_mode_for_video_mode(video_mode);
    return iplay_current_text_mode;
}

const IplayTextMode *iplay_text_default_mode(void) {
    return &IPLAY_TEXT_DEFAULT_MODE;
}

const IplayTextMode *iplay_text_fallback_mode(void) {
    return &IPLAY_TEXT_MODE_40X25;
}

const IplayTextMode *iplay_text_supported_mode(dw index) {
    if (index >= IPLAY_TEXT_SUPPORTED_MODE_COUNT) return 0;
    return iplay_supported_text_modes[index];
}

dw iplay_text_supported_mode_count(void) {
    return IPLAY_TEXT_SUPPORTED_MODE_COUNT;
}

int iplay_text_size_is_supported(dw cols, dw rows) {
    return iplay_text_mode_for_size(cols, rows) != 0;
}

int iplay_text_mode_is_supported(const IplayTextMode *mode) {
    return mode != 0 && iplay_text_size_is_supported(iplay_text_mode_cols(mode), iplay_text_mode_rows(mode));
}

#define iplay_text_mode_cols_field(state) ((state)->cols)
#define iplay_text_mode_rows_field(state) ((state)->rows)

dw iplay_text_mode_cols(const IplayTextMode *mode) {
    return iplay_text_mode_cols_field(mode);
}

dw iplay_text_mode_rows(const IplayTextMode *mode) {
    return iplay_text_mode_rows_field(mode);
}

dw iplay_text_mode_row_bytes(const IplayTextMode *mode) {
    return IPLAY_TEXT_ROW_BYTES(iplay_text_mode_cols(mode));
}

dw iplay_text_mode_cells(const IplayTextMode *mode) {
    return (dw)(iplay_text_mode_cols(mode) * iplay_text_mode_rows(mode));
}

dw iplay_text_mode_screen_bytes(const IplayTextMode *mode) {
    return (dw)(iplay_text_mode_cells(mode) * IPLAY_TEXT_CELL_BYTES);
}

int iplay_text_mode_fits_capacity(const IplayTextMode *mode, dw capacity_bytes) {
    return iplay_text_mode_is_supported(mode) && iplay_text_mode_screen_bytes(mode) <= capacity_bytes;
}

dw iplay_text_max_screen_bytes(void) {
    return IPLAY_TEXT_MAX_SCREEN_BYTES;
}

int iplay_text_mode_equals(const IplayTextMode *a, const IplayTextMode *b) {
    return iplay_text_mode_cols(a) == iplay_text_mode_cols(b) && iplay_text_mode_rows(a) == iplay_text_mode_rows(b);
}

db iplay_text_attr(IplayTextColor fg, IplayTextColor bg, int blink) {
    return (db)(((db)fg & 0x0fu) | (((db)bg & 0x07u) << 4) | (blink ? 0x80u : 0));
}

IplayTextColor iplay_text_attr_fg(db attr) {
    return (IplayTextColor)(attr & 0x0fu);
}

IplayTextColor iplay_text_attr_bg(db attr) {
    return (IplayTextColor)((attr >> 4) & 0x07u);
}

int iplay_text_attr_blink(db attr) {
    return (attr & 0x80u) != 0;
}

db *iplay_ncplane_cells_at(db *cells, dw stride_cols, dw origin_y, dw origin_x) {
    return cells + ((((dw)(origin_y * stride_cols)) + origin_x) * IPLAY_TEXT_CELL_BYTES);
}

void iplay_ncplane_init_at(IplayNcPlane *plane, db *cells, dw rows, dw cols, dw origin_y, dw origin_x, dw stride_cols) {
    iplay_ncplane_set_cells(plane, iplay_ncplane_cells_at(cells, stride_cols, origin_y, origin_x));
    iplay_ncplane_set_size(plane, rows, cols);
    iplay_ncplane_set_stride_cols(plane, stride_cols);
    iplay_ncplane_set_origin_yx(plane, origin_y, origin_x);
    iplay_ncplane_set_cursor_yx_raw(plane, 0, 0);
}

void iplay_ncplane_subplane(IplayNcPlane *child, const IplayNcPlane *parent, dw y, dw x, dw rows, dw cols) {
    dw max_rows = 0;
    dw max_cols = 0;
    dw parent_y = 0;
    dw parent_x = 0;
    if (y < iplay_ncplane_rows(parent)) max_rows = (dw)(iplay_ncplane_rows(parent) - y);
    if (x < iplay_ncplane_cols(parent)) max_cols = (dw)(iplay_ncplane_cols(parent) - x);
    if (rows > max_rows) rows = max_rows;
    if (cols > max_cols) cols = max_cols;
    iplay_ncplane_init_at(child, iplay_ncplane_cells(parent), rows, cols, y, x, iplay_ncplane_stride_cols(parent));
    iplay_ncplane_origin_yx(parent, &parent_y, &parent_x);
    iplay_ncplane_set_origin_yx(child, (dw)(parent_y + y), (dw)(parent_x + x));
}

void iplay_ncplane_resize(IplayNcPlane *plane, dw rows, dw cols) {
    dw cursor_y = iplay_ncplane_cursor_y(plane);
    dw cursor_x = iplay_ncplane_cursor_x(plane);
    iplay_ncplane_set_size(plane, rows, cols);
    if (cursor_y >= rows) cursor_y = rows ? (dw)(rows - 1u) : 0;
    if (cursor_x >= cols) cursor_x = cols ? (dw)(cols - 1u) : 0;
    iplay_ncplane_set_cursor_yx_raw(plane, cursor_y, cursor_x);
}

#define iplay_ncplane_origin_y_field(state) ((state)->origin_y)
#define iplay_ncplane_origin_x_field(state) ((state)->origin_x)
#define iplay_ncplane_rows_field(state) ((state)->rows)
#define iplay_ncplane_cols_field(state) ((state)->cols)
#define iplay_ncplane_stride_cols_field(state) ((state)->stride_cols)
#define iplay_ncplane_cells_field(state) ((state)->cells)

#define iplay_ncplane_set_cells_field(state, value) ((state)->cells = (value))

#define iplay_ncplane_set_size_field(state, value_rows, value_cols) ((state)->rows = (value_rows), (state)->cols = (value_cols))

#define iplay_ncplane_set_stride_cols_field(state, value) ((state)->stride_cols = (value))

#define iplay_ncplane_set_origin_yx_field(state, value_y, value_x) ((state)->origin_y = (value_y), (state)->origin_x = (value_x))

void iplay_ncplane_origin_yx(const IplayNcPlane *plane, dw *y, dw *x) {
    *y = iplay_ncplane_origin_y_field(plane);
    *x = iplay_ncplane_origin_x_field(plane);
}

dw iplay_ncplane_rows(const IplayNcPlane *plane) {
    return iplay_ncplane_rows_field(plane);
}

dw iplay_ncplane_cols(const IplayNcPlane *plane) {
    return iplay_ncplane_cols_field(plane);
}

dw iplay_ncplane_stride_cols(const IplayNcPlane *plane) {
    return iplay_ncplane_stride_cols_field(plane);
}

db *iplay_ncplane_cells(const IplayNcPlane *plane) {
    return iplay_ncplane_cells_field(plane);
}

void iplay_ncplane_set_cells(IplayNcPlane *plane, db *cells) {
    iplay_ncplane_set_cells_field(plane, cells);
}

void iplay_ncplane_set_size(IplayNcPlane *plane, dw rows, dw cols) {
    iplay_ncplane_set_size_field(plane, rows, cols);
}

void iplay_ncplane_set_stride_cols(IplayNcPlane *plane, dw stride_cols) {
    iplay_ncplane_set_stride_cols_field(plane, stride_cols);
}

void iplay_ncplane_set_origin_yx(IplayNcPlane *plane, dw y, dw x) {
    iplay_ncplane_set_origin_yx_field(plane, y, x);
}

int iplay_ncplane_is_empty(const IplayNcPlane *plane) {
    return iplay_ncplane_rows(plane) == 0 || iplay_ncplane_cols(plane) == 0;
}

#define iplay_ncplane_cursor_y_field(state) ((state)->cursor_y)
#define iplay_ncplane_cursor_x_field(state) ((state)->cursor_x)

#define iplay_ncplane_set_cursor_y_field(state, value) ((state)->cursor_y = (value))
#define iplay_ncplane_set_cursor_x_field(state, value) ((state)->cursor_x = (value))
#define iplay_ncplane_advance_cursor_x_field(state) (++(state)->cursor_x)

#define iplay_ncplane_cell_byte_field(state, offset_value) ((state)->cells[(offset_value)])

#define iplay_ncplane_set_cell_byte_field(state, offset_value, value) ((state)->cells[(offset_value)] = (value))

dw iplay_ncplane_cursor_y(const IplayNcPlane *plane) {
    return iplay_ncplane_cursor_y_field(plane);
}

dw iplay_ncplane_cursor_x(const IplayNcPlane *plane) {
    return iplay_ncplane_cursor_x_field(plane);
}

void iplay_ncplane_set_cursor_yx_raw(IplayNcPlane *plane, dw y, dw x) {
    iplay_ncplane_set_cursor_y_field(plane, y);
    iplay_ncplane_set_cursor_x_field(plane, x);
}

void iplay_ncplane_advance_cursor_x(IplayNcPlane *plane) {
    if (iplay_ncplane_cursor_x(plane) < iplay_ncplane_cols(plane)) iplay_ncplane_advance_cursor_x_field(plane);
}

dw iplay_ncplane_cell_offset(const IplayNcPlane *plane, dw y, dw x) {
    return (dw)((((dw)y * iplay_ncplane_stride_cols(plane)) + x) * IPLAY_TEXT_CELL_BYTES);
}

db iplay_ncplane_cell_ch(const IplayNcPlane *plane, dw offset) {
    return iplay_ncplane_cell_byte_field(plane, offset);
}

db iplay_ncplane_cell_attr(const IplayNcPlane *plane, dw offset) {
    return iplay_ncplane_cell_byte_field(plane, (dw)(offset + 1u));
}

void iplay_ncplane_put_cell_offset(IplayNcPlane *plane, dw offset, db ch, db attr) {
    iplay_ncplane_set_cell_byte_field(plane, offset, ch);
    iplay_ncplane_set_cell_byte_field(plane, (dw)(offset + 1u), attr);
}

void iplay_ncplane_copy_cell_offset(IplayNcPlane *plane, dw dst_offset, dw src_offset) {
    iplay_ncplane_put_cell_offset(plane,
                                  dst_offset,
                                  iplay_ncplane_cell_ch(plane, src_offset),
                                  iplay_ncplane_cell_attr(plane, src_offset));
}

void iplay_ncplane_cursor_yx(const IplayNcPlane *plane, dw *y, dw *x) {
    *y = iplay_ncplane_cursor_y(plane);
    *x = iplay_ncplane_cursor_x(plane);
}

void iplay_ncplane_cursor_move_yx(IplayNcPlane *plane, dw y, dw x) {
    dw rows = iplay_ncplane_rows(plane);
    dw cols = iplay_ncplane_cols(plane);
    if (iplay_ncplane_is_empty(plane)) {
        iplay_ncplane_set_cursor_yx_raw(plane, 0, 0);
        return;
    }
    if (y >= rows) y = (dw)(rows - 1u);
    if (x >= cols) x = (dw)(cols - 1u);
    iplay_ncplane_set_cursor_yx_raw(plane, y, x);
}

int iplay_ncplane_visible_region(const IplayNcPlane *plane, dw y, dw x, dw rows, dw cols, dw *visible_rows, dw *visible_cols) {
    dw plane_rows = iplay_ncplane_rows(plane);
    dw plane_cols = iplay_ncplane_cols(plane);
    dw vr = 0;
    dw vc = 0;
    if (y < plane_rows) {
        vr = (dw)(plane_rows - y);
        if (vr > rows) vr = rows;
    }
    if (x < plane_cols) {
        vc = (dw)(plane_cols - x);
        if (vc > cols) vc = cols;
    }
    *visible_rows = vr;
    *visible_cols = vc;
    return vr != 0 && vc != 0;
}

void iplay_ncplane_putc_yx(IplayNcPlane *plane, dw y, dw x, db ch, db attr) {
    dw off;
    if (y >= iplay_ncplane_rows(plane) || x >= iplay_ncplane_cols(plane)) return;
    off = iplay_ncplane_cell_offset(plane, y, x);
    iplay_ncplane_put_cell_offset(plane, off, ch, attr);
}

void iplay_ncplane_putc(IplayNcPlane *plane, db ch, db attr) {
    iplay_ncplane_putc_yx(plane, iplay_ncplane_cursor_y(plane), iplay_ncplane_cursor_x(plane), ch, attr);
    iplay_ncplane_advance_cursor_x(plane);
}

void iplay_ncplane_hline_yx(IplayNcPlane *plane, dw y, dw x, db ch, db attr, dw count) {
    dw i;
    dw visible_rows;
    dw visible_cols;
    if (!iplay_ncplane_visible_region(plane, y, x, 1u, count, &visible_rows, &visible_cols)) return;
    (void)visible_rows;
    for (i = 0; i < visible_cols; ++i) {
        iplay_ncplane_putc_yx(plane, y, (dw)(x + i), ch, attr);
    }
}

void iplay_ncplane_vline_yx(IplayNcPlane *plane, dw y, dw x, db ch, db attr, dw count) {
    dw i;
    dw visible_rows;
    dw visible_cols;
    if (!iplay_ncplane_visible_region(plane, y, x, count, 1u, &visible_rows, &visible_cols)) return;
    (void)visible_cols;
    for (i = 0; i < visible_rows; ++i) {
        iplay_ncplane_putc_yx(plane, (dw)(y + i), x, ch, attr);
    }
}

void iplay_ncplane_meter16_yx(IplayNcPlane *plane, dw y, dw x, db level, dw width, db fill_ch, db empty_ch, db fill_attr, db empty_attr) {
    dw filled;
    dw i;
    if (level > 15u) level = 15u;
    filled = (dw)(((dw)level * width + 14u) / 15u);
    if (level == 0) filled = 0;
    for (i = 0; i < width; ++i) {
        if (i < filled) {
            iplay_ncplane_putc_yx(plane, y, (dw)(x + i), fill_ch, fill_attr);
        } else {
            iplay_ncplane_putc_yx(plane, y, (dw)(x + i), empty_ch, empty_attr);
        }
    }
}

#define iplay_audio_levels_left_16_field(state) ((state)->left_16)
#define iplay_audio_levels_right_16_field(state) ((state)->right_16)

db iplay_audio_levels_left_16(const IplayAudioLevels *levels) {
    return iplay_audio_levels_left_16_field(levels);
}

db iplay_audio_levels_right_16(const IplayAudioLevels *levels) {
    return iplay_audio_levels_right_16_field(levels);
}

void iplay_audio_levels_draw_yx(IplayNcPlane *plane, dw y, dw x, const IplayAudioLevels *levels, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr) {
    iplay_ncplane_meter16_yx(plane, y, x, iplay_audio_levels_left_16(levels), width, fill_ch, empty_ch, left_attr, empty_attr);
    iplay_ncplane_meter16_yx(plane, (dw)(y + 1u), x, iplay_audio_levels_right_16(levels), width, fill_ch, empty_ch, right_attr, empty_attr);
}

void iplay_ncplane_fill_yx(IplayNcPlane *plane, dw y, dw x, dw rows, dw cols, db ch, db attr) {
    dw row;
    dw visible_rows;
    dw visible_cols;
    if (!iplay_ncplane_visible_region(plane, y, x, rows, cols, &visible_rows, &visible_cols)) return;
    for (row = 0; row < visible_rows; ++row) {
        iplay_ncplane_hline_yx(plane, (dw)(y + row), x, ch, attr, visible_cols);
    }
}

void iplay_ncplane_erase(IplayNcPlane *plane, db attr) {
    iplay_ncplane_fill_yx(plane, 0, 0, iplay_ncplane_rows(plane), iplay_ncplane_cols(plane), ' ', attr);
}

void iplay_ncplane_box_yx(IplayNcPlane *plane, dw y, dw x, dw rows, dw cols, db attr, db fill_attr) {
    static const db style3[6] = {0xda, 0xbf, 0xc0, 0xd9, 0xc4, 0xb3};
    dw inner_cols;
    if (rows < 2u || cols < 2u) return;
    inner_cols = (dw)(cols - 2u);
    iplay_ncplane_putc_yx(plane, y, x, style3[0], attr);
    iplay_ncplane_hline_yx(plane, y, (dw)(x + 1u), style3[4], attr, inner_cols);
    iplay_ncplane_putc_yx(plane, y, (dw)(x + cols - 1u), style3[1], fill_attr);
    iplay_ncplane_vline_yx(plane, (dw)(y + 1u), x, style3[5], attr, (dw)(rows - 2u));
    iplay_ncplane_fill_yx(plane, (dw)(y + 1u), (dw)(x + 1u), (dw)(rows - 2u), inner_cols, ' ', attr);
    iplay_ncplane_vline_yx(plane, (dw)(y + 1u), (dw)(x + cols - 1u), style3[5], fill_attr, (dw)(rows - 2u));
    iplay_ncplane_putc_yx(plane, (dw)(y + rows - 1u), x, style3[2], attr);
    iplay_ncplane_hline_yx(plane, (dw)(y + rows - 1u), (dw)(x + 1u), style3[4], fill_attr, inner_cols);
    iplay_ncplane_putc_yx(plane, (dw)(y + rows - 1u), (dw)(x + cols - 1u), style3[3], fill_attr);
}

#define iplay_window_set_plane_field(state, plane_value) ((state)->plane = *(plane_value))

#define iplay_window_plane_field(state) (&(state)->plane)
#define iplay_window_plane_const_field(state) (&(state)->plane)

void iplay_window_init_root(IplayWindow *window, IplayNcPlane *root) {
    iplay_window_set_plane_field(window, root);
}

void iplay_window_init_subwindow(IplayWindow *window, const IplayWindow *parent, dw y, dw x, dw rows, dw cols) {
    iplay_ncplane_subplane(iplay_window_plane(window), iplay_window_plane_const(parent), y, x, rows, cols);
}

IplayNcPlane *iplay_window_plane(IplayWindow *window) {
    return iplay_window_plane_field(window);
}

const IplayNcPlane *iplay_window_plane_const(const IplayWindow *window) {
    return iplay_window_plane_const_field(window);
}

void iplay_window_resize(IplayWindow *window, dw rows, dw cols) {
    iplay_ncplane_resize(iplay_window_plane(window), rows, cols);
}

void iplay_window_origin_yx(const IplayWindow *window, dw *y, dw *x) {
    iplay_ncplane_origin_yx(iplay_window_plane_const(window), y, x);
}

dw iplay_window_rows(const IplayWindow *window) {
    return iplay_ncplane_rows(iplay_window_plane_const(window));
}

dw iplay_window_cols(const IplayWindow *window) {
    return iplay_ncplane_cols(iplay_window_plane_const(window));
}

void iplay_window_erase(IplayWindow *window, db attr) {
    iplay_ncplane_erase(iplay_window_plane(window), attr);
}

void iplay_window_fill_yx(IplayWindow *window, dw y, dw x, dw rows, dw cols, db ch, db attr) {
    iplay_ncplane_fill_yx(iplay_window_plane(window), y, x, rows, cols, ch, attr);
}

void iplay_window_box_yx(IplayWindow *window, dw y, dw x, dw rows, dw cols, db attr, db fill_attr) {
    iplay_ncplane_box_yx(iplay_window_plane(window), y, x, rows, cols, attr, fill_attr);
}

void iplay_window_cursor_yx(const IplayWindow *window, dw *y, dw *x) {
    iplay_ncplane_cursor_yx(iplay_window_plane_const(window), y, x);
}

void iplay_window_cursor_move_yx(IplayWindow *window, dw y, dw x) {
    iplay_ncplane_cursor_move_yx(iplay_window_plane(window), y, x);
}

void iplay_window_putc(IplayWindow *window, db ch, db attr) {
    iplay_ncplane_putc(iplay_window_plane(window), ch, attr);
}

void iplay_window_putstr(IplayWindow *window, const char *text, db attr) {
    iplay_ncplane_putstr(iplay_window_plane(window), text, attr);
}

void iplay_window_putnstr(IplayWindow *window, const char *text, db attr, dw width) {
    iplay_ncplane_putnstr(iplay_window_plane(window), text, attr, width);
}

void iplay_window_putnstr_fill_yx(IplayWindow *window, dw y, dw x, const char *text, db attr, dw width) {
    iplay_ncplane_putnstr_fill_yx(iplay_window_plane(window), y, x, text, attr, width);
}

void iplay_window_scroll_up(IplayWindow *window, dw top, dw left, dw rows, dw cols, dw count, db fill_attr) {
    iplay_ncplane_scroll_up(iplay_window_plane(window), top, left, rows, cols, count, fill_attr);
}

void iplay_window_scroll_down(IplayWindow *window, dw top, dw left, dw rows, dw cols, dw count, db fill_attr) {
    iplay_ncplane_scroll_down(iplay_window_plane(window), top, left, rows, cols, count, fill_attr);
}

void iplay_window_draw_audio_levels(IplayWindow *window, dw y, dw x, const IplayAudioLevels *levels, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr) {
    iplay_audio_levels_draw_yx(iplay_window_plane(window), y, x, levels, width, fill_ch, empty_ch, left_attr, right_attr, empty_attr);
}

void iplay_ncplane_scroll_up(IplayNcPlane *plane, dw top, dw left, dw rows, dw cols, dw count, db fill_attr) {
    dw visible_rows;
    dw visible_cols;
    dw row;
    dw col;
    if (!iplay_ncplane_visible_region(plane, top, left, rows, cols, &visible_rows, &visible_cols)) return;
    if (count >= visible_rows) {
        iplay_ncplane_fill_yx(plane, top, left, visible_rows, visible_cols, ' ', fill_attr);
        return;
    }
    for (row = 0; row < (dw)(visible_rows - count); ++row) {
        for (col = 0; col < visible_cols; ++col) {
            dw dst = iplay_ncplane_cell_offset(plane, (dw)(top + row), (dw)(left + col));
            dw src = iplay_ncplane_cell_offset(plane, (dw)(top + row + count), (dw)(left + col));
            iplay_ncplane_copy_cell_offset(plane, dst, src);
        }
    }
    iplay_ncplane_fill_yx(plane, (dw)(top + visible_rows - count), left, count, visible_cols, ' ', fill_attr);
}

void iplay_ncplane_scroll_down(IplayNcPlane *plane, dw top, dw left, dw rows, dw cols, dw count, db fill_attr) {
    dw visible_rows;
    dw visible_cols;
    dw row;
    dw col;
    if (!iplay_ncplane_visible_region(plane, top, left, rows, cols, &visible_rows, &visible_cols)) return;
    if (count >= visible_rows) {
        iplay_ncplane_fill_yx(plane, top, left, visible_rows, visible_cols, ' ', fill_attr);
        return;
    }
    row = (dw)(visible_rows - count);
    while (row != 0) {
        --row;
        for (col = 0; col < visible_cols; ++col) {
            dw dst = iplay_ncplane_cell_offset(plane, (dw)(top + row + count), (dw)(left + col));
            dw src = iplay_ncplane_cell_offset(plane, (dw)(top + row), (dw)(left + col));
            iplay_ncplane_copy_cell_offset(plane, dst, src);
        }
    }
    iplay_ncplane_fill_yx(plane, top, left, count, visible_cols, ' ', fill_attr);
}

void iplay_ncplane_putstr_yx(IplayNcPlane *plane, dw y, dw x, const char *text, db attr) {
    while (*text) {
        iplay_ncplane_putc_yx(plane, y, x, (db)*text, attr);
        ++text;
        ++x;
    }
}

void iplay_ncplane_putstr(IplayNcPlane *plane, const char *text, db attr) {
    while (*text) {
        iplay_ncplane_putc(plane, (db)*text, attr);
        ++text;
    }
}

void iplay_ncplane_putnstr_yx(IplayNcPlane *plane, dw y, dw x, const char *text, db attr, dw width) {
    dw written = 0;
    dw visible_rows;
    dw visible_cols;
    if (!iplay_ncplane_visible_region(plane, y, x, 1u, width, &visible_rows, &visible_cols)) return;
    (void)visible_rows;
    while (*text && written < visible_cols) {
        iplay_ncplane_putc_yx(plane, y, (dw)(x + written), (db)*text, attr);
        ++text;
        ++written;
    }
}

void iplay_ncplane_putnstr(IplayNcPlane *plane, const char *text, db attr, dw width) {
    dw written = 0;
    while (*text && written < width) {
        iplay_ncplane_putc(plane, (db)*text, attr);
        ++text;
        ++written;
    }
}

void iplay_ncplane_putnstr_fill_yx(IplayNcPlane *plane, dw y, dw x, const char *text, db attr, dw width) {
    dw written = 0;
    dw visible_rows;
    dw visible_cols;
    if (!iplay_ncplane_visible_region(plane, y, x, 1u, width, &visible_rows, &visible_cols)) return;
    (void)visible_rows;
    while (*text && written < visible_cols) {
        iplay_ncplane_putc_yx(plane, y, (dw)(x + written), (db)*text, attr);
        ++text;
        ++written;
    }
    while (written < visible_cols) {
        iplay_ncplane_putc_yx(plane, y, (dw)(x + written), ' ', attr);
        ++written;
    }
}

void iplay_ncplane_putnstr_fill(IplayNcPlane *plane, const char *text, db attr, dw width) {
    dw written = 0;
    while (*text && written < width) {
        iplay_ncplane_putc(plane, (db)*text, attr);
        ++text;
        ++written;
    }
    while (written < width) {
        iplay_ncplane_putc(plane, ' ', attr);
        ++written;
    }
}

IplayAttributedTextResult iplay_message_1be77_to_buffer(db *mem, dw video_base, dw src_offset, db y, db attr) {
    IplayAttributedTextResult result;
    const IplayTextMode *mode = iplay_text_current_mode();
    dw si;
    dw di;
    dw ax;
    db len = (db)string_len_at(mem, src_offset);
    db cl;
    db x;
    db right;
    dw msg_off;
    cl = (db)(0x4eu - len);
    x = (db)(cl >> 1);
    right = (db)(0x2au + (len >> 1));
    msg_off = (dw)((dw)(y - 1u) * iplay_text_mode_row_bytes(mode) + ((dw)cl & 0xfffeu) + 0x00a4u);
    iplay_draw_frame(mem + video_base, 3, 0x7f, 0x78, x, (db)(y - 2u), right, (db)(y + 2u));
    si = src_offset;
    di = (dw)(video_base + msg_off);
    ax = put_controlled_attributed_text(mem, mem, &si, &di, attr);
    result.src_offset = si;
    result.dst_offset = di;
    result.ax = ax;
    return result;
}

void iplay_message_1be77(IplayRegs *r, db *mem, dw video_base) {
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    IplayAttributedTextResult result = iplay_message_1be77_to_buffer(mem, video_base, (dw)esi, (db)eax, (db)(eax >> 8));
    apply_full_regs6(r, (eax & 0xffff0000UL) | result.ax,
                     ebx, ecx, edx,
                     (esi & 0xffff0000UL) | result.src_offset,
                     (edi & 0xffff0000UL) | result.dst_offset);
}

void iplay_draw_frame_plane(IplayNcPlane *plane, db style, db attr, db fill_attr, db x, db y, db right, db bottom) {
    db width;
    db height;
    if (style != 3) return;
    if (right < x || bottom < y) return;
    width = (db)(right - x + 1u);
    height = (db)(bottom - y + 1u);
    if (width < 2 || height < 2) return;
    iplay_ncplane_box_yx(plane, y, x, height, width, attr, fill_attr);
}

void iplay_draw_frame(db *mem, db style, db attr, db fill_attr, db x, db y, db right, db bottom) {
    IplayNcPlane plane;
    const IplayTextMode *mode = iplay_text_current_mode();
    iplay_ncplane_init_mode(&plane, mem, mode);
    iplay_draw_frame_plane(&plane, style, attr, fill_attr, x, y, right, bottom);
}

void iplay_write_scr(IplayRegs *r, const db *src_mem, db *dst_mem) {
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ebp = abi_ebp(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    dw bp = (dw)edi;
    IplayScreenStreamResult result = iplay_write_screen_stream_to_buffer(src_mem, dst_mem, (dw)esi, bp);
    apply_full_regs6(r, (eax & 0xffff0000UL) | result.ax,
                     ebx, ecx, edx,
                     (esi & 0xffff0000UL) | result.src_offset,
                     (edi & 0xffff0000UL) | result.dst_offset);
    apply_ebp_reg(r, (ebp & 0xffff0000UL) | bp);
}

void iplay_txt_draw_top_title_plane(IplayNcPlane *plane) {
    static const db title[] = {
        0x52,0x01,0x7f,
        'I','n','e','r','t','i','a',' ','P','l','a','y','e','r',' ','V','1','.','2','2',' ','A','s','s','e','m','b','l','y',' ',0x27,'9','4',' ',0x43,0x44,0x20,0x45,'d','i','t','i','o','n',' ','b','y',' ','S','o','u','n','d',' ','S','o','l','u','t','i','o','n','s',
        1,0xf4,0x01,
        'C','o','p','y','r','i','g','h','t',' ','(','c',')',' ','1','9','9','4',',','1','9','9','5',' ','b','y',' ','S','t','e','f','a','n',' ','D','a','n','e','s',' ','a','n','d',' ','R','a','m','o','n',' ','v','a','n',' ','G','o','r','k','o','m',0,
        2,0x78,1,0xaa,0x01,
        'S','h','e','l','l',':',' ','1','3','/','0','2','/','9','5',' ','2','1',':','1','5',':','5','8',
        1,0x46,0x01,1,0x20,0x01,
        'P','l','a','y','e','r',':',' ','1','3','/','0','2','/','9','5',' ','2','1',':','1','5',':','5','8',0
    };
    dw si = 0;
    dw di = 0;
    iplay_ncplane_box_yx(plane, 1, 2, 4, 0x4cu, 0x7f, 0x78);
    put_screen_stream(title, iplay_ncplane_cells(plane), &si, 0, &di);
    iplay_ncplane_putc_yx(plane, 2u, 44u, 'D', 0x7f);
}

void iplay_txt_draw_top_title(db *mem) {
    IplayNcPlane plane;
    iplay_ncplane_init_mode(&plane, mem, iplay_text_current_mode());
    iplay_txt_draw_top_title_plane(&plane);
}

void iplay_text_screen_draw_top_title(IplayTextScreen *screen) {
    iplay_txt_draw_top_title_plane(iplay_text_screen_root(screen));
}

const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_ORIGINAL = {
    5u, 6u, 7u,
    21u, 11u, 61u, 44u,
    1u, 46u,
    10u, 7u, 13u, 6u, 5u, 4u
};

const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_80COL = {
    20u, 21u, 22u,
    21u, 11u, 61u, 44u,
    16u, 46u,
    10u, 7u, 13u, 6u, 5u, 4u
};

const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_80X28 = {
    23u, 24u, 25u,
    21u, 11u, 61u, 44u,
    19u, 46u,
    10u, 7u, 13u, 6u, 5u, 4u
};

const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_80X50 = {
    45u, 46u, 47u,
    21u, 11u, 61u, 44u,
    41u, 46u,
    10u, 7u, 13u, 6u, 5u, 4u
};

const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_40COL = {
    5u, 6u, 7u,
    9u, 1u, 29u, 19u,
    1u, 24u,
    10u, 7u, 13u, 6u, 5u, 4u
};

const IplayBottomLayout *iplay_bottom_layout(void) {
    return iplay_bottom_layout_for_mode(iplay_text_current_mode());
}

const IplayBottomLayout *iplay_bottom_layout_for_mode(const IplayTextMode *mode) {
    if (iplay_text_mode_equals(mode, &IPLAY_TEXT_MODE_40X25)) return &IPLAY_BOTTOM_LAYOUT_40COL;
    if (iplay_text_mode_equals(mode, &IPLAY_TEXT_MODE_80X28)) return &IPLAY_BOTTOM_LAYOUT_80X28;
    if (iplay_text_mode_equals(mode, &IPLAY_TEXT_MODE_80X50)) return &IPLAY_BOTTOM_LAYOUT_80X28;
    return &IPLAY_BOTTOM_LAYOUT_80COL;
}

#define iplay_bottom_layout_timing_y_field(state) ((state)->timing_y)
#define iplay_bottom_layout_module_y_field(state) ((state)->module_y)
#define iplay_bottom_layout_pattern_y_field(state) ((state)->pattern_y)
#define iplay_bottom_layout_playstate_y_field(state) ((state)->playstate_y)
#define iplay_bottom_layout_left_x_field(state) ((state)->left_x)
#define iplay_bottom_layout_value_x_field(state) ((state)->value_x)
#define iplay_bottom_layout_playstate_x_field(state) ((state)->playstate_x)
#define iplay_bottom_layout_flag_x_field(state) ((state)->flag_x)
#define iplay_bottom_layout_timing_width_field(state) ((state)->timing_width)
#define iplay_bottom_layout_module_width_field(state) ((state)->module_width)
#define iplay_bottom_layout_pattern_width_field(state) ((state)->pattern_width)
#define iplay_bottom_layout_value_width_field(state) ((state)->value_width)
#define iplay_bottom_layout_playstate_width_field(state) ((state)->playstate_width)
#define iplay_bottom_layout_mode_x_field(state) ((state)->mode_x)
#define iplay_bottom_layout_mode_width_field(state) ((state)->mode_width)

static int iplay_bottom_layout_rows_fit(const IplayBottomLayout *layout, dw rows) {
    if (iplay_bottom_layout_module_y_field(layout) >= rows) return 0;
    if (iplay_bottom_layout_pattern_y_field(layout) >= rows) return 0;
    if (iplay_bottom_layout_timing_y_field(layout) >= rows) return 0;
    if (iplay_bottom_layout_playstate_y_field(layout) >= rows) return 0;
    return 1;
}

static int iplay_bottom_layout_cols_fit(const IplayBottomLayout *layout, dw cols) {
    if (iplay_bottom_layout_flag_x_field(layout) >= cols) return 0;
    if ((dw)(iplay_bottom_layout_left_x_field(layout) + iplay_bottom_layout_module_width_field(layout)) > cols) return 0;
    if ((dw)(iplay_bottom_layout_left_x_field(layout) + iplay_bottom_layout_pattern_width_field(layout)) > cols) return 0;
    if ((dw)(iplay_bottom_layout_left_x_field(layout) + iplay_bottom_layout_timing_width_field(layout)) > cols) return 0;
    if ((dw)(iplay_bottom_layout_mode_x_field(layout) + iplay_bottom_layout_mode_width_field(layout)) > cols) return 0;
    if ((dw)(iplay_bottom_layout_value_x_field(layout) + iplay_bottom_layout_value_width_field(layout)) > cols) return 0;
    if ((dw)(iplay_bottom_layout_playstate_x_field(layout) + iplay_bottom_layout_playstate_width_field(layout)) > cols) return 0;
    return 1;
}

int iplay_bottom_layout_fits(const IplayBottomLayout *layout, const IplayTextMode *mode) {
    dw rows = iplay_text_mode_rows(mode);
    dw cols = iplay_text_mode_cols(mode);
    return iplay_bottom_layout_rows_fit(layout, rows) && iplay_bottom_layout_cols_fit(layout, cols);
}

dw iplay_audio_bytes_per_frame(const IplayAudioFormat *format) {
    return (dw)((iplay_audio_format_bits_per_sample(format) / 8u) * iplay_audio_format_channels(format));
}

dw iplay_audio_frames_for_bytes(const IplayAudioFormat *format, dw byte_count) {
    dw frame_bytes = iplay_audio_bytes_per_frame(format);
    if (frame_bytes == 0) return 0;
    return (dw)(byte_count / frame_bytes);
}

#define iplay_audio_format_sample_rate_field(state) ((state)->sample_rate)
#define iplay_audio_format_bits_per_sample_field(state) ((state)->bits_per_sample)
#define iplay_audio_format_channels_field(state) ((state)->channels)
#define iplay_audio_format_signed_samples_field(state) ((state)->signed_samples)

#define iplay_audio_format_set_sample_rate_field(state, value) ((state)->sample_rate = (value))

#define iplay_audio_format_set_bits_per_sample_field(state, value) ((state)->bits_per_sample = (value))

#define iplay_audio_format_set_channels_field(state, value) ((state)->channels = (value))

#define iplay_audio_format_set_signed_samples_field(state, value) ((state)->signed_samples = (value))

dw iplay_audio_format_sample_rate(const IplayAudioFormat *format) {
    return iplay_audio_format_sample_rate_field(format);
}

db iplay_audio_format_bits_per_sample(const IplayAudioFormat *format) {
    return iplay_audio_format_bits_per_sample_field(format);
}

db iplay_audio_format_channels(const IplayAudioFormat *format) {
    return iplay_audio_format_channels_field(format);
}

db iplay_audio_format_signed_samples(const IplayAudioFormat *format) {
    return iplay_audio_format_signed_samples_field(format);
}

void iplay_audio_format_set(IplayAudioFormat *format, dw sample_rate, db bits_per_sample, db channels, db signed_samples) {
    iplay_audio_format_set_sample_rate_field(format, sample_rate);
    iplay_audio_format_set_bits_per_sample_field(format, bits_per_sample);
    iplay_audio_format_set_channels_field(format, channels);
    iplay_audio_format_set_signed_samples_field(format, signed_samples);
}

int iplay_audio_format_equals(const IplayAudioFormat *a, const IplayAudioFormat *b) {
    return iplay_audio_format_sample_rate(a) == iplay_audio_format_sample_rate(b)
        && iplay_audio_format_bits_per_sample(a) == iplay_audio_format_bits_per_sample(b)
        && iplay_audio_format_channels(a) == iplay_audio_format_channels(b)
        && iplay_audio_format_signed_samples(a) == iplay_audio_format_signed_samples(b);
}

int iplay_audio_format_is_sb16_stereo_16(const IplayAudioFormat *format) {
    return iplay_audio_format_bits_per_sample(format) == 16u && iplay_audio_format_channels(format) == 2u && iplay_audio_format_signed_samples(format) != 0;
}

const char *iplay_audio_format_name(const IplayAudioFormat *format) {
    if (iplay_audio_format_bits_per_sample(format) == 8u && iplay_audio_format_channels(format) == 1u && iplay_audio_format_signed_samples(format) == 0) return "u8-mono";
    if (iplay_audio_format_bits_per_sample(format) == 8u && iplay_audio_format_channels(format) == 2u && iplay_audio_format_signed_samples(format) == 0) return "u8-stereo";
    if (iplay_audio_format_bits_per_sample(format) == 16u && iplay_audio_format_channels(format) == 1u && iplay_audio_format_signed_samples(format) != 0) return "s16-mono";
    if (iplay_audio_format_bits_per_sample(format) == 16u && iplay_audio_format_channels(format) == 2u && iplay_audio_format_signed_samples(format) != 0) return "s16-stereo";
    return "unsupported";
}

const char *iplay_audio_backend_name(IplayAudioBackend backend) {
    if (backend == IPLAY_AUDIO_BACKEND_SB16_STEREO) return "sb16-stereo";
    if (backend == IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE) return "sdl-compatible-sb16-stereo";
    return "unknown";
}

int iplay_audio_backend_is_sb16_scope(IplayAudioBackend backend) {
    return backend == IPLAY_AUDIO_BACKEND_SB16_STEREO || backend == IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE;
}

int iplay_audio_backend_is_sb16_hardware(IplayAudioBackend backend) {
    return backend == IPLAY_AUDIO_BACKEND_SB16_STEREO;
}

int iplay_audio_backend_is_sdl_compatible(IplayAudioBackend backend) {
    return backend == IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE;
}

#define iplay_sdl_audio_spec_backend_field(state) ((state)->backend)

#define iplay_sdl_audio_spec_set_backend_field(state, value) ((state)->backend = (value))

#define iplay_sdl_audio_spec_format_field(state) (&(state)->format)

#define iplay_sdl_audio_spec_set_format_field(state, value) ((state)->format = *(value))

#define iplay_sdl_audio_spec_hardware_enabled_field(state) ((state)->hardware_enabled)

#define iplay_sdl_audio_spec_set_hardware_enabled_field(state, value) ((state)->hardware_enabled = (value))

IplayAudioBackend iplay_sdl_audio_spec_backend(const IplaySdlAudioSpec *spec) {
    return iplay_sdl_audio_spec_backend_field(spec);
}

const char *iplay_sdl_audio_spec_backend_name(const IplaySdlAudioSpec *spec) {
    return iplay_audio_backend_name(iplay_sdl_audio_spec_backend(spec));
}

const IplayAudioFormat *iplay_sdl_audio_spec_format(const IplaySdlAudioSpec *spec) {
    return iplay_sdl_audio_spec_format_field(spec);
}

dw iplay_sdl_audio_spec_sample_rate(const IplaySdlAudioSpec *spec) {
    return iplay_audio_format_sample_rate(iplay_sdl_audio_spec_format(spec));
}

db iplay_sdl_audio_spec_bits_per_sample(const IplaySdlAudioSpec *spec) {
    return iplay_audio_format_bits_per_sample(iplay_sdl_audio_spec_format(spec));
}

db iplay_sdl_audio_spec_channels(const IplaySdlAudioSpec *spec) {
    return iplay_audio_format_channels(iplay_sdl_audio_spec_format(spec));
}

db iplay_sdl_audio_spec_signed_samples(const IplaySdlAudioSpec *spec) {
    return iplay_audio_format_signed_samples(iplay_sdl_audio_spec_format(spec));
}

int iplay_sdl_audio_spec_hardware_enabled(const IplaySdlAudioSpec *spec) {
    return iplay_sdl_audio_spec_hardware_enabled_field(spec) != 0;
}

int iplay_sdl_audio_spec_is_sb16_compatible(const IplaySdlAudioSpec *spec) {
    return iplay_audio_backend_is_sb16_scope(iplay_sdl_audio_spec_backend(spec))
        && iplay_audio_format_equals(iplay_sdl_audio_spec_format(spec), &IPLAY_AUDIO_SB16_STEREO_16);
}

int iplay_sdl_audio_spec_is_sb16_hardware(const IplaySdlAudioSpec *spec) {
    return iplay_sdl_audio_spec_hardware_enabled(spec)
        && iplay_audio_backend_is_sb16_hardware(iplay_sdl_audio_spec_backend(spec))
        && iplay_audio_format_equals(iplay_sdl_audio_spec_format(spec), &IPLAY_AUDIO_SB16_STEREO_16);
}

int iplay_sdl_audio_spec_is_sdl_compatible(const IplaySdlAudioSpec *spec) {
    return !iplay_sdl_audio_spec_hardware_enabled(spec)
        && iplay_audio_backend_is_sdl_compatible(iplay_sdl_audio_spec_backend(spec))
        && iplay_audio_format_equals(iplay_sdl_audio_spec_format(spec), &IPLAY_AUDIO_SB16_STEREO_16);
}

int iplay_audio_rates_match(const IplayAudioFormat *src_format, const IplayAudioFormat *dst_format) {
    return iplay_audio_format_sample_rate(src_format) == iplay_audio_format_sample_rate(dst_format);
}

const IplayAudioFormat *iplay_audio_source_format(db bits_per_sample, db channels, db signed_samples) {
    if (bits_per_sample == 8u && channels == 1u && signed_samples == 0) return &IPLAY_AUDIO_U8_MONO;
    if (bits_per_sample == 8u && channels == 2u && signed_samples == 0) return &IPLAY_AUDIO_U8_STEREO;
    if (bits_per_sample == 16u && channels == 1u && signed_samples != 0) return &IPLAY_AUDIO_S16_MONO;
    if (bits_per_sample == 16u && channels == 2u && signed_samples != 0) return &IPLAY_AUDIO_S16_STEREO;
    return 0;
}

int iplay_audio_make_source_format(IplayAudioFormat *format, dw sample_rate, db bits_per_sample, db channels, db signed_samples) {
    if (iplay_audio_source_format(bits_per_sample, channels, signed_samples) == 0) return 0;
    iplay_audio_format_set(format, sample_rate, bits_per_sample, channels, signed_samples ? 1u : 0u);
    return 1;
}

#define iplay_audio_sink_set_format_field(state, value) ((state)->format = *(value))

#define iplay_audio_sink_set_write_field(state, value) ((state)->write = (value))

#define iplay_audio_sink_set_user_field(state, value) ((state)->user = (value))

#define iplay_audio_sink_set_active_field(state, value) ((state)->active = (value) ? 1u : 0u)

#define iplay_audio_sink_set_capacity_field(state, value) ((state)->capacity_frames = (value))

#define iplay_audio_sink_format_field(state) (&(state)->format)
#define iplay_audio_sink_capacity_field(state) ((state)->capacity_frames)
#define iplay_audio_sink_frames_written_field(state) ((state)->frames_written)
#define iplay_audio_sink_underrun_frames_field(state) ((state)->underrun_frames)
#define iplay_audio_sink_dropped_frames_field(state) ((state)->dropped_frames)
#define iplay_audio_sink_is_active_field(state) ((state)->active != 0)
#define iplay_audio_sink_write_callback_field(state) ((state)->write)
#define iplay_audio_sink_write_user_field(state) ((state)->user)

#define iplay_audio_sink_set_frames_written_field(state, value) ((state)->frames_written = (value))

#define iplay_audio_sink_set_underrun_frames_field(state, value) ((state)->underrun_frames = (value))

#define iplay_audio_sink_set_dropped_frames_field(state, value) ((state)->dropped_frames = (value))

void iplay_audio_sink_init(IplayAudioSink *sink, const IplayAudioFormat *format, IplayAudioWriteFn write, void *user) {
    iplay_audio_sink_set_format(sink, format);
    iplay_audio_sink_set_write_callback(sink, write, user);
    iplay_audio_sink_reset_counters(sink);
    iplay_audio_sink_set_capacity(sink, 0xffffffffUL);
    iplay_audio_sink_set_active(sink, 0);
}

void iplay_audio_sink_set_format(IplayAudioSink *sink, const IplayAudioFormat *format) {
    iplay_audio_sink_set_format_field(sink, format);
}

void iplay_audio_sink_set_write_callback(IplayAudioSink *sink, IplayAudioWriteFn write, void *user) {
    iplay_audio_sink_set_write_field(sink, write);
    iplay_audio_sink_set_user_field(sink, user);
}

void iplay_audio_sink_start(IplayAudioSink *sink) {
    iplay_audio_sink_set_active(sink, 1);
}

void iplay_audio_sink_stop(IplayAudioSink *sink) {
    iplay_audio_sink_set_active(sink, 0);
}

void iplay_audio_sink_set_active(IplayAudioSink *sink, int active) {
    iplay_audio_sink_set_active_field(sink, active);
}

void iplay_audio_sink_reset_counters(IplayAudioSink *sink) {
    iplay_audio_sink_clear_frames_written(sink);
    iplay_audio_sink_clear_underrun_frames(sink);
    iplay_audio_sink_clear_dropped_frames(sink);
}

void iplay_audio_sink_set_capacity(IplayAudioSink *sink, dd capacity_frames) {
    iplay_audio_sink_set_capacity_field(sink, capacity_frames);
}

void iplay_audio_sink_add_capacity(IplayAudioSink *sink, dd capacity_frames) {
    dd old = iplay_audio_sink_capacity(sink);
    dd updated = old + capacity_frames;
    if (updated < old) updated = 0xffffffffUL;
    iplay_audio_sink_set_capacity(sink, updated);
}

const IplayAudioFormat *iplay_audio_sink_format(const IplayAudioSink *sink) {
    return iplay_audio_sink_format_field(sink);
}

dw iplay_audio_sink_bytes_per_frame(const IplayAudioSink *sink) {
    return iplay_audio_bytes_per_frame(iplay_audio_sink_format(sink));
}

dd iplay_audio_sink_capacity(const IplayAudioSink *sink) {
    return iplay_audio_sink_capacity_field(sink);
}

dd iplay_audio_sink_frames_written(const IplayAudioSink *sink) {
    return iplay_audio_sink_frames_written_field(sink);
}

dd iplay_audio_sink_underrun_frames(const IplayAudioSink *sink) {
    return iplay_audio_sink_underrun_frames_field(sink);
}

dd iplay_audio_sink_dropped_frames(const IplayAudioSink *sink) {
    return iplay_audio_sink_dropped_frames_field(sink);
}

int iplay_audio_sink_is_active(const IplayAudioSink *sink) {
    return iplay_audio_sink_is_active_field(sink);
}

IplayAudioWriteFn iplay_audio_sink_write_callback(const IplayAudioSink *sink) {
    return iplay_audio_sink_write_callback_field(sink);
}

void *iplay_audio_sink_write_user(const IplayAudioSink *sink) {
    return iplay_audio_sink_write_user_field(sink);
}

void iplay_audio_sink_set_frames_written(IplayAudioSink *sink, dd frames) {
    iplay_audio_sink_set_frames_written_field(sink, frames);
}

void iplay_audio_sink_set_underrun_frames(IplayAudioSink *sink, dd frames) {
    iplay_audio_sink_set_underrun_frames_field(sink, frames);
}

void iplay_audio_sink_set_dropped_frames(IplayAudioSink *sink, dd frames) {
    iplay_audio_sink_set_dropped_frames_field(sink, frames);
}

void iplay_audio_sink_clear_frames_written(IplayAudioSink *sink) {
    iplay_audio_sink_set_frames_written(sink, 0);
}

void iplay_audio_sink_clear_underrun_frames(IplayAudioSink *sink) {
    iplay_audio_sink_set_underrun_frames(sink, 0);
}

void iplay_audio_sink_clear_dropped_frames(IplayAudioSink *sink) {
    iplay_audio_sink_set_dropped_frames(sink, 0);
}

void iplay_audio_sink_add_frames_written(IplayAudioSink *sink, dd frames) {
    iplay_audio_sink_set_frames_written(sink, iplay_audio_sink_frames_written(sink) + frames);
}

void iplay_audio_sink_add_underrun_frames(IplayAudioSink *sink, dd frames) {
    iplay_audio_sink_set_underrun_frames(sink, iplay_audio_sink_underrun_frames(sink) + frames);
}

void iplay_audio_sink_add_dropped_frames(IplayAudioSink *sink, dd frames) {
    iplay_audio_sink_set_dropped_frames(sink, iplay_audio_sink_dropped_frames(sink) + frames);
}

void iplay_audio_sink_consume_capacity(IplayAudioSink *sink, dd frames) {
    iplay_audio_sink_set_capacity(sink, iplay_audio_sink_capacity(sink) - frames);
}

void iplay_audio_sink_write(IplayAudioSink *sink, const db *pcm, dw byte_count) {
    IplayAudioWriteFn write = iplay_audio_sink_write_callback(sink);
    const IplayAudioFormat *format = iplay_audio_sink_format(sink);
    dw frame_bytes = iplay_audio_bytes_per_frame(format);
    dd frames;
    if (!iplay_audio_sink_is_active(sink)) return;
    if (frame_bytes == 0) return;
    byte_count = (dw)(byte_count - (byte_count % frame_bytes));
    if (byte_count == 0) return;
    frames = iplay_audio_frames_for_bytes(format, byte_count);
    if (frames > iplay_audio_sink_capacity(sink)) {
        iplay_audio_sink_add_dropped_frames(sink, frames - iplay_audio_sink_capacity(sink));
        frames = iplay_audio_sink_capacity(sink);
        byte_count = (dw)(frames * frame_bytes);
    }
    if (frames == 0) return;
    if (write != 0) write(iplay_audio_sink_write_user(sink), pcm, byte_count);
    iplay_audio_sink_add_frames_written(sink, frames);
    iplay_audio_sink_consume_capacity(sink, frames);
}

void iplay_audio_sink_write_silence(IplayAudioSink *sink, dw frame_count) {
    db silence[16];
    dw frame_bytes = iplay_audio_sink_bytes_per_frame(sink);
    dw chunk_frames;
    dw i;
    if (frame_bytes == 0) return;
    if (frame_bytes > sizeof(silence)) return;
    if (!iplay_audio_sink_is_active(sink)) return;
    iplay_audio_sink_add_underrun_frames(sink, frame_count);
    for (i = 0; i < sizeof(silence); ++i) silence[i] = 0;
    while (frame_count != 0) {
        chunk_frames = (dw)(sizeof(silence) / frame_bytes);
        if (chunk_frames > frame_count) chunk_frames = frame_count;
        iplay_audio_sink_write(sink, silence, (dw)(chunk_frames * frame_bytes));
        frame_count = (dw)(frame_count - chunk_frames);
    }
}

static void put_s16le(db *dst, dw off, int16_t value) {
    dst[off] = (db)value;
    dst[(dw)(off + 1u)] = (db)((uint16_t)value >> 8);
}

dw iplay_audio_u8_to_s16_stereo(const db *src, dw src_frames, db src_channels, db *dst, dw dst_bytes) {
    dw frame;
    dw out_frames = (dw)(dst_bytes / 4u);
    if (src_channels != 1u && src_channels != 2u) return 0;
    if (out_frames > src_frames) out_frames = src_frames;
    for (frame = 0; frame < out_frames; ++frame) {
        db left_u8;
        db right_u8;
        int16_t left;
        int16_t right;
        if (src_channels == 1u) {
            left_u8 = src[frame];
            right_u8 = left_u8;
        } else {
            left_u8 = src[(dw)(frame * 2u)];
            right_u8 = src[(dw)(frame * 2u + 1u)];
        }
        left = left_u8 == 255u ? 32767 : (int16_t)(((int)left_u8 - 128) << 8);
        right = right_u8 == 255u ? 32767 : (int16_t)(((int)right_u8 - 128) << 8);
        put_s16le(dst, (dw)(frame * 4u), left);
        put_s16le(dst, (dw)(frame * 4u + 2u), right);
    }
    return (dw)(out_frames * 4u);
}

dw iplay_audio_s16_to_s16_stereo(const db *src, dw src_frames, db src_channels, db *dst, dw dst_bytes) {
    dw frame;
    dw out_frames = (dw)(dst_bytes / 4u);
    if (src_channels != 1u && src_channels != 2u) return 0;
    if (out_frames > src_frames) out_frames = src_frames;
    for (frame = 0; frame < out_frames; ++frame) {
        dw src_off = (dw)(frame * src_channels * 2u);
        dw dst_off = (dw)(frame * 4u);
        db left_lo = src[src_off];
        db left_hi = src[(dw)(src_off + 1u)];
        db right_lo = left_lo;
        db right_hi = left_hi;
        if (src_channels == 2u) {
            right_lo = src[(dw)(src_off + 2u)];
            right_hi = src[(dw)(src_off + 3u)];
        }
        dst[dst_off] = left_lo;
        dst[(dw)(dst_off + 1u)] = left_hi;
        dst[(dw)(dst_off + 2u)] = right_lo;
        dst[(dw)(dst_off + 3u)] = right_hi;
    }
    return (dw)(out_frames * 4u);
}

dw iplay_audio_convert_to_sink_format(const IplayAudioFormat *src_format, const db *src, dw src_frames, const IplayAudioFormat *dst_format, db *dst, dw dst_bytes) {
    if (!iplay_audio_format_is_sb16_stereo_16(dst_format)) return 0;
    if (!iplay_audio_rates_match(src_format, dst_format)) return 0;
    if (iplay_audio_format_bits_per_sample(src_format) == 8u && iplay_audio_format_signed_samples(src_format) == 0) {
        return iplay_audio_u8_to_s16_stereo(src, src_frames, iplay_audio_format_channels(src_format), dst, dst_bytes);
    }
    if (iplay_audio_format_bits_per_sample(src_format) == 16u && iplay_audio_format_signed_samples(src_format) != 0) {
        return iplay_audio_s16_to_s16_stereo(src, src_frames, iplay_audio_format_channels(src_format), dst, dst_bytes);
    }
    return 0;
}

dw iplay_audio_sink_write_converted(IplayAudioSink *sink, const IplayAudioFormat *src_format, const db *src, dw src_frames, db *scratch, dw scratch_bytes) {
    dw bytes = iplay_audio_convert_to_sink_format(src_format, src, src_frames, iplay_audio_sink_format(sink), scratch, scratch_bytes);
    iplay_audio_sink_write(sink, scratch, bytes);
    return bytes;
}

#define iplay_audio_output_sink_field(state) (&(state)->sink)
#define iplay_audio_output_sink_const_field(state) (&(state)->sink)
#define iplay_audio_output_source_format_mut_field(state) (&(state)->source_format)
#define iplay_audio_output_source_format_field(state) (&(state)->source_format)

#define iplay_audio_output_set_scratch_buffer_field(state, value) ((state)->scratch = (value))

#define iplay_audio_output_set_scratch_bytes_field(state, value) ((state)->scratch_bytes = (value))

#define iplay_audio_output_levels_field(state) (&(state)->levels)
#define iplay_audio_output_levels_mut_field(state) (&(state)->levels)
#define iplay_audio_output_scratch_field(state) ((state)->scratch)
#define iplay_audio_output_scratch_bytes_field(state) ((state)->scratch_bytes)

IplayAudioSink *iplay_audio_output_sink(IplayAudioOutput *output) {
    return iplay_audio_output_sink_field(output);
}

const IplayAudioSink *iplay_audio_output_sink_const(const IplayAudioOutput *output) {
    return iplay_audio_output_sink_const_field(output);
}

IplayAudioFormat *iplay_audio_output_source_format_mut(IplayAudioOutput *output) {
    return iplay_audio_output_source_format_mut_field(output);
}

void iplay_audio_output_set_source_format(IplayAudioOutput *output, const IplayAudioFormat *source_format) {
    iplay_audio_format_set(
        iplay_audio_output_source_format_mut(output),
        iplay_audio_format_sample_rate(source_format),
        iplay_audio_format_bits_per_sample(source_format),
        iplay_audio_format_channels(source_format),
        iplay_audio_format_signed_samples(source_format)
    );
}

void iplay_audio_output_set_scratch_buffer(IplayAudioOutput *output, db *scratch) {
    iplay_audio_output_set_scratch_buffer_field(output, scratch);
}

void iplay_audio_output_set_scratch_bytes(IplayAudioOutput *output, dw scratch_bytes) {
    iplay_audio_output_set_scratch_bytes_field(output, scratch_bytes);
}

void iplay_audio_output_set_scratch(IplayAudioOutput *output, db *scratch, dw scratch_bytes) {
    iplay_audio_output_set_scratch_buffer(output, scratch);
    iplay_audio_output_set_scratch_bytes(output, scratch_bytes);
}

void iplay_audio_output_init(IplayAudioOutput *output, const IplayAudioFormat *source_format, IplayAudioWriteFn write, void *user, db *scratch, dw scratch_bytes) {
    iplay_audio_output_set_source_format(output, source_format);
    iplay_audio_output_set_scratch(output, scratch, scratch_bytes);
    iplay_audio_sink_init(iplay_audio_output_sink(output), &IPLAY_AUDIO_SB16_STEREO_16, write, user);
    iplay_audio_output_reset_levels(output);
}

void iplay_audio_output_init_sb16_stereo(IplayAudioOutput *output, IplayAudioWriteFn write, void *user) {
    iplay_audio_output_init(output, &IPLAY_AUDIO_SB16_STEREO_16, write, user, 0, 0);
}

void iplay_audio_output_start(IplayAudioOutput *output) {
    iplay_audio_sink_start(iplay_audio_output_sink(output));
}

void iplay_audio_output_stop(IplayAudioOutput *output) {
    iplay_audio_sink_stop(iplay_audio_output_sink(output));
}

int iplay_audio_output_is_active(const IplayAudioOutput *output) {
    return iplay_audio_sink_is_active(iplay_audio_output_sink_const(output));
}

void iplay_audio_output_reset_counters(IplayAudioOutput *output) {
    iplay_audio_sink_reset_counters(iplay_audio_output_sink(output));
}

void iplay_audio_output_set_capacity(IplayAudioOutput *output, dd capacity_frames) {
    iplay_audio_sink_set_capacity(iplay_audio_output_sink(output), capacity_frames);
}

void iplay_audio_output_add_capacity(IplayAudioOutput *output, dd capacity_frames) {
    iplay_audio_sink_add_capacity(iplay_audio_output_sink(output), capacity_frames);
}

dd iplay_audio_output_capacity(const IplayAudioOutput *output) {
    return iplay_audio_sink_capacity(iplay_audio_output_sink_const(output));
}

dw iplay_audio_output_accepted_frames(const IplayAudioOutput *output, dw frame_count) {
    if (!iplay_audio_output_is_active(output)) return 0;
    if (frame_count > iplay_audio_output_capacity(output)) return (dw)iplay_audio_output_capacity(output);
    return frame_count;
}

dd iplay_audio_output_frames_written(const IplayAudioOutput *output) {
    return iplay_audio_sink_frames_written(iplay_audio_output_sink_const(output));
}

dd iplay_audio_output_underrun_frames(const IplayAudioOutput *output) {
    return iplay_audio_sink_underrun_frames(iplay_audio_output_sink_const(output));
}

dd iplay_audio_output_dropped_frames(const IplayAudioOutput *output) {
    return iplay_audio_sink_dropped_frames(iplay_audio_output_sink_const(output));
}

const IplayAudioFormat *iplay_audio_output_source_format(const IplayAudioOutput *output) {
    return iplay_audio_output_source_format_field(output);
}

const IplayAudioFormat *iplay_audio_output_sink_format(const IplayAudioOutput *output) {
    return iplay_audio_sink_format(iplay_audio_output_sink_const(output));
}

dw iplay_audio_output_bytes_per_frame(const IplayAudioOutput *output) {
    return iplay_audio_sink_bytes_per_frame(iplay_audio_output_sink_const(output));
}

dw iplay_audio_output_frames_for_bytes(const IplayAudioOutput *output, dw byte_count) {
    return iplay_audio_frames_for_bytes(iplay_audio_output_sink_format(output), byte_count);
}

dw iplay_audio_output_bytes_for_frames(const IplayAudioOutput *output, dw frame_count) {
    return (dw)(frame_count * iplay_audio_output_bytes_per_frame(output));
}

int iplay_audio_output_is_sb16_stereo(const IplayAudioOutput *output) {
    return iplay_audio_format_equals(iplay_audio_output_source_format(output), &IPLAY_AUDIO_SB16_STEREO_16)
        && iplay_audio_format_equals(iplay_audio_output_sink_format(output), &IPLAY_AUDIO_SB16_STEREO_16);
}

const IplayAudioLevels *iplay_audio_output_levels(const IplayAudioOutput *output) {
    return iplay_audio_output_levels_field(output);
}

IplayAudioLevels *iplay_audio_output_levels_mut(IplayAudioOutput *output) {
    return iplay_audio_output_levels_mut_field(output);
}

db *iplay_audio_output_scratch(IplayAudioOutput *output) {
    return iplay_audio_output_scratch_field(output);
}

dw iplay_audio_output_scratch_bytes(const IplayAudioOutput *output) {
    return iplay_audio_output_scratch_bytes_field(output);
}

#define iplay_audio_levels_set_left_peak_field(state, value) ((state)->left_peak = (value))

#define iplay_audio_levels_set_right_peak_field(state, value) ((state)->right_peak = (value))

#define iplay_audio_levels_set_left_16_field(state, value) ((state)->left_16 = (value))

#define iplay_audio_levels_set_right_16_field(state, value) ((state)->right_16 = (value))

void iplay_audio_levels_set(IplayAudioLevels *levels, dw left_peak, dw right_peak) {
    iplay_audio_levels_set_left_peak_field(levels, left_peak);
    iplay_audio_levels_set_right_peak_field(levels, right_peak);
    iplay_audio_levels_set_left_16_field(levels, iplay_audio_level_to_16(left_peak));
    iplay_audio_levels_set_right_16_field(levels, iplay_audio_level_to_16(right_peak));
}

void iplay_audio_levels_clear(IplayAudioLevels *levels) {
    iplay_audio_levels_set(levels, 0, 0);
}

void iplay_audio_output_draw_levels_yx(IplayNcPlane *plane, dw y, dw x, const IplayAudioOutput *output, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr) {
    iplay_audio_levels_draw_yx(plane, y, x, iplay_audio_output_levels(output), width, fill_ch, empty_ch, left_attr, right_attr, empty_attr);
}

void iplay_audio_output_reset_levels(IplayAudioOutput *output) {
    iplay_audio_levels_clear(iplay_audio_output_levels_mut(output));
}

dw iplay_audio_output_write_mixer_frames(IplayAudioOutput *output, const db *src, dw src_frames) {
    dw bytes;
    dw accepted_bytes;
    dw accepted_frames;
    db *scratch;
    const IplayAudioFormat *sink_format;
    if (src == 0) return 0;
    if (iplay_audio_output_is_sb16_stereo(output)) {
        return iplay_audio_output_write_sb16_frames(output, src, src_frames);
    }
    scratch = iplay_audio_output_scratch(output);
    sink_format = iplay_audio_output_sink_format(output);
    bytes = iplay_audio_convert_to_sink_format(iplay_audio_output_source_format(output), src, src_frames, sink_format, scratch, iplay_audio_output_scratch_bytes(output));
    accepted_frames = iplay_audio_output_frames_for_bytes(output, bytes);
    accepted_frames = iplay_audio_output_accepted_frames(output, accepted_frames);
    accepted_bytes = iplay_audio_output_bytes_for_frames(output, accepted_frames);
    iplay_audio_sb16_stereo_levels(iplay_audio_output_levels_mut(output), scratch, accepted_frames);
    iplay_audio_sink_write(iplay_audio_output_sink(output), scratch, bytes);
    return accepted_bytes;
}

dw iplay_audio_output_write_sb16_frames(IplayAudioOutput *output, const db *pcm, dw frame_count) {
    dw byte_count;
    dw accepted_bytes;
    dw accepted_frames = frame_count;
    if (pcm == 0) return 0;
    if (!iplay_audio_output_is_sb16_stereo(output)) return 0;
    accepted_frames = iplay_audio_output_accepted_frames(output, accepted_frames);
    iplay_audio_sb16_stereo_levels(iplay_audio_output_levels_mut(output), pcm, accepted_frames);
    byte_count = iplay_audio_output_bytes_for_frames(output, frame_count);
    accepted_bytes = iplay_audio_output_bytes_for_frames(output, accepted_frames);
    iplay_audio_sink_write(iplay_audio_output_sink(output), pcm, byte_count);
    return accepted_bytes;
}

void iplay_audio_output_write_silence(IplayAudioOutput *output, dw frame_count) {
    iplay_audio_sink_write_silence(iplay_audio_output_sink(output), frame_count);
    iplay_audio_output_reset_levels(output);
}

void iplay_sdl_audio_device_config_sb16_stereo(IplaySdlAudioDeviceConfig *config, void *userdata, IplayAudioBackend backend, db hardware_enabled) {
    iplay_sdl_audio_device_config_set_format(config, &IPLAY_AUDIO_SB16_STEREO_16);
    iplay_sdl_audio_device_config_set_samples(config, 1024u);
    iplay_sdl_audio_device_config_set_callback(config, iplay_sdl_audio_device_callback, userdata);
    iplay_sdl_audio_device_config_set_backend(config, backend, hardware_enabled);
}

#define iplay_sdl_audio_device_config_set_frequency_field(state, value) ((state)->frequency = (value))

#define iplay_sdl_audio_device_config_set_bits_per_sample_field(state, value) ((state)->bits_per_sample = (value))

#define iplay_sdl_audio_device_config_set_channels_field(state, value) ((state)->channels = (value))

#define iplay_sdl_audio_device_config_set_signed_samples_field(state, value) ((state)->signed_samples = (value))

#define iplay_sdl_audio_device_config_set_samples_field(state, value) ((state)->samples = (value))

#define iplay_sdl_audio_device_config_set_callback_field(state, value) ((state)->callback = (value))

#define iplay_sdl_audio_device_config_set_userdata_field(state, value) ((state)->userdata = (value))

#define iplay_sdl_audio_device_config_set_backend_field(state, value) ((state)->backend = (value))

#define iplay_sdl_audio_device_config_set_hardware_enabled_field(state, value) ((state)->hardware_enabled = (value))

#define iplay_sdl_audio_device_config_frequency_field(state) ((state)->frequency)
#define iplay_sdl_audio_device_config_bits_per_sample_field(state) ((state)->bits_per_sample)
#define iplay_sdl_audio_device_config_channels_field(state) ((state)->channels)
#define iplay_sdl_audio_device_config_signed_samples_field(state) ((state)->signed_samples)
#define iplay_sdl_audio_device_config_samples_field(state) ((state)->samples)
#define iplay_sdl_audio_device_config_callback_field(state) ((state)->callback)
#define iplay_sdl_audio_device_config_userdata_field(state) ((state)->userdata)
#define iplay_sdl_audio_device_config_backend_field(state) ((state)->backend)
#define iplay_sdl_audio_device_config_hardware_enabled_field(state) ((state)->hardware_enabled)

void iplay_sdl_audio_device_config_set_format(IplaySdlAudioDeviceConfig *config, const IplayAudioFormat *format) {
    iplay_sdl_audio_device_config_set_frequency_field(config, iplay_audio_format_sample_rate(format));
    iplay_sdl_audio_device_config_set_bits_per_sample_field(config, iplay_audio_format_bits_per_sample(format));
    iplay_sdl_audio_device_config_set_channels_field(config, iplay_audio_format_channels(format));
    iplay_sdl_audio_device_config_set_signed_samples_field(config, iplay_audio_format_signed_samples(format));
}

void iplay_sdl_audio_device_config_set_samples(IplaySdlAudioDeviceConfig *config, dw samples) {
    iplay_sdl_audio_device_config_set_samples_field(config, samples);
}

void iplay_sdl_audio_device_config_set_callback(IplaySdlAudioDeviceConfig *config, IplaySdlAudioCallback callback, void *userdata) {
    iplay_sdl_audio_device_config_set_callback_field(config, callback);
    iplay_sdl_audio_device_config_set_userdata_field(config, userdata);
}

void iplay_sdl_audio_device_config_set_backend(IplaySdlAudioDeviceConfig *config, IplayAudioBackend backend, db hardware_enabled) {
    iplay_sdl_audio_device_config_set_backend_field(config, backend);
    iplay_sdl_audio_device_config_set_hardware_enabled_field(config, hardware_enabled);
}

int iplay_sdl_audio_device_config_format(const IplaySdlAudioDeviceConfig *config, IplayAudioFormat *format) {
    return iplay_audio_make_source_format(format,
                                          iplay_sdl_audio_device_config_frequency(config),
                                          iplay_sdl_audio_device_config_bits_per_sample(config),
                                          iplay_sdl_audio_device_config_channels(config),
                                          iplay_sdl_audio_device_config_signed_samples(config));
}

dw iplay_sdl_audio_device_config_frequency(const IplaySdlAudioDeviceConfig *config) {
    return iplay_sdl_audio_device_config_frequency_field(config);
}

db iplay_sdl_audio_device_config_bits_per_sample(const IplaySdlAudioDeviceConfig *config) {
    return iplay_sdl_audio_device_config_bits_per_sample_field(config);
}

db iplay_sdl_audio_device_config_channels(const IplaySdlAudioDeviceConfig *config) {
    return iplay_sdl_audio_device_config_channels_field(config);
}

db iplay_sdl_audio_device_config_signed_samples(const IplaySdlAudioDeviceConfig *config) {
    return iplay_sdl_audio_device_config_signed_samples_field(config);
}

dw iplay_sdl_audio_device_config_samples(const IplaySdlAudioDeviceConfig *config) {
    return iplay_sdl_audio_device_config_samples_field(config);
}

IplaySdlAudioCallback iplay_sdl_audio_device_config_callback(const IplaySdlAudioDeviceConfig *config) {
    return iplay_sdl_audio_device_config_callback_field(config);
}

void *iplay_sdl_audio_device_config_userdata(const IplaySdlAudioDeviceConfig *config) {
    return iplay_sdl_audio_device_config_userdata_field(config);
}

IplayAudioBackend iplay_sdl_audio_device_config_backend(const IplaySdlAudioDeviceConfig *config) {
    return iplay_sdl_audio_device_config_backend_field(config);
}

int iplay_sdl_audio_device_config_hardware_enabled(const IplaySdlAudioDeviceConfig *config) {
    return iplay_sdl_audio_device_config_hardware_enabled_field(config) != 0;
}

int iplay_sdl_audio_device_config_is_sb16_stereo(const IplaySdlAudioDeviceConfig *config) {
    IplayAudioFormat format;
    if (!iplay_sdl_audio_device_config_format(config, &format)) return 0;
    return iplay_audio_format_equals(&format, &IPLAY_AUDIO_SB16_STEREO_16);
}

int iplay_sdl_audio_device_open(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config, IplayAudioWriteFn write, void *write_user) {
    if (!iplay_sdl_audio_device_config_is_sb16_stereo(config)) return 0;
    iplay_audio_output_init_sb16_stereo(iplay_sdl_audio_device_output(device), write, write_user);
    iplay_sdl_audio_device_finish_open(device, config);
    return 1;
}

#define iplay_sdl_audio_device_config_mut_field(state) (&(state)->config)
#define iplay_sdl_audio_device_config_field(state) (&(state)->config)
#define iplay_sdl_audio_device_output_field(state) (&(state)->output)
#define iplay_sdl_audio_device_output_const_field(state) (&(state)->output)

IplaySdlAudioDeviceConfig *iplay_sdl_audio_device_config_mut(IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_config_mut_field(device);
}

const IplaySdlAudioDeviceConfig *iplay_sdl_audio_device_config(const IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_config_field(device);
}

void iplay_sdl_audio_device_set_config(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config) {
    IplayAudioFormat format;
    IplaySdlAudioDeviceConfig *dst = iplay_sdl_audio_device_config_mut(device);
    (void)iplay_sdl_audio_device_config_format(config, &format);
    iplay_sdl_audio_device_config_set_format(dst, &format);
    iplay_sdl_audio_device_config_set_samples(dst, iplay_sdl_audio_device_config_samples(config));
    iplay_sdl_audio_device_config_set_callback(dst, iplay_sdl_audio_device_config_callback(config), iplay_sdl_audio_device_config_userdata(config));
    iplay_sdl_audio_device_config_set_backend(dst, iplay_sdl_audio_device_config_backend(config), (db)iplay_sdl_audio_device_config_hardware_enabled(config));
}

void iplay_sdl_audio_device_apply_config(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config) {
    iplay_sdl_audio_device_set_config(device, config);
    iplay_sdl_audio_device_set_backend(device, iplay_sdl_audio_device_config_backend(config));
    iplay_sdl_audio_device_set_hardware_enabled(device, iplay_sdl_audio_device_config_hardware_enabled(config));
}

void iplay_sdl_audio_device_finish_open(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config) {
    iplay_sdl_audio_device_apply_config(device, config);
    iplay_sdl_audio_device_set_paused(device, 1);
}

void iplay_sdl_audio_device_init_sb16_compatible(IplaySdlAudioDevice *device, IplayAudioWriteFn write, void *user) {
    IplaySdlAudioDeviceConfig config;
    iplay_sdl_audio_device_config_sb16_stereo(&config, device, IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);
    (void)iplay_sdl_audio_device_open(device, &config, write, user);
}

void iplay_sdl_audio_device_init_sb16_hardware(IplaySdlAudioDevice *device, IplayAudioWriteFn write, void *user) {
    IplaySdlAudioDeviceConfig config;
    iplay_sdl_audio_device_config_sb16_stereo(&config, device, IPLAY_AUDIO_BACKEND_SB16_STEREO, 1);
    (void)iplay_sdl_audio_device_open(device, &config, write, user);
}

IplaySdlAudioSpec iplay_sdl_audio_device_spec(const IplaySdlAudioDevice *device) {
    IplaySdlAudioSpec spec;
    iplay_sdl_audio_spec_set_backend_field(&spec, iplay_sdl_audio_device_backend(device));
    iplay_sdl_audio_spec_set_format_field(&spec, iplay_sdl_audio_device_format(device));
    iplay_sdl_audio_spec_set_hardware_enabled_field(&spec, (db)iplay_sdl_audio_device_hardware_enabled(device));
    return spec;
}

IplayAudioBackend iplay_sdl_audio_device_backend_raw(const IplaySdlAudioDevice *state) {
    return state->backend;
}

void iplay_sdl_audio_device_set_backend_raw(IplaySdlAudioDevice *state, IplayAudioBackend backend) {
    state->backend = backend;
}

IplayAudioBackend iplay_sdl_audio_device_backend(const IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_backend_raw(device);
}

void iplay_sdl_audio_device_set_backend(IplaySdlAudioDevice *device, IplayAudioBackend backend) {
    iplay_sdl_audio_device_set_backend_raw(device, backend);
}

const char *iplay_sdl_audio_device_backend_name(const IplaySdlAudioDevice *device) {
    return iplay_audio_backend_name(iplay_sdl_audio_device_backend(device));
}

IplayAudioOutput *iplay_sdl_audio_device_output(IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_output_field(device);
}

const IplayAudioOutput *iplay_sdl_audio_device_output_const(const IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_output_const_field(device);
}

const IplayAudioFormat *iplay_sdl_audio_device_format(const IplaySdlAudioDevice *device) {
    return iplay_audio_output_sink_format(iplay_sdl_audio_device_output_const(device));
}

dw iplay_sdl_audio_device_sample_rate(const IplaySdlAudioDevice *device) {
    return iplay_audio_format_sample_rate(iplay_sdl_audio_device_format(device));
}

db iplay_sdl_audio_device_bits_per_sample(const IplaySdlAudioDevice *device) {
    return iplay_audio_format_bits_per_sample(iplay_sdl_audio_device_format(device));
}

db iplay_sdl_audio_device_channels(const IplaySdlAudioDevice *device) {
    return iplay_audio_format_channels(iplay_sdl_audio_device_format(device));
}

db iplay_sdl_audio_device_signed_samples(const IplaySdlAudioDevice *device) {
    return iplay_audio_format_signed_samples(iplay_sdl_audio_device_format(device));
}

dw iplay_sdl_audio_device_bytes_per_frame(const IplaySdlAudioDevice *device) {
    return iplay_audio_output_bytes_per_frame(iplay_sdl_audio_device_output_const(device));
}

dw iplay_sdl_audio_device_samples(const IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_config_samples(iplay_sdl_audio_device_config(device));
}

IplaySdlAudioCallback iplay_sdl_audio_device_audio_callback(const IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_config_callback(iplay_sdl_audio_device_config(device));
}

void *iplay_sdl_audio_device_audio_userdata(const IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_config_userdata(iplay_sdl_audio_device_config(device));
}

int iplay_sdl_audio_device_is_sb16_compatible(const IplaySdlAudioDevice *device) {
    return iplay_audio_backend_is_sb16_scope(iplay_sdl_audio_device_backend(device))
        && iplay_audio_format_equals(iplay_sdl_audio_device_format(device), &IPLAY_AUDIO_SB16_STEREO_16);
}

int iplay_sdl_audio_device_is_sb16_hardware(const IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_hardware_enabled(device)
        && iplay_audio_backend_is_sb16_hardware(iplay_sdl_audio_device_backend(device))
        && iplay_audio_format_equals(iplay_sdl_audio_device_format(device), &IPLAY_AUDIO_SB16_STEREO_16);
}

int iplay_sdl_audio_device_is_sdl_compatible(const IplaySdlAudioDevice *device) {
    return !iplay_sdl_audio_device_hardware_enabled(device)
        && iplay_audio_backend_is_sdl_compatible(iplay_sdl_audio_device_backend(device))
        && iplay_audio_format_equals(iplay_sdl_audio_device_format(device), &IPLAY_AUDIO_SB16_STEREO_16);
}

db iplay_sdl_audio_device_hardware_enabled_flag(const IplaySdlAudioDevice *state) {
    return state->hardware_enabled;
}

void iplay_sdl_audio_device_set_hardware_enabled_flag(IplaySdlAudioDevice *state, db enabled) {
    state->hardware_enabled = enabled;
}

int iplay_sdl_audio_device_hardware_enabled(const IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_hardware_enabled_flag(device) != 0;
}

void iplay_sdl_audio_device_set_hardware_enabled(IplaySdlAudioDevice *device, int enabled) {
    iplay_sdl_audio_device_set_hardware_enabled_flag(device, enabled ? 1u : 0u);
}

const char *iplay_sdl_audio_device_status_text(const IplaySdlAudioDevice *device) {
    if (iplay_sdl_audio_device_hardware_enabled(device)) return "Playback enabled";
    return "Playback disabled in no-hardware build";
}

void iplay_sdl_audio_device_start(IplaySdlAudioDevice *device) {
    iplay_audio_output_start(iplay_sdl_audio_device_output(device));
    iplay_sdl_audio_device_set_paused(device, 0);
}

void iplay_sdl_audio_device_stop(IplaySdlAudioDevice *device) {
    iplay_audio_output_stop(iplay_sdl_audio_device_output(device));
    iplay_sdl_audio_device_set_paused(device, 1);
}

int iplay_sdl_audio_device_active(const IplaySdlAudioDevice *device) {
    return iplay_audio_output_is_active(iplay_sdl_audio_device_output_const(device));
}

void iplay_sdl_audio_device_pause(IplaySdlAudioDevice *device, db paused) {
    iplay_sdl_audio_device_set_paused(device, paused != 0);
    if (iplay_sdl_audio_device_paused(device)) {
        iplay_audio_output_stop(iplay_sdl_audio_device_output(device));
    } else {
        iplay_audio_output_start(iplay_sdl_audio_device_output(device));
    }
}

db iplay_sdl_audio_device_paused_flag(const IplaySdlAudioDevice *state) {
    return state->paused;
}

void iplay_sdl_audio_device_set_paused_flag(IplaySdlAudioDevice *state, db paused) {
    state->paused = paused;
}

int iplay_sdl_audio_device_paused(const IplaySdlAudioDevice *device) {
    return iplay_sdl_audio_device_paused_flag(device) != 0;
}

void iplay_sdl_audio_device_set_paused(IplaySdlAudioDevice *device, int paused) {
    iplay_sdl_audio_device_set_paused_flag(device, paused ? 1u : 0u);
}

void iplay_sdl_audio_device_reset_counters(IplaySdlAudioDevice *device) {
    iplay_audio_output_reset_counters(iplay_sdl_audio_device_output(device));
}

void iplay_sdl_audio_device_set_capacity(IplaySdlAudioDevice *device, dd capacity_frames) {
    iplay_audio_output_set_capacity(iplay_sdl_audio_device_output(device), capacity_frames);
}

void iplay_sdl_audio_device_add_capacity(IplaySdlAudioDevice *device, dd capacity_frames) {
    iplay_audio_output_add_capacity(iplay_sdl_audio_device_output(device), capacity_frames);
}

void iplay_sdl_audio_device_clear_queued(IplaySdlAudioDevice *device) {
    iplay_sdl_audio_device_set_capacity(device, 0);
}

dd iplay_sdl_audio_device_capacity(const IplaySdlAudioDevice *device) {
    return iplay_audio_output_capacity(iplay_sdl_audio_device_output_const(device));
}

dd iplay_sdl_audio_device_frames_written(const IplaySdlAudioDevice *device) {
    return iplay_audio_output_frames_written(iplay_sdl_audio_device_output_const(device));
}

dd iplay_sdl_audio_device_underrun_frames(const IplaySdlAudioDevice *device) {
    return iplay_audio_output_underrun_frames(iplay_sdl_audio_device_output_const(device));
}

dd iplay_sdl_audio_device_dropped_frames(const IplaySdlAudioDevice *device) {
    return iplay_audio_output_dropped_frames(iplay_sdl_audio_device_output_const(device));
}

dd iplay_sdl_audio_device_queued_frames(const IplaySdlAudioDevice *device) {
    if (device == 0) return 0;
    return iplay_sdl_audio_device_capacity(device);
}

dd iplay_sdl_audio_device_queued_bytes(const IplaySdlAudioDevice *device) {
    if (device == 0) return 0;
    return iplay_sdl_audio_device_queued_frames(device) * iplay_sdl_audio_device_bytes_per_frame(device);
}

dw iplay_sdl_audio_device_write_sb16_frames(IplaySdlAudioDevice *device, const db *pcm, dw frame_count) {
    return iplay_audio_output_write_sb16_frames(iplay_sdl_audio_device_output(device), pcm, frame_count);
}

int iplay_sdl_audio_device_can_queue(const IplaySdlAudioDevice *device) {
    return device != 0 && !iplay_sdl_audio_device_paused(device) && iplay_sdl_audio_device_bytes_per_frame(device) != 0;
}

dw iplay_sdl_audio_device_frames_for_bytes(const IplaySdlAudioDevice *device, dw byte_count) {
    dw frame_bytes;
    if (device == 0) return 0;
    frame_bytes = iplay_sdl_audio_device_bytes_per_frame(device);
    if (frame_bytes == 0) return 0;
    return (dw)(byte_count / frame_bytes);
}

dw iplay_sdl_audio_device_bytes_for_frames(const IplaySdlAudioDevice *device, dw frame_count) {
    if (device == 0) return 0;
    return (dw)(frame_count * iplay_sdl_audio_device_bytes_per_frame(device));
}

dw iplay_sdl_audio_device_callback(void *user, db *stream, dw byte_count) {
    IplaySdlAudioDevice *device = (IplaySdlAudioDevice *)user;
    dw frames;
    if (stream == 0) return 0;
    if (!iplay_sdl_audio_device_can_queue(device)) return 0;
    frames = iplay_sdl_audio_device_frames_for_bytes(device, byte_count);
    return iplay_sdl_audio_device_write_sb16_frames(device, stream, frames);
}

dw iplay_sdl_audio_device_queue(IplaySdlAudioDevice *device, const db *stream, dw byte_count) {
    dw frames;
    frames = iplay_sdl_audio_device_frames_for_bytes(device, byte_count);
    return iplay_sdl_audio_device_queue_frames(device, stream, frames);
}

dw iplay_sdl_audio_device_queue_frames(IplaySdlAudioDevice *device, const db *stream, dw frame_count) {
    if (device == 0 || stream == 0) return 0;
    if (!iplay_sdl_audio_device_can_queue(device)) return 0;
    if (frame_count == 0) return 0;
    iplay_sdl_audio_device_add_capacity(device, frame_count);
    return iplay_sdl_audio_device_callback(device, (db *)stream, iplay_sdl_audio_device_bytes_for_frames(device, frame_count));
}

void iplay_sdl_audio_device_write_silence(IplaySdlAudioDevice *device, dw frame_count) {
    iplay_audio_output_write_silence(iplay_sdl_audio_device_output(device), frame_count);
}

const IplayAudioLevels *iplay_sdl_audio_device_levels(const IplaySdlAudioDevice *device) {
    return iplay_audio_output_levels(iplay_sdl_audio_device_output_const(device));
}

void iplay_sdl_audio_device_reset_levels(IplaySdlAudioDevice *device) {
    iplay_audio_output_reset_levels(iplay_sdl_audio_device_output(device));
}

void iplay_runtime_init_vga_sb16(IplayRuntime *runtime, db *cells, const IplayTextMode *mode, IplayAudioWriteFn write, void *user) {
    iplay_notcurses_init_vga_memory_capacity(iplay_runtime_notcurses(runtime), cells, iplay_text_mode_screen_bytes(mode), mode);
    iplay_sdl_audio_device_init_sb16_hardware(iplay_runtime_audio(runtime), write, user);
    iplay_runtime_set_video_mode_ok(runtime, 1);
}

void iplay_runtime_init_vga_sdl_audio(IplayRuntime *runtime, db *cells, const IplayTextMode *mode, IplayAudioWriteFn audio_write, void *audio_user) {
    iplay_notcurses_init_vga_memory_capacity(iplay_runtime_notcurses(runtime), cells, iplay_text_mode_screen_bytes(mode), mode);
    iplay_sdl_audio_device_init_sb16_compatible(iplay_runtime_audio(runtime), audio_write, audio_user);
    iplay_runtime_set_video_mode_ok(runtime, 1);
}

void iplay_runtime_init_vga_sb16_present(IplayRuntime *runtime, db *cells, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn write, void *audio_user) {
    iplay_runtime_init_vga_sb16(runtime, cells, mode, write, audio_user);
    iplay_notcurses_set_present_callback(iplay_runtime_notcurses(runtime), present, present_user);
}

void iplay_runtime_init_callbacks(IplayRuntime *runtime, db *cells, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user) {
    iplay_runtime_init_callbacks_capacity(runtime, cells, iplay_text_mode_screen_bytes(mode), mode, present, present_user, audio_write, audio_user);
}

void iplay_runtime_init_callbacks_capacity(IplayRuntime *runtime, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user) {
    iplay_notcurses_init_vga_memory_capacity(iplay_runtime_notcurses(runtime), cells, cell_capacity_bytes, mode);
    iplay_sdl_audio_device_init_sb16_compatible(iplay_runtime_audio(runtime), audio_write, audio_user);
    iplay_notcurses_set_present_callback(iplay_runtime_notcurses(runtime), present, present_user);
    iplay_runtime_set_video_mode_ok(runtime, 1);
}

static void iplay_runtime_init_config_audio(IplayRuntime *runtime, const IplayRuntimeConfig *config) {
    iplay_notcurses_init_vga_memory_capacity(iplay_runtime_notcurses(runtime), iplay_runtime_config_cells(config), iplay_runtime_config_cell_capacity(config), iplay_runtime_config_mode(config));
    if (iplay_runtime_config_uses_sb16_hardware(config)) {
        iplay_sdl_audio_device_init_sb16_hardware(iplay_runtime_audio(runtime), iplay_runtime_config_audio_write(config), iplay_runtime_config_audio_user(config));
    } else {
        iplay_sdl_audio_device_init_sb16_compatible(iplay_runtime_audio(runtime), iplay_runtime_config_audio_write(config), iplay_runtime_config_audio_user(config));
    }
    iplay_notcurses_set_present_callback(iplay_runtime_notcurses(runtime), iplay_runtime_config_present(config), iplay_runtime_config_present_user(config));
    iplay_runtime_set_video_mode_ok(runtime, 1);
}

void iplay_runtime_init_config(IplayRuntime *runtime, const IplayRuntimeConfig *config) {
    if (!iplay_runtime_config_is_valid(config)) {
        iplay_runtime_init_callbacks_capacity(runtime, iplay_runtime_fallback_cells, IPLAY_TEXT_FALLBACK_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, 0, 0, 0, 0);
        return;
    }
    iplay_runtime_init_config_audio(runtime, config);
}

const IplayTextMode *iplay_runtime_start_config(IplayRuntime *runtime, const IplayRuntimeConfig *config, db video_mode) {
    iplay_runtime_init_config(runtime, config);
    iplay_runtime_audio_start(runtime);
    return iplay_runtime_set_video_mode(runtime, video_mode);
}

int iplay_runtime_start_config_checked(IplayRuntime *runtime, const IplayRuntimeConfig *config, db video_mode) {
    iplay_runtime_init_config(runtime, config);
    iplay_runtime_audio_start(runtime);
    return iplay_runtime_set_video_mode_checked(runtime, video_mode);
}

void iplay_runtime_config_no_hardware(IplayRuntimeConfig *config, db *cells, const IplayTextMode *mode, IplayAudioWriteFn audio_write, void *audio_user) {
    iplay_runtime_config_no_hardware_capacity(config, cells, iplay_text_mode_screen_bytes(mode), mode, audio_write, audio_user);
}

#define iplay_runtime_output_spec_set_video_backend_field(state, value) ((state)->video_backend = (value))

#define iplay_runtime_output_spec_set_audio_backend_field(state, value) ((state)->audio_backend = (value))

#define iplay_runtime_output_spec_set_audio_hardware_enabled_field(state, value) ((state)->audio_hardware_enabled = (value))

#define iplay_runtime_output_spec_video_backend_field(state) ((state)->video_backend)
#define iplay_runtime_output_spec_audio_backend_field(state) ((state)->audio_backend)
#define iplay_runtime_output_spec_audio_hardware_enabled_field(state) ((state)->audio_hardware_enabled)

void iplay_runtime_output_spec_init(IplayRuntimeOutputSpec *spec, IplayTerminalBackend video_backend, IplayAudioBackend audio_backend, db audio_hardware_enabled) {
    iplay_runtime_output_spec_set_video_backend_field(spec, video_backend);
    iplay_runtime_output_spec_set_audio_backend_field(spec, audio_backend);
    iplay_runtime_output_spec_set_audio_hardware_enabled_field(spec, audio_hardware_enabled);
}

IplayTerminalBackend iplay_runtime_output_spec_video_backend(const IplayRuntimeOutputSpec *spec) {
    return iplay_runtime_output_spec_video_backend_field(spec);
}

IplayAudioBackend iplay_runtime_output_spec_audio_backend(const IplayRuntimeOutputSpec *spec) {
    return iplay_runtime_output_spec_audio_backend_field(spec);
}

int iplay_runtime_output_spec_audio_hardware_enabled(const IplayRuntimeOutputSpec *spec) {
    return iplay_runtime_output_spec_audio_hardware_enabled_field(spec) != 0;
}

void iplay_runtime_output_spec_sdl(IplayRuntimeOutputSpec *spec) {
    iplay_runtime_output_spec_init(spec, IPLAY_TERMINAL_BACKEND_VGA_MEMORY, IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);
}

void iplay_runtime_output_spec_sb16_hardware(IplayRuntimeOutputSpec *spec) {
    iplay_runtime_output_spec_init(spec, IPLAY_TERMINAL_BACKEND_VGA_MEMORY, IPLAY_AUDIO_BACKEND_SB16_STEREO, 1);
}

void iplay_runtime_config_output_capacity(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user, const IplayRuntimeOutputSpec *output) {
    iplay_runtime_config_set_video(config, cells, cell_capacity_bytes, mode, iplay_runtime_output_spec_video_backend(output), present, present_user);
    iplay_runtime_config_set_audio(config, audio_write, audio_user, iplay_runtime_output_spec_audio_backend(output), (db)iplay_runtime_output_spec_audio_hardware_enabled(output));
}

#define iplay_runtime_config_set_cells_field(state, value) ((state)->cells = (value))

#define iplay_runtime_config_set_cell_capacity_field(state, value) ((state)->cell_capacity_bytes = (value))

#define iplay_runtime_config_set_mode_field(state, value) ((state)->mode = (value))

#define iplay_runtime_config_set_present_field(state, value) ((state)->present = (value))

#define iplay_runtime_config_set_present_user_field(state, value) ((state)->present_user = (value))

#define iplay_runtime_config_set_video_present_enabled_field(state, value) ((state)->video_present_enabled = (value))

#define iplay_runtime_config_set_video_backend_field(state, value) ((state)->video_backend = (value))

#define iplay_runtime_config_cells_field(state) ((state)->cells)
#define iplay_runtime_config_cell_capacity_field(state) ((state)->cell_capacity_bytes)
#define iplay_runtime_config_mode_field(state) ((state)->mode)
#define iplay_runtime_config_video_backend_field(state) ((state)->video_backend)
#define iplay_runtime_config_present_field(state) ((state)->present)
#define iplay_runtime_config_present_user_field(state) ((state)->present_user)
#define iplay_runtime_config_video_present_enabled_field(state) ((state)->video_present_enabled)

#define iplay_runtime_config_set_audio_write_field(state, value) ((state)->audio_write = (value))

#define iplay_runtime_config_set_audio_user_field(state, value) ((state)->audio_user = (value))

#define iplay_runtime_config_set_audio_backend_field(state, value) ((state)->audio_backend = (value))

#define iplay_runtime_config_set_audio_hardware_enabled_field(state, value) ((state)->audio_hardware_enabled = (value))

#define iplay_runtime_config_audio_write_field(state) ((state)->audio_write)
#define iplay_runtime_config_audio_user_field(state) ((state)->audio_user)
#define iplay_runtime_config_audio_backend_field(state) ((state)->audio_backend)
#define iplay_runtime_config_audio_hardware_enabled_field(state) ((state)->audio_hardware_enabled)

void iplay_runtime_config_set_video_memory(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode) {
    iplay_runtime_config_set_cells_field(config, cells);
    iplay_runtime_config_set_cell_capacity_field(config, cell_capacity_bytes);
    iplay_runtime_config_set_mode_field(config, mode);
}

void iplay_runtime_config_set_video_present(IplayRuntimeConfig *config, IplayVideoPresentFn present, void *present_user) {
    iplay_runtime_config_set_present_field(config, present);
    iplay_runtime_config_set_present_user_field(config, present_user);
    iplay_runtime_config_set_video_present_enabled_field(config, present != 0);
}

void iplay_runtime_config_set_video_backend(IplayRuntimeConfig *config, IplayTerminalBackend backend) {
    iplay_runtime_config_set_video_backend_field(config, backend);
}

void iplay_runtime_config_set_video(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayTerminalBackend backend, IplayVideoPresentFn present, void *present_user) {
    iplay_runtime_config_set_video_memory(config, cells, cell_capacity_bytes, mode);
    iplay_runtime_config_set_video_present(config, present, present_user);
    iplay_runtime_config_set_video_backend(config, backend);
}

void iplay_runtime_config_set_audio_sink(IplayRuntimeConfig *config, IplayAudioWriteFn audio_write, void *audio_user) {
    iplay_runtime_config_set_audio_write_field(config, audio_write);
    iplay_runtime_config_set_audio_user_field(config, audio_user);
}

void iplay_runtime_config_set_audio_backend(IplayRuntimeConfig *config, IplayAudioBackend backend, db hardware_enabled) {
    iplay_runtime_config_set_audio_backend_field(config, backend);
    iplay_runtime_config_set_audio_hardware_enabled_field(config, hardware_enabled);
}

void iplay_runtime_config_set_audio(IplayRuntimeConfig *config, IplayAudioWriteFn audio_write, void *audio_user, IplayAudioBackend backend, db hardware_enabled) {
    iplay_runtime_config_set_audio_sink(config, audio_write, audio_user);
    iplay_runtime_config_set_audio_backend(config, backend, hardware_enabled);
}

void iplay_runtime_config_no_hardware_capacity(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayAudioWriteFn audio_write, void *audio_user) {
    IplayRuntimeOutputSpec output;
    iplay_runtime_output_spec_sdl(&output);
    iplay_runtime_config_output_capacity(config, cells, cell_capacity_bytes, mode, 0, 0, audio_write, audio_user, &output);
}

void iplay_runtime_config_sdl(IplayRuntimeConfig *config, db *cells, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user) {
    iplay_runtime_config_sdl_capacity(config, cells, iplay_text_mode_screen_bytes(mode), mode, present, present_user, audio_write, audio_user);
}

void iplay_runtime_config_sdl_capacity(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user) {
    IplayRuntimeOutputSpec output;
    iplay_runtime_output_spec_sdl(&output);
    iplay_runtime_config_output_capacity(config, cells, cell_capacity_bytes, mode, present, present_user, audio_write, audio_user, &output);
}

void iplay_runtime_config_sb16_hardware(IplayRuntimeConfig *config, db *cells, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user) {
    iplay_runtime_config_sb16_hardware_capacity(config, cells, iplay_text_mode_screen_bytes(mode), mode, present, present_user, audio_write, audio_user);
}

void iplay_runtime_config_sb16_hardware_capacity(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user) {
    IplayRuntimeOutputSpec output;
    iplay_runtime_output_spec_sb16_hardware(&output);
    iplay_runtime_config_output_capacity(config, cells, cell_capacity_bytes, mode, present, present_user, audio_write, audio_user, &output);
}

int iplay_runtime_config_has_video_present(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_video_present_enabled(config) && iplay_runtime_config_present(config) != 0;
}

int iplay_runtime_config_has_audio_sink(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_audio_write(config) != 0;
}

int iplay_runtime_config_has_cell_capacity(const IplayRuntimeConfig *config) {
    return iplay_text_mode_fits_capacity(iplay_runtime_config_mode(config), iplay_runtime_config_cell_capacity(config));
}

db *iplay_runtime_config_cells(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_cells_field(config);
}

dw iplay_runtime_config_cell_capacity(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_cell_capacity_field(config);
}

const IplayTextMode *iplay_runtime_config_mode(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_mode_field(config);
}

IplayTerminalBackend iplay_runtime_config_video_backend(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_video_backend_field(config);
}

IplayVideoPresentFn iplay_runtime_config_present(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_present_field(config);
}

void *iplay_runtime_config_present_user(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_present_user_field(config);
}

int iplay_runtime_config_video_present_enabled(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_video_present_enabled_field(config) != 0;
}

IplayAudioWriteFn iplay_runtime_config_audio_write(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_audio_write_field(config);
}

void *iplay_runtime_config_audio_user(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_audio_user_field(config);
}

IplayAudioBackend iplay_runtime_config_audio_backend(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_audio_backend_field(config);
}

int iplay_runtime_config_audio_hardware_enabled(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_audio_hardware_enabled_field(config) != 0;
}

int iplay_runtime_config_uses_sb16_hardware(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_audio_hardware_enabled(config) || iplay_audio_backend_is_sb16_hardware(iplay_runtime_config_audio_backend(config));
}

db iplay_runtime_config_error(const IplayRuntimeConfig *config) {
    if (iplay_runtime_config_cells(config) == 0) return IPLAY_RUNTIME_CONFIG_MISSING_CELLS;
    if (iplay_runtime_config_mode(config) == 0) return IPLAY_RUNTIME_CONFIG_MISSING_MODE;
    if (!iplay_runtime_config_has_cell_capacity(config)) return IPLAY_RUNTIME_CONFIG_SMALL_CELLS;
    if (!iplay_runtime_config_has_audio_sink(config)) return IPLAY_RUNTIME_CONFIG_MISSING_AUDIO;
    return IPLAY_RUNTIME_CONFIG_OK;
}

const char *iplay_runtime_config_error_name(db error) {
    switch (error) {
    case IPLAY_RUNTIME_CONFIG_OK:
        return "ok";
    case IPLAY_RUNTIME_CONFIG_MISSING_CELLS:
        return "missing-cells";
    case IPLAY_RUNTIME_CONFIG_MISSING_MODE:
        return "missing-mode";
    case IPLAY_RUNTIME_CONFIG_MISSING_AUDIO:
        return "missing-audio";
    case IPLAY_RUNTIME_CONFIG_SMALL_CELLS:
        return "small-cells";
    default:
        return "unknown";
    }
}

int iplay_runtime_config_is_valid(const IplayRuntimeConfig *config) {
    return iplay_runtime_config_error(config) == IPLAY_RUNTIME_CONFIG_OK;
}

void iplay_runtime_shutdown(IplayRuntime *runtime) {
    iplay_runtime_audio_stop(runtime);
    iplay_runtime_audio_reset_levels(runtime);
}

#define iplay_runtime_notcurses_field(state) (&(state)->nc)
#define iplay_runtime_notcurses_const_field(state) (&(state)->nc)
#define iplay_runtime_audio_field(state) (&(state)->audio)
#define iplay_runtime_audio_const_field(state) (&(state)->audio)

IplayNotcurses *iplay_runtime_notcurses(IplayRuntime *runtime) {
    return iplay_runtime_notcurses_field(runtime);
}

const IplayNotcurses *iplay_runtime_notcurses_const(const IplayRuntime *runtime) {
    return iplay_runtime_notcurses_const_field(runtime);
}

IplayTerminal *iplay_runtime_terminal(IplayRuntime *runtime) {
    return iplay_notcurses_terminal(iplay_runtime_notcurses(runtime));
}

const IplayTerminal *iplay_runtime_terminal_const(const IplayRuntime *runtime) {
    return iplay_notcurses_terminal_const(iplay_runtime_notcurses_const(runtime));
}

IplaySdlAudioDevice *iplay_runtime_audio(IplayRuntime *runtime) {
    return iplay_runtime_audio_field(runtime);
}

const IplaySdlAudioDevice *iplay_runtime_audio_const(const IplayRuntime *runtime) {
    return iplay_runtime_audio_const_field(runtime);
}

IplayNcPlane *iplay_runtime_stdplane(IplayRuntime *runtime) {
    return iplay_notcurses_stdplane(iplay_runtime_notcurses(runtime));
}

IplayVideoSpec iplay_runtime_video_spec(const IplayRuntime *runtime) {
    return iplay_notcurses_video_spec(iplay_runtime_notcurses_const(runtime));
}

IplayTerminalBackend iplay_runtime_video_backend(const IplayRuntime *runtime) {
    IplayVideoSpec spec = iplay_runtime_video_spec(runtime);
    return iplay_video_spec_backend(&spec);
}

int iplay_runtime_video_present_enabled(const IplayRuntime *runtime) {
    IplayVideoSpec spec = iplay_runtime_video_spec(runtime);
    return iplay_video_spec_present_enabled(&spec);
}

int iplay_runtime_video_has_present(const IplayRuntime *runtime) {
    return iplay_notcurses_has_present(iplay_runtime_notcurses_const(runtime));
}

IplayVideoPresentFn iplay_runtime_video_present_callback(const IplayRuntime *runtime) {
    return iplay_notcurses_present_callback(iplay_runtime_notcurses_const(runtime));
}

void *iplay_runtime_video_present_user(const IplayRuntime *runtime) {
    return iplay_notcurses_present_user(iplay_runtime_notcurses_const(runtime));
}

void iplay_runtime_video_set_present_fn(IplayRuntime *runtime, IplayVideoPresentFn present) {
    iplay_notcurses_set_present_fn(iplay_runtime_notcurses(runtime), present);
}

void iplay_runtime_video_set_present_user(IplayRuntime *runtime, void *user) {
    iplay_notcurses_set_present_user(iplay_runtime_notcurses(runtime), user);
}

void iplay_runtime_video_set_present_callback(IplayRuntime *runtime, IplayVideoPresentFn present, void *user) {
    iplay_runtime_video_set_present_fn(runtime, present);
    iplay_runtime_video_set_present_user(runtime, user);
}

void iplay_runtime_video_clear_present_callback(IplayRuntime *runtime) {
    iplay_runtime_video_set_present_callback(runtime, 0, 0);
}

const IplayTextMode *iplay_runtime_video_mode(const IplayRuntime *runtime) {
    return iplay_notcurses_mode(iplay_runtime_notcurses_const(runtime));
}

const db *iplay_runtime_video_cells_const(const IplayRuntime *runtime) {
    return iplay_terminal_cells_const(iplay_runtime_terminal_const(runtime));
}

dd iplay_runtime_video_checksum(const IplayRuntime *runtime) {
    return iplay_text_cells_checksum(iplay_runtime_video_cells_const(runtime), iplay_runtime_video_screen_bytes(runtime));
}

dw iplay_runtime_video_nonblank_cells(const IplayRuntime *runtime) {
    return iplay_text_cells_nonblank_count(iplay_runtime_video_cells_const(runtime), iplay_runtime_video_screen_bytes(runtime));
}

dw iplay_runtime_video_capacity(const IplayRuntime *runtime) {
    return iplay_notcurses_capacity(iplay_runtime_notcurses_const(runtime));
}

dw iplay_runtime_video_cols(const IplayRuntime *runtime) {
    return iplay_notcurses_cols(iplay_runtime_notcurses_const(runtime));
}

dw iplay_runtime_video_rows(const IplayRuntime *runtime) {
    return iplay_notcurses_rows(iplay_runtime_notcurses_const(runtime));
}

dw iplay_runtime_video_row_bytes(const IplayRuntime *runtime) {
    return iplay_notcurses_row_bytes(iplay_runtime_notcurses_const(runtime));
}

dw iplay_runtime_video_screen_bytes(const IplayRuntime *runtime) {
    return iplay_notcurses_screen_bytes(iplay_runtime_notcurses_const(runtime));
}

int iplay_runtime_bottom_layout_fits(const IplayRuntime *runtime) {
    return iplay_notcurses_bottom_layout_fits(iplay_runtime_notcurses_const(runtime));
}

IplaySdlAudioSpec iplay_runtime_audio_spec(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_spec(iplay_runtime_audio_const(runtime));
}

IplayAudioBackend iplay_runtime_audio_backend(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_backend(iplay_runtime_audio_const(runtime));
}

const IplayAudioFormat *iplay_runtime_audio_format(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_format(iplay_runtime_audio_const(runtime));
}

dw iplay_runtime_audio_sample_rate(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_sample_rate(iplay_runtime_audio_const(runtime));
}

db iplay_runtime_audio_bits_per_sample(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_bits_per_sample(iplay_runtime_audio_const(runtime));
}

db iplay_runtime_audio_channels(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_channels(iplay_runtime_audio_const(runtime));
}

db iplay_runtime_audio_signed_samples(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_signed_samples(iplay_runtime_audio_const(runtime));
}

dw iplay_runtime_audio_samples(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_samples(iplay_runtime_audio_const(runtime));
}

const char *iplay_runtime_audio_backend_name(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_backend_name(iplay_runtime_audio_const(runtime));
}

int iplay_runtime_audio_hardware_enabled(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_hardware_enabled(iplay_runtime_audio_const(runtime));
}

const char *iplay_runtime_audio_status_text(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_status_text(iplay_runtime_audio_const(runtime));
}

dw iplay_runtime_audio_bytes_per_frame(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_bytes_per_frame(iplay_runtime_audio_const(runtime));
}

int iplay_runtime_audio_is_sb16_compatible(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_is_sb16_compatible(iplay_runtime_audio_const(runtime));
}

int iplay_runtime_audio_is_sb16_hardware(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_is_sb16_hardware(iplay_runtime_audio_const(runtime));
}

int iplay_runtime_audio_is_sdl_compatible(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_is_sdl_compatible(iplay_runtime_audio_const(runtime));
}

const IplayTextMode *iplay_runtime_resize(IplayRuntime *runtime, const IplayTextMode *mode) {
    iplay_runtime_set_video_mode_ok(runtime, iplay_notcurses_resize_checked(iplay_runtime_notcurses(runtime), mode));
    return iplay_notcurses_mode(iplay_runtime_notcurses(runtime));
}

int iplay_runtime_resize_checked(IplayRuntime *runtime, const IplayTextMode *mode) {
    iplay_runtime_set_video_mode_ok(runtime, iplay_notcurses_resize_checked(iplay_runtime_notcurses(runtime), mode));
    return iplay_runtime_video_mode_ok(runtime);
}

const IplayTextMode *iplay_runtime_resize_to_size(IplayRuntime *runtime, dw cols, dw rows) {
    (void)iplay_runtime_resize_to_size_checked(runtime, cols, rows);
    return iplay_runtime_video_mode(runtime);
}

int iplay_runtime_resize_to_size_checked(IplayRuntime *runtime, dw cols, dw rows) {
    const IplayTextMode *mode = iplay_text_mode_for_size(cols, rows);
    return iplay_runtime_resize_checked(runtime, mode);
}

const IplayTextMode *iplay_runtime_set_video_mode(IplayRuntime *runtime, db video_mode) {
    iplay_runtime_set_video_mode_ok(runtime, iplay_notcurses_set_video_mode_checked(iplay_runtime_notcurses(runtime), video_mode));
    return iplay_notcurses_mode(iplay_runtime_notcurses(runtime));
}

int iplay_runtime_set_video_mode_checked(IplayRuntime *runtime, db video_mode) {
    iplay_runtime_set_video_mode_ok(runtime, iplay_notcurses_set_video_mode_checked(iplay_runtime_notcurses(runtime), video_mode));
    return iplay_runtime_video_mode_ok(runtime);
}

#define iplay_runtime_set_video_mode_ok_field(state, value) ((state)->video_mode_ok = (value))

#define iplay_runtime_video_mode_ok_field(state) ((state)->video_mode_ok)

void iplay_runtime_set_video_mode_ok_flag(IplayRuntime *runtime, db ok) {
    iplay_runtime_set_video_mode_ok_field(runtime, ok);
}

void iplay_runtime_set_video_mode_ok(IplayRuntime *runtime, int ok) {
    iplay_runtime_set_video_mode_ok_flag(runtime, ok ? 1u : 0u);
}

db iplay_runtime_video_mode_ok_flag(const IplayRuntime *runtime) {
    return iplay_runtime_video_mode_ok_field(runtime);
}

int iplay_runtime_video_mode_ok(const IplayRuntime *runtime) {
    return iplay_runtime_video_mode_ok_flag(runtime) != 0;
}

const char *iplay_runtime_video_status_text(const IplayRuntime *runtime) {
    if (iplay_runtime_video_mode_ok(runtime)) return "Video mode accepted";
    return "Video mode rejected";
}

const char *iplay_runtime_video_status_token(const IplayRuntime *runtime) {
    if (iplay_runtime_video_mode_ok(runtime)) return "accepted";
    return "rejected";
}

void iplay_runtime_render_static(IplayRuntime *runtime, db erase_attr) {
    iplay_notcurses_render_static(iplay_runtime_notcurses(runtime), erase_attr);
}

void iplay_runtime_render_bottom(IplayRuntime *runtime, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif) {
    iplay_notcurses_render_bottom(iplay_runtime_notcurses(runtime), byte_1de72, byte_1de73, byte_1de74, byte_1de75, byte_1de76, flags, volume, amplif);
}

void iplay_runtime_audio_start(IplayRuntime *runtime) {
    iplay_sdl_audio_device_start(iplay_runtime_audio(runtime));
}

void iplay_runtime_audio_stop(IplayRuntime *runtime) {
    iplay_sdl_audio_device_stop(iplay_runtime_audio(runtime));
}

int iplay_runtime_audio_active(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_active(iplay_runtime_audio_const(runtime));
}

void iplay_runtime_audio_pause(IplayRuntime *runtime, db paused) {
    iplay_sdl_audio_device_pause(iplay_runtime_audio(runtime), paused);
}

int iplay_runtime_audio_paused(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_paused(iplay_runtime_audio_const(runtime));
}

void iplay_runtime_audio_reset_counters(IplayRuntime *runtime) {
    iplay_sdl_audio_device_reset_counters(iplay_runtime_audio(runtime));
}

void iplay_runtime_audio_set_capacity(IplayRuntime *runtime, dd capacity_frames) {
    iplay_sdl_audio_device_set_capacity(iplay_runtime_audio(runtime), capacity_frames);
}

void iplay_runtime_audio_add_capacity(IplayRuntime *runtime, dd capacity_frames) {
    iplay_sdl_audio_device_add_capacity(iplay_runtime_audio(runtime), capacity_frames);
}

void iplay_runtime_audio_clear_queued(IplayRuntime *runtime) {
    iplay_sdl_audio_device_clear_queued(iplay_runtime_audio(runtime));
}

dd iplay_runtime_audio_capacity(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_capacity(iplay_runtime_audio_const(runtime));
}

dd iplay_runtime_audio_frames_written(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_frames_written(iplay_runtime_audio_const(runtime));
}

dd iplay_runtime_audio_underrun_frames(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_underrun_frames(iplay_runtime_audio_const(runtime));
}

dd iplay_runtime_audio_dropped_frames(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_dropped_frames(iplay_runtime_audio_const(runtime));
}

dd iplay_runtime_audio_queued_frames(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_queued_frames(iplay_runtime_audio_const(runtime));
}

dd iplay_runtime_audio_queued_bytes(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_queued_bytes(iplay_runtime_audio_const(runtime));
}

int iplay_runtime_audio_can_queue(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_can_queue(iplay_runtime_audio_const(runtime));
}

dw iplay_runtime_audio_frames_for_bytes(const IplayRuntime *runtime, dw byte_count) {
    return iplay_sdl_audio_device_frames_for_bytes(iplay_runtime_audio_const(runtime), byte_count);
}

dw iplay_runtime_audio_bytes_for_frames(const IplayRuntime *runtime, dw frame_count) {
    return iplay_sdl_audio_device_bytes_for_frames(iplay_runtime_audio_const(runtime), frame_count);
}

dw iplay_runtime_audio_queue(IplayRuntime *runtime, const db *pcm, dw byte_count) {
    return iplay_sdl_audio_device_queue(iplay_runtime_audio(runtime), pcm, byte_count);
}

dw iplay_runtime_audio_queue_frames(IplayRuntime *runtime, const db *pcm, dw frame_count) {
    return iplay_sdl_audio_device_queue_frames(iplay_runtime_audio(runtime), pcm, frame_count);
}

dw iplay_runtime_write_sb16_frames(IplayRuntime *runtime, const db *pcm, dw frame_count) {
    return iplay_sdl_audio_device_write_sb16_frames(iplay_runtime_audio(runtime), pcm, frame_count);
}

void iplay_runtime_write_silence(IplayRuntime *runtime, dw frame_count) {
    iplay_sdl_audio_device_write_silence(iplay_runtime_audio(runtime), frame_count);
}

const IplayAudioLevels *iplay_runtime_audio_levels(const IplayRuntime *runtime) {
    return iplay_sdl_audio_device_levels(iplay_runtime_audio_const(runtime));
}

void iplay_runtime_audio_reset_levels(IplayRuntime *runtime) {
    iplay_sdl_audio_device_reset_levels(iplay_runtime_audio(runtime));
}

void iplay_runtime_draw_audio_levels(IplayRuntime *runtime, dw y, dw x, dw width) {
    iplay_notcurses_draw_audio_output_levels(iplay_runtime_notcurses(runtime), y, x, iplay_sdl_audio_device_output(iplay_runtime_audio(runtime)), width, 0xdb, 0xb0, 0x2a, 0x4c, 0x08);
}

void iplay_runtime_draw_live_audio_levels(IplayRuntime *runtime) {
    iplay_runtime_draw_status_field(runtime,
                                    IPLAY_RUNTIME_STATUS_LEVELS_ROW,
                                    "Output Levels ",
                                    "",
                                    IPLAY_RUNTIME_STATUS_LABEL_ATTR,
                                    IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_audio_levels(runtime,
                                    IPLAY_RUNTIME_STATUS_LEVELS_ROW,
                                    IPLAY_RUNTIME_STATUS_LEVELS_X,
                                    IPLAY_RUNTIME_STATUS_LEVELS_WIDTH);
}

static int iplay_runtime_original_channel_storage_y(
    const IplayRuntime *runtime,
    dw channel,
    dw *y) {
    dw storage_y = channel < 10u ? (dw)(6u + channel) : (dw)(28u + channel - 10u);
    if (!y || storage_y >= iplay_runtime_video_rows(runtime)) return 0;
    *y = storage_y;
    return 1;
}

void iplay_runtime_draw_original_channel_levels(IplayRuntime *runtime, dw channel_count) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    const IplayAudioLevels *levels = iplay_runtime_audio_levels(runtime);
    dw channel;
    if (iplay_runtime_video_cols(runtime) < 80u) {
        iplay_runtime_draw_live_audio_levels(runtime);
        return;
    }
    for (channel = 0u; channel < channel_count; ++channel) {
        dw y;
        dw level = (channel & 1u) ? iplay_audio_levels_right_16(levels) : iplay_audio_levels_left_16(levels);
        dw filled = (dw)(level * 2u);
        db channel_label = channel < 9u ? (db)('1' + (db)channel) : (db)('A' + (db)(channel - 9u));
        dw x;
        if (!iplay_runtime_original_channel_storage_y(runtime, channel, &y)) continue;
        if (filled > 30u) filled = 30u;
        iplay_ncplane_putc_yx(plane, y, 0u, 0xb3u, 0x7fu);
        iplay_ncplane_putc_yx(plane, y, 1u, 0xb3u, 0x78u);
        iplay_ncplane_putc_yx(plane, y, 2u, ' ', channel == 0u ? 0x1eu : 0x7eu);
        iplay_ncplane_putc_yx(plane, y, 3u, channel_label, channel == 0u ? 0x1eu : 0x7eu);
        iplay_ncplane_putc_yx(plane, y, 4u, ' ', channel == 0u ? 0x1eu : 0x7eu);
        for (x = 5u; x <= 7u; ++x) iplay_ncplane_putc_yx(plane, y, x, ' ', 0x7fu);
        for (x = 8u; x <= 31u; ++x) iplay_ncplane_putc_yx(plane, y, x, ' ', 0x78u);
        for (x = 0u; x < 30u; ++x) {
            db attr = 0x78u;
            if (x < filled) attr = x < 13u ? 0x7au : (x < 25u ? 0x7eu : 0x7cu);
            iplay_ncplane_putc_yx(plane, y, (dw)(32u + x), 0x16u, attr);
        }
        iplay_ncplane_putc_yx(plane, y, 62u, ' ', 0x78u);
        for (x = 63u; x <= 77u; ++x) iplay_ncplane_putc_yx(plane, y, x, ' ', 0x7eu);
        iplay_ncplane_putc_yx(plane, y, 78u, 0xb3u, 0x7fu);
        iplay_ncplane_putc_yx(plane, y, 79u, 0xb3u, 0x78u);
    }
}

void iplay_runtime_draw_original_channel_level(IplayRuntime *runtime, dw channel, dw level) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    dw y;
    dw filled;
    dw x;
    if (iplay_runtime_video_cols(runtime) < 80u
        || !iplay_runtime_original_channel_storage_y(runtime, channel, &y)) return;
    if (level > 30u) level = 30u;
    filled = level;
    for (x = 0u; x < 30u; ++x) {
        db attr = 0x78u;
        if (x < filled) attr = x < 13u ? 0x7au : (x < 25u ? 0x7eu : 0x7cu);
        iplay_ncplane_putc_yx(plane, y, (dw)(32u + x), 0x16u, attr);
    }
}

void iplay_runtime_set_audio_levels(IplayRuntime *runtime, dw left_peak, dw right_peak) {
    IplayAudioLevels *levels = iplay_audio_output_levels_mut(iplay_sdl_audio_device_output(iplay_runtime_audio(runtime)));
    if (left_peak > 15u) left_peak = 15u;
    if (right_peak > 15u) right_peak = 15u;
    iplay_audio_levels_set(levels, (dw)(left_peak * 2048u), (dw)(right_peak * 2048u));
}

void iplay_runtime_draw_original_channel_text(IplayRuntime *runtime, dw channel, const char *note, const char *sample, const char *effect) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    dw y;
    dw i;
    int text_ended = 0;
    if (iplay_runtime_video_cols(runtime) < 80u
        || !iplay_runtime_original_channel_storage_y(runtime, channel, &y)) return;
    for (i = 0u; i < 3u; ++i) {
        db ch = !text_ended && note && note[i] ? (db)note[i] : ' ';
        if (!text_ended && (!note || !note[i])) text_ended = 1;
        iplay_ncplane_putc_yx(plane, y, (dw)(5u + i), ch, 0x7fu);
    }
    text_ended = 0;
    for (i = 0u; i < 22u; ++i) {
        db ch = !text_ended && sample && sample[i] ? (db)sample[i] : ' ';
        if (!text_ended && (!sample || !sample[i])) text_ended = 1;
        iplay_ncplane_putc_yx(plane, y, (dw)(9u + i), ch, 0x7bu);
    }
    text_ended = 0;
    for (i = 0u; i < 15u; ++i) {
        db ch = !text_ended && effect && effect[i] ? (db)effect[i] : ' ';
        if (!text_ended && (!effect || !effect[i])) text_ended = 1;
        iplay_ncplane_putc_yx(plane, y, (dw)(63u + i), ch, 0x7eu);
    }
}

static void iplay_runtime_draw_original_attr_text(IplayRuntime *runtime, dw y, dw x, const char *text, const db *attrs);

void iplay_runtime_draw_original_volume_text(IplayRuntime *runtime, const char *text) {
    static const db attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x78,0x78,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x78,0x78,0x78,0x78,0x78,0x78,0x78
    };
    if (iplay_runtime_video_cols(runtime) < 80u) return;
    iplay_runtime_draw_original_attr_text(runtime, 24u, 44u, text, attrs);
}

void iplay_runtime_draw_audio_status(IplayRuntime *runtime) {
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_AUDIO_ROW, "Playing in Stereo, Free", "482KB", IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_HARDWARE_ROW, "Samples Used  ", "0/15", IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LEVELS_ROW, "Output Levels ", "", IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_audio_levels(runtime, IPLAY_RUNTIME_STATUS_LEVELS_ROW, IPLAY_RUNTIME_STATUS_LEVELS_X, IPLAY_RUNTIME_STATUS_LEVELS_WIDTH);
    iplay_runtime_draw_status_line(runtime, IPLAY_RUNTIME_STATUS_PLAYBACK_ROW, "24bit Interpolation      F-12", IPLAY_RUNTIME_STATUS_PLAYBACK_ATTR);
}

dw iplay_runtime_refresh_audio_status(IplayRuntime *runtime) {
    iplay_runtime_draw_audio_status(runtime);
    return iplay_runtime_present(runtime);
}

void iplay_runtime_draw_video_status(IplayRuntime *runtime) {
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_VIDEO_ROW, "Main Volume   ", " 100%      - +", IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
}

void iplay_module_status_init(IplayModuleStatus *status, const char *title, const char *module_path, dd module_size, const char *loader_symbol, dd module_type) {
    iplay_module_status_set_title(status, title);
    iplay_module_status_set_path(status, module_path);
    iplay_module_status_set_size(status, module_size);
    iplay_module_status_set_loader(status, loader_symbol);
    iplay_module_status_set_type(status, module_type);
}

#define iplay_module_status_set_title_field(state, value) ((state)->title = (value))
#define iplay_module_status_set_path_field(state, value) ((state)->module_path = (value))
#define iplay_module_status_set_size_field(state, value) ((state)->module_size = (value))
#define iplay_module_status_set_loader_field(state, value) ((state)->loader_symbol = (value))
#define iplay_module_status_set_type_field(state, value) ((state)->module_type = (value))

#define iplay_module_status_title_field(state) ((state)->title)
#define iplay_module_status_path_field(state) ((state)->module_path)
#define iplay_module_status_size_field(state) ((state)->module_size)
#define iplay_module_status_loader_field(state) ((state)->loader_symbol)
#define iplay_module_status_type_field(state) ((state)->module_type)

void iplay_module_status_set_title(IplayModuleStatus *status, const char *title) {
    iplay_module_status_set_title_field(status, title);
}

void iplay_module_status_set_path(IplayModuleStatus *status, const char *module_path) {
    iplay_module_status_set_path_field(status, module_path);
}

void iplay_module_status_set_size(IplayModuleStatus *status, dd module_size) {
    iplay_module_status_set_size_field(status, module_size);
}

void iplay_module_status_set_loader(IplayModuleStatus *status, const char *loader_symbol) {
    iplay_module_status_set_loader_field(status, loader_symbol);
}

const char *iplay_module_status_title(const IplayModuleStatus *status) {
    return iplay_module_status_title_field(status);
}

const char *iplay_module_status_path(const IplayModuleStatus *status) {
    return iplay_module_status_path_field(status);
}

dd iplay_module_status_size(const IplayModuleStatus *status) {
    return iplay_module_status_size_field(status);
}

const char *iplay_module_status_loader(const IplayModuleStatus *status) {
    return iplay_module_status_loader_field(status);
}

dd iplay_module_status_type(const IplayModuleStatus *status) {
    return iplay_module_status_type_field(status);
}

void iplay_module_status_set_type(IplayModuleStatus *status, dd module_type) {
    iplay_module_status_set_type_field(status, module_type);
}

void iplay_module_status_clear_type(IplayModuleStatus *status) {
    iplay_module_status_set_type(status, 0);
}

void iplay_module_status_type_hex(const IplayModuleStatus *status, char *dst) {
    dd value = iplay_module_status_type(status);
    dw off = 0;
    write_hex_dword((db *)dst, &off, value);
    dst[off] = 0;
}

#define IPLAY_RUNTIME_TAG4(a, b, c, d) ((dd)(db)(a) | ((dd)(db)(b) << 8) | ((dd)(db)(c) << 16) | ((dd)(db)(d) << 24))

static const char *iplay_runtime_module_type_text(dd module_type) {
    switch (module_type) {
    case IPLAY_RUNTIME_TAG4('N', '.', 'T', '.'): return "N.T.";
    case IPLAY_RUNTIME_TAG4('S', '3', 'M', ' '): return "S3M";
    case IPLAY_RUNTIME_TAG4('S', 'T', 'M', ' '): return "STM";
    case IPLAY_RUNTIME_TAG4('E', '6', '6', '9'): return "669";
    case IPLAY_RUNTIME_TAG4('M', 'T', 'M', ' '): return "MTM";
    case IPLAY_RUNTIME_TAG4('P', 'S', 'M', ' '): return "PSM";
    case IPLAY_RUNTIME_TAG4('F', 'A', 'R', ' '): return "FAR";
    case IPLAY_RUNTIME_TAG4('U', 'L', 'T', ' '): return "ULT";
    case IPLAY_RUNTIME_TAG4('E', 'X', 'T', ' '): return "EXT";
    case IPLAY_RUNTIME_TAG4('I', 'N', 'R', ' '): return "INR";
    default: return "Unknown";
    }
}

void iplay_runtime_draw_module_status_struct(IplayRuntime *runtime, const IplayModuleStatus *status) {
    iplay_runtime_draw_module_status(runtime, iplay_module_status_title(status), iplay_module_status_path(status), iplay_module_status_size(status), iplay_module_status_loader(status));
}

void iplay_runtime_draw_module_tag_struct(IplayRuntime *runtime, const IplayModuleStatus *status) {
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_TAG_ROW, "Module Type   ", iplay_runtime_module_type_text(iplay_module_status_type(status)), IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
}

static dw iplay_runtime_status_content_row(dw y) {
    if (y < IPLAY_RUNTIME_STATUS_TITLE_ROW) return 0;
    return (dw)(y - IPLAY_RUNTIME_STATUS_TITLE_ROW);
}

static void iplay_runtime_status_content_window(IplayRuntime *runtime, IplayWindow *window) {
    IplayWindow root;
    dw cols;
    iplay_window_init_root(&root, iplay_runtime_stdplane(runtime));
    cols = iplay_window_cols(&root);
    if (cols > 2u) {
        iplay_window_init_subwindow(window,
                                    &root,
                                    IPLAY_RUNTIME_STATUS_TITLE_ROW,
                                    1u,
                                    (dw)(IPLAY_RUNTIME_STATUS_PLAYBACK_ROW - IPLAY_RUNTIME_STATUS_TITLE_ROW + 1u),
                                    (dw)(cols - 2u));
    } else {
        iplay_window_init_root(window, iplay_runtime_stdplane(runtime));
    }
}

static void iplay_runtime_draw_status_panel(IplayRuntime *runtime) {
    IplayWindow window;
    iplay_window_init_root(&window, iplay_runtime_stdplane(runtime));
    /* inventory marker: iplay_window_box_yx(&root, */
    iplay_window_box_yx(&window,
                        IPLAY_RUNTIME_STATUS_PANEL_ROW,
                        0,
                        IPLAY_RUNTIME_STATUS_PANEL_HEIGHT,
                        iplay_window_cols(&window),
                        IPLAY_RUNTIME_STATUS_PANEL_ATTR,
                        IPLAY_RUNTIME_STATUS_PANEL_FILL_ATTR);
}

void iplay_runtime_draw_status_block(IplayRuntime *runtime, const IplayModuleStatus *status) {
    iplay_runtime_draw_status_panel(runtime);
    iplay_runtime_draw_module_status_struct(runtime, status);
    iplay_runtime_draw_audio_status(runtime);
    iplay_runtime_draw_video_status(runtime);
    iplay_runtime_draw_module_tag_struct(runtime, status);
}

void iplay_runtime_draw_module_status(IplayRuntime *runtime, const char *title, const char *module_path, dd module_size, const char *loader_symbol) {
    (void)module_size;
    (void)loader_symbol;
    iplay_runtime_draw_status_line(runtime, IPLAY_RUNTIME_STATUS_TITLE_ROW, title, IPLAY_RUNTIME_STATUS_TITLE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_MODULE_ROW, "Filename      ", module_path, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_SIZE_ROW, "Current Track ", "1/0", IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LOADER_ROW, "Track Position", "1/64", IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
}

void iplay_runtime_draw_module_tag(IplayRuntime *runtime, dd module_type) {
    iplay_runtime_draw_status_hex32(runtime, IPLAY_RUNTIME_STATUS_TAG_ROW, "Tag", module_type, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
}

void iplay_runtime_draw_original_filename_status(IplayRuntime *runtime, const char *text, db attr) {
    iplay_runtime_draw_status_line(runtime, IPLAY_RUNTIME_STATUS_MODULE_ROW, text, attr);
}

static void iplay_runtime_draw_original_info_line(IplayRuntime *runtime, dw y, const char *text) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    dw x = 5u;
    dw prefix_width = 16u;
    dw i = 0u;
    while (text && text[i]) {
        iplay_ncplane_putc_yx(plane, y, (dw)(x + i), (db)text[i], i < prefix_width ? 0x7eu : 0x7fu);
        ++i;
    }
}

static void iplay_runtime_draw_original_attr_text(IplayRuntime *runtime, dw y, dw x, const char *text, const db *attrs) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    dw i = 0u;
    while (text && text[i]) {
        iplay_ncplane_putc_yx(plane, y, (dw)(x + i), (db)text[i], attrs[i]);
        ++i;
    }
}

typedef struct IplayOriginalCell {
    dw x;
    db ch;
    db attr;
} IplayOriginalCell;

static void iplay_runtime_draw_original_cells(IplayRuntime *runtime, dw y, const IplayOriginalCell *cells, dw count) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    dw i;
    for (i = 0u; i < count; ++i) {
        iplay_ncplane_putc_yx(plane, y, cells[i].x, cells[i].ch, cells[i].attr);
    }
}

static void iplay_runtime_draw_original_hline(IplayRuntime *runtime, dw y, dw x0, dw x1, db ch, db attr) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    dw x;
    for (x = x0; x <= x1; ++x) {
        iplay_ncplane_putc_yx(plane, y, x, ch, attr);
    }
}

static void iplay_runtime_draw_original_blank_run(IplayRuntime *runtime, dw y, dw x0, dw x1, db attr) {
    iplay_runtime_draw_original_hline(runtime, y, x0, x1, ' ', attr);
}

static void iplay_runtime_draw_original_split_text(IplayRuntime *runtime, dw y, dw x, const char *text, dw highlight_start, dw highlight_end) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    dw i = 0u;
    while (text && text[i]) {
        db attr = (i >= highlight_start && i < highlight_end) ? 0x7fu : 0x7eu;
        iplay_ncplane_putc_yx(plane, y, (dw)(x + i), (db)text[i], attr);
        ++i;
    }
}

static void iplay_runtime_draw_original_frame_structure(IplayRuntime *runtime) {
    static const IplayOriginalCell top_row_cells[] = {
        { 0u, 0xdau, 0x7fu }, { 79u, 0xbfu, 0x78u },
    };
    static const IplayOriginalCell title_top_cells[] = {
        { 0u, 0xb3u, 0x7fu }, { 2u, 0xdau, 0x7fu }, { 77u, 0xbfu, 0x78u }, { 79u, 0xb3u, 0x78u },
    };
    static const IplayOriginalCell title_text_side_cells[] = {
        { 0u, 0xb3u, 0x7fu }, { 2u, 0xb3u, 0x7fu }, { 77u, 0xb3u, 0x78u }, { 79u, 0xb3u, 0x78u },
    };
    static const IplayOriginalCell title_bottom_cells[] = {
        { 0u, 0xb3u, 0x7fu }, { 2u, 0xc0u, 0x7fu }, { 77u, 0xd9u, 0x78u }, { 79u, 0xb3u, 0x78u },
    };
    static const IplayOriginalCell side_cells[] = {
        { 0u, 0xb3u, 0x7fu }, { 79u, 0xb3u, 0x78u },
    };
    static const IplayOriginalCell bottom_row_cells[] = {
        { 0u, 0xc0u, 0x7fu }, { 79u, 0xd9u, 0x78u },
    };
    iplay_runtime_draw_original_blank_run(runtime, 5u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 7u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 8u, 1u, 5u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 8u, 73u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 9u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 10u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 11u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 12u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 13u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 14u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 15u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 16u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 17u, 1u, 78u, 0x7fu);
    iplay_runtime_draw_original_cells(runtime, 0u, top_row_cells, (dw)(sizeof(top_row_cells) / sizeof(top_row_cells[0])));
    iplay_runtime_draw_original_hline(runtime, 0u, 1u, 78u, 0xc4u, 0x7fu);
    iplay_runtime_draw_original_cells(runtime, 1u, title_top_cells, (dw)(sizeof(title_top_cells) / sizeof(title_top_cells[0])));
    iplay_runtime_draw_original_hline(runtime, 1u, 3u, 76u, 0xc4u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 1u, 1u, 1u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 1u, 78u, 78u, 0x7fu);
    iplay_runtime_draw_original_cells(runtime, 2u, title_text_side_cells, (dw)(sizeof(title_text_side_cells) / sizeof(title_text_side_cells[0])));
    iplay_runtime_draw_original_blank_run(runtime, 2u, 1u, 1u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 2u, 78u, 78u, 0x7fu);
    iplay_runtime_draw_original_cells(runtime, 3u, title_text_side_cells, (dw)(sizeof(title_text_side_cells) / sizeof(title_text_side_cells[0])));
    iplay_runtime_draw_original_blank_run(runtime, 3u, 1u, 1u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 3u, 78u, 78u, 0x7fu);
    iplay_runtime_draw_original_cells(runtime, 4u, title_bottom_cells, (dw)(sizeof(title_bottom_cells) / sizeof(title_bottom_cells[0])));
    iplay_runtime_draw_original_hline(runtime, 4u, 3u, 76u, 0xc4u, 0x78u);
    iplay_runtime_draw_original_blank_run(runtime, 4u, 1u, 1u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 4u, 78u, 78u, 0x7fu);
    iplay_runtime_draw_original_cells(runtime, 5u, side_cells, (dw)(sizeof(side_cells) / sizeof(side_cells[0])));
    iplay_runtime_draw_original_cells(runtime, 7u, side_cells, (dw)(sizeof(side_cells) / sizeof(side_cells[0])));
    iplay_runtime_draw_original_cells(runtime, 11u, side_cells, (dw)(sizeof(side_cells) / sizeof(side_cells[0])));
    iplay_runtime_draw_original_cells(runtime, 12u, side_cells, (dw)(sizeof(side_cells) / sizeof(side_cells[0])));
    iplay_runtime_draw_original_cells(runtime, 17u, side_cells, (dw)(sizeof(side_cells) / sizeof(side_cells[0])));
    iplay_runtime_draw_original_cells(runtime, 18u, bottom_row_cells, (dw)(sizeof(bottom_row_cells) / sizeof(bottom_row_cells[0])));
    iplay_runtime_draw_original_hline(runtime, 18u, 1u, 78u, 0xc4u, 0x78u);
}

static void iplay_runtime_draw_original_middle_rows(IplayRuntime *runtime) {
    static const db row_6_text_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
    };
    static const IplayOriginalCell row_6_cells[] = {
        { 0u, 0xb3u, 0x7fu },
        { 3u, 0x48u, 0x7eu }, { 4u, 0x6fu, 0x7eu }, { 5u, 0x70u, 0x7eu }, { 6u, 0x65u, 0x7eu },
        { 8u, 0x79u, 0x7eu }, { 9u, 0x6fu, 0x7eu }, { 10u, 0x75u, 0x7eu },
        { 12u, 0x6cu, 0x7eu }, { 13u, 0x69u, 0x7eu }, { 14u, 0x6bu, 0x7eu }, { 15u, 0x65u, 0x7eu }, { 16u, 0x64u, 0x7eu },
        { 18u, 0x75u, 0x7eu }, { 19u, 0x73u, 0x7eu }, { 20u, 0x69u, 0x7eu }, { 21u, 0x6eu, 0x7eu }, { 22u, 0x67u, 0x7eu },
        { 24u, 0x74u, 0x7eu }, { 25u, 0x68u, 0x7eu }, { 26u, 0x65u, 0x7eu },
        { 28u, 0x49u, 0x7fu }, { 29u, 0x6eu, 0x7fu }, { 30u, 0x65u, 0x7fu }, { 31u, 0x72u, 0x7fu }, { 32u, 0x74u, 0x7fu }, { 33u, 0x69u, 0x7fu }, { 34u, 0x61u, 0x7fu },
        { 36u, 0x50u, 0x7fu }, { 37u, 0x6cu, 0x7fu }, { 38u, 0x61u, 0x7fu }, { 39u, 0x79u, 0x7fu }, { 40u, 0x65u, 0x7fu }, { 41u, 0x72u, 0x7fu },
        { 43u, 0x77u, 0x7eu }, { 44u, 0x68u, 0x7eu }, { 45u, 0x69u, 0x7eu }, { 46u, 0x63u, 0x7eu }, { 47u, 0x68u, 0x7eu },
        { 49u, 0x69u, 0x7eu }, { 50u, 0x73u, 0x7eu },
        { 52u, 0x77u, 0x7eu }, { 53u, 0x72u, 0x7eu }, { 54u, 0x69u, 0x7eu }, { 55u, 0x74u, 0x7eu }, { 56u, 0x74u, 0x7eu }, { 57u, 0x65u, 0x7eu }, { 58u, 0x6eu, 0x7eu },
        { 60u, 0x69u, 0x7eu }, { 61u, 0x6eu, 0x7eu },
        { 63u, 0x31u, 0x7fu }, { 64u, 0x30u, 0x7fu }, { 65u, 0x30u, 0x7fu }, { 66u, 0x25u, 0x7fu },
        { 68u, 0x61u, 0x7fu }, { 69u, 0x73u, 0x7fu }, { 70u, 0x73u, 0x7fu }, { 71u, 0x65u, 0x7fu }, { 72u, 0x6du, 0x7fu }, { 73u, 0x62u, 0x7fu }, { 74u, 0x6cu, 0x7fu }, { 75u, 0x65u, 0x7fu }, { 76u, 0x72u, 0x7fu }, { 77u, 0x21u, 0x7fu },
        { 79u, 0xb3u, 0x78u },
    };
    static const db row_8_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e
    };
    static const db row_9_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
    };
    static const db row_10_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
    };
    static const IplayOriginalCell row_8_10_border_cells[] = {
        { 0u, 0xb3u, 0x7fu },
        { 79u, 0xb3u, 0x78u },
    };
    iplay_runtime_draw_original_frame_structure(runtime);
    iplay_runtime_draw_original_attr_text(runtime, 6u, 3u, "Hope you liked using the Inertia Player which is written in 100% assembler!", row_6_text_attrs);
    iplay_runtime_draw_original_blank_run(runtime, 6u, 1u, 2u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 6u, 62u, 62u, 0x7eu);
    iplay_runtime_draw_original_blank_run(runtime, 6u, 78u, 78u, 0x7fu);
    iplay_runtime_draw_original_cells(runtime, 6u, row_6_cells, (dw)(sizeof(row_6_cells) / sizeof(row_6_cells[0])));
    iplay_runtime_draw_original_attr_text(runtime, 8u, 6u, "If you have bug-reports, suggestions or comments send a message to:", row_8_attrs);
    iplay_runtime_draw_original_cells(runtime, 8u, row_8_10_border_cells, (dw)(sizeof(row_8_10_border_cells) / sizeof(row_8_10_border_cells[0])));
    iplay_runtime_draw_original_attr_text(runtime, 9u, 6u, "Internet : sdanes@marvels.hacktic.nl", row_9_attrs);
    iplay_runtime_draw_original_cells(runtime, 9u, row_8_10_border_cells, (dw)(sizeof(row_8_10_border_cells) / sizeof(row_8_10_border_cells[0])));
    iplay_runtime_draw_original_attr_text(runtime, 10u, 6u, "FidoNet  : 2:284/116.8", row_10_attrs);
    iplay_runtime_draw_original_cells(runtime, 10u, row_8_10_border_cells, (dw)(sizeof(row_8_10_border_cells) / sizeof(row_8_10_border_cells[0])));
    iplay_runtime_draw_original_split_text(runtime, 13u, 3u, "Send email to listserver@oliver.sun.ac.za to subscribe to one or both of", 14u, 41u);
    iplay_runtime_draw_original_cells(runtime, 13u, row_8_10_border_cells, (dw)(sizeof(row_8_10_border_cells) / sizeof(row_8_10_border_cells[0])));
    iplay_runtime_draw_original_split_text(runtime, 14u, 3u, "the Inertia Mailinglists and write following text in your message:", 4u, 24u);
    iplay_runtime_draw_original_cells(runtime, 14u, row_8_10_border_cells, (dw)(sizeof(row_8_10_border_cells) / sizeof(row_8_10_border_cells[0])));
    iplay_runtime_draw_original_split_text(runtime, 15u, 3u, "To connect to Binary Inertia releases: subscribe inertia-list YourRealName", 39u, 74u);
    iplay_runtime_draw_original_cells(runtime, 15u, row_8_10_border_cells, (dw)(sizeof(row_8_10_border_cells) / sizeof(row_8_10_border_cells[0])));
    iplay_runtime_draw_original_split_text(runtime, 16u, 3u, "To connect to Discussion Mailing list: subscribe inertia-talk YourRealName", 39u, 74u);
    iplay_runtime_draw_original_cells(runtime, 16u, row_8_10_border_cells, (dw)(sizeof(row_8_10_border_cells) / sizeof(row_8_10_border_cells[0])));
}

static void iplay_runtime_draw_original_lower_row_borders(IplayRuntime *runtime) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    dw y;
    for (y = 19u; y <= 26u; ++y) {
        iplay_ncplane_putc_yx(plane, y, 0u, 0xb3u, 0x7fu);
        iplay_ncplane_putc_yx(plane, y, 3u, 0xb3u, 0x78u);
        iplay_ncplane_putc_yx(plane, y, 37u, 0xb3u, 0x7fu);
        iplay_ncplane_putc_yx(plane, y, 42u, 0xb3u, 0x78u);
        if (y != 24u) iplay_ncplane_putc_yx(plane, y, 44u, 0xfeu, 0x78u);
        iplay_ncplane_putc_yx(plane, y, 76u, 0xb3u, 0x7fu);
        iplay_ncplane_putc_yx(plane, y, 79u, 0xb3u, 0x78u);
    }
    if (iplay_runtime_video_rows(runtime) >= 28u) {
        iplay_ncplane_putc_yx(plane, 26u, 3u, 0xc0u, 0x7fu);
        iplay_runtime_draw_original_hline(runtime, 26u, 4u, 36u, 0xc4u, 0x7fu);
        iplay_ncplane_putc_yx(plane, 26u, 37u, 0xd9u, 0x7fu);
        iplay_ncplane_putc_yx(plane, 26u, 42u, 0xc0u, 0x7fu);
        iplay_runtime_draw_original_hline(runtime, 26u, 43u, 75u, 0xc4u, 0x7fu);
        iplay_ncplane_putc_yx(plane, 26u, 76u, 0xd9u, 0x7fu);
        iplay_ncplane_putc_yx(plane, 27u, 0u, 0xc0u, 0x7fu);
        iplay_runtime_draw_original_hline(runtime, 27u, 1u, 78u, 0xc4u, 0x7fu);
        iplay_ncplane_putc_yx(plane, 27u, 79u, 0xd9u, 0x7fu);
    }
}

static void iplay_runtime_draw_original_lower_blank_runs(IplayRuntime *runtime) {
    dw y;
    for (y = 19u; y <= 25u; ++y) {
        iplay_runtime_draw_original_blank_run(runtime, y, 4u, 36u, 0x78u);
        iplay_runtime_draw_original_blank_run(runtime, y, 43u, 75u, 0x78u);
    }
    for (y = 20u; y <= 25u; ++y) {
        iplay_runtime_draw_original_blank_run(runtime, y, 1u, 2u, 0x7fu);
        iplay_runtime_draw_original_blank_run(runtime, y, 4u, 4u, 0x78u);
        iplay_runtime_draw_original_blank_run(runtime, y, 25u, 36u, 0x78u);
        iplay_runtime_draw_original_blank_run(runtime, y, 38u, 41u, 0x7fu);
        iplay_runtime_draw_original_blank_run(runtime, y, 43u, 43u, 0x78u);
        iplay_runtime_draw_original_blank_run(runtime, y, 77u, 78u, 0x7fu);
    }
    for (y = 20u; y <= 25u; ++y) {
        iplay_runtime_draw_original_blank_run(runtime, y, 45u, 45u, 0x7eu);
    }
    for (y = 20u; y <= 25u; ++y) {
        iplay_runtime_draw_original_blank_run(runtime, y, 74u, 75u, 0x78u);
    }
    iplay_runtime_draw_original_blank_run(runtime, 21u, 22u, 24u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 22u, 25u, 27u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 23u, 24u, 26u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 24u, 25u, 26u, 0x7fu);
}

static void iplay_runtime_draw_original_live_upper_rows(
    IplayRuntime *runtime,
    const char *module_title,
    const char *driver_line) {
    IplayNcPlane *plane = iplay_runtime_stdplane(runtime);
    dw y;
    dw i;
    if (iplay_runtime_video_cols(runtime) < 80u) return;
    iplay_runtime_draw_original_frame_structure(runtime);
    for (y = 5u; y <= 17u; ++y) {
        iplay_runtime_draw_original_blank_run(runtime, y, 1u, 78u, 0x7fu);
        iplay_ncplane_putc_yx(plane, y, 0u, 0xb3u, 0x7fu);
        iplay_ncplane_putc_yx(plane, y, 79u, 0xb3u, 0x78u);
    }
    iplay_ncplane_putc_yx(plane, 5u, 1u, 0xdau, 0x78u);
    for (i = 2u; i <= 77u; ++i) {
        iplay_ncplane_putc_yx(plane, 5u, i, 0xc4u, 0x78u);
    }
    iplay_ncplane_putc_yx(plane, 5u, 78u, 0xbfu, 0x7fu);
    iplay_ncplane_putc_yx(plane, 16u, 1u, 0xc0u, 0x78u);
    for (i = 2u; i <= 77u; ++i) {
        iplay_ncplane_putc_yx(plane, 16u, i, 0xc4u, 0x7fu);
    }
    iplay_ncplane_putc_yx(plane, 16u, 78u, 0xd9u, 0x7fu);
    for (y = 10u; y <= 15u; ++y) {
        iplay_ncplane_putc_yx(plane, y, 1u, 0xb3u, 0x78u);
        iplay_runtime_draw_original_blank_run(runtime, y, 2u, 77u, 0x78u);
        iplay_ncplane_putc_yx(plane, y, 78u, 0xb3u, 0x7fu);
    }
    iplay_ncplane_putc_yx(plane, 17u, 3u, 0xdau, 0x78u);
    for (i = 4u; i <= 36u; ++i) {
        iplay_ncplane_putc_yx(plane, 17u, i, 0xc4u, 0x78u);
    }
    iplay_ncplane_putc_yx(plane, 17u, 37u, 0xbfu, 0x7fu);
    iplay_ncplane_putc_yx(plane, 17u, 42u, 0xdau, 0x78u);
    for (i = 43u; i <= 75u; ++i) {
        iplay_ncplane_putc_yx(plane, 17u, i, 0xc4u, 0x78u);
    }
    iplay_ncplane_putc_yx(plane, 17u, 76u, 0xbfu, 0x7fu);
    iplay_ncplane_putc_yx(plane, 18u, 0u, 0xb3u, 0x7fu);
    iplay_ncplane_putc_yx(plane, 18u, 3u, 0xb3u, 0x78u);
    iplay_ncplane_putc_yx(plane, 18u, 37u, 0xb3u, 0x7fu);
    iplay_ncplane_putc_yx(plane, 18u, 42u, 0xb3u, 0x78u);
    iplay_ncplane_putc_yx(plane, 18u, 76u, 0xb3u, 0x7fu);
    iplay_ncplane_putc_yx(plane, 18u, 79u, 0xb3u, 0x78u);
    iplay_runtime_draw_original_blank_run(runtime, 18u, 1u, 2u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 18u, 4u, 36u, 0x78u);
    iplay_runtime_draw_original_blank_run(runtime, 18u, 38u, 41u, 0x7fu);
    iplay_runtime_draw_original_blank_run(runtime, 18u, 43u, 75u, 0x78u);
    iplay_runtime_draw_original_blank_run(runtime, 18u, 77u, 78u, 0x7fu);
    for (i = 0u; module_title && module_title[i] && i < 32u; ++i) {
        iplay_ncplane_putc_yx(plane, 18u, (dw)(5u + i), (db)module_title[i], 0x7fu);
    }
    for (i = 0u; driver_line && driver_line[i] && i < 30u; ++i) {
        iplay_ncplane_putc_yx(plane, 18u, (dw)(44u + i), (db)driver_line[i], 0x7fu);
    }
}

static void iplay_runtime_draw_original_module_info_impl(
    IplayRuntime *runtime,
    const char *filename_line,
    const char *module_type_line,
    const char *playing_line,
    const char *channels_line,
    const char *samples_line,
    const char *track_line,
    const char *position_line,
    int include_middle_rows) {
    static const db playing_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x78,0x7f,0x7f,0x7f,0x7f,0x7f
    };
    static const db ignore_bpm_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7e,0x7e,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78
    };
    static const db loop_module_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7e,0x7e,0x7e,0x7e,0x7e,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78
    };
    static const db interpolation_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x7e,0x7e,0x7e,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78
    };
    static const db protracker_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
        0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78
    };
    static const db channels_attrs[32] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x78,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
    };
    static const db samples_attrs[32] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x78,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
    };
    static const db track_attrs[32] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x78,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
    };
    static const db position_attrs[32] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x78,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
    };
    static const db volume_attrs[] = {
        0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x78,0x78,
        0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x78,0x78,0x78,0x78,0x78,0x78,0x78
    };
    iplay_ncplane_putc_yx(iplay_runtime_stdplane(runtime), 2u, 44u, 'D', 0x7f);
    iplay_ncplane_putc_yx(iplay_runtime_stdplane(runtime), 3u, 44u, 'D', 0x7f);
    if (include_middle_rows) iplay_runtime_draw_original_middle_rows(runtime);
    iplay_runtime_draw_original_lower_row_borders(runtime);
    iplay_runtime_draw_original_lower_blank_runs(runtime);
    iplay_runtime_draw_original_info_line(runtime, 19u, filename_line);
    iplay_runtime_draw_original_info_line(runtime, 20u, module_type_line);
    iplay_runtime_draw_original_attr_text(runtime, 19u, 46u, playing_line, playing_attrs);
    iplay_runtime_draw_original_attr_text(runtime, 20u, 46u, "ProTracker 1.0           F-9", protracker_attrs);
    iplay_runtime_draw_original_attr_text(runtime, 21u, 5u, channels_line, channels_attrs);
    iplay_runtime_draw_original_attr_text(runtime, 21u, 46u, "Ignore BPM changes       F-10", ignore_bpm_attrs);
    iplay_runtime_draw_original_attr_text(runtime, 22u, 5u, samples_line, samples_attrs);
    iplay_runtime_draw_original_attr_text(runtime, 22u, 46u, "Loop Module when done    F-11", loop_module_attrs);
    iplay_runtime_draw_original_attr_text(runtime, 23u, 5u, track_line, track_attrs);
    iplay_runtime_draw_original_attr_text(runtime, 23u, 46u, "24bit Interpolation      F-12", interpolation_attrs);
    iplay_runtime_draw_original_attr_text(runtime, 24u, 5u, position_line, position_attrs);
    iplay_runtime_draw_original_attr_text(runtime, 24u, 44u, "Main Volume   :  100%      - +", volume_attrs);
}

void iplay_runtime_draw_original_module_info(IplayRuntime *runtime, const char *filename_line, const char *module_type_line) {
    iplay_runtime_draw_original_module_info_impl(
        runtime,
        filename_line,
        module_type_line,
        "Playing in Stereo, Free: 482KB",
        "Channels      : 2",
        "Samples Used  : 0/15",
        "Current Track : 1/0",
        "Track Position: 1/64",
        1);
}

void iplay_runtime_draw_original_live_module_info(
    IplayRuntime *runtime,
    const char *filename_line,
    const char *module_type_line,
    const char *playing_line,
    const char *channels_line,
    const char *samples_line,
    const char *module_title,
    const char *driver_line,
    const char *track_line,
    const char *position_line) {
    iplay_runtime_draw_original_live_upper_rows(runtime, module_title, driver_line);
    iplay_runtime_draw_original_module_info_impl(
        runtime,
        filename_line,
        module_type_line,
        playing_line,
        channels_line,
        samples_line,
        track_line,
        position_line,
        0);
}

void iplay_runtime_draw_status_line(IplayRuntime *runtime, dw y, const char *text, db attr) {
    IplayWindow window;
    iplay_runtime_status_content_window(runtime, &window);
    iplay_window_draw_status_line(&window, iplay_runtime_status_content_row(y), text, attr);
}

void iplay_window_draw_status_line(IplayWindow *window, dw y, const char *text, db attr) {
    IplayNcPlane *plane = iplay_window_plane(window);
    iplay_ncplane_putnstr_fill_yx(plane, y, 0, text, attr, iplay_ncplane_cols(plane));
}

void iplay_window_draw_status_field(IplayWindow *window, dw y, const char *label, const char *value, db label_attr, db value_attr) {
    IplayNcPlane *plane = iplay_window_plane(window);
    dw cols = iplay_ncplane_cols(plane);
    dw x = 0;
    while (*label && x < cols) {
        iplay_ncplane_putc_yx(plane, y, x, (db)*label, label_attr);
        ++label;
        ++x;
    }
    if (x < cols) {
        iplay_ncplane_putc_yx(plane, y, x, ':', label_attr);
        ++x;
    }
    if (x < cols) {
        iplay_ncplane_putc_yx(plane, y, x, ' ', label_attr);
        ++x;
    }
    if (x < cols) {
        iplay_ncplane_putnstr_fill_yx(plane, y, x, value, value_attr, (dw)(cols - x));
    }
}

void iplay_runtime_draw_status_field(IplayRuntime *runtime, dw y, const char *label, const char *value, db label_attr, db value_attr) {
    IplayWindow window;
    iplay_runtime_status_content_window(runtime, &window);
    iplay_window_draw_status_field(&window, iplay_runtime_status_content_row(y), label, value, label_attr, value_attr);
}

void iplay_runtime_draw_status_u32(IplayRuntime *runtime, dw y, const char *label, dd value, db label_attr, db value_attr) {
    IplayWindow window;
    iplay_runtime_status_content_window(runtime, &window);
    iplay_window_draw_status_u32(&window, iplay_runtime_status_content_row(y), label, value, label_attr, value_attr);
}

void iplay_window_draw_status_u32(IplayWindow *window, dw y, const char *label, dd value, db label_attr, db value_attr) {
    char buf[12];
    char tmp[12];
    unsigned count = 0;
    unsigned i;
    if (value == 0) {
        buf[0] = '0';
        buf[1] = 0;
    } else {
        while (value != 0 && count < sizeof(tmp)) {
            tmp[count++] = (char)('0' + (value % 10u));
            value /= 10u;
        }
        for (i = 0; i < count; ++i) {
            buf[i] = tmp[count - 1u - i];
        }
        buf[count] = 0;
    }
    iplay_window_draw_status_field(window, y, label, buf, label_attr, value_attr);
}

void iplay_runtime_draw_status_hex32(IplayRuntime *runtime, dw y, const char *label, dd value, db label_attr, db value_attr) {
    IplayWindow window;
    iplay_runtime_status_content_window(runtime, &window);
    iplay_window_draw_status_hex32(&window, iplay_runtime_status_content_row(y), label, value, label_attr, value_attr);
}

void iplay_window_draw_status_hex32(IplayWindow *window, dw y, const char *label, dd value, db label_attr, db value_attr) {
    static const char digits[] = "0123456789ABCDEF";
    char buf[9];
    unsigned i;
    for (i = 0; i < 8u; ++i) {
        unsigned shift = (7u - i) * 4u;
        buf[i] = digits[(value >> shift) & 0x0fu];
    }
    buf[8] = 0;
    iplay_window_draw_status_field(window, y, label, buf, label_attr, value_attr);
}

dw iplay_runtime_present(IplayRuntime *runtime) {
    return iplay_notcurses_present(iplay_runtime_notcurses(runtime));
}

static int16_t get_s16le(const db *src, dw off) {
    return (int16_t)((dw)src[off] | ((dw)src[(dw)(off + 1u)] << 8));
}

static dw abs_s16_to_peak(int16_t sample) {
    int32_t value = sample;
    if (value < 0) value = -value;
    return (dw)value;
}

db iplay_audio_level_to_16(dw peak) {
    dd scaled = (dd)peak / 2048u;
    if (scaled > 15u) scaled = 15u;
    return (db)scaled;
}

void iplay_audio_sb16_stereo_levels(IplayAudioLevels *levels, const db *pcm, dw frame_count) {
    dw frame;
    dw left_peak = 0;
    dw right_peak = 0;
    for (frame = 0; frame < frame_count; ++frame) {
        dw off = (dw)(frame * 4u);
        dw left = abs_s16_to_peak(get_s16le(pcm, off));
        dw right = abs_s16_to_peak(get_s16le(pcm, (dw)(off + 2u)));
        if (left > left_peak) left_peak = left;
        if (right > right_peak) right_peak = right;
    }
    iplay_audio_levels_set(levels, left_peak, right_peak);
}

static char *append_u32_dec(char *p, dd value) {
    db tmp[16];
    dw off = 0;
    dw count = 0;
    dw i;
    write_u32_base(tmp, &off, value, 10, &count);
    for (i = 0; i < count; ++i) *p++ = (char)tmp[i];
    return p;
}

static char *append_2digits(char *p, unsigned value) {
    *p++ = (char)('0' + (value / 10u) % 10u);
    *p++ = (char)('0' + value % 10u);
    return p;
}

static char *append_4digits(char *p, unsigned value) {
    *p++ = (char)('0' + (value / 1000u) % 10u);
    *p++ = (char)('0' + (value / 100u) % 10u);
    *p++ = (char)('0' + (value / 10u) % 10u);
    *p++ = (char)('0' + value % 10u);
    return p;
}

void iplay_txt_draw_bottom_plane(IplayNcPlane *plane, const IplayBottomLayout *layout, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif) {
    char buf[32];
    char *p;
    db attr;

    p = buf;
    p = append_u32_dec(p, byte_1de75);
    *p++ = ' '; *p++ = 'a'; *p++ = 't'; *p++ = ' ';
    p = append_u32_dec(p, byte_1de76);
    *p++ = 'b'; *p++ = 'p'; *p++ = 'm';
    while (p < buf + 13) *p++ = ' ';
    *p = 0;
    iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_timing_y_field(layout), iplay_bottom_layout_left_x_field(layout), buf, 0x7f, iplay_bottom_layout_timing_width_field(layout));

    iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_timing_y_field(layout), iplay_bottom_layout_mode_x_field(layout), (flags & 8u) ? "(PAL) " : "(NTSC)", 0x7e, iplay_bottom_layout_mode_width_field(layout));
    if (iplay_bottom_layout_mode_x_field(layout) >= 6u) {
        iplay_ncplane_putnstr_fill_yx(
            plane,
            iplay_bottom_layout_timing_y_field(layout),
            (dw)(iplay_bottom_layout_mode_x_field(layout) - 6u),
            "Speed ",
            0x7e,
            6u);
    }

    p = buf;
    p = append_u32_dec(p, (db)(byte_1de72 + 1u));
    *p++ = '/';
    p = append_u32_dec(p, byte_1de73);
    *p++ = ' '; *p++ = ' '; *p++ = ' '; *p = 0;
    iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_module_y_field(layout), iplay_bottom_layout_left_x_field(layout), buf, 0x7f, iplay_bottom_layout_module_width_field(layout));

    p = buf;
    p = append_u32_dec(p, (db)(byte_1de74 + 1u));
    *p++ = '/'; *p++ = '6'; *p++ = '4'; *p++ = ' '; *p++ = ' '; *p = 0;
    iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_pattern_y_field(layout), iplay_bottom_layout_left_x_field(layout), buf, 0x7f, iplay_bottom_layout_pattern_width_field(layout));

    attr = (flags & 1u) ? 0x7c : 0x78;
    iplay_ncplane_putc_yx(plane, (dw)(iplay_bottom_layout_module_y_field(layout) - 3u), iplay_bottom_layout_flag_x_field(layout), 0xfe, attr);
    attr = (flags & 2u) ? 0x7c : 0x78;
    iplay_ncplane_putc_yx(plane, (dw)(iplay_bottom_layout_module_y_field(layout) - 2u), iplay_bottom_layout_flag_x_field(layout), 0xfe, attr);
    attr = (flags & 4u) ? 0x7c : 0x78;
    iplay_ncplane_putc_yx(plane, (dw)(iplay_bottom_layout_module_y_field(layout) - 1u), iplay_bottom_layout_flag_x_field(layout), 0xfe, attr);
    attr = (flags & 0x10u) ? 0x7c : 0x78;
    iplay_ncplane_putc_yx(plane, iplay_bottom_layout_module_y_field(layout), iplay_bottom_layout_flag_x_field(layout), 0xfe, attr);

    p = buf;
    p = append_u32_dec(p, ((dd)volume * 100u) >> 8);
    *p++ = '%'; *p++ = ' '; *p++ = ' '; *p = 0;
    iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_pattern_y_field(layout), iplay_bottom_layout_value_x_field(layout), buf, 0x7f, iplay_bottom_layout_value_width_field(layout));

    p = buf;
    p = append_u32_dec(p, amplif);
    *p++ = '%'; *p++ = ' '; *p++ = ' '; *p = 0;
    if (iplay_bottom_layout_value_x_field(layout) >= 17u) {
        iplay_ncplane_putnstr_fill_yx(
            plane,
            iplay_bottom_layout_timing_y_field(layout),
            (dw)(iplay_bottom_layout_value_x_field(layout) - 17u),
            "Volume Amplify:",
            0x7e,
            17u);
    }
    iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_timing_y_field(layout), iplay_bottom_layout_value_x_field(layout), buf, 0x7f, iplay_bottom_layout_value_width_field(layout));
    if (iplay_bottom_layout_mode_x_field(layout) >= 10u) {
        iplay_ncplane_putnstr_yx(
            plane,
            iplay_bottom_layout_timing_y_field(layout),
            33u,
            "Tab",
            0x78u,
            3u);
        iplay_ncplane_putnstr_yx(
            plane,
            iplay_bottom_layout_timing_y_field(layout),
            71u,
            "[ ]",
            0x78u,
            3u);
    }
}

void iplay_txt_draw_bottom(db *mem, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif) {
    IplayNcPlane plane;
    const IplayBottomLayout *layout = iplay_bottom_layout();
    iplay_ncplane_init_mode(&plane, mem, iplay_text_current_mode());
    iplay_txt_draw_bottom_plane(&plane, layout, byte_1de72, byte_1de73, byte_1de74, byte_1de75, byte_1de76, flags, volume, amplif);
}

void iplay_text_screen_draw_bottom(IplayTextScreen *screen, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif) {
    iplay_txt_draw_bottom_plane(iplay_text_screen_root(screen), iplay_text_screen_bottom_layout(screen), byte_1de72, byte_1de73, byte_1de74, byte_1de75, byte_1de76, flags, volume, amplif);
}

void iplay_text_screen_draw_audio_output_levels(IplayTextScreen *screen, dw y, dw x, const IplayAudioOutput *output, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr) {
    iplay_audio_output_draw_levels_yx(iplay_text_screen_root(screen), y, x, output, width, fill_ch, empty_ch, left_attr, right_attr, empty_attr);
}

static void put_row_cell(db *row, unsigned index, db ch, db attr) {
    row[index * 2u] = ch;
    row[index * 2u + 1u] = attr;
}

static unsigned put_row_text(db *row, unsigned index, const char *text, db attr) {
    while (*text) put_row_cell(row, index++, (db)*text++, attr);
    return index;
}

void iplay_filelist_row(db *row, db entry_type, db flags, dw time_word, dw date_word, dd size, const char *name) {
    static const char *months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    unsigned i = 0;
    char buf[40];
    unsigned n;
    (void)flags;
    if (entry_type == 1) {
        for (n = 0; name[n] && n < 12; ++n) put_row_cell(row, i++, (db)name[n], 0x7b);
        while (i < 63) put_row_cell(row, i++, ' ', 0x7e);
        return;
    }
    {
        int ended = 0;
        for (n = 0; n < 12; ++n) {
        db ch = ' ';
        if (!ended && name[n] != 0) {
            ch = (db)name[n];
        } else {
            ended = 1;
        }
        put_row_cell(row, i++, ch, 0x7e);
        }
    }
    {
        char digits[16];
        char *end = append_u32_dec(digits, size);
        unsigned len = (unsigned)(end - digits);
        unsigned pad = len < 8 ? 8 - len : 0;
        while (pad-- != 0) put_row_cell(row, i++, ' ', 0x7f);
        for (n = 0; n < len; ++n) put_row_cell(row, i++, (db)digits[n], 0x7f);
    }
    put_row_cell(row, i++, ' ', 0x7f);
    put_row_cell(row, i++, ' ', 0x7f);
    {
        unsigned day = date_word & 0x1f;
        unsigned month = (date_word >> 5) & 0x0f;
        unsigned year = 1980u + ((date_word >> 9) & 0x7f);
        const char *mon = (month >= 1 && month <= 12) ? months[month - 1] : "???";
        char *p = buf;
        if (day >= 10) *p++ = (char)('0' + day / 10);
        *p++ = (char)('0' + day % 10);
        *p++ = '-';
        *p++ = mon[0]; *p++ = mon[1]; *p++ = mon[2];
        *p++ = '-';
        p = append_4digits(p, year);
        *p = 0;
        i = put_row_text(row, i, buf, 0x7f);
    }
    put_row_cell(row, i++, ' ', 0x7f);
    {
        unsigned hour = (time_word >> 11) & 0x1f;
        unsigned minute = (time_word >> 5) & 0x3f;
        char *p = buf;
        p = append_2digits(p, hour);
        *p++ = ':';
        p = append_2digits(p, minute);
        *p = 0;
        i = put_row_text(row, i, buf, 0x7f);
    }
    put_row_cell(row, i++, ' ', 0x7f);
    put_row_text(row, i, "Description", 0x7e);
}

void iplay_find_mods_no_nul_guard(IplayRegs *r, db *mem, dw dseg) {
    unsigned i;
    dd eax = dseg;
    dd edi = 0x137c;
    for (i = 0; i < 120; ++i) {
        ++edi;
        if (mem[0x137c + i] == 0) {
            apply_eax_edi_regs(r, eax, edi);
            return;
        }
    }
    mem[0x168e] = 2;
    mem[0x1640] = 0x77;
    mem[0x1641] = 0x12;
    mem[0x1642] = (db)dseg;
    mem[0x1643] = (db)(dseg >> 8);
    apply_eax_edi_regs(r, eax, edi);
}

static db recolor_attribute_run(db *mem, dw *offset, db color, unsigned count) {
    db value = 0;
    unsigned i;
    for (i = 0; i < count; ++i) {
        value = (db)((mem[*offset] & 0x0fu) | color);
        mem[*offset] = value;
        *offset = (dw)(*offset + 2u);
    }
    return value;
}

IplayRecolorResult iplay_recolor_text_row(db *mem, const IplayTextMode *mode, dw row, db color) {
    IplayRecolorResult result;
    dw di = (dw)((row * iplay_text_mode_row_bytes(mode)) + IPLAY_TEXT_OFFSET(iplay_text_mode_cols(mode), 10u, 8u) + 1u);
    db al = recolor_attribute_run(mem, &di, color, 64);
    result.dst_offset = di;
    result.ax = (dw)((row & 0xff00u) | al);
    return result;
}

static int mouse_rect_hit(dw *x, dw *y, dw *left, dw *top, dw *right, dw *bottom) {
    dw tmp;
    if (*left > *right) { tmp = *left; *left = *right; *right = tmp; }
    if (*top > *bottom) { tmp = *top; *top = *bottom; *bottom = tmp; }
    if (*x < *left || *x > *right || *y < *top || *y > *bottom) return 1;
    *x = (dw)(*x - *left);
    *y = (dw)(*y - *top);
    return 0;
}

static int mouse_table_lookup(const db *mem, dw *table, dw *x, dw *y, dw *left, dw *top, dw *right, dw *bottom, dw *id) {
    for (;;) {
        dw entry = *table;
        dw px = *x;
        dw py = *y;
        *left = get_word(mem, entry);
        if (*left == 0xffffu) {
            *id = entry;
            return 1;
        }
        *top = get_word(mem, (dw)(entry + 2u));
        *right = get_word(mem, (dw)(entry + 4u));
        *bottom = get_word(mem, (dw)(entry + 6u));
        if (!mouse_rect_hit(&px, &py, left, top, right, bottom)) {
            *x = px;
            *y = py;
            *id = get_word(mem, (dw)(entry + 8u));
            return 0;
        }
        *table = (dw)(entry + 0x0au);
    }
}

void iplay_recolor_txt(IplayRegs *r, db *mem) {
    const IplayTextMode *mode = iplay_text_current_mode();
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    IplayRecolorResult result = iplay_recolor_text_row(mem, mode, (dw)eax, (db)ebx);
    apply_full_regs6(r, (eax & 0xffff0000UL) | result.ax,
                     ebx, ecx & 0xffff0000UL, edx,
                     esi, (edi & 0xffff0000UL) | result.dst_offset);
}

int iplay_mouse_1c7a9(IplayRegs *r) {
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ebp = abi_ebp(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    dw ax = (dw)eax;
    dw bp = (dw)ebp;
    dw cx = (dw)ecx;
    dw dx = (dw)edx;
    dw si = (dw)esi;
    dw di = (dw)edi;
    int outside = mouse_rect_hit(&ax, &bp, &cx, &dx, &si, &di);
    apply_full_regs6(r, eax, ebx,
                     (ecx & 0xffff0000UL) | cx,
                     (edx & 0xffff0000UL) | dx,
                     (esi & 0xffff0000UL) | si,
                     (edi & 0xffff0000UL) | di);
    if (outside) return 1;
    apply_eax_reg(r, (eax & 0xffff0000UL) | ax);
    apply_ebp_reg(r, (ebp & 0xffff0000UL) | bp);
    return 0;
}

int iplay_mouse_1c7cf(IplayRegs *r, const db *mem) {
    dd eax = abi_eax(r);
    dd ebx = abi_ebx(r);
    dd ebp = abi_ebp(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);
    dd esi = abi_esi(r);
    dd edi = abi_edi(r);
    dw bx = (dw)ebx;
    dw ax = (dw)eax;
    dw bp = (dw)ebp;
    dw cx = 0;
    dw dx = 0;
    dw si = 0;
    dw di = 0;
    dw id = bx;
    int miss = mouse_table_lookup(mem, &bx, &ax, &bp, &cx, &dx, &si, &di, &id);
    apply_full_regs6(r, (eax & 0xffff0000UL) | ax,
                     (ebx & 0xffff0000UL) | id,
                     (ecx & 0xffff0000UL) | cx,
                     (edx & 0xffff0000UL) | dx,
                     (esi & 0xffff0000UL) | si,
                     (edi & 0xffff0000UL) | di);
    apply_ebp_reg(r, (ebp & 0xffff0000UL) | bp);
    return miss;
}

static db int24_action_from_ah(db ah) {
    db al = 1;
    if (ah & 0x08u) al = 3;
    else if (ah & 0x20u) al = 0;
    return al;
}

void iplay_int24(IplayRegs *r) {
    dd old_eax = abi_eax(r);
    dd eax = (old_eax & 0xffffff00UL) | int24_action_from_ah((db)(old_eax >> 8));
    apply_eax_reg(r, eax);
}

void iplay_ems_restore_mapctx_guard(IplayRegs *r, db ems_enabled, db mapctx_saved) {
    (void)r;
    (void)ems_enabled;
    (void)mapctx_saved;
}

void iplay_ems_init_config(IplayRegs *r, db *globals, dw config_word) {
    dd eax = (abi_eax(r) & 0xffff0000UL) | 1u;
    globals[0x0104] = 0;
    apply_full_regs6(r, eax, abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    if (config_word & 2u) {
        /* Hardware EMS probing is intentionally outside this rewrite slice. */
    }
}

void iplay_ems_disabled_guard(IplayRegs *r, db ems_enabled) {
    (void)r;
    (void)ems_enabled;
}

#ifndef IPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS
void ems_release(void) {}

void ems_realloc(void) {}

void ems_deinit(void) {}

void ems_save_mapctx(void) {}

void ems_restore_mapctx(void) {}

void ems_mapmem(void) {}

void ems_mapmem2(void) {}
#endif

static dd mapcopy_low_word(dd value, dd add) {
    return (value + add) & 0xffffu;
}

void iplay_ems_local_mapcopy(db *mem, const char *symbol, dw base) {
    dd source;
    dd dest;
    if (strcmp(symbol, "ems_mapmemx") == 0) {
        source = mapcopy_low_word(get_dword(mem, (dw)(base + 0x2cu)), 1u);
        dest = mapcopy_low_word(get_dword(mem, (dw)(base + 0x20u)), 0x800u);
        memmove(mem + dest, mem + source, 0x800u);
        if (source + 0x800u <= dest || dest + 0x800u <= source) {
            memset(mem + source, 0, 0x800u);
        }
    } else {
        source = mapcopy_low_word(get_dword(mem, (dw)(base + 0x20u)), 0x800u);
        dest = mapcopy_low_word(get_dword(mem, (dw)(base + 0x2cu)), 1u);
        memmove(mem + dest, mem + source, 0x800u);
    }
}

static void ems_note_realloc_fallback(db *mem) {
    mem[0x0077] = (db)(mem[0x0077] + 1u);
}

void iplay_ems_realloc2_fallback(IplayRegs *r, db *mem, dw di) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dd eax = (old_eax & 0xffff0000UL) | 8u;
    dd ecx = (old_ecx & 0xffff0000UL) | 0xffffu;
    (void)di;
    ems_note_realloc_fallback(mem);
    apply_full_regs6(r, eax, old_ebx, ecx, old_edx, old_esi, old_edi);
}

void iplay_clean_11c43(db *mem, db flag_playsettings, db byte_2461e, db byte_2461f) {
    dw freq = (flag_playsettings & 8u) ? 8287u : 8363u;
    unsigned i;
    unsigned row;
    mem[0x0032] = 0; mem[0x0033] = 0;
    mem[0x0034] = 4; mem[0x0035] = 0;
    mem[0x0036] = 4; mem[0x0037] = 0;
    mem[0x0038] = 0; mem[0x0039] = 0;
    mem[0x003a] = 0; mem[0x003b] = 0;
    mem[0x003e] = (db)freq; mem[0x003f] = (db)(freq >> 8);
    memset(mem + 0x0050, 0, 12);
    mem[0x005e] = 100; mem[0x005f] = 0;
    mem[0x007a] = 0;
    mem[0x0085] = 0;
    mem[0x0090] = 2; mem[0x0091] = 0;
    mem[0x00d3] = 0;
    mem[0x00d9] = 6;
    mem[0x00da] = 125;
    mem[0x00de] = 0;
    mem[0x0130] = 1; mem[0x0131] = 0;

    memset(mem + 0x1368u, 0, 0x0a00u);
    memset(mem + 0x1d68u, 0, 0x18c0u);
    for (row = 0; row < 0x63u; ++row) {
        dw base = (dw)(0x1d68u + row * 0x40u);
        memset(mem + base, ' ', 0x20u);
        mem[base + 0x32u] = 0xffu;
        mem[base + 0x33u] = 0xffu;
    }
    memset(mem + 0x3648u, 0, 0x0200u);
    memset(mem + 0x3a48u, 0, 0x0100u);
    memset(mem + 0x3b48u, 0, 0x0100u);
    memset(mem + 0x3d48u, 0, 0x0020u);
    memset(mem + 0x3c48u, '?', 0x0100u);

    for (i = 0; i < 8u; ++i) {
        dw base = (dw)(0x3628u + i * 4u);
        mem[base] = byte_2461e;
        mem[base + 1u] = byte_2461f;
        mem[base + 2u] = byte_2461f;
        mem[base + 3u] = byte_2461e;
    }
}

static db apply_delta_decode(db *mem, dw *offset, dw *count, db previous) {
    db value = previous;
    while (*count != 0) {
        value = (db)(value + mem[*offset]);
        mem[*offset] = value;
        *offset = (dw)(*offset + 1u);
        *count = (dw)(*count - 1u);
    }
    return value;
}

static db write_packed_mod_event(db *mem, dw *di, dw *dx, dw bx, db cl, db ch, db *current_max) {
    db bl = (db)bx;
    db bh = (db)(bx >> 8);
    if (!((bl == 0 || bl == 0xffu) && (bh == 0 || bh == 0xffu))) ch |= 0x20u;
    if (cl <= 0x40u) ch |= 0x40u;
    if (*dx != 0) {
        if ((db)*dx == 0) *dx = (dw)((*dx & 0xff00u) | 0x1du);
        ch |= 0x80u;
    }
    if ((ch & 0xe0u) != 0) {
        mem[*di] = ch;
        *di = (dw)(*di + 1u);
        if (ch & 0x80u) {
            mem[*di] = (db)*dx;
            *di = (dw)(*di + 1u);
            mem[*di] = (db)(*dx >> 8);
            *di = (dw)(*di + 1u);
        }
        if (ch & 0x40u) {
            mem[*di] = cl;
            *di = (dw)(*di + 1u);
        }
        if (ch & 0x20u) {
            mem[*di] = (db)bx;
            *di = (dw)(*di + 1u);
            mem[*di] = (db)(bx >> 8);
            *di = (dw)(*di + 1u);
        }
        if ((ch & 0x1fu) > *current_max) *current_max = (db)(ch & 0x1fu);
    }
    return ch;
}

void iplay_mod_sub_delta(IplayRegs *r, db *mem, db flag, db reset, db *previous) {
    db al;
    dw si;
    dw cx;
    dd old_eax;
    dd old_ebx;
    dd old_ecx;
    dd old_edx;
    dd old_esi;
    dd old_edi;
    if (flag != 1) return;
    al = *previous;
    if (reset != 0) al = 0;
    old_eax = abi_eax(r);
    old_ebx = abi_ebx(r);
    old_ecx = abi_ecx(r);
    old_edx = abi_edx(r);
    old_esi = abi_esi(r);
    old_edi = abi_edi(r);
    si = (dw)old_esi;
    cx = (dw)old_ecx;
    al = apply_delta_decode(mem, &si, &cx, al);
    *previous = al;
    apply_full_regs6(r, (old_eax & 0xffffff00UL) | al, old_ebx,
                     (old_ecx & 0xffff0000UL) | cx, old_edx,
                     (old_esi & 0xffff0000UL) | si, old_edi);
}

void iplay_sub_11ba6(IplayRegs *r, db *mem, db *current_max) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    db ch = (db)((old_ecx >> 8) & 0x1fu);
    db cl = (db)old_ecx;
    dw bx = (dw)old_ebx;
    dw dx = (dw)old_edx;
    dw di = (dw)old_edi;
    ch = write_packed_mod_event(mem, &di, &dx, bx, cl, ch, current_max);
    apply_full_regs6(r, (old_eax & 0xffffff00UL) | (ch & 0x1fu),
                     old_ebx,
                     (old_ecx & 0xffff0000UL) | ((dw)ch << 8) | cl,
                     (old_edx & 0xffff0000UL) | dx,
                     old_esi,
                     (old_edi & 0xffff0000UL) | di);
}

dw iplay_mod_102f5(const db *orders) {
    db maxv = 0;
    unsigned i;
    for (i = 0; i < 128; ++i) {
        db value = (db)(orders[i] & 0x7fu);
        if (value >= maxv) maxv = value;
    }
    return (dw)(maxv + 1u);
}

static void pack_sub_126a9_regs(dw word_245fa, dw size1, dw channels, db realloc_count, dd module_type, dd *eax, dw *bx, dw *cx, dw *si, dw *di) {
    *eax = module_type;
    *bx = (dw)(((dw)(size1 & 0xffu) << 8) | (word_245fa & 0xffu));
    *cx = (dw)(((dw)realloc_count << 8) | (channels & 0xffu));
    *si = 0x1d68u;
    *di = 0x0110u;
}

void iplay_sub_126a9(IplayRegs *r, dw word_245fa, dw size1, dw channels, db realloc_count, dd module_type) {
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dd eax;
    dw bx;
    dw cx;
    dw si;
    dw di;
    pack_sub_126a9_regs(word_245fa, size1, channels, realloc_count, module_type, &eax, &bx, &cx, &si, &di);
    apply_full_regs6(
        r,
        eax,
        (old_ebx & 0xffff0000UL) | bx,
        (old_ecx & 0xffff0000UL) | cx,
        old_edx,
        (old_esi & 0xffff0000UL) | si,
        (old_edi & 0xffff0000UL) | di);
}

void iplay_ult_read_fast(IplayRegs *r, db *mem) {
    (void)r;
    mem[0xc09c] = (db)(mem[0xc09c] - 1u);
}

static void pack_sub_1265d_regs(dw volume, db sndcard, db byte_24666, db byte_24667, db sndflags, db byte_24628, db stereo, db byte_24671, dw word_245f6, dw word_245f0, dw *ax, dw *bx, dw *cx, dw *dx, dw *bp, dw *si, dw *di) {
    db dh = (db)(byte_24628 - 1u);
    dh = (db)((dh & 3u) << 1);
    dh = (db)((dh | stereo) << 1);
    dh = (db)((dh | byte_24671) << 3);
    *ax = (dw)((word_245f0 & 0xffu) << 8) | (word_245f6 & 0xffu);
    *bx = (dw)((dw)byte_24667 << 8);
    *cx = (dw)(((dw)byte_24666 << 8) | ((volume - 1u) & 0xffu));
    *dx = (dw)(((dw)dh << 8) | sndflags);
    *bp = sndcard;
    *si = 0x1368u;
    *di = 0x0110u;
}

void iplay_sub_1265d(IplayRegs *r, dw volume, db sndcard, db byte_24666, db byte_24667, db sndflags, db byte_24628, db stereo, db byte_24671, dw word_245f6, dw word_245f0) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_ebp = abi_ebp(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dw ax;
    dw bx;
    dw cx;
    dw dx;
    dw bp;
    dw si;
    dw di;
    pack_sub_1265d_regs(volume, sndcard, byte_24666, byte_24667, sndflags, byte_24628, stereo, byte_24671, word_245f6, word_245f0, &ax, &bx, &cx, &dx, &bp, &si, &di);
    apply_full_regs6(
        r,
        (old_eax & 0xffff0000UL) | ax,
        (old_ebx & 0xffff0000UL) | bx,
        (old_ecx & 0xffff0000UL) | cx,
        (old_edx & 0xffff0000UL) | dx,
        (old_esi & 0xffff0000UL) | si,
        (old_edi & 0xffff0000UL) | di);
    apply_ebp_reg(r, (old_ebp & 0xffff0000UL) | bp);
}

void iplay_memfree_125da_guard(IplayRegs *r) {
    apply_eax_reg(r, (abi_eax(r) & 0xffff0000UL) | 0x156au);
}

void iplay_mod_1021e(db *out, db first, db second, const db *pattern, const db *title) {
    unsigned i;
    db title_ch;
    out[0] = (second < 0x78u) ? second : 0;
    out[1] = 0;
    out[2] = first;
    out[3] = 0;
    memcpy(out + 4, pattern, 128);
    for (i = 0; i < 20; ++i) {
        title_ch = title[i];
        if (title_ch < 0x20u) break;
        out[132 + i] = title_ch;
    }
    while (i < 20) out[132 + i++] = ' ';
}

static dw get_be_word(const db *mem, unsigned off) {
    return (dw)(((dw)mem[off] << 8) | mem[off + 1u]);
}

static void put_dword(db *mem, unsigned off, dd value) {
    mem[off] = (db)value;
    mem[off + 1u] = (db)(value >> 8);
    mem[off + 2u] = (db)(value >> 16);
    mem[off + 3u] = (db)(value >> 24);
}

void iplay_mod_1024a(db *out, dw sample_count, const db *headers, dw freq) {
    dd total = 0;
    dw overflow = 0;
    dw i;
    memset(out, 0, 6u + (dw)(sample_count * 0x40u));
    for (i = 0; i < sample_count; ++i) {
        const db *src = headers + (dw)(i * 30u);
        db *dst = out + 6u + (dw)(i * 0x40u);
        dd length = (dd)get_be_word(src, 22) << 1;
        dd loop_start = (dd)get_be_word(src, 26) << 1;
        dd loop_len = (dd)get_be_word(src, 28) << 1;
        dd loop_end = loop_len;
        unsigned n;
        for (n = 0; n < 22; ++n) {
            if (src[n] < 0x20u) break;
            dst[n] = src[n];
        }
        if (length >= 0x100000u) ++overflow;
        put_dword(dst, 0x20, length);
        total += length;
        dst[0x3e] = (db)(src[24] & 0x0fu);
        dst[0x36] = (db)freq;
        dst[0x37] = (db)(freq >> 8);
        dst[0x3d] = src[25];
        put_dword(dst, 0x28, loop_len);
        if (loop_len > 2u) {
            if (loop_start < length) {
                dst[0x3c] |= 8u;
                loop_end = loop_len + loop_start;
                if (loop_end > length) {
                    loop_end = (loop_len >> 1) + loop_start;
                    if (loop_end > length) loop_end = length;
                }
            } else {
                loop_start >>= 1;
                if (loop_start < length) {
                    dst[0x3c] |= 8u;
                    loop_end = loop_len + loop_start;
                    if (loop_end > length) {
                        loop_end = (loop_len >> 1) + loop_start;
                        if (loop_end > length) loop_end = length;
                    }
                } else {
                    loop_end = length;
                }
            }
        } else {
            loop_end = length;
        }
        put_dword(dst, 0x2c, loop_end - 1u);
        put_dword(dst, 0x24, loop_start);
    }
    put_dword(out, 0, total);
    out[4] = (db)overflow;
    out[5] = (db)(overflow >> 8);
}

void iplay_memfree_18a28_guard(IplayRegs *r, db memflag) {
    (void)r;
    (void)memflag;
}

#if !defined(IPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS) && !defined(IPLAY_REWRITE_EXTERNAL_MEMFREE18A28)
void memfree_18A28(void) {}
#endif

static void scan_sub_11c0c_stream(const db *mem, db *al, db *bl, dw *si) {
    static const db skip_table[8] = {0, 2, 1, 3, 2, 4, 3, 5};
    *si = 0;
    if (*al == 0) {
        *bl = 0;
        return;
    }
    *bl = 0;
    for (;;) {
        *bl = skip_table[*bl & 7u];
        *si = (dw)(*si + *bl);
        *bl = mem[*si];
        *si = (dw)(*si + 1u);
        *bl >>= 5;
        if (*bl != 0) continue;
        *al = (db)(*al - 1u);
        if (*al == 0) break;
    }
}

void iplay_sub_11c0c(IplayRegs *r, const db *mem) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_edi = abi_edi(r);
    db al = (db)old_eax;
    db bl = (db)old_ebx;
    dw si;
    scan_sub_11c0c_stream(mem, &al, &bl, &si);
    if ((db)old_eax == 0) {
        apply_ecx_esi_regs(r, old_ecx, si);
        return;
    }
    apply_full_regs6(r, (old_eax & 0xffffff00UL) | al,
                     (old_ebx & 0xffffff00UL) | bl,
                     old_ecx, old_edx, si, old_edi);
}

void iplay_sub_1415e(IplayRegs *r, db *mem, dw index, dw total, db segment_index, db pending) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_edi = abi_edi(r);
    db al = pending;
    db bl = (db)old_ebx;
    dw si;
    unsigned bit_off = 0x3d48u + (index >> 3);
    mem[0x0014] = 0;
    mem[0x0015] = 0;
    mem[0x0050] = (db)index;
    mem[0x0051] = (db)(index >> 8);
    mem[0x0054] = segment_index;
    mem[0x0055] = 0;
    mem[0x0056] = pending;
    mem[0x0057] = 0;
    mem[0x005a] = (db)total;
    mem[0x005b] = (db)(total >> 8);
    mem[0x00c9] = 0;
    mem[0x00ca] = 0;
    mem[0x00cb] = 0;
    mem[0x00cc] = 0;
    mem[0x00cd] = 0;
    mem[bit_off] = (db)(mem[bit_off] | (1u << (index & 7u)));
    scan_sub_11c0c_stream(mem, &al, &bl, &si);
    if (pending != 0) {
        apply_full_regs6(r, (old_eax & 0xffffff00UL) | al,
                         (old_ebx & 0xffffff00UL) | bl,
                         old_ecx, old_edx, si, old_edi);
    } else {
        apply_eax_esi_regs(r, (old_eax & 0xffffff00UL) | pending, si);
    }
    mem[0x0014] = (db)si;
    mem[0x0015] = (db)(si >> 8);
}

void iplay_sub_12f56(IplayRegs *r, db *mem, dw index, dw total, db segment_index, db pending, db bh) {
    (void)bh;
    iplay_sub_1415e(r, mem, index, total, segment_index, pending);
}

static void clear_zero_event_state(db *mem) {
    mem[0x0014] = 1;
    mem[0x0015] = 0x28;
    mem[0x1368 + 0x0a] = 0;
    mem[0x1368 + 0x0b] = 0;
    mem[0x1368 + 0x3d] = 0;
}

static void pack_zero_event_regs(dd *eax, dd *ebx, dd *ecx, dd *edx, dd *esi) {
    *eax = 0;
    *ebx = 0x13b8;
    *ecx = 0;
    *edx = 0x0100;
    *esi = 0x13a6;
}

void iplay_sub_135ca_zero_event(IplayRegs *r, db *mem) {
    dd old_edi = abi_edi(r);
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd esi;
    clear_zero_event_state(mem);
    pack_zero_event_regs(&eax, &ebx, &ecx, &edx, &esi);
    apply_full_regs6(r, eax, ebx, ecx, edx, esi, old_edi);
}

static void setup_spectr_len2_memory(db *mem, dw buf) {
    static const db work[12] = {0x00,0x00,0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x00,0x01,0x00};
    put_dword(mem, buf + 0, 0x00030000u);
    put_dword(mem, buf + 4, 0xffff0000u);
    put_dword(mem, buf + 8, 0x00030000u);
    put_dword(mem, buf + 12, 0x00040000u);
    mem[0x7d1e] = (db)buf;
    mem[0x7d1f] = (db)(buf >> 8);
    memcpy(mem + 0x7cd8, work, sizeof(work));
}

static void pack_spectr_len2_regs(dd *eax, dd *ebx, dd *ecx, dd *edx, dd *esi, dd *edi, dd *ebp) {
    *eax = 0;
    *ebx = 0;
    *ecx = 0;
    *edx = 0;
    *esi = 0x7ce4;
    *edi = 0x2b5e;
    *ebp = 0x2b00;
}

void iplay_spectr_1b084_len2(IplayRegs *r, db *mem, dw buf) {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd esi;
    dd edi;
    dd ebp;
    setup_spectr_len2_memory(mem, buf);
    pack_spectr_len2_regs(&eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
    apply_full_regs6(r, eax, ebx, ecx, edx, esi, edi);
    apply_ebp_reg(r, ebp);
}

static void clear_inactive_spectrum_buffers(db *mem) {
    memset(mem + 0x7a14, 0, 16);
    memset(mem + 0x7758, 0, 16);
}

static void pack_inactive_spectrum_regs(dd *eax, dd *ebx, dd *ecx, dd *edx, dd *esi, dd *edi, dd *ebp) {
    *eax = 0x156a;
    *ebx = 0x78e8;
    *ecx = 0;
    *edx = 0;
    *esi = 0x7768;
    *edi = 0x2a80;
    *ebp = 0xf9ff;
}

void iplay_f5_draw_spectr_inactive(IplayRegs *r, db *mem) {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd esi;
    dd edi;
    dd ebp;
    clear_inactive_spectrum_buffers(mem);
    pack_inactive_spectrum_regs(&eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
    apply_full_regs6(r, eax, ebx, ecx, edx, esi, edi);
    apply_ebp_reg(r, ebp);
}

IplayRegs6Result iplay_fill_dma_small_result(db *mem, const char *symbol, dw src, dw dst, dw count, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    dw i;
    if (strcmp(symbol, "fill_dmabuf8") == 0) {
        for (i = 0; i < count; ++i) {
            mem[(dw)(dst + i)] = (db)(mem[(dw)(src + i * 8u)] + 0x80u);
        }
        if (count != 0) {
            eax = (eax & 0xffff0000UL) | ((dw)mem[(dw)(dst + count - 1u)] << 8) | mem[(dw)(dst + count - 1u)];
        }
        ebx = (ebx & 0xffff0000UL) | (dw)(count * 2u);
        edx = (edx & 0xffff0000UL) | 0x0100u;
    } else if (strcmp(symbol, "fill_dmabuf8stereo") == 0) {
        for (i = 0; i < count; ++i) {
            mem[(dw)(dst + i)] = (db)(mem[(dw)(src + 1u + i * 4u)] + 0x80u);
        }
        if (count != 0) {
            eax = (eax & 0xffff0000UL) | ((dw)mem[(dw)(dst + count - 1u)] << 8) | mem[dst];
        }
        ebx = (ebx & 0xffff0000UL) | count;
        edx = (edx & 0xffff0000UL) | 0x8080u;
    } else if (strcmp(symbol, "fill_dmabuf16stereo") == 0) {
        for (i = 0; i < count; ++i) {
            mem[(dw)(dst + i * 4u + 0u)] = mem[(dw)(src + i * 8u + 0u)];
            mem[(dw)(dst + i * 4u + 1u)] = mem[(dw)(src + i * 8u + 1u)];
            mem[(dw)(dst + i * 4u + 2u)] = mem[(dw)(src + i * 8u + 4u)];
            mem[(dw)(dst + i * 4u + 3u)] = mem[(dw)(src + i * 8u + 5u)];
        }
        if (count != 0) {
            eax = (eax & 0xffff0000UL) | ((dw)mem[(dw)(dst + 1u)] << 8) | mem[dst];
        }
        ebx = (ebx & 0xffff0000UL) | (dw)(count * 2u);
        edx = (edx & 0xffff0000UL) | 0x0100u;
    }
    result.eax = eax;
    result.ebx = ebx;
    result.ecx = ecx & 0xffff0000UL;
    result.edx = edx;
    result.esi = (esi & 0xffff0000UL) | (dw)(src + 0x108u);
    result.edi = (edi & 0xffff0000UL) | (dw)(dst + 0x108u);
    return result;
}

void iplay_fill_dma_small(IplayRegs *r, db *mem, const char *symbol, dw src, dw dst, dw count) {
    IplayRegs6Result result = iplay_fill_dma_small_result(
        mem,
        symbol,
        src,
        dst,
        count,
        abi_eax(r),
        abi_ebx(r),
        abi_ecx(r),
        abi_edx(r),
        abi_esi(r),
        abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

static void init_inactive_mono_dma_bytes(db *mem, dw dma_off) {
    mem[dma_off + 0] = 0x90;
    mem[dma_off + 1] = 0x98;
    mem[dma_off + 2] = 0xa0;
    mem[dma_off + 3] = 0;
    mem[dma_off + 4] = 0;
    mem[dma_off + 5] = 0;
    mem[dma_off + 6] = 0;
    mem[dma_off + 7] = 0;
}

static void pack_inactive_mono_dma_regs(dd *eax, dd *ebx, dd *ecx, dd *edx, dd *esi, dd *edi) {
    *eax = 0x10a0;
    *ebx = 0x0006;
    *ecx = 0;
    *edx = 0x0100;
    *esi = 0xbf81;
    *edi = 0x0003;
}

IplayRegs6Result iplay_fill_dma_inactive_mono_result(db *mem, dw dma_off) {
    IplayRegs6Result result;
    init_inactive_mono_dma_bytes(mem, dma_off);
    pack_inactive_mono_dma_regs(&result.eax, &result.ebx, &result.ecx, &result.edx, &result.esi, &result.edi);
    return result;
}

void iplay_fill_dma_inactive_mono(IplayRegs *r, db *mem, dw dma_off) {
    IplayRegs6Result result = iplay_fill_dma_inactive_mono_result(mem, dma_off);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

static void apply_full_regs6(IplayRegs *r, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    r->eax = eax;
    r->ebx = ebx;
    r->ecx = ecx;
    r->edx = edx;
    r->esi = esi;
    r->edi = edi;
}

static void apply_eax_reg(IplayRegs *r, dd eax) {
    r->eax = eax;
}

static void apply_ebp_reg(IplayRegs *r, dd ebp) {
    r->ebp = ebp;
}

static void apply_esi_reg(IplayRegs *r, dd esi) {
    r->esi = esi;
}

static void apply_edi_reg(IplayRegs *r, dd edi) {
    r->edi = edi;
}

static void apply_eax_edi_regs(IplayRegs *r, dd eax, dd edi) {
    r->eax = eax;
    r->edi = edi;
}

static void apply_eax_esi_regs(IplayRegs *r, dd eax, dd esi) {
    r->eax = eax;
    r->esi = esi;
}

static void apply_ecx_esi_regs(IplayRegs *r, dd ecx, dd esi) {
    r->ecx = ecx;
    r->esi = esi;
}

static void apply_eax_edx_regs(IplayRegs *r, dd eax, dd edx) {
    r->eax = eax;
    r->edx = edx;
}

static void apply_eax_ecx_edx_regs(IplayRegs *r, dd eax, dd ecx, dd edx) {
    r->eax = eax;
    r->ecx = ecx;
    r->edx = edx;
}

static void apply_mix_setup_regs(IplayRegs *r, dd ebx, dd ebp, dd ecx, dd esi) {
    r->ebx = ebx;
    r->ebp = ebp;
    r->ecx = ecx;
    r->esi = esi;
}

static void apply_sndsettings_regs(IplayRegs *r, dd eax, dd ebx, dd ecx, dd edx, dd ebp, dd esi) {
    r->eax = eax;
    r->ebx = ebx;
    r->ecx = ecx;
    r->edx = edx;
    r->ebp = ebp;
    r->esi = esi;
}

void iplay_get_keybsw(IplayRegs *r, db *mem, dw value) {
    (void)r;
    mem[0] = (db)value;
    mem[1] = (db)(value >> 8);
}

void iplay_set_keybsw(IplayRegs *r, db *mem, dw value) {
    (void)r;
    (void)value;
    mem[0] = 0x00;
    mem[1] = 0x06;
}

static void init_int9_no_scancode_bytes(db *mem) {
    mem[0] = 0x00;
    mem[1] = 0x00;
    mem[2] = 0x57;
    mem[3] = 0x13;
    mem[4] = 0x00;
}

static void pack_int9_no_scancode_regs(dd *eax, dd *ebx, dd *ecx, dd *edx, dd *esi, dd *edi) {
    *eax = 0x1234;
    *ebx = 0x8033;
    *ecx = 0x9abc;
    *edx = 0xdef0;
    *esi = 0x0042;
    *edi = 0x1000;
}

void iplay_int9_keyb_no_scancode(IplayRegs *r, db *mem) {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd esi;
    dd edi;
    init_int9_no_scancode_bytes(mem);
    pack_int9_no_scancode_regs(&eax, &ebx, &ecx, &edx, &esi, &edi);
    apply_full_regs6(r, eax, ebx, ecx, edx, esi, edi);
}

static void write_sub_197f2_labels(db *mem, dw configword) {
    const db *text = (configword & 0x0020u) ? (const db *)"On On " : (const db *)"OffOff";
    memcpy(mem, text, 6);
}

void iplay_sub_197f2_labels(IplayRegs *r, db *mem, dw configword) {
    write_sub_197f2_labels(mem, configword);
    apply_full_regs6(r, 0x1234, 0x5678, 0, 0xdef0, 0x0e7c, 0x2806);
}

void iplay_useless_11787_zero(IplayRegs *r, db *mem, dw channel) {
    dd esi = (dw)(channel + 0x34u);
    mem[0] = 0;
    mem[1] = 0;
    mem[2] = 0;
    mem[3] = 0;
    mem[4] = 0x55;
    mem[5] = 0x55;
    mem[6] = 0x66;
    mem[7] = 0x66;
    apply_full_regs6(r, 0x4321, 0x2222, 0, 0x4444, esi, 0x2808);
}

void iplay_useless_11787_zero_public_layout(IplayRegs *r, db *mem, dw channel) {
    dd esi = (dw)(channel + 0x34u);
    mem[(dw)(channel + 0x20u)] = 0;
    mem[(dw)(channel + 0x21u)] = 0;
    mem[(dw)(channel + 0x22u)] = 0;
    mem[(dw)(channel + 0x23u)] = 0;
    apply_full_regs6(r, 0x4321, 0x2222, 0, 0x4444, esi, 0x2808);
}

void iplay_timer_port_no_device(IplayRegs *r, db *mem, const char *symbol, dw ax_value) {
    dd eax;
    mem[0] = 0;
    mem[1] = 0;
    if (strcmp(symbol, "clean_timer") == 0) {
        eax = (dw)(ax_value & 0xff00u);
    } else {
        eax = (dw)(((ax_value >> 8) & 0xffu) * 0x0101u);
    }
    apply_full_regs6(r, eax, 0x5678, 0, 0xdef0, 0x4f70, 0x2a82);
}

void iplay_set_timer_no_device(IplayRegs *r, db *mem, dw ax_value) {
    mem[0x4f6eu] = (db)ax_value;
    mem[0x4f6fu] = (db)(ax_value >> 8);
    apply_full_regs6(
        r,
        (dw)((((ax_value >> 8) & 0xffu) * 0x0101u)),
        abi_ebx(r),
        abi_ecx(r),
        abi_edx(r),
        abi_esi(r),
        abi_edi(r));
}

void iplay_clean_timer_no_device(IplayRegs *r, dw ax_value) {
    apply_full_regs6(
        r,
        (dw)(ax_value & 0xff00u),
        abi_ebx(r),
        abi_ecx(r),
        abi_edx(r),
        abi_esi(r),
        abi_edi(r));
}

void iplay_set_timer_int_alloc_fail(IplayRegs *r, db *mem) {
    mem[0] = 1;
    mem[1] = 1;
    mem[2] = 1;
    mem[3] = 1;
    apply_full_regs6(r, 0x3508, 0x0008, 0, 0x0054, 0x001c, 0x2a94);
}

void iplay_useless_doswrite2_header(IplayRegs *r, db *mem) {
    put_dword(mem, 0, 0x504d4153u);
    put_dword(mem, 4, 0x12345678u);
    apply_full_regs6(r, 0x000d, 0xffff, 0, 0xbf68, 0xbf70, 0x2808);
}

void iplay_useless_doswrite_header(IplayRegs *r, db *mem) {
    put_dword(mem, 0, 0x54534c50u);
    put_dword(mem, 4, 0x00000080u);
    apply_full_regs6(r, 0x000d, 0xffff, 0, 0x7fe8, 0xbf70, 0x2808);
}

void iplay_ult_1150b(IplayRegs *r, dw value) {
    dd old_ebx = abi_ebx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dd eax = value;
    dd ecx = 0x55aa;
    dd edx = 0xa55a;
    switch (value & 0xffu) {
    case 0x05: eax = 0; break;
    case 0x0a: eax = 0x330a; break;
    case 0x0b: eax = 0x8b0e; break;
    case 0x0c: eax = 0; ecx = 0x5529; break;
    default: break;
    }
    apply_full_regs6(r, eax, old_ebx, ecx, edx, old_esi, old_edi);
}

void iplay_ega_seq_no_device(IplayRegs *r, int set_mode) {
    dd eax = set_mode ? 0x1220 : 0x1200;
    dd ebx = 0x5678;
    dd ecx = 0x9abc;
    dd edx = 0x03c5;
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

void iplay_useless_unset_egaseq(IplayRegs *r, db mode_bits) {
    db bits = (db)(mode_bits & 3u);
    dd eax = ((dw)bits << 8) | bits;
    dd ebx = 0;
    dd ecx = 0x00ff;
    dd edx = 0x03cf;
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

void iplay_useless_strange_short(IplayRegs *r, db *mem) {
    mem[0] = 'O';
    mem[1] = 0x1e;
    mem[2] = 'K';
    mem[3] = 0x1e;
    apply_full_regs6(r, 0x1e00, 0, 0x000a, 0x0100, 0x7777, 0x2804);
    apply_ebp_reg(r, 0x0ffc);
}

void iplay_useless_writeinr_118_header(IplayRegs *r, db *mem) {
    static const db header[96] = {
        0x49,0x6e,0x65,0x72,0x74,0x69,0x61,0x20,0x53,0x61,0x6d,0x70,0x6c,0x65,0x3a,0x20,
        0x53,0x48,0x4f,0x52,0x54,0x20,0x53,0x41,0x4d,0x50,0x4c,0x45,0x20,0x4e,0x41,0x4d,
        0x45,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x0d,0x0a,0x1a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x78,0x56,0x34,0x12,0x10,0x40,0x7f,0xa5,0x21,0x43,0x00,0x00,0x11,0x11,0x11,0x11,
        0x22,0x22,0x22,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    };
    memcpy(mem, header, sizeof(header));
    apply_full_regs6(r, 0xfffe, 0xffff, 0x5678, 0x12a6, 0x1dc8, 0x1da8);
}

void iplay_useless_writeinr_fail(IplayRegs *r) {
    apply_full_regs6(r, 0xffff, 0x0002, 0x0020, 0xffff, 0x0042, 0x1000);
}

void iplay_useless_12d61_no_device(IplayRegs *r, db *mem) {
    mem[0] = 0;      /* sndcard_type */
    mem[1] = 0xff;   /* snd_base_port */
    mem[2] = 0xff;
    mem[3] = 0xff;   /* irq_number */
    mem[4] = 0xff;   /* dma_channel */
    mem[5] = 0x22;   /* freq_246D7 */
    mem[6] = 0xff;   /* byte_246D8 */
    mem[7] = 0xff;   /* byte_246D9 */
    apply_full_regs6(r, 0, 0, 0, 0x0286, 0x013a, 0x2808);
}

void iplay_txt_blink_no_device(IplayRegs *r, int enable) {
    dd ebx = enable ? 0x5601 : 0x5600;
    apply_full_regs6(r, 0x1000, ebx, 0x9abc, 0xdef0, abi_esi(r), abi_edi(r));
}

static int format_u32_dec(char *buf, dd value) {
    char *end = append_u32_dec(buf, value);
    *end = 0;
    return (int)(end - buf);
}

static int format_i32_dec(char *buf, int32_t value) {
    dw off = 0;
    dw count = 0;
    dd magnitude;
    write_i32_decimal((db *)buf, &off, value, &count, &magnitude);
    buf[off] = 0;
    return (int)off;
}

static int format_hex_width(char *buf, dd value, int width) {
    int i;
    for (i = width - 1; i >= 0; --i) {
        buf[width - 1 - i] = (char)hex_digit((db)(value >> (i * 4)));
    }
    buf[width] = 0;
    return width;
}

void iplay_useless_sprintf_chunk(IplayRegs *r, db *mem, const char *symbol, dd value) {
    char buf[16];
    int len = 0;
    dd eax;
    dd ecx;
    dd edx;
    memset(mem, 0xa5, 16);
    if (strcmp(symbol, "useless_sprint_6") == 0) {
        format_u32_dec(buf, value);
        eax = 0; ecx = 8; edx = '8';
    } else if (strcmp(symbol, "useless_sprint_7") == 0) {
        format_i32_dec(buf, (int32_t)(int8_t)(value & 0xffu));
        eax = 0; ecx = 2; edx = '5';
    } else if (strcmp(symbol, "useless_sprint_8") == 0) {
        format_i32_dec(buf, (int32_t)(int16_t)(value & 0xffffu));
        eax = 0; ecx = 4; edx = '3';
    } else if (strcmp(symbol, "useless_sprint_9") == 0) {
        format_i32_dec(buf, (int32_t)value);
        eax = 0; ecx = 7; edx = '6';
    } else if (strcmp(symbol, "useless_sprint_10") == 0) {
        format_hex_width(buf, value & 0xffu, 2);
        eax = 0x0d00; ecx = 0x00ff; edx = 0x0100;
    } else if (strcmp(symbol, "useless_sprint_11") == 0) {
        format_hex_width(buf, value & 0xffffu, 4);
        eax = (value & 0xffu) << 8; ecx = 0x00ff; edx = 0x0100;
    } else {
        format_hex_width(buf, value, 8);
        eax = (value & 0xffu) << 8; ecx = 0x00ff; edx = 0x0100;
    }
    len = (int)strlen(buf);
    memcpy(mem, buf, (size_t)len);
    apply_full_regs6(r, eax, 0, ecx, edx, 0x2904, (dw)(0x2940u + (dw)len));
}

void iplay_useless_mysprintf(IplayRegs *r, db *mem) {
    dd old_eax = abi_eax(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dw si = (dw)old_esi;
    dw di = (dw)old_edi;
    dd eax = old_eax;
    dd ebx = abi_ebx(r);
    dd ecx = abi_ecx(r);
    dd edx = abi_edx(r);

    for (;;) {
        db op = mem[si];
        si = (dw)(si + 1u);
        eax = (eax & 0xffffff00UL) | op;
        if (op >= 0x20u) {
            mem[di] = op;
            di = (dw)(di + 1u);
            continue;
        }
        if (op > 0x0cu) break;
        si = (dw)(si + 1u);
        if (op == 0) break;
        if (op == 1) {
            dw src = get_word(mem, si);
            dw count;
            copy_count_until_nul(mem, mem, &src, &di, &count);
            ecx = (ecx & 0xffff0000UL) | count;
            si = (dw)(si + 2u);
            continue;
        }
        if (op == 2 || op == 3) {
            dw index_ptr = get_word(mem, si);
            dw table = get_word(mem, (dw)(si + 2u));
            dw index = op == 2 ? mem[index_ptr] : get_word(mem, index_ptr);
            dw src = get_word(mem, (dw)(table + index * 2u));
            dw count;
            ebx = (ebx & 0xffff0000UL) | index;
            copy_count_until_nul(mem, mem, &src, &di, &count);
            ecx = (ecx & 0xffff0000UL) | count;
            si = (dw)(si + 4u);
            continue;
        }
        {
            char buf[16];
            dw value_ptr = get_word(mem, si);
            dd value = 0;
            int len = 0;
            int i;
            if (op == 4 || op == 7 || op == 10) {
                value = mem[value_ptr];
            } else if (op == 5 || op == 8 || op == 11) {
                value = get_word(mem, value_ptr);
            } else {
                value = (dd)mem[value_ptr]
                    | ((dd)mem[(dw)(value_ptr + 1u)] << 8)
                    | ((dd)mem[(dw)(value_ptr + 2u)] << 16)
                    | ((dd)mem[(dw)(value_ptr + 3u)] << 24);
            }
            switch (op) {
            case 4:
            case 5:
            case 6:
                len = format_u32_dec(buf, value);
                break;
            case 7:
                len = format_i32_dec(buf, (int32_t)(int8_t)(value & 0xffu));
                break;
            case 8:
                len = format_i32_dec(buf, (int32_t)(int16_t)(value & 0xffffu));
                break;
            case 9:
                len = format_i32_dec(buf, (int32_t)value);
                break;
            case 10:
                len = format_hex_width(buf, value & 0xffu, 2);
                break;
            case 11:
                len = format_hex_width(buf, value & 0xffffu, 4);
                break;
            case 12:
                len = format_hex_width(buf, value, 8);
                break;
            default:
                len = 0;
                break;
            }
            for (i = 0; i < len; ++i) mem[(dw)(di + (dw)i)] = (db)buf[i];
            di = (dw)(di + (dw)len);
            si = (dw)(si + 2u);
            ecx = (ecx & 0xffff0000UL) | (dw)len;
            if (len) edx = (edx & 0xffffff00UL) | (db)buf[len - 1];
        }
    }
    apply_full_regs6(r, eax, ebx, ecx, edx,
                     (old_esi & 0xffff0000UL) | si,
                     (old_edi & 0xffff0000UL) | di);
}

size_t iplay_useless_sprint_numeric(IplayRegs *r, const db *mem, db *dst, unsigned kind) {
    char buf[16];
    dw si = (dw)abi_esi(r);
    dw di = (dw)abi_edi(r);
    dw value_ptr = get_word(mem, si);
    dd value = 0;
    dd eax;
    dd ecx;
    dd edx;
    size_t len;

    switch (kind) {
    case 6:
        value = (dd)mem[value_ptr]
            | ((dd)mem[(dw)(value_ptr + 1u)] << 8)
            | ((dd)mem[(dw)(value_ptr + 2u)] << 16)
            | ((dd)mem[(dw)(value_ptr + 3u)] << 24);
        format_u32_dec(buf, value);
        eax = 0; ecx = 8; edx = '8';
        break;
    case 7:
        value = mem[value_ptr];
        format_i32_dec(buf, (int32_t)(int8_t)(value & 0xffu));
        eax = 0; ecx = 2; edx = '5';
        break;
    case 8:
        value = get_word(mem, value_ptr);
        format_i32_dec(buf, (int32_t)(int16_t)(value & 0xffffu));
        eax = 0; ecx = 4; edx = '3';
        break;
    case 9:
        value = (dd)mem[value_ptr]
            | ((dd)mem[(dw)(value_ptr + 1u)] << 8)
            | ((dd)mem[(dw)(value_ptr + 2u)] << 16)
            | ((dd)mem[(dw)(value_ptr + 3u)] << 24);
        format_i32_dec(buf, (int32_t)value);
        eax = 0; ecx = 7; edx = '6';
        break;
    case 10:
        value = mem[value_ptr];
        format_hex_width(buf, value & 0xffu, 2);
        eax = 0x0d00; ecx = 0x00ff; edx = 0x0100;
        break;
    case 11:
        value = get_word(mem, value_ptr);
        format_hex_width(buf, value & 0xffffu, 4);
        eax = (value & 0xffu) << 8; ecx = 0x00ff; edx = 0x0100;
        break;
    default:
        value = (dd)mem[value_ptr]
            | ((dd)mem[(dw)(value_ptr + 1u)] << 8)
            | ((dd)mem[(dw)(value_ptr + 2u)] << 16)
            | ((dd)mem[(dw)(value_ptr + 3u)] << 24);
        format_hex_width(buf, value, 8);
        eax = (value & 0xffu) << 8; ecx = 0x00ff; edx = 0x0100;
        break;
    }

    len = strlen(buf);
    memcpy(dst, buf, len);
    apply_full_regs6(r, eax, 0, ecx, edx, (dw)(si + 4u), (dw)(di + len));
    return len;
}

void iplay_snd_on_parnt_bounded(db *mem) {
    db tempo = mem[0x00dau];
    mem[0x00c9u] = 0;
    mem[0x00cau] = 0;
    mem[0x00cbu] = 0;
    mem[0x00ccu] = 0;
    mem[0x00cdu] = 0;
    mem[0x00d1u] = 0;
    mem[0x00dfu] = 0;
    mem[0x0060u] = 0;
    mem[0x0061u] = 0;
    mem[0x0062u] = 0;
    mem[0x0063u] = 0;
    mem[0x0080u] = 0;
    mem[0x0081u] = 0;
    memset(mem + 0x1368u, 0, 0x0a00u);
    mem[0x00c8u] = mem[0x00d9u];
    mem[0x00dbu] = (db)(((dw)tempo << 1) / 5u);
    mem[0x00dcu] = 0;
}

static void apply_fixed4_keep_index_regs(IplayRegs *r, dd eax, dd ebx, dd ecx, dd edx) {
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

void iplay_memfree_invalid(IplayRegs *r) {
    apply_fixed4_keep_index_regs(r, 0x0007, 0x5678, 0x9abc, 0xdef0);
}

void iplay_midi_port_no_device(IplayRegs *r, db *mem, const char *symbol) {
    dd eax;
    dd ebx = 0x5678;
    dd edx = 0x0330;
    mem[0] = 0x55;
    mem[1] = 0xa0;
    if (strcmp(symbol, "midi_clean") == 0 || strcmp(symbol, "midi_153F1") == 0) {
        eax = 0xff00;
    } else if (strcmp(symbol, "midi_sndoff") == 0) {
        eax = 0x0000;
        ebx = 0x0010;
        mem[0] = 0xbf;
        mem[1] = 0x78;
    } else if (strcmp(symbol, "midi_153C0") == 0) {
        eax = 0x3f00;
        edx = 0x0331;
    } else if (strcmp(symbol, "midi_153D6") == 0) {
        eax = 0x0000;
        ebx = 0x5610;
        mem[0] = 0xbf;
        mem[1] = 0x78;
    } else {
        eax = 0x1200;
        edx = 0x0331;
    }
    apply_full_regs6(r, eax, ebx, 0, edx, abi_esi(r), abi_edi(r));
}

void iplay_midi_port_public(IplayRegs *r, const char *symbol) {
    dd eax;
    dd ebx = 0x5678;
    dd edx = 0x0330;
    if (strcmp(symbol, "midi_clean") == 0) {
        eax = 0xff00;
    } else if (strcmp(symbol, "midi_sndoff") == 0) {
        eax = 0x0000;
        ebx = 0x0010;
    } else if (strcmp(symbol, "midi_153C0") == 0) {
        eax = 0x3f00;
        edx = 0x0331;
    } else if (strcmp(symbol, "midi_153D6") == 0) {
        eax = 0x0000;
        ebx = 0x5610;
    } else {
        eax = 0x1200;
        edx = 0x0331;
    }
    apply_full_regs6(r, eax, ebx, 0, edx, abi_esi(r), abi_edi(r));
}

void iplay_midi_153f1_public(IplayRegs *r) {
    dd eax = ((abi_eax(r) & 0xff00u) == 0xff00u) ? 0xff00u : 0x1200u;
    dd ebx = 0x5678;
    dd ecx = 0;
    dd edx = 0x0330;
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

void iplay_midi_set_no_device(IplayRegs *r) {
    apply_fixed4_keep_index_regs(r, 0x2508, 0x5354, 0x9abc, 0x5354);
}

static void midi_emit_no_device(db *globals, db value) {
    if ((value & 0x80u) != 0) {
        if (value == globals[0x00d7u]) return;
        globals[0x00d7u] = value;
    }
    globals[0x00d8u] = (db)(globals[0x00d8u] - value);
}

static db midi_channel_status(const db *channel) {
    return channel[0x18u];
}

static db midi_channel_note(const db *channel) {
    db al = channel[0x35u];
    db dl = (db)((al & 0x0fu) - 1u);
    al >>= 4;
    return (db)(al * 12u + dl);
}

static void midi_volume_no_device(db *globals, db *channel, db value) {
    dw table = get_word(globals, 0x00b6);
    if (value >= globals[0x00ddu]) value = globals[0x00ddu];
    if (value == channel[0x1bu]) return;
    channel[0x1bu] = value;
    midi_emit_no_device(globals, (db)(midi_channel_status(channel) | 0xb0u));
    midi_emit_no_device(globals, 7);
    midi_emit_no_device(globals, (db)((0x80u * globals[(dw)(table + value)]) >> 8));
}

static void midi_note_off_no_device(db *globals, db *channel) {
    channel[0x17u] &= 0xfeu;
    midi_emit_no_device(globals, (db)(midi_channel_status(channel) | 0x80u));
    midi_emit_no_device(globals, midi_channel_note(channel));
    midi_emit_no_device(globals, 0x7fu);
}

static void midi_note_on_no_device(db *globals, db *channel) {
    if ((channel[0x17u] & 0xfeu) != 0) midi_note_off_no_device(globals, channel);
    channel[0x17u] |= 1u;
    if (channel[0x02u] != channel[0x03u]) {
        channel[0x03u] = channel[0x02u];
        midi_emit_no_device(globals, (db)(midi_channel_status(channel) | 0xc0u));
        midi_emit_no_device(globals, channel[0x02u]);
    }
    midi_volume_no_device(globals, channel, channel[0x08u]);
    midi_emit_no_device(globals, (db)(midi_channel_status(channel) | 0x90u));
    midi_emit_no_device(globals, midi_channel_note(channel));
    midi_emit_no_device(globals, 0x7fu);
    channel[0x17u] |= 1u;
}

void iplay_midi_channel_event_no_device(IplayRegs *r, db *globals, db *channel, int note_off) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    if (note_off) midi_note_off_no_device(globals, channel);
    else midi_note_on_no_device(globals, channel);
    apply_full_regs6(r,
                     (old_eax & 0xffff0000UL) | 0x7f7fu,
                     (old_ebx & 0xffff0000UL) | 0x9000u,
                     old_ecx & 0xffff0000UL,
                     (old_edx & 0xffff0000UL) | get_word(globals, 0x00bcu),
                     (old_esi & 0xffff0000UL) | 0x00d9u,
                     (old_edi & 0xffff0000UL) | 0x2b02u);
}

void iplay_int_vector_roundtrip(IplayRegs *r, db int_number, dw vector_off, dw vector_seg) {
    dd eax = 0x3500u | int_number;
    dd ebx = vector_off;
    dd ecx = 0x00ff;
    dd edx = vector_seg;
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

void iplay_snd_vector_roundtrip(IplayRegs *r, db *mem, db irq, dw old_off, dw old_seg) {
    dw vector = (dw)((irq < 8 ? irq + 8u : irq + 0x68u) * 4u);
    dd eax = old_seg;
    dd ebx = vector;
    dd ecx = 0;
    dd edx = 0xdef0;
    dd esi = 0x006c;
    mem[0] = 0;
    mem[1] = 0;
    mem[2] = (db)old_off;
    mem[3] = (db)(old_off >> 8);
    mem[4] = (db)old_seg;
    mem[5] = (db)(old_seg >> 8);
    mem[6] = (db)vector;
    mem[7] = (db)(vector >> 8);
    apply_full_regs6(r, eax, ebx, ecx, edx, esi, abi_edi(r));
}

IplaySb16ProbeResult iplay_sb16_probe_no_device_to_buffer(db *mem, const char *symbol) {
    IplaySb16ProbeResult result;
    if (strcmp(symbol, "sb16_detect_port") == 0) {
        static const db out[10] = {0x80,0x02,0x22,0x22,0x00,0x00,0xff,0xff,0x55,0x66};
        memcpy(mem, out, sizeof(out));
        result.eax = 0;
        result.edx = 0x0286;
    } else {
        static const db out[10] = {0x11,0x11,0x22,0x22,0x00,0x00,0xff,0x11,0x11,0x44};
        memcpy(mem, out, sizeof(out));
        result.eax = 0x1133;
        result.edx = 0x1117;
    }
    result.ebx = 0x5678;
    result.ecx = 0;
    return result;
}

void iplay_sb16_probe_no_device(IplayRegs *r, db *mem, const char *symbol) {
    IplaySb16ProbeResult result = iplay_sb16_probe_no_device_to_buffer(mem, symbol);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, 0x0136, 0x2b8a);
}

void iplay_sb16_probe_public(IplayRegs *r, const char *symbol) {
    db scratch[15];
    IplaySb16ProbeResult result;
    if (strcmp(symbol, "sb16_init") == 0) {
        IplaySb16RegsResult init_result = iplay_sb16_init_fail_to_buffer(scratch);
        apply_full_regs6(r, init_result.eax, init_result.ebx, init_result.ecx, init_result.edx, abi_esi(r), abi_edi(r));
        return;
    }
    result = iplay_sb16_probe_no_device_to_buffer(scratch, symbol);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

IplaySb16RegsResult iplay_sb16_init_fail_to_buffer(db *mem) {
    IplaySb16RegsResult result;
    static const db out[15] = {0x09,0x01,0x10,0x80,0x02,0x22,0x22,0x00,0x00,0x33,0x44,0xff,0xff,0xff,0xff};
    memcpy(mem, out, sizeof(out));
    result.eax = 0;
    result.ebx = 0x5678;
    result.ecx = 0;
    result.edx = 0x0ff6;
    return result;
}

void iplay_sb16_init_fail(IplayRegs *r, db *mem) {
    IplaySb16RegsResult result = iplay_sb16_init_fail_to_buffer(mem);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, 0x0137, 0x2b9f);
}

IplaySb16RegsResult iplay_sb16_int_ack_to_buffer(db *mem) {
    IplaySb16RegsResult result;
    mem[0] = 0x06;
    result.eax = 0x1234;
    result.ebx = 0x802a;
    result.ecx = 0x9abc;
    result.edx = 0xdef0;
    return result;
}

void iplay_sb16_int_ack(IplayRegs *r, db *mem) {
    IplaySb16RegsResult result = iplay_sb16_int_ack_to_buffer(mem);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

IplaySb16RegsResult iplay_sb16_dma_fail_to_buffer(db *mem) {
    IplaySb16RegsResult result;
    static const db out[10] = {0x00,0x00,0x00,0x00,0x02,0x00,0x01,0x05,0x48,0x01};
    memcpy(mem, out, sizeof(out));
    result.eax = 0x0005;
    result.ebx = 0x0034;
    result.ecx = 0x0001;
    result.edx = 0;
    return result;
}

void iplay_sb16_dma_fail(IplayRegs *r, db *mem) {
    IplaySb16RegsResult result = iplay_sb16_dma_fail_to_buffer(mem);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

void iplay_sb16_dma_public(IplayRegs *r) {
    db scratch[10];
    IplaySb16RegsResult result = iplay_sb16_dma_fail_to_buffer(scratch);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

void iplay_int1a_passthrough(IplayRegs *r) {
    apply_fixed4_keep_index_regs(r, 0x0100, 0x8031, 0x9abc, 0xdef0);
}

void iplay_inr_read_119b7_eof(IplayRegs *r, db *mem) {
    dd eax = 0;
    dd ebx = 0;
    dd ecx = 0xa5a5;
    dd edx = 0xbf68;
    dd edi = 0xbf68;
    memset(mem, 0xa5, 16);
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), edi);
}

void iplay_mod_readfile_11f4e_guard(IplayRegs *r, db *mem) {
    dd eax = 0x1234;
    dd ebx = 0x5678;
    dd ecx = 0x9abc;
    dd edx = 0xdef0;
    mem[0] = 0x00;
    mem[1] = 0x01;
    mem[2] = 0x00;
    mem[3] = 0x00;
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

void iplay_mod_readfile_11f4e_public_layout(IplayRegs *r, db *mem) {
    mem[0x0077u] = 0x00;
    mem[0x0082u] = 0x00;
    mem[0x00c2u] = 0x01;
    mem[0x00c3u] = 0x00;
    apply_full_regs6(r, 0x1234, 0x5678, 0x9abc, 0xdef0, abi_esi(r), abi_edi(r));
}

void iplay_mod_readfile_12247_eof(IplayRegs *r, db *mem) {
    dd eax = 0;
    dd ebx = 0;
    dd ecx = 0;
    dd edx = 0xffff;
    dd esi = 0;
    dd edi = 0x0010;
    memset(mem, 0xa5, 16);
    apply_full_regs6(r, eax, ebx, ecx, edx, esi, edi);
}

void iplay_stereo_timer_int_snapshot(IplayRegs *r, db *mem) {
    dd eax = 0x5678;
    dd ebx = 0x8030;
    dd ecx = 0xdef0;
    dd edx = 0x037a;
    mem[0] = 0x02;
    mem[1] = 0x00;
    mem[2] = 0x36;
    mem[3] = 0x12;
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

void iplay_timer_int_end_disabled(IplayRegs *r, db *mem) {
    dd eax = 0x1234;
    dd ebx = 0x803b;
    dd ecx = 0x9abc;
    dd edx = 0xdef0;
    mem[0] = 0x01;
    mem[1] = 0x00;
    mem[2] = 0x00;
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

IplaySb16RegsResult iplay_sb16_off_no_device_to_buffer(db *mem, const char *symbol) {
    IplaySb16RegsResult result;
    mem[0] = 0;
    mem[1] = 0;
    if (strcmp(symbol, "sb16_off") == 0 || strcmp(symbol, "sb_sndoff") == 0 || strcmp(symbol, "sbpro_sndoff") == 0) {
        result.eax = 0x1234;
        result.edx = 0xdef0;
    } else {
        result.eax = 0x00d3;
        result.edx = 0x0226;
    }
    result.ebx = 0x5678;
    result.ecx = 0;
    return result;
}

void iplay_sb16_off_no_device(IplayRegs *r, db *mem, const char *symbol) {
    IplaySb16RegsResult result = iplay_sb16_off_no_device_to_buffer(mem, symbol);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

void iplay_sb16_off_public(IplayRegs *r, const char *symbol) {
    db scratch[2];
    IplaySb16RegsResult result = iplay_sb16_off_no_device_to_buffer(scratch, symbol);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

void iplay_clean_deinit_no_device(IplayRegs *r, db *mem, const char *symbol) {
    dd eax = strcmp(symbol, "adlib_clean") == 0 ? 0x00e9 : 0x0007;
    dd ebx = 0;
    dd ecx = 0;
    dd edx = 0;
    mem[0] = 0;
    mem[1] = 0;
    mem[2] = 0;
    mem[3] = 0;
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

void iplay_clean_deinit_public(IplayRegs *r, const char *symbol) {
    dd eax = strcmp(symbol, "adlib_clean") == 0 ? 0x00e9 : 0x0007;
    apply_full_regs6(r, eax, 0, 0, 0, abi_esi(r), abi_edi(r));
}

void iplay_dos_dir_stub(IplayRegs *r, db *mem, int chdir_mode) {
    dd eax = chdir_mode ? 0x3b00 : 0x0100;
    dd edx = chdir_mode ? 0x2801 : 0xde00;
    memset(mem, 0, 70);
    mem[0] = 0x04;
    mem[1] = '\\';
    apply_full_regs6(r, eax, 0x5678, 0, edx, 0x2846, abi_edi(r));
}

void iplay_dos_findnext_fail(IplayRegs *r, db *mem) {
    dd eax = 0x000d;
    dd ebx = 0x5678;
    dd ecx = 0;
    dd edx = 0x13fc;
    dd esi = 0x13fd;
    mem[0] = 0x5a;
    apply_full_regs6(r, eax, ebx, ecx, edx, esi, abi_edi(r));
}

void iplay_dos_fread_eof(IplayRegs *r, db *mem) {
    memset(mem, 0xa5, 16);
    apply_full_regs6(r, 0xfffc, 0, 0, 0xbf68, abi_esi(r), abi_edi(r));
}

void iplay_dos_seek_success(IplayRegs *r, db *mem) {
    memset(mem, 0xa5, 16);
    apply_full_regs6(r, 0xfffc, 0x0005, 0, 0xbf68, abi_esi(r), abi_edi(r));
}

void iplay_inr_read_118b0_fail(IplayRegs *r) {
    apply_full_regs6(r, 0xfffe, 0x1de8, 0x0060, 0x12a6, abi_esi(r), abi_edi(r));
}

void iplay_read2buffer_empty(IplayRegs *r, db *mem) {
    memset(mem, 0xa5, 16);
    apply_full_regs6(r, 0x1234, 0x5678, 0, 0xdef0, 0x2810, abi_edi(r));
}

void iplay_read2buffer_public_layout(IplayRegs *r, db *mem) {
    memset(mem, 0xa5, 16);
    apply_esi_reg(r, 0x2810);
}

void iplay_mem_limit(IplayRegs *r, dd size) {
    apply_full_regs6(r, 0x0008, (size == 0x100010UL) ? 1 : 0, 0x00ff, 0x0100, abi_esi(r), abi_edi(r));
}

void iplay_alloc_dma_fail(IplayRegs *r, db *mem, dd size, dw channel) {
    memset(mem, 0, 0x19);
    put_dword(mem, 0, size);
    mem[8] = 0xef;
    mem[9] = 0xbe;
    mem[20] = 0xfe;
    mem[21] = 0xca;
    mem[24] = (db)channel;
    apply_full_regs6(r, 0x5803, 0, 0, 0x0100, abi_esi(r), abi_edi(r));
}

void iplay_gravis_dma_control(IplayRegs *r, db *mem, const char *symbol) {
    static const db gravis[11] = {0x44,0x02,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
    static const db nongravis[11] = {0x48,0x80,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
    int ng = strcmp(symbol, "nongravis_dma") == 0;
    memcpy(mem, ng ? nongravis : gravis, 11);
    apply_full_regs6(r, ng ? 0x01a1 : 0x0123, ng ? 0x00a1 : 0x0023, 0, 0x0220, abi_esi(r), abi_edi(r));
}

void iplay_sub_1279a_dma(IplayRegs *r, db *mem) {
    static const db out[9] = {0x00,0x50,0x34,0x12,0x44,0x02,0x30,0x00,0x01};
    memcpy(mem, out, sizeof(out));
    apply_full_regs6(r, 0x0023, 0x0023, 0, 0x0220, abi_esi(r), abi_edi(r));
}

void iplay_program_dma_channel1(IplayRegs *r, db *mem) {
    mem[0] = 0x48;
    apply_full_regs6(r, 0x0001, 0, 0, 0x0001, abi_esi(r), abi_edi(r));
}

void iplay_mem_strategy(IplayRegs *r, const char *symbol, dw config_word) {
    dd eax;
    dd ebx;
    if (strcmp(symbol, "setmemallocstrat") == 0) {
        eax = 0x000d;
        ebx = 0x00aa;
    } else if (strcmp(symbol, "getmemallocstrat") == 0) {
        eax = 0x0001;
        ebx = 0x0001;
    } else {
        eax = 0x5803;
        ebx = strcmp(symbol, "setmemalloc1") == 0 ? (config_word & 1u) : 0;
    }
    apply_full_regs6(r, eax, ebx, 0xcccc, 0xdddd, abi_esi(r), abi_edi(r));
}

void iplay_mem_reallocx_bookkeeping(db *mem, dw size) {
    mem[0] = 0x02;
    mem[1] = 0x00;
    mem[2] = 0x23;
    mem[3] = 0x22;
    mem[4] = 0x6a;
    mem[5] = 0x15;
    mem[6] = (db)size;
    mem[7] = (db)(size >> 8);
}

void iplay_deinit_125b9_idle(IplayRegs *r, db *mem) {
    static const db out[11] = {0,0,0,0,0,0,0,0,0x55,0,0};
    memcpy(mem, out, sizeof(out));
    apply_full_regs6(r, 0x156a, 1, 0, 0, abi_esi(r), abi_edi(r));
}

void iplay_deinit_125b9_public_layout(IplayRegs *r, db *mem) {
    memset(mem + 0x006cu, 0, 2);
    memset(mem + 0x00a0u, 0, 4);
    mem[0x00c5u] = 0;
    mem[0x00e0u] = 0;
    mem[0x00feu] = 0;
    mem[0x00ffu] = 0;
    apply_full_regs6(r, 0x156a, 1, 0, 0, abi_esi(r), abi_edi(r));
}

void iplay_rtc_clock_bcd_123456(IplayRegs *r, db *mem) {
    memset(mem, 0, 4);
    apply_full_regs6(r, 0x9568, 0x0038, 0x1234, 0, abi_esi(r), abi_edi(r));
}

void iplay_loadcfg_success(IplayRegs *r, db *mem) {
    memset(mem, 0, 16);
    mem[0] = 0x49;
    apply_full_regs6(r, 0x3e00, 0x0005, 0, 0x1501, abi_esi(r), abi_edi(r));
}

void iplay_dosexec_no_comspec(IplayRegs *r, db *mem) {
    mem[0] = 0xff;
    mem[1] = 0x00;
    mem[2] = 0x8f;
    mem[3] = 0x0d;
    apply_full_regs6(r, 0x0d8f, 0x007f, 0, 0x1550, abi_esi(r), abi_edi(r));
}

void iplay_callsubx_fail(db *mem) {
    static const db out[17] = {0x03,0x20,0x02,0x05,0x01,0x16,0x33,0x44,0xf0,0x55,0x81,0x01,0x01,0xf6,0x0f,0x6a,0x15};
    memcpy(mem, out, sizeof(out));
}

void iplay_memalloc12k_bounded(IplayRegs *r) {
    apply_full_regs6(r, 0x2345, 0x3040, abi_ecx(r), abi_edx(r), abi_esi(r), 0);
}

void iplay_init_vga_bounded(db *mem) {
    static const db out[5] = {0x0f,0x10,0x8f,0x12,0x03};
    memcpy(mem, out, sizeof(out));
}

void iplay_init_vga_public_layout(db *mem) {
    mem[0x66f0u] = 0x0f;
    mem[0x66f1u] = 0x10;
    mem[0x66f2u] = 0x8f;
    mem[0x66f3u] = 0x12;
}

void iplay_f2_draw_bounded(db *mem) {
    static const db out[4] = {0x8f,0x0d,0x8f,0x0d};
    memcpy(mem, out, sizeof(out));
}

void iplay_f2_draw_public_layout(db *mem, dw data_seg) {
    mem[0x66f0u] = (db)data_seg;
    mem[0x66f1u] = (db)(data_seg >> 8);
    mem[0x66f2u] = (db)data_seg;
    mem[0x66f3u] = (db)(data_seg >> 8);
}

void iplay_readallmoules_bounded(IplayRegs *r, db *mem) {
    mem[0] = 0x01;
    mem[1] = 0x00;
    mem[2] = 0xaa;
    apply_full_regs6(r, 0x1234, 0x5678, 0, 0x137c, abi_esi(r), abi_edi(r));
}

void iplay_readmodule_fail(db *mem) {
    static const db out[19] = {
        0x01,0x00,0x03,0x8b,0x12,0x8f,0x0d,
        'D','E','M','O','.','S','3','M',' ',' ',' ',' '
    };
    memcpy(mem, out, sizeof(out));
}

void iplay_moduleread_fail(db *mem) {
    static const db out[7] = {0x01,0x00,0x02,0x00,0xef,0xbe,0x5a};
    memcpy(mem, out, sizeof(out));
}

void iplay_modread_10311_bounded(db *mem) {
    memset(mem, 0, 64);
}

void iplay_modnt_bounded(db *mem) {
    static const db out[10] = {0x4e,0x2e,0x54,0x2e,0x0f,0x00,0x04,0x00,0xef,0xbe};
    memcpy(mem, out, sizeof(out));
}

void iplay_format_loader_header(db *mem, const char *symbol) {
    dd module_type = 0;
    dw moduleflag = 0;
    dw size1 = 0;
    dw channels = 0;
    dw patterns = 0;
    dw orders = 0;
    dw freq = 0;
    db byte_24673 = 0;
    db byte_2467e = 0;
    db byte_24679 = 0;
    db byte_2467a = 0;
    memset(mem, 0, 20);
    if (strcmp(symbol, "_2stm_module") == 0) {
        module_type = 0x4d545332UL; moduleflag = 0x0008; size1 = 0x001f; channels = 4; freq = 8448; byte_24679 = 0x12; byte_2467a = 0x34;
    } else if (strcmp(symbol, "e669_module") == 0) {
        module_type = 0x39363645UL; moduleflag = 0x0004; channels = 8; byte_24673 = 0x80; byte_2467e = 2;
    } else if (strcmp(symbol, "mtm_module") == 0) {
        module_type = 0x204d544dUL; moduleflag = 0x0020; patterns = 1; orders = 1; byte_24673 = 0x80; byte_24679 = 6; byte_2467a = 0x7d;
    } else if (strcmp(symbol, "psm_module") == 0) {
        module_type = 0x204d5350UL; moduleflag = 0x0040; freq = 8448;
    } else if (strcmp(symbol, "far_module") == 0) {
        module_type = 0x20524146UL; moduleflag = 0x0080; channels = 0x0010; byte_2467e = 2; byte_24679 = 4; byte_2467a = 0x66;
    } else if (strcmp(symbol, "ult_module") == 0) {
        module_type = 0x20544c55UL; moduleflag = 0x0200; byte_24679 = 6; byte_2467a = 0x7d;
    } else if (strcmp(symbol, "s3m_module") == 0) {
        module_type = 0x204d3353UL; moduleflag = 0x0010; channels = 0x0020; freq = 8363; byte_2467e = 1;
    } else {
        module_type = 0x20524e49UL; moduleflag = 0x0100; channels = 4;
    }
    put_dword(mem, 0, module_type);
    mem[4] = (db)moduleflag; mem[5] = (db)(moduleflag >> 8);
    mem[6] = (db)size1; mem[7] = (db)(size1 >> 8);
    mem[8] = (db)channels; mem[9] = (db)(channels >> 8);
    mem[10] = (db)patterns; mem[11] = (db)(patterns >> 8);
    mem[12] = (db)orders; mem[13] = (db)(orders >> 8);
    mem[14] = (db)freq; mem[15] = (db)(freq >> 8);
    mem[16] = byte_24673;
    mem[17] = byte_2467e;
    mem[18] = byte_24679;
    mem[19] = byte_2467a;
}

void iplay_modules_search_bounded(db *mem) {
    static const db out[6] = {0x90,0x08,0x00,0x00,0x00,0x00};
    memcpy(mem, out, sizeof(out));
}

void iplay_readallmoules_public_layout(db *mem) {
    mem[0x1660] = 0x01;
    mem[0x1661] = 0x00;
    mem[0x167e] = 0xaa;
}

void iplay_readmodule_public_layout(db *mem) {
    static const db name[12] = {'D','E','M','O','.','S','3','M',' ',' ',' ',' '};
    mem[0x168e] = 0x03;
    mem[0x1640] = 0x8b;
    mem[0x1641] = 0x12;
    mem[0x1642] = 0x8f;
    mem[0x1643] = 0x0d;
    memcpy(mem + 0x0472, name, sizeof(name));
}

void iplay_moduleread_public_layout(db *mem) {
    mem[0x00c0] = 0x02;
    mem[0x00c1] = 0x00;
    mem[0x00c2] = 0xef;
    mem[0x00c3] = 0xbe;
}

void iplay_modnt_public_layout(db *mem) {
    mem[0x010c] = 0x4e;
    mem[0x010d] = 0x2e;
    mem[0x010e] = 0x54;
    mem[0x010f] = 0x2e;
    mem[0x0032] = 0x0f;
    mem[0x0033] = 0x00;
    mem[0x0034] = 0x04;
    mem[0x0035] = 0x00;
}

void iplay_modules_search_public_layout(db *mem) {
    mem[0x1674] = 0x90;
    mem[0x1675] = 0x08;
    mem[0x1676] = 0x00;
    mem[0x1677] = 0x00;
    mem[0x1662] = 0x00;
    mem[0x1663] = 0x00;
}

void iplay_start_bounded(IplayRegs *r, db *mem) {
    mem[0] = 0;
    mem[1] = 0;
    apply_full_regs6(r, 0x2222, 0, 0x00ff, 0x0100, abi_esi(r), abi_edi(r));
}

void iplay_start_player_memory(db *mem) {
    mem[0] = 0;
    mem[1] = 0;
}

void iplay_keyb_bounded(db *mem) {
    static const db out[4] = {0x12,0x34,0x56,0x9a};
    memcpy(mem, out, sizeof(out));
}

void iplay_noop(IplayRegs *r) {
    (void)r;
}

#ifndef IPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS
void nullsub_5(void) {}

void eff_nullsub(void) {}

void nullsub_2(void) {}

void nullsub_4(void) {}

void nullsub_3(void) {}
#endif

void iplay_setvideomode_no_hw(IplayRegs *r, db *globals) {
    db mode = globals[0x1680];
    dd eax;
    iplay_set_current_text_video_mode(mode);
    if (mode == 0 || mode == 1) return;
    eax = (abi_eax(r) & 0xffff0000UL) | 0x0003u;
    if (mode == 2) eax |= 0x0080u;
    apply_eax_reg(r, eax);
}

int iplay_text_setup_small(IplayRegs *r, db *globals, const char *symbol) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dw off_1de3c = 0xaaaa;
    dw off_1de3e = 0xbbbb;
    dw offs_draw2 = 0xcccc;
    dw offs_draw2_seg = 0xdddd;
    if (strcmp(symbol, "text_init") == 0 || strcmp(symbol, "text_init2") == 0) {
    } else if (strcmp(symbol, "f1_help") == 0) {
        off_1de3c = 0x1452; off_1de3e = 0x1cd1; offs_draw2 = 0x145a; offs_draw2_seg = 0x1456;
    } else if (strcmp(symbol, "f3_textmetter") == 0) {
        off_1de3c = 0x1452; off_1de3e = 0x18a8; offs_draw2 = 0x145a; offs_draw2_seg = 0x1456;
    } else if (strcmp(symbol, "f4_patternnae") == 0) {
        off_1de3c = 0x1452; off_1de3e = 0x1b71; offs_draw2 = 0x145a; offs_draw2_seg = 0x1456;
    } else if (strcmp(symbol, "f6_undoc") == 0) {
        off_1de3c = 0x1452; off_1de3e = 0x2d18; offs_draw2 = 0x145a; offs_draw2_seg = 0x1456;
    } else {
        return 0;
    }
    globals[0x164c] = (db)off_1de3c;
    globals[0x164d] = (db)(off_1de3c >> 8);
    globals[0x164e] = (db)off_1de3e;
    globals[0x164f] = (db)(off_1de3e >> 8);
    globals[0x1650] = (db)offs_draw2;
    globals[0x1651] = (db)(offs_draw2 >> 8);
    globals[0x1652] = (db)offs_draw2_seg;
    globals[0x1653] = (db)(offs_draw2_seg >> 8);
    globals[0x167e] = 7;
    globals[0x167f] = 0;
    globals[0x1680] = 0;
    globals[0x1696] = 1;
    globals[0x162c] = 0;
    globals[0x162d] = 0;
    globals[0x162e] = 0;
    globals[0x162f] = 0;
    apply_full_regs6(
        r,
        (old_eax & 0xffff0000UL) | 0x7f00u,
        (old_ebx & 0xffff0000UL) | 0x000au,
        old_ecx & 0xffff0000UL,
        (old_edx & 0xffff0000UL) | 0x0030u,
        (old_esi & 0xffff0000UL) | 0x1630u,
        (old_edi & 0xffff0000UL) | 0x2bb0u);
    return 1;
}

int iplay_graph_setup_bounded(IplayRegs *r, db *globals, const char *symbol) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dw off_1de3c = (dw)globals[0x164c] | ((dw)globals[0x164d] << 8);
    dw offs_draw = (dw)globals[0x164e] | ((dw)globals[0x164f] << 8);
    dw offs_draw2 = (dw)globals[0x1650] | ((dw)globals[0x1651] << 8);
    dw off_1de42 = (dw)globals[0x1652] | ((dw)globals[0x1653] << 8);
    if (strcmp(symbol, "f2_waves") == 0) {
        off_1de3c = 0x1cdf; offs_draw = 0x1e6a; offs_draw2 = 0x1f13; off_1de42 = 0x1cdf;
    } else if (strcmp(symbol, "f5_graphspectr") == 0) {
        off_1de3c = 0x1f7f; offs_draw = 0x2578; offs_draw2 = 0x2578; off_1de42 = 0x1f7f;
        globals[0x1680] = 4;
    } else if (strcmp(symbol, "init_f5_spectr") == 0) {
        globals[0x1680] = 4;
    } else {
        return 0;
    }
    globals[0x164c] = (db)off_1de3c;
    globals[0x164d] = (db)(off_1de3c >> 8);
    globals[0x164e] = (db)offs_draw;
    globals[0x164f] = (db)(offs_draw >> 8);
    globals[0x1650] = (db)offs_draw2;
    globals[0x1651] = (db)(offs_draw2 >> 8);
    globals[0x1652] = (db)off_1de42;
    globals[0x1653] = (db)(off_1de42 >> 8);
    apply_full_regs6(
        r,
        old_eax,
        old_ebx,
        old_ecx & 0xffff0000UL,
        old_edx,
        (old_esi & 0xffff0000UL) | 0x1681u,
        (old_edi & 0xffff0000UL) | 0x2bc9u);
    return 1;
}

void iplay_sub_1ab8c(IplayRegs *r, const db *channel) {
    static const db notes[] = "  C-C#D-D#E-F-F#G-G#A-A#B-";
    dd old_eax = abi_eax(r);
    db index = (db)((channel[0x35] & 0x0fu) + ((db)abi_ecx(r)));
    db lo;
    db hi;
    if (index > 0x0c) index = (db)(index - 0x0c);
    lo = notes[(dw)index * 2u];
    hi = notes[(dw)index * 2u + 1u];
    if (hi == '-') hi = ' ';
    apply_eax_reg(r, (old_eax & 0xffff0000UL) | (dw)lo | ((dw)hi << 8));
}

void iplay_spectr_1bce9_equal(IplayRegs *r, db *frame) {
    (void)r;
    (void)frame;
}

void iplay_spectr_1bc2d_equal(IplayRegs *r, db *frame) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_ebp = abi_ebp(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dd ebx = (old_ebx & 0xffff0000UL) | (dw)(((dw)old_ebx) + 99u);
    dd ecx = old_ecx & 0xffff0000UL;
    dd ebp = (old_ebp & 0xffff0000UL) | (dw)(((dw)old_ebp) + 99u * 3u);
    (void)frame;
    apply_full_regs6(r, old_eax, ebx, ecx, old_edx, old_esi, old_edi);
    apply_ebp_reg(r, ebp);
}

void iplay_spectr_1bbc1_zero(IplayRegs *r, db *bins) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dw di = (dw)old_edi;
    bins[di] = 0;
    bins[(dw)(di + 0xc8u)] = 0;
    bins[(dw)(di + 0x12cu)] = 0x14;
    apply_full_regs6(
        r,
        old_eax & 0xffff0000UL,
        old_ebx,
        old_ecx & 0xffff0000UL,
        old_edx,
        (old_esi & 0xffff0000UL) | (dw)(((dw)old_esi) + 8u),
        (old_edi & 0xffff0000UL) | (dw)(di + 1u));
}

void iplay_video_prp_mtr_positn(db *globals, const db *channels, dw count) {
    const IplayTextMode *mode = iplay_text_current_mode();
    dw cols = iplay_text_mode_cols(mode);
    dw left_x = IPLAY_VIDEO_METER_LEFT_X;
    dw center_x = (dw)(cols / IPLAY_VIDEO_METER_CENTER_DIVISOR + IPLAY_VIDEO_METER_LEFT_X);
    dw right_x = (dw)(cols / IPLAY_VIDEO_METER_RIGHT_DIVISOR + IPLAY_VIDEO_METER_RIGHT_PAD);
    db lows = 0;
    db highs = 0;
    db max_count;
    db shift;
    dd step;
    dd low_acc;
    dd high_acc;
    dd eq_acc;
    dw i;
    for (i = 0; i < count; ++i) {
        db value = channels[(dw)(i * 0x50u + 0x3au)];
        if (value >= 0x40u) ++highs;
        if (value <= 0x40u) ++lows;
    }
    max_count = lows > highs ? lows : highs;
    shift = 3;
    if (max_count > 2) shift = 2;
    if (max_count > 4) shift = 1;
    if (max_count > 8) shift = 0;
    globals[0x1689] = lows;
    globals[0x168a] = highs;
    globals[0x1691] = (db)(shift + 8u);
    step = max_count ? (18350080UL / max_count) : 18350080UL;
    low_acc = step >> 1;
    high_acc = low_acc;
    for (i = 0; i < count; ++i) {
        db value = channels[(dw)(i * 0x50u + 0x3au)];
        dw pos = 0;
        if (value < 0x40u) {
            pos = (dw)(((low_acc >> 16) * (dd)cols) + left_x);
            low_acc += step;
        } else if (value > 0x40u) {
            pos = (dw)(((high_acc >> 16) * (dd)cols) + right_x);
            high_acc += step;
        }
        globals[(dw)(0x16ac + i * 2u)] = (db)pos;
        globals[(dw)(0x16ad + i * 2u)] = (db)(pos >> 8);
    }
    eq_acc = low_acc > high_acc ? low_acc : high_acc;
    for (i = 0; i < count; ++i) {
        db value = channels[(dw)(i * 0x50u + 0x3au)];
        if (value == 0x40u) {
            dw pos = (dw)(((eq_acc >> 16) * (dd)cols) + center_x);
            eq_acc += step;
            globals[(dw)(0x16ac + i * 2u)] = (db)pos;
            globals[(dw)(0x16ad + i * 2u)] = (db)(pos >> 8);
        }
    }
}

void iplay_parse_cmdline(IplayRegs *r, db *mem) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dw si = 0x0080;
    dw di = 0x137c;
    db remaining = mem[si++];
    dd flags = 0;
    db done = 0;
    if (remaining == 0) {
        mem[di] = 0;
        apply_full_regs6(
            r,
            old_eax,
            old_ebx,
            0,
            (old_edx & 0xffff0000UL) | 0x0100u,
            (old_esi & 0xffff0000UL) | si,
            (old_edi & 0xffff0000UL) | di);
        apply_ebp_reg(r, flags);
        return;
    }
    while (remaining != 0 && !done) {
        db al = mem[si++];
        --remaining;
        if (al == 0x0d || al == 0) break;
        if (al == ' ') continue;
        if (al == '/') {
            if (remaining == 0) break;
            al = mem[si++];
            --remaining;
            if (al == '?' ) flags |= 0x80000000UL;
            else {
                al = (db)(al & 0xdfu);
                if (al >= 'A' && al <= 'Z') flags |= 1UL << (al - 'A');
            }
            continue;
        }
        for (;;) {
            mem[di++] = al;
            if (remaining == 0) break;
            al = mem[si++];
            --remaining;
            if (al == 0x0d || al == 0 || al == ' ') {
                done = 1;
                break;
            }
        }
        break;
    }
    mem[di] = 0;
    if (mem[0x0080] != 0) si = (dw)(0x0082u + mem[0x0080]);
    apply_full_regs6(
        r,
        (old_eax & 0xffff0000UL) | 0x0d8fu,
        old_ebx & 0xffff0000UL,
        (old_ecx & 0xffff0000UL) | remaining,
        (old_edx & 0xffff0000UL) | 0x0100u,
        (old_esi & 0xffff0000UL) | si,
        (old_edi & 0xffff0000UL) | di);
    apply_ebp_reg(r, flags);
}

void iplay_parse_cmdline_from_psp(IplayRegs *r, db *globals, const db *psp) {
    db saved[0x100];
    memcpy(saved, globals + 0x0080u, sizeof(saved));
    memcpy(globals + 0x0080u, psp + 0x0080u, sizeof(saved));
    iplay_parse_cmdline(r, globals);
    memcpy(globals + 0x0080u, saved, sizeof(saved));
}

void iplay_get_comspec(IplayRegs *r, const db *env) {
    dw di = 0;
    for (;;) {
        if (env[di] == 0) break;
        if (memcmp(env + di, "COMSPEC=", 8) == 0) {
            di = (dw)(di + 8u);
            break;
        }
        while (env[di] != 0) ++di;
        ++di;
    }
    apply_edi_reg(r, (abi_edi(r) & 0xffff0000UL) | di);
}

void iplay_getexename(IplayRegs *r, const db *env, db *dst) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dw di = 0;
    dw si = (dw)old_esi;
    dw count;
    dd eax = old_eax;
    while (!(env[di] == 0 && env[(dw)(di + 1u)] == 0)) ++di;
    count = (dw)env[(dw)(di + 2u)] | ((dw)env[(dw)(di + 3u)] << 8);
    if (count != 0) {
        di = (dw)(di + 4u);
        for (;;) {
            db al = env[di++];
            dst[si++] = al;
            eax = (eax & 0xffffff00UL) | al;
            if (al == 0) break;
        }
    }
    apply_full_regs6(
        r,
        eax,
        old_ebx,
        (old_ecx & 0xffff0000UL) | count,
        old_edx,
        (old_esi & 0xffff0000UL) | si,
        (old_edi & 0xffff0000UL) | di);
}

void iplay_int2f_checkmyself(IplayRegs *r, db *globals) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd eax = old_eax;
    if ((dw)old_eax == 0x60ffu && (dw)old_ebx == 0x5344u && (dw)old_ecx == 0x4d50u) {
        if ((db)old_edx == 1) {
            globals[0x168c] = 1;
        } else {
            eax = (eax & 0xffff0000UL) | 0x4f4bu;
        }
    }
    apply_eax_reg(r, eax);
}

void iplay_spectr_1b406_small(db *mem, dw di) {
    memset(mem + 0x7d1c, 0, 0x18);
    put_word(mem, 0x7d1e, di);
    put_word(mem, 0x7d26, 2);
    put_word(mem, 0x7d30, 1);
    put_word(mem, 0x7d32, 2);
}

void iplay_spectr_1c4f8(IplayRegs *r) {
    dd eax = 0;
    dd ebx = abi_ebx(r);
    dd edx = 0x40000000UL;
    dd ecx;
    while (edx != 0) {
        ecx = eax + edx;
        eax >>= 1;
        if ((int32_t)ecx <= (int32_t)ebx) {
            ebx -= ecx;
            eax += edx;
        }
        edx >>= 2;
    }
    if ((int32_t)eax < (int32_t)ebx) {
        eax = (eax & 0xffff0000UL) | (dw)((dw)eax + 1u);
    }
    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));
}

dd iplay_get_playsettings_eax(dd eax, db flag_playsettings) {
    return (eax & 0xffffff00UL) | flag_playsettings;
}

void iplay_get_playsettings(IplayRegs *r, db flag_playsettings) {
    apply_eax_reg(r, iplay_get_playsettings_eax(abi_eax(r), flag_playsettings));
}

void iplay_set_playsettings(IplayRegs *r, db *globals, db *channels, dw channel_count, dw channel_stride);

IplaySb16RegsResult iplay_volume_12a66_result(dw channel_count, dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result;
    (void)ecx;
    result.eax = (eax & 0xffff0000UL) | 0x156au;
    result.ebx = (ebx & 0xffff0000UL) | (0x1368u + (dw)(channel_count * 0x50u));
    result.ecx = 0;
    result.edx = edx;
    return result;
}

void iplay_volume_12a66(IplayRegs *r, dw channel_count) {
    IplaySb16RegsResult result = iplay_volume_12a66_result(channel_count, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

IplaySb16RegsResult iplay_vlm_141df_result(db *globals, dw channel_count, dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result = iplay_volume_12a66_result(channel_count, eax, ebx, ecx, edx);
    result.eax &= 0xffff0000UL;
    result.ebx = (result.ebx & 0xffff0000UL) | 0x5344u;
    result.ecx = (result.ecx & 0xffff0000UL) | 0x4d50u;
    result.edx = (result.edx & 0xffff0000UL) | 0xde01u;
    globals[0x00d1] = 1;
    return result;
}

void iplay_vlm_141df(IplayRegs *r, db *globals, dw channel_count) {
    IplaySb16RegsResult result = iplay_vlm_141df_result(globals, channel_count, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

IplaySb16RegsResult iplay_change_volume_result(db *globals, db *channels, dw channel_count, dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result;
    dw value = (dw)eax;
    dw i;
    if (value != 0xffffu) {
        globals[0x005c] = (db)value;
        globals[0x005d] = (db)(value >> 8);
        for (i = 0; i < channel_count; ++i) {
            db *channel = channels + i * 0x50u;
            eax = (eax & 0xffffff00UL) | channel[0x08];
        }
        ecx = 0;
        ebx = (ebx & 0xffff0000UL) | (0x1368u + (dw)(channel_count * 0x50u));
    }
    eax = (eax & 0xffff0000UL) | get_word(globals, 0x005c);
    result.eax = eax;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    return result;
}

void iplay_change_volume(IplayRegs *r, db *globals, db *channels, dw channel_count) {
    IplaySb16RegsResult result = iplay_change_volume_result(globals, channels, channel_count, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

dd iplay_getset_playstate_eax(dd eax, db play_state) {
    db al = (db)eax;
    if (al != 0xff) play_state = al;
    return (eax & 0xffffff00UL) | play_state;
}

db iplay_getset_playstate(IplayRegs *r, db play_state) {
    dd eax = iplay_getset_playstate_eax(abi_eax(r), play_state);
    apply_full_regs6(r, eax, abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    return (db)eax;
}

IplaySb16RegsResult iplay_get_12f7c_result(dw word_245f0, dw word_245f6, dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result;
    result.eax = (eax & 0xffff0000UL) | word_245f0;
    result.ebx = (ebx & 0xffff0000UL) | word_245f6;
    result.ecx = ecx;
    result.edx = edx;
    return result;
}

void iplay_get_12f7c(IplayRegs *r, dw word_245f0, dw word_245f6) {
    IplaySb16RegsResult result = iplay_get_12f7c_result(word_245f0, word_245f6, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

IplayRegs6Result iplay_memclean_result(db *mem, dw di, dw size, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    (void)ecx;
    (void)edi;
    memset(mem + di, 0, size);
    result.eax = eax & 0xffff0000UL;
    result.ebx = ebx;
    result.ecx = 0;
    result.edx = edx;
    result.esi = esi;
    result.edi = (dw)(di + size);
    return result;
}

void iplay_memclean(IplayRegs *r, db *mem, dw size) {
    dd old_edi = abi_edi(r);
    IplayRegs6Result result = iplay_memclean_result(mem, (dw)old_edi, size, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), old_edi);
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}


void iplay_sub_12afd(IplayRegs *r, db *channels, dw channel_count, db channel_index, db sndflags) {
    db *channel;
    if (channel_index >= channel_count) return;
    channel = channels + (dw)channel_index * 0x50u;
    iplay_eff_13a43(r, channel, sndflags);
}

void iplay_sub_12b18(db *globals, db *channels, const db *src, dw channel_count, db sndflags) {
    dw i;
    db count_zero = 0;
    db count_nonzero = 0;
    for (i = 0; i < channel_count; ++i) {
        db *channel = channels + i * 0x50u;
        db al = src[i];
        channel[0x18] = (db)i;
        if ((sndflags & 4u) == 0) {
            al = (al < 0x40) ? 0 : 0x80;
        }
        channel[0x3a] = al;
        if (channel[0x3a] != 0) ++count_nonzero;
        else ++count_zero;
    }
    globals[0x007c] = count_zero;
    globals[0x007d] = count_nonzero;
}

void iplay_someplaymode(db *globals, db *channels, dw channel_count, dw channel_stride) {
    dd numerator_lo = 1775763456UL;
    dd dword_245c0 = 0x0369d800UL;
    dw freq = get_word(globals, 0x00be);
    db shift = globals[0x007a];
    dd divisor = ((dd)freq) << shift;
    dd result = 0;
    dw i;
    if (shift == 0) {
        numerator_lo = 1643177984UL;
        dword_245c0 = 0x0361f0f0UL;
        if ((globals[0x00d2] & 8) == 0) {
            numerator_lo = 1776914432UL;
            dword_245c0 = 0x0369e990UL;
        }
    }
    if (divisor != 0) {
        uint64_t n = (((uint64_t)3) << 32) | numerator_lo;
        result = (dd)(n / divisor);
    }
    put_dword(globals, 0x001c, result);
    put_dword(globals, 0x0020, dword_245c0);
    if ((globals[0x0082] & 4) != 0) {
        dd factor = (globals[0x00d2] & 8) ? 385532977UL : 389081954UL;
        uint64_t product = (uint64_t)factor * globals[0x0089];
        put_dword(globals, 0x009c, (dd)(product >> (12 + shift)));
    }
    for (i = 0; i < channel_count; ++i) {
        db *channel = channels + i * channel_stride;
        channel[0x3e] = 0;
        channel[0x3f] = 0;
    }
}

void iplay_sub_13044(db *globals, db *vlm_table) {
    db mode = globals[0x00de];
    if (mode == 1 || mode == 2) {
        globals[0x00dd] = 0x3f;
        globals[0x008e] = (mode == 1) ? 0xf8 : 0x78;
        globals[0x008f] = (mode == 1) ? 0x01 : 0x02;
        globals[0x00b6] = (mode == 1) ? 0x81 : 0xc1;
        globals[0x00b7] = 0x0c;
    } else {
        globals[0x00de] = 0;
        globals[0x00dd] = 0x40;
        globals[0x008e] = 0x76;
        globals[0x008f] = 0x01;
        globals[0x00b6] = 0x40;
        globals[0x00b7] = 0x0c;
    }
    memset(vlm_table, 0, 32);
}

void iplay_sub_12b83_state(db *globals, db *channels, dw channel_stride, const db *types, db requested_count) {
    db al = requested_count;
    dw count;
    dw i;
    db type0 = 0, type1 = 0, type2 = 0;
    if (al >= 0x20) al = 0x20;
    if (al <= 2) al = 2;
    count = al;
    globals[0x0034] = (db)count;
    globals[0x0035] = (db)(count >> 8);
    for (i = 0; i < count; ++i) {
        db *channel = channels + i * channel_stride;
        channel[0x1d] = types[i];
        if (channel[0x1d] == 0) channel[0x18] = type0++;
        else if (channel[0x1d] == 1) channel[0x18] = type1++;
        else if (channel[0x1d] == 2) channel[0x18] = type2++;
    }
    globals[0x0036] = type0;
    globals[0x0037] = 0;
    globals[0x0038] = type1;
    globals[0x0039] = 0;
    globals[0x003a] = type2;
    globals[0x003b] = 0;
    iplay_sub_13044(globals, globals + 0x3d68);
    iplay_someplaymode(globals, channels, count, channel_stride);
}

void iplay_sub_12b83(IplayRegs *r, db *globals, db *channels, dw channel_stride, const db *types) {
    iplay_sub_12b83_state(globals, channels, channel_stride, types, (db)abi_eax(r));
}

IplayRegs6Result iplay_sub_13623_guard_result(dw channel_count, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    static const db skip_len[8] = {0, 2, 1, 3, 2, 4, 3, 5};
    IplayRegs6Result result;
    db al = (db)eax;
    db dh = al & 0xe0u;
    dw channel = al & 0x1fu;
    edx = (edx & 0xffff00ffUL) | ((dw)dh << 8);
    if (channel >= channel_count) {
        db index = dh >> 5;
        db len = skip_len[index];
        eax = (eax & 0xffff0000UL) | len;
        esi = (esi & 0xffff0000UL) | (dw)((dw)esi + len);
        edi = (edi & 0xffff0000UL) | index;
    }
    result.eax = eax;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    result.esi = esi;
    result.edi = edi;
    return result;
}

void iplay_sub_13623_guard(IplayRegs *r, dw channel_count) {
    IplayRegs6Result result = iplay_sub_13623_guard_result(channel_count, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

IplayRegs6Result iplay_sub_12cad_guard_result(db *event_store, dw channel_count, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    event_store[0] = (db)edx;
    event_store[1] = (db)(edx >> 8);
    event_store[2] = (db)ecx;
    event_store[3] = (db)ebx;
    event_store[4] = (db)(ebx >> 8);
    esi = (esi & 0xffff0000UL) | 0x0106u;
    eax = (eax & 0xffffff00UL) | (((db)(ecx >> 8)) | 0xe0u);
    return iplay_sub_13623_guard_result(channel_count, eax, ebx, ecx, edx, esi, edi);
}

void iplay_sub_12cad_guard(IplayRegs *r, db *event_store, dw channel_count) {
    IplayRegs6Result result = iplay_sub_12cad_guard_result(event_store, channel_count, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

IplayRegs6Result iplay_sub_12d05_to_buffer(db *dst, db snd_init, db sndcard_type, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    static const char default_msg[] = "Device not initialised!";
    const char *src = default_msg;
    IplayRegs6Result result;
    size_t len;
    (void)sndcard_type;
    if (snd_init == 1) {
        src = "Sound device";
    }
    len = strlen(src) + 1;
    memcpy(dst, src, len);
    result.eax = (eax & 0xffff0000UL) | 0x1500u;
    result.ebx = ebx;
    result.ecx = (ecx & 0xffff0000UL) | (dw)len;
    result.edx = edx;
    result.esi = (esi & 0xffff0000UL) | 0x1086u;
    result.edi = (edi & 0xffff0000UL) | (dw)(((dw)edi) + len);
    return result;
}

void iplay_sub_12d05(IplayRegs *r, db *dst, db snd_init, db sndcard_type) {
    IplayRegs6Result result = iplay_sub_12d05_to_buffer(dst, snd_init, sndcard_type, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

db iplay_sub_12d35_disable_code(dd eax) {
    return ((db)eax == 1u) ? 1u : 0u;
}

void iplay_sub_12d35_disable(IplayRegs *r, db *code_byte) {
    *code_byte = iplay_sub_12d35_disable_code(abi_eax(r));
}

void iplay_sub_12da8_guard_state(db *globals, dd eax, dd ebx, dd ecx, dd edx, dd esi) {
    db al = (db)eax;
    db ah = (db)(eax >> 8);
    db bl = (db)ebx;
    db bh = (db)(ebx >> 8);
    db cl = (db)ecx;
    db ch = (db)(ecx >> 8);
    dw dx = (dw)edx;
    dw si = (dw)esi;
    dw freq = (dw)ah * 1000u;
    globals[0x0132] = al;
    put_word(globals, 0x0133, dx);
    globals[0x0135] = cl;
    globals[0x0136] = ch;
    globals[0x0137] = ah;
    globals[0x0138] = bl;
    globals[0x0139] = bh;
    put_word(globals, 0x013a, si);
    globals[0x013c] = 0x4b;
    put_word(globals, 0x00be, freq);
    globals[0x00e1] = 0;
}

void iplay_sub_12da8_guard(IplayRegs *r, db *globals) {
    iplay_sub_12da8_guard_state(globals, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r));
}

IplayRegs6Result iplay_sub_1281a_small_result(db *dst, const db *samples, const db *vlm_table, const db *channel, dw word_24610, dw size, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    dw product = (dw)((word_24610 & 0xffu) * get_word(channel, 0x20));
    dw bp = product >> 8;
    db dh = (db)product;
    db dl = 0;
    dw si = (dw)eax;
    dw i;
    const db *table = vlm_table + ((dw)channel[0x23] << 9) + 1;
    for (i = 0; i < size; ++i) {
        db sample = samples[0];
        dst[i] = table[(dw)sample * 2u];
        eax = (eax & 0xffffff00UL) | dst[i];
        (void)si;
        (void)bp;
        (void)dl;
        (void)dh;
    }
    result.eax = eax;
    result.ebx = ebx;
    result.ecx = ecx & 0xffff0000UL;
    result.edx = (edx & 0xffff0000UL) | ((dw)dh << 8) | dl;
    result.esi = (esi & 0xffff0000UL) | si;
    result.edi = (edi & 0xffff0000UL) | (dw)((dw)edi + size);
    return result;
}

void iplay_sub_1281a_small(IplayRegs *r, db *dst, const db *samples, const db *vlm_table, const db *channel, dw word_24610, dw size) {
    IplayRegs6Result result = iplay_sub_1281a_small_result(dst, samples, vlm_table, channel, word_24610, size, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

void iplay_sub_13017_bounded(db *globals, db *samples, dw sample_count) {
    dw i;
    for (i = 0; i < sample_count; ++i) {
        db *sample = samples + i * 0x40u;
        if ((sample[0x3c] & 8) == 0) {
            sample[0x24] = sample[0x2c];
            sample[0x25] = sample[0x2d];
            sample[0x26] = sample[0x2e];
            sample[0x27] = sample[0x2f];
        }
    }
    globals[0x0060] = 0x01;
    globals[0x0061] = 0x08;
}

IplayRegs6Result iplay_configure_timer_bounded_result(db *globals, db *samples, dw sample_count, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    iplay_sub_13017_bounded(globals, samples, sample_count);
    result.eax = eax & 0xffff0000UL;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = (edx & 0xffff0000UL) | 0x09b0u;
    result.esi = esi;
    result.edi = edi;
    return result;
}

void iplay_configure_timer_bounded(IplayRegs *r, db *globals, db *samples, dw sample_count) {
    IplayRegs6Result result = iplay_configure_timer_bounded_result(globals, samples, sample_count, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

IplaySb16RegsResult iplay_set_playsettings_result(db *globals, db *channels, dw channel_count, dw channel_stride, dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result;
    db value = (db)eax;
    globals[0x00d2] = value;
    iplay_someplaymode(globals, channels, channel_count, channel_stride);
    globals[0x013b] &= 0xfe;
    if ((globals[0x00d2] & 0x10) != 0) globals[0x013b] |= 1;
    result.eax = eax & 0xffff0000UL;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    return result;
}

void iplay_set_playsettings(IplayRegs *r, db *globals, db *channels, dw channel_count, dw channel_stride) {
    IplaySb16RegsResult result = iplay_set_playsettings_result(globals, channels, channel_count, channel_stride, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

void iplay_sub_131da(db *channel) {
    if (channel[0x1d] == 1) return;
    if ((channel[0x17] & 1) == 0) return;
    channel[0x17] &= 0xfe;
    channel[0x35] = 0;
}

void iplay_sub_131ef(db *channel, db value, dw volume, db max_volume) {
    dw product;
    if (channel[0x1d] == 1) return;
    channel[0x3d] &= 0xbf;
    if (value > max_volume) value = max_volume;
    channel[0x22] = value;
    product = (dw)value * volume;
    channel[0x36] = channel[0x23];
    channel[0x37] = (db)(product >> 8);
    channel[0x23] = (db)(product >> 8);
}

void iplay_sub_13177(db *channel, dw period, dd dword_245bc, dd dword_245c0, db shift) {
    dd divisor;
    if (period == 0) return;
    channel[0x3d] |= 4;
    divisor = period;
    if (channel[0x1d] != 1) {
        dw old_period = get_word(channel, 0x3e);
        dw quotient;
        if (old_period == period) return;
        put_word(channel, 0x3e, period);
        quotient = (dw)(dword_245bc / divisor);
        put_word(channel, 0x20, quotient);
    }
    divisor <<= (shift & 31);
    if (divisor == 0) return;
    put_word(channel, 0x1e, (dw)(dword_245c0 / divisor));
}

void iplay_sub_13429_guard(IplayRegs *r, db *channel) {
    (void)r;
    if ((channel[0x17] & 4) == 0) return;
}

void iplay_sub_137d5_guard(IplayRegs *r, db *channel) {
    apply_edi_reg(r, (abi_edi(r) & 0xffff0000UL) | channel[0x0a]);
}

void iplay_sub_13813_guard(IplayRegs *r, db *channel) {
    apply_edi_reg(r, (abi_edi(r) & 0xffff0000UL) | channel[0x0a]);
}

void iplay_sub_140b6_guard(IplayRegs *r, db *globals) {
    (void)r;
    if (globals[0x00d1] == 1) return;
}

void iplay_eff_13bc0(db *channel, db value) {
    channel[0x09] = (db)((channel[0x09] & 0xf0) | (value & 0x0f));
}

void iplay_eff_13c34(db *channel, db value) {
    channel[0x09] = (db)(((value & 0x0f) << 4) | (channel[0x09] & 0x0f));
}

void iplay_eff_13a43_state(db *channel, db input, db sndflags) {
    db al = input;
    if (al == 0xa4) channel[0x17] |= 0x80;
    else if (al == 0xa5) channel[0x17] &= 0x7f;
    else if (al == 0xa6) channel[0x17] ^= 0x80;
    else if (al <= 0x80 && (sndflags & 4)) {
        /* Gravis-specific path is outside this non-HW slice. */
    }
}

void iplay_eff_13a43(IplayRegs *r, db *channel, db sndflags) {
    iplay_eff_13a43_state(channel, (db)abi_eax(r), sndflags);
}

void iplay_eff_13bb2_state(db *channel, db input) {
    if (input != 0) channel[0x17] |= 0x20;
    else channel[0x17] &= 0xdf;
}

void iplay_eff_13bb2(IplayRegs *r, db *channel) {
    iplay_eff_13bb2_state(channel, (db)abi_eax(r));
}

IplayRegs6Result iplay_eff_13ba3_result(db *channel, db input, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    db table_offset = (db)((input >> 3) & 0x1e);
    if (table_offset == 6) iplay_eff_13bb2_state(channel, (db)(input & 0x0f));
    result.eax = (eax & 0xffffff00UL) | (input & 0x0f);
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    result.esi = esi;
    result.edi = (edi & 0xffff0000UL) | table_offset;
    return result;
}

void iplay_eff_13ba3(IplayRegs *r, db *channel) {
    IplayRegs6Result result = iplay_eff_13ba3_result(channel, (db)abi_eax(r), abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

dd iplay_eff_13886_eax(db *channel, db input, dd eax) {
    dw step = (dw)input << 4;
    dw period = eff_13de5_def_period_core(get_word(channel, 0), step, 0);
    put_word(channel, 0, period);
    return (eax & 0xffff0000UL) | period;
}

void iplay_eff_13886(IplayRegs *r, db *channel) {
    apply_eax_reg(r, iplay_eff_13886_eax(channel, (db)abi_eax(r), abi_eax(r)));
}

dd iplay_eff_138a4_eax(db *channel, db input, dd eax) {
    dw step = (dw)input << 4;
    dw period = eff_13de5_def_period_core(get_word(channel, 0), step, 1);
    put_word(channel, 0, period);
    return (eax & 0xffff0000UL) | period;
}

void iplay_eff_138a4(IplayRegs *r, db *channel) {
    apply_eax_reg(r, iplay_eff_138a4_eax(channel, (db)abi_eax(r), abi_eax(r)));
}

dd iplay_eff_1387f_eax(db *channel, db input, db active_channel, dd eax) {
    if (active_channel != 0) return eax;
    return iplay_eff_13886_eax(channel, input, eax);
}

void iplay_eff_1387f(IplayRegs *r, db *channel, db active_channel) {
    apply_eax_reg(r, iplay_eff_1387f_eax(channel, (db)abi_eax(r), active_channel, abi_eax(r)));
}

dd iplay_eff_1389d_eax(db *channel, db input, db active_channel, dd eax) {
    if (active_channel != 0) return eax;
    return iplay_eff_138a4_eax(channel, input, eax);
}

void iplay_eff_1389d(IplayRegs *r, db *channel, db active_channel) {
    apply_eax_reg(r, iplay_eff_1389d_eax(channel, (db)abi_eax(r), active_channel, abi_eax(r)));
}

dw iplay_calc_14043_ax(db byte_2467b, db byte_2467c) {
    db al = (db)(byte_2467b + byte_2467c);
    return (dw)(((dw)al * 5u) >> 1);
}

void iplay_calc_14043(IplayRegs *r, db byte_2467b, db byte_2467c) {
    apply_eax_reg(r, (abi_eax(r) & 0xffff0000UL) | iplay_calc_14043_ax(byte_2467b, byte_2467c));
}

IplayRegs3Result iplay_sub_14087_result(db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    db al = input;
    dd out_eax = eax & 0xffff0000UL;
    dd out_edx = edx;
    if (al != 0) channel[0x34] = al;
    else al = channel[0x34];
    if (byte_24668 != 0) {
        out_eax |= (al < 0xe0) ? ((dw)al << 2) : 0;
        result.eax = out_eax;
        result.ecx = ecx;
        result.edx = out_edx;
        return result;
    }
    if (al <= 0xe0) {
        result.eax = out_eax;
        result.ecx = ecx;
        result.edx = out_edx;
        return result;
    }
    out_edx = (edx & 0xffffff00UL) | al;
    al &= 0x0f;
    out_eax |= (al > 0 && ((db)out_edx) > 0xf0) ? ((dw)al << 2) : al;
    result.eax = out_eax;
    result.ecx = ecx;
    result.edx = out_edx;
    return result;
}

void iplay_sub_14087(IplayRegs *r, db *channel, db byte_24668) {
    IplayRegs3Result result = iplay_sub_14087_result(channel, (db)abi_eax(r), byte_24668, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

static void put_word(db *mem, unsigned off, dw value) {
    mem[off] = (db)value;
    mem[off + 1] = (db)(value >> 8);
}

static dw get_word(const db *mem, unsigned off) {
    return (dw)mem[off] | ((dw)mem[off + 1] << 8);
}

static dd get_dword(const db *mem, unsigned off) {
    return (dd)get_word(mem, off) | ((dd)get_word(mem, off + 2) << 16);
}

static db iplay_sub_13e9b(db value, dw *ax_out);

IplayRegs3Result iplay_sub_13cf6_result(db *globals, db tempo, dw freq, dw buffer_size, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    dw cx = (dw)tempo;
    dd first_div;
    dw ax;
    dw dx;
    globals[0x00c6] = tempo;
    if (cx == 0) {
        ax = 0;
        dx = buffer_size;
    } else {
        cx = (dw)(cx << 1);
        first_div = (5UL * freq) / cx;
        ax = (dw)(first_div / buffer_size);
        dx = (dw)(first_div % buffer_size);
        ++ax;
        if (dx == 0) {
            --ax;
            dx = buffer_size;
        }
    }
    put_word(globals, 0x004a, dx);
    put_word(globals, 0x004c, ax);
    put_word(globals, 0x004e, ax);
    put_word(globals, 0x0044, buffer_size);
    result.eax = (eax & 0xffff0000UL) | buffer_size;
    result.ecx = (ecx & 0xffff0000UL) | cx;
    result.edx = (edx & 0xffff0000UL) | dx;
    return result;
}

void iplay_sub_13cf6(IplayRegs *r, db *globals, dw freq, dw buffer_size) {
    IplayRegs3Result result = iplay_sub_13cf6_result(globals, (db)abi_eax(r), freq, buffer_size, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_ecx_edx_regs(r, result.eax, result.ecx, result.edx);
}

IplayRegs3Result iplay_eff_14030_result(db *globals, db input, db byte_2467c, dw freq, dw buffer_size, dd eax, dd ecx, dd edx) {
    static const db table_14057[16] = {0xff, 0x80, 0x40, 0x2a, 0x20, 0x19, 0x15, 0x12, 0x10, 0x0e, 0x0c, 0x0b, 0x0a, 0x09, 0x09, 0x08};
    db index = (db)(input & 0x0f);
    dw ax;
    globals[0x00dc] = byte_2467c;
    globals[0x00db] = table_14057[index];
    ax = iplay_calc_14043_ax(globals[0x00db], globals[0x00dc]);
    return iplay_sub_13cf6_result(globals, (db)ax, freq, buffer_size, (eax & 0xffff0000UL) | ax, ecx, edx);
}

void iplay_eff_14030(IplayRegs *r, db *globals, db byte_2467c, dw freq, dw buffer_size) {
    IplayRegs3Result result = iplay_eff_14030_result(globals, (db)abi_eax(r), byte_2467c, freq, buffer_size, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_ecx_edx_regs(r, result.eax, result.ecx, result.edx);
}

IplayRegs3Result iplay_eff_14067_result(db *globals, db input, db byte_2467b, db byte_2467c, dw freq, dw buffer_size, dd eax, dd ecx, dd edx) {
    db al = input;
    dw ax;
    globals[0x00db] = byte_2467b;
    globals[0x00dc] = byte_2467c;
    if (al == 0) globals[0x00dc] = 0;
    else if ((al & 0x0f) != 0) globals[0x00dc] = (db)(globals[0x00dc] - (al & 0x0f));
    else globals[0x00dc] = (db)(globals[0x00dc] + (al >> 4));
    ax = iplay_calc_14043_ax(globals[0x00db], globals[0x00dc]);
    return iplay_sub_13cf6_result(globals, (db)ax, freq, buffer_size, (eax & 0xffff0000UL) | ax, ecx, edx);
}

void iplay_eff_14067(IplayRegs *r, db *globals, db byte_2467b, db byte_2467c, dw freq, dw buffer_size) {
    IplayRegs3Result result = iplay_eff_14067_result(globals, (db)abi_eax(r), byte_2467b, byte_2467c, freq, buffer_size, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_ecx_edx_regs(r, result.eax, result.ecx, result.edx);
}

void iplay_snd_guard_state(db *globals, unsigned op) {
    switch (op) {
    case 0: /* snd_initialze */
        if (globals[0x00e0] == 1) return;
        return;
    case 1: /* snd_on */
        if (globals[0x00e0] != 1) return;
        if (globals[0x00e1] == 1) return;
        globals[0x00e1] = 1;
        return;
    case 2: /* snd_off */
        if (globals[0x00e0] != 1) return;
        if (globals[0x00e1] == 0) return;
        globals[0x00e1] = 0;
        return;
    case 3: /* snd_deinit */
        if (globals[0x00e0] != 1) return;
        globals[0x00e0] = 0;
        return;
    case 4: /* snd_offx */
        if (globals[0x00e0] != 1) return;
        if (globals[0x00e1] == 0) return;
        globals[0x00e1] = 0;
        return;
    default:
        return;
    }
}

void iplay_snd_guard(IplayRegs *r, db *globals, unsigned op) {
    (void)r;
    iplay_snd_guard_state(globals, op);
}

void iplay_eff_13a94(IplayRegs *r, db *channel, db byte_2461a) {
    db al = (db)abi_eax(r);
    dd scaled;
    dd sample_end;
    dd eax;
    if (al != 0) channel[0x16] = al;
    scaled = (dd)channel[0x16] << 8;
    sample_end = get_dword(channel, 0x30);
    eax = scaled;
    if (scaled <= sample_end) {
        put_word(channel, 0x4c, (dw)scaled);
    } else if (byte_2461a == 0) {
        channel[0x17] = (db)((channel[0x17] & 0xfb) | 0x40);
        channel[0x03] = 0;
    } else {
        eax = sample_end;
        put_word(channel, 0x4c, (dw)sample_end);
    }
    apply_full_regs6(r, eax, abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
}

static void eff_13ad7_core(db *channel, db input, db max_volume, db *al_out, db *dl_out) {
    db al = input;
    db dl = channel[0x08];
    if ((al & 0xf0) != 0) {
        al >>= 4;
        al = (db)(dl + al);
        if (al > max_volume) al = max_volume;
    } else {
        al &= 0x0f;
        al = (dl >= al) ? (db)(dl - al) : 0;
    }
    channel[0x08] = al;
    *al_out = al;
    *dl_out = dl;
}

void iplay_eff_13ad7(IplayRegs *r, db *channel, db max_volume) {
    IplayRegs3Result result = iplay_eff_13ad7_result(channel, (db)abi_eax(r), max_volume, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

IplayRegs3Result iplay_eff_13ad7_result(db *channel, db input, db max_volume, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    db al;
    db dl;
    eff_13ad7_core(channel, input, max_volume, &al, &dl);
    result.eax = (eax & 0xffffff00UL) | al;
    result.ecx = ecx;
    result.edx = (edx & 0xffffff00UL) | dl;
    return result;
}

dw iplay_eff_13b06_ax(db *globals, db input) {
    dw ax = input;
    ax = (dw)(ax - 1u);
    put_word(globals, 0x0050, ax);
    ax = (dw)(ax + 1u);
    globals[0x00c9] = 0;
    globals[0x00ca] = 1;
    return ax;
}

void iplay_eff_13b06(IplayRegs *r, db *globals, db flag_playsettings) {
    dw ax;
    (void)flag_playsettings;
    ax = iplay_eff_13b06_ax(globals, (db)abi_eax(r));
    apply_eax_reg(r, (abi_eax(r) & 0xffff0000UL) | ax);
}

db iplay_eff_13b78_al(db *channel, db input, db max_volume) {
    db al = input;
    if (al > max_volume) al = max_volume;
    channel[0x08] = al;
    return al;
}

void iplay_eff_13b78(IplayRegs *r, db *channel, db max_volume) {
    db al = iplay_eff_13b78_al(channel, (db)abi_eax(r), max_volume);
    apply_eax_reg(r, (abi_eax(r) & 0xffffff00UL) | al);
}

static void eff_13b88_core(db *globals, db input, db *al_out, db *dl_out) {
    db al = input;
    db dl = (db)(al & 0x0f);
    al >>= 4;
    al = (db)(al * 10u + dl);
    if (al <= 0x3f) globals[0x00c9] = al;
    else globals[0x00c9] = 0;
    globals[0x00ca] = 1;
    *al_out = al;
    *dl_out = dl;
}

void iplay_eff_13b88(IplayRegs *r, db *globals) {
    IplayRegs3Result result = iplay_eff_13b88_result(globals, (db)abi_eax(r), abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

IplayRegs3Result iplay_eff_13b88_result(db *globals, db input, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    db al;
    db dl;
    eff_13b88_core(globals, input, &al, &dl);
    result.eax = (eax & 0xffffff00UL) | al;
    result.ecx = ecx;
    result.edx = (edx & 0xffffff00UL) | dl;
    return result;
}

static void eff_13bc8_core(db *channel, db input, dw dx, db byte_2461a, dw *ax_out, dw *di_out) {
    static const dw table_246f6[16] = {
        8363, 8422, 8482, 8543, 8604, 8667, 8730, 8794,
        7901, 7954, 8007, 8062, 8116, 8191, 8231, 8305
    };
    dw index = (dw)(input & 0x0f);
    if (byte_2461a == 0) {
        dw pointer = (dw)(0x03f8u + index * 120u);
        put_word(channel, 0x38, pointer);
        *ax_out = pointer;
        *di_out = (dw)(index << 7);
    } else {
        put_word(channel, 0x14, dx);
        *ax_out = table_246f6[index];
        *di_out = (dw)(index << 1);
    }
}

void iplay_eff_13bc8(IplayRegs *r, db *channel, db byte_2461a) {
    IplayRegs6Result result = iplay_eff_13bc8_result(channel, (db)abi_eax(r), (dw)abi_edx(r), byte_2461a, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

IplayRegs6Result iplay_eff_13bc8_result(db *channel, db input, dw dx, db byte_2461a, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    dw ax;
    dw di;
    eff_13bc8_core(channel, input, dx, byte_2461a, &ax, &di);
    result.eax = (eax & 0xffff0000UL) | ax;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    result.esi = esi;
    result.edi = (edi & 0xffff0000UL) | di;
    return result;
}

static db eff_13c02_core(db *channel, db *globals, db input, dw word_245f6, dw *ax_out) {
    db al = input;
    if (globals[0x00c8] != 0) return 0;
    if (al != 0) {
        if (channel[0x3c] == 0) channel[0x3c] = (db)(al + 1u);
        channel[0x3c] = (db)(channel[0x3c] - 1u);
        if (channel[0x3c] == 0) return 0;
        globals[0x00c9] = channel[0x3b];
        globals[0x00cb] = 1;
        *ax_out = channel[0x3b];
    } else {
        channel[0x3b] = (db)word_245f6;
        *ax_out = word_245f6;
    }
    return 1;
}

void iplay_eff_13c02(IplayRegs *r, db *channel, db *globals, dw word_245f6) {
    dd old_eax = abi_eax(r);
    dd eax = iplay_eff_13c02_eax(channel, globals, (db)old_eax, word_245f6, old_eax);
    if (eax != old_eax) apply_eax_reg(r, eax);
}

dd iplay_eff_13c02_eax(db *channel, db *globals, db input, dw word_245f6, dd eax) {
    dw ax;
    if (!eff_13c02_core(channel, globals, input, word_245f6, &ax)) return eax;
    if (input != 0) return (eax & 0xffffff00UL) | (db)ax;
    return (eax & 0xffff0000UL) | ax;
}

static db eff_13c3f_core(db input, db byte_24668, dw *ax_out, dw *di_out) {
    static const db table_13c54[16] = {0, 9, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x40, 0x40, 0x4a, 0x53, 0x5c, 0x65, 0x6e, 0x77, 0x80};
    db index = input & 0x0f;
    if (byte_24668 != 0) return 0;
    *ax_out = table_13c54[index];
    *di_out = index;
    return 1;
}

IplayRegs6Result iplay_eff_13c3f_result(db *channel, db input, db byte_24668, db sndflags, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    dw ax;
    dw di;
    result.eax = eax;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    result.esi = esi;
    result.edi = edi;
    if (!eff_13c3f_core(input, byte_24668, &ax, &di)) return result;
    result.eax = (eax & 0xffffff00UL) | (db)ax;
    result.edi = (edi & 0xffff0000UL) | di;
    iplay_eff_13a43_state(channel, (db)result.eax, sndflags);
    return result;
}

void iplay_eff_13c3f(IplayRegs *r, db *channel, db byte_24668, db sndflags) {
    IplayRegs6Result result = iplay_eff_13c3f_result(channel, (db)abi_eax(r), byte_24668, sndflags, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

static db eff_13c64_core(const db *channel, db input, db byte_24668, dw *ax_out) {
    db dl = input;
    if (dl == 0) return 0;
    if (byte_24668 == 0 && (channel[0x3d] & 8) != 0) return 0;
    *ax_out = (dw)(((byte_24668 % dl) << 8) | (byte_24668 / dl));
    return 1;
}

IplayRegs3Result iplay_eff_13c64_result(const db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    dw ax;
    result.eax = eax;
    result.ecx = ecx;
    result.edx = edx;
    if (!eff_13c64_core(channel, input, byte_24668, &ax)) return result;
    result.eax = (eax & 0xffff0000UL) | ax;
    result.edx = (edx & 0xffffff00UL) | input;
    return result;
}

void iplay_eff_13c64(IplayRegs *r, db *channel, db byte_24668) {
    IplayRegs3Result result = iplay_eff_13c64_result(channel, (db)abi_eax(r), byte_24668, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

static db eff_13c88_95_core(db *channel, db input, db byte_24668, db max_volume, db subtract, db *al_out, db *dl_out) {
    db al = input;
    db dl;
    if (byte_24668 != 0) return 0;
    dl = channel[0x08];
    if (subtract) {
        al = (dl >= al) ? (db)(dl - al) : 0;
    } else {
        al = (db)(dl + al);
        if (al > max_volume) al = max_volume;
    }
    channel[0x08] = al;
    *al_out = al;
    *dl_out = dl;
    return 1;
}

IplayRegs3Result iplay_eff_13c88_result(db *channel, db input, db byte_24668, db max_volume, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    db al;
    db dl;
    result.eax = eax;
    result.ecx = ecx;
    result.edx = edx;
    if (!eff_13c88_95_core(channel, input, byte_24668, max_volume, 0, &al, &dl)) return result;
    result.eax = (eax & 0xffffff00UL) | al;
    result.edx = (edx & 0xffffff00UL) | dl;
    return result;
}

void iplay_eff_13c88(IplayRegs *r, db *channel, db byte_24668, db max_volume) {
    IplayRegs3Result result = iplay_eff_13c88_result(channel, (db)abi_eax(r), byte_24668, max_volume, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

IplayRegs3Result iplay_eff_13c95_result(db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    db al;
    db dl;
    result.eax = eax;
    result.ecx = ecx;
    result.edx = edx;
    if (!eff_13c88_95_core(channel, input, byte_24668, 0, 1, &al, &dl)) return result;
    result.eax = (eax & 0xffffff00UL) | al;
    result.edx = (edx & 0xffffff00UL) | dl;
    return result;
}

void iplay_eff_13c95(IplayRegs *r, db *channel, db byte_24668) {
    IplayRegs3Result result = iplay_eff_13c95_result(channel, (db)abi_eax(r), byte_24668, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

static db eff_13ca2_core(db input, db byte_24668) {
    return input == byte_24668;
}

dd iplay_eff_13ca2_eax(db input, db byte_24668, dd eax) {
    if (eff_13ca2_core(input, byte_24668)) return eax & 0xffffff00UL;
    return eax;
}

void iplay_eff_13ca2(IplayRegs *r, db *globals, db byte_24668) {
    (void)globals;
    apply_eax_reg(r, iplay_eff_13ca2_eax((db)abi_eax(r), byte_24668, abi_eax(r)));
}

static void eff_13cb3_core(db *channel, db input, db byte_24668) {
    if (input == byte_24668 && get_word(channel, 0) != 0) {
        channel[0x0a] = 0;
        channel[0x0b] = 0;
    }
}

void iplay_eff_13cb3_state(db *channel, db input, db byte_24668) {
    eff_13cb3_core(channel, input, byte_24668);
}

void iplay_eff_13cb3(IplayRegs *r, db *channel, db byte_24668) {
    iplay_eff_13cb3_state(channel, (db)abi_eax(r), byte_24668);
}

static db eff_13cc9_core(db *globals, db input, db byte_24668, db byte_2466d, db *al_out) {
    db al;
    if (byte_24668 != 0 || byte_2466d != 0) return 0;
    al = (db)(input + 1u);
    globals[0x00cc] = al;
    *al_out = al;
    return 1;
}

void iplay_eff_13cc9(IplayRegs *r, db *globals, db byte_24668, db byte_2466d) {
    dd eax = iplay_eff_13cc9_eax(globals, (db)abi_eax(r), byte_24668, byte_2466d, abi_eax(r));
    apply_eax_reg(r, eax);
}

dd iplay_eff_13cc9_eax(db *globals, db input, db byte_24668, db byte_2466d, dd eax) {
    db al;
    if (!eff_13cc9_core(globals, input, byte_24668, byte_2466d, &al)) return eax;
    return (eax & 0xffffff00UL) | al;
}

static void eff_13cdd_ce8_core(db *globals, db input, db flag_playsettings, db check_playsettings) {
    db al = input;
    if (check_playsettings && (flag_playsettings & 2) == 0 && al > 0x20) {
        return;
    }
    if (al != 0) {
        globals[0x00c7] = al;
        globals[0x00c8] = 0;
    }
}

void iplay_eff_13cdd(IplayRegs *r, db *globals, db flag_playsettings) {
    iplay_eff_13cdd_state(globals, (db)abi_eax(r), flag_playsettings);
}

void iplay_eff_13cdd_state(db *globals, db input, db flag_playsettings) {
    eff_13cdd_ce8_core(globals, input, flag_playsettings, 1);
}

void iplay_eff_13ce8_state(db *globals, db input) {
    eff_13cdd_ce8_core(globals, input, 0, 0);
}

void iplay_eff_13ce8(IplayRegs *r, db *globals) {
    iplay_eff_13ce8_state(globals, (db)abi_eax(r));
}

static void sub_13d95_core(db *globals, dw input_cx, dw *ax_out, dw *cx_out) {
    dw cx = input_cx;
    db level = 1;
    dw quotient;
    for (;;) {
        quotient = (cx == 0) ? 0xffff : (dw)(31250u / cx);
        if (quotient <= 0xffu) break;
        cx = (dw)(cx << 1);
        ++level;
    }
    globals[0x0078] = level;
    globals[0x0079] = level;
    *ax_out = (dw)(((dw)level << 8) | ((db)(-(int)quotient)));
    *cx_out = cx;
}

IplayRegs3Result iplay_sub_13d95_result(db *globals, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    dw ax;
    dw cx;
    sub_13d95_core(globals, (dw)ecx, &ax, &cx);
    result.eax = (eax & 0xffff0000UL) | ax;
    result.ecx = (ecx & 0xffff0000UL) | cx;
    result.edx = edx & 0xffff0000UL;
    return result;
}

void iplay_sub_13d95(IplayRegs *r, db *globals) {
    IplayRegs3Result result = iplay_sub_13d95_result(globals, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_ecx_edx_regs(r, result.eax, result.ecx, result.edx);
}

void iplay_sub_13e9b_public(IplayRegs *r) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    dw ax;
    db high = iplay_sub_13e9b((db)old_eax, &ax);
    apply_full_regs6(r, (old_eax & 0xffff0000UL) | ax, old_ebx, old_ecx,
                     (old_edx & 0xffffff00UL) | high, old_esi,
                     (old_edi & 0xffff0000UL) | high);
}

IplayRegs6Result iplay_sub_13826_result(db *channel, db input, db byte_2461a, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    static const dw base_periods[12] = {
        0x6b00, 0x6500, 0x5f40, 0x5a00, 0x54c0, 0x5000,
        0x4b80, 0x4740, 0x4340, 0x3f80, 0x3c00, 0x38a0
    };
    db value = input;
    db cl = value;
    dw di = (dw)(((value & 0x0f) - 1u) & 0x0f);
    dw ax = 0;
    di <<= 1;
    cl >>= 4;
    if (byte_2461a == 0) {
        db ch = cl;
        cl = 0;
        if (ch != 0) {
            ax = 24;
            --ch;
            if (ch != 0) {
                ax = 48;
                --ch;
                if (ch != 0) {
                    ax = 72;
                    --ch;
                    if (ch != 0) {
                        ax = 96;
                        --ch;
                        if (ch != 0) cl = ch;
                    }
                }
            }
        }
        di = (dw)(di + ax + get_word(channel, 0x38) - 0x013e);
    }
    ax = base_periods[(di >> 1) % 12];
    ax >>= cl;
    result.eax = (eax & 0xffff0000UL) | ax;
    result.ebx = ebx;
    result.ecx = (ecx & 0xffff0000UL) | ((dw)get_word(channel, 0x14));
    result.edx = edx;
    result.esi = esi;
    result.edi = (edi & 0xffff0000UL) | di;
    return result;
}

void iplay_sub_13826(IplayRegs *r, db *channel, db byte_2461a) {
    IplayRegs6Result result = iplay_sub_13826_result(channel, (db)abi_eax(r), byte_2461a, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

void iplay_sub_13826_full(IplayRegs *r, db *globals, db *channel) {
    dd old_eax = abi_eax(r);
    dd old_ebx = abi_ebx(r);
    dd old_ecx = abi_ecx(r);
    dd old_edx = abi_edx(r);
    dd old_esi = abi_esi(r);
    dd old_edi = abi_edi(r);
    db value = (db)old_eax;
    db cl = value;
    dw di = (dw)(((value & 0x0f) - 1u) & 0x0f);
    dw ax = 0;
    dw cx;
    di <<= 1;
    cl >>= 4;
    if (globals[0x007a] == 0) {
        db ch = cl;
        cl = 0;
        if (ch != 0) {
            ax = 24;
            --ch;
            if (ch != 0) {
                ax = 48;
                --ch;
                if (ch != 0) {
                    ax = 72;
                    --ch;
                    if (ch != 0) {
                        ax = 96;
                        --ch;
                        if (ch != 0) cl = ch;
                    }
                }
            }
        }
        di = (dw)(di + ax + get_word(channel, 0x38) - 0x013e);
    }
    ax = get_word(globals, (dw)(0x013e + di));
    ax = (cl >= 16u) ? 0 : (dw)(ax >> cl);
    cx = get_word(channel, 0x14);
    if (cx != 0) {
        dd scaled = (dd)ax * get_word(globals, 0x003e);
        ax = (dw)(scaled / cx);
    }
    apply_full_regs6(r, (old_eax & 0xffff0000UL) | ax, old_ebx,
                     (old_ecx & 0xffff0000UL) | cx,
                     old_edx, old_esi, (old_edi & 0xffff0000UL) | di);
}

static db target_slide_core(db *channel, dw *ax_out) {
    dw target = get_word(channel, 0x10);
    dw current = get_word(channel, 0);
    dw step;
    if (target == 0) return 0;
    step = get_word(channel, 0x12);
    if (target >= current) {
        dd next = (dd)current + step;
        if (next >= target) {
            current = target;
            put_word(channel, 0x10, 0);
            channel[0x17] &= 0xef;
        } else {
            current = (dw)next;
        }
    } else {
        current = (current > step) ? (dw)(current - step) : 0;
        if ((int16_t)target >= (int16_t)current) {
            current = target;
            put_word(channel, 0x10, 0);
            channel[0x17] &= 0xef;
        }
    }
    put_word(channel, 0, current);
    if ((channel[0x17] & 0x20) != 0 && get_word(channel, 0x10) != 0) {
        *ax_out = 0x0032;
    } else {
        *ax_out = current;
    }
    return 1;
}

static db eff_slide_step_core(db *channel, db input, db shift, dw *ax_out) {
    if (input != 0) put_word(channel, 0x12, (dw)input << shift);
    return target_slide_core(channel, ax_out);
}

dd iplay_eff_slide_step_eax(db *channel, db input, db shift, dd eax) {
    dw ax;
    if (!eff_slide_step_core(channel, input, shift, &ax)) return eax;
    return (eax & 0xffff0000UL) | ax;
}

dd iplay_eff_13e1e_eax(db *channel, db input, dd eax) {
    return iplay_eff_slide_step_eax(channel, input, 2, eax);
}

void iplay_eff_13e1e(IplayRegs *r, db *channel) {
    apply_eax_reg(r, iplay_eff_13e1e_eax(channel, (db)abi_eax(r), abi_eax(r)));
}

dd iplay_eff_138d2_eax(db *channel, db input, dd eax) {
    return iplay_eff_slide_step_eax(channel, input, 4, eax);
}

void iplay_eff_138d2(IplayRegs *r, db *channel) {
    apply_eax_reg(r, iplay_eff_138d2_eax(channel, (db)abi_eax(r), abi_eax(r)));
}

static const db effect_wave[32] = {
    0x00, 0x18, 0x31, 0x4a, 0x61, 0x78, 0x8d, 0xa1,
    0xb4, 0xc5, 0xd4, 0xe0, 0xeb, 0xf4, 0xfa, 0xfd,
    0xff, 0xfd, 0xfa, 0xf4, 0xeb, 0xe0, 0xd4, 0xc5,
    0xb4, 0xa1, 0x8d, 0x78, 0x61, 0x4a, 0x31, 0x18
};

dd iplay_vibrato_eax(db *channel, db input, db flag_playsettings, db base_shift, db update_memory, dd eax_in) {
    db al = input;
    db dl;
    dw ax;
    db dh;
    if (update_memory && al != 0) {
        db ch = al;
        dl = channel[0x0c];
        if ((al & 0x0f) != 0) dl = (db)((dl & 0xf0) | (al & 0x0f));
        if ((ch & 0xf0) != 0) dl = (db)((dl & 0x0f) | (ch & 0xf0));
        channel[0x0c] = dl;
    }
    al = (db)((channel[0x0d] >> 2) & 0x1f);
    dl = (db)(channel[0x09] & 3);
    if (dl != 0) {
        al <<= 3;
        if (dl == 1) {
            dl = al;
            if (channel[0x0d] & 0x80) dl = (db)(0xffu - al);
        } else {
            dl = 0xff;
        }
    } else {
        dl = effect_wave[al];
    }
    ax = (dw)(channel[0x0c] & 0x0f) * dl;
    ax >>= (db)(base_shift + (flag_playsettings & 1));
    if (channel[0x0d] & 0x80) ax = (dw)(-((int16_t)ax));
    ax = (dw)(ax + get_word(channel, 0));
    dh = (db)((channel[0x0c] >> 2) & 0x3c);
    channel[0x0d] = (db)(channel[0x0d] + dh);
    return (eax_in & 0xffff0000UL) | ax;
}

dd iplay_eff_13e2d_eax(db *channel, db input, db flag_playsettings, dd eax) {
    return iplay_vibrato_eax(channel, input, flag_playsettings, 5, 1, eax);
}

void iplay_eff_13e2d(IplayRegs *r, db *channel, db flag_playsettings) {
    apply_eax_reg(r, iplay_eff_13e2d_eax(channel, (db)abi_eax(r), flag_playsettings, abi_eax(r)));
}

void iplay_eff_1392f(IplayRegs *r, db *channel, db flag_playsettings) {
    apply_eax_reg(r, iplay_eff_1392f_eax(channel, (db)abi_eax(r), flag_playsettings, abi_eax(r)));
}

dd iplay_eff_1392f_eax(db *channel, db input, db flag_playsettings, dd eax) {
    return iplay_vibrato_eax(channel, input, flag_playsettings, 3, 1, eax);
}

IplayRegs3Result iplay_eff_139ac_result(db *channel, db input, db max_volume, dd eax_in, dd ecx, dd edx) {
    IplayRegs3Result result;
    db al;
    db dl;
    dw ax;
    result.eax = eax_in;
    result.ecx = ecx;
    result.edx = edx;
    eff_13ad7_core(channel, input, max_volume, &al, &dl);
    if (target_slide_core(channel, &ax)) {
        result.eax = (eax_in & 0xffff0000UL) | ax;
    } else {
        result.eax = (eax_in & 0xffffff00UL) | al;
    }
    result.edx = (edx & 0xffffff00UL) | dl;
    return result;
}

void iplay_eff_139ac(IplayRegs *r, db *channel, db max_volume) {
    IplayRegs3Result result = iplay_eff_139ac_result(channel, (db)abi_eax(r), max_volume, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

IplayRegs3Result iplay_eff_139b2_result(db *channel, db input, db max_volume, db flag_playsettings, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    db al;
    db dl;
    eff_13ad7_core(channel, input, max_volume, &al, &dl);
    result.eax = iplay_vibrato_eax(channel, al, flag_playsettings, 3, 0, (eax & 0xffffff00UL) | al);
    result.ecx = ecx;
    result.edx = (edx & 0xffffff00UL) | dl;
    return result;
}

void iplay_eff_139b2(IplayRegs *r, db *channel, db max_volume, db flag_playsettings) {
    IplayRegs3Result result = iplay_eff_139b2_result(channel, (db)abi_eax(r), max_volume, flag_playsettings, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

dd iplay_eff_139b9_eax(db *channel, db input, db max_volume, dd eax_in) {
    db al = input;
    db dl;
    db dh;
    dw ax;
    if (al != 0) {
        db cl = al;
        dl = channel[0x0e];
        if ((al & 0x0f) != 0) dl = (db)((dl & 0xf0) | (al & 0x0f));
        if ((cl & 0xf0) != 0) dl = (db)((dl & 0x0f) | (cl & 0xf0));
        channel[0x0e] = dl;
    }
    al = (db)((channel[0x0f] >> 2) & 0x1f);
    dl = (db)((channel[0x09] >> 4) & 3);
    if (dl != 0) {
        al <<= 3;
        if (dl == 1) {
            dl = al;
            if (channel[0x0f] & 0x80) dl = (db)(0xffu - al);
        } else {
            dl = 0xff;
        }
    } else {
        dl = effect_wave[al];
    }
    ax = (dw)(channel[0x0e] & 0x0f) * dl;
    ax >>= 6;
    al = channel[0x08];
    if ((channel[0x0f] & 0x80) == 0) {
        al = (db)(al + (db)ax);
        if (al > max_volume) al = max_volume;
    } else {
        db delta = (db)ax;
        al = (al >= delta) ? (db)(al - delta) : 0;
    }
    dh = (db)((channel[0x0e] >> 2) & 0x3c);
    channel[0x0f] = (db)(channel[0x0f] + dh);
    return (eax_in & 0xffff0000UL) | ((dw)(db)ax << 8) | al;
}

void iplay_eff_139b9(IplayRegs *r, db *channel, db max_volume) {
    apply_eax_reg(r, iplay_eff_139b9_eax(channel, (db)abi_eax(r), max_volume, abi_eax(r)));
}

IplayRegs3Result iplay_eff_13e7f_result(db *channel, db input, db byte_24668, db max_volume, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result = iplay_eff_13e32_result(channel, input, byte_24668, max_volume, eax, ecx, edx);
    dw ax;
    if (!target_slide_core(channel, &ax)) return result;
    result.eax = (result.eax & 0xffff0000UL) | ax;
    return result;
}

void iplay_eff_13e7f(IplayRegs *r, db *channel, db byte_24668, db max_volume) {
    IplayRegs3Result result = iplay_eff_13e7f_result(channel, (db)abi_eax(r), byte_24668, max_volume, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

IplayRegs3Result iplay_eff_13e84_result(db *channel, db input, db byte_24668, db max_volume, db flag_playsettings, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result = iplay_eff_13e32_result(channel, input, byte_24668, max_volume, eax, ecx, edx);
    result.eax = iplay_vibrato_eax(channel, (db)result.eax, flag_playsettings, 5, 0, result.eax);
    return result;
}

void iplay_eff_13e84(IplayRegs *r, db *channel, db byte_24668, db max_volume, db flag_playsettings) {
    IplayRegs3Result result = iplay_eff_13e84_result(channel, (db)abi_eax(r), byte_24668, max_volume, flag_playsettings, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

IplayRegs6Result iplay_eff_13fbe_result(db *channel, db input, db byte_24668, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    db al = input;
    db dl;
    db ah;
    result.eax = eax;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    result.esi = esi;
    result.edi = edi;
    if (al == 0) {
        al = channel[0x34];
        if (al == 0) return result;
        channel[0x0b] = al;
    }
    channel[0x34] = al;
    al = channel[0x35];
    dl = (db)(al & 0x0f);
    if (dl == 0) return result;
    --dl;
    al >>= 4;
    dl = (db)(dl + al * 12u);
    ah = (db)(byte_24668 % 3u);
    if (ah == 0) {
        result.eax = (eax & 0xffff0000UL) | get_word(channel, 0);
        return result;
    }
    {
        db dh = channel[0x0b];
        if (ah != 2) dh >>= 4;
        dh &= 0x0f;
        dl = (db)(dl + dh);
    }
    {
        db quotient = (db)(dl / 12u);
        db rem = (db)(dl % 12u);
        db packed = (db)((quotient << 4) | (rem + 1u));
        result.eax = (eax & 0xffff0000UL) | ((dw)ah << 8) | packed;
        result = iplay_sub_13826_result(channel, (db)result.eax, 0, result.eax, ebx, ecx, edx, esi, edi);
        result.eax &= 0xffff0000UL;
        return result;
    }
}

void iplay_eff_13fbe(IplayRegs *r, db *channel, db byte_24668) {
    IplayRegs6Result result = iplay_eff_13fbe_result(channel, (db)abi_eax(r), byte_24668, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

static dw eff_13de5_def_period_core(dw current, dw step, db up) {
    if (up) {
        dd period = (dd)current + step;
        if (period > 0x3580UL) period = 0x3580UL;
        return (dw)period;
    } else {
        dw period = (current > step) ? (dw)(current - step) : 0;
        if (period < 0x00a0) period = 0x00a0;
        return period;
    }
}

static IplayRegs3Result eff_13de5_def_result(db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx, db up) {
    IplayRegs3Result result = iplay_sub_14087_result(channel, input, byte_24668, eax, ecx, edx);
    dw step = (dw)result.eax;
    if (step != 0) {
        dw period = eff_13de5_def_period_core(get_word(channel, 0), step, up);
        put_word(channel, 0, period);
        result.eax = (result.eax & 0xffff0000UL) | period;
    }
    return result;
}

IplayRegs3Result iplay_eff_13de5_result(db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx) {
    return eff_13de5_def_result(channel, input, byte_24668, eax, ecx, edx, 0);
}

void iplay_eff_13de5(IplayRegs *r, db *channel, db byte_24668) {
    IplayRegs3Result result = iplay_eff_13de5_result(channel, (db)abi_eax(r), byte_24668, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

IplayRegs3Result iplay_eff_13def_result(db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx) {
    return eff_13de5_def_result(channel, input, byte_24668, eax, ecx, edx, 1);
}

void iplay_eff_13def(IplayRegs *r, db *channel, db byte_24668) {
    IplayRegs3Result result = iplay_eff_13def_result(channel, (db)abi_eax(r), byte_24668, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

IplayRegs3Result iplay_eff_13e32_result(db *channel, db input, db byte_24668, db max_volume, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    db al = input;
    db lo;
    db hi;
    result.eax = eax;
    result.ecx = ecx;
    result.edx = edx;
    if (al != 0) channel[0x34] = al;
    al = channel[0x34];
    lo = (db)(al & 0x0f);
    hi = (db)(al >> 4);
    if (lo == 0x0f) {
        if (hi == 0) {
            al = lo;
            goto do_ad7;
        } else if (byte_24668 == 0) {
            al = hi;
            goto do_b78;
        }
    } else if (hi == 0x0f) {
        if (lo != 0 && byte_24668 == 0) {
            al = lo;
            goto do_ad7;
        }
    } else if (lo != 0) {
        al = lo;
        goto do_ad7;
    } else {
        db dl = channel[0x08];
        al = (db)(dl + hi);
        if (al > max_volume) al = max_volume;
        channel[0x08] = al;
        result.eax = (eax & 0xffffff00UL) | al;
        result.edx = (edx & 0xffffff00UL) | dl;
    }
    return result;
do_ad7:
    return iplay_eff_13ad7_result(channel, al, max_volume, eax, ecx, edx);
do_b78:
    al = iplay_eff_13b78_al(channel, al, max_volume);
    result.eax = (eax & 0xffffff00UL) | al;
    return result;
}

void iplay_eff_13e32(IplayRegs *r, db *channel, db byte_24668, db max_volume) {
    IplayRegs3Result result = iplay_eff_13e32_result(channel, (db)abi_eax(r), byte_24668, max_volume, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

static db iplay_sub_13e9b(db value, dw *ax_out) {
    static const db table_13ec3[16] = {140, 50, 25, 15, 10, 7, 6, 4, 3, 3, 2, 2, 2, 2, 1, 1};
    db di = (db)(value >> 4);
    db dl = (db)(value & 0x0f);
    dw ax = (dw)((dw)dl * table_13ec3[di]);
    ax >>= 4;
    ax = (dw)(0x31u - ax);
    ax = (dw)((ax * 5u) >> 1);
    *ax_out = (dw)((di << 8) | (ax & 0xff));
    return di;
}

IplayRegs3Result iplay_eff_13e8c_result(db *globals, db input, dw freq, dw buffer_size, dd eax, dd ecx, dd edx) {
    dw ax;
    db high = iplay_sub_13e9b(input, &ax);
    globals[0x00c7] = high;
    globals[0x00c8] = 0;
    return iplay_sub_13cf6_result(globals, (db)ax, freq, buffer_size, (eax & 0xffff0000UL) | ax, ecx, edx);
}

void iplay_eff_13e8c(IplayRegs *r, db *globals, dw freq, dw buffer_size) {
    IplayRegs3Result result = iplay_eff_13e8c_result(globals, (db)abi_eax(r), freq, buffer_size, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_ecx_edx_regs(r, result.eax, result.ecx, result.edx);
}

dd iplay_eff_13f05_eax(db *channel, db input, db byte_24668, dd eax_in) {
    db al = input;
    db stored;
    db hi;
    db lo;
    db total;
    if (al != 0) channel[0x34] = al;
    stored = channel[0x34];
    hi = (db)(stored >> 4);
    lo = (db)(stored & 0x0f);
    total = (db)(hi + lo);
    if (total == 0) return eax_in;
    {
        dd eax = (eax_in & 0xffff0000UL) | (dw)(((byte_24668 % total) << 8) | (byte_24668 / total));
        if ((byte_24668 % total) >= hi) return eax & 0xffffff00UL;
        return (eax & 0xffffff00UL) | channel[0x08];
    }
}

void iplay_eff_13f05(IplayRegs *r, db *channel, db byte_24668) {
    apply_eax_reg(r, iplay_eff_13f05_eax(channel, (db)abi_eax(r), byte_24668, abi_eax(r)));
}

IplayRegs3Result iplay_eff_13f3b_result(db *channel, db input, db byte_24668, db max_volume, dd eax, dd ecx, dd edx) {
    IplayRegs3Result result;
    db al = input;
    db ch;
    db op;
    result.eax = eax;
    result.ecx = ecx;
    result.edx = edx;
    if (al != 0) channel[0x34] = al;
    ch = channel[0x34];
    op = (db)(ch >> 4);
    if ((op & 7) != 0) {
        if ((op & 8) == 0) {
            if (op == 6) channel[0x08] = (db)(((dw)channel[0x08] * 2u) / 3u);
            else if (op == 7) channel[0x08] >>= 1;
            else {
                db delta = (db)(1u << (op - 1u));
                channel[0x08] = (channel[0x08] >= delta) ? (db)(channel[0x08] - delta) : 0;
            }
        } else {
            op &= 7;
            if (op == 6) al = (db)(((dw)channel[0x08] * 3u) >> 1);
            else if (op == 7) al = (db)(channel[0x08] << 1);
            else al = (db)(channel[0x08] + (1u << (op - 1u)));
            if (al > max_volume) al = max_volume;
            channel[0x08] = al;
        }
    }
    result.eax = (eax & 0xffffff00UL) | (ch & 0x0f);
    return iplay_eff_13c64_result(channel, (db)result.eax, byte_24668, result.eax, ecx, edx);
}

void iplay_eff_13f3b(IplayRegs *r, db *channel, db byte_24668, db max_volume) {
    IplayRegs3Result result = iplay_eff_13f3b_result(channel, (db)abi_eax(r), byte_24668, max_volume, abi_eax(r), abi_ecx(r), abi_edx(r));
    apply_eax_edx_regs(r, result.eax, result.edx);
}

dd iplay_change_amplif_eax(db *globals, db sound_mode, dd eax_in) {
    dw ax = (dw)eax_in;
    if (ax != 0xffffu) {
        put_word(globals, 0x005e, ax);
        globals[0x0085] = (ax > 100u) ? 1 : 0;
        globals[0x00dd] = (sound_mode == 0) ? 0x40 : 0x3f;
        iplay_sub_13044(globals, globals + 0x3d68u);
    }
    return (eax_in & 0xffff0000UL) | get_word(globals, 0x005e);
}

void iplay_change_amplif(IplayRegs *r, db *globals, db sound_mode) {
    apply_eax_reg(r, iplay_change_amplif_eax(globals, sound_mode, abi_eax(r)));
}

dd iplay_eff_14020_eax(db *globals, db sound_mode, dd eax) {
    return iplay_change_amplif_eax(globals, sound_mode, (eax & 0xffff0000UL) | (dw)(((db)eax) << 2));
}

void iplay_eff_14020(IplayRegs *r, db *globals, db sound_mode) {
    apply_eax_reg(r, iplay_eff_14020_eax(globals, sound_mode, abi_eax(r)));
}

void iplay_midi_154da(IplayRegs *r, const db *channel) {
    apply_eax_reg(r, (abi_eax(r) & 0xffff00ffUL) | ((dw)channel[0x18] << 8));
}

void iplay_midi_154de(IplayRegs *r, const db *channel) {
    db al = channel[0x35];
    db dl = (db)((al & 0x0f) - 1);
    al >>= 4;
    al = (db)(al * 12 + dl);
    apply_eax_edx_regs(r, (abi_eax(r) & 0xffff0000UL) | ((dw)al << 8) | al,
                       (abi_edx(r) & 0xffffff00UL) | dl);
}

void iplay_midi_154ac(IplayRegs *r, db *channel, db max_volume) {
    db al = (db)abi_eax(r);
    dd eax;
    if (al >= max_volume) al = max_volume;
    eax = (abi_eax(r) & 0xffffff00UL) | al;
    apply_eax_reg(r, eax);
    if (al == channel[0x1b]) return;
    channel[0x1b] = al;
    apply_edi_reg(r, al);
    iplay_midi_154da(r, channel);
    eax = (abi_eax(r) | 0xb000u) & 0xffff00ffUL;
    apply_eax_edi_regs(r, eax | 0x0700u, abi_edi(r));
}

void iplay_midi_15413_guard(IplayRegs *r, db *last_status) {
    db ah = (db)(abi_eax(r) >> 8);
    if ((ah & 0x80) != 0 && ah == *last_status) return;
    *last_status = ah;
}

void iplay_sub_154f4(IplayRegs *r, db *globals, const db *channel) {
    dw buffer_size2 = get_word(globals, 0x0044);
    dw interp_word = get_word(channel, 0x36);
    db interp = 0;
    dd sample_pos = get_dword(channel, 0x04) & 0x0fffu;
    dw period = get_word(channel, 0x20);
    dd ebx;
    dd ebp;
    dd ecx;
    dd esi;
    globals[0x00e3] = (db)(buffer_size2 >> 4);
    globals[0x0074] = (db)interp_word;
    globals[0x0075] = (db)(interp_word >> 8);
    if ((globals[0x00d2] & 0x10) != 0 && channel[0x36] != channel[0x37]) {
        interp = 1;
    }
    globals[0x0076] = interp;
    ebx = (abi_ebx(r) & 0xffff0000UL) | ((dw)channel[0x23] << 9);
    ebp = (abi_ebp(r) & 0xffff0000UL) | (period >> 8);
    ecx = (abi_ecx(r) & 0xffff0000UL) | ((dw)(period & 0xffu) << 8) | (db)(sample_pos >> 8);
    esi = (abi_esi(r) & 0xffff0000UL) | (dw)sample_pos;
    apply_mix_setup_regs(r, ebx, ebp, ecx, esi);
}

static int16_t iplay_mix_read_i16(const db *p) {
    return (int16_t)get_word(p, 0);
}

static int32_t iplay_mix_read_i32(const db *p) {
    return (int32_t)get_dword(p, 0);
}

static void iplay_mix_write_i16(db *p, int16_t value) {
    put_word(p, 0, (dw)value);
}

static void iplay_mix_write_i32(db *p, int32_t value) {
    put_dword(p, 0, (dd)value);
}

void iplay_mix_channel_8bit(
    db *channel,
    const db *samples,
    const int16_t *volume_table,
    db *mix_buffer,
    dw frame_count,
    int interpolation,
    int wide_accumulator) {
    dd old_position;
    dd position;
    dw sample_index;
    db fraction;
    dw period;
    dw integer_step;
    db fractional_step;
    dw frame;

    if ((channel[0x17] & 1u) == 0) return;

    old_position = get_dword(channel, 0x04);
    sample_index = (dw)((old_position & 0x0fffu) >> 8);
    fraction = (db)old_position;
    period = get_word(channel, 0x20);
    integer_step = period >> 8;
    fractional_step = (db)period;

    for (frame = 0; frame < frame_count; ++frame) {
        int32_t value = volume_table[samples[sample_index]];
        unsigned fraction_sum;

        if (interpolation) {
            int32_t next = volume_table[samples[(dw)(sample_index + 1u)]];
            value += ((next - value) * (int32_t)fraction) >> 8;
        }

        if (interpolation || wide_accumulator) {
            int32_t accumulated = iplay_mix_read_i32(mix_buffer);
            iplay_mix_write_i32(mix_buffer, accumulated + value);
        } else {
            int16_t accumulated = iplay_mix_read_i16(mix_buffer);
            iplay_mix_write_i16(mix_buffer, (int16_t)(accumulated + value));
        }

        fraction_sum = (unsigned)fraction + fractional_step;
        sample_index = (dw)(sample_index + integer_step + (fraction_sum >> 8));
        fraction = (db)fraction_sum;
        mix_buffer += 8;
    }

    channel[0x23] = channel[0x37];
    position = (old_position & 0xfffff000UL) + ((dd)sample_index << 8) + fraction;
    if ((position >> 8) <= get_dword(channel, 0x48)) {
        put_dword(channel, 0x04, position);
        return;
    }

    if ((channel[0x19] & 8u) == 0) {
        put_dword(channel, 0x04, get_dword(channel, 0x40) << 8);
        channel[0x17] &= 0xfeu;
        channel[0x35] = 0;
        return;
    }

    {
        dd loop_start = get_dword(channel, 0x40) << 8;
        dd loop_length = get_dword(channel, 0x44) << 8;
        if (loop_length != 0) {
            do {
                dd before = position;
                position -= loop_length;
                if (position > before || position <= loop_start) break;
            } while (1);
            position += loop_length;
        }
        put_dword(channel, 0x04, position);
    }
}

void iplay_sub_15577_disabled(IplayRegs *r, db *channel) {
    (void)r;
    (void)channel;
}

IplayRegs6Result iplay_sub_1609f_disabled_result(db *dst, dw buffer_size, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    dw i;
    for (i = 0; i < buffer_size; ++i) {
        dst[i * 8u + 0] = 0;
        dst[i * 8u + 1] = 0;
        dst[i * 8u + 2] = 0;
        dst[i * 8u + 3] = 0;
    }
    result.eax = eax & 0xffff0000UL;
    result.ebx = (ebx & 0xffff0000UL) | 4u;
    result.ecx = ecx & 0xffff0000UL;
    result.edx = edx;
    result.esi = esi;
    result.edi = (edi & 0xffff0000UL) | (dw)((dw)edi + buffer_size * 8u);
    return result;
}

void iplay_sub_1609f_disabled(IplayRegs *r, db *dst, dw buffer_size) {
    IplayRegs6Result result = iplay_sub_1609f_disabled_result(dst, buffer_size, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

IplayRegs6Result iplay_volume_prep_inactive_result(db *globals, db *dst, dw word_24610, dw size, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    (void)ecx;
    globals[0x0070] = (db)word_24610;
    globals[0x0071] = (db)(word_24610 >> 8);
    globals[0x0072] = (db)size;
    globals[0x0073] = (db)(size >> 8);
    memset(dst, 0, size);
    result.eax = eax;
    result.ebx = ebx;
    result.ecx = 0;
    result.edx = edx;
    result.esi = esi;
    result.edi = (edi & 0xffff0000UL) | (dw)((dw)edi + size);
    return result;
}

void iplay_volume_prep_inactive(IplayRegs *r, db *globals, db *dst, dw word_24610, dw size) {
    IplayRegs6Result result = iplay_volume_prep_inactive_result(globals, dst, word_24610, size, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

IplaySb16RegsResult iplay_sb_helper_no_device_result(const char *symbol, dw base_port, dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result;
    if (strcmp(symbol, "ReadMixerSB") == 0 || strcmp(symbol, "ReadSB") == 0) {
        eax &= 0xffffff00UL;
    } else if (strcmp(symbol, "CheckSB") == 0) {
        eax &= 0xffff0000UL;
        edx = (edx & 0xffff0000UL) | (dw)(base_port + 6u);
    }
    result.eax = eax;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    return result;
}

void iplay_sb_helper_no_device(IplayRegs *r, const char *symbol, dw base_port) {
    IplaySb16RegsResult result = iplay_sb_helper_no_device_result(symbol, base_port, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

void iplay_sb_write_no_device_state(void) {
}

void iplay_sb_write_no_device(IplayRegs *r) {
    (void)r;
    iplay_sb_write_no_device_state();
}

IplaySb16RegsResult iplay_set_dmachn_mask_no_device_result(dw channel, dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result;
    result.eax = (eax & 0xffffff00UL) | (db)((channel & 3u) | 4u);
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    return result;
}

void iplay_set_dmachn_mask_no_device(IplayRegs *r, dw channel) {
    IplaySb16RegsResult result = iplay_set_dmachn_mask_no_device_result(channel, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

IplaySb16RegsResult iplay_adlib_delay_no_device_result(const char *symbol, dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result;
    if (strcmp(symbol, "adlib_18389") == 0) {
        eax = (eax & 0xffff0000UL) | 0x00e9u;
    }
    result.eax = eax;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx;
    return result;
}

void iplay_adlib_delay_no_device(IplayRegs *r, const char *symbol) {
    IplaySb16RegsResult result = iplay_adlib_delay_no_device_result(symbol, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

void iplay_adlib_delay_public(IplayRegs *r) {
    dd eax = (abi_eax(r) & 0xffff0000UL) | 0x00e9u;
    apply_full_regs6(r, eax, abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
}

IplayRegs6Result iplay_sb_legacy_init_no_device_result(db *globals, int sbpro_mode, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    globals[0x0082] = 0x09;
    globals[0x0083] = sbpro_mode ? 1 : 0;
    globals[0x0084] = 0x08;
    result.eax = eax & 0xffff0000UL;
    result.ebx = ebx & 0xffff0000UL;
    result.ecx = ecx & 0xffff0000UL;
    result.edx = (edx & 0xffff0000UL) | 0x0ff6u;
    result.esi = (esi & 0xffff0000UL) | 0x0085u;
    result.edi = (edi & 0xffff0000UL) | 0x2803u;
    return result;
}

void iplay_sb_legacy_init_no_device(IplayRegs *r, db *globals, int sbpro_mode) {
    IplayRegs6Result result = iplay_sb_legacy_init_no_device_result(globals, sbpro_mode, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

IplaySb16RegsResult iplay_sb_detect_irq_no_device_result(dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result;
    result.eax = eax & 0xffff0000UL;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = (edx & 0xffff0000UL) | 0x0ff6u;
    return result;
}

void iplay_sb_detect_irq_no_device(IplayRegs *r) {
    IplaySb16RegsResult result = iplay_sb_detect_irq_no_device_result(abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

void iplay_sb_detect_irq_public(IplayRegs *r) {
    IplaySb16RegsResult result = iplay_sb_detect_irq_no_device_result(abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

IplayRegs6Result iplay_sb_test_interrupt_no_device_result(db *counter, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    *counter = 0;
    result.eax = (eax & 0xffff0000UL) | 0x0054u;
    result.ebx = (ebx & 0xffff0000UL) | 0x0020u;
    result.ecx = ecx & 0xffff0000UL;
    result.edx = edx;
    result.esi = (esi & 0xffff0000UL) | 0x00d1u;
    result.edi = (edi & 0xffff0000UL) | 0x2801u;
    return result;
}

void iplay_sb_test_interrupt_no_device(IplayRegs *r, db *counter) {
    IplayRegs6Result result = iplay_sb_test_interrupt_no_device_result(counter, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

size_t iplay_sb16_start_commands(
    db *commands,
    size_t capacity,
    dw output_frequency,
    db bit_mode,
    db stereo_flag,
    dw dma_block_bytes) {
    dw transfer_count = (dw)(dma_block_bytes >> 2);
    db mode = (db)((stereo_flag & 1u) << 5);

    if (bit_mode == 16u) {
        transfer_count >>= 1;
        mode |= 0x10u;
    }
    if (transfer_count != 0u) --transfer_count;

    if (commands != NULL && capacity >= 7u) {
        commands[0] = 0x41u;
        commands[1] = (db)(output_frequency >> 8);
        commands[2] = (db)output_frequency;
        commands[3] = bit_mode == 16u ? 0xb6u : 0xc6u;
        commands[4] = mode;
        commands[5] = (db)transfer_count;
        commands[6] = (db)(transfer_count >> 8);
    }
    return 7u;
}

size_t iplay_sb16_dma_channel5_events(
    db *events,
    size_t capacity,
    dw buffer_segment,
    dw buffer_offset,
    dd buffer_addend,
    dw dma_block_bytes,
    db dma_mode,
    dw config_word) {
    dd physical = ((dd)buffer_segment << 4) + buffer_offset + buffer_addend;
    dw word_address = (dw)(physical >> 1);
    dw word_count = (dw)((dma_block_bytes + 1u) >> 1);
    static const dw ports[9] = {
        0x00d4u, 0x00d8u, 0x00d6u, 0x00c4u, 0x00c4u,
        0x008bu, 0x00c6u, 0x00c6u, 0x00d4u
    };
    db values[9];
    size_t i;

    if (word_count != 0u) --word_count;
    values[0] = 5u;
    values[1] = 0u;
    if ((config_word & 0x1000u) != 0u) dma_mode &= 0xefu;
    values[2] = (db)(dma_mode | 1u);
    values[3] = (db)word_address;
    values[4] = (db)(word_address >> 8);
    values[5] = (db)(physical >> 16);
    values[6] = (db)word_count;
    values[7] = (db)(word_count >> 8);
    values[8] = 1u;

    if (events != NULL && capacity >= 27u) {
        for (i = 0; i < 9u; ++i) {
            events[i * 3u] = (db)ports[i];
            events[i * 3u + 1u] = (db)(ports[i] >> 8);
            events[i * 3u + 2u] = values[i];
        }
    }
    return 9u;
}

void iplay_sb_on_bounded(db *globals, const char *symbol) {
    (void)symbol;
    globals[0x006e] = 0x00;
    globals[0x006f] = 0x10;
    globals[0x00ce] = 0x01;
    globals[0x00cf] = 0x58;
}

void iplay_sb_handler_int_bounded_state(db *globals) {
    (void)globals;
}

void iplay_sb_handler_int_bounded(IplayRegs *r, db *globals) {
    (void)r;
    iplay_sb_handler_int_bounded_state(globals);
}

IplaySb16RegsResult iplay_sub_19050_bounded_result(dd eax, dd ebx, dd ecx, dd edx) {
    IplaySb16RegsResult result;
    result.eax = (eax & 0xffff0000UL) | 0x0900u;
    result.ebx = ebx;
    result.ecx = ecx;
    result.edx = edx & 0xffff0000UL;
    return result;
}

void iplay_sub_19050_bounded(IplayRegs *r, db *globals) {
    IplaySb16RegsResult result;
    (void)globals;
    result = iplay_sub_19050_bounded_result(abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));
}

IplayRegs6Result iplay_memfill8080_result(db *dma, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result;
    memset(dma, 0x80, 0x400u * 4u);
    result.eax = (eax & 0xffff0000UL) | 0x8080u;
    result.ebx = (ebx & 0xffff0000UL) | 0xdef0u;
    result.ecx = ecx & 0xffff0000UL;
    result.edx = edx;
    result.esi = esi;
    result.edi = edi;
    return result;
}

void iplay_memfill8080(IplayRegs *r, db *dma) {
    IplayRegs6Result result = iplay_memfill8080_result(dma, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

IplayRegs6Result iplay_sndoff_fill_result(db *dma, const char *symbol, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi) {
    IplayRegs6Result result = iplay_memfill8080_result(dma, eax, ebx, ecx, edx, esi, edi);
    if (strcmp(symbol, "pcspeaker_sndoff") == 0) {
        result.eax = (result.eax & 0xffff0000UL) | 0x8000u;
    }
    return result;
}

void iplay_sndoff_fill(IplayRegs *r, db *dma, const char *symbol) {
    IplayRegs6Result result = iplay_sndoff_fill_result(dma, symbol, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));
    apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, result.esi, result.edi);
}

db iplay_audio_init_failure(db *globals, db *text, const char *symbol) {
    static const db covox_text[16] = {0x08,0xf1,0x8e,0xda,0xa0,0x00,0xf0,0xba,0x03,0x00,0xee,0xb0,0x20,0xe6,0x20,0x1f};
    static const db stereo_text[16] = {0x08,0xf1,0x8e,0xda,0xba,0x05,0x00,0xb0,0x02,0xee,0x80,0xea,0x02,0xa1,0x00,0xf0};
    static const db pc_text[16] = {0x08,0xf1,0x8e,0xdb,0x32,0xff,0x8a,0x1e,0x00,0xf0,0x2e,0x8a,0x9f,0xe8,0x51,0x8a};
    static const db adlib_text[16] = {0xb8,0x08,0xf1,0x8e,0xd8,0xa0,0x00,0xf0,0xbb,0x6a,0x15,0x8e,0xdb,0xbb,0xf8,0x02};
    const db *src = covox_text;
    globals[0x0082] = 3;
    globals[0x0083] = 0;
    globals[0x0084] = 8;
    globals[0x0132] = 0x78;
    globals[0x0133] = 0x03;
    put_dword(globals, 0x0018, 0);
    if (strcmp(symbol, "stereo_init") == 0) {
        src = stereo_text;
        globals[0x0083] = 1;
    } else if (strcmp(symbol, "pcspeaker_init") == 0) {
        src = pc_text;
    } else if (strcmp(symbol, "adlib_init") == 0) {
        src = adlib_text;
        globals[0x0082] = 0x0b;
    }
    memcpy(text, src, 16);
    return 1;
}

IplaySndSettingsResult iplay_read_sndsettings_result(
    dd eax,
    dd ebx,
    dd ecx,
    dd edx,
    dd ebp,
    dd esi,
    db sndcard_type,
    dw snd_base_port,
    db irq_number,
    db dma_channel,
    db freq_code,
    db byte_246d8,
    db byte_246d9,
    dw snd_output_frq,
    dw freq2,
    dw config_word,
    db sndflags) {
    IplaySndSettingsResult result;
    result.eax = (eax & 0xffff0000UL) | ((dw)freq_code << 8) | sndcard_type;
    result.edx = (edx & 0xffff0000UL) | snd_base_port;
    result.ecx = (ecx & 0xffff0000UL) | ((dw)dma_channel << 8) | irq_number;
    result.ebx = (ebx & 0xffff0000UL) | ((dw)byte_246d9 << 8) | byte_246d8;
    result.ebp = (ebp & 0xffff0000UL) | ((sndflags & 4) ? freq2 : snd_output_frq);
    result.esi = (esi & 0xffff0000UL) | config_word;
    return result;
}

void iplay_read_sndsettings(
    IplayRegs *r,
    db sndcard_type,
    dw snd_base_port,
    db irq_number,
    db dma_channel,
    db freq_code,
    db byte_246d8,
    db byte_246d9,
    dw snd_output_frq,
    dw freq2,
    dw config_word,
    db sndflags) {
    IplaySndSettingsResult result = iplay_read_sndsettings_result(
        abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_ebp(r), abi_esi(r),
        sndcard_type, snd_base_port, irq_number, dma_channel, freq_code,
        byte_246d8, byte_246d9, snd_output_frq, freq2, config_word, sndflags);
    apply_sndsettings_regs(r, result.eax, result.ebx, result.ecx, result.edx, result.ebp, result.esi);
}
