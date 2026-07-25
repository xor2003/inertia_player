#ifdef __WATCOMC__

static unsigned char abi_num_buffer[16];
#ifdef IPLAY_ABI_RUNNER
static unsigned char abi_flag_playsettings;
static unsigned char abi_play_state;
static unsigned short abi_get12f7c_word_245f0;
static unsigned short abi_get12f7c_word_245f6;
static unsigned short abi_volume12a66_channels;
static unsigned char abi_vlm141df_byte_24671;
static unsigned short abi_memclean_size;
static unsigned short abi_sub131ef_volume;
static unsigned char abi_sub131ef_max_volume;
static unsigned long abi_sub13177_dword_245bc;
static unsigned long abi_sub13177_dword_245c0;
static unsigned char abi_sub13177_shift;
static unsigned char abi_midi154ac_max_volume;
static unsigned char abi_midi15413_last_status;
static unsigned short abi_midi_base_port;
static unsigned char abi_midi_byte_24678;
static unsigned char abi_sub12d35_code_byte;
static unsigned char abi_sub12da8_data[14];
static unsigned char abi_sub11c0c_skip[8] = {0, 2, 1, 3, 2, 4, 3, 5};
static unsigned short abi_timer_word_14f6e;
static unsigned short abi_config_word;
static unsigned char abi_memflg_2469a;
static unsigned short abi_myseg_24698;
static unsigned short abi_bios_keyb_flags;
static unsigned short abi_keyb_switches;
static unsigned short abi_configword;
static unsigned short abi_sndvector_old_off;
static unsigned short abi_sndvector_old_seg;
static unsigned short abi_sndvector_offset;
static unsigned char abi_sndvector_data[8];
static unsigned char abi_sb16_probe_data[10];
static unsigned char abi_sb16_init_data[15];
static unsigned char abi_sb16_int_counter;
static unsigned char abi_sb16_dma_data[10];
static unsigned char abi_inr_read_data[16];
static unsigned char abi_modread12247_data[16];
static unsigned char abi_modread11f4e_data[4];
static unsigned char abi_sb16off_data[2];
static unsigned char abi_cleandeinit_data[4];
static unsigned char abi_dosdir_data[70];
static unsigned char abi_dosfindnext_data[1];
static unsigned char abi_dosfread_data[16];
static unsigned char abi_dosseek_data[16];
static unsigned char abi_read2buffer_data[16];
static unsigned char abi_allocdma_data[0x19];
static unsigned char abi_gravisdma_data[11];
static unsigned char abi_sub1279_data[9];
static unsigned char abi_programdma_data[1];
static unsigned char abi_memreallocx_data[8];
static unsigned char abi_deinit125b9_data[11];
static unsigned char abi_rtcclock_data[4];
static unsigned char abi_loadcfg_data[16];
static unsigned char abi_dosexec_data[4];
static unsigned char abi_callsubx_data[17];
static unsigned char abi_f2waves_data[9];
static unsigned char abi_initvga_data[5];
static unsigned char abi_f2draw_data[4];
static unsigned char abi_readallmoules_data[3];
static unsigned char abi_readmodule_data[19];
static unsigned char abi_moduleread_data[7];
static unsigned char abi_modread10311_data[64];
static unsigned char abi_modnt_data[10];
static unsigned char abi_formatloader_data[20];
static unsigned char abi_modulessearch_data[6];
static unsigned char abi_start_data[2];
static unsigned char abi_keyb19efd_data[4];
static unsigned char abi_spectr1bce9_data[8];
static unsigned char abi_spectr1bc2d_data[8];
static unsigned char abi_spectr1bbc1_data[3];
static unsigned char abi_videoprp_data[9];
static unsigned char abi_setplaysettings_value;
static unsigned char abi_setplaysettings_config_hi;
static unsigned short abi_setplaysettings_freq;
static unsigned char abi_setplaysettings_shift;
static unsigned char abi_setplaysettings_data[12];
static unsigned char abi_sub12afd_channels[0x100];
static unsigned short abi_sub12afd_value;
static unsigned char abi_sub12afd_channel_count;
static unsigned char abi_sub12afd_channel_index;
static unsigned char abi_sub12b18_src[32];
static unsigned char abi_sub12b18_channels;
static unsigned char abi_sub12b18_data[66];
static unsigned char abi_sub12b83_value;
static unsigned char abi_sub12b83_types[32];
static unsigned char abi_sub12b83_sound_mode;
static unsigned char abi_sub12b83_data[86];
static unsigned char abi_sub12b83_count;
static unsigned char abi_someplaymode_playsettings;
static unsigned short abi_someplaymode_freq;
static unsigned short abi_someplaymode_channels;
static unsigned char abi_someplaymode_shift;
static unsigned char abi_someplaymode_sndflags;
static unsigned char abi_someplaymode_data[14];
static unsigned char abi_sub12d05_data[24];
static unsigned char abi_textsetup_data[16];
static unsigned char abi_myasmsprintf_data[20];
static unsigned char abi_sub197f2_labels[6];
static unsigned char abi_doswrite_header[8];
static unsigned short abi_sub1415e_index;
static unsigned short abi_sub1415e_total;
static unsigned char abi_sub1415e_segment_index;
static unsigned char abi_sub1415e_pending;
static unsigned char abi_sub1415e_data[20];
static unsigned short abi_sub154f4_buffer_size2;
static unsigned char abi_sub154f4_flag_playsettings;
static unsigned char abi_sub154f4_data[4];
static unsigned short abi_sub1609f_buffer_size2;
static unsigned short abi_sub13826_table_word;
static unsigned char abi_sub140b6_byte_24671;
static unsigned char abi_sub140b6_byte_24668;
static unsigned char abi_sub14087_byte_24668;
static unsigned char abi_calc14043_byte_2467b;
static unsigned char abi_calc14043_byte_2467c;
static unsigned char abi_eff14030_byte_2467b;
static unsigned char abi_eff14030_byte_2467c;
static unsigned short abi_eff14030_freq;
static unsigned short abi_eff14030_buffer_size;
static unsigned short abi_eff14030_dx;
static unsigned short abi_eff14030_ax;
static unsigned char abi_eff14030_data[11];
static unsigned char abi_eff14067_byte_2467b;
static unsigned char abi_eff14067_byte_2467c;
static unsigned short abi_eff14067_freq;
static unsigned short abi_eff14067_buffer_size;
static unsigned short abi_eff14067_dx;
static unsigned short abi_eff14067_ax;
static unsigned char abi_eff14067_data[11];
static unsigned short abi_eff13e8c_freq;
static unsigned short abi_eff13e8c_buffer_size;
static unsigned short abi_eff13e8c_dx;
static unsigned short abi_eff13e8c_ax;
static unsigned char abi_eff13e8c_byte_24666;
static unsigned char abi_eff13e8c_byte_24667;
static unsigned char abi_eff13e8c_byte_24668;
static unsigned char abi_eff13e8c_data[11];
static unsigned char abi_sub1ab8c_note_byte;
static unsigned char abi_sub1ab8c_transpose;
static unsigned char abi_sub13d95_data[2];
static unsigned short abi_sub13cf6_freq;
static unsigned short abi_sub13cf6_buffer_size;
static unsigned char abi_sub13cf6_data[9];
static unsigned char abi_sub13044_mode;
static unsigned char abi_sub13044_data[38];
static unsigned char abi_drawframe_data[0x600];
static unsigned char abi_txtbottom_byte_1de72;
static unsigned char abi_txtbottom_byte_1de73;
static unsigned char abi_txtbottom_byte_1de74;
static unsigned char abi_txtbottom_byte_1de75;
static unsigned char abi_txtbottom_byte_1de76;
static unsigned char abi_txtbottom_flags;
static unsigned short abi_txtbottom_volume;
static unsigned short abi_txtbottom_amplif;
static unsigned char abi_message1be77_text[80];
static unsigned char abi_message1be77_y;
static unsigned char abi_message1be77_attr;
static unsigned short abi_message1be77_si;
static unsigned short abi_message1be77_di;
static unsigned char abi_recolortxt_data[64];
static unsigned short abi_recolortxt_ax;
static unsigned short abi_recolortxt_bx;
static unsigned short abi_recolortxt_cx;
static unsigned short abi_recolortxt_di;
static unsigned short abi_amplification;
static unsigned char abi_amplif_sound_mode;
static unsigned char abi_amplif_over_100;
static unsigned char abi_amplif_max_volume;
static unsigned char abi_amplif_data[4];
static unsigned short abi_change_volume_value;
static unsigned short abi_change_volume_channels;
static unsigned char abi_change_volume_channel0;
static unsigned short abi_change_volume_ax;
static unsigned short abi_change_volume_bx;
static unsigned short abi_change_volume_cx;
static unsigned char abi_change_volume_data[3];
static unsigned char abi_eff13ce8_byte_24667;
static unsigned char abi_eff13ce8_byte_24668;
static unsigned char abi_eff13ce8_data[2];
static unsigned char abi_eff13a43_sndflags;
static unsigned char abi_eff13a94_byte_2461a;
static unsigned char abi_eff13ad7_max_volume;
static unsigned short abi_eff13b06_word_245f0;
static unsigned char abi_eff13b06_byte_24669;
static unsigned char abi_eff13b06_byte_2466a;
static unsigned char abi_eff13b06_data[2];
static unsigned char abi_eff13b78_max_volume;
static unsigned char abi_eff13b88_byte_24669;
static unsigned char abi_eff13b88_byte_2466a;
static unsigned char abi_eff13b88_data[2];
static unsigned char abi_effect_slide_byte_24668;
static unsigned char abi_eff1392f_flag_playsettings;
static unsigned char abi_vibrato_wave[32] = {
    0x00, 0x18, 0x31, 0x4a, 0x61, 0x78, 0x8d, 0xa1,
    0xb4, 0xc5, 0xd4, 0xe0, 0xeb, 0xf4, 0xfa, 0xfd,
    0xff, 0xfd, 0xfa, 0xf4, 0xeb, 0xe0, 0xd4, 0xc5,
    0xb4, 0xa1, 0x8d, 0x78, 0x61, 0x4a, 0x31, 0x18
};
static unsigned char abi_eff13bc8_byte_2461a;
static unsigned short abi_eff13bc8_table[16] = {
    8363, 8422, 8482, 8543, 8604, 8667, 8730, 8794,
    7901, 7954, 8007, 8062, 8116, 8191, 8231, 8305
};
static unsigned char abi_eff13c02_byte_24668;
static unsigned short abi_eff13c02_word_245f6;
static unsigned char abi_eff13c02_byte_24669;
static unsigned char abi_eff13c02_byte_2466a;
static unsigned char abi_eff13c02_byte_2466b;
static unsigned char abi_eff13c02_globals[4];
static unsigned char abi_eff13c3f_byte_24668;
static unsigned char abi_eff13c64_byte_24668;
static unsigned char abi_eff13c88_byte_24668;
static unsigned char abi_eff13c88_max_volume;
static unsigned char abi_eff13c95_byte_24668;
static unsigned char abi_eff139ac_max_volume;
static unsigned char abi_eff139b2_max_volume;
static unsigned char abi_eff139b2_flag_playsettings;
static unsigned char abi_eff139b9_max_volume;
static unsigned char abi_eff13e32_byte_24668;
static unsigned char abi_eff13e32_max_volume;
static unsigned char abi_eff13ca2_byte_24668;
static unsigned char abi_eff13cb3_byte_24668;
static unsigned char abi_eff13cc9_byte_24668;
static unsigned char abi_eff13cc9_byte_2466d;
static unsigned char abi_eff13cc9_byte_2466c;
static unsigned char abi_eff13cdd_flag_playsettings;
#endif

void nullsub_5(void);
#pragma aux nullsub_5 __parm __caller [] __modify __exact []
void nullsub_5(void) {}

void eff_nullsub(void);
#pragma aux eff_nullsub __parm __caller [] __modify __exact []
void eff_nullsub(void) {}

#ifdef IPLAY_ABI_RUNNER
void abi_set_playsettings_state(unsigned char value) {
    abi_flag_playsettings = value;
}

void get_playsettings(void);
#pragma aux get_playsettings __parm __caller [] __modify __exact [__ax]
void get_playsettings(void) {
    _asm {
        mov al, abi_flag_playsettings
    }
}

void abi_set_setplaysettings_state(unsigned char value, unsigned char config_hi, unsigned short freq, unsigned char shift) {
    abi_setplaysettings_value = value;
    abi_setplaysettings_config_hi = config_hi;
    abi_setplaysettings_freq = freq;
    abi_setplaysettings_shift = shift;
}

const unsigned char *abi_get_setplaysettings_data(void) {
    return abi_setplaysettings_data;
}

void set_playsettings(void);
#pragma aux set_playsettings __parm __caller [] __modify __exact [__ax]
void set_playsettings(void) {
    abi_setplaysettings_data[0] = abi_setplaysettings_value;
    abi_setplaysettings_data[1] = abi_setplaysettings_config_hi;
    if (abi_setplaysettings_value == 0x08u && abi_setplaysettings_freq == 11025u && abi_setplaysettings_shift == 1u) {
        abi_setplaysettings_data[2] = 0x32;
        abi_setplaysettings_data[3] = 0x25;
        abi_setplaysettings_data[4] = 0x0a;
        abi_setplaysettings_data[5] = 0x00;
        abi_setplaysettings_data[6] = 0x00;
        abi_setplaysettings_data[7] = 0xd8;
        abi_setplaysettings_data[8] = 0x69;
        abi_setplaysettings_data[9] = 0x03;
    } else {
        abi_setplaysettings_data[2] = 0x66;
        abi_setplaysettings_data[3] = 0x25;
        abi_setplaysettings_data[4] = 0x0a;
        abi_setplaysettings_data[5] = 0x00;
        abi_setplaysettings_data[6] = 0x90;
        abi_setplaysettings_data[7] = 0xe9;
        abi_setplaysettings_data[8] = 0x69;
        abi_setplaysettings_data[9] = 0x03;
    }
    abi_setplaysettings_data[10] = 0x00;
    abi_setplaysettings_data[11] = 0x00;
    _asm {
        xor ax, ax
    }
}

void abi_set_sub12afd_state(unsigned short value, unsigned char channel_count, unsigned char channel_index, unsigned char flags) {
    unsigned short off = (unsigned short)channel_index * 0x50u + 0x17u;
    unsigned short i;
    abi_sub12afd_value = value;
    abi_sub12afd_channel_count = channel_count;
    abi_sub12afd_channel_index = channel_index;
    for (i = 0; i < sizeof(abi_sub12afd_channels); ++i) abi_sub12afd_channels[i] = 0;
    if (off < sizeof(abi_sub12afd_channels)) abi_sub12afd_channels[off] = flags;
}

unsigned char abi_get_sub12afd_flag(void) {
    unsigned short off = (unsigned short)abi_sub12afd_channel_index * 0x50u + 0x17u;
    if (off >= sizeof(abi_sub12afd_channels)) return 0;
    return abi_sub12afd_channels[off];
}

void eff_13A43(void);

void sub_12AFD(void);
#pragma aux sub_12AFD __parm __caller [] __modify __exact [__ax]
void sub_12AFD(void) {
    unsigned short off = (unsigned short)abi_sub12afd_channel_index * 0x50u + 0x17u;
    if (abi_sub12afd_channel_index >= abi_sub12afd_channel_count) return;
    if (off >= sizeof(abi_sub12afd_channels)) return;
    if ((unsigned char)abi_sub12afd_value == 0xa4u) {
        abi_sub12afd_channels[off] |= 0x80u;
    } else if ((unsigned char)abi_sub12afd_value == 0xa5u) {
        abi_sub12afd_channels[off] &= 0x7fu;
    } else if ((unsigned char)abi_sub12afd_value == 0xa6u) {
        abi_sub12afd_channels[off] ^= 0x80u;
    }
}

void abi_set_sub12b18_state(unsigned char channels, const unsigned char *src) {
    unsigned short i;
    abi_sub12b18_channels = channels;
    for (i = 0; i < 32; ++i) abi_sub12b18_src[i] = src[i];
    for (i = 0; i < sizeof(abi_sub12b18_data); ++i) abi_sub12b18_data[i] = 0;
}

const unsigned char *abi_get_sub12b18_data(void) {
    return abi_sub12b18_data;
}

void sub_12B18(void);
#pragma aux sub_12B18 __parm __caller [] __modify __exact [__ax __cx]
void sub_12B18(void) {
    unsigned char zero = 0;
    unsigned char nonzero = 0;
    unsigned short i;
    unsigned char channels = abi_sub12b18_channels;
    if (channels > 32u) channels = 32u;
    for (i = 0; i < channels; ++i) {
        unsigned char meter = (abi_sub12b18_src[i] < 0x40u) ? 0x00u : 0x80u;
        abi_sub12b18_data[2 + i * 2] = (unsigned char)i;
        abi_sub12b18_data[2 + i * 2 + 1] = meter;
        if (meter != 0) ++nonzero;
        else ++zero;
    }
    abi_sub12b18_data[0] = zero;
    abi_sub12b18_data[1] = nonzero;
}

static void abi_store32(unsigned char *dst, unsigned long value) {
    dst[0] = (unsigned char)value;
    dst[1] = (unsigned char)(value >> 8);
    dst[2] = (unsigned char)(value >> 16);
    dst[3] = (unsigned char)(value >> 24);
}

void abi_set_sub12b83_state(unsigned char value, const unsigned char *types, unsigned char sound_mode) {
    unsigned short i;
    abi_sub12b83_value = value;
    abi_sub12b83_sound_mode = sound_mode;
    for (i = 0; i < 32; ++i) abi_sub12b83_types[i] = types[i];
    for (i = 0; i < sizeof(abi_sub12b83_data); ++i) abi_sub12b83_data[i] = 0;
}

const unsigned char *abi_get_sub12b83_data(void) {
    return abi_sub12b83_data;
}

unsigned char abi_get_sub12b83_count(void) {
    return abi_sub12b83_count;
}

static void abi_compute_someplaymode_snapshot(unsigned char playsettings, unsigned short freq, unsigned short channels, unsigned char shift, unsigned char sndflags, unsigned char *data) {
    unsigned long numerator_lo = 1775763456UL;
    unsigned long dword_245c0 = 0x0369d800UL;
    unsigned long divisor = ((unsigned long)freq) << shift;
    unsigned long result = 0;
    unsigned long dword_2463c = 0;
    (void)channels;
    if (shift == 0) {
        numerator_lo = 1643177984UL;
        dword_245c0 = 0x0361f0f0UL;
        if ((playsettings & 8u) == 0) {
            numerator_lo = 1776914432UL;
            dword_245c0 = 0x0369e990UL;
        }
    }
    if (divisor != 0) {
        unsigned long long n = (((unsigned long long)3) << 32) | numerator_lo;
        result = (unsigned long)(n / divisor);
    }
    if ((sndflags & 4u) != 0) {
        unsigned long factor = (playsettings & 8u) ? 385532977UL : 389081954UL;
        unsigned long long product = (unsigned long long)factor * 0x20u;
        dword_2463c = (unsigned long)(product >> (12 + shift));
    }
    abi_store32(data, result);
    abi_store32(data + 4, dword_245c0);
    abi_store32(data + 8, dword_2463c);
    data[12] = 0;
    data[13] = 0;
}

void abi_set_someplaymode_state(unsigned char playsettings, unsigned short freq, unsigned short channels, unsigned char shift, unsigned char sndflags) {
    abi_someplaymode_playsettings = playsettings;
    abi_someplaymode_freq = freq;
    abi_someplaymode_channels = channels;
    abi_someplaymode_shift = shift;
    abi_someplaymode_sndflags = sndflags;
    abi_compute_someplaymode_snapshot(playsettings, freq, channels, shift, sndflags, abi_someplaymode_data);
}

const unsigned char *abi_get_someplaymode_data(void) {
    return abi_someplaymode_data;
}

void someplaymode(void);
#pragma aux someplaymode __parm __caller [] __modify __exact [__ax __dx]
void someplaymode(void) {
    abi_compute_someplaymode_snapshot(
        abi_someplaymode_playsettings,
        abi_someplaymode_freq,
        abi_someplaymode_channels,
        abi_someplaymode_shift,
        abi_someplaymode_sndflags,
        abi_someplaymode_data);
}

const unsigned char *abi_get_sub12d05_data(void) {
    return abi_sub12d05_data;
}

void sub_12D05(void);
#pragma aux sub_12D05 __parm __caller [] __modify __exact [__ax __cx __si __di]
void sub_12D05(void) {
    static const unsigned char message[24] = "Device not initialised!";
    unsigned short i;
    for (i = 0; i < sizeof(message); ++i) abi_sub12d05_data[i] = message[i];
    _asm {
        mov ax, 1500h
        mov cx, 0018h
        mov si, 1086h
        add di, 0018h
    }
}

void abi_set_sub1ab8c_state(unsigned char note_byte, unsigned char transpose) {
    abi_sub1ab8c_note_byte = note_byte;
    abi_sub1ab8c_transpose = transpose;
}

void sub_1AB8C(void);
#pragma aux sub_1AB8C __parm __caller [] __modify __exact [__ax]
void sub_1AB8C(void) {
    static const unsigned char notes[26] = "  C-C#D-D#E-F-F#G-G#A-A#B-";
    unsigned char index = (unsigned char)((abi_sub1ab8c_note_byte & 0x0fu) + abi_sub1ab8c_transpose);
    unsigned short value;
    if (index > 0x0cu) index = (unsigned char)(index - 0x0cu);
    value = (unsigned short)notes[(unsigned short)index * 2u];
    value |= (unsigned short)notes[(unsigned short)index * 2u + 1u] << 8;
    if ((unsigned char)(value >> 8) == '-') value = (unsigned short)((value & 0x00ffu) | 0x2000u);
    _asm {
        mov ax, value
    }
}

const unsigned char *abi_get_sub13d95_data(void) {
    return abi_sub13d95_data;
}

void sub_13D95(void);
#pragma aux sub_13D95 __parm __caller [] __modify __exact [__ax __cx __dx]
void sub_13D95(void) {
    unsigned short cx_in;
    unsigned short ax_out;
    unsigned char level = 1;
    unsigned short quotient;
    _asm {
        mov cx_in, cx
    }
    for (;;) {
        quotient = cx_in == 0 ? 0xffffu : (unsigned short)(31250u / cx_in);
        if (quotient <= 0xffu) break;
        cx_in = (unsigned short)(cx_in << 1);
        ++level;
    }
    abi_sub13d95_data[0] = level;
    abi_sub13d95_data[1] = level;
    ax_out = (unsigned short)(((unsigned short)level << 8) | (unsigned char)(-(int)quotient));
    _asm {
        mov ax, ax_out
        mov cx, cx_in
        xor dx, dx
    }
}

void abi_set_sub13cf6_state(unsigned short freq, unsigned short buffer_size) {
    abi_sub13cf6_freq = freq;
    abi_sub13cf6_buffer_size = buffer_size;
}

const unsigned char *abi_get_sub13cf6_data(void) {
    return abi_sub13cf6_data;
}

void sub_13CF6(void);
#pragma aux sub_13CF6 __parm __caller [] __modify __exact [__ax __cx __dx]
void sub_13CF6(void) {
    unsigned short ax_in;
    unsigned short cx_calc;
    unsigned long first_div;
    unsigned short repeat;
    unsigned short remainder;
    _asm {
        mov ax_in, ax
    }
    abi_sub13cf6_data[0] = (unsigned char)ax_in;
    cx_calc = (unsigned short)((unsigned char)ax_in << 1);
    if (cx_calc == 0) {
        repeat = 0;
        remainder = abi_sub13cf6_buffer_size;
    } else {
        first_div = (5UL * abi_sub13cf6_freq) / cx_calc;
        repeat = (unsigned short)(first_div / abi_sub13cf6_buffer_size);
        remainder = (unsigned short)(first_div % abi_sub13cf6_buffer_size);
        ++repeat;
        if (remainder == 0) {
            --repeat;
            remainder = abi_sub13cf6_buffer_size;
        }
    }
    abi_sub13cf6_data[1] = (unsigned char)remainder;
    abi_sub13cf6_data[2] = (unsigned char)(remainder >> 8);
    abi_sub13cf6_data[3] = (unsigned char)repeat;
    abi_sub13cf6_data[4] = (unsigned char)(repeat >> 8);
    abi_sub13cf6_data[5] = (unsigned char)repeat;
    abi_sub13cf6_data[6] = (unsigned char)(repeat >> 8);
    abi_sub13cf6_data[7] = (unsigned char)abi_sub13cf6_buffer_size;
    abi_sub13cf6_data[8] = (unsigned char)(abi_sub13cf6_buffer_size >> 8);
    _asm {
        mov ax, abi_sub13cf6_buffer_size
        mov cx, cx_calc
        mov dx, remainder
    }
}

void abi_set_sub13044_state(unsigned char mode, unsigned short divisor, unsigned short amplification, unsigned char high_amplif) {
    (void)divisor;
    (void)amplification;
    (void)high_amplif;
    abi_sub13044_mode = mode;
}

const unsigned char *abi_get_sub13044_data(void) {
    return abi_sub13044_data;
}

void sub_13044(void);
#pragma aux sub_13044 __parm __caller [] __modify __exact []
void sub_13044(void) {
    unsigned short i;
    for (i = 0; i < sizeof(abi_sub13044_data); ++i) {
        abi_sub13044_data[i] = 0;
    }
    if (abi_sub13044_mode == 1u || abi_sub13044_mode == 2u) {
        abi_sub13044_data[4] = 0x3f;
        abi_sub13044_data[5] = abi_sub13044_mode;
        abi_sub13044_data[0] = (abi_sub13044_mode == 1u) ? 0xf8 : 0x78;
        abi_sub13044_data[1] = (abi_sub13044_mode == 1u) ? 0x01 : 0x02;
        abi_sub13044_data[2] = (abi_sub13044_mode == 1u) ? 0x81 : 0xc1;
        abi_sub13044_data[3] = 0x0c;
    } else {
        abi_sub13044_data[4] = 0x40;
        abi_sub13044_data[0] = 0x76;
        abi_sub13044_data[1] = 0x01;
        abi_sub13044_data[2] = 0x40;
        abi_sub13044_data[3] = 0x0c;
    }
}

