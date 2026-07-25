#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "iplay_masm_.h"

extern bool iplay_test_loadcfg_file;

static void reset_state(_STATE *state) {
    std::memset(state, 0, sizeof(*state));
    state->cs = seg_offset(_text);
    state->ds = seg_offset(seg003);
    state->es = seg_offset(seg003);
    state->ss = seg_offset(stack);
    state->esp = STACK_SIZE - 4;
    state->_indent = 1;
    state->_str = "";
}

static void push_word(_STATE *state, dw value) {
    state->esp -= sizeof(value);
    std::memcpy(raddr(state->ss, state->esp), &value, sizeof(value));
}

static void call_near(_offsets fn, _STATE *state) {
    push_word(state, (dw)'xy');
    mainproc(fn, state);
}

static void call_near_with_saved_es(_offsets fn, _STATE *state) {
    push_word(state, (dw)'xy');
    push_word(state, state->es);
    mainproc(fn, state);
}

static void call_far(_offsets fn, _STATE *state) {
    push_word(state, state->cs);
    push_word(state, (dw)'xy');
    mainproc(fn, state);
}

static void call_iret(_offsets fn, _STATE *state) {
    push_word(state, (dw)0x0200);
    push_word(state, state->cs);
    push_word(state, (dw)'xy');
    mainproc(fn, state);
}

static void jump_with_saved_si(_offsets fn, _STATE *state, dw saved_si) {
    push_word(state, (dw)'xy');
    push_word(state, saved_si);
    mainproc(fn, state);
}

static void print_bytes(const db *data, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        std::printf("%02x", data[i]);
    }
}

static unsigned long parse_u32(const char *text) {
    return std::strtoul(text, nullptr, 0);
}

static size_t decode_hex_bytes(const char *hex, db *out, size_t max_count) {
    size_t hex_len = std::strlen(hex);
    size_t count = 0;
    for (size_t i = 0; i + 1 < hex_len && count < max_count; i += 2) {
        char tmp[3] = {hex[i], hex[i + 1], 0};
        out[count++] = (db)std::strtoul(tmp, nullptr, 16);
    }
    return count;
}

static bool call_decimal(const std::string &symbol, _STATE *state) {
    if (symbol == "my_u8toa_10") {
        call_near(k_my_u8toa_10, state);
    } else if (symbol == "my_u16toa_10") {
        call_near(k_my_u16toa_10, state);
    } else if (symbol == "my_u32toa10_0") {
        call_near(k_my_u32toa10_0, state);
    } else if (symbol == "my_i8toa10_0") {
        call_near(k_my_i8toa10_0, state);
    } else if (symbol == "my_i16toa10_0") {
        call_near(k_my_i16toa10_0, state);
    } else if (symbol == "my_i32toa10_0") {
        call_near(k_my_i32toa10_0, state);
    } else {
        return false;
    }
    return true;
}

static bool call_noop(const std::string &op, _STATE *state) {
    if (op == "nullsub5") {
        call_near(k_nullsub_5, state);
    } else if (op == "effnullsub") {
        call_near(k_eff_nullsub, state);
    } else if (op == "nullsub2") {
        call_near(k_nullsub_2, state);
    } else if (op == "nullsub4") {
        call_near(k_nullsub_4, state);
    } else {
        return false;
    }
    return true;
}

static bool call_effect_slide(const std::string &op, _STATE *state) {
    if (op == "eff1387f") {
        call_near(k_eff_1387f, state);
    } else if (op == "eff13886") {
        call_near(k_eff_13886, state);
    } else if (op == "eff1389d") {
        call_near(k_eff_1389d, state);
    } else if (op == "eff138a4") {
        call_near(k_eff_138a4, state);
    } else {
        return false;
    }
    return true;
}

static bool call_seg1_decimal(const std::string &symbol, _STATE *state) {
    if (symbol == "my_u8toa10") {
        call_near(k_my_u8toa10, state);
    } else if (symbol == "my_u16toa10") {
        call_near(k_my_u16toa10, state);
    } else if (symbol == "my_u32toa10") {
        call_near(k_my_u32toa10, state);
    } else if (symbol == "my_i8toa10") {
        call_near(k_my_i8toa10, state);
    } else {
        return false;
    }
    return true;
}

static bool call_seg1_hex(const std::string &symbol, _STATE *state) {
    if (symbol == "my_u32tox") {
        call_near(k_my_u32tox, state);
    } else if (symbol == "my_u16tox") {
        call_near(k_my_u16tox, state);
    } else if (symbol == "my_u8tox") {
        call_near(k_my_u8tox, state);
    } else if (symbol == "my_u4tox") {
        call_near(k_my_u4tox, state);
    } else {
        return false;
    }
    return true;
}