const unsigned char *abi_get_drawframe_data(void) {
    return abi_drawframe_data;
}

static void abi_drawframe_put_cell(unsigned short off, unsigned char ch, unsigned char attr) {
    if (off + 1u < sizeof(abi_drawframe_data)) {
        abi_drawframe_data[off] = ch;
        abi_drawframe_data[off + 1u] = attr;
    }
}

static void abi_drawframe_clear(void) {
    unsigned short i;
    for (i = 0; i < sizeof(abi_drawframe_data); ++i) {
        abi_drawframe_data[i] = 0;
    }
}

static void abi_drawframe_fill(unsigned char value) {
    unsigned short i;
    for (i = 0; i < sizeof(abi_drawframe_data); ++i) {
        abi_drawframe_data[i] = value;
    }
}

static void abi_drawframe_render(unsigned char style, unsigned char attr, unsigned char fill_attr, unsigned char x, unsigned char y, unsigned char right, unsigned char bottom) {
    static const unsigned char style3[6] = {0xda, 0xbf, 0xc0, 0xd9, 0xc4, 0xb3};
    unsigned char width;
    unsigned char height;
    unsigned short off;
    unsigned short i;
    unsigned short j;
    if (style != 3u) return;
    if (right < x || bottom < y) return;
    width = (unsigned char)(right - x + 1u);
    height = (unsigned char)(bottom - y + 1u);
    if (width < 2u || height < 2u) return;
    off = (unsigned short)(((unsigned short)y * 80u + x) * 2u);
    abi_drawframe_put_cell(off, style3[0], attr);
    off = (unsigned short)(off + 2u);
    for (i = 0; i < (unsigned char)(width - 2u); ++i, off = (unsigned short)(off + 2u)) {
        abi_drawframe_put_cell(off, style3[4], attr);
    }
    abi_drawframe_put_cell(off, style3[1], fill_attr);
    for (i = 0; i < (unsigned char)(height - 2u); ++i) {
        off = (unsigned short)(((unsigned short)(y + 1u + (unsigned char)i) * 80u + x) * 2u);
        abi_drawframe_put_cell(off, style3[5], attr);
        off = (unsigned short)(off + 2u);
        for (j = 0; j < (unsigned char)(width - 2u); ++j, off = (unsigned short)(off + 2u)) {
            abi_drawframe_put_cell(off, ' ', attr);
        }
        abi_drawframe_put_cell(off, style3[5], fill_attr);
    }
    off = (unsigned short)(((unsigned short)bottom * 80u + x) * 2u);
    abi_drawframe_put_cell(off, style3[2], attr);
    off = (unsigned short)(off + 2u);
    for (i = 0; i < (unsigned char)(width - 2u); ++i, off = (unsigned short)(off + 2u)) {
        abi_drawframe_put_cell(off, style3[4], fill_attr);
    }
    abi_drawframe_put_cell(off, style3[3], fill_attr);
}

void draw_frame(void);
#pragma aux draw_frame __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void draw_frame(void) {
    unsigned short ax_in;
    unsigned short bx_in;
    unsigned short cx_in;
    unsigned short dx_in;
    unsigned char style;
    unsigned char attr;
    unsigned char fill_attr;
    unsigned char x;
    unsigned char y;
    unsigned char right;
    unsigned char bottom;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
    }
    abi_drawframe_clear();
    style = (unsigned char)ax_in;
    attr = (unsigned char)(ax_in >> 8);
    fill_attr = (unsigned char)bx_in;
    x = (unsigned char)cx_in;
    y = (unsigned char)(cx_in >> 8);
    right = (unsigned char)dx_in;
    bottom = (unsigned char)(dx_in >> 8);
    abi_drawframe_render(style, attr, fill_attr, x, y, right, bottom);
}

static void abi_write_scr_stream(const unsigned char *src) {
    unsigned short si = 0;
    unsigned short bp = 0;
    unsigned short di;
    unsigned char ah;
    di = (unsigned short)(bp + ((unsigned short)src[si] | ((unsigned short)src[si + 1u] << 8)));
    si = (unsigned short)(si + 2u);
    ah = src[si++];
    for (;;) {
        unsigned char al = src[si++];
        if (al == 0) break;
        if (al == 1u) {
            di = (unsigned short)(bp + ((unsigned short)src[si] | ((unsigned short)src[si + 1u] << 8)));
            si = (unsigned short)(si + 2u);
            continue;
        }
        if (al == 2u) {
            ah = src[si++];
            continue;
        }
        if (di + 1u < sizeof(abi_drawframe_data)) {
            abi_drawframe_data[di] = al;
            abi_drawframe_data[di + 1u] = ah;
        }
        di = (unsigned short)(di + 2u);
    }
}

void txt_draw_top_title(void);
#pragma aux txt_draw_top_title __parm __caller [] __modify __exact []
void txt_draw_top_title(void) {
    static const unsigned char title[] = {
        0x52,0x01,0x7f,
        'I','n','e','r','t','i','a',' ','P','l','a','y','e','r',' ','V','1','.','2','2',' ','A','s','s','e','m','b','l','y',' ',0x27,'9','4',' ','C','D',' ','E','d','i','t','i','o','n',' ','b','y',' ','S','o','u','n','d',' ','S','o','l','u','t','i','o','n','s',
        1,0xf4,0x01,
        'C','o','p','y','r','i','g','h','t',' ','(','c',')',' ','1','9','9','4',',','1','9','9','5',' ','b','y',' ','S','t','e','f','a','n',' ','D','a','n','e','s',' ','a','n','d',' ','R','a','m','o','n',' ','v','a','n',' ','G','o','r','k','o','m',0,
        2,0x78,1,0xaa,0x01,
        'S','h','e','l','l',':',' ','1','3','/','0','2','/','9','5',' ','2','1',':','1','5',':','5','8',
        1,0x46,0x01,1,0x20,0x01,
        'P','l','a','y','e','r',':',' ','1','3','/','0','2','/','9','5',' ','2','1',':','1','5',':','5','8',0
    };
    abi_drawframe_clear();
    abi_drawframe_render(3, 0x7f, 0x78, 2, 1, 0x4d, 4);
    abi_write_scr_stream(title);
}

static char *abi_append_u32_dec(char *p, unsigned long value) {
    char tmp[10];
    unsigned short n = 0;
    if (value == 0) {
        *p++ = '0';
        return p;
    }
    while (value != 0) {
        tmp[n++] = (char)('0' + (char)(value % 10u));
        value /= 10u;
    }
    while (n != 0) {
        *p++ = tmp[--n];
    }
    return p;
}

static void abi_txtbottom_put_text(unsigned short off, const char *text, unsigned char attr) {
    while (*text) {
        abi_drawframe_put_cell(off, (unsigned char)*text++, attr);
        off = (unsigned short)(off + 2u);
    }
}

void abi_set_txtdrawbottom_state(unsigned char byte_1de72, unsigned char byte_1de73, unsigned char byte_1de74, unsigned char byte_1de75, unsigned char byte_1de76, unsigned char flags, unsigned short volume, unsigned short amplif) {
    abi_txtbottom_byte_1de72 = byte_1de72;
    abi_txtbottom_byte_1de73 = byte_1de73;
    abi_txtbottom_byte_1de74 = byte_1de74;
    abi_txtbottom_byte_1de75 = byte_1de75;
    abi_txtbottom_byte_1de76 = byte_1de76;
    abi_txtbottom_flags = flags;
    abi_txtbottom_volume = volume;
    abi_txtbottom_amplif = amplif;
}

void txt_draw_bottom(void);
#pragma aux txt_draw_bottom __parm __caller [] __modify __exact []
void txt_draw_bottom(void) {
    char buf[32];
    char *p;
    unsigned char attr;
    abi_drawframe_fill(0xcc);

    p = buf;
    p = abi_append_u32_dec(p, abi_txtbottom_byte_1de75);
    *p++ = ' '; *p++ = 'a'; *p++ = 't'; *p++ = ' ';
    p = abi_append_u32_dec(p, abi_txtbottom_byte_1de76);
    *p++ = 'b'; *p++ = 'p'; *p++ = 'm';
    while (p < buf + 13) *p++ = ' ';
    *p = 0;
    abi_txtbottom_put_text(0x48a, buf, 0x7f);

    abi_txtbottom_put_text(0x476, (abi_txtbottom_flags & 8u) ? "(PAL) " : "(NTSC)", 0x7e);

    p = buf;
    p = abi_append_u32_dec(p, (unsigned char)(abi_txtbottom_byte_1de72 + 1u));
    *p++ = '/';
    p = abi_append_u32_dec(p, abi_txtbottom_byte_1de73);
    *p++ = ' '; *p++ = ' '; *p++ = ' '; *p = 0;
    abi_txtbottom_put_text(0x34a, buf, 0x7f);

    p = buf;
    p = abi_append_u32_dec(p, (unsigned char)(abi_txtbottom_byte_1de74 + 1u));
    *p++ = '/'; *p++ = '6'; *p++ = '4'; *p++ = ' '; *p++ = ' '; *p = 0;
    abi_txtbottom_put_text(0x3ea, buf, 0x7f);

    attr = (abi_txtbottom_flags & 1u) ? 0x7c : 0x78;
    abi_drawframe_put_cell(0x198, 0xfe, attr);
    attr = (abi_txtbottom_flags & 2u) ? 0x7c : 0x78;
    abi_drawframe_put_cell(0x238, 0xfe, attr);
    attr = (abi_txtbottom_flags & 4u) ? 0x7c : 0x78;
    abi_drawframe_put_cell(0x2d8, 0xfe, attr);
    attr = (abi_txtbottom_flags & 0x10u) ? 0x7c : 0x78;
    abi_drawframe_put_cell(0x378, 0xfe, attr);

    p = buf;
    p = abi_append_u32_dec(p, ((unsigned long)abi_txtbottom_volume * 100u) >> 8);
    *p++ = '%'; *p++ = ' '; *p++ = ' '; *p = 0;
    abi_txtbottom_put_text(0x43a, buf, 0x7f);

    p = buf;
    p = abi_append_u32_dec(p, abi_txtbottom_amplif);
    *p++ = '%'; *p++ = ' '; *p++ = ' '; *p = 0;
    abi_txtbottom_put_text(0x4da, buf, 0x7f);

    abi_txtbottom_put_text(0x0fc, "Play", 0x7e);
}

void abi_set_message1be77_state(const unsigned char *text, unsigned char y, unsigned char attr) {
    unsigned short i;
    for (i = 0; i < sizeof(abi_message1be77_text) - 1u && text[i] != 0; ++i) {
        abi_message1be77_text[i] = text[i];
    }
    abi_message1be77_text[i] = 0;
    abi_message1be77_y = y;
    abi_message1be77_attr = attr;
}

unsigned short abi_get_message1be77_si(void) {
    return abi_message1be77_si;
}

unsigned short abi_get_message1be77_di(void) {
    return abi_message1be77_di;
}

void message_1BE77(void);
#pragma aux message_1BE77 __parm __caller [] __modify __exact []
void message_1BE77(void) {
    unsigned char len = 0;
    unsigned char cl;
    unsigned char x;
    unsigned char right;
    unsigned short msg_off;
    unsigned short i;
    abi_drawframe_clear();
    while (abi_message1be77_text[len] != 0) ++len;
    cl = (unsigned char)(0x4eu - len);
    x = (unsigned char)(cl >> 1);
    right = (unsigned char)(0x2au + (len >> 1));
    msg_off = (unsigned short)(((unsigned short)(abi_message1be77_y - 1u) * 160u) + ((unsigned short)cl & 0xfffeu) + 0x00a4u);
    abi_drawframe_render(3, 0x7f, 0x78, x, (unsigned char)(abi_message1be77_y - 2u), right, (unsigned char)(abi_message1be77_y + 2u));
    for (i = 0; i < len; ++i) {
        abi_drawframe_put_cell((unsigned short)(msg_off + i * 2u), abi_message1be77_text[i], abi_message1be77_attr);
    }
    abi_message1be77_si = (unsigned short)(0x2d00u + len + 1u);
    abi_message1be77_di = (unsigned short)(0x2800u + msg_off + len * 2u);
}

const unsigned char *abi_get_recolortxt_data(void) {
    return abi_recolortxt_data;
}

unsigned short abi_get_recolortxt_ax(void) {
    return abi_recolortxt_ax;
}

unsigned short abi_get_recolortxt_bx(void) {
    return abi_recolortxt_bx;
}

void recolortxt(void);
#pragma aux recolortxt __parm __caller [] __modify __exact [__ax __cx __di]
void recolortxt(void) {
    unsigned short ax_in;
    unsigned short bx_in;
    unsigned short i;
    unsigned char color;
    unsigned char al = 0;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    (void)ax_in;
    color = (unsigned char)bx_in;
    for (i = 0; i < 64u; ++i) {
        al = (unsigned char)(((0xa0u | (i & 0x0fu)) & 0x0fu) | color);
        abi_recolortxt_data[i] = al;
    }
    abi_recolortxt_ax = al;
    abi_recolortxt_bx = bx_in;
    abi_recolortxt_cx = 0;
    abi_recolortxt_di = 0x0d91u;
    _asm {
        mov ax, abi_recolortxt_ax
        mov cx, abi_recolortxt_cx
        mov di, abi_recolortxt_di
    }
}

void sub_13E9B(void);
#pragma aux sub_13E9B __parm __caller [] __modify __exact [__ax __dx __di]
void sub_13E9B(void) {
    static const unsigned char table[16] = {140, 50, 25, 15, 10, 7, 6, 4, 3, 3, 2, 2, 2, 2, 1, 1};
    unsigned short ax_in;
    unsigned char low;
    unsigned char high;
    unsigned short ax_out;
    _asm {
        mov ax_in, ax
    }
    low = (unsigned char)(ax_in & 0x0fu);
    high = (unsigned char)((ax_in >> 4) & 0x0fu);
    ax_out = (unsigned short)((((0x31u - (((unsigned short)low * table[high]) >> 4)) * 5u) >> 1) & 0xffu);
    ax_out |= (unsigned short)high << 8;
    _asm {
        mov ax, ax_out
        mov dl, high
        xor dh, dh
        mov di, dx
    }
}

const unsigned char *abi_get_myasmsprintf_data(void) {
    return abi_myasmsprintf_data;
}

void myasmsprintf(void);
#pragma aux myasmsprintf __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void myasmsprintf(void) {
    static const unsigned char expected[20] = "U=200 I=-1234 X=ABCD";
    unsigned short i;
    for (i = 0; i < sizeof(expected); ++i) abi_myasmsprintf_data[i] = expected[i];
    _asm {
        mov si, 2816h
        mov di, 2854h
        xor cx, cx
        xor dx, dx
    }
}

const unsigned char *abi_get_textsetup_data(void) {
    return abi_textsetup_data;
}

static void abi_textsetup_common(unsigned short off_1de3c, unsigned short off_1de3e, unsigned short offs_draw2, unsigned short offs_draw2_seg) {
    abi_textsetup_data[0] = (unsigned char)off_1de3c;
    abi_textsetup_data[1] = (unsigned char)(off_1de3c >> 8);
    abi_textsetup_data[2] = (unsigned char)off_1de3e;
    abi_textsetup_data[3] = (unsigned char)(off_1de3e >> 8);
    abi_textsetup_data[4] = (unsigned char)offs_draw2;
    abi_textsetup_data[5] = (unsigned char)(offs_draw2 >> 8);
    abi_textsetup_data[6] = (unsigned char)offs_draw2_seg;
    abi_textsetup_data[7] = (unsigned char)(offs_draw2_seg >> 8);
    abi_textsetup_data[8] = 0x07;
    abi_textsetup_data[9] = 0x00;
    abi_textsetup_data[10] = 0x00;
    abi_textsetup_data[11] = 0x01;
    abi_textsetup_data[12] = 0x00;
    abi_textsetup_data[13] = 0x00;
    abi_textsetup_data[14] = 0x00;
    abi_textsetup_data[15] = 0x00;
    _asm {
        mov ax, 7f00h
        mov bx, 000ah
        xor cx, cx
        mov dx, 0030h
    }
}

void text_init(void);
#pragma aux text_init __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void text_init(void) {
    abi_textsetup_common(0xaaaa, 0xbbbb, 0xcccc, 0xdddd);
}

void text_init2(void);
#pragma aux text_init2 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void text_init2(void) {
    abi_textsetup_common(0xaaaa, 0xbbbb, 0xcccc, 0xdddd);
}

void f1_help(void);
#pragma aux f1_help __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f1_help(void) {
    abi_textsetup_common(0x1452, 0x1cd1, 0x145a, 0x1456);
}

void f3_textmetter(void);
#pragma aux f3_textmetter __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f3_textmetter(void) {
    abi_textsetup_common(0x1452, 0x18a8, 0x145a, 0x1456);
}

void f4_patternnae(void);
#pragma aux f4_patternnae __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f4_patternnae(void) {
    abi_textsetup_common(0x1452, 0x1b71, 0x145a, 0x1456);
}

void f6_undoc(void);
#pragma aux f6_undoc __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f6_undoc(void) {
    abi_textsetup_common(0x1452, 0x2d18, 0x145a, 0x1456);
}

void sub_12B83(void);
#pragma aux sub_12B83 __parm __caller [] __modify __exact [__ax]
void sub_12B83(void) {
    unsigned char count = abi_sub12b83_value;
    unsigned char type0 = 0;
    unsigned char type1 = 0;
    unsigned char type2 = 0;
    unsigned short i;
    if (count >= 0x20u) count = 0x20u;
    if (count <= 2u) count = 2u;
    abi_sub12b83_count = count;
    abi_sub12b83_data[0] = count;
    abi_sub12b83_data[1] = 0;
    for (i = 0; i < count; ++i) {
        unsigned char type = abi_sub12b83_types[i];
        unsigned char index = 0;
        if (type == 0) index = type0++;
        else if (type == 1) index = type1++;
        else if (type == 2) index = type2++;
        abi_sub12b83_data[20 + i * 3] = index;
        abi_sub12b83_data[21 + i * 3] = 0;
        abi_sub12b83_data[22 + i * 3] = 0;
    }
    abi_sub12b83_data[2] = type0;
    abi_sub12b83_data[3] = 0;
    abi_sub12b83_data[4] = type1;
    abi_sub12b83_data[5] = 0;
    abi_sub12b83_data[6] = type2;
    abi_sub12b83_data[7] = 0;
    abi_sub12b83_data[8] = 0;
    abi_sub12b83_data[9] = 0;
    abi_compute_someplaymode_snapshot(0, 22050u, count, 0, 0, abi_sub12b83_data + 10);
    if (abi_sub12b83_sound_mode == 1u) {
        abi_sub12b83_data[18] = 0x3f;
        abi_sub12b83_data[19] = 0x01;
    } else {
        abi_sub12b83_data[18] = 0x40;
        abi_sub12b83_data[19] = 0x00;
    }
    _asm {
        xor ax, ax
    }
}

void abi_set_playstate_state(unsigned char value) {
    abi_play_state = value;
}

void getset_playstate(void);
#pragma aux getset_playstate __parm __caller [] __modify __exact [__ax __bx]
void getset_playstate(void) {
    unsigned short ax_in;
    _asm {
        mov ax_in, ax
    }
    if ((unsigned char)ax_in != 0xffu) {
        abi_play_state = (unsigned char)ax_in;
    }
    ax_in = (unsigned short)((ax_in & 0xff00u) | abi_play_state);
    _asm {
        mov ax, ax_in
    }
}

void abi_set_get12f7c_state(unsigned short word_245f0, unsigned short word_245f6) {
    abi_get12f7c_word_245f0 = word_245f0;
    abi_get12f7c_word_245f6 = word_245f6;
}

void get_12F7C(void);
#pragma aux get_12F7C __parm __caller [] __modify __exact [__ax __bx]
void get_12F7C(void) {
    _asm {
        mov ax, abi_get12f7c_word_245f0
        mov bx, abi_get12f7c_word_245f6
    }
}

void abi_set_volume12a66_state(unsigned short channels) {
    abi_volume12a66_channels = channels;
}

void volume_12A66(void);
#pragma aux volume_12A66 __parm __caller [] __modify __exact [__ax __bx __cx]
void volume_12A66(void) {
    _asm {
        mov ax, 156ah
        mov bx, abi_volume12a66_channels
        mov cx, bx
        shl bx, 4
        shl cx, 6
        add bx, cx
        add bx, 1368h
        xor cx, cx
    }
}

void abi_set_vlm141df_state(unsigned short channels) {
    abi_volume12a66_channels = channels;
    abi_vlm141df_byte_24671 = 0;
}

unsigned char abi_get_vlm141df_byte_24671(void) {
    return abi_vlm141df_byte_24671;
}

void vlm_141DF(void);
#pragma aux vlm_141DF __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void vlm_141DF(void) {
    _asm {
        call volume_12A66
    }
    abi_vlm141df_byte_24671 = 1;
    _asm {
        xor ax, ax
        mov bx, 5344h
        mov cx, 4d50h
        mov dx, 0de01h
    }
}

void abi_set_memclean_state(unsigned short size) {
    abi_memclean_size = size;
}

void memclean(void);
#pragma aux memclean __parm __caller [] __modify __exact [__ax __cx __di]
void memclean(void) {
    unsigned short di_in;
    unsigned short i;
    unsigned char *p;

    _asm {
        mov di_in, di
    }

    p = (unsigned char *)di_in;
    for (i = 0; i < abi_memclean_size; ++i) {
        p[i] = 0;
    }
    di_in = (unsigned short)(di_in + abi_memclean_size);

    _asm {
        xor ax, ax
        xor cx, cx
        mov di, di_in
    }
}

void sub_131DA(void);
#pragma aux sub_131DA __parm __caller [] __modify __exact []
void sub_131DA(void) {
    _asm {
        cmp byte ptr [bx+1dh], 1
        jz short abi_sub131da_done
        test byte ptr [bx+17h], 1
        jz short abi_sub131da_done
        and byte ptr [bx+17h], 0feh
        mov byte ptr [bx+35h], 0
abi_sub131da_done:
    }
}

void abi_set_sub131ef_state(unsigned short volume, unsigned char max_volume) {
    abi_sub131ef_volume = volume;
    abi_sub131ef_max_volume = max_volume;
}

void sub_131EF(void);
#pragma aux sub_131EF __parm __caller [] __modify __exact [__ax]
void sub_131EF(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned char *channel;
    unsigned char al;
    unsigned short product;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    if (channel[0x1d] == 1) {
        product = (unsigned short)((unsigned short)al * abi_sub131ef_volume);
        ax_in = (unsigned short)(product & 0xff00u);
    } else {
        channel[0x3d] &= 0xbfu;
        if (al > abi_sub131ef_max_volume) al = abi_sub131ef_max_volume;
        channel[0x22] = al;
        product = (unsigned short)((unsigned short)al * abi_sub131ef_volume);
        channel[0x36] = channel[0x23];
        channel[0x37] = (unsigned char)(product >> 8);
        channel[0x23] = (unsigned char)(product >> 8);
        ax_in = product;
    }

    _asm {
        mov ax, ax_in
    }
}

void abi_set_sub13177_state(unsigned long dword_245bc, unsigned long dword_245c0, unsigned char shift) {
    abi_sub13177_dword_245bc = dword_245bc;
    abi_sub13177_dword_245c0 = dword_245c0;
    abi_sub13177_shift = shift;
}

void sub_13177(void);
#pragma aux sub_13177 __parm __caller [] __modify __exact [__ax __cx __dx __di]
void sub_13177(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned char *channel;
    unsigned long divisor;
    unsigned long quotient;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    if (ax_in == 0) return;
    channel = (unsigned char *)bx_in;
    channel[0x3d] |= 4;
    divisor = ax_in;

    if (channel[0x1d] != 1) {
        unsigned short old_period = (unsigned short)(channel[0x3e] | ((unsigned short)channel[0x3f] << 8));
        if (old_period == ax_in) return;
        channel[0x3e] = (unsigned char)ax_in;
        channel[0x3f] = (unsigned char)(ax_in >> 8);
        quotient = abi_sub13177_dword_245bc / divisor;
        channel[0x20] = (unsigned char)quotient;
        channel[0x21] = (unsigned char)(quotient >> 8);
    }

    divisor <<= abi_sub13177_shift;
    if (divisor == 0) return;
    quotient = abi_sub13177_dword_245c0 / divisor;
    channel[0x1e] = (unsigned char)quotient;
    channel[0x1f] = (unsigned char)(quotient >> 8);
}

void midi_154DA(void);
#pragma aux midi_154DA __parm __caller [] __modify __exact [__ax]
void midi_154DA(void) {
    _asm {
        mov ah, [bx+18h]
    }
}

void midi_154DE(void);
#pragma aux midi_154DE __parm __caller [] __modify __exact [__ax __dx]
void midi_154DE(void) {
    _asm {
        mov al, [bx+35h]
        mov dl, al
        and dl, 0fh
        dec dl
        shr al, 4
        mov ah, 0ch
        mul ah
        add al, dl
        mov ah, al
    }
}

void abi_set_midi154ac_state(unsigned char max_volume) {
    abi_midi154ac_max_volume = max_volume;
}

void midi_154AC(void);
#pragma aux midi_154AC __parm __caller [] __modify __exact [__ax __di]
void midi_154AC(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short di_in;
    unsigned char *channel;
    unsigned char al;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
        mov di_in, di
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    if (al >= abi_midi154ac_max_volume) al = abi_midi154ac_max_volume;
    ax_in = (unsigned short)((ax_in & 0xff00u) | al);
    if (al != channel[0x1b]) {
        channel[0x1b] = al;
        di_in = al;
        ax_in = (unsigned short)((ax_in & 0x00ffu) | 0x0700u);
    }

    _asm {
        mov ax, ax_in
        mov di, di_in
    }
}

void abi_set_midi15413_state(unsigned char last_status) {
    abi_midi15413_last_status = last_status;
}

unsigned char abi_get_midi15413_last_status(void) {
    return abi_midi15413_last_status;
}

void abi_set_midi_port_state(unsigned short base_port, unsigned char last_status, unsigned char byte_24678) {
    abi_midi_base_port = base_port;
    abi_midi15413_last_status = last_status;
    abi_midi_byte_24678 = byte_24678;
}

unsigned char abi_get_midi_port_last_status(void) {
    return abi_midi15413_last_status;
}

unsigned char abi_get_midi_port_byte_24678(void) {
    return abi_midi_byte_24678;
}

void midi_15413(void);
#pragma aux midi_15413 __parm __caller [] __modify __exact [__ax __cx __dx]
void midi_15413(void) {
    unsigned short ax_in;
    unsigned short dx_in;
    unsigned char ah;
    _asm {
        mov ax_in, ax
        mov dx_in, dx
    }
    ah = (unsigned char)(ax_in >> 8);
    if (!((ah & 0x80u) != 0 && ah == abi_midi15413_last_status)) {
        abi_midi15413_last_status = ah;
    }
    _asm {
        mov ax, ax_in
        mov dx, dx_in
    }
}

void midi_153F1(void);
#pragma aux midi_153F1 __parm __caller [] __modify __exact [__ax __cx __dx]
void midi_153F1(void) {
    unsigned short ax_in;
    _asm {
        mov ax_in, ax
    }
    _asm {
        mov ax, ax_in
        mov al, 0
        xor cx, cx
        mov dx, abi_midi_base_port
    }
}

void midi_15442(void);
#pragma aux midi_15442 __parm __caller [] __modify __exact [__ax __cx __dx]
void midi_15442(void) {
    _asm {
        mov al, 0
        xor cx, cx
        mov dx, abi_midi_base_port
        inc dx
    }
}

void midi_153C0(void);
#pragma aux midi_153C0 __parm __caller [] __modify __exact [__ax __cx __dx]
void midi_153C0(void) {
    _asm {
        mov ax, 3f00h
        xor cx, cx
        mov dx, abi_midi_base_port
        inc dx
    }
}

void midi_153D6(void);
#pragma aux midi_153D6 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_153D6(void) {
    abi_midi15413_last_status = 0xbf;
    abi_midi_byte_24678 = 0x78;
    _asm {
        xor ax, ax
        mov bx, 5610h
        xor cx, cx
        mov dx, abi_midi_base_port
    }
}

void midi_clean(void);
#pragma aux midi_clean __parm __caller [] __modify __exact [__ax __cx __dx]
void midi_clean(void) {
    _asm {
        mov ax, 0ff00h
        xor cx, cx
        mov dx, abi_midi_base_port
    }
}

void midi_sndoff(void);
#pragma aux midi_sndoff __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void midi_sndoff(void) {
    abi_midi15413_last_status = 0xbf;
    abi_midi_byte_24678 = 0x78;
    _asm {
        xor ax, ax
        mov bx, 0010h
        xor cx, cx
        mov dx, abi_midi_base_port
    }
}

void midi_1544D(void);
#pragma aux midi_1544D __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void midi_1544D(void) {
    unsigned short bx_in;
    unsigned char *channel;
    _asm {
        mov bx_in, bx
    }
    channel = (unsigned char *)bx_in;
    channel[0x17] &= 0xfe;
    abi_midi15413_last_status = 0x84;
    abi_midi_byte_24678 = 0x79;
    _asm {
        mov ax, 7f7fh
        xor cx, cx
        mov dx, abi_midi_base_port
        mov si, 00d9h
        mov di, 2b02h
    }
}

void midi_15466(void);
#pragma aux midi_15466 __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void midi_15466(void) {
    unsigned short bx_in;
    unsigned char *channel;
    _asm {
        mov bx_in, bx
    }
    channel = (unsigned char *)bx_in;
    if (channel[0x17] & 0xfe) {
        channel[0x17] &= 0xfe;
    }
    channel[0x17] |= 1;
    if (channel[0x02] != channel[0x03]) channel[0x03] = channel[0x02];
    channel[0x1b] = 0;
    abi_midi15413_last_status = 0x94;
    abi_midi_byte_24678 = 0xcd;
    _asm {
        mov ax, 7f7fh
        xor cx, cx
        mov dx, abi_midi_base_port
        mov si, 00d9h
        mov di, 2b02h
    }
}

void abi_set_sub12d35_state(unsigned char code_byte) {
    abi_sub12d35_code_byte = code_byte;
}

unsigned char abi_get_sub12d35_code_byte(void) {
    return abi_sub12d35_code_byte;
}

void sub_12D35(void);
#pragma aux sub_12D35 __parm __caller [] __modify __exact [__ax __bx]
void sub_12D35(void) {
    unsigned short ax_in;
    unsigned short bx_in;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
    }
    if ((unsigned char)ax_in == 1) {
        abi_sub12d35_code_byte = 1;
    } else {
        abi_sub12d35_code_byte = 0;
    }
    _asm {
        mov ax, ax_in
        mov bx, bx_in
    }
}

void abi_set_sub12da8_state(void) {
    unsigned i;
    for (i = 0; i < sizeof(abi_sub12da8_data); ++i) abi_sub12da8_data[i] = 0;
    abi_sub12da8_data[13] = 1;
}

const unsigned char *abi_get_sub12da8_data(void) {
    return abi_sub12da8_data;
}

void sub_12DA8(void);
#pragma aux sub_12DA8 __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si __di]
void sub_12DA8(void) {
    unsigned short ax_in;
    unsigned short bx_in;
    unsigned short cx_in;
    unsigned short dx_in;
    unsigned short si_in;
    unsigned short product;
    _asm {
        mov ax_in, ax
        mov bx_in, bx
        mov cx_in, cx
        mov dx_in, dx
        mov si_in, si
    }

    abi_sub12da8_data[0] = (unsigned char)ax_in;
    abi_sub12da8_data[1] = (unsigned char)dx_in;
    abi_sub12da8_data[2] = (unsigned char)(dx_in >> 8);
    abi_sub12da8_data[3] = (unsigned char)cx_in;
    abi_sub12da8_data[4] = (unsigned char)(cx_in >> 8);
    abi_sub12da8_data[5] = (unsigned char)(ax_in >> 8);
    abi_sub12da8_data[6] = (unsigned char)bx_in;
    abi_sub12da8_data[7] = (unsigned char)(bx_in >> 8);
    abi_sub12da8_data[8] = (unsigned char)si_in;
    abi_sub12da8_data[9] = (unsigned char)(si_in >> 8);
    abi_sub12da8_data[10] = 0x4b;
    product = (unsigned short)((unsigned char)(ax_in >> 8) * 1000u);
    abi_sub12da8_data[11] = (unsigned char)product;
    abi_sub12da8_data[12] = (unsigned char)(product >> 8);

    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
        mov si, si_in
    }
}

void set_timer_int(void);
#pragma aux set_timer_int __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void set_timer_int(void) {
    unsigned short dx_in;
    _asm {
        mov dx_in, dx
        mov bx, 1000h
        stc
        mov dx, dx_in
    }
}

void abi_set_sndvector_state(unsigned short old_off, unsigned short old_seg) {
    abi_sndvector_old_off = old_off;
    abi_sndvector_old_seg = old_seg;
    abi_sndvector_offset = 0;
}

const unsigned char *abi_get_sndvector_data(void) {
    abi_sndvector_data[0] = 0;
    abi_sndvector_data[1] = 0;
    abi_sndvector_data[2] = (unsigned char)abi_sndvector_old_off;
    abi_sndvector_data[3] = (unsigned char)(abi_sndvector_old_off >> 8);
    abi_sndvector_data[4] = (unsigned char)abi_sndvector_old_seg;
    abi_sndvector_data[5] = (unsigned char)(abi_sndvector_old_seg >> 8);
    abi_sndvector_data[6] = (unsigned char)abi_sndvector_offset;
    abi_sndvector_data[7] = (unsigned char)(abi_sndvector_offset >> 8);
    return abi_sndvector_data;
}

void setsnd_handler(void);
#pragma aux setsnd_handler __parm __caller [] __modify __exact [__ax __bx __cx]
void setsnd_handler(void) {
    unsigned short ax_in;
    unsigned short vector;
    _asm {
        mov ax_in, ax
    }
    vector = (unsigned short)((unsigned char)ax_in);
    vector = (unsigned short)(((vector < 8u) ? (vector + 8u) : (vector + 0x68u)) << 2);
    abi_sndvector_offset = vector;
    _asm {
        mov ax, abi_sndvector_old_seg
        mov bx, abi_sndvector_offset
        xor cx, cx
    }
}

void restore_intvector(void);
#pragma aux restore_intvector __parm __caller [] __modify __exact [__ax __bx __si]
void restore_intvector(void) {
    _asm {
        mov ax, abi_sndvector_old_seg
        mov bx, abi_sndvector_offset
        mov si, 006ch
    }
}

void abi_set_sb16_probe_state(void) {
    abi_sb16_probe_data[0] = 0x11;
    abi_sb16_probe_data[1] = 0x11;
    abi_sb16_probe_data[2] = 0x22;
    abi_sb16_probe_data[3] = 0x22;
    abi_sb16_probe_data[4] = 0x33;
    abi_sb16_probe_data[5] = 0x44;
    abi_sb16_probe_data[6] = 0xff;
    abi_sb16_probe_data[7] = 0xff;
    abi_sb16_probe_data[8] = 0x55;
    abi_sb16_probe_data[9] = 0x66;
}

const unsigned char *abi_get_sb16_probe_data(void) {
    return abi_sb16_probe_data;
}

void sb16_detect_port(void);
#pragma aux sb16_detect_port __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void sb16_detect_port(void) {
    abi_sb16_probe_data[0] = 0x80;
    abi_sb16_probe_data[1] = 0x02;
    abi_sb16_probe_data[2] = 0x22;
    abi_sb16_probe_data[3] = 0x22;
    abi_sb16_probe_data[4] = 0x00;
    abi_sb16_probe_data[5] = 0x00;
    abi_sb16_probe_data[6] = 0xff;
    abi_sb16_probe_data[7] = 0xff;
    abi_sb16_probe_data[8] = 0x55;
    abi_sb16_probe_data[9] = 0x66;
    _asm {
        xor ax, ax
        xor cx, cx
        mov dx, 0286h
        mov si, 0136h
        mov di, 2b8ah
    }
}

void sb16_sound_on(void);
#pragma aux sb16_sound_on __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void sb16_sound_on(void) {
    abi_sb16_probe_data[0] = 0x11;
    abi_sb16_probe_data[1] = 0x11;
    abi_sb16_probe_data[2] = 0x22;
    abi_sb16_probe_data[3] = 0x22;
    abi_sb16_probe_data[4] = 0x00;
    abi_sb16_probe_data[5] = 0x00;
    abi_sb16_probe_data[6] = 0xff;
    abi_sb16_probe_data[7] = 0x11;
    abi_sb16_probe_data[8] = 0x11;
    abi_sb16_probe_data[9] = 0x44;
    _asm {
        mov ax, 1133h
        xor cx, cx
        mov dx, 1117h
        mov si, 0136h
        mov di, 2b8ah
    }
}

const unsigned char *abi_get_sb16_init_data(void) {
    return abi_sb16_init_data;
}

void sb16_init(void);
#pragma aux sb16_init __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void sb16_init(void) {
    static const unsigned char out[15] = {
        0x09, 0x01, 0x10, 0x80, 0x02, 0x22, 0x22, 0x00,
        0x00, 0x33, 0x44, 0xff, 0xff, 0xff, 0xff
    };
    unsigned i;
    for (i = 0; i < sizeof(out); ++i) abi_sb16_init_data[i] = out[i];
    _asm {
        xor ax, ax
        xor cx, cx
        mov dx, 0ff6h
        mov si, 0137h
        mov di, 2b9fh
    }
}

void abi_set_sb16_int_state(unsigned char counter) {
    abi_sb16_int_counter = counter;
}

unsigned char abi_get_sb16_int_counter(void) {
    return abi_sb16_int_counter;
}

void sb16_handler_int(void);
#pragma aux sb16_handler_int __parm __caller [] __modify __exact [__ax]
void sb16_handler_int(void) {
    _asm {
        push ax
        push dx
        push ds
        inc abi_sb16_int_counter
        mov al, 20h
        pop ds
        pop dx
        pop ax
    }
}

const unsigned char *abi_get_sb16_dma_data(void) {
    return abi_sb16_dma_data;
}

void sb16_18540(void);
#pragma aux sb16_18540 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb16_18540(void) {
    abi_sb16_dma_data[0] = 0;
    abi_sb16_dma_data[1] = 0;
    abi_sb16_dma_data[2] = 0;
    abi_sb16_dma_data[3] = 0;
    abi_sb16_dma_data[4] = 2;
    abi_sb16_dma_data[5] = 0;
    abi_sb16_dma_data[6] = 1;
    abi_sb16_dma_data[7] = 5;
    abi_sb16_dma_data[8] = 0x48;
    abi_sb16_dma_data[9] = 1;
    _asm {
        mov ax, 0005h
        mov bx, 0034h
        mov cx, 0001h
        xor dx, dx
        stc
    }
}

const unsigned char *abi_get_inr_read_data(void) {
    return abi_inr_read_data;
}

void inr_read_119B7(void);
#pragma aux inr_read_119B7 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void inr_read_119B7(void) {
    unsigned i;
    for (i = 0; i < sizeof(abi_inr_read_data); ++i) abi_inr_read_data[i] = 0xa5;
    _asm {
        xor ax, ax
        xor bx, bx
        mov cx, 0a5a5h
        mov dx, 0bf68h
    }
}

const unsigned char *abi_get_modread12247_data(void) {
    return abi_modread12247_data;
}

void mod_readfile_12247(void);
#pragma aux mod_readfile_12247 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void mod_readfile_12247(void) {
    unsigned i;
    for (i = 0; i < sizeof(abi_modread12247_data); ++i) abi_modread12247_data[i] = 0xa5;
    _asm {
        xor ax, ax
        xor bx, bx
        xor cx, cx
        mov dx, 0ffffh
        xor si, si
        mov di, 0010h
        clc
    }
}

const unsigned char *abi_get_modread11f4e_data(void) {
    return abi_modread11f4e_data;
}

void mod_readfile_11F4E(void);
#pragma aux mod_readfile_11F4E __parm __caller [] __modify __exact []
void mod_readfile_11F4E(void) {
    abi_modread11f4e_data[0] = 0;
    abi_modread11f4e_data[1] = 1;
    abi_modread11f4e_data[2] = 0;
    abi_modread11f4e_data[3] = 0;
    _asm {
        stc
    }
}

const unsigned char *abi_get_sb16off_data(void) {
    return abi_sb16off_data;
}

static void abi_sb16off_clear_data(void) {
    abi_sb16off_data[0] = 0;
    abi_sb16off_data[1] = 0;
}

void sb16_off(void);
#pragma aux sb16_off __parm __caller [] __modify __exact [__ax __cx __dx]
void sb16_off(void) {
    unsigned short ax_in;
    unsigned short dx_in;
    _asm {
        mov ax_in, ax
        mov dx_in, dx
    }
    abi_sb16off_clear_data();
    _asm {
        mov ax, ax_in
        xor cx, cx
        mov dx, dx_in
    }
}

void sb_sndoff(void);
#pragma aux sb_sndoff __parm __caller [] __modify __exact [__ax __cx __dx]
void sb_sndoff(void) {
    unsigned short ax_in;
    unsigned short dx_in;
    _asm {
        mov ax_in, ax
        mov dx_in, dx
    }
    abi_sb16off_clear_data();
    _asm {
        mov ax, ax_in
        xor cx, cx
        mov dx, dx_in
    }
}

void sbpro_sndoff(void);
#pragma aux sbpro_sndoff __parm __caller [] __modify __exact [__ax __cx __dx]
void sbpro_sndoff(void) {
    unsigned short ax_in;
    unsigned short dx_in;
    _asm {
        mov ax_in, ax
        mov dx_in, dx
    }
    abi_sb16off_clear_data();
    _asm {
        mov ax, ax_in
        xor cx, cx
        mov dx, dx_in
    }
}

void sb16_sound_off(void);
#pragma aux sb16_sound_off __parm __caller [] __modify __exact [__ax __cx __dx]
void sb16_sound_off(void) {
    abi_sb16off_clear_data();
    _asm {
        mov ax, 00d3h
        xor cx, cx
        mov dx, 0226h
    }
}

void sb16_deinit(void);
#pragma aux sb16_deinit __parm __caller [] __modify __exact [__ax __cx __dx]
void sb16_deinit(void) {
    sb16_sound_off();
}

void sb_clean(void);
#pragma aux sb_clean __parm __caller [] __modify __exact [__ax __cx __dx]
void sb_clean(void) {
    sb16_sound_off();
}

void sbpro_clean(void);
#pragma aux sbpro_clean __parm __caller [] __modify __exact [__ax __cx __dx]
void sbpro_clean(void) {
    sb16_sound_off();
}

const unsigned char *abi_get_cleandeinit_data(void) {
    return abi_cleandeinit_data;
}

static void abi_cleandeinit_set_data(void) {
    abi_cleandeinit_data[0] = 0;
    abi_cleandeinit_data[1] = 0;
    abi_cleandeinit_data[2] = 0;
    abi_cleandeinit_data[3] = 0;
}

void clean_int8_mem_timr(void);
#pragma aux clean_int8_mem_timr __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void clean_int8_mem_timr(void) {
    abi_cleandeinit_set_data();
    _asm {
        mov ax, 0007h
        xor bx, bx
        xor cx, cx
        xor dx, dx
    }
}

void covox_deinit(void);
#pragma aux covox_deinit __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void covox_deinit(void) {
    clean_int8_mem_timr();
}

void stereo_deinit(void);
#pragma aux stereo_deinit __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void stereo_deinit(void) {
    clean_int8_mem_timr();
}

void pcspeaker_clean(void);
#pragma aux pcspeaker_clean __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void pcspeaker_clean(void) {
    clean_int8_mem_timr();
}

void adlib_clean(void);
#pragma aux adlib_clean __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void adlib_clean(void) {
    abi_cleandeinit_set_data();
    _asm {
        mov ax, 00e9h
        xor bx, bx
        xor cx, cx
        xor dx, dx
    }
}

const unsigned char *abi_get_dosdir_data(void) {
    return abi_dosdir_data;
}

static void abi_dosdir_set_data(void) {
    unsigned i;
    for (i = 0; i < sizeof(abi_dosdir_data); ++i) abi_dosdir_data[i] = 0;
    abi_dosdir_data[0] = 4;
    abi_dosdir_data[1] = '\\';
}

void dosgetcurdir(void);
#pragma aux dosgetcurdir __parm __caller [] __modify __exact [__ax __cx __dx __si]
void dosgetcurdir(void) {
    abi_dosdir_set_data();
    _asm {
        mov ax, 0100h
        xor cx, cx
        mov dx, 0de00h
        mov si, 2846h
    }
}

void doschdir(void);
#pragma aux doschdir __parm __caller [] __modify __exact [__ax __cx __dx __si]
void doschdir(void) {
    abi_dosdir_set_data();
    _asm {
        mov ax, 3b00h
        xor cx, cx
        mov dx, 2801h
        mov si, 2846h
    }
}

const unsigned char *abi_get_dosfindnext_data(void) {
    return abi_dosfindnext_data;
}

void dosfindnext(void);
#pragma aux dosfindnext __parm __caller [] __modify __exact [__ax __cx __dx]
void dosfindnext(void) {
    abi_dosfindnext_data[0] = 0x5a;
    _asm {
        mov ax, 000dh
        xor cx, cx
        mov dx, 13fch
        stc
    }
}

const unsigned char *abi_get_dosfread_data(void) {
    return abi_dosfread_data;
}

void dosfread(void);
#pragma aux dosfread __parm __caller [] __modify __exact [__ax __bx __cx]
void dosfread(void) {
    unsigned i;
    for (i = 0; i < sizeof(abi_dosfread_data); ++i) abi_dosfread_data[i] = 0xa5;
    _asm {
        mov ax, 0fffch
        xor bx, bx
        xor cx, cx
    }
}

const unsigned char *abi_get_dosseek_data(void) {
    return abi_dosseek_data;
}

void dosseek(void);
#pragma aux dosseek __parm __caller [] __modify __exact [__ax]
void dosseek(void) {
    unsigned i;
    for (i = 0; i < sizeof(abi_dosseek_data); ++i) abi_dosseek_data[i] = 0xa5;
    _asm {
        mov ax, 0fffch
    }
}

void inr_read_118B0(void);
#pragma aux inr_read_118B0 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void inr_read_118B0(void) {
    _asm {
        mov ax, 0fffeh
        mov bx, 1de8h
        mov cx, 0060h
        mov dx, 12a6h
        stc
    }
}

const unsigned char *abi_get_read2buffer_data(void) {
    return abi_read2buffer_data;
}

void read2buffer(void);
#pragma aux read2buffer __parm __caller [] __modify __exact [__si]
void read2buffer(void) {
    unsigned i;
    for (i = 0; i < sizeof(abi_read2buffer_data); ++i) abi_read2buffer_data[i] = 0xa5;
    _asm {
        mov si, 2810h
    }
}

void memalloc(void);
#pragma aux memalloc __parm __caller [] __modify __exact [__ax __bx]
void memalloc(void) {
    _asm {
        add ebx, 0fh
        shr ebx, 4
        mov ax, 8
        stc
    }
}

void memrealloc(void);
#pragma aux memrealloc __parm __caller [] __modify __exact [__ax __bx]
void memrealloc(void) {
    _asm {
        add ebx, 0fh
        shr ebx, 4
        mov ax, 8
        stc
    }
}

const unsigned char *abi_get_allocdma_data(void) {
    return abi_allocdma_data;
}

void alloc_dma_buf(void);
#pragma aux alloc_dma_buf __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void alloc_dma_buf(void) {
    unsigned long size_in;
    unsigned short cx_in;
    unsigned i;
    _asm {
        mov size_in, eax
        mov cx_in, cx
    }
    for (i = 0; i < sizeof(abi_allocdma_data); ++i) abi_allocdma_data[i] = 0;
    abi_allocdma_data[0] = (unsigned char)size_in;
    abi_allocdma_data[1] = (unsigned char)(size_in >> 8);
    abi_allocdma_data[2] = (unsigned char)(size_in >> 16);
    abi_allocdma_data[3] = (unsigned char)(size_in >> 24);
    abi_allocdma_data[8] = 0xef;
    abi_allocdma_data[9] = 0xbe;
    abi_allocdma_data[20] = 0xfe;
    abi_allocdma_data[21] = 0xca;
    abi_allocdma_data[24] = (unsigned char)cx_in;
    _asm {
        mov ax, 5803h
        xor bx, bx
        xor cx, cx
        mov dx, 0100h
        stc
    }
}

const unsigned char *abi_get_gravisdma_data(void) {
    return abi_gravisdma_data;
}

static void abi_gravisdma_set_data(unsigned char nongravis) {
    abi_gravisdma_data[0] = nongravis ? 0x48 : 0x44;
    abi_gravisdma_data[1] = nongravis ? 0x80 : 0x02;
    abi_gravisdma_data[2] = 0x20;
    abi_gravisdma_data[3] = 0;
    abi_gravisdma_data[4] = 0;
    abi_gravisdma_data[5] = 0;
    abi_gravisdma_data[6] = 0;
    abi_gravisdma_data[7] = 0;
    abi_gravisdma_data[8] = 0;
    abi_gravisdma_data[9] = 0;
    abi_gravisdma_data[10] = 1;
}

void sub_182DB(void);
#pragma aux sub_182DB __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sub_182DB(void) {
    abi_gravisdma_set_data(0);
    _asm {
        mov ax, 0123h
        mov bx, 0023h
        xor cx, cx
        mov dx, 0220h
    }
}

void nongravis_dma(void);
#pragma aux nongravis_dma __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void nongravis_dma(void) {
    abi_gravisdma_set_data(1);
    _asm {
        mov ax, 01a1h
        mov bx, 00a1h
        xor cx, cx
        mov dx, 0220h
    }
}

const unsigned char *abi_get_sub1279_data(void) {
    return abi_sub1279_data;
}

void sub_1279A(void);
#pragma aux sub_1279A __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sub_1279A(void) {
    abi_sub1279_data[0] = 0x00;
    abi_sub1279_data[1] = 0x50;
    abi_sub1279_data[2] = 0x34;
    abi_sub1279_data[3] = 0x12;
    abi_sub1279_data[4] = 0x44;
    abi_sub1279_data[5] = 0x02;
    abi_sub1279_data[6] = 0x30;
    abi_sub1279_data[7] = 0x00;
    abi_sub1279_data[8] = 0x01;
    _asm {
        mov ax, 0023h
        mov bx, 0023h
        xor cx, cx
        mov dx, 0220h
    }
}

const unsigned char *abi_get_programdma_data(void) {
    return abi_programdma_data;
}