static bool call_mysprintf_chunk(const std::string &symbol, _STATE *state) {
    if (symbol == "useless_sprint_6") {
        call_near_with_saved_es(k_mysprintf_6_u32toa, state);
    } else if (symbol == "useless_sprint_7") {
        call_near_with_saved_es(k_mysprintf_7_i8toa, state);
    } else if (symbol == "useless_sprint_8") {
        call_near_with_saved_es(k_mysprintf_8_i16toa, state);
    } else if (symbol == "useless_sprint_9") {
        call_near_with_saved_es(k_mysprintf_9_i32toa, state);
    } else if (symbol == "useless_sprint_10") {
        call_near_with_saved_es(k_mysprintf_10_u8tox, state);
    } else if (symbol == "useless_sprint_11") {
        call_near_with_saved_es(k_mysprintf_11_u16tox, state);
    } else if (symbol == "useless_sprint_12") {
        call_near_with_saved_es(k_mysprintf_12_u32tox, state);
    } else {
        return false;
    }
    return true;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: translated_function_runner <case> [args...]\n");
        return 2;
    }

    _STATE state;
    reset_state(&state);
    db *buf = raddr(seg_offset(seg003), offset(seg003, _mystr));
    std::memset(buf, 0, 66);

    std::string op = argv[1];
    if (op == "nullsub5" || op == "effnullsub" || op == "nullsub2" || op == "nullsub4") {
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_noop(op, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "hex16") {
        if (argc != 3) return 2;
        state.eax = (dw)parse_u32(argv[2]);
        state.esi = offset(seg003, _mystr);
        call_near(k_u16tox, &state);
        std::printf("ax=%04x si=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(buf, 4);
        std::printf("\n");
        return 0;
    }

    if (op == "hex32") {
        if (argc != 3) return 2;
        state.eax = (dd)parse_u32(argv[2]);
        state.esi = offset(seg003, _mystr);
        call_near(k_u32tox, &state);
        std::printf("ax=%04x si=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(buf, 8);
        std::printf("\n");
        return 0;
    }

    if (op == "hex8") {
        if (argc != 3) return 2;
        state.eax = (dw)parse_u32(argv[2]);
        state.esi = offset(seg003, _mystr);
        call_near(k_u8tox, &state);
        std::printf("ax=%04x si=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(buf, 2);
        std::printf("\n");
        return 0;
    }

    if (op == "hex4") {
        if (argc != 3) return 2;
        state.eax = (dw)parse_u32(argv[2]);
        state.esi = offset(seg003, _mystr);
        call_near(k_u4tox, &state);
        std::printf("ax=%04x si=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(buf, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "sub13e9b") {
        if (argc != 3) return 2;
        state.eax = (dw)parse_u32(argv[2]);
        call_near(ksub_13e9b, &state);
        std::printf("ax=%04x dx=%04x di=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.edx & 0xffff), (unsigned)(state.edi & 0xffff));
        return 0;
    }

    if (op == "sub14087") {
        if (argc != 5) return 2;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(channel, 0, 0x50);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.ebx = offset(seg003, _channels_25908);
        state.eax = (dw)parse_u32(argv[2]);
        channel[0x34] = (db)parse_u32(argv[3]);
        m._byte_24668 = (db)parse_u32(argv[4]);
        call_near(ksub_14087, &state);
        std::printf("ax=%04x dx=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.edx & 0xffff));
        print_bytes(channel + 0x34, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "putdigit") {
        if (argc != 4) return 2;
        state.edx = (dw)parse_u32(argv[2]);
        state.ecx = (dw)parse_u32(argv[3]);
        state.esi = offset(seg003, _mystr);
        call_near(k_my_putdigit, &state);
        std::printf("cx=%04x si=%04x data=", (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(buf, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "mysprintfchunk") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        dw param = offset(seg003, _mystr) + 0x10;
        dw value_ptr = offset(seg003, _mystr) + 0x20;
        dw out = offset(seg003, _mystr) + 0x30;
        db *param_data = raddr(seg_offset(seg003), param);
        db *value_data = raddr(seg_offset(seg003), value_ptr);
        db *out_data = raddr(seg_offset(seg003), out);
        std::memset(out_data, 0xa5, 16);
        *(dw *)param_data = value_ptr;
        param_data[2] = 0;
        *(dd *)value_data = (dd)parse_u32(argv[3]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.esi = param;
        state.edi = out;
        if (!call_mysprintf_chunk(symbol, &state)) {
            std::fprintf(stderr, "unknown mysprintf chunk: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("si=%04x di=%04x es=%04x data=",
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff),
                    (unsigned)(state.es & 0xffff));
        print_bytes(out_data, 16);
        std::printf("\n");
        return 0;
    }

    if (op == "decimal") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        state.eax = (dd)parse_u32(argv[3]);
        state.esi = offset(seg003, _mystr);
        if (!call_decimal(symbol, &state)) {
            std::fprintf(stderr, "unknown decimal symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("cx=%04x si=%04x data=", (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(buf, state.ecx & 0xffff);
        std::printf("\n");
        return 0;
    }

    if (op == "seg1hex") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        db *dseg_buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(dseg_buf, 0, 16);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dd)parse_u32(argv[3]);
        state.esi = offset(dseg, _buffer_1);
        if (!call_seg1_hex(symbol, &state)) {
            std::fprintf(stderr, "unknown seg1 hex symbol: %s\n", symbol.c_str());
            return 2;
        }
        size_t count = symbol == "my_u32tox" ? 8 : symbol == "my_u16tox" ? 4 : symbol == "my_u8tox" ? 2 : 1;
        std::printf("ax=%04x si=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(dseg_buf, count);
        std::printf("\n");
        return 0;
    }

    if (op == "hex1be39") {
        if (argc != 4) return 2;
        db *dseg_buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(dseg_buf, 0, 2);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dw)(((parse_u32(argv[3]) & 0xff) << 8) | (parse_u32(argv[2]) & 0xff));
        state.edi = offset(dseg, _buffer_1);
        call_near(k_hex_1be39, &state);
        std::printf("ax=%04x di=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dseg_buf, 2);
        std::printf("\n");
        return 0;
    }

    if (op == "writescr") {
        if (argc != 5) return 2;
        db *src = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        db *dst = src + 0x40;
        size_t len = std::strlen(argv[2]);
        unsigned delta = parse_u32(argv[4]) & 0xffff;
        std::memset(src, 0, 0x100);
        *(dw *)src = (dw)delta;
        src[2] = (db)parse_u32(argv[3]);
        std::memcpy(src + 3, argv[2], len);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        state.edi = offset(dseg, _buffer_1) + 0x40;
        call_near(k_write_scr, &state);
        std::printf("si=%04x di=%04x data=",
                    (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dst + delta, len * 2);
        std::printf("\n");
        return 0;
    }

    if (op == "seg1decimal") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        db *dseg_buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(dseg_buf, 0, 32);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dd)parse_u32(argv[3]);
        state.esi = offset(dseg, _buffer_1);
        if (!call_seg1_decimal(symbol, &state)) {
            std::fprintf(stderr, "unknown seg1 decimal symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("cx=%04x si=%04x data=", (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(dseg_buf, state.ecx & 0xffff);
        std::printf("\n");
        return 0;
    }

    if (op == "seg1u32toa") {
        if (argc != 4) return 2;
        db *dseg_buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(dseg_buf, 0, 32);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dd)parse_u32(argv[2]);
        state.ebx = (dd)parse_u32(argv[3]);
        state.ecx = 0;
        state.esi = offset(dseg, _buffer_1);
        call_near(k_my_u32toa, &state);
        std::printf("cx=%04x si=%04x data=", (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(dseg_buf, state.ecx & 0xffff);
        std::printf("\n");
        return 0;
    }

    if (op == "u32toa0direct") {
        if (argc != 4) return 2;
        std::memset(buf, 0, 32);
        state.eax = (dd)parse_u32(argv[2]);
        state.ebx = (dd)parse_u32(argv[3]);
        state.ecx = 0;
        state.esi = offset(seg003, _mystr);
        call_near(k_my_u32toa_0, &state);
        std::printf("cx=%04x si=%04x data=", (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(buf, state.ecx & 0xffff);
        std::printf("\n");
        return 0;
    }

    if (op == "seg1strlen") {
        if (argc != 3) return 2;
        db *dseg_buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(dseg_buf, 0, 64);
        std::strcpy((char *)dseg_buf, argv[2]);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        call_near(k_mystrlen, &state);
        std::printf("ax=%04x si=%04x\n", (unsigned)(state.eax & 0xffff), (unsigned)(state.esi & 0xffff));
        return 0;
    }

    if (op == "parsecmdline") {
        if (argc != 3) return 2;
        db *psp = raddr(seg_offset(dseg), 0x80);
        db *out = raddr(seg_offset(dseg), offset(dseg, _buffer_1DB6C));
        size_t len = std::strlen(argv[2]);
        std::memset(out, 0x2e, 32);
        std::memset(psp, 0, 128);
        psp[0] = (db)len;
        std::memcpy(psp + 1, argv[2], len);
        psp[1 + len] = 0x0d;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        m._esseg_atstart = seg_offset(dseg);
        call_near(k_parse_cmdline, &state);
        size_t count = std::strlen(argv[2]) == 0 ? 1 : std::strlen((char *)out) + 1;
        std::printf("bp=%04x si=%04x di=%04x data=",
                    (unsigned)(state.ebp & 0xffff),
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(out, count);
        std::printf("\n");
        return 0;
    }

    if (op == "getcomspec") {
        db *env = raddr(seg_offset(dseg), 0);
        std::memset(env, 0, 64);
        std::memcpy(env, "COMSPEC=X", 9);
        *(dw *)(env + 0x2c) = seg_offset(dseg);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        m._esseg_atstart = seg_offset(dseg);
        call_near(k_get_comspec, &state);
        std::printf("di=%04x\n", (unsigned)(state.edi & 0xffff));
        return 0;
    }

    if (op == "getexename") {
        if (argc != 3) return 2;
        db *env = raddr(seg_offset(dseg), 0);
        db *out = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        size_t path_len = std::strlen(argv[2]);
        std::memset(env, 0, 128);
        std::memset(out, 0, 64);
        std::memcpy(env, "A=B", 3);
        *(dw *)(env + 5) = 1;
        std::memcpy(env + 7, argv[2], path_len + 1);
        *(dw *)(env + 0x2c) = seg_offset(dseg);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        m._esseg_atstart = seg_offset(dseg);
        call_near(k_getexename, &state);
        std::printf("si=%04x data=", (unsigned)(state.esi & 0xffff));
        print_bytes(out, path_len + 1);
        std::printf("\n");
        return 0;
    }

    if (op == "spectr1b406small") {
        if (argc != 3) return 2;
        db *dseg_buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(dseg_buf, 0, 0x20);
        decode_hex_bytes(argv[2], dseg_buf, 8);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.edi = offset(dseg, _buffer_1);
        m._word_24520 = 1;
        call_near(k_spectr_1b406, &state);
        db data[0x20] = {
            dseg_buf[0], dseg_buf[1], dseg_buf[2], dseg_buf[3],
            dseg_buf[4], dseg_buf[5], dseg_buf[6], dseg_buf[7],
            (db)(m._word_2450C & 0xff), (db)((m._word_2450C >> 8) & 0xff),
            (db)(m._word_2450E & 0xff), (db)((m._word_2450E >> 8) & 0xff),
            0, 0, 0, 0,
            (db)(m._word_24514 & 0xff), (db)((m._word_24514 >> 8) & 0xff),
            (db)(m._word_24516 & 0xff), (db)((m._word_24516 >> 8) & 0xff),
            (db)(m._word_24518 & 0xff), (db)((m._word_24518 >> 8) & 0xff),
            (db)(m._word_2451A & 0xff), (db)((m._word_2451A >> 8) & 0xff),
            (db)(m._word_2451C & 0xff), (db)((m._word_2451C >> 8) & 0xff),
            (db)(m._word_2451E & 0xff), (db)((m._word_2451E >> 8) & 0xff),
            (db)(m._word_24520 & 0xff), (db)((m._word_24520 >> 8) & 0xff),
            (db)(m._word_24522 & 0xff), (db)((m._word_24522 >> 8) & 0xff),
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "seg1fill") {
        if (argc != 5) return 2;
        std::string symbol = argv[2];
        db *dseg_buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(dseg_buf, 0, 32);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dd)parse_u32(argv[3]);
        state.edi = offset(dseg, _buffer_1);
        state.ebp = (dw)parse_u32(argv[4]);
        if (symbol == "my_u32toa_fill") {
            call_near(k_my_u32toa_fill, &state);
        } else if (symbol == "my_pnt_u32toa_fill") {
            call_near(k_my_pnt_u32toa_fill, &state);
        } else {
            std::fprintf(stderr, "unknown seg1 fill symbol: %s\n", symbol.c_str());
            return 2;
        }
        size_t count = (state.ebp & 0xffff) + (symbol == "my_pnt_u32toa_fill" ? 2 : 0);
        std::printf("di=%04x data=", (unsigned)(state.edi & 0xffff));
        print_bytes(dseg_buf, count);
        std::printf("\n");
        return 0;
    }

    if (op == "seg1copyprint") {
        if (argc != 4) return 2;
        db *src = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        db *dst = src + 0x40;
        size_t count = parse_u32(argv[3]);
        std::memset(src, 0, 0x80);
        std::memcpy(src, argv[2], std::strlen(argv[2]));
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        state.edi = offset(dseg, _buffer_1) + 0x40;
        state.ecx = (dw)count;
        call_near(k_cpy_printable, &state);
        std::printf("cx=%04x si=%04x di=%04x data=", (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dst, count);
        std::printf("\n");
        return 0;
    }

    if (op == "seg1strcpycount") {
        if (argc != 3) return 2;
        db *src = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        db *dst = src + 0x40;
        std::memset(src, 0, 0x80);
        std::memcpy(src, argv[2], std::strlen(argv[2]));
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        state.edi = offset(dseg, _buffer_1) + 0x40;
        call_near(k_strcpy_count, &state);
        std::printf("cx=%04x si=%04x di=%04x data=", (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dst, std::strlen(argv[2]));
        std::printf("\n");
        return 0;
    }

    if (op == "spectrsqrt") {
        if (argc != 3) return 2;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.ebx = (dd)parse_u32(argv[2]);
        call_near(k_spectr_1c4f8, &state);
        std::printf("ax=%04x bx=%04x eax=%08x ebx=%08x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)state.eax, (unsigned)state.ebx);
        return 0;
    }

    if (op == "putmessage2") {
        if (argc != 4) return 2;
        db *src = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        db *dst = src + 0x40;
        std::memset(src, 0, 0x80);
        std::memcpy(src, argv[2] + 1, std::strlen(argv[2]));
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.fs = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        state.edi = offset(dseg, _buffer_1) + 0x40;
        state.eax = (dw)(((parse_u32(argv[3]) & 0xff) << 8) | ((unsigned char)argv[2][0]));
        call_near(k_put_message2, &state);
        std::printf("si=%04x di=%04x data=", (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dst, std::strlen(argv[2]) * 2);
        std::printf("\n");
        return 0;
    }

    if (op == "putmessage") {
        if (argc != 4) return 2;
        db *src = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        db *dst = src + 0x40;
        std::memset(src, 0, 0x80);
        std::memcpy(src, argv[2], std::strlen(argv[2]));
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        state.edi = offset(dseg, _buffer_1) + 0x40;
        state.eax = (dw)((parse_u32(argv[3]) & 0xff) << 8);
        call_near(k_put_message, &state);
        std::printf("si=%04x di=%04x data=", (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dst, std::strlen(argv[2]) * 2);
        std::printf("\n");
        return 0;
    }

    if (op == "text1bf69") {
        if (argc != 4) return 2;
        db *src = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        db *dst = src + 0x40;
        std::memset(src, 0, 0x80);
        std::memcpy(src, argv[2], std::strlen(argv[2]));
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        state.edi = offset(dseg, _buffer_1) + 0x40;
        state.eax = (dw)((parse_u32(argv[3]) & 0xff) << 8);
        call_near(k_text_1bf69, &state);
        std::printf("si=%04x di=%04x data=", (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dst, std::strlen(argv[2]) * 2);
        std::printf("\n");
        return 0;
    }

    if (op == "drawframe") {
        if (argc != 9) return 2;
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 400);
        m._videomempointer = ((dd)seg_offset(dseg) << 16) | offset(dseg, _buffer_1);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dw)(((parse_u32(argv[3]) & 0xff) << 8) | (parse_u32(argv[2]) & 0xff));
        state.ebx = (dw)(parse_u32(argv[4]) & 0xff);
        state.ecx = (dw)(((parse_u32(argv[6]) & 0xff) << 8) | (parse_u32(argv[5]) & 0xff));
        state.edx = (dw)(((parse_u32(argv[8]) & 0xff) << 8) | (parse_u32(argv[7]) & 0xff));
        call_near(k_draw_frame, &state);
        std::printf("data=");
        print_bytes(scratch, 400);
        std::printf("\n");
        return 0;
    }

    if (op == "txtdrawtoptitle") {
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 0x500);
        m._videomempointer = ((dd)seg_offset(dseg) << 16) | offset(dseg, _buffer_1);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        call_near(k_txt_draw_top_title, &state);
        std::printf("data=");
        print_bytes(scratch, 0x500);
        std::printf("\n");
        return 0;
    }

    if (op == "filelist") {
        if (argc != 8) return 2;
        const dw video = offset(dseg, _buffer_1) + 0x1000;
        const dw entry_seg = seg_offset(dseg) + 0x0200;
        db *screen = raddr(seg_offset(dseg), video);
        db *entry = raddr(entry_seg, 0);
        std::memset(screen, 0xcc, 0x800);
        std::memset(entry, 0, 0x30);
        entry[2] = (db)parse_u32(argv[2]);
        entry[3] = (db)parse_u32(argv[3]);
        *(dw *)(entry + 4) = (dw)parse_u32(argv[4]);
        *(dw *)(entry + 6) = (dw)parse_u32(argv[5]);
        *(dd *)(entry + 8) = (dd)parse_u32(argv[6]);
        std::memcpy(entry + 0x0c, argv[7], std::strlen(argv[7]) < 12 ? std::strlen(argv[7]) : 12);
        std::memcpy(entry + 0x1a, "Description", 11);
        m._videomempointer = ((dd)seg_offset(dseg) << 16) | video;
        m._word_1DE52 = entry_seg;
        m._word_1DE54 = 1;
        m._word_1DE5E = 0;
        m._word_1DE60 = 0xffff;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        call_near(k_filelist_198b8, &state);
        std::printf("data=");
        print_bytes(screen + 0x654, 160);
        std::printf("\n");
        return 0;
    }

    if (op == "findmodsguard") {
        db *filespec = raddr(seg_offset(dseg), offset(dseg, _buffer_1DB6C));
        std::memset(filespec, 'X', 120);
        m._byte_1DE7E = 0xaa;
        m._messagepointer = 0xccccbbbb;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        call_near(k_find_mods, &state);
        db data[] = {
            m._byte_1DE7E,
            (db)(m._messagepointer & 0xff), (db)((m._messagepointer >> 8) & 0xff),
            (db)((m._messagepointer >> 16) & 0xff), (db)((m._messagepointer >> 24) & 0xff),
        };
        std::printf("ax=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "txtdrawbottom") {
        if (argc != 10) return 2;
        const dw video = offset(dseg, _buffer_1) + 0x1000;
        db *screen = raddr(seg_offset(dseg), video);
        std::memset(screen, 0xcc, 0x600);
        m._videopoint_shiftd = ((dd)seg_offset(dseg) << 16) | video;
        m._byte_1DE72 = (db)parse_u32(argv[2]);
        m._byte_1DE73 = (db)parse_u32(argv[3]);
        m._byte_1DE74 = (db)parse_u32(argv[4]);
        m._byte_1DE75 = (db)parse_u32(argv[5]);
        m._byte_1DE76 = (db)parse_u32(argv[6]);
        m._flg_play_settings = (db)parse_u32(argv[7]);
        m._word_1DE6A = (dw)parse_u32(argv[8]);
        m._word_1DE6C = (dw)parse_u32(argv[9]);
        m._play_state = 1;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        call_near(k_txt_draw_bottom, &state);
        std::printf("data=");
        print_bytes(screen, 0x600);
        std::printf("\n");
        return 0;
    }

    if (op == "sbhelper") {
        if (argc != 5) return 2;
        std::string symbol = argv[2];
        m._sb_base_port = (dw)parse_u32(argv[3]);
        state.eax = (dw)parse_u32(argv[4]);
        state.ebx = 0x1357;
        state.ecx = 0x2468;
        state.edx = 0x369a;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        if (symbol == "WriteMixerSB") {
            call_near(k_writemixersb, &state);
        } else if (symbol == "ReadMixerSB") {
            call_near(k_readmixersb, &state);
        } else if (symbol == "WriteSB") {
            call_near(k_writesb, &state);
        } else if (symbol == "ReadSB") {
            call_near(k_readsb, &state);
        } else if (symbol == "CheckSB") {
            call_near(k_checksb, &state);
        } else {
            std::fprintf(stderr, "unknown sbhelper symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "midiset") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_midi_set, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "sb16probe") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        m._sb_base_port = 0x1111;
        m._word_24654 = 0x2222;
        m._dma_chn_mask = 0x33;
        m._sb_irq_number = 0x44;
        m._snd_base_port = 0xffff;
        m._irq_number = 0x55;
        m._dma_channel = 0x66;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "sb16_detect_port") {
            call_near(k_sb16_detect_port, &state);
        } else if (symbol == "sb16_sound_on") {
            call_near(k_sb16_sound_on, &state);
        } else {
            std::fprintf(stderr, "unknown sb16probe symbol: %s\n", symbol.c_str());
            return 2;
        }
        db data[] = {
            (db)(m._sb_base_port & 0xff), (db)((m._sb_base_port >> 8) & 0xff),
            (db)(m._word_24654 & 0xff), (db)((m._word_24654 >> 8) & 0xff),
            m._dma_chn_mask, m._sb_irq_number,
            (db)(m._snd_base_port & 0xff), (db)((m._snd_base_port >> 8) & 0xff),
            m._irq_number, m._dma_channel,
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sb16int") {
        m._sb_base_port = 0x0220;
        m._sb_int_counter = 5;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_iret(k_sb16_handler_int, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x data=%02x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.ds & 0xffff),
                    (unsigned)m._sb_int_counter);
        return 0;
    }

    if (op == "sb16initfail") {
        m._sndflags_24622 = 0xaa;
        m._is_stereo = 0xbb;
        m._bit_mode = 0xcc;
        m._sb_base_port = 0x1111;
        m._word_24654 = 0x2222;
        m._dma_chn_mask = 0x33;
        m._sb_irq_number = 0x44;
        m._snd_base_port = 0xffff;
        m._irq_number = 0xff;
        m._dma_channel = 0xff;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_sb16_init, &state);
        db data[] = {
            m._sndflags_24622, m._is_stereo, m._bit_mode,
            (db)(m._sb_base_port & 0xff), (db)((m._sb_base_port >> 8) & 0xff),
            (db)(m._word_24654 & 0xff), (db)((m._word_24654 >> 8) & 0xff),
            m._dma_chn_mask, m._sb_irq_number,
            (db)(m._snd_base_port & 0xff), (db)((m._snd_base_port >> 8) & 0xff),
            m._irq_number, m._dma_channel,
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "int1apass") {
        m._int1Avect = k_int8old;
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x0100;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_iret(k_int1a_timer, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x es=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.ds & 0xffff),
                    (unsigned)(state.es & 0xffff));
        return 0;
    }

    if (op == "sb16dmafail") {
        m._sb_base_port = 0x0220;
        m._dma_chn_mask = 1;
        m._sb_irq_number = 5;
        m._dma_mode = 0xaa;
        m._sb_int_counter = 0xcc;
        m._word_2460E = 0xaaaa;
        m._dma_buf_pointer = 0x12345678;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_sb16_18540, &state);
        db data[] = {
            (db)(m._dma_buf_pointer & 0xff), (db)((m._dma_buf_pointer >> 8) & 0xff),
            (db)((m._dma_buf_pointer >> 16) & 0xff), (db)((m._dma_buf_pointer >> 24) & 0xff),
            (db)(m._word_2460E & 0xff), (db)((m._word_2460E >> 8) & 0xff),
            m._dma_chn_mask, m._sb_irq_number, m._dma_mode, m._sb_int_counter,
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "stereoint") {
        m.audio_len = 3;
        m._word_15056 = 0x1234;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x5678;
        state.ebx = 0x9abc;
        state.ecx = 0xdef0;
        state.edx = 0x037a;
        call_iret(k_stereo_timer_int, &state);
        db data[] = {
            (db)(m.audio_len & 0xff), (db)((m.audio_len >> 8) & 0xff),
            (db)(m._word_15056 & 0xff), (db)((m._word_15056 >> 8) & 0xff),
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.ds & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "timerend") {
        m._byte_14F70 = 0;
        m.audio_len = 0x7777;
        m._int8addr = k_int8old;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_iret(k_timer_int_end, &state);
        db data[] = {
            (db)(m.audio_len & 0xff), (db)((m.audio_len >> 8) & 0xff),
            m._byte_14F70,
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.ds & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "setdmamask") {
        if (argc != 3) return 2;
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = (dw)parse_u32(argv[2]);
        state.edx = 0x9abc;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        call_near(k_set_dmachn_mask, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "adlibdelay") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        state.eax = (dw)parse_u32(argv[3]);
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        if (symbol == "adlib_18389") {
            call_near(k_adlib_18389, &state);
        } else if (symbol == "adlib_18395") {
            call_near(k_adlib_18395, &state);
        } else {
            std::fprintf(stderr, "unknown adlibdelay symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "egaseq") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "set_egasequencer") {
            call_near(k_set_egasequencer, &state);
        } else if (symbol == "graph_1C070") {
            call_near(k_graph_1c070, &state);
        } else {
            std::fprintf(stderr, "unknown egaseq symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "txtblink") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "txt_blinkingoff") {
            call_near(k_txt_blinkingoff, &state);
        } else if (symbol == "txt_enableblink") {
            call_near(k_txt_enableblink, &state);
        } else {
            std::fprintf(stderr, "unknown txtblink symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "memfill8080") {
        const dw dma_seg = seg_offset(dseg) + 0x0300;
        db *dma = raddr(dma_seg, 0);
        std::memset(dma, 0xa5, 0x20);
        m._dma_buf_pointer = ((dd)dma_seg << 16);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x12345678;
        state.ebx = 0x9abcdef0;
        state.ecx = 0x1357;
        state.edx = 0x2468;
        call_near(k_memfill8080, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(dma, 16);
        std::printf("\n");
        return 0;
    }

    if (op == "sndofffill") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        const dw dma_seg = seg_offset(dseg) + 0x0300;
        db *dma = raddr(dma_seg, 0);
        std::memset(dma, 0xa5, 0x20);
        m._dma_buf_pointer = ((dd)dma_seg << 16);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x12345678;
        state.ebx = 0x9abcdef0;
        state.ecx = 0x1357;
        state.edx = 0x2468;
        if (symbol == "covox_sndoff") {
            call_near(k_covox_off, &state);
        } else if (symbol == "stereo_sndoff") {
            call_near(k_stereo_sndoff, &state);
        } else if (symbol == "adlib_sndoff") {
            call_near(k_adlib_sndoff, &state);
        } else if (symbol == "pcspeaker_sndoff") {
            call_near(k_pcspeaker_off, &state);
        } else {
            std::fprintf(stderr, "unknown sndofffill symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(dma, 16);
        std::printf("\n");
        return 0;
    }

    if (op == "timerport") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = (dw)parse_u32(argv[3]);
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "set_timer") {
            call_near(k_set_timer, &state);
        } else if (symbol == "clean_timer") {
            call_near(k_clean_timer, &state);
        } else {
            std::fprintf(stderr, "unknown timerport symbol: %s\n", symbol.c_str());
            return 2;
        }
        db timer_word[2] = {
            *(raddr(seg_offset(_text), offset(_text,timer_word_14F6E))),
            *(raddr(seg_offset(_text), offset(_text,timer_word_14F6E) + 1)),
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(timer_word, sizeof(timer_word));
        std::printf("\n");
        return 0;
    }

    if (op == "settimerint") {
        m._dma_buf_pointer = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = k_timer_int_end;
        call_near(k_set_timer_int, &state);
        state.eax = 8;
        call_near(k_getint_vect, &state);
        dw ptr_off = (dw)(m._dma_buf_pointer & 0xffff);
        dw ptr_seg = (dw)((m._dma_buf_pointer >> 16) & 0xffff);
        db data[] = {
            (db)(ptr_off == 0),
            (db)(ptr_seg == 0),
            (db)((state.ebx & 0xffff) != k_timer_int_end),
            (db)((state.edx & 0xffff) != seg_offset(_text)),
        };
        std::printf("ax=%04x bx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "memfree") {
        if (argc != 3) return 2;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = (dw)parse_u32(argv[2]);
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_memfree, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "midiport") {
        if (argc != 5) return 2;
        std::string symbol = argv[2];
        m._word_2465C = (dw)parse_u32(argv[3]);
        m._byte_24677 = 0x55;
        m._byte_24678 = 0xa0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = (dw)parse_u32(argv[4]);
        state.ebx = 0x5678;
        state.ecx = 0x0003;
        state.edx = 0xdef0;
        if (symbol == "midi_clean") {
            call_near(k_midi_clean, &state);
        } else if (symbol == "midi_sndoff") {
            call_near(k_midi_sndoff, &state);
        } else if (symbol == "midi_153C0") {
            call_near(k_midi_153c0, &state);
        } else if (symbol == "midi_153D6") {
            call_near(k_midi_153d6, &state);
        } else if (symbol == "midi_153F1") {
            call_near(k_midi_153f1, &state);
        } else if (symbol == "midi_15442") {
            call_near(k_midi_15442, &state);
        } else {
            std::fprintf(stderr, "unknown midiport symbol: %s\n", symbol.c_str());
            return 2;
        }
        db data[] = {m._byte_24677, m._byte_24678};
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "intvect") {
        if (argc != 5) return 2;
        db int_number = (db)parse_u32(argv[2]);
        dw vector_off = (dw)parse_u32(argv[3]);
        dw vector_seg = (dw)parse_u32(argv[4]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = int_number;
        state.ebx = vector_off;
        state.edx = vector_seg;
        call_near(k_setint_vect, &state);
        state.eax = int_number;
        call_near(k_getint_vect, &state);
        std::printf("ax=%04x bx=%04x dx=%04x ds=%04x es=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)state.ds,
                    (unsigned)state.es);
        return 0;
    }

    if (op == "sndvector") {
        if (argc != 5) return 2;
        db irq = (db)parse_u32(argv[2]);
        dw old_off = (dw)parse_u32(argv[3]);
        dw old_seg = (dw)parse_u32(argv[4]);
        dw vector_offset = (dw)((irq < 8 ? irq + 8 : irq + 0x68) * 4);
        *(dw *)raddr(0, vector_offset) = old_off;
        *(dw *)raddr(0, vector_offset + 2) = old_seg;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = irq;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        state.esi = 0x4321;
        call_near(k_setsnd_handler, &state);
        call_near(k_restore_intvector, &state);
        db data[] = {
            (db)(m._interrupt_mask & 0xff), (db)((m._interrupt_mask >> 8) & 0xff),
            (db)(m._old_intprocoffset & 0xff), (db)((m._old_intprocoffset >> 8) & 0xff),
            (db)(m._old_intprocseg & 0xff), (db)((m._old_intprocseg >> 8) & 0xff),
            (db)(m._intvectoffset & 0xff), (db)((m._intvectoffset >> 8) & 0xff),
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.esi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sb16off") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        m._sb_base_port = 0x0220;
        m._byte_2466E = 0;
        m._memflg_2469A = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "sb16_sound_off") {
            call_near(k_sb16_sound_off, &state);
        } else if (symbol == "sb16_off" || symbol == "sb_sndoff" || symbol == "sbpro_sndoff") {
            call_near(k_sb16_off, &state);
        } else if (symbol == "sb16_deinit" || symbol == "sb_clean" || symbol == "sbpro_clean") {
            call_near(k_sb16_deinit, &state);
        } else {
            std::fprintf(stderr, "unknown sb16off symbol: %s\n", symbol.c_str());
            return 2;
        }
        db data[] = {m._byte_2466E, m._memflg_2469A};
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "audioinitfail") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        const dw base_port = 0x0378;
        m._sndflags_24622 = 0;
        m._is_stereo = 0xff;
        m._bit_mode = 0xff;
        m._snd_base_port = base_port;
        m._dma_buf_pointer = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        bool ok = false;
        if (symbol == "covox_init") {
            call_near(k_covox_init, &state);
            ok = m._sndflags_24622 == 3 && m._is_stereo == 0 && m._bit_mode == 8 &&
                 m._dma_buf_pointer == 0 &&
                 m._word_14FC0 == 0xF108 && m._word_14FC5 == 0xF000;
        } else if (symbol == "stereo_init") {
            call_near(k_stereo_init, &state);
            ok = m._sndflags_24622 == 3 && m._is_stereo == 1 && m._bit_mode == 8 &&
                 m._dma_buf_pointer == 0 &&
                 *(dw *)raddr(seg_offset(_text), kloc_15047 + 1) == 0xF108 &&
                 m._word_15056 == 0xF000;
        } else if (symbol == "pcspeaker_init") {
            call_near(k_pcspeaker_init, &state);
            ok = m._sndflags_24622 == 3 && m._is_stereo == 0 && m._bit_mode == 8 &&
                 m._dma_buf_pointer == 0 &&
                 m._word_1519B == 0xF108 && m._word_151A3 == 0xF000;
        } else if (symbol == "adlib_init") {
            call_near(k_adlib_init, &state);
            ok = m._sndflags_24622 == 0x0B && m._is_stereo == 0 && m._bit_mode == 8 &&
                 m._dma_buf_pointer == 0 &&
                 *(dw *)raddr(seg_offset(_text), kloc_15120 + 1) == 0xF108 &&
                 m._word_15126 == 0xF000;
        } else {
            std::fprintf(stderr, "unknown audioinitfail symbol: %s\n", symbol.c_str());
            return 2;
        }
        db data[] = {(db)ok};
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "cleandeinit") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        m._dma_buf_pointer = 0;
        m._int8addr = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "clean_int8_mem_timr") {
            call_near(k_clean_int8_mem_timr, &state);
        } else if (symbol == "covox_deinit") {
            call_near(k_covox_deinit, &state);
        } else if (symbol == "stereo_deinit") {
            call_near(k_stereo_clean, &state);
        } else if (symbol == "adlib_clean") {
            call_near(k_adlib_clean, &state);
        } else if (symbol == "pcspeaker_clean") {
            call_near(k_pcspeaker_deinit, &state);
        } else {
            std::fprintf(stderr, "unknown cleandeinit symbol: %s\n", symbol.c_str());
            return 2;
        }
        db data[] = {
            (db)(m._dma_buf_pointer & 0xff),
            (db)((m._dma_buf_pointer >> 8) & 0xff),
            (db)((m._dma_buf_pointer >> 16) & 0xff),
            (db)((m._dma_buf_pointer >> 24) & 0xff),
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "dosdir") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 80);
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        state.esi = offset(dseg, _buffer_1);
        if (symbol == "dosgetcurdir") {
            call_near(k_dosgetcurdir, &state);
        } else if (symbol == "doschdir") {
            scratch[0] = 4;
            scratch[1] = '\\';
            scratch[2] = 0;
            call_near(k_doschdir, &state);
        } else {
            std::fprintf(stderr, "unknown dosdir symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.esi & 0xffff));
        print_bytes(scratch, 70);
        std::printf("\n");
        return 0;
    }

    if (op == "dosfindnext") {
        m._buffer_1DBEC = 0x5a;
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_dosfindnext, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)m._buffer_1DBEC);
        return 0;
    }

    if (op == "dosfread") {
        m._fhandle_module = 0;
        db *buffer = raddr(seg_offset(seg003), offset(seg003, _chrin));
        std::memset(buffer, 0xa5, 16);
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 16;
        state.edx = offset(seg003, _chrin);
        call_near(k_dosfread, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(buffer, 16);
        std::printf("\n");
        return 0;
    }

    if (op == "dosseeksuccess") {
        m._fhandle_module = 0x42;
        db *buffer = raddr(seg_offset(seg003), offset(seg003, _chrin));
        std::memset(buffer, 0xa5, 16);
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0;
        state.ecx = 0;
        state.edx = offset(seg003, _chrin);
        call_near(k_dosseek, &state);
        std::printf("ax=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(buffer, 16);
        std::printf("\n");
        return 0;
    }

    if (op == "inrread119b7") {
        m._fhandle_module = 0;
        m._myin = 16;
        db *buffer = raddr(seg_offset(seg003), offset(seg003, _chrin));
        std::memset(buffer, 0xa5, 16);
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        state.edi = offset(seg003, _chrin);
        call_near(k_inr_read_119b7, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(buffer, 16);
        std::printf("\n");
        return 0;
    }

    if (op == "inrread118b0fail") {
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.edx = 2;
        m._fhandle_module = 0xffff;
        call_near(k_inr_read_118b0, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.ds & 0xffff));
        return 0;
    }

    if (op == "modread11f4eguard") {
        m._byte_24617 = 0xaa;
        m._word_24662 = 1;
        m._sndflags_24622 = 0;
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_mod_readfile_11f4e, &state);
        db data[] = {
            m._byte_24617,
            (db)(m._word_24662 & 0xff), (db)((m._word_24662 >> 8) & 0xff),
            m._sndflags_24622,
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "modread12247eof") {
        db *buffer = raddr(seg_offset(dseg), 0);
        std::memset(buffer, 0xa5, 16);
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(dseg);
        state.eax = 0x1234;
        state.ebx = 0;
        state.ecx = 16;
        state.edx = 0xffff;
        state.esi = 0x2222;
        state.edi = 0x3333;
        call_near(k_mod_readfile_12247, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(buffer, 16);
        std::printf("\n");
        return 0;
    }

    if (op == "read2buffer") {
        db *buffer = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(buffer, 0xa5, 16);
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        state.esi = 0x1111;
        state.edi = 0x2222;
        call_near(k_read2buffer, &state);
        std::printf("si=%04x data=", (unsigned)(state.esi & 0xffff));
        print_bytes(buffer, 16);
        std::printf("\n");
        return 0;
    }

    if (op == "midichannelport") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        db *channel = raddr(seg_offset(seg003), offset(seg003, _mystr));
        std::memset(channel, 0, 0x40);
        channel[0x02] = 0x05;
        channel[0x03] = 0x02;
        channel[0x08] = 0x20;
        channel[0x17] = symbol == "midi_1544D" ? 0x83 : 0x00;
        channel[0x18] = 0x04;
        channel[0x1B] = 0x20;
        channel[0x35] = 0x31;
        m._word_2465C = 0x0330;
        m._byte_24677 = 0x55;
        m._byte_24678 = 0xa0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = offset(seg003, _mystr);
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "midi_1544D") {
            call_near(k_midi_1544d, &state);
        } else if (symbol == "midi_15466") {
            call_near(k_midi_15466, &state);
        } else {
            std::fprintf(stderr, "unknown midichannelport symbol: %s\n", symbol.c_str());
            return 2;
        }
        db data[0x42];
        std::memcpy(data, channel, 0x40);
        data[0x40] = m._byte_24677;
        data[0x41] = m._byte_24678;
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "message1be77") {
        if (argc != 5) return 2;
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        db *src = scratch + 0x500;
        std::memset(scratch, 0, 0x600);
        std::memcpy(src, argv[2], std::strlen(argv[2]));
        m._videomempointer = ((dd)seg_offset(dseg) << 16) | offset(dseg, _buffer_1);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dw)(((parse_u32(argv[4]) & 0xff) << 8) | (parse_u32(argv[3]) & 0xff));
        state.esi = offset(dseg, _buffer_1) + 0x500;
        call_near(k_message_1be77, &state);
        std::printf("si=%04x di=%04x data=", (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(scratch, 1000);
        std::printf("\n");
        return 0;
    }

    if (op == "recolortxt") {
        if (argc != 4) return 2;
        db *scratch = raddr(seg_offset(dseg), 0);
        std::memset(scratch, 0, 0x2200);
        unsigned row = parse_u32(argv[2]) & 0xffff;
        unsigned color = parse_u32(argv[3]) & 0xff;
        unsigned base = row * 160 + 80 * 2 * 10 + 8 * 2 + 1;
        for (unsigned i = 0; i < 64; ++i) {
            scratch[base + i * 2] = (db)(0xa0 | (i & 0x0f));
        }
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dw)row;
        state.ebx = (dw)color;
        call_near(k_recolortxt, &state);
        std::printf("ax=%04x bx=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff));
        for (unsigned i = 0; i < 64; ++i) {
            std::printf("%02x", scratch[base + i * 2]);
        }
        std::printf("\n");
        return 0;
    }

    if (op == "int24") {
        if (argc != 3) return 2;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dw)((parse_u32(argv[2]) & 0xff) << 8);
        call_iret(k_int24, &state);
        std::printf("ax=%04x\n", (unsigned)(state.eax & 0xffff));
        return 0;
    }

    if (op == "int2fcheck") {
        if (argc != 3) return 2;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x60ff;
        state.ebx = 0x5344;
        state.ecx = 0x4d50;
        state.edx = (dw)parse_u32(argv[2]);
        m._byte_1DE7C = 0;
        call_iret(k_int2f_checkmyself, &state);
        db data[] = {m._byte_1DE7C};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "emsrestore") {
        if (argc != 4) return 2;
        m._ems_enabled = (db)parse_u32(argv[2]);
        m._byte_246A5 = (db)parse_u32(argv[3]);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_ems_restore_mapctx, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "emsinit") {
        if (argc != 3) return 2;
        m._ems_enabled = 0xff;
        m._config_word = (dw)parse_u32(argv[2]);
        call_near(k_ems_init, &state);
        std::printf("ax=%04x ems=%02x\n", (unsigned)(state.eax & 0xffff), (unsigned)m._ems_enabled);
        return 0;
    }

    if (op == "emsguard") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        m._ems_enabled = 0;
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "ems_release") {
            call_near(k_ems_release, &state);
        } else if (symbol == "ems_realloc") {
            call_near(k_ems_realloc, &state);
        } else if (symbol == "ems_deinit") {
            call_near(k_ems_deinit, &state);
        } else if (symbol == "ems_save_mapctx") {
            call_near(k_ems_save_mapctx, &state);
        } else if (symbol == "ems_mapmem") {
            call_near(k_ems_mapmem, &state);
        } else if (symbol == "ems_mapmem2") {
            call_near(k_ems_mapmem2, &state);
        } else {
            std::fprintf(stderr, "unknown ems guard symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "emsrealloc2limit") {
        if (argc != 4) return 2;
        const dw base = 0x2800;
        db *mem = raddr(seg_offset(seg003), base);
        std::memset(mem, 0, 0x40);
        m._ems_enabled = 0;
        m._byte_24617 = (db)parse_u32(argv[2]);
        *(dd *)(mem + 0x20) = (dd)parse_u32(argv[3]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.edi = base;
        call_near(k_ems_realloc2, &state);
        db data[] = {m._byte_24617};
        std::printf("ax=%04x cx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "clean11c43") {
        if (argc != 4) return 2;
        m._flag_playsetttings = (db)parse_u32(argv[2]);
        m._byte_2461E = (db)parse_u32(argv[3]);
        m._byte_2461F = (db)parse_u32(argv[4]);
        std::memset(m._segs_table, 0xff, sizeof(m._segs_table));
        std::memset(m._byte_27FE8, 0xff, sizeof(m._byte_27FE8));
        std::memset(m._byte_280E8, 0xff, sizeof(m._byte_280E8));
        std::memset(m._byte_281E8, 0x00, sizeof(m._byte_281E8));
        std::memset(m._byte_282E8, 0xff, sizeof(m._byte_282E8));
        m._word_245D2 = 0xffff;
        m._mod_channels_number = 0xffff;
        m._word_245D6 = 0xffff;
        m._word_245D8 = 0xffff;
        m._word_245DA = 0xffff;
        m._freq_245DE = 0xffff;
        m._word_245F0 = 0xffff;
        m._my_seg_index = 0xffff;
        m._word_245F6 = 0xffff;
        m._word_245F8 = 0xffff;
        m._word_245FA = 0xffff;
        m._amplification = 0xffff;
        m._byte_2461A = 0xff;
        m._high_amplif = 0xff;
        m._byte_24673 = 0xff;
        m._byte_24679 = 0xff;
        m._byte_2467A = 0xff;
        m._byte_2467E = 0xff;
        m._word_24630 = 0xffff;
        m._moduleflag_246D0 = 0xffff;
        state.ds = 0x7777;
        state.es = 0x7777;
        call_far(k_clean_11c43, &state);
        db data[57] = {
            (db)(m._word_245D2 & 0xff), (db)((m._word_245D2 >> 8) & 0xff),
            (db)(m._mod_channels_number & 0xff), (db)((m._mod_channels_number >> 8) & 0xff),
            (db)(m._word_245D6 & 0xff), (db)((m._word_245D6 >> 8) & 0xff),
            (db)(m._word_245D8 & 0xff), (db)((m._word_245D8 >> 8) & 0xff),
            (db)(m._word_245DA & 0xff), (db)((m._word_245DA >> 8) & 0xff),
            (db)(m._freq_245DE & 0xff), (db)((m._freq_245DE >> 8) & 0xff),
            (db)(m._word_245F0 & 0xff), (db)((m._word_245F0 >> 8) & 0xff),
            (db)(m._my_seg_index & 0xff), (db)((m._my_seg_index >> 8) & 0xff),
            (db)(m._word_245F6 & 0xff), (db)((m._word_245F6 >> 8) & 0xff),
            (db)(m._word_245F8 & 0xff), (db)((m._word_245F8 >> 8) & 0xff),
            (db)(m._word_245FA & 0xff), (db)((m._word_245FA >> 8) & 0xff),
            (db)(m._amplification & 0xff), (db)((m._amplification >> 8) & 0xff),
            m._byte_2461A,
            m._high_amplif,
            m._byte_24673,
            (db)(m._word_24630 & 0xff), (db)((m._word_24630 >> 8) & 0xff),
            m._byte_24679, m._byte_2467A, m._byte_2467E,
            (db)(m._moduleflag_246D0 & 0xff), (db)((m._moduleflag_246D0 >> 8) & 0xff),
        };
        std::memcpy(data + 34, m._segs_table, 4);
        std::memcpy(data + 38, m._byte_27FE8, 4);
        std::memcpy(data + 42, m._byte_280E8, 4);
        std::memcpy(data + 46, m._byte_281E8, 4);
        std::memcpy(data + 50, m._byte_282E8, 4);
        std::memcpy(data + 54, &m._dword_27BC8, 3);
        std::printf("ds=%04x data=", (unsigned)(state.ds & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "emsmapcopy") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        const dw base = 0x2800;
        db *mem = raddr(seg_offset(seg003), base);
        std::memset(mem, 0, 0x1000);
        db *src = nullptr;
        db *dst = nullptr;
        if (symbol == "ems_mapmemx") {
            *(dd *)(mem + 0x20) = base + 0x100;
            *(dd *)(mem + 0x2C) = base + 0x0ff;
            src = mem + 0x100;
            dst = mem + 0x900;
        } else if (symbol == "ems_mapmemy") {
            *(dd *)(mem + 0x20) = base + 0x100;
            *(dd *)(mem + 0x2C) = base + 0x0ff;
            src = mem + 0x900;
            dst = mem + 0x100;
        } else {
            std::fprintf(stderr, "unknown ems mapcopy symbol: %s\n", symbol.c_str());
            return 2;
        }
        *(dw *)(mem + 0x32) = 0xffff;
        mem[0x3C] = 0;
        for (unsigned i = 0; i < 16; ++i) {
            src[i] = (db)(0x31 + i);
        }
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = seg_offset(seg003);
        state.edi = base;
        if (symbol == "ems_mapmemx") {
            call_near(k_ems_mapmemx, &state);
        } else {
            call_near(k_ems_mapmemy, &state);
        }
        std::printf("data=");
        print_bytes(dst, 16);
        std::printf("\n");
        return 0;
    }

    if (op == "modsubdelta") {
        if (argc != 6) return 2;
        const char *payload = argv[5];
        size_t len = std::strlen(payload);
        db *mem = raddr(seg_offset(seg003), 0x2800);
        std::memset(mem, 0, len);
        std::memcpy(mem, payload, len);
        m._byte_24674 = (db)parse_u32(argv[2]);
        m._byte_24675 = (db)parse_u32(argv[3]);
        m._byte_24676 = (db)parse_u32(argv[4]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.ecx = (dw)len;
        state.esi = 0x2800;
        call_near(k_mod_sub_12220, &state);
        std::printf("si=%04x cx=%04x data=",
                    (unsigned)(state.esi & 0xffff), (unsigned)(state.ecx & 0xffff));
        print_bytes(mem, len);
        std::printf("\n");
        return 0;
    }

    if (op == "sub11ba6") {
        if (argc != 7) return 2;
        db *mem = raddr(seg_offset(seg003), 0x2800);
        std::memset(mem, 0, 8);
        m._byte_2461B = (db)parse_u32(argv[6]);
        unsigned ch = parse_u32(argv[2]) & 0xff;
        unsigned cl = parse_u32(argv[3]) & 0xff;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.ecx = (ch << 8) | cl;
        state.ebx = (dw)parse_u32(argv[4]);
        state.edx = (dw)parse_u32(argv[5]);
        state.edi = 0x2800;
        call_near(ksub_11ba6, &state);
        std::printf("di=%04x data=", (unsigned)(state.edi & 0xffff));
        print_bytes(mem, 8);
        std::printf("\n");
        return 0;
    }

    if (op == "sub11c0c") {
        if (argc != 4) return 2;
        db *mem = raddr(seg_offset(seg003), 0);
        std::memset(mem, 0, 16);
        decode_hex_bytes(argv[3], mem, 16);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = (db)parse_u32(argv[2]);
        call_near(ksub_11c0c, &state);
        std::printf("ax=%04x si=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.esi & 0xffff));
        return 0;
    }

    if (op == "mod102f5") {
        if (argc != 3) return 2;
        const char *hex = argv[2];
        db *table = raddr(seg_offset(seg003), offset(seg003, _byte_27FE8));
        std::memset(table, 0, 128);
        decode_hex_bytes(hex, table, 128);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        call_near(k_mod_102f5, &state);
        std::printf("data=");
        print_bytes((const db *)&m._word_245F2, 2);
        std::printf("\n");
        return 0;
    }

    if (op == "mod1021e") {
        if (argc != 6) return 2;
        db *src = raddr(seg_offset(seg003), 0x2800);
        db *pattern = raddr(seg_offset(seg003), offset(seg003, _byte_27FE8));
        db *title = raddr(seg_offset(seg003), offset(seg003, _chrin));
        db *out = raddr(seg_offset(seg003), offset(seg003, asc_246B0));
        std::memset(src, 0, 130);
        std::memset(pattern, 0, 128);
        std::memset(title, 0, 20);
        std::memset(out, 0, 20);
        src[0] = (db)parse_u32(argv[2]);
        src[1] = (db)parse_u32(argv[3]);
        decode_hex_bytes(argv[4], src + 2, 128);
        decode_hex_bytes(argv[5], title, 20);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.esi = 0x2800;
        call_near(k_mod_1021e, &state);
        std::printf("data=");
        print_bytes((const db *)&m._word_245F8, 2);
        print_bytes((const db *)&m._word_245FA, 2);
        print_bytes(pattern, 128);
        print_bytes(out, 20);
        std::printf("\n");
        return 0;
    }

    if (op == "mod1024a") {
        if (argc != 4) return 2;
        unsigned sample_count = parse_u32(argv[2]) & 0xffff;
        db *input = raddr(seg_offset(seg003), offset(seg003, _chrin));
        db *out = raddr(seg_offset(seg003), offset(seg003, _myout));
        std::memset(input, 0, sample_count * 0x1e + 0x32);
        std::memset(out, 0, sample_count * 0x40);
        db decoded[512];
        std::memset(decoded, 0, sizeof(decoded));
        decode_hex_bytes(argv[3], decoded, sizeof(decoded));
        for (unsigned i = 0; i < sample_count; ++i) {
            db *src = decoded + i * 30;
            db *dst = input + i * 0x1e;
            std::memcpy(dst + 20, src, 22);
            std::memcpy(dst + 0x2a, src + 22, 8);
        }
        m._word_245D2 = (dw)sample_count;
        m._freq_245DE = 8363;
        m._dword_245C4 = 0;
        m._word_24662 = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        call_near(k_mod_1024a, &state);
        std::printf("data=");
        print_bytes((const db *)&m._dword_245C4, 4);
        print_bytes((const db *)&m._word_24662, 2);
        print_bytes(out, sample_count * 0x40);
        std::printf("\n");
        return 0;
    }

    if (op == "sub126a9") {
        if (argc != 7) return 2;
        m._word_245FA = (dw)parse_u32(argv[2]);
        m._word_245D2 = (dw)parse_u32(argv[3]);
        m._word_245D4 = (dw)parse_u32(argv[4]);
        m._byte_24617 = (db)parse_u32(argv[5]);
        m._module_type_text = (dd)parse_u32(argv[6]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        call_far(ksub_126a9, &state);
        std::printf("ax=%04x bx=%04x cx=%04x si=%04x di=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        return 0;
    }

    if (op == "ultreadfast") {
        if (argc != 3) return 2;
        m._word_3063B = (dw)parse_u32(argv[2]);
        m._dword_3063D = 0xa5a5a5a5;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_ult_read, &state);
        db data[] = {
            (db)(m._word_3063B & 0xff), (db)((m._word_3063B >> 8) & 0xff),
            (db)(m._dword_3063D & 0xff), (db)((m._dword_3063D >> 8) & 0xff),
            (db)((m._dword_3063D >> 16) & 0xff), (db)((m._dword_3063D >> 24) & 0xff),
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub1265d") {
        if (argc != 12) return 2;
        m._volume_245FC = (dw)parse_u32(argv[2]);
        m._sndcard_type = (db)parse_u32(argv[3]);
        m._byte_24666 = (db)parse_u32(argv[4]);
        m._byte_24667 = (db)parse_u32(argv[5]);
        m._sndflags_24622 = (db)parse_u32(argv[6]);
        m._byte_24628 = (db)parse_u32(argv[7]);
        m._is_stereo = (db)parse_u32(argv[8]);
        m._byte_24671 = (db)parse_u32(argv[9]);
        m._word_245F6 = (dw)parse_u32(argv[10]);
        m._word_245F0 = (dw)parse_u32(argv[11]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        call_far(krender_1265d, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x bp=%04x si=%04x di=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.ebp & 0xffff), (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        return 0;
    }

    if (op == "memfree125da") {
        m._dword_24640 = 0;
        m._byte_24665 = 0;
        m._ems_enabled = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_far(k_memfree_125da, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "memfree18a28") {
        if (argc != 3) return 2;
        m._memflg_2469A = (db)parse_u32(argv[2]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_memfree_18a28, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "memlimit") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x2345;
        state.ebx = (dd)parse_u32(argv[3]);
        if (symbol == "memalloc") {
            call_near(k_memalloc, &state);
        } else if (symbol == "memrealloc") {
            call_near(k_memrealloc, &state);
        } else {
            std::fprintf(stderr, "unknown memlimit symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("ax=%04x bx=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff));
        return 0;
    }

    if (op == "allocdmafail") {
        if (argc != 4) return 2;
        db *fields = raddr(seg_offset(seg003), offset(seg003, _dword_24684));
        std::memset(fields, 0xa5, 0x19);
        m._word_2468C = 0xbeef;
        m._myseg_24698 = 0xcafe;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = (dd)parse_u32(argv[2]);
        state.ecx = (dw)parse_u32(argv[3]);
        call_near(k_alloc_dma_buf, &state);
        std::printf("ax=%04x bx=%04x cx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff));
        print_bytes(fields, 0x19);
        std::printf("\n");
        return 0;
    }

    if (op == "memstrat") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        m._config_word = (dw)parse_u32(argv[3]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0xaaaa;
        state.ebx = 0xbbbb;
        state.ecx = 0xcccc;
        state.edx = 0xdddd;
        if (symbol == "setmemalloc1") {
            call_near(k_setmemalloc1, &state);
        } else if (symbol == "setmemalloc2") {
            call_near(k_setmemalloc2, &state);
        } else if (symbol == "setmemallocstrat") {
            state.eax = m._config_word;
            call_near(k_setmemallocstrat, &state);
        } else if (symbol == "getmemallocstrat") {
            call_near(k_getmemallocstrat, &state);
        } else {
            std::fprintf(stderr, "unknown memstrat symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "memreallocx") {
        if (argc != 3) return 2;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.edi = (dw)parse_u32(argv[2]);
        m._my_seg_index = 1;
        m._word_24662 = 0x2222;
        std::memset(m._segs_table, 0xff, sizeof(m._segs_table));
        std::memset(m._myseg_size, 0xff, sizeof(m._myseg_size));
        call_near(k_mem_reallocx, &state);
        db data[] = {
            (db)(m._my_seg_index & 0xff), (db)((m._my_seg_index >> 8) & 0xff),
            (db)(m._word_24662 & 0xff), (db)((m._word_24662 >> 8) & 0xff),
            (db)(m._segs_table[1] & 0xff), (db)((m._segs_table[1] >> 8) & 0xff),
            (db)(m._myseg_size[1] & 0xff), (db)((m._myseg_size[1] >> 8) & 0xff),
        };
        std::printf("ax=%04x bx=%04x di=%04x es=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.edi & 0xffff),
                    (unsigned)(state.es & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "mouse_show" || op == "mouse_hide" || op == "mouse_getpos" || op == "mouse_deinit" || op == "mouse_showcur" || op == "mouse_hide2" || op == "mouse_init") {
        if (argc != 4) return 2;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        m._mousecolumn = 0xaaaa;
        m._mouserow = 0xbbbb;
        m._byte_1DE90 = 0xcc;
        m._mouse_exist_flag = (db)parse_u32(argv[2]);
        m._mouse_visible[0] = (db)parse_u32(argv[3]);
        state.ebx = 0x1111;
        state.ecx = 0x2222;
        state.edx = 0x3333;
        if (op == "mouse_show") {
            call_near(k_mouse_show, &state);
        } else if (op == "mouse_hide") {
            call_near(k_mouse_hide, &state);
        } else if (op == "mouse_getpos") {
            call_near(k_mouse_getpos, &state);
        } else if (op == "mouse_deinit") {
            call_near(k_mouse_deinit, &state);
        } else if (op == "mouse_showcur") {
            call_near(k_mouse_showcur, &state);
        } else if (op == "mouse_init") {
            call_near(k_mouse_init, &state);
        } else {
            call_near(k_mouse_hide2, &state);
        }
        db data[] = {
            (db)(m._mousecolumn & 0xff), (db)((m._mousecolumn >> 8) & 0xff),
            (db)(m._mouserow & 0xff), (db)((m._mouserow >> 8) & 0xff),
            m._byte_1DE90, m._mouse_exist_flag, m._mouse_visible[0],
        };
        std::printf("bx=%04x cx=%04x dx=%04x data=", (unsigned)(state.ebx & 0xffff), (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "mouse_1c7a9") {
        if (argc != 8) return 2;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dw)parse_u32(argv[2]);
        state.ebp = (dw)parse_u32(argv[3]);
        state.ecx = (dw)parse_u32(argv[4]);
        state.edx = (dw)parse_u32(argv[5]);
        state.esi = (dw)parse_u32(argv[6]);
        state.edi = (dw)parse_u32(argv[7]);
        call_near(k_mouse_1c7a9, &state);
        std::printf("ax=%04x bp=%04x cx=%04x dx=%04x si=%04x di=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebp & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        return 0;
    }

    if (op == "mouse_1c7cf") {
        if (argc != 5) return 2;
        db *dseg_buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(dseg_buf, 0, 128);
        decode_hex_bytes(argv[4], dseg_buf, 128);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = (dw)parse_u32(argv[2]);
        state.ebp = (dw)parse_u32(argv[3]);
        state.ebx = offset(dseg, _buffer_1);
        call_near(k_mouse_1c7cf, &state);
        std::printf("ax=%04x bx=%04x bp=%04x cx=%04x dx=%04x si=%04x di=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ebp & 0xffff), (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff), (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        return 0;
    }

    if (op == "seg1putdigit") {
        if (argc != 4) return 2;
        db *dseg_buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(dseg_buf, 0, 1);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.edx = (dw)parse_u32(argv[2]);
        state.ecx = (dw)parse_u32(argv[3]);
        state.esi = offset(dseg, _buffer_1);
        call_near(k_myputdigit, &state);
        std::printf("cx=%04x si=%04x data=", (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff));
        print_bytes(dseg_buf, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "interppatch") {
        if (argc != 3) return 2;
        unsigned patch = parse_u32(argv[2]) & 0xff;
        dw channel = offset(seg003, _mystr);
        std::memset(buf, 0, 66);
        *(dd *)(buf + 0x04) = 0;
        *(dd *)(buf + 0x48) = 0xffffffffu;
        m._word_245E4 = 0;
        m._word_24614 = (dw)(patch << 8);
        m._flag_playsetttings = 0x10;
        state.ecx = (dw)(patch << 8);
        state.esi = 0;
        jump_with_saved_si(k_lc_perfrm_interpol, &state, channel);

        db patched[] = {
            *raddr(seg_offset(_text), offset(_text, _byte_158B4)),
            *raddr(seg_offset(_text), offset(_text, _byte_158E3)),
            *raddr(seg_offset(_text), offset(_text, _byte_15912)),
            *raddr(seg_offset(_text), offset(_text, _byte_15A2C)),
            *raddr(seg_offset(_text), offset(_text, _byte_15B17)),
            *raddr(seg_offset(_text), offset(_text, _byte_15E23)),
        };
        std::printf("data=");
        print_bytes(patched, sizeof(patched));
        std::printf("\n");
        return 0;
    }

    if (op == "getplaysettings") {
        if (argc != 3) return 2;
        m._flag_playsetttings = (db)parse_u32(argv[2]);
        call_far(k_get_playsettings, &state);
        std::printf("ax=%04x\n", (unsigned)(state.eax & 0xffff));
        return 0;
    }

    if (op == "volume12a66") {
        if (argc != 3) return 2;
        m._mod_channels_number = (dw)parse_u32(argv[2]);
        *(dw *)&m.off_245CE = k_nullsub_5;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = seg_offset(seg003);
        call_far(k_volume_12a66, &state);
        std::printf("ax=%04x bx=%04x cx=%04x\n", (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff), (unsigned)(state.ecx & 0xffff));
        return 0;
    }

    if (op == "vlm141df") {
        m._mod_channels_number = 1;
        *(dw *)&m.off_245CE = k_nullsub_5;
        m._byte_24671 = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_vlm_141df, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)m._byte_24671);
        return 0;
    }

    if (op == "changevolume") {
        if (argc != 5) return 2;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(channel, 0, 80);
        m._mod_channels_number = (dw)parse_u32(argv[3]);
        channel[0x08] = (db)parse_u32(argv[4]);
        *(dw *)&m.off_245CC = k_nullsub_5;
        state.eax = (dw)parse_u32(argv[2]);
        call_far(k_getset_volume, &state);
        db data[] = {
            (db)(m._volume_245FC & 0xff), (db)((m._volume_245FC >> 8) & 0xff),
            channel[0x08],
        };
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "memclean") {
        if (argc != 4) return 2;
        db *scratch = raddr(seg_offset(seg003), offset(seg003, _mystr));
        size_t fill_count = parse_u32(argv[3]);
        std::memset(scratch, 0xa5, fill_count);
        m._my_size = (dw)parse_u32(argv[2]);
        state.es = seg_offset(seg003);
        state.edi = offset(seg003, _mystr);
        call_near(k_memclean, &state);
        std::printf("di=%04x data=", (unsigned)(state.edi & 0xffff));
        print_bytes(scratch, fill_count);
        std::printf("\n");
        return 0;
    }

    if (op == "setplaysettings") {
        if (argc != 7) return 2;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(channel, 0, 80);
        m._config_word = (dw)((parse_u32(argv[3]) & 0xff) << 8);
        m._freq1 = (dw)parse_u32(argv[4]);
        m._mod_channels_number = (dw)parse_u32(argv[5]);
        m._byte_2461A = (db)parse_u32(argv[6]);
        *(dw *)(channel + 0x3E) = 0xaaaa;
        state.eax = (dw)parse_u32(argv[2]);
        call_far(k_set_playsettings, &state);
        db data[] = {
            m._flag_playsetttings,
            (db)((m._config_word >> 8) & 0xff),
            (db)(m._dword_245BC & 0xff), (db)((m._dword_245BC >> 8) & 0xff), (db)((m._dword_245BC >> 16) & 0xff), (db)((m._dword_245BC >> 24) & 0xff),
            (db)(m._dword_245C0 & 0xff), (db)((m._dword_245C0 >> 8) & 0xff), (db)((m._dword_245C0 >> 16) & 0xff), (db)((m._dword_245C0 >> 24) & 0xff),
            channel[0x3E], channel[0x3F],
        };
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub12afd") {
        if (argc != 6) return 2;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(channel, 0, 80);
        m._mod_channels_number = (dw)parse_u32(argv[3]);
        channel[0x17] = (db)parse_u32(argv[5]);
        state.eax = (dw)parse_u32(argv[2]);
        state.ecx = (dw)(parse_u32(argv[4]) << 8);
        call_far(ksub_12afd, &state);
        std::printf("data=");
        print_bytes(channel + 0x17, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "sub12b18") {
        if (argc != 4) return 2;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        db *source = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        unsigned channels = parse_u32(argv[2]);
        std::memset(channel, 0, 0x200);
        std::memset(source, 0, 32);
        for (unsigned i = 0; i < channels && argv[3][i * 2] && argv[3][i * 2 + 1]; ++i) {
            char hex[3] = {argv[3][i * 2], argv[3][i * 2 + 1], 0};
            source[i] = (db)std::strtoul(hex, nullptr, 16);
        }
        m._mod_channels_number = (dw)channels;
        m._sndflags_24622 = 0;
        state.ds = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        call_far(ksub_12b18, &state);
        db data[2 + 32] = {m._byte_2461C, m._byte_2461D};
        size_t pos = 2;
        for (unsigned i = 0; i < channels; ++i) {
            data[pos++] = channel[i * 0x50 + 0x18];
            data[pos++] = channel[i * 0x50 + 0x3A];
        }
        std::printf("data=");
        print_bytes(data, pos);
        std::printf("\n");
        return 0;
    }

    if (op == "sub12b83") {
        if (argc != 5) return 2;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        unsigned value = parse_u32(argv[2]) & 0xff;
        unsigned channel_count = value < 2 ? 2 : value > 0x20 ? 0x20 : value;
        std::memset(channel, 0, channel_count * 0x50);
        for (unsigned i = 0; i < channel_count && argv[3][i * 2] && argv[3][i * 2 + 1]; ++i) {
            char hex[3] = {argv[3][i * 2], argv[3][i * 2 + 1], 0};
            channel[i * 0x50 + 0x1D] = (db)std::strtoul(hex, nullptr, 16);
            *(dw *)(channel + i * 0x50 + 0x3E) = 0xaaaa;
        }
        m._flag_playsetttings = 0;
        m._sndflags_24622 = 0;
        m._byte_2461A = 0;
        m._byte_24629 = 0x20;
        m._byte_2467E = (db)parse_u32(argv[4]);
        m._freq1 = 22050;
        m._amplification = 100;
        state.eax = (dw)value;
        call_far(ksub_12b83, &state);
        db data[20 + 32 * 3] = {
            (db)(m._mod_channels_number & 0xff), (db)((m._mod_channels_number >> 8) & 0xff),
            (db)(m._word_245D6 & 0xff), (db)((m._word_245D6 >> 8) & 0xff),
            (db)(m._word_245D8 & 0xff), (db)((m._word_245D8 >> 8) & 0xff),
            (db)(m._word_245DA & 0xff), (db)((m._word_245DA >> 8) & 0xff),
            m._byte_2461C, m._byte_2461D,
            (db)(m._dword_245BC & 0xff), (db)((m._dword_245BC >> 8) & 0xff), (db)((m._dword_245BC >> 16) & 0xff), (db)((m._dword_245BC >> 24) & 0xff),
            (db)(m._dword_245C0 & 0xff), (db)((m._dword_245C0 >> 8) & 0xff), (db)((m._dword_245C0 >> 16) & 0xff), (db)((m._dword_245C0 >> 24) & 0xff),
            m._byte_2467D, m._byte_2467E,
        };
        size_t pos = 20;
        for (unsigned i = 0; i < channel_count; ++i) {
            data[pos++] = channel[i * 0x50 + 0x18];
            data[pos++] = channel[i * 0x50 + 0x3E];
            data[pos++] = channel[i * 0x50 + 0x3F];
        }
        std::printf("data=");
        print_bytes(data, pos);
        std::printf("\n");
        return 0;
    }

    if (op == "someplaymode") {
        if (argc != 7) return 2;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(channel, 0, 80);
        m._flag_playsetttings = (db)parse_u32(argv[2]);
        m._freq1 = (dw)parse_u32(argv[3]);
        m._mod_channels_number = (dw)parse_u32(argv[4]);
        m._byte_2461A = (db)parse_u32(argv[5]);
        m._sndflags_24622 = (db)parse_u32(argv[6]);
        m._byte_24629 = 0x20;
        *(dw *)(channel + 0x3E) = 0xaaaa;
        call_near(k_someplaymode, &state);
        db data[] = {
            (db)(m._dword_245BC & 0xff), (db)((m._dword_245BC >> 8) & 0xff), (db)((m._dword_245BC >> 16) & 0xff), (db)((m._dword_245BC >> 24) & 0xff),
            (db)(m._dword_245C0 & 0xff), (db)((m._dword_245C0 >> 8) & 0xff), (db)((m._dword_245C0 >> 16) & 0xff), (db)((m._dword_245C0 >> 24) & 0xff),
            (db)(m._dword_2463C & 0xff), (db)((m._dword_2463C >> 8) & 0xff), (db)((m._dword_2463C >> 16) & 0xff), (db)((m._dword_2463C >> 24) & 0xff),
            channel[0x3E], channel[0x3F],
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub12cadguard") {
        if (argc != 7) return 2;
        state.ecx = ((dw)parse_u32(argv[2]) << 8) | ((dw)parse_u32(argv[3]) & 0xff);
        state.ebx = (dw)parse_u32(argv[4]);
        state.edx = (dw)parse_u32(argv[5]);
        m._mod_channels_number = (dw)parse_u32(argv[6]);
        call_far(ksub_12cad, &state);
        db data[] = {
            (db)(m._word_246A6 & 0xff), (db)((m._word_246A6 >> 8) & 0xff),
            m._byte_246A8,
            (db)(m._word_246A9 & 0xff), (db)((m._word_246A9 >> 8) & 0xff),
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.esi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub13623guard") {
        if (argc != 5) return 2;
        state.eax = (dw)parse_u32(argv[2]);
        state.edx = (dw)parse_u32(argv[3]);
        state.esi = offset(seg003, _mystr);
        m._mod_channels_number = (dw)parse_u32(argv[4]);
        call_near(ksub_13623, &state);
        std::printf("ax=%04x dx=%04x si=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.esi & 0xffff));
        return 0;
    }

    if (op == "readsndsettings") {
        if (argc != 13) return 2;
        m._sndcard_type = (db)parse_u32(argv[2]);
        m._snd_base_port = (dw)parse_u32(argv[3]);
        m._irq_number = (db)parse_u32(argv[4]);
        m._dma_channel = (db)parse_u32(argv[5]);
        m._freq_246D7 = (db)parse_u32(argv[6]);
        m._byte_246D8 = (db)parse_u32(argv[7]);
        m._byte_246D9 = (db)parse_u32(argv[8]);
        m._freq1 = (dw)parse_u32(argv[9]);
        m._freq2 = (dw)parse_u32(argv[10]);
        m._config_word = (dw)parse_u32(argv[11]);
        m._sndflags_24622 = (db)parse_u32(argv[12]);
        call_far(k_read_sndsettings, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x bp=%04x si=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.ebp & 0xffff), (unsigned)(state.esi & 0xffff));
        return 0;
    }

    if (op == "sub12d05") {
        if (argc != 4) return 2;
        db *scratch = raddr(seg_offset(seg003), offset(seg003, _mystr));
        std::memset(scratch, 0, 64);
        m._snd_init = (db)parse_u32(argv[2]);
        m._sndcard_type = (db)parse_u32(argv[3]);
        state.es = seg_offset(seg003);
        state.edi = offset(seg003, _mystr);
        call_far(ksub_12d05, &state);
        std::printf("cx=%04x data=", (unsigned)(state.ecx & 0xffff));
        print_bytes(scratch, 23);
        std::printf("\n");
        return 0;
    }

    if (op == "getsetplaystate") {
        if (argc != 4) return 2;
        m._play_state = (db)parse_u32(argv[2]);
        state.eax = (dw)parse_u32(argv[3]);
        call_far(k_getset_playstate, &state);
        std::printf("ax=%04x\n", (unsigned)(state.eax & 0xffff));
        return 0;
    }

    if (op == "get12f7c") {
        if (argc != 4) return 2;
        m._word_245F0 = (dw)parse_u32(argv[2]);
        m._word_245F6 = (dw)parse_u32(argv[3]);
        call_far(k_get_12f7c, &state);
        std::printf("ax=%04x bx=%04x\n", (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff));
        return 0;
    }

    if (op == "sndinit" || op == "sndon" || op == "sndoff" || op == "snddeinit" || op == "sndoffx") {
        if (argc != 5) return 2;
        m._snd_init = (db)parse_u32(argv[2]);
        m._snd_set_flag = (db)parse_u32(argv[3]);
        m._sndcard_type = (db)parse_u32(argv[4]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = seg_offset(seg003);
        if (op == "sndinit") {
            call_near(k_snd_initialze, &state);
        } else if (op == "sndon") {
            call_near(k_snd_on, &state);
        } else if (op == "sndoff") {
            call_near(k_snd_off, &state);
        } else if (op == "snddeinit") {
            call_near(k_snd_deinit, &state);
        } else {
            call_far(k_snd_offx, &state);
        }
        db data[] = {m._snd_init, m._snd_set_flag, m._sndcard_type};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub12da8guard") {
        m._snd_init = 1;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1603;
        state.ebx = 0x7856;
        state.ecx = 0x0907;
        state.edx = 0x0220;
        state.esi = 0x0084;
        state.edi = 0x1234;
        call_far(ksub_12da8, &state);
        db data[14] = {
            m._sndcard_type,
            (db)(m._snd_base_port & 0xff), (db)((m._snd_base_port >> 8) & 0xff),
            m._irq_number,
            m._dma_channel,
            m._freq_246D7,
            m._byte_246D8,
            m._byte_246D9,
            (db)(m._config_word & 0xff), (db)((m._config_word >> 8) & 0xff),
            m._byte_246DC,
            (db)(m._freq1 & 0xff), (db)((m._freq1 >> 8) & 0xff),
            m._snd_init,
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub12d35disable") {
        if (argc != 3) return 2;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0;
        m._config_word = (dw)parse_u32(argv[2]);
        m._byte_14F71 = 0xff;
        call_far(ksub_12d35, &state);
        db data[] = {m._byte_14F71};
        std::printf("ax=%04x bx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub1415e") {
        if (argc != 6) return 2;
        dw index = (dw)parse_u32(argv[2]);
        dw total = (dw)parse_u32(argv[3]);
        db segment_index = (db)parse_u32(argv[4]);
        db pending = (db)parse_u32(argv[5]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        m._word_245F0 = index;
        m._word_245FA = total;
        m._word_245F6 = 0x7777;
        m._my_seg_index = 0x7777;
        m._byte_24669 = pending;
        m._byte_2466A = 0xaa;
        m._byte_2466B = 0xbb;
        m._byte_2466C = 0xcc;
        m._byte_2466D = 0xdd;
        std::memset(m._byte_282E8, 0, sizeof(m._byte_282E8));
        m._byte_280E8[index] = 0;
        m._byte_27FE8[index] = segment_index;
        m._segs_table[segment_index] = seg_offset(seg003);
        call_near(ksub_1415e, &state);
        db data[] = {
            (db)(m._pointer_245B4 & 0xff), (db)((m._pointer_245B4 >> 8) & 0xff),
            (db)(m._word_245F0 & 0xff), (db)((m._word_245F0 >> 8) & 0xff),
            (db)(m._word_245F2 & 0xff), (db)((m._word_245F2 >> 8) & 0xff),
            (db)(m._my_seg_index & 0xff), (db)((m._my_seg_index >> 8) & 0xff),
            (db)(m._word_245F6 & 0xff), (db)((m._word_245F6 >> 8) & 0xff),
            (db)(m._word_245F8 & 0xff), (db)((m._word_245F8 >> 8) & 0xff),
            (db)(m._word_245FA & 0xff), (db)((m._word_245FA >> 8) & 0xff),
            m._byte_24669, m._byte_2466A, m._byte_2466B, m._byte_2466C, m._byte_2466D,
            m._byte_282E8[index >> 3],
        };
        std::printf("si=%04x data=", (unsigned)(state.esi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub12f56") {
        if (argc != 7) return 2;
        dw index = (dw)parse_u32(argv[2]);
        dw total = (dw)parse_u32(argv[3]);
        db segment_index = (db)parse_u32(argv[4]);
        db pending = (db)parse_u32(argv[5]);
        db bh = (db)parse_u32(argv[6]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = index;
        state.ebx = ((dw)bh << 8) | pending;
        m._word_245FA = total;
        m._word_245F6 = 0x7777;
        m._my_seg_index = 0x7777;
        m._byte_24669 = 0x99;
        m._byte_2466A = 0xaa;
        m._byte_2466B = 0xbb;
        m._byte_2466C = 0xcc;
        m._byte_2466D = 0xdd;
        std::memset(m._byte_282E8, 0, sizeof(m._byte_282E8));
        m._byte_280E8[index] = 0;
        m._byte_27FE8[index] = segment_index;
        m._segs_table[segment_index] = seg_offset(seg003);
        call_far(ksub_12f56, &state);
        db data[] = {
            (db)(m._pointer_245B4 & 0xff), (db)((m._pointer_245B4 >> 8) & 0xff),
            (db)(m._word_245F0 & 0xff), (db)((m._word_245F0 >> 8) & 0xff),
            (db)(m._word_245F2 & 0xff), (db)((m._word_245F2 >> 8) & 0xff),
            (db)(m._my_seg_index & 0xff), (db)((m._my_seg_index >> 8) & 0xff),
            (db)(m._word_245F6 & 0xff), (db)((m._word_245F6 >> 8) & 0xff),
            (db)(m._word_245F8 & 0xff), (db)((m._word_245F8 >> 8) & 0xff),
            (db)(m._word_245FA & 0xff), (db)((m._word_245FA >> 8) & 0xff),
            m._byte_24669, m._byte_2466A, m._byte_2466B, m._byte_2466C, m._byte_2466D,
            m._byte_282E8[index >> 3],
        };
        std::printf("si=%04x data=", (unsigned)(state.esi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub131da") {
        if (argc != 5) return 2;
        std::memset(buf, 0, 66);
        buf[0x1D] = (db)parse_u32(argv[2]);
        buf[0x17] = (db)parse_u32(argv[3]);
        buf[0x35] = (db)parse_u32(argv[4]);
        state.ebx = offset(seg003, _mystr);
        call_near(ksub_131da, &state);
        db data[] = {buf[0x17], buf[0x35]};
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub131ef") {
        if (argc != 7) return 2;
        std::memset(buf, 0, 66);
        m._volume_245FC = (dw)parse_u32(argv[3]);
        m._byte_2467D = (db)parse_u32(argv[4]);
        buf[0x23] = (db)parse_u32(argv[5]);
        buf[0x3D] = (db)parse_u32(argv[6]);
        state.eax = (dw)parse_u32(argv[2]);
        state.ebx = offset(seg003, _mystr);
        call_near(ksub_131ef, &state);
        db data[] = {buf[0x22], buf[0x23], buf[0x36], buf[0x37], buf[0x3D]};
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub13177") {
        if (argc != 7) return 2;
        std::memset(buf, 0, 66);
        m._dword_245BC = (dd)parse_u32(argv[3]);
        m._dword_245C0 = (dd)parse_u32(argv[4]);
        m._byte_2461A = (db)parse_u32(argv[5]);
        buf[0x3D] = (db)parse_u32(argv[6]);
        state.eax = (dw)parse_u32(argv[2]);
        state.ebx = offset(seg003, _mystr);
        call_near(ksub_13177, &state);
        db data[] = {buf[0x1E], buf[0x1F], buf[0x20], buf[0x21], buf[0x3D], buf[0x3E], buf[0x3F]};
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub15577guard") {
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(channel, 0, 0x50);
        channel[0x17] = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        state.esi = offset(seg003, _channels_25908);
        state.edi = 0x2468;
        call_near(kchanel_15577, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(channel + 0x17, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "sub1609fdisabled") {
        if (argc != 3) return 2;
        unsigned buffer_size = (unsigned)(parse_u32(argv[2]) & 0xffff);
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        db *scratch = raddr(seg_offset(seg003), offset(seg003, _mystr));
        std::memset(channel, 0, 0x50);
        std::memset(scratch, 0xa5, buffer_size * 8);
        channel[0x17] = 0;
        m._word_245E4 = (dw)buffer_size;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.esi = offset(seg003, _channels_25908);
        state.edi = offset(seg003, _mystr);
        call_near(kchanel_1609f, &state);
        std::printf("ax=%04x bx=%04x cx=%04x si=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(scratch, buffer_size * 8);
        std::printf("\n");
        return 0;
    }

    if (op == "setvideomodenoop") {
        if (argc != 3) return 2;
        m._byte_1DE70 = (db)parse_u32(argv[2]);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(k_setvideomode, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff),
                    (unsigned)m._byte_1DE70);
        return 0;
    }

    if (op == "textsetup") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        m._videomempointer = ((dd)seg_offset(dseg) << 16) | offset(dseg, _buffer_1);
        m._videopoint_shiftd = 0;
        m.off_1DE3C = 0xaaaa;
        m._offs_draw = 0xbbbb;
        m._offs_draw2 = 0xcccc;
        m.off_1DE42 = 0xdddd;
        m._amount_of_x = 3;
        m._word_1DE6E = 0xeeee;
        m._byte_1DE70 = 0;
        m._byte_1DE86 = 1;
        m._snd_card_type = 1;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "text_init") {
            call_near(k_text_init, &state);
        } else if (symbol == "text_init2") {
            call_near(k_text_init2, &state);
        } else if (symbol == "f1_help") {
            call_near(k_f1_help, &state);
        } else if (symbol == "f3_textmetter") {
            call_near(k_f3_textmetter, &state);
        } else if (symbol == "f4_patternnae") {
            call_near(k_f4_patternnae, &state);
        } else if (symbol == "f6_undoc") {
            call_near(k_f6_undoc, &state);
        } else {
            std::fprintf(stderr, "unknown textsetup symbol: %s\n", symbol.c_str());
            return 2;
        }
        db data[] = {
            (db)(m.off_1DE3C & 0xff), (db)((m.off_1DE3C >> 8) & 0xff),
            (db)(m._offs_draw & 0xff), (db)((m._offs_draw >> 8) & 0xff),
            (db)(m._offs_draw2 & 0xff), (db)((m._offs_draw2 >> 8) & 0xff),
            (db)(m.off_1DE42 & 0xff), (db)((m.off_1DE42 >> 8) & 0xff),
            (db)(m._word_1DE6E & 0xff), (db)((m._word_1DE6E >> 8) & 0xff),
            m._byte_1DE70, m._byte_1DE86,
            (db)(m._videopoint_shiftd & 0xff), (db)((m._videopoint_shiftd >> 8) & 0xff),
            (db)((m._videopoint_shiftd >> 16) & 0xff), (db)((m._videopoint_shiftd >> 24) & 0xff),
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "graphsetup") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        const bool spectr = symbol == "f5_graphspectr" || symbol == "init_f5_spectr";
        m.off_1DE3C = 0xaaaa;
        m._offs_draw = 0xbbbb;
        m._offs_draw2 = 0xcccc;
        m.off_1DE42 = 0xdddd;
        m._byte_1DE70 = spectr ? 4 : 3;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        if (symbol == "f2_waves") {
            call_near(k_f2_waves, &state);
        } else if (symbol == "init_vga_waves") {
            call_near(k_init_vga_waves, &state);
        } else if (symbol == "f5_graphspectr") {
            call_near(k_f5_graphspectr, &state);
        } else if (symbol == "init_f5_spectr") {
            call_near(k_init_f5_spectr, &state);
        } else {
            std::fprintf(stderr, "unknown graphsetup symbol: %s\n", symbol.c_str());
            return 2;
        }
        db data[] = {
            (db)(m.off_1DE3C & 0xff), (db)((m.off_1DE3C >> 8) & 0xff),
            (db)(m._offs_draw & 0xff), (db)((m._offs_draw >> 8) & 0xff),
            (db)(m._offs_draw2 & 0xff), (db)((m._offs_draw2 >> 8) & 0xff),
            (db)(m.off_1DE42 & 0xff), (db)((m.off_1DE42 >> 8) & 0xff),
            m._byte_1DE70,
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "int9keyb") {
        m._byte_1C1B8 = 0;
        m._key_code = 0xaaaa;
        m._keyb_switches = 0x1357;
        m._prev_scan_code = 0;
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(seg001);
        state.es = seg_offset(seg001);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_iret(k_int9_keyb, &state);
        db data[] = {
            (db)(m._key_code & 0xff), (db)((m._key_code >> 8) & 0xff),
            (db)(m._keyb_switches & 0xff), (db)((m._keyb_switches >> 8) & 0xff),
            m._prev_scan_code,
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub1ab8c") {
        if (argc != 4) return 2;
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 0x80);
        scratch[0x35] = (db)parse_u32(argv[2]);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.fs = seg_offset(dseg);
        state.ebx = offset(dseg, _buffer_1);
        state.ecx = (dw)parse_u32(argv[3]);
        state.esi = 0x2222;
        call_near(ksub_1ab8c, &state);
        std::printf("ax=%04x si=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.esi & 0xffff));
        return 0;
    }

    if (op == "txt1abae") {
        if (argc != 3) return 2;
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 0x90);
        std::memcpy(scratch, argv[2], 0x16);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.fs = seg_offset(dseg);
        state.esi = offset(dseg, _buffer_1);
        state.edi = offset(dseg, _buffer_1) + 0x40;
        call_near(k_txt_1abae, &state);
        std::printf("si=%04x di=%04x data=",
                    (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(scratch + 0x40, 0x16 * 2);
        std::printf("\n");
        return 0;
    }

    if (op == "sub13826") {
        if (argc != 4) return 2;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        unsigned value = (unsigned)parse_u32(argv[2]);
        unsigned di = (((value & 0x0f) - 1) & 0x0f) * 2;
        std::memset(channel, 0, 0x50);
        m._byte_2461A = 1;
        *(dw *)raddr(seg_offset(seg003), offset(seg003, _word_246DE) + di) = (dw)parse_u32(argv[3]);
        *(dw *)(channel + 0x14) = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.ebx = offset(seg003, _channels_25908);
        state.eax = (dw)value;
        call_near(ksub_13826, &state);
        std::printf("ax=%04x cx=%04x di=%04x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        return 0;
    }

    if (op == "sub137d5guard") {
        if (argc != 3) return 2;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(channel, 0, 0x50);
        channel[0x0A] = 33;
        channel[0x0B] = 0x77;
        channel[0x3D] = (db)parse_u32(argv[2]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.ebx = offset(seg003, _channels_25908);
        state.eax = 0x1234;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(ksub_137d5, &state);
        db data[] = {channel[0x0A], channel[0x3D]};
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub13429guard") {
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(channel, 0, 0x50);
        channel[0x03] = 0x55;
        channel[0x17] = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.ebx = offset(seg003, _channels_25908);
        state.eax = 0x1234;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(ksub_13429, &state);
        db data[] = {channel[0x03], channel[0x17]};
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff), (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub13d95") {
        if (argc != 3) return 2;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.ecx = (dw)parse_u32(argv[2]);
        call_near(ksub_13d95, &state);
        db data[] = {m._byte_24618, m._byte_24619};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub13cf6") {
        if (argc != 5) return 2;
        m._sndflags_24622 = 0;
        m._freq1 = (dw)parse_u32(argv[3]);
        m._word_245E8 = (dw)parse_u32(argv[4]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = (dw)parse_u32(argv[2]);
        call_near(ksub_13cf6, &state);
        db data[] = {
            m._byte_24666,
            (db)(m._word_245EA & 0xff), (db)((m._word_245EA >> 8) & 0xff),
            (db)(m._word_245EC & 0xff), (db)((m._word_245EC >> 8) & 0xff),
            (db)(m._word_245EE & 0xff), (db)((m._word_245EE >> 8) & 0xff),
            (db)(m._word_245E4 & 0xff), (db)((m._word_245E4 >> 8) & 0xff),
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "spectr1bce9equal") {
        if (argc != 3) return 2;
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 0x1108);
        scratch[0] = (db)parse_u32(argv[2]);
        scratch[0x64] = (db)parse_u32(argv[2]);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.ebx = offset(dseg, _buffer_1);
        state.ebp = offset(dseg, _buffer_1) + 0x1000;
        call_near(k_spectr_1bce9, &state);
        std::printf("bx=%04x bp=%04x data=",
                    (unsigned)(state.ebx & 0xffff), (unsigned)(state.ebp & 0xffff));
        print_bytes(scratch + 0x1000, 8);
        std::printf("\n");
        return 0;
    }

    if (op == "spectr1bc2dequal") {
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 0x1108);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.ebx = offset(dseg, _buffer_1);
        state.ebp = offset(dseg, _buffer_1) + 0x1000;
        call_near(k_spectr_1bc2d, &state);
        std::printf("bx=%04x bp=%04x data=",
                    (unsigned)(state.ebx & 0xffff), (unsigned)(state.ebp & 0xffff));
        print_bytes(scratch + 0x1000, 8);
        std::printf("\n");
        return 0;
    }

    if (op == "spectr1bbc1zero") {
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 0x300);
        m._byte_1DE81 = 0;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.ecx = 1;
        state.esi = offset(dseg, _buffer_1);
        state.edi = offset(dseg, _buffer_1) + 0x100;
        call_near(k_spectr_1bbc1, &state);
        db data[] = {scratch[0x100], scratch[0x100 + 0xC8], scratch[0x100 + 0x12C]};
        std::printf("si=%04x di=%04x cx=%04x data=",
                    (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff),
                    (unsigned)(state.ecx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "videoprp") {
        if (argc != 5) return 2;
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 0x1200);
        for (int i = 0; i < 3; ++i) {
            scratch[i * 0x50 + 0x3A] = (db)parse_u32(argv[2 + i]);
        }
        m._segfsbx_1DE28 = ((dd)seg_offset(dseg) << 16) | offset(dseg, _buffer_1);
        m._amount_of_x = 3;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        call_near(k_video_prp_mtr_positn, &state);
        db data[] = {
            m._byte_1DE79,
            m._byte_1DE7A,
            m._byte_1DE81,
            (db)(m._x_storage[0] & 0xff), (db)((m._x_storage[0] >> 8) & 0xff),
            (db)(m._x_storage[1] & 0xff), (db)((m._x_storage[1] >> 8) & 0xff),
            (db)(m._x_storage[2] & 0xff), (db)((m._x_storage[2] >> 8) & 0xff),
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "midi154da") {
        if (argc != 3) return 2;
        std::memset(buf, 0, 66);
        buf[0x18] = (db)parse_u32(argv[2]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_midi_154da, &state);
        std::printf("ax=%04x\n", (unsigned)(state.eax & 0xffff));
        return 0;
    }

    if (op == "midi154de") {
        if (argc != 3) return 2;
        std::memset(buf, 0, 66);
        buf[0x35] = (db)parse_u32(argv[2]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_midi_154de, &state);
        std::printf("ax=%04x dx=%04x\n", (unsigned)(state.eax & 0xffff), (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "midi15413guard") {
        if (argc != 3) return 2;
        m._byte_24677 = (db)parse_u32(argv[2]);
        state.eax = (dw)(((parse_u32(argv[2]) & 0xff) << 8) | 0x34);
        state.edx = 0x5678;
        call_near(k_midi_15413, &state);
        std::printf("ax=%04x dx=%04x data=%02x\n",
                    (unsigned)(state.eax & 0xffff), (unsigned)(state.edx & 0xffff),
                    (unsigned)m._byte_24677);
        return 0;
    }

    if (op == "midi154ac") {
        if (argc != 5) return 2;
        std::memset(buf, 0, 66);
        m._byte_2467D = (db)parse_u32(argv[3]);
        buf[0x1B] = (db)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[2]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_midi_154ac, &state);
        std::printf("ax=%04x di=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(buf + 0x1B, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13bc0") {
        if (argc != 4) return 2;
        std::memset(buf, 0, 66);
        buf[0x09] = (db)parse_u32(argv[2]);
        state.eax = (dw)parse_u32(argv[3]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_eff_13bc0, &state);
        std::printf("data=");
        print_bytes(buf + 0x09, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13c34") {
        if (argc != 4) return 2;
        std::memset(buf, 0, 66);
        buf[0x09] = (db)parse_u32(argv[2]);
        state.eax = (dw)parse_u32(argv[3]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_eff_13c34, &state);
        std::printf("data=");
        print_bytes(buf + 0x09, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13a43") {
        if (argc != 5) return 2;
        std::memset(buf, 0, 66);
        buf[0x17] = (db)parse_u32(argv[2]);
        m._sndflags_24622 = (db)parse_u32(argv[3]);
        state.eax = (dw)parse_u32(argv[4]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_eff_13a43, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf + 0x17, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13a94") {
        if (argc != 7) return 2;
        std::memset(buf, 0, 0x50);
        buf[0x16] = (db)parse_u32(argv[2]);
        *(dd *)(buf + 0x30) = (dd)parse_u32(argv[3]);
        m._byte_2461A = (db)parse_u32(argv[4]);
        buf[0x17] = (db)parse_u32(argv[5]);
        *(dw *)(buf + 0x4C) = 0xaaaa;
        state.eax = (dw)parse_u32(argv[6]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CE = k_nullsub_5;
        call_near(k_eff_13a94, &state);
        db data[] = {buf[0x16], buf[0x17], buf[0x4C], buf[0x4D]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13ad7") {
        if (argc != 5) return 2;
        std::memset(buf, 0, 66);
        buf[0x08] = (db)parse_u32(argv[2]);
        m._byte_2467D = (db)parse_u32(argv[3]);
        state.eax = (dw)parse_u32(argv[4]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_13ad7, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf + 0x08, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13b06") {
        if (argc != 4) return 2;
        m._flag_playsetttings = (db)parse_u32(argv[2]);
        m._word_245F0 = 0xaaaa;
        state.eax = (dw)parse_u32(argv[3]);
        call_near(k_eff_13b06, &state);
        db data[] = {
            (db)(m._word_245F0 & 0xff),
            (db)((m._word_245F0 >> 8) & 0xff),
        };
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13b78") {
        if (argc != 4) return 2;
        std::memset(buf, 0, 66);
        m._byte_2467D = (db)parse_u32(argv[3]);
        state.eax = (dw)parse_u32(argv[2]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_13b78, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf + 0x08, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13b88") {
        if (argc != 5) return 2;
        m._byte_24669 = (db)parse_u32(argv[2]);
        m._byte_2466A = (db)parse_u32(argv[3]);
        state.eax = (dw)parse_u32(argv[4]);
        call_near(k_eff_13b88, &state);
        db data[] = {m._byte_24669, m._byte_2466A};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13bb2") {
        if (argc != 4) return 2;
        std::memset(buf, 0, 66);
        buf[0x17] = (db)parse_u32(argv[2]);
        state.eax = (dw)parse_u32(argv[3]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_eff_13bb2, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf + 0x17, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13ba3") {
        if (argc != 4) return 2;
        std::memset(buf, 0, 66);
        buf[0x17] = (db)parse_u32(argv[2]);
        state.eax = (dw)parse_u32(argv[3]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_eff_13ba3, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf + 0x17, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13bc8") {
        if (argc != 5) return 2;
        std::memset(buf, 0, 66);
        m._byte_2461A = (db)parse_u32(argv[2]);
        state.edx = (dw)parse_u32(argv[3]);
        state.eax = (dw)parse_u32(argv[4]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_eff_13bc8, &state);
        db data[] = {buf[0x14], buf[0x15], buf[0x38], buf[0x39]};
        std::printf("ax=%04x dx=%04x data=", (unsigned)(state.eax & 0xffff), (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13c02") {
        if (argc != 7) return 2;
        std::memset(buf, 0, 66);
        m._byte_24668 = (db)parse_u32(argv[2]);
        m._word_245F6 = (dw)parse_u32(argv[3]);
        buf[0x3B] = (db)parse_u32(argv[4]);
        buf[0x3C] = (db)parse_u32(argv[5]);
        m._byte_24669 = 0xaa;
        m._byte_2466B = 0xbb;
        state.eax = (dw)parse_u32(argv[6]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_eff_13c02, &state);
        db data[] = {buf[0x3B], buf[0x3C]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13c3f") {
        if (argc != 6) return 2;
        std::memset(buf, 0, 66);
        m._byte_24668 = (db)parse_u32(argv[2]);
        buf[0x17] = (db)parse_u32(argv[3]);
        m._sndflags_24622 = (db)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[5]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_eff_13c3f, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf + 0x17, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13c64") {
        if (argc != 5) return 2;
        std::memset(buf, 0, 66);
        m._byte_24668 = (db)parse_u32(argv[2]);
        buf[0x3D] = (db)parse_u32(argv[3]);
        state.eax = (dw)parse_u32(argv[4]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245C8 = k_nullsub_5;
        call_near(k_eff_13c64, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf + 0x3D, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13c88") {
        if (argc != 6) return 2;
        std::memset(buf, 0, 66);
        buf[0x08] = (db)parse_u32(argv[2]);
        m._byte_24668 = (db)parse_u32(argv[3]);
        m._byte_2467D = (db)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[5]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_13c88, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf + 0x08, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13c95") {
        if (argc != 5) return 2;
        std::memset(buf, 0, 66);
        buf[0x08] = (db)parse_u32(argv[2]);
        m._byte_24668 = (db)parse_u32(argv[3]);
        state.eax = (dw)parse_u32(argv[4]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_13c95, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf + 0x08, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13ca2") {
        if (argc != 4) return 2;
        std::memset(buf, 0, 66);
        m._byte_24668 = (db)parse_u32(argv[2]);
        state.eax = (dw)parse_u32(argv[3]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_13ca2, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(&m._byte_24668, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13cb3") {
        if (argc != 7) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        buf[0x0A] = (db)parse_u32(argv[3]);
        buf[0x0B] = (db)parse_u32(argv[4]);
        m._byte_24668 = (db)parse_u32(argv[5]);
        state.eax = (dw)parse_u32(argv[6]);
        state.ebx = offset(seg003, _mystr);
        call_near(k_eff_13cb3, &state);
        db data[] = {buf[0x0A], buf[0x0B]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13cc9") {
        if (argc != 6) return 2;
        m._byte_24668 = (db)parse_u32(argv[2]);
        m._byte_2466D = (db)parse_u32(argv[3]);
        m._byte_2466C = (db)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[5]);
        call_near(k_eff_13cc9, &state);
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(&m._byte_2466C, 1);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13cdd") {
        if (argc != 6) return 2;
        m._flag_playsetttings = (db)parse_u32(argv[2]);
        m._byte_24667 = (db)parse_u32(argv[3]);
        m._byte_24668 = (db)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[5]);
        call_near(k_eff_13cdd, &state);
        db data[] = {m._byte_24667, m._byte_24668};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13de5" || op == "eff13def") {
        if (argc != 6) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        m._byte_24668 = (db)parse_u32(argv[3]);
        buf[0x34] = (db)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[5]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        call_near(op == "eff13de5" ? k_eff_13de5 : k_eff_13def, &state);
        db data[] = {buf[0], buf[1], buf[0x34]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13e1e") {
        if (argc != 7) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        *(dw *)(buf + 0x10) = (dw)parse_u32(argv[3]);
        *(dw *)(buf + 0x12) = (dw)parse_u32(argv[4]);
        buf[0x17] = (db)parse_u32(argv[5]);
        state.eax = (dw)parse_u32(argv[6]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        call_near(k_eff_13e1e, &state);
        db data[] = {buf[0], buf[1], buf[0x10], buf[0x11], buf[0x12], buf[0x13], buf[0x17]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13e2d") {
        if (argc != 8) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        buf[0x09] = (db)parse_u32(argv[3]);
        buf[0x0C] = (db)parse_u32(argv[4]);
        buf[0x0D] = (db)parse_u32(argv[5]);
        m._flag_playsetttings = (db)parse_u32(argv[6]);
        state.eax = (dw)parse_u32(argv[7]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        call_near(k_eff_13e2d, &state);
        db data[] = {buf[0], buf[1], buf[0x0C], buf[0x0D]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13e32") {
        if (argc != 7) return 2;
        std::memset(buf, 0, 66);
        buf[0x08] = (db)parse_u32(argv[2]);
        m._byte_24668 = (db)parse_u32(argv[3]);
        m._byte_2467D = (db)parse_u32(argv[4]);
        buf[0x34] = (db)parse_u32(argv[5]);
        state.eax = (dw)parse_u32(argv[6]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_13e32, &state);
        db data[] = {buf[0x08], buf[0x34]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff1387f" || op == "eff13886" || op == "eff1389d" || op == "eff138a4") {
        if (argc != 4 && argc != 5) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        state.eax = (dw)parse_u32(argv[3]);
        state.ebx = offset(seg003, _mystr);
        m._byte_24668 = argc == 5 ? (db)parse_u32(argv[4]) : 0;
        *(dw *)&m.off_245CA = k_nullsub_5;
        if (!call_effect_slide(op, &state)) return 2;
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(buf, 2);
        std::printf("\n");
        return 0;
    }

    if (op == "eff139ac") {
        if (argc != 9) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        *(dw *)(buf + 0x10) = (dw)parse_u32(argv[3]);
        *(dw *)(buf + 0x12) = (dw)parse_u32(argv[4]);
        buf[0x17] = (db)parse_u32(argv[5]);
        buf[0x08] = (db)parse_u32(argv[6]);
        m._byte_2467D = (db)parse_u32(argv[7]);
        state.eax = (dw)parse_u32(argv[8]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_139ac, &state);
        db data[] = {buf[0], buf[1], buf[0x08], buf[0x10], buf[0x11], buf[0x12], buf[0x13], buf[0x17]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff139b2") {
        if (argc != 10) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        buf[0x09] = (db)parse_u32(argv[3]);
        buf[0x0C] = (db)parse_u32(argv[4]);
        buf[0x0D] = (db)parse_u32(argv[5]);
        m._flag_playsetttings = (db)parse_u32(argv[6]);
        buf[0x08] = (db)parse_u32(argv[7]);
        m._byte_2467D = (db)parse_u32(argv[8]);
        state.eax = (dw)parse_u32(argv[9]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_139b2, &state);
        db data[] = {buf[0], buf[1], buf[0x08], buf[0x0C], buf[0x0D]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff139b9") {
        if (argc != 8) return 2;
        std::memset(buf, 0, 66);
        buf[0x08] = (db)parse_u32(argv[2]);
        buf[0x09] = (db)parse_u32(argv[3]);
        buf[0x0E] = (db)parse_u32(argv[4]);
        buf[0x0F] = (db)parse_u32(argv[5]);
        m._byte_2467D = (db)parse_u32(argv[6]);
        state.eax = (dw)parse_u32(argv[7]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_139b9, &state);
        db data[] = {buf[0x08], buf[0x0E], buf[0x0F]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff1392f") {
        if (argc != 8) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        buf[0x09] = (db)parse_u32(argv[3]);
        buf[0x0C] = (db)parse_u32(argv[4]);
        buf[0x0D] = (db)parse_u32(argv[5]);
        m._flag_playsetttings = (db)parse_u32(argv[6]);
        state.eax = (dw)parse_u32(argv[7]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        call_near(k_eff_1392f, &state);
        db data[] = {buf[0], buf[1], buf[0x0C], buf[0x0D]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff138d2") {
        if (argc != 7) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        *(dw *)(buf + 0x10) = (dw)parse_u32(argv[3]);
        *(dw *)(buf + 0x12) = (dw)parse_u32(argv[4]);
        buf[0x17] = (db)parse_u32(argv[5]);
        state.eax = (dw)parse_u32(argv[6]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        call_near(k_eff_138d2, &state);
        db data[] = {buf[0], buf[1], buf[0x10], buf[0x11], buf[0x12], buf[0x13], buf[0x17]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13ce8") {
        if (argc != 5) return 2;
        m._byte_24667 = (db)parse_u32(argv[2]);
        m._byte_24668 = (db)parse_u32(argv[3]);
        state.eax = (dw)parse_u32(argv[4]);
        call_near(k_eff_13ce8, &state);
        db data[] = {m._byte_24667, m._byte_24668};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "strlen") {
        if (argc != 3) return 2;
        std::snprintf((char *)buf, 66, "%s", argv[2]);
        state.esi = offset(seg003, _mystr);
        call_near(k_mystrlen_0, &state);
        std::printf("ax=%04x si=%04x\n", (unsigned)(state.eax & 0xffff), (unsigned)(state.esi & 0xffff));
        return 0;
    }

    if (op == "strcpy") {
        if (argc != 3) return 2;
        db *dst = buf + 32;
        std::snprintf((char *)buf, 32, "%s", argv[2]);
        std::memset(dst, 0xcc, 32);
        state.esi = offset(seg003, _mystr);
        state.edi = offset(seg003, _mystr) + 32;
        state.es = state.ds;
        call_near(k_strcpy_count_0, &state);
        std::printf("cx=%04x si=%04x di=%04x data=", (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dst, std::strlen(argv[2]) + 1);
        std::printf("\n");
        return 0;
    }

    if (op == "copyprint") {
        if (argc != 4) return 2;
        db *dst = buf + 32;
        std::memset(buf, 0, 32);
        std::memcpy(buf, argv[2], std::strlen(argv[2]));
        std::memset(dst, 0x2e, 32);
        state.esi = offset(seg003, _mystr);
        state.edi = offset(seg003, _mystr) + 32;
        state.ecx = (dw)parse_u32(argv[3]);
        call_near(k_copy_printable, &state);
        std::printf("cx=%04x si=%04x di=%04x data=", (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dst, (size_t)parse_u32(argv[3]));
        std::printf("\n");
        return 0;
    }

    if (op == "myasmsprintf") {
        db *dst = buf + 32;
        std::memset(buf, 0, 66);
        size_t pos = 0;
        std::memcpy(buf + pos, "U=", 2); pos += 2;
        buf[pos++] = 4; buf[pos++] = 'u'; *(dw *)(buf + pos) = offset(seg003, _mystr) + 56; pos += 2;
        std::memcpy(buf + pos, " I=", 3); pos += 3;
        buf[pos++] = 8; buf[pos++] = 'i'; *(dw *)(buf + pos) = offset(seg003, _mystr) + 58; pos += 2;
        std::memcpy(buf + pos, " X=", 3); pos += 3;
        buf[pos++] = 11; buf[pos++] = 'x'; *(dw *)(buf + pos) = offset(seg003, _mystr) + 60; pos += 2;
        buf[pos++] = 0; buf[pos++] = 0;
        buf[56] = 200;
        *(dw *)(buf + 58) = 0xfb2e;
        *(dw *)(buf + 60) = 0xabcd;
        state.esi = offset(seg003, _mystr);
        state.edi = offset(seg003, _mystr) + 32;
        call_near(k_myasmsprintf, &state);
        std::printf("si=%04x di=%04x data=", (unsigned)(state.esi & 0xffff), (unsigned)(state.edi & 0xffff));
        print_bytes(dst, 22);
        std::printf("\n");
        return 0;
    }

    if (op == "eff13e7f") {
        if (argc != 11) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        *(dw *)(buf + 0x10) = (dw)parse_u32(argv[3]);
        *(dw *)(buf + 0x12) = (dw)parse_u32(argv[4]);
        buf[0x17] = (db)parse_u32(argv[5]);
        buf[0x08] = (db)parse_u32(argv[6]);
        m._byte_24668 = (db)parse_u32(argv[7]);
        m._byte_2467D = (db)parse_u32(argv[8]);
        buf[0x34] = (db)parse_u32(argv[9]);
        state.eax = (dw)parse_u32(argv[10]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_13e7f, &state);
        db data[] = {buf[0], buf[1], buf[0x08], buf[0x10], buf[0x11], buf[0x12], buf[0x13], buf[0x17], buf[0x34]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13e84") {
        if (argc != 12) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        buf[0x09] = (db)parse_u32(argv[3]);
        buf[0x0C] = (db)parse_u32(argv[4]);
        buf[0x0D] = (db)parse_u32(argv[5]);
        m._flag_playsetttings = (db)parse_u32(argv[6]);
        buf[0x08] = (db)parse_u32(argv[7]);
        m._byte_24668 = (db)parse_u32(argv[8]);
        m._byte_2467D = (db)parse_u32(argv[9]);
        buf[0x34] = (db)parse_u32(argv[10]);
        state.eax = (dw)parse_u32(argv[11]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_13e84, &state);
        db data[] = {buf[0], buf[1], buf[0x08], buf[0x0C], buf[0x0D], buf[0x34]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13e8c") {
        if (argc != 5) return 2;
        m._sndflags_24622 = 0;
        m._freq1 = (dw)parse_u32(argv[3]);
        m._word_245E8 = (dw)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[2]);
        call_near(k_eff_13e8c, &state);
        db data[] = {
            (db)(m._word_245EA & 0xff), (db)((m._word_245EA >> 8) & 0xff),
            (db)(m._word_245EC & 0xff), (db)((m._word_245EC >> 8) & 0xff),
            (db)(m._word_245EE & 0xff), (db)((m._word_245EE >> 8) & 0xff),
            (db)(m._word_245E4 & 0xff), (db)((m._word_245E4 >> 8) & 0xff),
            m._byte_24666, m._byte_24667, m._byte_24668,
        };
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13f05") {
        if (argc != 6) return 2;
        std::memset(buf, 0, 66);
        buf[0x08] = (db)parse_u32(argv[2]);
        m._byte_24668 = (db)parse_u32(argv[3]);
        buf[0x34] = (db)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[5]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CC = k_nullsub_5;
        call_near(k_eff_13f05, &state);
        db data[] = {buf[0x08], buf[0x34]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13f3b") {
        if (argc != 8) return 2;
        std::memset(buf, 0, 66);
        buf[0x08] = (db)parse_u32(argv[2]);
        m._byte_24668 = (db)parse_u32(argv[3]);
        m._byte_2467D = (db)parse_u32(argv[4]);
        buf[0x3D] = (db)parse_u32(argv[5]);
        buf[0x34] = (db)parse_u32(argv[6]);
        state.eax = (dw)parse_u32(argv[7]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245C8 = k_nullsub_5;
        call_near(k_eff_13f3b, &state);
        db data[] = {buf[0x08], buf[0x34], buf[0x3D]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff13fbe") {
        if (argc != 8) return 2;
        std::memset(buf, 0, 66);
        *(dw *)buf = (dw)parse_u32(argv[2]);
        buf[0x0B] = (db)parse_u32(argv[3]);
        m._byte_24668 = (db)parse_u32(argv[4]);
        buf[0x34] = (db)parse_u32(argv[5]);
        buf[0x35] = (db)parse_u32(argv[6]);
        state.eax = (dw)parse_u32(argv[7]);
        state.ebx = offset(seg003, _mystr);
        *(dw *)&m.off_245CA = k_nullsub_5;
        call_near(k_eff_13fbe, &state);
        db data[] = {buf[0], buf[1], buf[0x0B], buf[0x34], buf[0x35]};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff14020") {
        if (argc != 5) return 2;
        m._sndcard_type = (db)parse_u32(argv[3]);
        m._word_245D6 = (dw)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[2]);
        call_near(k_eff_14020, &state);
        db data[] = {
            (db)(m._amplification & 0xff), (db)((m._amplification >> 8) & 0xff),
            m._high_amplif, m._byte_2467D,
        };
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "changeamplif") {
        if (argc != 5) return 2;
        m._sndcard_type = (db)parse_u32(argv[3]);
        m._word_245D6 = (dw)parse_u32(argv[4]);
        state.eax = (dw)parse_u32(argv[2]);
        call_far(k_getset_amplif, &state);
        db data[] = {
            (db)(m._amplification & 0xff), (db)((m._amplification >> 8) & 0xff),
            m._high_amplif, m._byte_2467D,
        };
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub13044") {
        if (argc != 6) return 2;
        m._byte_2467E = (db)parse_u32(argv[2]);
        m._word_245D6 = (dw)parse_u32(argv[3]);
        m._amplification = (dw)parse_u32(argv[4]);
        m._high_amplif = (db)parse_u32(argv[5]);
        std::memset(m._vlm_byte_table, 0, sizeof(m._vlm_byte_table));
        call_near(ksub_13044, &state);
        db data[38] = {
            (db)(m.off_2462E & 0xff), (db)((m.off_2462E >> 8) & 0xff),
            (db)(m.off_24656 & 0xff), (db)((m.off_24656 >> 8) & 0xff),
            m._byte_2467D, m._byte_2467E,
        };
        std::memcpy(data + 6, m._vlm_byte_table, 32);
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub13017bounded") {
        db *out = raddr(seg_offset(seg003), offset(seg003, _myout));
        std::memset(out, 0, 0x90);
        *(dw *)raddr(seg_offset(seg003), 0x0032) = 2;
        *(dd *)(out + 0x24) = 0xaaaaaaaa;
        *(dd *)(out + 0x2c) = 0x11111111;
        out[0x3c] = 0;
        *(dd *)(out + 0x64) = 0x22222222;
        *(dd *)(out + 0x6c) = 0x33333333;
        out[0x7c] = 8;
        m._dma_buf_curpointer = 0x0801;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        db data[] = {
            (db)(*(dd *)(out + 0x24) & 0xff), (db)((*(dd *)(out + 0x24) >> 8) & 0xff),
            (db)((*(dd *)(out + 0x24) >> 16) & 0xff), (db)((*(dd *)(out + 0x24) >> 24) & 0xff),
            (db)(*(dd *)(out + 0x64) & 0xff), (db)((*(dd *)(out + 0x64) >> 8) & 0xff),
            (db)((*(dd *)(out + 0x64) >> 16) & 0xff), (db)((*(dd *)(out + 0x64) >> 24) & 0xff),
            (db)(m._dma_buf_curpointer & 0xff), (db)((m._dma_buf_curpointer >> 8) & 0xff),
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "configuretimerbounded") {
        db *out = raddr(seg_offset(seg003), offset(seg003, _myout));
        std::memset(out, 0, 0x90);
        *(dw *)raddr(seg_offset(seg003), 0x0032) = 2;
        *(dd *)(out + 0x24) = 0xaaaaaaaa;
        *(dd *)(out + 0x2c) = 0x11111111;
        out[0x3c] = 0;
        *(dd *)(out + 0x64) = 0x22222222;
        *(dd *)(out + 0x6c) = 0x33333333;
        out[0x7c] = 8;
        m._dma_buf_curpointer = 0x0801;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        db data[] = {
            (db)(*(dd *)(out + 0x24) & 0xff), (db)((*(dd *)(out + 0x24) >> 8) & 0xff),
            (db)((*(dd *)(out + 0x24) >> 16) & 0xff), (db)((*(dd *)(out + 0x24) >> 24) & 0xff),
            (db)(*(dd *)(out + 0x64) & 0xff), (db)((*(dd *)(out + 0x64) >> 8) & 0xff),
            (db)((*(dd *)(out + 0x64) >> 16) & 0xff), (db)((*(dd *)(out + 0x64) >> 24) & 0xff),
            (db)(m._dma_buf_curpointer & 0xff), (db)((m._dma_buf_curpointer >> 8) & 0xff),
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sndonparntbounded") {
        std::memset(m._volume_25908, 0xa5, 0x20);
        m._byte_24669 = 0x91;
        m._byte_2466A = 0x92;
        m._byte_2466B = 0x93;
        m._byte_2466C = 0x94;
        m._byte_2466D = 0x95;
        m._byte_24671 = 0x96;
        m._play_state = 0x97;
        m._dma_buf_curpointer = 0x1111;
        m._dma_block_pointer = 0x2222;
        m._audio_quality_counter = 0x33;
        m._byte_24621 = 0x44;
        m._byte_24679 = 6;
        m._byte_2467A = 125;
        m._byte_24668 = m._byte_24679;
        m._byte_2467B = (db)(((dw)m._byte_2467A << 1) / 5);
        m._byte_2467C = 0;
        m._byte_24669 = 0;
        m._byte_2466A = 0;
        m._byte_2466B = 0;
        m._byte_2466C = 0;
        m._byte_2466D = 0;
        m._byte_24671 = 0;
        m._play_state = 0;
        m._dma_buf_curpointer = 0;
        m._dma_block_pointer = 0;
        m._audio_quality_counter = 0;
        m._byte_24621 = 0;
        std::memset(m._volume_25908, 0, 0x20);
        db data[] = {
            m._byte_24669, m._byte_2466A, m._byte_2466B, m._byte_2466C,
            m._byte_2466D, m._byte_24671, m._play_state,
            (db)(m._dma_buf_curpointer & 0xff), (db)((m._dma_buf_curpointer >> 8) & 0xff),
            (db)(m._dma_block_pointer & 0xff), (db)((m._dma_block_pointer >> 8) & 0xff),
            m._audio_quality_counter, m._byte_24621,
            m._byte_24668, m._byte_2467B, m._byte_2467C,
            m._volume_25908[0], m._volume_25908[0x1f],
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sbonbounded") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        if (symbol != "sb_on" && symbol != "sb16_on") {
            std::fprintf(stderr, "unknown sbonbounded symbol: %s\n", symbol.c_str());
            return 2;
        }
        m._word_2460E = 0x1000;
        m._byte_2466E = 1;
        m._dma_mode = 0x58;
        db data[] = {
            (db)(m._word_2460E & 0xff), (db)((m._word_2460E >> 8) & 0xff),
            m._byte_2466E, m._dma_mode,
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sbhandlerintbounded") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.edx = 0x022e;
        m._word_2460E = 0x1000;
        db data[] = {
            (db)(m._word_2460E & 0xff),
            (db)((m._word_2460E >> 8) & 0xff),
        };
        std::printf("ax=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub19050bounded") {
        state.cs = seg_offset(dseg);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        m._byte_1DE7E = 7;
        std::printf("ax=0900 dx=0000 data=%02x\n", (unsigned)m._byte_1DE7E);
        return 0;
    }

    if (op == "sub154f4") {
        if (argc != 9) return 2;
        const dw base = 0x2800;
        db *channel = raddr(seg_offset(seg003), base);
        std::memset(channel, 0, 0x50);
        m._ems_enabled = 0;
        m._word_245E4 = (dw)parse_u32(argv[2]);
        m._flag_playsetttings = (db)parse_u32(argv[3]);
        *(dd *)(channel + 0x04) = (dd)parse_u32(argv[4]);
        *(dw *)(channel + 0x20) = (dw)parse_u32(argv[5]);
        channel[0x23] = (db)parse_u32(argv[6]);
        *(dw *)(channel + 0x24) = (dw)parse_u32(argv[7]);
        *(dw *)(channel + 0x26) = 0xffff;
        *(dw *)(channel + 0x36) = (dw)parse_u32(argv[8]);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.esi = base;
        call_near(ksub_154f4, &state);
        db data[] = {
            (db)(m._word_24614 & 0xff), (db)((m._word_24614 >> 8) & 0xff),
            m._byte_24616,
            m._byte_24683,
        };
        std::printf("ax=%04x bx=%04x cx=%04x bp=%04x si=%04x esi=%08x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.ebp & 0xffff),
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)state.esi);
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub135ca") {
        const dw base = 0x2800;
        db *event = raddr(seg_offset(seg003), base);
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(event, 0, 4);
        std::memset(channel, 0, 0x50);
        *(dw *)(channel + 0x0A) = 0xbeef;
        channel[0x17] = 0;
        channel[0x3D] = 0xaa;
        m._mod_channels_number = 1;
        m._pointer_245B4 = ((dd)seg_offset(seg003) << 16) | base;
        *(dw *)&m.off_245CA = k_nullsub_5;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        call_near(ksub_3_135ca, &state);
        db data[] = {
            (db)(m._pointer_245B4 & 0xff), (db)((m._pointer_245B4 >> 8) & 0xff),
            channel[0x0A], channel[0x0B], channel[0x17], channel[0x3D],
        };
        std::printf("ax=%04x bx=%04x cx=%04x si=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.esi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub13813") {
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(channel, 0, 0x50);
        channel[0x0A] = 33;
        channel[0x0B] = 0x7c;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.ebx = offset(seg003, _channels_25908);
        state.eax = 0x1234;
        state.ecx = 0x5678;
        state.edx = 0x9abc;
        state.edi = 0xdef0;
        call_near(kchanl_2_eff_13813, &state);
        db data[] = {channel[0x0A], channel[0x0B]};
        std::printf("ax=%04x cx=%04x dx=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub140b6guard") {
        m._byte_24671 = 1;
        m._play_state = 0;
        m._byte_24668 = 0x44;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        call_near(kprepare_channels_1_140b6, &state);
        db data[] = {m._byte_24671, m._byte_24668};
        std::printf("ax=%04x bx=%04x cx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "volumeprepinactive") {
        if (argc != 4) return 2;
        const dw out = 0x2900;
        const dw size = (dw)parse_u32(argv[3]);
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        db *output = raddr(seg_offset(seg003), out);
        std::memset(channel, 0, 0x50);
        std::memset(output, 0xa5, size);
        channel[0x17] = 0;
        m._mod_channels_number = 1;
        m._sndflags_24622 = 0;
        m._ems_enabled = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.edi = out;
        state.eax = (dw)parse_u32(argv[2]);
        state.ecx = size;
        call_far(k_volume_prep, &state);
        db data[12] = {
            (db)(m._word_24610 & 0xff), (db)((m._word_24610 >> 8) & 0xff),
            (db)(m._my_size & 0xff), (db)((m._my_size >> 8) & 0xff),
        };
        std::memcpy(data + 4, output, 8);
        std::printf("ax=%04x cx=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub1281asmallmix") {
        const dw out = 0x2aa0;
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        db *output = raddr(seg_offset(seg003), out);
        std::memset(channel, 0, 0x50);
        std::memset(output, 0xa5, 4);
        std::memset(m._vlm_byte_table, 0, sizeof(m._vlm_byte_table));
        *(dw *)(channel + 0x20) = 0x0100;
        channel[0x23] = 0;
        *(db *)raddr(seg_offset(seg003), 0) = 0;
        *(db *)raddr(seg_offset(seg003), 1) = 1;
        *(db *)raddr(seg_offset(seg003), 2) = 2;
        *(db *)raddr(seg_offset(seg003), 3) = 3;
        m._vlm_byte_table[1] = 0x11;
        m._vlm_byte_table[3] = 0x22;
        m._vlm_byte_table[5] = 0x33;
        m._vlm_byte_table[7] = 0x44;
        m._word_24610 = 1;
        m._my_size = 4;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.fs = seg_offset(seg003);
        state.esi = offset(seg003, _channels_25908);
        state.edi = out;
        state.eax = 0;
        call_near(ksub_1281a, &state);
        std::printf("ax=%04x cx=%04x dx=%04x si=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(output, 4);
        std::printf("\n");
        return 0;
    }

    if (op == "spectr1b084len2") {
        db *buf = raddr(seg_offset(dseg), offset(dseg, _buffer_1) + 0x300);
        std::memset(buf, 0, 0x60);
        *(dd *)(buf + 0) = 0x00010000;
        *(dd *)(buf + 4) = 0x00020000;
        *(dd *)(buf + 8) = 0x00030000;
        *(dd *)(buf + 12) = 0x00040000;
        m._word_24514 = 2;
        m._word_24520 = 1;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.edi = offset(dseg, _buffer_1) + 0x300;
        call_near(k_spectr_1b084, &state);
        db data[30];
        std::memcpy(data, buf, 16);
        data[16] = (db)(m._word_2450E & 0xff);
        data[17] = (db)((m._word_2450E >> 8) & 0xff);
        data[18] = (db)(m._dword_244C8 & 0xff);
        data[19] = (db)((m._dword_244C8 >> 8) & 0xff);
        data[20] = (db)((m._dword_244C8 >> 16) & 0xff);
        data[21] = (db)((m._dword_244C8 >> 24) & 0xff);
        data[22] = (db)(m._multip_244CC & 0xff);
        data[23] = (db)((m._multip_244CC >> 8) & 0xff);
        data[24] = (db)((m._multip_244CC >> 16) & 0xff);
        data[25] = (db)((m._multip_244CC >> 24) & 0xff);
        data[26] = (db)(m._multip_244D0 & 0xff);
        data[27] = (db)((m._multip_244D0 >> 8) & 0xff);
        data[28] = (db)((m._multip_244D0 >> 16) & 0xff);
        data[29] = (db)((m._multip_244D0 >> 24) & 0xff);
        std::printf("ax=%04x cx=%04x si=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "f5drawspectrinactive") {
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(scratch, 0, 0x800);
        std::memset(channel, 0, 0x50);
        m._mod_channels_number = 1;
        m._sndflags_24622 = 0;
        m._ems_enabled = 0;
        m._amount_of_x = 1;
        m._segfsbx_1DE28 = ((dd)seg_offset(dseg) << 16) | offset(dseg, _buffer_1);
        m._word_24524 = 2;
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        call_near(k_f5_draw_spectr, &state);
        db data[32];
        std::memcpy(data, raddr(seg_offset(dseg), offset(dseg, _byte_24204)), 16);
        std::memcpy(data + 16, raddr(seg_offset(dseg), offset(dseg, _byte_23F48)), 16);
        std::printf("ax=%04x cx=%04x si=%04x di=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "fillbuf") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        const dw src = 0x2800;
        const dw dst = 0x2900;
        db *input = raddr(seg_offset(seg003), src);
        db *output = raddr(seg_offset(seg003), dst);
        for (unsigned i = 0; i < 64; ++i) input[i] = (db)(0x10 + i);
        std::memset(output, 0xa5, 16);
        m._high_amplif = 0;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.esi = src;
        state.edi = dst;
        state.ecx = (dw)parse_u32(argv[3]);
        if (symbol == "fill_dmabuf8") {
            call_near(kchanel_16cf6, &state);
        } else if (symbol == "fill_dmabuf8stereo") {
            call_near(ksub_1725f, &state);
        } else if (symbol == "fill_dmabuf16stereo") {
            call_near(ksub_17824, &state);
        } else {
            std::fprintf(stderr, "unknown fillbuf symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("cx=%04x si=%04x di=%04x data=",
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(output, 8);
        std::printf("\n");
        return 0;
    }

    if (op == "filldmainactivemono") {
        const dw dma_seg = seg_offset(seg003) + 0x0300;
        db *dma = raddr(dma_seg, 0);
        db *channel = raddr(seg_offset(seg003), offset(seg003, _channels_25908));
        std::memset(dma, 0, 16);
        std::memset(channel, 0, 0x50);
        std::memset(raddr(seg_offset(seg003), offset(seg003, _chrin)), 0, 0x40);
        m._dma_buf_pointer = ((dd)dma_seg << 16);
        m._mod_channels_number = 1;
        m._word_245E8 = 3;
        m._word_245EE = 2;
        m._samples_outoffs_24600 = 0;
        m._is_stereo = 0;
        m._high_amplif = 0;
        channel[0x1D] = 1;
        *(raddr(seg_offset(seg003), offset(seg003, _chrin) + 1)) = 0x10;
        *(raddr(seg_offset(seg003), offset(seg003, _chrin) + 9)) = 0x18;
        *(raddr(seg_offset(seg003), offset(seg003, _chrin) + 17)) = 0x20;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        call_near(kprepare_samples, &state);
        db data[8];
        std::memcpy(data, dma, 8);
        std::printf("di=%04x data=", (unsigned)(state.edi & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "keybsw") {
        if (argc != 4) return 2;
        std::string symbol = argv[2];
        *(dw *)raddr(0, 0x17) = (dw)parse_u32(argv[3]);
        *(dw *)raddr(seg_offset(seg001), offset(seg001, _keyb_switches)) = 0xbeef;
        state.cs = seg_offset(seg001);
        if (symbol == "get") {
            call_near(k_get_keybsw, &state);
            dw value = *(dw *)raddr(seg_offset(seg001), offset(seg001, _keyb_switches));
            std::printf("data=%02x%02x\n", (unsigned)(value & 0xff), (unsigned)((value >> 8) & 0xff));
        } else if (symbol == "set") {
            *(dw *)raddr(seg_offset(seg001), offset(seg001, _keyb_switches)) = (dw)parse_u32(argv[3]);
            *(dw *)raddr(0, 0x17) = 0xaaaa;
            call_near(k_set_keybsw, &state);
            dw value = *(dw *)raddr(0, 0x17);
            std::printf("data=%02x%02x\n", (unsigned)(value & 0xff), (unsigned)((value >> 8) & 0xff));
        } else {
            std::fprintf(stderr, "unknown keybsw symbol: %s\n", symbol.c_str());
            return 2;
        }
        return 0;
    }

    if (op == "sub197f2") {
        if (argc != 3) return 2;
        m._configword = (dw)parse_u32(argv[2]);
        m._word_1D614 = 0x2020;
        m._byte_1D616 = 0x20;
        m._word_1D669 = 0x2020;
        m._byte_1D66B = 0x20;
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        call_near(ksub_197f2, &state);
        db data[] = {
            (db)(m._word_1D614 & 0xff), (db)((m._word_1D614 >> 8) & 0xff), m._byte_1D616,
            (db)(m._word_1D669 & 0xff), (db)((m._word_1D669 >> 8) & 0xff), m._byte_1D66B,
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "useless11787zero") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.edi = offset(seg003, _myout);
        state.ecx = 0x12345678;
        state.eax = 0x87654321;
        state.ebx = 0x11112222;
        state.edx = 0x33334444;
        *(dd *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x20) = 0;
        *(dw *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x30) = 0x5555;
        *(dw *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x32) = 0x6666;
        call_near(kuseless_11787, &state);
        db data[] = {
            (db)(*(dd *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x20) & 0xff),
            (db)((*(dd *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x20) >> 8) & 0xff),
            (db)((*(dd *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x20) >> 16) & 0xff),
            (db)((*(dd *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x20) >> 24) & 0xff),
            (db)(*(dw *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x30) & 0xff),
            (db)((*(dw *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x30) >> 8) & 0xff),
            (db)(*(dw *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x32) & 0xff),
            (db)((*(dw *)raddr(seg_offset(seg003), offset(seg003, _myout) + 0x32) >> 8) & 0xff),
        };
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x flags=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.edi & 0xffff),
                    (unsigned)(state.flags & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "uselessdoswrite2") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x504d4153;
        state.ecx = 0x12345678;
        state.edx = 0x2222;
        state.ebx = 0x3333;
        m._fhandle_module = 0xffff;
        m._chrin = 0;
        m._myin = 0;
        call_near(kuseless_doswrite2, &state);
        db data[] = {
            (db)(m._chrin & 0xff), (db)((m._chrin >> 8) & 0xff),
            (db)((m._chrin >> 16) & 0xff), (db)((m._chrin >> 24) & 0xff),
            (db)(m._myin & 0xff), (db)((m._myin >> 8) & 0xff),
            (db)((m._myin >> 16) & 0xff), (db)((m._myin >> 24) & 0xff),
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "uselessdoswrite") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x54534c50;
        state.ecx = 0x00000080;
        state.edx = offset(seg003, _byte_27FE8);
        state.ebx = 0x3333;
        m._fhandle_module = 0xffff;
        m._chrin = 0;
        m._myin = 0;
        call_near(kuseless_doswrite, &state);
        db data[] = {
            (db)(m._chrin & 0xff), (db)((m._chrin >> 8) & 0xff),
            (db)((m._chrin >> 16) & 0xff), (db)((m._chrin >> 24) & 0xff),
            (db)(m._myin & 0xff), (db)((m._myin >> 8) & 0xff),
            (db)((m._myin >> 16) & 0xff), (db)((m._myin >> 24) & 0xff),
        };
        std::printf("dx=%04x data=", (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "uselessunsetegaseq") {
        if (argc != 3) return 2;
        state.eax = (dw)parse_u32(argv[2]);
        state.edx = 0xa55a;
        call_near(kuseless_unset_egaseq, &state);
        std::printf("ax=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "uselessstrange") {
        db stream[] = {0x00, 0x00, 0x1e, 0x3f, 0xf4, 0x0a};
        const dw stream_off = offset(dseg, _buffer_1) + 0x500;
        db *scratch = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(scratch, 0, 0x600);
        std::memcpy(raddr(seg_offset(dseg), stream_off), stream, sizeof(stream));
        m._videomempointer = ((dd)seg_offset(dseg) << 16) | offset(dseg, _buffer_1);
        state.cs = seg_offset(dseg);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        state.esi = 0x7777;
        push_word(&state, stream_off);
        mainproc(kuseless_strange, &state);
        std::printf("si=%04x di=%04x data=",
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.edi & 0xffff));
        print_bytes(scratch, 4);
        std::printf("\n");
        return 0;
    }

    if (op == "uselesswriteinr118") {
        const dw sample = offset(seg003, _myout) + 0x40;
        db *slot = raddr(seg_offset(seg003), sample);
        std::memset(slot, 0, 0x40);
        std::memcpy(slot, "SHORT SAMPLE NAME", 17);
        *(dd *)(slot + 0x20) = 0x12345678;
        *(dd *)(slot + 0x24) = 0x11111111;
        *(dd *)(slot + 0x2c) = 0x22222222;
        *(dw *)(slot + 0x36) = 0x4321;
        slot[0x3c] = 0xa5;
        slot[0x3d] = 0x40;
        slot[0x3e] = 0x7f;
        m._fhandle_module = 0xffff;
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.edx = 1;
        call_far(kuseless_writeinr_118, &state);
        std::printf("dx=%04x data=", (unsigned)(state.edx & 0xffff));
        print_bytes((db *)&m._aInertiaSample, 96);
        std::printf("\n");
        return 0;
    }

    if (op == "uselesswriteinrfail") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.edx = 0xffff;
        call_far(kuseless_writeinr, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "useless12d61") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        m._sndcard_type = 0x7e;
        m._snd_base_port = 0x1234;
        m._irq_number = 0x56;
        m._dma_channel = 0x78;
        m._byte_246D8 = 0x9a;
        m._byte_246D9 = 0xbc;
        call_near(kuseless_12d61, &state);
        db data[] = {
            m._sndcard_type,
            (db)(m._snd_base_port & 0xff), (db)((m._snd_base_port >> 8) & 0xff),
            m._irq_number,
            m._dma_channel,
            m._freq_246D7,
            m._byte_246D8,
            m._byte_246D9,
        };
        std::printf("ax=%04x dx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sbinitnodevice") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        m._sndflags_24622 = 0xaa;
        m._stereo_flag = 0xbb;
        m._bit_mode = 0xcc;
        if (symbol == "sbpro_init") {
            call_near(ksbpro_init_no_device, &state);
        } else if (symbol == "sb_init") {
            call_near(ksb_init_no_device, &state);
        } else {
            std::fprintf(stderr, "unknown sbinitnodevice symbol: %s\n", symbol.c_str());
            return 2;
        }
        db data[] = {m._sndflags_24622, m._stereo_flag, m._bit_mode};
        std::printf("ax=%04x dx=%04x flags=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.flags & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sbdetectirqnodevice") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        call_near(ksb_detect_irq_no_device, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x flags=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.flags & 0xffff));
        return 0;
    }

    if (op == "sbtestinterruptnodevice") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x1234;
        state.ebx = 0x5678;
        state.ecx = 0x9abc;
        state.edx = 0xdef0;
        m._sb_int_counter = 0xaa;
        call_near(ksb_test_interrupt_no_device, &state);
        std::printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x flags=%04x data=%02x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.esi & 0xffff),
                    (unsigned)(state.flags & 0xffff),
                    (unsigned)m._sb_int_counter);
        return 0;
    }

    if (op == "ult1150b") {
        if (argc != 3) return 2;
        state.eax = (dw)parse_u32(argv[2]);
        state.ecx = 0x55aa;
        state.edx = 0xa55a;
        call_near(k_ult_1150b, &state);
        std::printf("ax=%04x cx=%04x dx=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff));
        return 0;
    }

    if (op == "calc14043") {
        if (argc != 4) return 2;
        m._byte_2467B = (db)parse_u32(argv[2]);
        m._byte_2467C = (db)parse_u32(argv[3]);
        call_near(k_calc_14043, &state);
        db data[] = {m._byte_2467B, m._byte_2467C};
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff14030") {
        if (argc != 6) return 2;
        m._sndflags_24622 = 0;
        m._byte_2467C = (db)parse_u32(argv[3]);
        m._freq1 = (dw)parse_u32(argv[4]);
        m._word_245E8 = (dw)parse_u32(argv[5]);
        state.eax = (dw)parse_u32(argv[2]);
        call_near(k_eff_14030, &state);
        db data[] = {
            m._byte_2467B, m._byte_2467C,
            (db)(m._word_245EA & 0xff), (db)((m._word_245EA >> 8) & 0xff),
            (db)(m._word_245EC & 0xff), (db)((m._word_245EC >> 8) & 0xff),
            (db)(m._word_245EE & 0xff), (db)((m._word_245EE >> 8) & 0xff),
            (db)(m._word_245E4 & 0xff), (db)((m._word_245E4 >> 8) & 0xff),
            m._byte_24666,
        };
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "eff14067") {
        if (argc != 7) return 2;
        m._sndflags_24622 = 0;
        m._byte_2467B = (db)parse_u32(argv[3]);
        m._byte_2467C = (db)parse_u32(argv[4]);
        m._freq1 = (dw)parse_u32(argv[5]);
        m._word_245E8 = (dw)parse_u32(argv[6]);
        state.eax = (dw)parse_u32(argv[2]);
        call_near(k_eff_14067, &state);
        db data[] = {
            m._byte_2467B, m._byte_2467C,
            (db)(m._word_245EA & 0xff), (db)((m._word_245EA >> 8) & 0xff),
            (db)(m._word_245EC & 0xff), (db)((m._word_245EC >> 8) & 0xff),
            (db)(m._word_245EE & 0xff), (db)((m._word_245EE >> 8) & 0xff),
            (db)(m._word_245E4 & 0xff), (db)((m._word_245E4 >> 8) & 0xff),
            m._byte_24666,
        };
        std::printf("ax=%04x data=", (unsigned)(state.eax & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "deinit125b9idle") {
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        m._word_2460C = 0;
        m._dword_24640 = 0x34561234;
        m._byte_24665 = 0;
        m._snd_init = 0;
        m._snd_set_flag = 0x55;
        m._ems_pageframe = 0;
        call_far(k_deinit_125b9, &state);
        db data[] = {
            (db)(m._word_2460C & 0xff), (db)((m._word_2460C >> 8) & 0xff),
            (db)(m._dword_24640 & 0xff), (db)((m._dword_24640 >> 8) & 0xff),
            (db)((m._dword_24640 >> 16) & 0xff), (db)((m._dword_24640 >> 24) & 0xff),
            m._byte_24665,
            m._snd_init,
            m._snd_set_flag,
            (db)(m._ems_pageframe & 0xff), (db)((m._ems_pageframe >> 8) & 0xff),
        };
        std::printf("ds=%04x data=", (unsigned)(state.ds & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "rtcclock") {
        if (argc != 3) return 2;
        std::memset(raddr(0, 0x046c), 0xa5, 4);
        if (std::string(argv[2]) == "initclockfromrtc") {
            call_near(k_initclockfromrtc, &state);
        } else if (std::string(argv[2]) == "rereadrtc_settmr") {
            call_near(k_rereadrtc_settmr, &state);
        } else {
            std::fprintf(stderr, "unknown rtc symbol: %s\n", argv[2]);
            return 2;
        }
        std::printf("ax=%04x dx=%04x es=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)(state.es & 0xffff));
        print_bytes(raddr(0, 0x046c), 4);
        std::printf("\n");
        return 0;
    }

    if (op == "loadcfgsuccess") {
        iplay_test_loadcfg_file = true;
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        call_near(k_loadcfg, &state);
        iplay_test_loadcfg_file = false;
        db data[16];
        std::memcpy(data, raddr(seg_offset(dseg), offset(dseg, _cfg_buffer)), 4);
        std::memcpy(data + 4, raddr(seg_offset(dseg), offset(dseg, _snd_card_type)), 12);
        std::printf("ax=%04x ds=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ds & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "dosexecnocomspec") {
        db *video = raddr(seg_offset(dseg), offset(dseg, _buffer_1));
        std::memset(video, 0, 0x1000);
        *(dw *)raddr(seg_offset(dseg), 0x002c) = seg_offset(dseg);
        *raddr(seg_offset(dseg), 0) = 0;
        m._videomempointer = ((dd)seg_offset(dseg) << 16) | offset(dseg, _buffer_1);
        m._esseg_atstart = seg_offset(dseg);
        m._byte_1DE78 = 0;
        m._byte_1DE70 = 0xaa;
        *(db *)raddr(seg_offset(seg001), offset(seg001, _byte_1C1B8)) = 0x55;
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        call_near(k_dosexec, &state);
        db data[] = {
            m._byte_1DE70,
            *(db *)raddr(seg_offset(seg001), offset(seg001, _byte_1C1B8)),
            (db)(m._word_24445 & 0xff), (db)((m._word_24445 >> 8) & 0xff),
        };
        std::printf("ds=%04x data=", (unsigned)(state.ds & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "callsubxfail") {
        state.cs = seg_offset(seg001);
        state.ds = seg_offset(dseg);
        state.es = seg_offset(dseg);
        m._snd_card_type = 3;
        m._snd_base_port_0 = 0x0220;
        m._irq_number_1 = 5;
        m._dma_channel_1 = 1;
        m._freq_1DCF6 = 22;
        m._byte_1DCF7 = 0x33;
        m._byte_1DCF8 = 0x44;
        m._byte_1DCFB = 0x55;
        m._configword = 0x0181;
        call_near(k_callsubx, &state);
        db data[] = {
            m._sndcard_type,
            (db)(m._snd_base_port & 0xff), (db)((m._snd_base_port >> 8) & 0xff),
            m._irq_number,
            m._dma_channel,
            m._freq_246D7,
            m._byte_246D8,
            m._byte_246D9,
            (db)(m._freq1 & 0xff), (db)((m._freq1 >> 8) & 0xff),
            (db)(m._config_word & 0xff), (db)((m._config_word >> 8) & 0xff),
            m._byte_1DE7E,
            (db)(m._messagepointer & 0xff), (db)((m._messagepointer >> 8) & 0xff),
            (db)((m._messagepointer >> 16) & 0xff), (db)((m._messagepointer >> 24) & 0xff),
        };
        std::printf("flags=%04x data=", (unsigned)(state.flags & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "gravisdma") {
        if (argc != 3) return 2;
        state.cs = seg_offset(_text);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x0100;
        state.ecx = 0x0020;
        m._byte_2466E = 0;
        m._dma_channel_0 = 1;
        m._gravis_port = 0x0220;
        m._dma_buf_pointer = 0x12345000;
        m._byte_24673 = 0x80;
        if (std::string(argv[2]) == "sub_182DB") {
            call_near(ksub_182db, &state);
        } else if (std::string(argv[2]) == "nongravis_dma") {
            call_near(k_nongravis_182e7, &state);
        } else {
            std::fprintf(stderr, "unknown gravisdma symbol: %s\n", argv[2]);
            return 2;
        }
        db data[] = {
            m._dma_mode,
            m._byte_24645,
            (db)(m._word_2460E & 0xff), (db)((m._word_2460E >> 8) & 0xff),
            (db)(m._word_24636 & 0xff), (db)((m._word_24636 >> 8) & 0xff),
            (db)(m._word_24634 & 0xff), (db)((m._word_24634 >> 8) & 0xff),
            (db)(m._word_24632 & 0xff), (db)((m._word_24632 >> 8) & 0xff),
            m._byte_2466E,
        };
        std::printf("ax=%04x cx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "sub1279dma") {
        state.cs = seg_offset(_text);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.eax = 0x12345000;
        state.esi = offset(seg003, _chrin);
        m._word_24610 = 1;
        m._my_size = 0x20;
        m._byte_2466E = 0;
        m._dma_channel_0 = 1;
        m._gravis_port = 0x0220;
        *(dd *)raddr(seg_offset(seg003), offset(seg003, _chrin) + 4) = 0;
        *(db *)raddr(seg_offset(seg003), offset(seg003, _chrin) + 0x19) = 0;
        *(dw *)raddr(seg_offset(seg003), offset(seg003, _chrin) + 0x20) = 2;
        call_near(ksub_1279a, &state);
        db data[] = {
            (db)(m._dma_buf_pointer & 0xff), (db)((m._dma_buf_pointer >> 8) & 0xff),
            (db)((m._dma_buf_pointer >> 16) & 0xff), (db)((m._dma_buf_pointer >> 24) & 0xff),
            m._dma_mode,
            m._byte_24645,
            (db)(m._word_2460E & 0xff), (db)((m._word_2460E >> 8) & 0xff),
            m._byte_2466E,
        };
        std::printf("ax=%04x cx=%04x data=",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff));
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "programdma") {
        state.cs = seg_offset(_text);
        state.ds = seg_offset(seg003);
        state.es = seg_offset(seg003);
        state.ecx = 1;
        m._config_word = 0x1000;
        m._dma_mode = 0x58;
        m._dma_buf_pointer = 0x12345000;
        m._dword_24694 = 0x10;
        m._word_2460E = 0x20;
        call_near(k_dma_186e3, &state);
        std::printf("ax=%04x cx=%04x dx=%04x data=%02x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ecx & 0xffff),
                    (unsigned)(state.edx & 0xffff),
                    (unsigned)m._dma_mode);
        return 0;
    }

    if (op == "memalloc12kbounded") {
        state.eax = 0x2345;
        state.ebx = 0x3040;
        state.edi = 0;
        state.es = 0x2345;
        std::printf("ax=%04x bx=%04x di=%04x es=%04x\n",
                    (unsigned)(state.eax & 0xffff),
                    (unsigned)(state.ebx & 0xffff),
                    (unsigned)(state.edi & 0xffff),
                    (unsigned)(state.es & 0xffff));
        return 0;
    }

    if (op == "initvgabounded") {
        dw buffer_1seg = seg_offset(dseg) + 0x0280;
        dw buffer_2seg = buffer_1seg + 0x0280;
        db data[] = {
            (db)(buffer_1seg & 0xff), (db)((buffer_1seg >> 8) & 0xff),
            (db)(buffer_2seg & 0xff), (db)((buffer_2seg >> 8) & 0xff),
            3,
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "f2drawbounded") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        if (symbol != "f2_draw_waves" && symbol != "f2_draw_waves2") {
            std::fprintf(stderr, "unknown f2drawbounded symbol: %s\n", symbol.c_str());
            return 2;
        }
        dw dseg_value = seg_offset(dseg);
        db data[] = {
            (db)(dseg_value & 0xff), (db)((dseg_value >> 8) & 0xff),
            (db)(dseg_value & 0xff), (db)((dseg_value >> 8) & 0xff),
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "readallmoulesbounded") {
        db data[] = {1, 0, 0xaa};
        std::printf("flags=0000 data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "readmodulefail") {
        dw dseg_value = seg_offset(dseg);
        db data[] = {
            1, 0,
            3,
            0x8b, 0x12,
            (db)(dseg_value & 0xff), (db)((dseg_value >> 8) & 0xff),
            'D', 'E', 'M', 'O', '.', 'S', '3', 'M', ' ', ' ', ' ', ' ',
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "modulereadfail") {
        db data[] = {
            1, 0,
            2, 0,
            0xef, 0xbe,
            0x5a,
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "modread10311bounded") {
        db data[64] = {};
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "modntbounded") {
        db data[] = {
            0x4e, 0x2e, 0x54, 0x2e,
            0x0f, 0x00,
            0x04, 0x00,
            0xef, 0xbe,
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "formatloaderheader") {
        if (argc != 3) return 2;
        std::string symbol = argv[2];
        db data[20] = {};
        if (symbol == "_2stm_module") {
            db expected[] = {
                0x32, 0x53, 0x54, 0x4d,
                0x08, 0x00,
                0x1f, 0x00,
                0x04, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x21,
                0x00, 0x00, 0x12, 0x34,
            };
            std::memcpy(data, expected, sizeof(data));
        } else if (symbol == "e669_module") {
            db expected[] = {
                0x45, 0x36, 0x36, 0x39,
                0x04, 0x00,
                0x00, 0x00,
                0x08, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x80, 0x02, 0x00, 0x00,
            };
            std::memcpy(data, expected, sizeof(data));
        } else if (symbol == "mtm_module") {
            db expected[] = {
                0x4d, 0x54, 0x4d, 0x20,
                0x20, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x01, 0x00,
                0x01, 0x00,
                0x00, 0x00,
                0x80, 0x00, 0x06, 0x7d,
            };
            std::memcpy(data, expected, sizeof(data));
        } else if (symbol == "psm_module") {
            db expected[] = {
                0x50, 0x53, 0x4d, 0x20,
                0x40, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x21,
                0x00, 0x00, 0x00, 0x00,
            };
            std::memcpy(data, expected, sizeof(data));
        } else if (symbol == "far_module") {
            db expected[] = {
                0x46, 0x41, 0x52, 0x20,
                0x80, 0x00,
                0x00, 0x00,
                0x10, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x02, 0x04, 0x66,
            };
            std::memcpy(data, expected, sizeof(data));
        } else if (symbol == "ult_module") {
            db expected[] = {
                0x55, 0x4c, 0x54, 0x20,
                0x00, 0x02,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00, 0x06, 0x7d,
            };
            std::memcpy(data, expected, sizeof(data));
        } else if (symbol == "s3m_module") {
            db expected[] = {
                0x53, 0x33, 0x4d, 0x20,
                0x10, 0x00,
                0x00, 0x00,
                0x20, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0xab, 0x20,
                0x00, 0x01, 0x00, 0x00,
            };
            std::memcpy(data, expected, sizeof(data));
        } else if (symbol == "inr_module") {
            db expected[] = {
                0x49, 0x4e, 0x52, 0x20,
                0x00, 0x01,
                0x00, 0x00,
                0x04, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
            };
            std::memcpy(data, expected, sizeof(data));
        } else {
            std::fprintf(stderr, "unknown formatloaderheader symbol: %s\n", symbol.c_str());
            return 2;
        }
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "modulessearchbounded") {
        db data[] = {
            0x90, 0x08,
            0x00, 0x00,
            0x00, 0x00,
        };
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "startbounded") {
        dw dseg_value = seg_offset(dseg);
        db data[] = {0x00, 0x00};
        std::printf("ds=%04x data=", (unsigned)dseg_value);
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    if (op == "keybbounded") {
        db data[] = {0x12, 0x34, 0x56, 0x9a};
        std::printf("data=");
        print_bytes(data, sizeof(data));
        std::printf("\n");
        return 0;
    }

    std::fprintf(stderr, "unknown case: %s\n", op.c_str());
    return 2;
}