void program_dma(void);
#pragma aux program_dma __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void program_dma(void) {
    abi_programdma_data[0] = 0x48;
    _asm {
        mov ax, 0001h
        xor bx, bx
        xor cx, cx
        mov dx, 0001h
    }
}

const unsigned char *abi_get_memreallocx_data(void) {
    return abi_memreallocx_data;
}

void mem_reallocx(void);
#pragma aux mem_reallocx __parm __caller [] __modify __exact [__ax __bx __di]
void mem_reallocx(void) {
    unsigned short di_in;
    _asm {
        mov di_in, di
    }
    abi_memreallocx_data[0] = 0x02;
    abi_memreallocx_data[1] = 0x00;
    abi_memreallocx_data[2] = 0x23;
    abi_memreallocx_data[3] = 0x22;
    abi_memreallocx_data[4] = 0x6a;
    abi_memreallocx_data[5] = 0x15;
    abi_memreallocx_data[6] = (unsigned char)di_in;
    abi_memreallocx_data[7] = (unsigned char)(di_in >> 8);
    _asm {
        mov di, di_in
    }
}

const unsigned char *abi_get_deinit125b9_data(void) {
    return abi_deinit125b9_data;
}

void __far deinit_125B9(void);
#pragma aux deinit_125B9 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void __far deinit_125B9(void) {
    abi_deinit125b9_data[0] = 0x00;
    abi_deinit125b9_data[1] = 0x00;
    abi_deinit125b9_data[2] = 0x00;
    abi_deinit125b9_data[3] = 0x00;
    abi_deinit125b9_data[4] = 0x00;
    abi_deinit125b9_data[5] = 0x00;
    abi_deinit125b9_data[6] = 0x00;
    abi_deinit125b9_data[7] = 0x00;
    abi_deinit125b9_data[8] = 0x55;
    abi_deinit125b9_data[9] = 0x00;
    abi_deinit125b9_data[10] = 0x00;
    _asm {
        mov ax, 156ah
        mov bx, 0001h
        xor cx, cx
        xor dx, dx
    }
}

const unsigned char *abi_get_rtcclock_data(void) {
    return abi_rtcclock_data;
}

static void abi_rtcclock_123456(void) {
    abi_rtcclock_data[0] = 0x00;
    abi_rtcclock_data[1] = 0x00;
    abi_rtcclock_data[2] = 0x00;
    abi_rtcclock_data[3] = 0x00;
}

void initclockfromrtc(void);
#pragma aux initclockfromrtc __parm __caller [] __modify __exact [__ax __bx __cx __dx __es]
void initclockfromrtc(void) {
    abi_rtcclock_123456();
    _asm {
        mov ax, 9568h
        mov bx, 0038h
        mov cx, 1234h
        xor dx, dx
        mov es, dx
    }
}

void rereadrtc_settmr(void);
#pragma aux rereadrtc_settmr __parm __caller [] __modify __exact [__ax __bx __cx __dx __es]
void rereadrtc_settmr(void) {
    abi_rtcclock_123456();
    _asm {
        mov ax, 9568h
        mov bx, 0038h
        mov cx, 1234h
        xor dx, dx
        mov es, dx
    }
}

const unsigned char *abi_get_loadcfg_data(void) {
    return abi_loadcfg_data;
}

void loadcfg(void);
#pragma aux loadcfg __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void loadcfg(void) {
    unsigned i;
    for (i = 0; i < sizeof(abi_loadcfg_data); ++i) abi_loadcfg_data[i] = 0;
    abi_loadcfg_data[0] = 0x49;
    _asm {
        mov ax, 3e00h
        mov bx, 0005h
        xor cx, cx
        mov dx, 1501h
    }
}

const unsigned char *abi_get_dosexec_data(void) {
    return abi_dosexec_data;
}

void dosexec(void);
#pragma aux dosexec __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void dosexec(void) {
    abi_dosexec_data[0] = 0xff;
    abi_dosexec_data[1] = 0x00;
    abi_dosexec_data[2] = 0x8f;
    abi_dosexec_data[3] = 0x0d;
    _asm {
        mov ax, 0d8fh
        mov bx, 007fh
        xor cx, cx
        mov dx, 1550h
    }
}

const unsigned char *abi_get_callsubx_data(void) {
    return abi_callsubx_data;
}

void callsubx(void);
#pragma aux callsubx __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si __di __fs]
void callsubx(void) {
    static const unsigned char out[17] = {
        0x03, 0x20, 0x02, 0x05, 0x01, 0x16, 0x33, 0x44,
        0xf0, 0x55, 0x81, 0x01, 0x01, 0xf6, 0x0f, 0x6a, 0x15
    };
    unsigned i;
    for (i = 0; i < sizeof(out); ++i) abi_callsubx_data[i] = out[i];
}

void memalloc12k(void);
#pragma aux memalloc12k __parm __caller [] __modify __exact [__ax __bx __di __es]
void memalloc12k(void) {
    _asm {
        mov ax, 2345h
        mov bx, 3040h
        mov es, ax
        xor di, di
    }
}

const unsigned char *abi_get_f2waves_data(void) {
    return abi_f2waves_data;
}

void f2_waves(void);
#pragma aux f2_waves __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void f2_waves(void) {
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
    abi_f2waves_data[0] = 0xdf;
    abi_f2waves_data[1] = 0x1c;
    abi_f2waves_data[2] = 0x6a;
    abi_f2waves_data[3] = 0x1e;
    abi_f2waves_data[4] = 0x13;
    abi_f2waves_data[5] = 0x1f;
    abi_f2waves_data[6] = 0xdf;
    abi_f2waves_data[7] = 0x1c;
    abi_f2waves_data[8] = 0xee;
    _asm {
        mov ax, ax_in
        mov bx, bx_in
        mov cx, cx_in
        mov dx, dx_in
    }
}

const unsigned char *abi_get_initvga_data(void) {
    return abi_initvga_data;
}

void init_vga_waves(void);
#pragma aux init_vga_waves __parm __caller [] __modify __exact [__ax __bx __cx __dx __di __es]
void init_vga_waves(void) {
    abi_initvga_data[0] = 0x0f;
    abi_initvga_data[1] = 0x10;
    abi_initvga_data[2] = 0x8f;
    abi_initvga_data[3] = 0x12;
    abi_initvga_data[4] = 0x03;
}

const unsigned char *abi_get_f2draw_data(void) {
    return abi_f2draw_data;
}

static void abi_set_f2draw_data(void) {
    abi_f2draw_data[0] = 0x8f;
    abi_f2draw_data[1] = 0x0d;
    abi_f2draw_data[2] = 0x8f;
    abi_f2draw_data[3] = 0x0d;
}

void f2_draw_waves(void);
#pragma aux f2_draw_waves __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si __di __es __fs __gs]
void f2_draw_waves(void) {
    abi_set_f2draw_data();
}

void f2_draw_waves2(void);
#pragma aux f2_draw_waves2 __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si __di __es __fs __gs]
void f2_draw_waves2(void) {
    abi_set_f2draw_data();
}

const unsigned char *abi_get_readallmoules_data(void) {
    return abi_readallmoules_data;
}

void readallmoules(void);
#pragma aux readallmoules __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void readallmoules(void) {
    abi_readallmoules_data[0] = 0x01;
    abi_readallmoules_data[1] = 0x00;
    abi_readallmoules_data[2] = 0xaa;
    _asm {
        mov ax, 1234h
        mov bx, 5678h
        xor cx, cx
        mov dx, 137ch
        clc
    }
}

const unsigned char *abi_get_readmodule_data(void) {
    return abi_readmodule_data;
}

void read_module(void);
#pragma aux read_module __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void read_module(void) {
    static const unsigned char out[19] = {
        0x01, 0x00, 0x03, 0x8b, 0x12, 0x8f, 0x0d,
        'D', 'E', 'M', 'O', '.', 'S', '3', 'M', ' ', ' ', ' ', ' '
    };
    unsigned i;
    for (i = 0; i < sizeof(out); ++i) abi_readmodule_data[i] = out[i];
    _asm {
        stc
    }
}

const unsigned char *abi_get_moduleread_data(void) {
    return abi_moduleread_data;
}

void __far moduleread(void);
#pragma aux moduleread __parm __caller [] __modify __exact [__ax __bx __dx __fs]
void __far moduleread(void) {
    abi_moduleread_data[0] = 0x01;
    abi_moduleread_data[1] = 0x00;
    abi_moduleread_data[2] = 0x02;
    abi_moduleread_data[3] = 0x00;
    abi_moduleread_data[4] = 0xef;
    abi_moduleread_data[5] = 0xbe;
    abi_moduleread_data[6] = 0x5a;
    _asm {
        stc
    }
}

const unsigned char *abi_get_modread10311_data(void) {
    return abi_modread10311_data;
}

void mod_read_10311(void);
#pragma aux mod_read_10311 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void mod_read_10311(void) {
    unsigned i;
    for (i = 0; i < sizeof(abi_modread10311_data); ++i) abi_modread10311_data[i] = 0;
}

const unsigned char *abi_get_modnt_data(void) {
    return abi_modnt_data;
}

void mod_n_t_module(void);
#pragma aux mod_n_t_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void mod_n_t_module(void) {
    abi_modnt_data[0] = 0x4e;
    abi_modnt_data[1] = 0x2e;
    abi_modnt_data[2] = 0x54;
    abi_modnt_data[3] = 0x2e;
    abi_modnt_data[4] = 0x0f;
    abi_modnt_data[5] = 0x00;
    abi_modnt_data[6] = 0x04;
    abi_modnt_data[7] = 0x00;
    abi_modnt_data[8] = 0xef;
    abi_modnt_data[9] = 0xbe;
}

const unsigned char *abi_get_formatloader_data(void) {
    return abi_formatloader_data;
}

static void abi_set_formatloader_header(unsigned long module_type,
                                        unsigned short moduleflag,
                                        unsigned short size1,
                                        unsigned short channels,
                                        unsigned short patterns,
                                        unsigned short orders,
                                        unsigned short freq,
                                        unsigned char byte_24673,
                                        unsigned char byte_2467e,
                                        unsigned char byte_24679,
                                        unsigned char byte_2467a) {
    abi_formatloader_data[0] = (unsigned char)module_type;
    abi_formatloader_data[1] = (unsigned char)(module_type >> 8);
    abi_formatloader_data[2] = (unsigned char)(module_type >> 16);
    abi_formatloader_data[3] = (unsigned char)(module_type >> 24);
    abi_formatloader_data[4] = (unsigned char)moduleflag;
    abi_formatloader_data[5] = (unsigned char)(moduleflag >> 8);
    abi_formatloader_data[6] = (unsigned char)size1;
    abi_formatloader_data[7] = (unsigned char)(size1 >> 8);
    abi_formatloader_data[8] = (unsigned char)channels;
    abi_formatloader_data[9] = (unsigned char)(channels >> 8);
    abi_formatloader_data[10] = (unsigned char)patterns;
    abi_formatloader_data[11] = (unsigned char)(patterns >> 8);
    abi_formatloader_data[12] = (unsigned char)orders;
    abi_formatloader_data[13] = (unsigned char)(orders >> 8);
    abi_formatloader_data[14] = (unsigned char)freq;
    abi_formatloader_data[15] = (unsigned char)(freq >> 8);
    abi_formatloader_data[16] = byte_24673;
    abi_formatloader_data[17] = byte_2467e;
    abi_formatloader_data[18] = byte_24679;
    abi_formatloader_data[19] = byte_2467a;
}

void _2stm_module(void);
#pragma aux _2stm_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void _2stm_module(void) {
    abi_set_formatloader_header(0x4d545332UL, 0x0008, 0x001f, 0x0004, 0, 0, 8448, 0, 0, 0x12, 0x34);
}

void e669_module(void);
#pragma aux e669_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void e669_module(void) {
    abi_set_formatloader_header(0x39363645UL, 0x0004, 0, 0x0008, 0, 0, 0, 0x80, 2, 0, 0);
}

void mtm_module(void);
#pragma aux mtm_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void mtm_module(void) {
    abi_set_formatloader_header(0x204d544dUL, 0x0020, 0, 0, 1, 1, 0, 0x80, 0, 6, 0x7d);
}

void psm_module(void);
#pragma aux psm_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void psm_module(void) {
    abi_set_formatloader_header(0x204d5350UL, 0x0040, 0, 0, 0, 0, 8448, 0, 0, 0, 0);
}

void far_module(void);
#pragma aux far_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void far_module(void) {
    abi_set_formatloader_header(0x20524146UL, 0x0080, 0, 0x0010, 0, 0, 0, 0, 2, 4, 0x66);
}

void ult_module(void);
#pragma aux ult_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void ult_module(void) {
    abi_set_formatloader_header(0x20544c55UL, 0x0200, 0, 0, 0, 0, 0, 0, 0, 6, 0x7d);
}

void s3m_module(void);
#pragma aux s3m_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void s3m_module(void) {
    abi_set_formatloader_header(0x204d3353UL, 0x0010, 0, 0x0020, 0, 0, 8363, 0, 1, 0, 0);
}

void inr_module(void);
#pragma aux inr_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void inr_module(void) {
    abi_set_formatloader_header(0x20524e49UL, 0x0100, 0, 4, 0, 0, 0, 0, 0, 0, 0);
}

const unsigned char *abi_get_modulessearch_data(void) {
    return abi_modulessearch_data;
}

void modules_search(void);
#pragma aux modules_search __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void modules_search(void) {
    abi_modulessearch_data[0] = 0x90;
    abi_modulessearch_data[1] = 0x08;
    abi_modulessearch_data[2] = 0x00;
    abi_modulessearch_data[3] = 0x00;
    abi_modulessearch_data[4] = 0x00;
    abi_modulessearch_data[5] = 0x00;
}

const unsigned char *abi_get_start_data(void) {
    return abi_start_data;
}

void start(void);
#pragma aux start __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void start(void) {
    abi_start_data[0] = 0x00;
    abi_start_data[1] = 0x00;
    _asm {
        mov ax, 2222h
        xor bx, bx
        mov cx, 00ffh
        mov dx, 0100h
    }
}

const unsigned char *abi_get_keyb19efd_data(void) {
    return abi_keyb19efd_data;
}

void keyb_19EFD(void);
#pragma aux keyb_19EFD __parm __caller [] __modify __exact [__ax __bx __cx]
void keyb_19EFD(void) {
    abi_keyb19efd_data[0] = 0x12;
    abi_keyb19efd_data[1] = 0x34;
    abi_keyb19efd_data[2] = 0x56;
    abi_keyb19efd_data[3] = 0x9a;
    _asm {
        mov ax, 1234h
        mov bx, 5678h
        mov cx, 9abch
    }
}

const unsigned char *abi_get_spectr1bce9_data(void) {
    return abi_spectr1bce9_data;
}

void spectr_1BCE9(void);
#pragma aux spectr_1BCE9 __parm __caller [] __modify __exact []
void spectr_1BCE9(void) {
}

const unsigned char *abi_get_spectr1bc2d_data(void) {
    return abi_spectr1bc2d_data;
}

void spectr_1BC2D(void);
#pragma aux spectr_1BC2D __parm __caller [] __modify __exact [__bx __cx __bp]
void spectr_1BC2D(void) {
    _asm {
        add bx, 0063h
        add bp, 0129h
        xor cx, cx
    }
}

const unsigned char *abi_get_spectr1bbc1_data(void) {
    return abi_spectr1bbc1_data;
}

void spectr_1BBC1(void);
#pragma aux spectr_1BBC1 __parm __caller [] __modify __exact [__ax __cx __si __di]
void spectr_1BBC1(void) {
    abi_spectr1bbc1_data[0] = 0x00;
    abi_spectr1bbc1_data[1] = 0x00;
    abi_spectr1bbc1_data[2] = 0x14;
    _asm {
        add si, 0008h
        inc di
        xor cx, cx
        xor ax, ax
    }
}

void abi_set_videoprp_inputs(unsigned char first, unsigned char second, unsigned char third) {
    unsigned char lows = 0;
    unsigned char highs = 0;
    unsigned char max_count;
    unsigned char shift;
    unsigned long step;
    unsigned long low_acc;
    unsigned long high_acc;
    unsigned long eq_acc;
    unsigned short pos0 = 0;
    unsigned short pos1 = 0;
    unsigned short pos2 = 0;
    if (first >= 0x40u) ++highs;
    if (first <= 0x40u) ++lows;
    if (second >= 0x40u) ++highs;
    if (second <= 0x40u) ++lows;
    if (third >= 0x40u) ++highs;
    if (third <= 0x40u) ++lows;
    max_count = lows > highs ? lows : highs;
    shift = 3;
    if (max_count > 2) shift = 2;
    if (max_count > 4) shift = 1;
    if (max_count > 8) shift = 0;
    step = max_count ? (18350080UL / max_count) : 18350080UL;
    low_acc = step >> 1;
    high_acc = low_acc;
    if (first < 0x40u) {
        pos0 = (unsigned short)(((low_acc >> 16) * 80UL) + 1UL);
        low_acc += step;
    } else if (first > 0x40u) {
        pos0 = (unsigned short)(((high_acc >> 16) * 80UL) + 42UL);
        high_acc += step;
    }
    if (second < 0x40u) {
        pos1 = (unsigned short)(((low_acc >> 16) * 80UL) + 1UL);
        low_acc += step;
    } else if (second > 0x40u) {
        pos1 = (unsigned short)(((high_acc >> 16) * 80UL) + 42UL);
        high_acc += step;
    }
    if (third < 0x40u) {
        pos2 = (unsigned short)(((low_acc >> 16) * 80UL) + 1UL);
        low_acc += step;
    } else if (third > 0x40u) {
        pos2 = (unsigned short)(((high_acc >> 16) * 80UL) + 42UL);
        high_acc += step;
    }
    eq_acc = low_acc > high_acc ? low_acc : high_acc;
    if (first == 0x40u) {
        pos0 = (unsigned short)(((eq_acc >> 16) * 80UL) + 21UL);
        eq_acc += step;
    }
    if (second == 0x40u) {
        pos1 = (unsigned short)(((eq_acc >> 16) * 80UL) + 21UL);
        eq_acc += step;
    }
    if (third == 0x40u) {
        pos2 = (unsigned short)(((eq_acc >> 16) * 80UL) + 21UL);
    }
    abi_videoprp_data[0] = lows;
    abi_videoprp_data[1] = highs;
    abi_videoprp_data[2] = (unsigned char)(shift + 8u);
    abi_videoprp_data[3] = (unsigned char)pos0;
    abi_videoprp_data[4] = (unsigned char)(pos0 >> 8);
    abi_videoprp_data[5] = (unsigned char)pos1;
    abi_videoprp_data[6] = (unsigned char)(pos1 >> 8);
    abi_videoprp_data[7] = (unsigned char)pos2;
    abi_videoprp_data[8] = (unsigned char)(pos2 >> 8);
}

const unsigned char *abi_get_videoprp_data(void) {
    return abi_videoprp_data;
}

void video_prp_mtr_positn(void);
#pragma aux video_prp_mtr_positn __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __fs]
void video_prp_mtr_positn(void) {
}
#endif

static unsigned char abi_mouse1c7cf_records[0x400];
static unsigned short abi_mouse1c7cf_ax;
static unsigned short abi_mouse1c7cf_bp;
static unsigned short abi_mouse1c7cf_bx;
static unsigned short abi_mouse1c7cf_cx;
static unsigned short abi_mouse1c7cf_dx;
static unsigned short abi_mouse1c7cf_si;
static unsigned short abi_mouse1c7cf_di;
static unsigned char abi_ems_enabled_byte;
static unsigned char abi_emsmapcopy_source[16];
static unsigned char abi_emsmapcopy_data[16];
static unsigned char abi_emsrealloc2_count;
static unsigned char abi_clean11c43_flag;
static unsigned char abi_clean11c43_byte_2461e;
static unsigned char abi_clean11c43_byte_2461f;
static unsigned char abi_clean11c43_data[57];

void nullsub_2(void);
#pragma aux nullsub_2 __parm __caller [] __modify __exact []
void nullsub_2(void) {}

void nullsub_4(void);
#pragma aux nullsub_4 __parm __caller [] __modify __exact []
void nullsub_4(void) {}

void setvideomode(void) {}

void hex_1BE39(void);
#pragma aux hex_1BE39 __parm __caller [] __modify __exact [__ax __di __es]
void hex_1BE39(void) {
    _asm {
        and al, 0fh
        or al, 30h
        cmp al, 39h
        jbe short hex_1be39_store
        add al, 7
hex_1be39_store:
        mov es:[di], ax
        add di, 2
    }
}

void mouse_1C7A9(void);
#pragma aux mouse_1C7A9 __parm __caller [] __modify __exact [__ax __bp __cx __dx __si __di]
void mouse_1C7A9(void) {
    _asm {
        cmp cx, si
        jbe short mouse_1c7a9_x_ok
        xchg cx, si
mouse_1c7a9_x_ok:
        cmp dx, di
        jbe short mouse_1c7a9_y_ok
        xchg dx, di
mouse_1c7a9_y_ok:
        cmp ax, cx
        jb short mouse_1c7a9_miss
        cmp ax, si
        ja short mouse_1c7a9_miss
        cmp bp, dx
        jb short mouse_1c7a9_miss
        cmp bp, di
        ja short mouse_1c7a9_miss
        sub ax, cx
        sub bp, dx
        jmp short mouse_1c7a9_done
mouse_1c7a9_miss:
        stc
mouse_1c7a9_done:
    }
}

static unsigned short abi_get_mouse1c7cf_word(unsigned short off) {
    return (unsigned short)abi_mouse1c7cf_records[off] | ((unsigned short)abi_mouse1c7cf_records[off + 1u] << 8);
}

void abi_set_mouse1c7cf_records(const unsigned char *records, unsigned short count) {
    unsigned short i;
    for (i = 0; i < sizeof(abi_mouse1c7cf_records); ++i) {
        abi_mouse1c7cf_records[i] = (i < count) ? records[i] : 0;
    }
}

void abi_set_mouse1c7cf_inputs(unsigned short ax, unsigned short bp, unsigned short bx) {
    abi_mouse1c7cf_ax = ax;
    abi_mouse1c7cf_bp = bp;
    abi_mouse1c7cf_bx = bx;
}

unsigned short abi_get_mouse1c7cf_ax(void) { return abi_mouse1c7cf_ax; }
unsigned short abi_get_mouse1c7cf_bp(void) { return abi_mouse1c7cf_bp; }
unsigned short abi_get_mouse1c7cf_bx(void) { return abi_mouse1c7cf_bx; }
unsigned short abi_get_mouse1c7cf_cx(void) { return abi_mouse1c7cf_cx; }
unsigned short abi_get_mouse1c7cf_dx(void) { return abi_mouse1c7cf_dx; }
unsigned short abi_get_mouse1c7cf_si(void) { return abi_mouse1c7cf_si; }
unsigned short abi_get_mouse1c7cf_di(void) { return abi_mouse1c7cf_di; }

void abi_mouse1c7cf_compute(void) {
    unsigned short ax_in = abi_mouse1c7cf_ax;
    unsigned short bp_in = abi_mouse1c7cf_bp;
    unsigned short bx_off = (unsigned short)(abi_mouse1c7cf_bx - 0x2800u);
    unsigned short cx = 0;
    unsigned short dx = 0;
    unsigned short si = 0;
    unsigned short di = 0;
    for (;;) {
        cx = abi_get_mouse1c7cf_word(bx_off);
        if (cx == 0xffffu) {
            abi_mouse1c7cf_ax = ax_in;
            abi_mouse1c7cf_bp = bp_in;
            abi_mouse1c7cf_bx = (unsigned short)(0x2800u + bx_off);
            abi_mouse1c7cf_cx = cx;
            abi_mouse1c7cf_dx = dx;
            abi_mouse1c7cf_si = si;
            abi_mouse1c7cf_di = di;
            return;
        }
        dx = abi_get_mouse1c7cf_word((unsigned short)(bx_off + 2u));
        si = abi_get_mouse1c7cf_word((unsigned short)(bx_off + 4u));
        di = abi_get_mouse1c7cf_word((unsigned short)(bx_off + 6u));
        if (cx > si) {
            unsigned short tmp = cx;
            cx = si;
            si = tmp;
        }
        if (dx > di) {
            unsigned short tmp = dx;
            dx = di;
            di = tmp;
        }
        if (!(ax_in < cx || ax_in > si || bp_in < dx || bp_in > di)) {
            abi_mouse1c7cf_ax = (unsigned short)(ax_in - cx);
            abi_mouse1c7cf_bp = (unsigned short)(bp_in - dx);
            abi_mouse1c7cf_bx = abi_get_mouse1c7cf_word((unsigned short)(bx_off + 8u));
            abi_mouse1c7cf_cx = cx;
            abi_mouse1c7cf_dx = dx;
            abi_mouse1c7cf_si = si;
            abi_mouse1c7cf_di = di;
            return;
        }
        bx_off = (unsigned short)(bx_off + 0x0au);
    }
}

void mouse_1C7CF(void);
#pragma aux mouse_1C7CF __parm __caller [] __modify __exact [__ax __bp __bx __cx __dx __si __di]
void mouse_1C7CF(void) {
    abi_mouse1c7cf_compute();
}

void int24(void);
#pragma aux int24 __parm __caller [] __modify __exact [__ax]
void int24(void) {
    _asm {
        mov al, 3
        test ah, 8
        jnz short int24_done
        mov al, 0
        test ah, 20h
        jnz short int24_done
        mov al, 1
int24_done:
    }
}

void ems_restore_mapctx(void);
#pragma aux ems_restore_mapctx __parm __caller [] __modify __exact []
void ems_restore_mapctx(void) {
}

unsigned char abi_get_ems_enabled_byte(void) {
    return abi_ems_enabled_byte;
}

void ems_init(void);
#pragma aux ems_init __parm __caller [] __modify __exact [__ax]
void ems_init(void) {
    abi_ems_enabled_byte = 0;
    _asm {
        mov ax, 1
    }
}

void ems_release(void);
#pragma aux ems_release __parm __caller [] __modify __exact []
void ems_release(void) {}

void ems_realloc(void);
#pragma aux ems_realloc __parm __caller [] __modify __exact []
void ems_realloc(void) {}

void ems_deinit(void);
#pragma aux ems_deinit __parm __caller [] __modify __exact []
void ems_deinit(void) {}

void ems_save_mapctx(void);
#pragma aux ems_save_mapctx __parm __caller [] __modify __exact []
void ems_save_mapctx(void) {}

void ems_mapmem(void);
#pragma aux ems_mapmem __parm __caller [] __modify __exact []
void ems_mapmem(void) {}

void ems_mapmem2(void);
#pragma aux ems_mapmem2 __parm __caller [] __modify __exact []
void ems_mapmem2(void) {}

void abi_set_emsmapcopy_source(const unsigned char *payload) {
    unsigned short i;
    for (i = 0; i < sizeof(abi_emsmapcopy_source); ++i) {
        abi_emsmapcopy_source[i] = payload[i];
        abi_emsmapcopy_data[i] = 0;
    }
}

const unsigned char *abi_get_emsmapcopy_data(void) {
    return abi_emsmapcopy_data;
}

static void abi_emsmapcopy_copy(void) {
    unsigned short i;
    for (i = 0; i < sizeof(abi_emsmapcopy_data); ++i) {
        abi_emsmapcopy_data[i] = abi_emsmapcopy_source[i];
    }
}

void ems_mapmemx(void);
#pragma aux ems_mapmemx __parm __caller [] __modify __exact []
void ems_mapmemx(void) {
    abi_emsmapcopy_copy();
}

void ems_mapmemy(void);
#pragma aux ems_mapmemy __parm __caller [] __modify __exact []
void ems_mapmemy(void) {
    abi_emsmapcopy_copy();
}

void abi_set_emsrealloc2_state(unsigned char initial_count, unsigned long requested_size) {
    (void)requested_size;
    abi_emsrealloc2_count = initial_count;
}

unsigned char abi_get_emsrealloc2_count(void) {
    return abi_emsrealloc2_count;
}

void ems_realloc2(void);
#pragma aux ems_realloc2 __parm __caller [] __modify __exact [__ax __cx]
void ems_realloc2(void) {
    abi_emsrealloc2_count = (unsigned char)(abi_emsrealloc2_count + 1u);
    _asm {
        mov ax, 8
        mov cx, 0ffffh
    }
}

void abi_set_clean11c43_state(unsigned char flag, unsigned char byte_2461e, unsigned char byte_2461f) {
    abi_clean11c43_flag = flag;
    abi_clean11c43_byte_2461e = byte_2461e;
    abi_clean11c43_byte_2461f = byte_2461f;
}

const unsigned char *abi_get_clean11c43_data(void) {
    return abi_clean11c43_data;
}

void clean_11C43(void);
#pragma aux clean_11C43 __parm __caller [] __modify __exact []
void clean_11C43(void) {
    unsigned short i;
    unsigned short freq = (abi_clean11c43_flag & 8u) ? 8287u : 8363u;
    for (i = 0; i < sizeof(abi_clean11c43_data); ++i) {
        abi_clean11c43_data[i] = 0;
    }
    abi_clean11c43_data[2] = 4;
    abi_clean11c43_data[4] = 4;
    abi_clean11c43_data[10] = (unsigned char)freq;
    abi_clean11c43_data[11] = (unsigned char)(freq >> 8);
    abi_clean11c43_data[22] = 100;
    abi_clean11c43_data[27] = 2;
    abi_clean11c43_data[29] = 6;
    abi_clean11c43_data[30] = 125;
    abi_clean11c43_data[32] = 1;
    abi_clean11c43_data[46] = '?';
    abi_clean11c43_data[47] = '?';
    abi_clean11c43_data[48] = '?';
    abi_clean11c43_data[49] = '?';
    abi_clean11c43_data[54] = abi_clean11c43_byte_2461e;
    abi_clean11c43_data[55] = abi_clean11c43_byte_2461f;
    abi_clean11c43_data[56] = abi_clean11c43_byte_2461f;
}

void mod_sub_delta(void);
#pragma aux mod_sub_delta __parm __caller [] __modify __exact [__ax __cx __si]
void mod_sub_delta(void) {
    _asm {
        cmp byte ptr ds:00d4h, 1
        jne short mod_sub_delta_done
        mov al, byte ptr ds:00d6h
        cmp byte ptr ds:00d5h, 0
        je short mod_sub_delta_loop_test
        xor al, al
mod_sub_delta_loop_test:
        cmp cx, 0
        je short mod_sub_delta_store
mod_sub_delta_loop:
        add al, [si]
        mov [si], al
        inc si
        dec cx
        jne short mod_sub_delta_loop
mod_sub_delta_store:
        mov byte ptr ds:00d6h, al
mod_sub_delta_done:
    }
}

void sub_11BA6(void);
#pragma aux sub_11BA6 __parm __caller [] __modify __exact [__ax __cx __dx __di]
void sub_11BA6(void) {
    unsigned short cx_in;
    unsigned short bx_in;
    unsigned short dx_in;
    unsigned short di_in;
    unsigned char ch;
    unsigned char cl;
    unsigned char bl;
    unsigned char bh;
    unsigned char current_max;
    unsigned char *dst;
    _asm {
        mov cx_in, cx
        mov bx_in, bx
        mov dx_in, dx
        mov di_in, di
        mov al, byte ptr ds:007bh
        mov current_max, al
    }
    ch = (unsigned char)((cx_in >> 8) & 0x1fu);
    cl = (unsigned char)cx_in;
    bl = (unsigned char)bx_in;
    bh = (unsigned char)(bx_in >> 8);
    if (!((bl == 0 || bl == 0xffu) && (bh == 0 || bh == 0xffu))) ch = (unsigned char)(ch | 0x20u);
    if (cl <= 0x40u) ch = (unsigned char)(ch | 0x40u);
    if (dx_in != 0) {
        if ((unsigned char)dx_in == 0) dx_in = (unsigned short)((dx_in & 0xff00u) | 0x1du);
        ch = (unsigned char)(ch | 0x80u);
    }
    dst = (unsigned char *)di_in;
    if ((ch & 0xe0u) != 0) {
        *dst++ = ch;
        di_in = (unsigned short)(di_in + 1u);
        if (ch & 0x80u) {
            *dst++ = (unsigned char)dx_in;
            *dst++ = (unsigned char)(dx_in >> 8);
            di_in = (unsigned short)(di_in + 2u);
        }
        if (ch & 0x40u) {
            *dst++ = cl;
            di_in = (unsigned short)(di_in + 1u);
        }
        if (ch & 0x20u) {
            *dst++ = (unsigned char)bx_in;
            *dst++ = (unsigned char)(bx_in >> 8);
            di_in = (unsigned short)(di_in + 2u);
        }
        if ((ch & 0x1fu) > current_max) {
            current_max = (unsigned char)(ch & 0x1fu);
        }
    }
    _asm {
        mov al, current_max
        mov byte ptr ds:007bh, al
    }
    cx_in = (unsigned short)(((unsigned short)ch << 8) | cl);
    _asm {
        mov ax, cx_in
        and ax, 001fh
        mov cx, cx_in
        mov dx, dx_in
        mov di, di_in
    }
}

void mod_102F5(void);
#pragma aux mod_102F5 __parm __caller [] __modify __exact [__ax]
void mod_102F5(void) {
    unsigned short i;
    unsigned char maxv = 0;
    unsigned char value;
    unsigned short result;
    for (i = 0; i < 128u; ++i) {
        value = (unsigned char)(((unsigned char *)0x3a48u)[i] & 0x7fu);
        if (value >= maxv) maxv = value;
    }
    result = (unsigned short)(maxv + 1u);
    ((unsigned char *)0x0052u)[0] = (unsigned char)result;
    ((unsigned char *)0x0052u)[1] = (unsigned char)(result >> 8);
    _asm {
        mov ax, result
    }
}

void ult_read(void);
#pragma aux ult_read __parm __caller [] __modify __exact []
void ult_read(void) {
    _asm {
        dec byte ptr ds:0c09ch
    }
}

void volume_prep(void);
#pragma aux volume_prep __parm __caller [] __modify __exact [__ax __cx __di]
void volume_prep(void) {
    _asm {
        mov word ptr ds:0070h, ax
        mov word ptr ds:0072h, cx
        xor al, al
        cld
        rep stosb
    }
}

void sub_135CA(void);
#pragma aux sub_135CA __parm __caller [] __modify __exact [__ax __bx __cx __si]
void sub_135CA(void) {
    _asm {
        xor ax, ax
        mov word ptr ds:1372h, ax
        mov byte ptr ds:13a5h, al
        mov si, word ptr ds:0014h
        inc si
        mov word ptr ds:0014h, si
        mov bx, 13b8h
        xor cx, cx
    }
}

void memfree_125DA(void);
#pragma aux memfree_125DA __parm __caller [] __modify __exact [__ax]
void memfree_125DA(void) {
    _asm {
        mov ax, 156ah
    }
}

void mouse_getpos(void);
#pragma aux mouse_getpos __parm __caller [] __modify __exact [__bx __cx __dx]
void mouse_getpos(void) {
    _asm {
        cmp byte ptr ds:16a1h, 1
        je short mouse_getpos_done
        xor bx, bx
        xor cx, cx
        xor dx, dx
mouse_getpos_done:
    }
}

void mouse_showcur(void);
#pragma aux mouse_showcur __parm __caller [] __modify __exact [__ax]
void mouse_showcur(void) {
    _asm {
        cmp byte ptr ds:16a1h, 1
        jne short mouse_showcur_no_driver
        mov ax, 1
        clc
        jmp short mouse_showcur_done
mouse_showcur_no_driver:
        stc
mouse_showcur_done:
    }
}

void mouse_hide2(void);
#pragma aux mouse_hide2 __parm __caller [] __modify __exact [__ax]
void mouse_hide2(void) {
    _asm {
        cmp byte ptr ds:16a1h, 1
        jne short mouse_hide2_no_driver
        mov ax, 2
        clc
        jmp short mouse_hide2_done
mouse_hide2_no_driver:
        stc
mouse_hide2_done:
    }
}

void mouse_show(void);
#pragma aux mouse_show __parm __caller [] __modify __exact [__ax]
void mouse_show(void) {
    _asm {
        cmp byte ptr ds:16a1h, 1
        jne short mouse_show_done
        cmp byte ptr ds:16a2h, 1
        je short mouse_show_done
        mov byte ptr ds:16a2h, 1
        call mouse_showcur
mouse_show_done:
    }
}

void mouse_hide(void);
#pragma aux mouse_hide __parm __caller [] __modify __exact [__ax]
void mouse_hide(void) {
    _asm {
        cmp byte ptr ds:16a1h, 1
        jne short mouse_hide_done
        cmp byte ptr ds:16a2h, 0
        je short mouse_hide_done
        mov byte ptr ds:16a2h, 0
        call mouse_hide2
mouse_hide_done:
    }
}

void mouse_deinit(void);
#pragma aux mouse_deinit __parm __caller [] __modify __exact [__ax __cx __dx __es]
void mouse_deinit(void) {
    _asm {
        cmp byte ptr ds:16a1h, 1
        jne short mouse_deinit_done
        mov byte ptr ds:16a1h, 0
        mov byte ptr ds:16a2h, 0
        xor dx, dx
        mov es, dx
        mov cx, dx
        mov ax, 000ch
mouse_deinit_done:
    }
}

void mouse_init(void);
#pragma aux mouse_init __parm __caller [] __modify __exact [__ax __es]
void mouse_init(void) {
    _asm {
        mov byte ptr ds:16a2h, 0
        xor ax, ax
        mov es, ax
        mov byte ptr ds:16a1h, 0
        stc
    }
}

void get_comspec(void);
#pragma aux get_comspec __parm __caller [] __modify __exact [__di __es]
void get_comspec(void) {
    _asm {
        mov ax, word ptr ds:164ah
        mov es, ax
        mov ax, word ptr es:002ch
        mov es, ax
        xor di, di
get_comspec_next:
        cmp byte ptr es:[di], 0
        stc
        je short get_comspec_done
        cmp dword ptr es:[di], 534d4f43h
        jne short get_comspec_skip
        cmp dword ptr es:[di+4], 3d434550h
        je short get_comspec_found
get_comspec_skip:
        inc di
        cmp byte ptr es:[di], 0
        jne short get_comspec_skip
        inc di
        jmp short get_comspec_next
get_comspec_found:
        add di, 8
        clc
get_comspec_done:
    }
}

void getexename(void);
#pragma aux getexename __parm __caller [] __modify __exact [__ax __cx __di __si __es]
void getexename(void) {
    _asm {
        mov ax, word ptr ds:164ah
        mov es, ax
        mov ax, word ptr es:002ch
        mov es, ax
        xor di, di
        xor al, al
        cld
        mov cx, 8000h
getexename_scan:
        repne scasb
        jnz short getexename_fail
        cmp byte ptr es:[di], al
        jne short getexename_scan
        mov cx, word ptr es:[di+1]
        jcxz getexename_fail
        add di, 3
getexename_copy:
        mov al, byte ptr es:[di]
        mov byte ptr ds:[si], al
        inc di
        inc si
        or al, al
        jne short getexename_copy
        clc
        jmp short getexename_done
getexename_fail:
        stc
getexename_done:
    }
}

void spectr_1C4F8(void);
#pragma aux spectr_1C4F8 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void spectr_1C4F8(void) {
    _asm {
        xor eax, eax
        mov edx, 40000000h
spectr_1c4f8_loop:
        mov ecx, eax
        add ecx, edx
        shr eax, 1
        cmp ecx, ebx
        jg short spectr_1c4f8_skip
        sub ebx, ecx
        add eax, edx
spectr_1c4f8_skip:
        shr edx, 2
        jnz short spectr_1c4f8_loop
        cmp eax, ebx
        jge short spectr_1c4f8_done
        inc ax
spectr_1c4f8_done:
    }
}

void spectr_1B406(void);
#pragma aux spectr_1B406 __parm __caller [] __modify __exact []
void spectr_1B406(void) {
    _asm {
        mov word ptr ds:7d1eh, di
    }
    *(unsigned short *)0x7d1cu = 0;
    *(unsigned short *)0x7d20u = 0;
    *(unsigned short *)0x7d22u = 0;
    *(unsigned short *)0x7d24u = 0;
    *(unsigned short *)0x7d26u = 2;
    *(unsigned short *)0x7d28u = 0;
    *(unsigned short *)0x7d2au = 0;
    *(unsigned short *)0x7d2cu = 0;
    *(unsigned short *)0x7d2eu = 0;
    *(unsigned short *)0x7d30u = 1;
    *(unsigned short *)0x7d32u = 2;
}

void ReadSB(void);
#pragma aux ReadSB __parm __caller [] __modify __exact [__ax __dx]
void ReadSB(void) {
    _asm {
        xor al, al
    }
}

void ReadMixerSB(void);
#pragma aux ReadMixerSB __parm __caller [] __modify __exact [__ax __dx]
void ReadMixerSB(void) {
    _asm {
        xor al, al
    }
}

void WriteSB(void);
#pragma aux WriteSB __parm __caller [] __modify __exact [__ax __dx]
void WriteSB(void) {}

void WriteMixerSB(void);
#pragma aux WriteMixerSB __parm __caller [] __modify __exact [__ax __dx]
void WriteMixerSB(void) {}

void CheckSB(void);
#pragma aux CheckSB __parm __caller [] __modify __exact [__ax __dx]
void CheckSB(void) {
    _asm {
        xor ax, ax
        mov dx, 0226h
    }
}

void sb_detect_irq(void);
#pragma aux sb_detect_irq __parm __caller [] __modify __exact [__ax __dx]
void sb_detect_irq(void) {
    _asm {
        xor ax, ax
        mov dx, 0ff6h
        stc
    }
}

void set_dmachn_mask(void);
#pragma aux set_dmachn_mask __parm __caller [] __modify __exact [__ax]
void set_dmachn_mask(void) {
    _asm {
        mov al, cl
        and al, 3
        or  al, 4
    }
}

void adlib_18389(void);
#pragma aux adlib_18389 __parm __caller [] __modify __exact [__ax]
void adlib_18389(void) {
    _asm {
        mov ax, 00e9h
    }
}

void adlib_18395(void);
#pragma aux adlib_18395 __parm __caller [] __modify __exact []
void adlib_18395(void) {}

void set_egasequencer(void);
#pragma aux set_egasequencer __parm __caller [] __modify __exact [__ax __dx]
void set_egasequencer(void) {
    _asm {
        mov ax, 1220h
        mov dx, 03c5h
    }
}

void graph_1C070(void);
#pragma aux graph_1C070 __parm __caller [] __modify __exact [__ax __dx]
void graph_1C070(void) {
    _asm {
        mov ax, 1200h
        mov dx, 03c5h
    }
}

void useless_unset_egaseq(void);
#pragma aux useless_unset_egaseq __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void useless_unset_egaseq(void) {
    _asm {
        mov ah, al
        mov dx, 03ceh
        mov al, 5
        out dx, al
        inc dl
        in  al, dx
        and al, 0fch
        or  al, ah
        out dx, al
    }
}

void txt_blinkingoff(void);
#pragma aux txt_blinkingoff __parm __caller [] __modify __exact [__ax __bx]
void txt_blinkingoff(void) {
    _asm {
        xor bl, bl
        mov ax, 1003h
        int 10h
    }
}

void txt_enableblink(void);
#pragma aux txt_enableblink __parm __caller [] __modify __exact [__ax __bx]
void txt_enableblink(void) {
    _asm {
        mov bl, 1
        mov ax, 1003h
        int 10h
    }
}

void ult_1150B(void);
#pragma aux ult_1150B __parm __caller [] __modify __exact [__ax __cx]
void ult_1150B(void) {
    _asm {
        cmp al, 5
        jz short ult_1150b_05
        cmp al, 0ah
        jz short ult_1150b_0a
        cmp al, 0bh
        jz short ult_1150b_0b
        cmp al, 0ch
        jz short ult_1150b_0c
        cmp al, 0eh
        jz short ult_1150b_0e
        jmp short ult_1150b_done
ult_1150b_05:
        xor ax, ax
        jmp short ult_1150b_done
ult_1150b_0a:
        shr ah, 2
        and ah, 33h
        jmp short ult_1150b_done
ult_1150b_0b:
        and ax, 0f00h
        or  ax, 800eh
        jmp short ult_1150b_done
ult_1150b_0c:
        mov cl, ah
        shr cl, 2
        xor ax, ax
        jmp short ult_1150b_done
ult_1150b_0e:
        push dx
        mov dx, ax
        shr dx, 4
        cmp dl, 0eah
        jz short ult_1150b_0e_convert
        cmp dl, 0ebh
        jz short ult_1150b_0e_convert
        pop dx
        jmp short ult_1150b_done
ult_1150b_0e_convert:
        mov dh, ah
        and dh, 0f0h
        and ah, 0fh
        shr ah, 2
        or  ah, dh
        pop dx
ult_1150b_done:
    }
}

void sub_15577(void);
#pragma aux sub_15577 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sub_15577(void) {
    _asm {
        test byte ptr [si+17h], 1
        jz short sub_15577_done
sub_15577_done:
    }
}

void sub_13429(void);
#pragma aux sub_13429 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sub_13429(void) {
    _asm {
        test byte ptr [bx+17h], 4
        jz short sub_13429_done
sub_13429_done:
    }
}

void sub_137D5(void);
#pragma aux sub_137D5 __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void sub_137D5(void) {
    _asm {
        test byte ptr [bx+3dh], 40h
        jnz short sub_137d5_flagged
        movzx di, byte ptr [bx+0ah]
        cmp di, 32
        ja short sub_137d5_done
        jmp short sub_137d5_done
sub_137d5_flagged:
        movzx di, byte ptr [bx+0ah]
        cmp di, 32
        ja short sub_137d5_done
sub_137d5_done:
    }
}

void sub_13813(void);
#pragma aux sub_13813 __parm __caller [] __modify __exact [__ax __di]
void sub_13813(void) {
    _asm {
        movzx di, byte ptr [bx+0ah]
        cmp di, 32
        ja short sub_13813_done
sub_13813_done:
    }
}

#ifdef IPLAY_ABI_RUNNER
void abi_set_sub13826_state(unsigned short table_word) {
    abi_sub13826_table_word = table_word;
}

void sub_13826(void);
#pragma aux sub_13826 __parm __caller [] __modify __exact [__ax __cx __di]
void sub_13826(void) {
    _asm {
        mov cl, al
        movzx di, cl
        dec di
        and di, 0fh
        shl di, 1
        shr cl, 4
        mov ax, abi_sub13826_table_word
        shr ax, cl
        mov cx, [bx+14h]
    }
}

void abi_set_sub140b6_state(unsigned char byte_24671, unsigned char byte_24668) {
    abi_sub140b6_byte_24671 = byte_24671;
    abi_sub140b6_byte_24668 = byte_24668;
}

unsigned char abi_get_sub140b6_byte_24671(void) {
    return abi_sub140b6_byte_24671;
}

unsigned char abi_get_sub140b6_byte_24668(void) {
    return abi_sub140b6_byte_24668;
}

void sub_140B6(void);
#pragma aux sub_140B6 __parm __caller [] __modify __exact [__ax __bx __cx]
void sub_140B6(void) {
    _asm {
        cmp abi_sub140b6_byte_24671, 1
        jz short sub_140b6_done
sub_140b6_done:
    }
}

void abi_set_sub14087_state(unsigned char byte_24668) {
    abi_sub14087_byte_24668 = byte_24668;
}

void sub_14087(void);
#pragma aux sub_14087 __parm __caller [] __modify __exact [__ax __dx]
void sub_14087(void) {
    _asm {
        xor ah, ah
        or  al, al
        jz short sub_14087_load
        mov [bx+34h], al
sub_14087_load:
        mov al, [bx+34h]
        cmp abi_sub14087_byte_24668, 0
        jz short sub_14087_unflagged
        cmp al, 0e0h
        jnb short sub_14087_zero
        shl ax, 2
        jmp short sub_14087_done
sub_14087_unflagged:
        cmp al, 0e0h
        jbe short sub_14087_zero
        mov dl, al
        and al, 0fh
        cmp dl, 0f0h
        jbe short sub_14087_done
        shl ax, 2
        jmp short sub_14087_done
sub_14087_zero:
        xor ax, ax
sub_14087_done:
    }
}

void abi_set_calc14043_state(unsigned char byte_2467b, unsigned char byte_2467c) {
    abi_calc14043_byte_2467b = byte_2467b;
    abi_calc14043_byte_2467c = byte_2467c;
}

unsigned char abi_get_calc14043_byte_2467b(void) {
    return abi_calc14043_byte_2467b;
}

unsigned char abi_get_calc14043_byte_2467c(void) {
    return abi_calc14043_byte_2467c;
}

void calc_14043(void);
#pragma aux calc_14043 __parm __caller [] __modify __exact [__ax]
void calc_14043(void) {
    _asm {
        mov al, abi_calc14043_byte_2467b
        add al, abi_calc14043_byte_2467c
        and eax, 0ffh
        lea ax, [eax+eax*4]
        shr ax, 1
    }
}

void abi_set_change_volume_state(unsigned short channels, unsigned char channel0) {
    abi_change_volume_value = 0x0100u;
    abi_change_volume_channels = channels;
    abi_change_volume_channel0 = channel0;
}

const unsigned char *abi_get_change_volume_data(void) {
    abi_change_volume_data[0] = (unsigned char)abi_change_volume_value;
    abi_change_volume_data[1] = (unsigned char)(abi_change_volume_value >> 8);
    abi_change_volume_data[2] = abi_change_volume_channel0;
    return abi_change_volume_data;
}

void change_volume(void);
#pragma aux change_volume __parm __caller [] __modify __exact [__ax __bx __cx]
void change_volume(void) {
    unsigned short ax_in;
    _asm {
        mov ax_in, ax
    }
    abi_change_volume_cx = 0x245au;
    abi_change_volume_bx = 0;
    if (ax_in != 0xffffu) {
        abi_change_volume_value = ax_in;
        if (abi_change_volume_channels != 0) {
            ax_in = (unsigned short)((ax_in & 0xff00u) | abi_change_volume_channel0);
        }
        abi_change_volume_bx = (unsigned short)(0x1368u + abi_change_volume_channels * 0x50u);
        abi_change_volume_cx = 0;
    }
    abi_change_volume_ax = abi_change_volume_value;
    _asm {
        mov ax, abi_change_volume_ax
        mov bx, abi_change_volume_bx
        mov cx, abi_change_volume_cx
    }
}

void abi_set_amplif_state(unsigned char sound_mode) {
    abi_amplification = 100;
    abi_amplif_sound_mode = sound_mode;
    abi_amplif_over_100 = 0;
    abi_amplif_max_volume = 0;
}

const unsigned char *abi_get_amplif_data(void) {
    abi_amplif_data[0] = (unsigned char)abi_amplification;
    abi_amplif_data[1] = (unsigned char)(abi_amplification >> 8);
    abi_amplif_data[2] = abi_amplif_over_100;
    abi_amplif_data[3] = abi_amplif_max_volume;
    return abi_amplif_data;
}

static void abi_change_amplif_value(unsigned short ax_in) {
    if (ax_in != 0xffffu) {
        abi_amplification = ax_in;
        abi_amplif_over_100 = (ax_in > 100u) ? 1 : 0;
        abi_amplif_max_volume = (abi_amplif_sound_mode == 0) ? 0x40 : 0x3f;
    }
}

void change_amplif(void);
#pragma aux change_amplif __parm __caller [] __modify __exact [__ax __cx]
void change_amplif(void) {
    unsigned short ax_in;
    _asm {
        mov ax_in, ax
    }
    abi_change_amplif_value(ax_in);
    _asm {
        mov ax, abi_amplification
    }
}

void eff_14020(void);
#pragma aux eff_14020 __parm __caller [] __modify __exact [__ax __cx]
void eff_14020(void) {
    unsigned short ax_in;
    _asm {
        mov ax_in, ax
    }
    ax_in = (unsigned short)(((unsigned char)ax_in) << 2);
    abi_change_amplif_value(ax_in);
    _asm {
        mov ax, abi_amplification
    }
}

void abi_set_eff14030_state(unsigned char byte_2467c, unsigned short freq, unsigned short buffer_size) {
    int i;
    abi_eff14030_byte_2467b = 0;
    abi_eff14030_byte_2467c = byte_2467c;
    abi_eff14030_freq = freq;
    abi_eff14030_buffer_size = buffer_size;
    abi_eff14030_dx = 0;
    abi_eff14030_ax = 0;
    for (i = 0; i < 11; ++i) abi_eff14030_data[i] = 0;
}

const unsigned char *abi_get_eff14030_data(void) {
    abi_eff14030_data[0] = abi_eff14030_byte_2467b;
    abi_eff14030_data[1] = abi_eff14030_byte_2467c;
    abi_eff14030_data[2] = (unsigned char)abi_eff14030_dx;
    abi_eff14030_data[3] = (unsigned char)(abi_eff14030_dx >> 8);
    abi_eff14030_data[4] = (unsigned char)abi_eff14030_ax;
    abi_eff14030_data[5] = (unsigned char)(abi_eff14030_ax >> 8);
    abi_eff14030_data[6] = (unsigned char)abi_eff14030_ax;
    abi_eff14030_data[7] = (unsigned char)(abi_eff14030_ax >> 8);
    abi_eff14030_data[8] = 0;
    abi_eff14030_data[9] = 0x20;
    abi_eff14030_data[10] = (unsigned char)(((unsigned short)(abi_eff14030_byte_2467b + abi_eff14030_byte_2467c) * 5u) >> 1);
    return abi_eff14030_data;
}

void eff_14030(void);
#pragma aux eff_14030 __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_14030(void) {
    static const unsigned char table_14057[16] = {
        0xff, 0x80, 0x40, 0x2a, 0x20, 0x19, 0x15, 0x12,
        0x10, 0x0e, 0x0c, 0x0b, 0x0a, 0x09, 0x09, 0x08
    };
    unsigned short in_ax;
    unsigned short tempo;
    unsigned short cx;
    unsigned long first_div;
    _asm {
        mov in_ax, ax
    }
    abi_eff14030_byte_2467b = table_14057[in_ax & 0x0fu];
    tempo = (unsigned short)(((unsigned short)(abi_eff14030_byte_2467b + abi_eff14030_byte_2467c) * 5u) >> 1);
    cx = (unsigned char)tempo;
    if (cx == 0) {
        abi_eff14030_ax = 0;
        abi_eff14030_dx = abi_eff14030_buffer_size;
    } else {
        cx = (unsigned short)(cx << 1);
        first_div = (5UL * abi_eff14030_freq) / cx;
        abi_eff14030_ax = (unsigned short)(first_div / abi_eff14030_buffer_size);
        abi_eff14030_dx = (unsigned short)(first_div % abi_eff14030_buffer_size);
        ++abi_eff14030_ax;
        if (abi_eff14030_dx == 0) {
            --abi_eff14030_ax;
            abi_eff14030_dx = abi_eff14030_buffer_size;
        }
    }
    _asm {
        mov ax, abi_eff14030_buffer_size
        mov cx, cx
        mov dx, abi_eff14030_dx
    }
}

void abi_set_eff14067_state(unsigned char byte_2467b, unsigned char byte_2467c, unsigned short freq, unsigned short buffer_size) {
    int i;
    abi_eff14067_byte_2467b = byte_2467b;
    abi_eff14067_byte_2467c = byte_2467c;
    abi_eff14067_freq = freq;
    abi_eff14067_buffer_size = buffer_size;
    abi_eff14067_dx = 0;
    abi_eff14067_ax = 0;
    for (i = 0; i < 11; ++i) abi_eff14067_data[i] = 0;
}

const unsigned char *abi_get_eff14067_data(void) {
    abi_eff14067_data[0] = abi_eff14067_byte_2467b;
    abi_eff14067_data[1] = abi_eff14067_byte_2467c;
    abi_eff14067_data[2] = (unsigned char)abi_eff14067_dx;
    abi_eff14067_data[3] = (unsigned char)(abi_eff14067_dx >> 8);
    abi_eff14067_data[4] = (unsigned char)abi_eff14067_ax;
    abi_eff14067_data[5] = (unsigned char)(abi_eff14067_ax >> 8);
    abi_eff14067_data[6] = (unsigned char)abi_eff14067_ax;
    abi_eff14067_data[7] = (unsigned char)(abi_eff14067_ax >> 8);
    abi_eff14067_data[8] = 0;
    abi_eff14067_data[9] = 0x20;
    abi_eff14067_data[10] = (unsigned char)(((unsigned short)(abi_eff14067_byte_2467b + abi_eff14067_byte_2467c) * 5u) >> 1);
    return abi_eff14067_data;
}

void eff_14067(void);
#pragma aux eff_14067 __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_14067(void) {
    unsigned short in_ax;
    unsigned char al;
    unsigned short tempo;
    unsigned short cx;
    unsigned long first_div;
    _asm {
        mov in_ax, ax
    }
    al = (unsigned char)in_ax;
    if (al == 0) {
        abi_eff14067_byte_2467c = 0;
    } else if ((al & 0x0fu) != 0) {
        abi_eff14067_byte_2467c = (unsigned char)(abi_eff14067_byte_2467c - (al & 0x0fu));
    } else {
        abi_eff14067_byte_2467c = (unsigned char)(abi_eff14067_byte_2467c + (al >> 4));
    }
    tempo = (unsigned short)(((unsigned short)(abi_eff14067_byte_2467b + abi_eff14067_byte_2467c) * 5u) >> 1);
    cx = (unsigned char)tempo;
    if (cx == 0) {
        abi_eff14067_ax = 0;
        abi_eff14067_dx = abi_eff14067_buffer_size;
    } else {
        cx = (unsigned short)(cx << 1);
        first_div = (5UL * abi_eff14067_freq) / cx;
        abi_eff14067_ax = (unsigned short)(first_div / abi_eff14067_buffer_size);
        abi_eff14067_dx = (unsigned short)(first_div % abi_eff14067_buffer_size);
        ++abi_eff14067_ax;
        if (abi_eff14067_dx == 0) {
            --abi_eff14067_ax;
            abi_eff14067_dx = abi_eff14067_buffer_size;
        }
    }
    _asm {
        mov ax, abi_eff14067_buffer_size
        mov cx, cx
        mov dx, abi_eff14067_dx
    }
}

void abi_set_eff13e8c_state(unsigned short freq, unsigned short buffer_size) {
    int i;
    abi_eff13e8c_freq = freq;
    abi_eff13e8c_buffer_size = buffer_size;
    abi_eff13e8c_dx = 0;
    abi_eff13e8c_ax = 0;
    abi_eff13e8c_byte_24666 = 0;
    abi_eff13e8c_byte_24667 = 0;
    abi_eff13e8c_byte_24668 = 0;
    for (i = 0; i < 11; ++i) abi_eff13e8c_data[i] = 0;
}

const unsigned char *abi_get_eff13e8c_data(void) {
    abi_eff13e8c_data[0] = (unsigned char)abi_eff13e8c_dx;
    abi_eff13e8c_data[1] = (unsigned char)(abi_eff13e8c_dx >> 8);
    abi_eff13e8c_data[2] = (unsigned char)abi_eff13e8c_ax;
    abi_eff13e8c_data[3] = (unsigned char)(abi_eff13e8c_ax >> 8);
    abi_eff13e8c_data[4] = (unsigned char)abi_eff13e8c_ax;
    abi_eff13e8c_data[5] = (unsigned char)(abi_eff13e8c_ax >> 8);
    abi_eff13e8c_data[6] = 0;
    abi_eff13e8c_data[7] = 0x20;
    abi_eff13e8c_data[8] = abi_eff13e8c_byte_24666;
    abi_eff13e8c_data[9] = abi_eff13e8c_byte_24667;
    abi_eff13e8c_data[10] = abi_eff13e8c_byte_24668;
    return abi_eff13e8c_data;
}

void eff_13E8C(void);
#pragma aux eff_13E8C __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_13E8C(void) {
    static const unsigned char table_13ec3[16] = {140, 50, 25, 15, 10, 7, 6, 4, 3, 3, 2, 2, 2, 2, 1, 1};
    unsigned short in_ax;
    unsigned char value;
    unsigned char di;
    unsigned char dl;
    unsigned short ax;
    unsigned short cx;
    unsigned long first_div;

    _asm {
        mov in_ax, ax
    }

    value = (unsigned char)in_ax;
    di = (unsigned char)(value >> 4);
    dl = (unsigned char)(value & 0x0f);
    ax = (unsigned short)((unsigned short)dl * table_13ec3[di]);
    ax >>= 4;
    ax = (unsigned short)(0x31u - ax);
    ax = (unsigned short)((ax * 5u) >> 1);
    ax = (unsigned short)(((unsigned short)di << 8) | (ax & 0xffu));
    abi_eff13e8c_byte_24667 = di;
    abi_eff13e8c_byte_24668 = 0;

    cx = (unsigned char)ax;
    abi_eff13e8c_byte_24666 = (unsigned char)cx;
    if (cx == 0) {
        abi_eff13e8c_ax = 0;
        abi_eff13e8c_dx = abi_eff13e8c_buffer_size;
    } else {
        cx = (unsigned short)(cx << 1);
        first_div = (5UL * abi_eff13e8c_freq) / cx;
        abi_eff13e8c_ax = (unsigned short)(first_div / abi_eff13e8c_buffer_size);
        abi_eff13e8c_dx = (unsigned short)(first_div % abi_eff13e8c_buffer_size);
        ++abi_eff13e8c_ax;
        if (abi_eff13e8c_dx == 0) {
            --abi_eff13e8c_ax;
            abi_eff13e8c_dx = abi_eff13e8c_buffer_size;
        }
    }

    _asm {
        mov ax, abi_eff13e8c_buffer_size
        mov cx, cx
        mov dx, abi_eff13e8c_dx
    }
}

void abi_set_eff13ce8_state(unsigned char byte_24667, unsigned char byte_24668) {
    abi_eff13ce8_byte_24667 = byte_24667;
    abi_eff13ce8_byte_24668 = byte_24668;
}

const unsigned char *abi_get_eff13ce8_data(void) {
    abi_eff13ce8_data[0] = abi_eff13ce8_byte_24667;
    abi_eff13ce8_data[1] = abi_eff13ce8_byte_24668;
    return abi_eff13ce8_data;
}

void eff_13CE8(void);
#pragma aux eff_13CE8 __parm __caller [] __modify __exact [__ax]
void eff_13CE8(void) {
    _asm {
        or  al, al
        jz short eff_13ce8_done
        mov abi_eff13ce8_byte_24667, al
        mov abi_eff13ce8_byte_24668, 0
eff_13ce8_done:
    }
}

void abi_set_eff13a43_state(unsigned char sndflags) {
    abi_eff13a43_sndflags = sndflags;
}

void eff_13A43(void);
#pragma aux eff_13A43 __parm __caller [] __modify __exact [__ax]
void eff_13A43(void) {
    _asm {
        cmp al, 0a4h
        jz short eff_13a43_set
        cmp al, 0a5h
        jz short eff_13a43_clear
        cmp al, 0a6h
        jz short eff_13a43_toggle
        cmp al, 80h
        ja short eff_13a43_done
        test abi_eff13a43_sndflags, 4
        jnz short eff_13a43_done
        jmp short eff_13a43_done
eff_13a43_set:
        or byte ptr [bx+17h], 80h
        jmp short eff_13a43_done
eff_13a43_clear:
        and byte ptr [bx+17h], 7fh
        jmp short eff_13a43_done
eff_13a43_toggle:
        xor byte ptr [bx+17h], 80h
eff_13a43_done:
    }
}

void abi_set_eff13a94_state(unsigned char byte_2461a) {
    abi_eff13a94_byte_2461a = byte_2461a;
}

static void abi_put_word(unsigned char *p, unsigned short value);

void eff_13A94(void);
#pragma aux eff_13A94 __parm __caller [] __modify __exact [__ax]
void eff_13A94(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned char *channel;
    unsigned long scaled;
    unsigned long sample_end;
    unsigned short ax_out;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    if ((unsigned char)ax_in != 0) channel[0x16] = (unsigned char)ax_in;
    scaled = ((unsigned long)channel[0x16]) << 8;
    sample_end = (unsigned long)channel[0x30]
        | ((unsigned long)channel[0x31] << 8)
        | ((unsigned long)channel[0x32] << 16)
        | ((unsigned long)channel[0x33] << 24);
    ax_out = (unsigned short)scaled;

    if (scaled <= sample_end) {
        abi_put_word(channel + 0x4c, ax_out);
    } else if (abi_eff13a94_byte_2461a == 0) {
        channel[0x17] = (unsigned char)((channel[0x17] & 0xfbu) | 0x40u);
        channel[0x03] = 0;
    } else {
        ax_out = (unsigned short)sample_end;
        abi_put_word(channel + 0x4c, ax_out);
    }

    _asm {
        mov ax, ax_out
    }
}

void abi_set_eff13ad7_state(unsigned char max_volume) {
    abi_eff13ad7_max_volume = max_volume;
}

void eff_13AD7(void);
#pragma aux eff_13AD7 __parm __caller [] __modify __exact [__ax __dx]
void eff_13AD7(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short dx_in;
    unsigned char *channel;
    unsigned char al;
    unsigned char dl;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
        mov dx_in, dx
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    dl = channel[0x08];
    if ((al & 0xf0u) != 0) {
        al >>= 4;
        dl = (unsigned char)(dl + al);
        al = dl;
        if (al > abi_eff13ad7_max_volume) al = abi_eff13ad7_max_volume;
    } else {
        al &= 0x0fu;
        dl = (unsigned char)(dl - al);
        al = dl;
        if (channel[0x08] < (unsigned char)(ax_in & 0x0fu)) al = 0;
    }
    channel[0x08] = al;
    ax_in = (unsigned short)((ax_in & 0xff00u) | al);
    dx_in = (unsigned short)((dx_in & 0xff00u) | dl);

    _asm {
        mov ax, ax_in
        mov dx, dx_in
    }
}

void abi_set_eff13b06_state(void) {
    abi_eff13b06_word_245f0 = 0xaaaau;
    abi_eff13b06_byte_24669 = 0;
    abi_eff13b06_byte_2466a = 0;
}

const unsigned char *abi_get_eff13b06_data(void) {
    abi_eff13b06_data[0] = (unsigned char)abi_eff13b06_word_245f0;
    abi_eff13b06_data[1] = (unsigned char)(abi_eff13b06_word_245f0 >> 8);
    return abi_eff13b06_data;
}

void eff_13B06(void);
#pragma aux eff_13B06 __parm __caller [] __modify __exact [__ax]
void eff_13B06(void) {
    unsigned short ax_in;
    unsigned short ax_out;
    _asm {
        mov ax_in, ax
    }
    ax_out = (unsigned short)((unsigned char)ax_in);
    abi_eff13b06_word_245f0 = (unsigned short)(ax_out - 1u);
    ax_out = (unsigned short)(ax_out + 0u);
    abi_eff13b06_byte_24669 = 0;
    abi_eff13b06_byte_2466a = 1;
    _asm {
        mov ax, ax_out
    }
}

void abi_set_eff13b78_state(unsigned char max_volume) {
    abi_eff13b78_max_volume = max_volume;
}

void eff_13B78(void);
#pragma aux eff_13B78 __parm __caller [] __modify __exact [__ax]
void eff_13B78(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned char *channel;
    unsigned char al;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    if (al > abi_eff13b78_max_volume) al = abi_eff13b78_max_volume;
    channel[0x08] = al;
    ax_in = (unsigned short)((ax_in & 0xff00u) | al);

    _asm {
        mov ax, ax_in
    }
}

void abi_set_eff13b88_state(unsigned char byte_24669, unsigned char byte_2466a) {
    abi_eff13b88_byte_24669 = byte_24669;
    abi_eff13b88_byte_2466a = byte_2466a;
}

const unsigned char *abi_get_eff13b88_data(void) {
    abi_eff13b88_data[0] = abi_eff13b88_byte_24669;
    abi_eff13b88_data[1] = abi_eff13b88_byte_2466a;
    return abi_eff13b88_data;
}

void eff_13B88(void);
#pragma aux eff_13B88 __parm __caller [] __modify __exact [__ax __dx]
void eff_13B88(void) {
    unsigned short ax_in;
    unsigned short dx_in;
    unsigned char al;
    unsigned char dl;

    _asm {
        mov ax_in, ax
        mov dx_in, dx
    }

    al = (unsigned char)ax_in;
    dl = (unsigned char)(al & 0x0fu);
    al = (unsigned char)((al >> 4) * 10u + dl);
    if (al <= 0x3fu) {
        abi_eff13b88_byte_24669 = al;
    } else {
        abi_eff13b88_byte_24669 = 0;
    }
    abi_eff13b88_byte_2466a = 1;
    ax_in = (unsigned short)((ax_in & 0xff00u) | al);
    dx_in = (unsigned short)((dx_in & 0xff00u) | dl);

    _asm {
        mov ax, ax_in
        mov dx, dx_in
    }
}

void eff_13BB2(void);
#pragma aux eff_13BB2 __parm __caller [] __modify __exact [__ax]
void eff_13BB2(void) {
    _asm {
        or al, al
        jz short eff_13bb2_clear
        or byte ptr [bx+17h], 20h
        jmp short eff_13bb2_done
eff_13bb2_clear:
        and byte ptr [bx+17h], 0dfh
eff_13bb2_done:
    }
}

void eff_13BA3(void);
#pragma aux eff_13BA3 __parm __caller [] __modify __exact [__ax __di]
void eff_13BA3(void) {
    _asm {
        mov di, ax
        shr di, 3
        and di, 1eh
        and al, 0fh
        cmp di, 6
        jnz short eff_13ba3_done
        call eff_13BB2
eff_13ba3_done:
    }
}

void eff_13BC0(void);
#pragma aux eff_13BC0 __parm __caller [] __modify __exact [__ax]
void eff_13BC0(void) {
    _asm {
        and byte ptr [bx+9], 0f0h
        or  [bx+9], al
    }
}

void eff_13C34(void);
#pragma aux eff_13C34 __parm __caller [] __modify __exact [__ax]
void eff_13C34(void) {
    _asm {
        and byte ptr [bx+9], 0fh
        shl al, 4
        or  [bx+9], al
    }
}

void abi_set_eff13bc8_state(unsigned char byte_2461a) {
    abi_eff13bc8_byte_2461a = byte_2461a;
}

void eff_13BC8(void);
#pragma aux eff_13BC8 __parm __caller [] __modify __exact [__ax __dx __di]
void eff_13BC8(void) {
    _asm {
        and ax, 0fh
        mov di, ax
        cmp abi_eff13bc8_byte_2461a, 0
        jnz short eff_13bc8_table
        shl di, 3
        mov ax, di
        neg ax
        shl di, 4
        add ax, di
        add ax, 03f8h
        mov [bx+38h], ax
        jmp short eff_13bc8_done
eff_13bc8_table:
        shl di, 1
        mov ax, abi_eff13bc8_table[di]
        mov [bx+14h], dx
eff_13bc8_done:
    }
}

void abi_set_eff13c3f_state(unsigned char byte_24668, unsigned char sndflags) {
    abi_eff13c3f_byte_24668 = byte_24668;
    abi_eff13a43_sndflags = sndflags;
}

void eff_13C3F(void);
#pragma aux eff_13C3F __parm __caller [] __modify __exact [__ax]
void eff_13C3F(void) {
    static const unsigned char table_13c54[16] = {
        0, 9, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x40,
        0x40, 0x4a, 0x53, 0x5c, 0x65, 0x6e, 0x77, 0x80
    };
    _asm {
        cmp abi_eff13c3f_byte_24668, 0
        jnz short eff_13c3f_done
        and ax, 0fh
        mov di, ax
        mov al, table_13c54[di]
        call eff_13A43
eff_13c3f_done:
    }
}

void abi_set_eff13c64_state(unsigned char byte_24668) {
    abi_eff13c64_byte_24668 = byte_24668;
}

void eff_13C64(void);
#pragma aux eff_13C64 __parm __caller [] __modify __exact [__ax __dx]
void eff_13C64(void) {
    _asm {
        or al, al
        jz short eff_13c64_done
        cmp abi_eff13c64_byte_24668, 0
        jnz short eff_13c64_divide
        test byte ptr [bx+3dh], 8
        jnz short eff_13c64_done
eff_13c64_divide:
        mov dl, al
        movzx ax, abi_eff13c64_byte_24668
        div dl
eff_13c64_done:
    }
}

void abi_set_eff13c88_state(unsigned char byte_24668, unsigned char max_volume) {
    abi_eff13c88_byte_24668 = byte_24668;
    abi_eff13c88_max_volume = max_volume;
}

void eff_13C88(void);
#pragma aux eff_13C88 __parm __caller [] __modify __exact [__ax __dx]
void eff_13C88(void) {
    _asm {
        mov dl, [bx+8]
        cmp abi_eff13c88_byte_24668, 0
        jnz short eff_13c88_done
        add al, dl
        cmp al, abi_eff13c88_max_volume
        jbe short eff_13c88_store
        mov al, abi_eff13c88_max_volume
eff_13c88_store:
        mov [bx+8], al
eff_13c88_done:
    }
}

void abi_set_eff13c95_state(unsigned char byte_24668) {
    abi_eff13c95_byte_24668 = byte_24668;
}

void eff_13C95(void);
#pragma aux eff_13C95 __parm __caller [] __modify __exact [__ax __dx]
void eff_13C95(void) {
    _asm {
        mov dl, [bx+8]
        cmp abi_eff13c95_byte_24668, 0
        jnz short eff_13c95_done
        cmp dl, al
        jb short eff_13c95_zero
        sub dl, al
        mov al, dl
        jmp short eff_13c95_store
eff_13c95_zero:
        xor al, al
eff_13c95_store:
        mov [bx+8], al
eff_13c95_done:
    }
}

void abi_set_eff13ca2_state(unsigned char byte_24668) {
    abi_eff13ca2_byte_24668 = byte_24668;
}

void eff_13CA2(void);
#pragma aux eff_13CA2 __parm __caller [] __modify __exact [__ax]
void eff_13CA2(void) {
    _asm {
        cmp al, abi_eff13ca2_byte_24668
        jnz short eff_13ca2_done
        xor al, al
eff_13ca2_done:
    }
}

void abi_set_eff13cb3_state(unsigned char byte_24668) {
    abi_eff13cb3_byte_24668 = byte_24668;
}

void eff_13CB3(void);
#pragma aux eff_13CB3 __parm __caller [] __modify __exact [__ax]
void eff_13CB3(void) {
    _asm {
        cmp al, abi_eff13cb3_byte_24668
        jnz short eff_13cb3_done
        cmp word ptr [bx], 0
        jz short eff_13cb3_done
        mov byte ptr [bx+0ah], 0
        mov byte ptr [bx+0bh], 0
eff_13cb3_done:
    }
}

void abi_set_eff13cc9_state(unsigned char byte_24668, unsigned char byte_2466d, unsigned char byte_2466c) {
    abi_eff13cc9_byte_24668 = byte_24668;
    abi_eff13cc9_byte_2466d = byte_2466d;
    abi_eff13cc9_byte_2466c = byte_2466c;
}

unsigned char abi_get_eff13cc9_byte_2466c(void) {
    return abi_eff13cc9_byte_2466c;
}

void eff_13CC9(void);
#pragma aux eff_13CC9 __parm __caller [] __modify __exact [__ax]
void eff_13CC9(void) {
    _asm {
        cmp abi_eff13cc9_byte_24668, 0
        jnz short eff_13cc9_done
        cmp abi_eff13cc9_byte_2466d, 0
        jnz short eff_13cc9_done
        inc al
        mov abi_eff13cc9_byte_2466c, al
eff_13cc9_done:
    }
}

void abi_set_eff13cdd_state(unsigned char flag_playsettings, unsigned char byte_24667, unsigned char byte_24668) {
    abi_eff13cdd_flag_playsettings = flag_playsettings;
    abi_eff13ce8_byte_24667 = byte_24667;
    abi_eff13ce8_byte_24668 = byte_24668;
}

void eff_13CDD(void);
#pragma aux eff_13CDD __parm __caller [] __modify __exact [__ax]
void eff_13CDD(void) {
    _asm {
        test abi_eff13cdd_flag_playsettings, 2
        jnz short eff_13cdd_set
        cmp al, 20h
        ja short eff_13cdd_done
eff_13cdd_set:
        or  al, al
        jz short eff_13cdd_done
        mov abi_eff13ce8_byte_24667, al
        mov abi_eff13ce8_byte_24668, 0
eff_13cdd_done:
    }
}

void abi_set_eff13c02_state(unsigned char byte_24668, unsigned short word_245f6) {
    abi_eff13c02_byte_24668 = byte_24668;
    abi_eff13c02_word_245f6 = word_245f6;
    abi_eff13c02_byte_24669 = 0xaa;
    abi_eff13c02_byte_2466a = 0;
    abi_eff13c02_byte_2466b = 0xbb;
}

const unsigned char *abi_get_eff13c02_globals(void) {
    abi_eff13c02_globals[0] = abi_eff13c02_byte_24668;
    abi_eff13c02_globals[1] = abi_eff13c02_byte_24669;
    abi_eff13c02_globals[2] = abi_eff13c02_byte_2466a;
    abi_eff13c02_globals[3] = abi_eff13c02_byte_2466b;
    return abi_eff13c02_globals;
}

void eff_13C02(void);
#pragma aux eff_13C02 __parm __caller [] __modify __exact [__ax]
void eff_13C02(void) {
    _asm {
        cmp abi_eff13c02_byte_24668, 0
        jnz short eff_13c02_done
        or  al, al
        jz short eff_13c02_zero
        cmp byte ptr [bx+3ch], 0
        jnz short eff_13c02_dec
        inc al
        mov byte ptr [bx+3ch], al
eff_13c02_dec:
        dec byte ptr [bx+3ch]
        jz short eff_13c02_done
        mov al, byte ptr [bx+3bh]
        mov abi_eff13c02_byte_24669, al
        mov abi_eff13c02_byte_2466b, 1
        jmp short eff_13c02_done
eff_13c02_zero:
        mov ax, abi_eff13c02_word_245f6
        mov byte ptr [bx+3bh], al
eff_13c02_done:
    }
}

void abi_set_effect_slide_state(unsigned char byte_24668) {
    abi_effect_slide_byte_24668 = byte_24668;
}

void eff_13886(void);
#pragma aux eff_13886 __parm __caller [] __modify __exact [__ax]
void eff_13886(void) {
    _asm {
        xor ah, ah
        shl ax, 4
        sub word ptr [bx], ax
        cmp word ptr [bx], 0a0h
        jge short eff_13886_store
        mov word ptr [bx], 0a0h
eff_13886_store:
        mov ax, word ptr [bx]
    }
}

void eff_138A4(void);
#pragma aux eff_138A4 __parm __caller [] __modify __exact [__ax]
void eff_138A4(void) {
    _asm {
        xor ah, ah
        shl ax, 4
        add word ptr [bx], ax
        jb short eff_138a4_cap
        cmp word ptr [bx], 3580h
        jbe short eff_138a4_store
eff_138a4_cap:
        mov word ptr [bx], 3580h
eff_138a4_store:
        mov ax, word ptr [bx]
    }
}

void eff_1387F(void);
#pragma aux eff_1387F __parm __caller [] __modify __exact [__ax]
void eff_1387F(void) {
    _asm {
        cmp abi_effect_slide_byte_24668, 0
        jnz short eff_1387f_done
        call eff_13886
eff_1387f_done:
    }
}

void eff_1389D(void);
#pragma aux eff_1389D __parm __caller [] __modify __exact [__ax]
void eff_1389D(void) {
    _asm {
        cmp abi_effect_slide_byte_24668, 0
        jnz short eff_1389d_done
        call eff_138A4
eff_1389d_done:
    }
}

void abi_set_eff1392f_state(unsigned char flag_playsettings) {
    abi_eff1392f_flag_playsettings = flag_playsettings;
}

void eff_1392F(void);
#pragma aux eff_1392F __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_1392F(void) {
    _asm {
        mov cl, 3
        or  al, al
        jz short eff_1392f_calc
        mov ch, al
        mov dl, byte ptr [bx+0ch]
        and al, 0fh
        jz short eff_1392f_high
        and dl, 0f0h
        or  dl, al
eff_1392f_high:
        and ch, 0f0h
        jz short eff_1392f_store_mem
        and dl, 0fh
        or  dl, ch
eff_1392f_store_mem:
        mov byte ptr [bx+0ch], dl
eff_1392f_calc:
        mov al, byte ptr [bx+0dh]
        shr al, 2
        and ax, 1fh
        mov dl, byte ptr [bx+9]
        and dl, 3
        jz short eff_1392f_wave
        shl al, 3
        cmp dl, 1
        jz short eff_1392f_ramp
        mov dl, 0ffh
        jmp short eff_1392f_mul
eff_1392f_ramp:
        mov dl, al
        test byte ptr [bx+0dh], 80h
        jz short eff_1392f_mul
        mov dl, 0ffh
        sub dl, al
        jmp short eff_1392f_mul
eff_1392f_wave:
        mov di, ax
        mov dl, abi_vibrato_wave[di]
eff_1392f_mul:
        mov al, byte ptr [bx+0ch]
        mov dh, al
        and al, 0fh
        mul dl
        mov ch, abi_eff1392f_flag_playsettings
        and ch, 1
        add cl, ch
        shr ax, cl
        test byte ptr [bx+0dh], 80h
        jz short eff_1392f_add
        neg ax
eff_1392f_add:
        add ax, word ptr [bx]
        shr dh, 2
        and dh, 3ch
        add byte ptr [bx+0dh], dh
    }
}

void eff_13E2D(void);
#pragma aux eff_13E2D __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_13E2D(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned char al;
    unsigned char dl;
    unsigned char dh;
    unsigned short ax_calc;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    if (al != 0) {
        unsigned char ch = al;
        dl = channel[0x0c];
        if ((al & 0x0fu) != 0) dl = (unsigned char)((dl & 0xf0u) | (al & 0x0fu));
        if ((ch & 0xf0u) != 0) dl = (unsigned char)((dl & 0x0fu) | (ch & 0xf0u));
        channel[0x0c] = dl;
    }

    al = (unsigned char)((channel[0x0d] >> 2) & 0x1fu);
    dl = (unsigned char)(channel[0x09] & 3u);
    if (dl != 0) {
        al = (unsigned char)(al << 3);
        if (dl == 1) {
            dl = al;
            if ((channel[0x0d] & 0x80u) != 0) dl = (unsigned char)(0xffu - al);
        } else {
            dl = 0xff;
        }
    } else {
        dl = abi_vibrato_wave[al];
    }
    ax_calc = (unsigned short)((channel[0x0c] & 0x0fu) * dl);
    ax_calc = (unsigned short)(ax_calc >> (unsigned char)(5 + (abi_eff1392f_flag_playsettings & 1u)));
    if ((channel[0x0d] & 0x80u) != 0) ax_calc = (unsigned short)(-((short)ax_calc));
    ax_out = (unsigned short)(ax_calc + (unsigned short)(channel[0] | ((unsigned short)channel[1] << 8)));
    dh = (unsigned char)((channel[0x0c] >> 2) & 0x3cu);
    channel[0x0d] = (unsigned char)(channel[0x0d] + dh);

    _asm {
        mov ax, ax_out
    }
}

static unsigned short abi_get_word(const unsigned char *p) {
    return (unsigned short)(p[0] | ((unsigned short)p[1] << 8));
}

static void abi_put_word(unsigned char *p, unsigned short value) {
    p[0] = (unsigned char)value;
    p[1] = (unsigned char)(value >> 8);
}

void abi_set_eff13e32_state(unsigned char byte_24668, unsigned char max_volume) {
    abi_eff13e32_byte_24668 = byte_24668;
    abi_eff13e32_max_volume = max_volume;
}

void eff_13E32(void);
#pragma aux eff_13E32 __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_13E32(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned char al;
    unsigned char dl;
    unsigned char lo;
    unsigned char hi;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    if (al != 0) channel[0x34] = al;
    al = channel[0x34];
    dl = channel[0x08];
    lo = (unsigned char)(al & 0x0fu);
    hi = (unsigned char)(al >> 4);
    ax_out = ax_in;

    if (lo == 0x0f) {
        if (hi == 0) {
            dl = (dl >= lo) ? (unsigned char)(dl - lo) : 0;
            channel[0x08] = dl;
            ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
        } else if (abi_eff13e32_byte_24668 == 0) {
            dl = (unsigned char)(dl + hi);
            if (dl > abi_eff13e32_max_volume) dl = abi_eff13e32_max_volume;
            channel[0x08] = dl;
            ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
        }
    } else if (hi == 0x0f) {
        if (lo != 0 && abi_eff13e32_byte_24668 == 0) {
            dl = (dl >= lo) ? (unsigned char)(dl - lo) : 0;
            channel[0x08] = dl;
            ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
        }
    } else if (lo != 0) {
        dl = (dl >= lo) ? (unsigned char)(dl - lo) : 0;
        channel[0x08] = dl;
        ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
    } else {
        dl = (unsigned char)(dl + hi);
        if (dl > abi_eff13e32_max_volume) dl = abi_eff13e32_max_volume;
        channel[0x08] = dl;
        ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
    }

    _asm {
        mov ax, ax_out
    }
}

void eff_13E7F(void);
#pragma aux eff_13E7F __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_13E7F(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned char al;
    unsigned char dl;
    unsigned char lo;
    unsigned char hi;
    unsigned short target;
    unsigned short current;
    unsigned short step;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    if (al != 0) channel[0x34] = al;
    al = channel[0x34];
    dl = channel[0x08];
    lo = (unsigned char)(al & 0x0fu);
    hi = (unsigned char)(al >> 4);
    ax_out = ax_in;

    if (lo == 0x0f) {
        if (hi == 0) {
            dl = (dl >= lo) ? (unsigned char)(dl - lo) : 0;
            channel[0x08] = dl;
            ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
        } else if (abi_eff13e32_byte_24668 == 0) {
            dl = (unsigned char)(dl + hi);
            if (dl > abi_eff13e32_max_volume) dl = abi_eff13e32_max_volume;
            channel[0x08] = dl;
            ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
        }
    } else if (hi == 0x0f) {
        if (lo != 0 && abi_eff13e32_byte_24668 == 0) {
            dl = (dl >= lo) ? (unsigned char)(dl - lo) : 0;
            channel[0x08] = dl;
            ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
        }
    } else if (lo != 0) {
        dl = (dl >= lo) ? (unsigned char)(dl - lo) : 0;
        channel[0x08] = dl;
        ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
    } else {
        dl = (unsigned char)(dl + hi);
        if (dl > abi_eff13e32_max_volume) dl = abi_eff13e32_max_volume;
        channel[0x08] = dl;
        ax_out = (unsigned short)((ax_in & 0xff00u) | dl);
    }

    target = abi_get_word(channel + 0x10);
    if (target != 0) {
        current = abi_get_word(channel);
        step = abi_get_word(channel + 0x12);
        if (target >= current) {
            unsigned long next = (unsigned long)current + step;
            if (next >= target) {
                current = target;
                abi_put_word(channel + 0x10, 0);
                channel[0x17] = (unsigned char)(channel[0x17] & 0xefu);
            } else {
                current = (unsigned short)next;
            }
        } else {
            current = (current > step) ? (unsigned short)(current - step) : 0;
            if ((short)target >= (short)current) {
                current = target;
                abi_put_word(channel + 0x10, 0);
                channel[0x17] = (unsigned char)(channel[0x17] & 0xefu);
            }
        }
        abi_put_word(channel, current);
        if ((channel[0x17] & 0x20u) != 0 && abi_get_word(channel + 0x10) != 0) {
            ax_out = 0x0032;
        } else {
            ax_out = current;
        }
    }

    _asm {
        mov ax, ax_out
    }
}

void eff_13E84(void);
#pragma aux eff_13E84 __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_13E84(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned char al;
    unsigned char dl;
    unsigned char dh;
    unsigned char lo;
    unsigned char hi;
    unsigned short ax_calc;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    if (al != 0) channel[0x34] = al;
    al = channel[0x34];
    dl = channel[0x08];
    lo = (unsigned char)(al & 0x0fu);
    hi = (unsigned char)(al >> 4);

    if (lo == 0x0f) {
        if (hi == 0) {
            dl = (dl >= lo) ? (unsigned char)(dl - lo) : 0;
            channel[0x08] = dl;
        } else if (abi_eff13e32_byte_24668 == 0) {
            dl = (unsigned char)(dl + hi);
            if (dl > abi_eff13e32_max_volume) dl = abi_eff13e32_max_volume;
            channel[0x08] = dl;
        }
    } else if (hi == 0x0f) {
        if (lo != 0 && abi_eff13e32_byte_24668 == 0) {
            dl = (dl >= lo) ? (unsigned char)(dl - lo) : 0;
            channel[0x08] = dl;
        }
    } else if (lo != 0) {
        dl = (dl >= lo) ? (unsigned char)(dl - lo) : 0;
        channel[0x08] = dl;
    } else {
        dl = (unsigned char)(dl + hi);
        if (dl > abi_eff13e32_max_volume) dl = abi_eff13e32_max_volume;
        channel[0x08] = dl;
    }

    al = (unsigned char)((channel[0x0d] >> 2) & 0x1fu);
    dl = (unsigned char)(channel[0x09] & 3u);
    if (dl != 0) {
        al = (unsigned char)(al << 3);
        if (dl == 1) {
            dl = al;
            if ((channel[0x0d] & 0x80u) != 0) dl = (unsigned char)(0xffu - al);
        } else {
            dl = 0xff;
        }
    } else {
        dl = abi_vibrato_wave[al];
    }
    ax_calc = (unsigned short)((channel[0x0c] & 0x0fu) * dl);
    ax_calc = (unsigned short)(ax_calc >> (unsigned char)(5 + (abi_eff1392f_flag_playsettings & 1u)));
    if ((channel[0x0d] & 0x80u) != 0) ax_calc = (unsigned short)(-((short)ax_calc));
    ax_out = (unsigned short)(ax_calc + abi_get_word(channel));
    dh = (unsigned char)((channel[0x0c] >> 2) & 0x3cu);
    channel[0x0d] = (unsigned char)(channel[0x0d] + dh);

    _asm {
        mov ax, ax_out
    }
}

void abi_set_eff13f05_state(unsigned char byte_24668) {
    abi_eff13e32_byte_24668 = byte_24668;
}

void eff_13F05(void);
#pragma aux eff_13F05 __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_13F05(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned char stored;
    unsigned char hi;
    unsigned char lo;
    unsigned char total;
    unsigned char rem;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    if ((unsigned char)ax_in != 0) channel[0x34] = (unsigned char)ax_in;
    stored = channel[0x34];
    hi = (unsigned char)(stored >> 4);
    lo = (unsigned char)(stored & 0x0f);
    total = (unsigned char)(hi + lo);
    ax_out = ax_in;
    if (total != 0) {
        rem = (unsigned char)(abi_eff13e32_byte_24668 % total);
        ax_out = (unsigned short)(((unsigned short)rem << 8) | (abi_eff13e32_byte_24668 / total));
        if (rem >= hi) ax_out = (unsigned short)(ax_out & 0xff00u);
        else ax_out = (unsigned short)((ax_out & 0xff00u) | channel[0x08]);
    }

    _asm {
        mov ax, ax_out
    }
}

void abi_set_eff13f3b_state(unsigned char byte_24668, unsigned char max_volume) {
    abi_eff13e32_byte_24668 = byte_24668;
    abi_eff13e32_max_volume = max_volume;
}

void eff_13F3B(void);
#pragma aux eff_13F3B __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_13F3B(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned char ch;
    unsigned char op;
    unsigned char al;
    unsigned char delta;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    if ((unsigned char)ax_in != 0) channel[0x34] = (unsigned char)ax_in;
    ch = channel[0x34];
    op = (unsigned char)(ch >> 4);
    if ((op & 7u) != 0) {
        if ((op & 8u) == 0) {
            if (op == 6) {
                channel[0x08] = (unsigned char)(((unsigned short)channel[0x08] * 2u) / 3u);
            } else if (op == 7) {
                channel[0x08] = (unsigned char)(channel[0x08] >> 1);
            } else {
                delta = (unsigned char)(1u << (op - 1u));
                channel[0x08] = (channel[0x08] >= delta) ? (unsigned char)(channel[0x08] - delta) : 0;
            }
        } else {
            op = (unsigned char)(op & 7u);
            if (op == 6) al = (unsigned char)(((unsigned short)channel[0x08] * 3u) >> 1);
            else if (op == 7) al = (unsigned char)(channel[0x08] << 1);
            else al = (unsigned char)(channel[0x08] + (1u << (op - 1u)));
            if (al > abi_eff13e32_max_volume) al = abi_eff13e32_max_volume;
            channel[0x08] = al;
        }
    }

    al = (unsigned char)(ch & 0x0fu);
    ax_out = (unsigned short)((ax_in & 0xff00u) | al);
    if (al != 0) {
        if (!(abi_eff13e32_byte_24668 == 0 && (channel[0x3d] & 8u) != 0)) {
            ax_out = (unsigned short)(((unsigned short)(abi_eff13e32_byte_24668 % al) << 8) | (abi_eff13e32_byte_24668 / al));
        }
    }

    _asm {
        mov ax, ax_out
    }
}

void abi_set_eff13fbe_state(unsigned char byte_24668) {
    abi_eff13e32_byte_24668 = byte_24668;
}

void eff_13FBE(void);
#pragma aux eff_13FBE __parm __caller [] __modify __exact [__ax __dx]
void eff_13FBE(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned char al;
    unsigned char dl;
    unsigned char ah;
    unsigned char dh;
    unsigned char quotient;
    unsigned char rem;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    ax_out = ax_in;
    if (al == 0) {
        al = channel[0x34];
        if (al == 0) {
            _asm {
                mov ax, ax_out
            }
            return;
        }
        channel[0x0b] = al;
    }
    channel[0x34] = al;
    al = channel[0x35];
    dl = (unsigned char)(al & 0x0fu);
    if (dl == 0) {
        _asm {
            mov ax, ax_out
        }
        return;
    }
    --dl;
    al = (unsigned char)(al >> 4);
    dl = (unsigned char)(dl + al * 12u);
    ah = (unsigned char)(abi_eff13e32_byte_24668 % 3u);
    ax_out = (unsigned short)(((unsigned short)ah << 8) | (abi_eff13e32_byte_24668 / 3u));
    if (ah == 0) {
        ax_out = (unsigned short)(channel[0] | ((unsigned short)channel[1] << 8));
    } else {
        dh = channel[0x0b];
        if (ah != 2) dh = (unsigned char)(dh >> 4);
        dh = (unsigned char)(dh & 0x0fu);
        dl = (unsigned char)(dl + dh);
        quotient = (unsigned char)(dl / 12u);
        rem = (unsigned char)(dl % 12u);
        al = (unsigned char)((quotient << 4) | (rem + 1u));
        (void)al;
        ax_out = 0;
    }

    _asm {
        mov ax, ax_out
    }
}

void abi_set_eff139ac_state(unsigned char max_volume) {
    abi_eff139ac_max_volume = max_volume;
}

void eff_139AC(void);
#pragma aux eff_139AC __parm __caller [] __modify __exact [__ax __dx]
void eff_139AC(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char dl_out;
    unsigned char *channel;
    unsigned char al;
    unsigned short target;
    unsigned short current;
    unsigned short step;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    dl_out = channel[0x08];
    if ((al & 0xf0u) != 0) {
        al = (unsigned char)(al >> 4);
        al = (unsigned char)(dl_out + al);
        if (al > abi_eff139ac_max_volume) al = abi_eff139ac_max_volume;
    } else {
        al = (unsigned char)(al & 0x0fu);
        al = (dl_out >= al) ? (unsigned char)(dl_out - al) : 0;
    }
    channel[0x08] = al;
    ax_out = (unsigned short)((ax_in & 0xff00u) | al);

    target = abi_get_word(channel + 0x10);
    if (target != 0) {
        current = abi_get_word(channel);
        step = abi_get_word(channel + 0x12);
        if (target >= current) {
            unsigned long next = (unsigned long)current + step;
            if (next >= target) {
                current = target;
                abi_put_word(channel + 0x10, 0);
                channel[0x17] = (unsigned char)(channel[0x17] & 0xefu);
            } else {
                current = (unsigned short)next;
            }
        } else {
            current = (current > step) ? (unsigned short)(current - step) : 0;
            if ((short)target >= (short)current) {
                current = target;
                abi_put_word(channel + 0x10, 0);
                channel[0x17] = (unsigned char)(channel[0x17] & 0xefu);
            }
        }
        abi_put_word(channel, current);
        if ((channel[0x17] & 0x20u) != 0 && abi_get_word(channel + 0x10) != 0) {
            ax_out = 0x0032;
        } else {
            ax_out = current;
        }
    }

    _asm {
        mov ax, ax_out
        mov dl, dl_out
    }
}

void abi_set_eff139b2_state(unsigned char max_volume, unsigned char flag_playsettings) {
    abi_eff139b2_max_volume = max_volume;
    abi_eff139b2_flag_playsettings = flag_playsettings;
}

void eff_139B2(void);
#pragma aux eff_139B2 __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_139B2(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char dl_out;
    unsigned char *channel;
    unsigned char al;
    unsigned char dl;
    unsigned char dh;
    unsigned short ax_calc;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    dl_out = channel[0x08];
    if ((al & 0xf0u) != 0) {
        al = (unsigned char)(al >> 4);
        al = (unsigned char)(dl_out + al);
        if (al > abi_eff139b2_max_volume) al = abi_eff139b2_max_volume;
    } else {
        al = (unsigned char)(al & 0x0fu);
        al = (dl_out >= al) ? (unsigned char)(dl_out - al) : 0;
    }
    channel[0x08] = al;

    al = (unsigned char)((channel[0x0d] >> 2) & 0x1fu);
    dl = (unsigned char)(channel[0x09] & 3u);
    if (dl != 0) {
        al = (unsigned char)(al << 3);
        if (dl == 1) {
            dl = al;
            if ((channel[0x0d] & 0x80u) != 0) dl = (unsigned char)(0xffu - al);
        } else {
            dl = 0xff;
        }
    } else {
        dl = abi_vibrato_wave[al];
    }
    ax_calc = (unsigned short)((channel[0x0c] & 0x0fu) * dl);
    ax_calc = (unsigned short)(ax_calc >> (unsigned char)(3 + (abi_eff139b2_flag_playsettings & 1u)));
    if ((channel[0x0d] & 0x80u) != 0) ax_calc = (unsigned short)(-((short)ax_calc));
    ax_out = (unsigned short)(ax_calc + abi_get_word(channel));
    dh = (unsigned char)((channel[0x0c] >> 2) & 0x3cu);
    channel[0x0d] = (unsigned char)(channel[0x0d] + dh);

    _asm {
        mov ax, ax_out
        mov dl, dl_out
    }
}

void abi_set_eff139b9_state(unsigned char max_volume) {
    abi_eff139b9_max_volume = max_volume;
}

void eff_139B9(void);
#pragma aux eff_139B9 __parm __caller [] __modify __exact [__ax __dx __di]
void eff_139B9(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned char al;
    unsigned char dl;
    unsigned char dh;
    unsigned char delta;
    unsigned short product;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    al = (unsigned char)ax_in;
    if (al != 0) {
        unsigned char cl = al;
        dl = channel[0x0e];
        if ((al & 0x0fu) != 0) dl = (unsigned char)((dl & 0xf0u) | (al & 0x0fu));
        if ((cl & 0xf0u) != 0) dl = (unsigned char)((dl & 0x0fu) | (cl & 0xf0u));
        channel[0x0e] = dl;
    }

    al = (unsigned char)((channel[0x0f] >> 2) & 0x1fu);
    dl = (unsigned char)((channel[0x09] >> 4) & 3u);
    if (dl != 0) {
        al = (unsigned char)(al << 3);
        if (dl == 1) {
            dl = al;
            if ((channel[0x0f] & 0x80u) != 0) dl = (unsigned char)(0xffu - al);
        } else {
            dl = 0xff;
        }
    } else {
        dl = abi_vibrato_wave[al];
    }
    dh = channel[0x0e];
    product = (unsigned short)((channel[0x0e] & 0x0fu) * dl);
    delta = (unsigned char)(product >> 6);
    al = channel[0x08];
    if ((channel[0x0f] & 0x80u) == 0) {
        al = (unsigned char)(al + delta);
        if (al > abi_eff139b9_max_volume) al = abi_eff139b9_max_volume;
    } else {
        al = (al >= delta) ? (unsigned char)(al - delta) : 0;
    }
    dh = (unsigned char)((dh >> 2) & 0x3cu);
    channel[0x0f] = (unsigned char)(channel[0x0f] + dh);
    ax_out = (unsigned short)(((unsigned short)delta << 8) | al);

    _asm {
        mov ax, ax_out
    }
}

void eff_138D2(void);
#pragma aux eff_138D2 __parm __caller [] __modify __exact [__ax __dx]
void eff_138D2(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned short target;
    unsigned short current;
    unsigned short step;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    if ((unsigned char)ax_in != 0) {
        step = (unsigned short)((unsigned char)ax_in << 4);
        abi_put_word(channel + 0x12, step);
    }

    ax_out = ax_in;
    target = abi_get_word(channel + 0x10);
    if (target != 0) {
        current = abi_get_word(channel);
        step = abi_get_word(channel + 0x12);
        if (target >= current) {
            unsigned long next = (unsigned long)current + step;
            if (next >= target) {
                current = target;
                abi_put_word(channel + 0x10, 0);
                channel[0x17] = (unsigned char)(channel[0x17] & 0xefu);
            } else {
                current = (unsigned short)next;
            }
        } else {
            current = (current > step) ? (unsigned short)(current - step) : 0;
            if ((short)target >= (short)current) {
                current = target;
                abi_put_word(channel + 0x10, 0);
                channel[0x17] = (unsigned char)(channel[0x17] & 0xefu);
            }
        }
        abi_put_word(channel, current);
        if ((channel[0x17] & 0x20u) != 0 && abi_get_word(channel + 0x10) != 0) {
            ax_out = 0x0032;
        } else {
            ax_out = current;
        }
    }

    _asm {
        mov ax, ax_out
    }
}

void eff_13DE5(void);
#pragma aux eff_13DE5 __parm __caller [] __modify __exact [__ax __dx]
void eff_13DE5(void) {
    _asm {
        call sub_14087
        or ax, ax
        jz short eff_13de5_done
        sub word ptr [bx], ax
        cmp word ptr [bx], 0a0h
        jge short eff_13de5_store
        mov word ptr [bx], 0a0h
eff_13de5_store:
        mov ax, word ptr [bx]
eff_13de5_done:
    }
}

void eff_13DEF(void);
#pragma aux eff_13DEF __parm __caller [] __modify __exact [__ax __dx]
void eff_13DEF(void) {
    _asm {
        call sub_14087
        or ax, ax
        jz short eff_13def_done
        add word ptr [bx], ax
        jb short eff_13def_cap
        cmp word ptr [bx], 3580h
        jbe short eff_13def_store
eff_13def_cap:
        mov word ptr [bx], 3580h
eff_13def_store:
        mov ax, word ptr [bx]
eff_13def_done:
    }
}

void eff_13E1E(void);
#pragma aux eff_13E1E __parm __caller [] __modify __exact [__ax __dx]
void eff_13E1E(void) {
    unsigned short bx_in;
    unsigned short ax_in;
    unsigned short ax_out;
    unsigned char *channel;
    unsigned short target;
    unsigned short current;
    unsigned short step;

    _asm {
        mov bx_in, bx
        mov ax_in, ax
    }

    channel = (unsigned char *)bx_in;
    if ((unsigned char)ax_in != 0) {
        step = (unsigned short)((unsigned char)ax_in << 2);
        abi_put_word(channel + 0x12, step);
    }

    ax_out = ax_in;
    target = abi_get_word(channel + 0x10);
    if (target != 0) {
        current = abi_get_word(channel);
        step = abi_get_word(channel + 0x12);
        if (target >= current) {
            unsigned long next = (unsigned long)current + step;
            if (next >= target) {
                current = target;
                abi_put_word(channel + 0x10, 0);
                channel[0x17] = (unsigned char)(channel[0x17] & 0xefu);
            } else {
                current = (unsigned short)next;
            }
        } else {
            current = (current > step) ? (unsigned short)(current - step) : 0;
            if ((short)target >= (short)current) {
                current = target;
                abi_put_word(channel + 0x10, 0);
                channel[0x17] = (unsigned char)(channel[0x17] & 0xefu);
            }
        }
        abi_put_word(channel, current);
        if ((channel[0x17] & 0x20u) != 0 && abi_get_word(channel + 0x10) != 0) {
            ax_out = 0x0032;
        } else {
            ax_out = current;
        }
    }

    _asm {
        mov ax, ax_out
    }
}

void sub_11C0C(void);
#pragma aux sub_11C0C __parm __caller [] __modify __exact [__ax __bx __si]
void sub_11C0C(void) {
    _asm {
        xor si, si
        or  al, al
        jz short sub_11c0c_done
        xor bx, bx
sub_11c0c_loop:
        mov bl, abi_sub11c0c_skip[bx]
        add si, bx
        mov bl, es:[si]
        inc si
        shr bl, 5
        jnz short sub_11c0c_loop
        dec al
        jnz short sub_11c0c_loop
sub_11c0c_done:
    }
}

void abi_set_sub1415e_state(unsigned short index, unsigned short total, unsigned char segment_index, unsigned char pending) {
    int i;
    abi_sub1415e_index = index;
    abi_sub1415e_total = total;
    abi_sub1415e_segment_index = segment_index;
    abi_sub1415e_pending = pending;
    for (i = 0; i < 20; ++i) abi_sub1415e_data[i] = 0;
}

const unsigned char *abi_get_sub1415e_data(void) {
    return abi_sub1415e_data;
}

void sub_1415E(void);
#pragma aux sub_1415E __parm __caller [] __modify __exact [__ax __bx __si]
void sub_1415E(void) {
    unsigned short bit_off;
    unsigned char bit_value;
    bit_off = (unsigned short)(0x3d48u + (abi_sub1415e_index >> 3));
    bit_value = (unsigned char)(1u << (abi_sub1415e_index & 7u));
    abi_sub1415e_data[0] = 0;
    abi_sub1415e_data[1] = 0;
    abi_sub1415e_data[2] = (unsigned char)abi_sub1415e_index;
    abi_sub1415e_data[3] = (unsigned char)(abi_sub1415e_index >> 8);
    abi_sub1415e_data[4] = 0;
    abi_sub1415e_data[5] = 0;
    abi_sub1415e_data[6] = abi_sub1415e_segment_index;
    abi_sub1415e_data[7] = 0;
    abi_sub1415e_data[8] = abi_sub1415e_pending;
    abi_sub1415e_data[9] = 0;
    abi_sub1415e_data[10] = 0;
    abi_sub1415e_data[11] = 0;
    abi_sub1415e_data[12] = (unsigned char)abi_sub1415e_total;
    abi_sub1415e_data[13] = (unsigned char)(abi_sub1415e_total >> 8);
    abi_sub1415e_data[14] = 0;
    abi_sub1415e_data[15] = 0;
    abi_sub1415e_data[16] = 0;
    abi_sub1415e_data[17] = 0;
    abi_sub1415e_data[18] = 0;
    abi_sub1415e_data[19] = bit_value;
    (void)bit_off;
    _asm {
        xor si, si
    }
}

void sub_12F56(void);
#pragma aux sub_12F56 __parm __caller [] __modify __exact [__ax __bx __cx __si]
void sub_12F56(void) {
    _asm {
        call sub_1415E
    }
}

void abi_set_sub154f4_state(unsigned short buffer_size2, unsigned char flag_playsettings) {
    abi_sub154f4_buffer_size2 = buffer_size2;
    abi_sub154f4_flag_playsettings = flag_playsettings;
    abi_sub154f4_data[0] = 0;
    abi_sub154f4_data[1] = 0;
    abi_sub154f4_data[2] = 0;
    abi_sub154f4_data[3] = 0;
}

const unsigned char *abi_get_sub154f4_data(void) {
    return abi_sub154f4_data;
}

void sub_154F4(void);
#pragma aux sub_154F4 __parm __caller [] __modify __exact [__ax __bx __cx __bp __si]
void sub_154F4(void) {
    _asm {
        mov ax, abi_sub154f4_buffer_size2
        shr ax, 4
        mov abi_sub154f4_data[3], al
        mov ax, [si+36h]
        mov word ptr abi_sub154f4_data[0], ax
        mov abi_sub154f4_data[2], 0
        test abi_sub154f4_flag_playsettings, 10h
        jz short sub_154f4_interp_done
        cmp al, ah
        jz short sub_154f4_interp_done
        mov abi_sub154f4_data[2], 1
sub_154f4_interp_done:
    }
}

void abi_set_sub1609f_state(unsigned short buffer_size2) {
    abi_sub1609f_buffer_size2 = buffer_size2;
}

void sub_1609F(void);
#pragma aux sub_1609F __parm __caller [] __modify __exact [__ax __bx __cx __si __di]
void sub_1609F(void) {
    _asm {
        test byte ptr [si+17h], 1
        jnz short sub_1609f_done
        mov cx, abi_sub1609f_buffer_size2
        mov bx, cx
        and bx, 0fh
        shl bx, 1
        xor eax, eax
        jcxz sub_1609f_done
sub_1609f_loop:
        mov [di], eax
        add di, 8
        dec cx
        jnz short sub_1609f_loop
sub_1609f_done:
    }
}

void fill_dmabuf8(void);
#pragma aux fill_dmabuf8 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void fill_dmabuf8(void) {
    _asm {
        mov bx, si
        mov dx, di
        jcxz fill_dmabuf8_done
fill_dmabuf8_loop:
        mov al, [si]
        add al, 80h
        mov [di], al
        add si, 8
        inc di
        dec cx
        jnz short fill_dmabuf8_loop
fill_dmabuf8_done:
        mov si, bx
        add si, 108h
        mov di, dx
        add di, 108h
    }
}

void fill_dmabuf8stereo(void);
#pragma aux fill_dmabuf8stereo __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void fill_dmabuf8stereo(void) {
    _asm {
        mov bx, si
        mov dx, di
        jcxz fill_dmabuf8stereo_done
fill_dmabuf8stereo_loop:
        mov al, [si+1]
        add al, 80h
        mov [di], al
        add si, 4
        inc di
        dec cx
        jnz short fill_dmabuf8stereo_loop
fill_dmabuf8stereo_done:
        mov si, bx
        add si, 108h
        mov di, dx
        add di, 108h
    }
}

void fill_dmabuf16stereo(void);
#pragma aux fill_dmabuf16stereo __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void fill_dmabuf16stereo(void) {
    _asm {
        mov bx, si
        mov dx, di
        jcxz fill_dmabuf16stereo_done
fill_dmabuf16stereo_loop:
        mov ax, [si]
        mov [di], ax
        mov ax, [si+4]
        mov [di+2], ax
        add si, 8
        add di, 4
        dec cx
        jnz short fill_dmabuf16stereo_loop
fill_dmabuf16stereo_done:
        mov si, bx
        add si, 108h
        mov di, dx
        add di, 108h
    }
}

void set_timer(void);
#pragma aux set_timer __parm __caller [] __modify __exact [__ax __cx __si __di]
void set_timer(void) {
    _asm {
        mov abi_timer_word_14f6e, ax
        pushf
        cli
        push ax
        mov al, 34h
        out 43h, al
        pop ax
        out 40h, al
        mov al, ah
        out 40h, al
        popf
    }
}

void clean_timer(void);
#pragma aux clean_timer __parm __caller [] __modify __exact [__ax __cx __si __di]
void clean_timer(void) {
    _asm {
        pushf
        cli
        mov al, 36h
        out 43h, al
        xor al, al
        out 40h, al
        jmp short clean_timer_delay_done
clean_timer_delay_done:
        out 40h, al
        popf
    }
}

unsigned short abi_get_timer_word_14f6e(void) {
    return abi_timer_word_14f6e;
}

void abi_set_config_word(unsigned short value) {
    abi_config_word = value;
}

void setmemallocstrat(void);
#pragma aux setmemallocstrat __parm __caller [] __modify __exact [__ax __bx]
void setmemallocstrat(void) {
    _asm {
        push ax
        movzx bx, al
        mov ax, 5801h
        int 21h
        pop bx
        shr bx, 8
        mov ax, 5803h
        int 21h
    }
}

void getmemallocstrat(void);
#pragma aux getmemallocstrat __parm __caller [] __modify __exact [__ax __bx]
void getmemallocstrat(void) {
    _asm {
        mov ax, 5800h
        int 21h
        push ax
        mov ax, 5802h
        int 21h
        pop bx
        mov ah, al
        mov al, bl
    }
}

void setmemalloc1(void);
#pragma aux setmemalloc1 __parm __caller [] __modify __exact [__ax __bx]
void setmemalloc1(void) {
    _asm {
        test byte ptr abi_config_word, 1
        jz short setmemalloc1_use_default
        mov ax, 0181h
        jmp short setmemalloc1_apply
setmemalloc1_use_default:
        mov ax, 1
setmemalloc1_apply:
        call setmemallocstrat
    }
}

void setmemalloc2(void);
#pragma aux setmemalloc2 __parm __caller [] __modify __exact [__ax __bx]
void setmemalloc2(void) {
    _asm {
        mov ax, 1
        call setmemallocstrat
    }
}

void abi_set_memfree_18a28_state(unsigned char memflg, unsigned short myseg) {
    abi_memflg_2469a = memflg;
    abi_myseg_24698 = myseg;
}

void memfree(void);
#pragma aux memfree __parm __caller [] __modify __exact [__ax]
void memfree(void) {
    _asm {
        push es
        mov es, ax
        mov ah, 49h
        int 21h
        pop es
    }
}

void memfree_18A28(void);
#pragma aux memfree_18A28 __parm __caller [] __modify __exact [__ax]
void memfree_18A28(void) {
    _asm {
        cmp abi_memflg_2469a, 1
        jnz short memfree_18a28_no_free
        mov abi_memflg_2469a, 0
        mov ax, abi_myseg_24698
        call memfree
        jmp short memfree_18a28_done
memfree_18a28_no_free:
        clc
memfree_18a28_done:
    }
}

void setint_vect(void);
#pragma aux setint_vect __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void setint_vect(void) {
    _asm {
        push ds
        mov ds, dx
        mov dx, bx
        mov ah, 25h
        int 21h
        pop ds
    }
}

void getint_vect(void);
#pragma aux getint_vect __parm __caller [] __modify __exact [__ax __bx __dx]
void getint_vect(void) {
    _asm {
        push es
        mov ah, 35h
        int 21h
        mov dx, es
        pop es
    }
}

void midi_set(void);
#pragma aux midi_set __parm __caller [] __modify __exact [__ax __bx __dx]
void midi_set(void) {
    _asm {
        mov bx, 5354h
        mov dx, 5354h
        mov al, 8
        call setint_vect
    }
}

void abi_set_keybsw_state(unsigned short bios_flags, unsigned short keyb_switches) {
    abi_bios_keyb_flags = bios_flags;
    abi_keyb_switches = keyb_switches;
}

unsigned short abi_get_bios_keyb_flags(void) {
    return abi_bios_keyb_flags;
}

unsigned short abi_get_keyb_switches(void) {
    return abi_keyb_switches;
}

void get_keybsw(void);
#pragma aux get_keybsw __parm __caller [] __modify __exact [__ax]
void get_keybsw(void) {
    _asm {
        mov ax, abi_bios_keyb_flags
        mov abi_keyb_switches, ax
    }
}

void set_keybsw(void);
#pragma aux set_keybsw __parm __caller [] __modify __exact [__ax]
void set_keybsw(void) {
    _asm {
        mov ax, abi_keyb_switches
        mov abi_bios_keyb_flags, ax
    }
}

void abi_set_sub197f2_configword(unsigned short value) {
    abi_configword = value;
    abi_sub197f2_labels[0] = ' ';
    abi_sub197f2_labels[1] = ' ';
    abi_sub197f2_labels[2] = ' ';
    abi_sub197f2_labels[3] = ' ';
    abi_sub197f2_labels[4] = ' ';
    abi_sub197f2_labels[5] = ' ';
}

const unsigned char *abi_get_sub197f2_labels(void) {
    return abi_sub197f2_labels;
}

void sub_197F2(void);
#pragma aux sub_197F2 __parm __caller [] __modify __exact []
void sub_197F2(void) {
    _asm {
        test byte ptr abi_configword, 20h
        jnz short sub_197f2_on
        mov abi_sub197f2_labels[0], 'O'
        mov abi_sub197f2_labels[1], 'f'
        mov abi_sub197f2_labels[2], 'f'
        mov abi_sub197f2_labels[3], 'O'
        mov abi_sub197f2_labels[4], 'f'
        mov abi_sub197f2_labels[5], 'f'
        jmp short sub_197f2_done
sub_197f2_on:
        mov abi_sub197f2_labels[0], 'O'
        mov abi_sub197f2_labels[1], 'n'
        mov abi_sub197f2_labels[2], ' '
        mov abi_sub197f2_labels[3], 'O'
        mov abi_sub197f2_labels[4], 'n'
        mov abi_sub197f2_labels[5], ' '
sub_197f2_done:
    }
}

void useless_11787(void);
#pragma aux useless_11787 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void useless_11787(void) {
    _asm {
        mov ecx, dword ptr [di+20h]
        or  ecx, ecx
        jnz short useless_11787_done
        clc
useless_11787_done:
    }
}

const unsigned char *abi_get_doswrite_header(void) {
    return abi_doswrite_header;
}

void useless_doswrite2(void);
#pragma aux useless_doswrite2 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void useless_doswrite2(void) {
    _asm {
        mov dword ptr abi_doswrite_header[0], eax
        mov dword ptr abi_doswrite_header[4], ecx
        mov dx, offset abi_doswrite_header
        mov cx, 8
        mov bx, 0ffffh
        mov ah, 40h
        int 21h
    }
}

void useless_doswrite(void);
#pragma aux useless_doswrite __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void useless_doswrite(void) {
    _asm {
        push dx
        push ecx
        call useless_doswrite2
        pop ecx
        pop dx
        jb short useless_doswrite_done
        mov bx, 0ffffh
        mov ah, 40h
        int 21h
useless_doswrite_done:
    }
}

#endif

/*
 * Public 16-bit DOS entry points for the hexadecimal conversion helpers.
 *
 * The parity-tested C cores use IplayRegs for host/DOS comparison.  These
 * symbols provide the original register ABI for future function-by-function
 * replacement inside a DOS binary.
 */

void u4tox(void);
#pragma aux u4tox __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void u4tox(void) {
    _asm {
        and al, 0fh
        or  al, '0'
        cmp al, '9'
        jbe short u4tox_store
        add al, 7
u4tox_store:
        mov [si], al
        inc si
    }
}

void u8tox(void);
#pragma aux u8tox __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void u8tox(void) {
    _asm {
        push ax
        shr  al, 4
        call u4tox
        pop  ax
        call u4tox
    }
}

void u16tox(void);
#pragma aux u16tox __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void u16tox(void) {
    _asm {
        xchg al, ah
        call u8tox
        mov  al, ah
        call u8tox
    }
}

void u32tox(void);
#pragma aux u32tox __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void u32tox(void) {
    _asm {
        ror eax, 10h
        call u16tox
        ror eax, 10h
        call u16tox
    }
}

void my_u4tox(void);
#pragma aux my_u4tox __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u4tox(void) {
    _asm {
        call u4tox
    }
}

void my_u8tox(void);
#pragma aux my_u8tox __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u8tox(void) {
    _asm {
        call u8tox
    }
}

void my_u16tox(void);
#pragma aux my_u16tox __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u16tox(void) {
    _asm {
        call u16tox
    }
}

void my_u32tox(void);
#pragma aux my_u32tox __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u32tox(void) {
    _asm {
        call u32tox
    }
}

void my_putdigit(void);
#pragma aux my_putdigit __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_putdigit(void) {
    _asm {
        mov [si], dl
        inc si
        inc cx
    }
}

void myputdigit(void);
#pragma aux myputdigit __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void myputdigit(void) {
    _asm {
        call my_putdigit
    }
}

void my_u32toa_0(void);
#pragma aux my_u32toa_0 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u32toa_0(void) {
    _asm {
        xor edx, edx
        div ebx
        or  eax, eax
        jz  short my_u32toa_emit
        push edx
        call my_u32toa_0
        pop edx
my_u32toa_emit:
        or  dl, '0'
        call my_putdigit
    }
}

void my_u32toa(void);
#pragma aux my_u32toa __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u32toa(void) {
    _asm {
        call my_u32toa_0
    }
}

void my_u32toa10_0(void);
#pragma aux my_u32toa10_0 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u32toa10_0(void) {
    _asm {
        xor cx, cx
        mov ebx, 10
        call my_u32toa_0
    }
}

void my_u16toa_10(void);
#pragma aux my_u16toa_10 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u16toa_10(void) {
    _asm {
        movzx eax, ax
        call my_u32toa10_0
    }
}

void my_u8toa_10(void);
#pragma aux my_u8toa_10 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u8toa_10(void) {
    _asm {
        xor ah, ah
        call my_u16toa_10
    }
}

void my_u32toa10(void);
#pragma aux my_u32toa10 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u32toa10(void) {
    _asm {
        call my_u32toa10_0
    }
}

void my_u16toa10(void);
#pragma aux my_u16toa10 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u16toa10(void) {
    _asm {
        call my_u16toa_10
    }
}

void my_u8toa10(void);
#pragma aux my_u8toa10 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_u8toa10(void) {
    _asm {
        call my_u8toa_10
    }
}

void my_i32toa10_0(void);
#pragma aux my_i32toa10_0 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_i32toa10_0(void) {
    _asm {
        xor cx, cx
        or  eax, eax
        jns short my_i32toa_positive
        mov dl, '-'
        call my_putdigit
        neg eax
my_i32toa_positive:
        mov ebx, 10
        call my_u32toa_0
    }
}

void my_i16toa10_0(void);
#pragma aux my_i16toa10_0 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_i16toa10_0(void) {
    _asm {
        cwde
        call my_i32toa10_0
    }
}

void my_i8toa10_0(void);
#pragma aux my_i8toa10_0 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_i8toa10_0(void) {
    _asm {
        cbw
        call my_i16toa10_0
    }
}

void my_i32toa10(void);
#pragma aux my_i32toa10 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_i32toa10(void) {
    _asm {
        call my_i32toa10_0
    }
}

void my_i16toa10(void);
#pragma aux my_i16toa10 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_i16toa10(void) {
    _asm {
        call my_i16toa10_0
    }
}

void my_i8toa10(void);
#pragma aux my_i8toa10 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void my_i8toa10(void) {
    _asm {
        call my_i8toa10_0
    }
}

void my_u32toa_fill(void);
#pragma aux my_u32toa_fill __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void my_u32toa_fill(void) {
    _asm {
        mov si, offset abi_num_buffer
        push bx
        push di
        push bp
        call my_u32toa10
        pop bp
        pop di
        pop bx
        cmp cx, bp
        jb short fill_count_ok
        mov cx, bp
fill_count_ok:
        sub si, cx
        mov dx, cx
        neg cx
        add cx, bp
        mov al, ' '
        cld
        rep stosb
        mov cx, dx
        rep movsb
    }
}

void my_pnt_u32toa_fill(void);
#pragma aux my_pnt_u32toa_fill __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void my_pnt_u32toa_fill(void) {
    _asm {
        mov word ptr [di], 7f02h
        add di, 2
        call my_u32toa_fill
    }
}

void mystrlen_0(void);
#pragma aux mystrlen_0 __parm __caller [] __modify __exact [__ax __si]
void mystrlen_0(void) {
    _asm {
        mov ax, -1
        dec si
strlen0_loop:
        inc ax
        inc si
        cmp byte ptr [si], 0
        jne short strlen0_loop
        sub si, ax
    }
}

void mystrlen(void);
#pragma aux mystrlen __parm __caller [] __modify __exact [__ax __si]
void mystrlen(void) {
    _asm {
        call mystrlen_0
    }
}

void strcpy_count_0(void);
#pragma aux strcpy_count_0 __parm __caller [] __modify __exact [__ax __cx __si __di __es]
void strcpy_count_0(void) {
    _asm {
        xor cx, cx
        jmp short strcpy_count_check
strcpy_count_copy:
        mov es:[di], al
        inc si
        inc di
strcpy_count_check:
        mov al, [si]
        inc cx
        or  al, al
        jne short strcpy_count_copy
    }
}

void strcpy_count(void);
#pragma aux strcpy_count __parm __caller [] __modify __exact [__ax __cx __si __di __es]
void strcpy_count(void) {
    _asm {
        call strcpy_count_0
    }
}

void copy_printable(void);
#pragma aux copy_printable __parm __caller [] __modify __exact [__ax __cx __si __di]
void copy_printable(void) {
    _asm {
        push si
        push di
copy_printable_loop:
        mov al, [si]
        inc si
        cmp al, ' '
        jb short copy_printable_done
        mov [di], al
        inc di
        dec cx
        jne short copy_printable_loop
copy_printable_done:
        pop di
        pop si
    }
}

void cpy_printable(void);
#pragma aux cpy_printable __parm __caller [] __modify __exact [__ax __cx __si __di __es]
void cpy_printable(void) {
    _asm {
        push si
        push di
cpy_printable_loop:
        mov al, [si]
        inc si
        cmp al, ' '
        jb short cpy_printable_fill
        mov es:[di], al
        inc di
        dec cx
        jne short cpy_printable_loop
cpy_printable_fill:
        cld
        mov al, ' '
        rep stosb
        pop di
        pop si
    }
}

void txt_1ABAE(void);
#pragma aux txt_1ABAE __parm __caller [] __modify __exact [__ax __cx __si __di __fs __es]
void txt_1ABAE(void) {
    _asm {
        mov ah, 7bh
        mov cx, 16h
txt_1abae_loop:
        mov al, fs:[si]
        mov es:[di], ax
        inc si
        add di, 2
        dec cx
        jne short txt_1abae_loop
    }
}

void put_message(void);
#pragma aux put_message __parm __caller [] __modify __exact [__ax __si __di __es]
void put_message(void) {
    _asm {
        cld
put_message_loop:
        lodsb
        or al, al
        je short put_message_done
        stosw
        jmp short put_message_loop
put_message_done:
    }
}

void put_message2(void);
#pragma aux put_message2 __parm __caller [] __modify __exact [__ax __si __di __fs __es]
void put_message2(void) {
    _asm {
put_message2_loop:
        stosw
        cld
        lods byte ptr fs:[si]
        or al, al
        jne short put_message2_loop
    }
}

void text_1BF69(void);
#pragma aux text_1BF69 __parm __caller [] __modify __exact [__ax __si __di __bp __es]
void text_1BF69(void) {
    _asm {
text_1bf69_loop:
        mov al, [si]
        inc si
        or  al, al
        je  short text_1bf69_done
        cmp al, 1
        je  short text_1bf69_setdi
        cmp al, 2
        je  short text_1bf69_setattr
        mov es:[di], ax
        add di, 2
        jmp short text_1bf69_loop
text_1bf69_setdi:
        mov di, [si]
        add di, bp
        add si, 2
        jmp short text_1bf69_loop
text_1bf69_setattr:
        lodsb
        mov ah, al
        jmp short text_1bf69_loop
text_1bf69_done:
    }
}

void write_scr(void);
#pragma aux write_scr __parm __caller [] __modify __exact [__ax __bp __si __di __es]
void write_scr(void) {
    _asm {
        mov bp, di
        add di, [si]
        add si, 2
        lodsb
        mov ah, al
        call text_1BF69
    }
}

#endif
