#include "iplay_rewrite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORIG_SRC_OFF 0x9000u
#define ORIG_DST_OFF 0x9100u

static db mem[0x9400];

unsigned short abi_get_timer_word_14f6e(void);
void abi_set_config_word(unsigned short value);
void abi_set_memfree_18a28_state(unsigned char memflg, unsigned short myseg);
void abi_set_keybsw_state(unsigned short bios_flags, unsigned short keyb_switches);
unsigned short abi_get_bios_keyb_flags(void);
unsigned short abi_get_keyb_switches(void);
void abi_set_sndvector_state(unsigned short old_off, unsigned short old_seg);
const unsigned char *abi_get_sndvector_data(void);
void abi_set_sb16_probe_state(void);
const unsigned char *abi_get_sb16_probe_data(void);
const unsigned char *abi_get_sb16_init_data(void);
void abi_set_sb16_int_state(unsigned char counter);
unsigned char abi_get_sb16_int_counter(void);
const unsigned char *abi_get_sb16_dma_data(void);
const unsigned char *abi_get_inr_read_data(void);
const unsigned char *abi_get_modread12247_data(void);
const unsigned char *abi_get_modread11f4e_data(void);
const unsigned char *abi_get_sb16off_data(void);
const unsigned char *abi_get_cleandeinit_data(void);
const unsigned char *abi_get_dosdir_data(void);
const unsigned char *abi_get_dosfindnext_data(void);
const unsigned char *abi_get_dosfread_data(void);
const unsigned char *abi_get_dosseek_data(void);
const unsigned char *abi_get_read2buffer_data(void);
const unsigned char *abi_get_allocdma_data(void);
const unsigned char *abi_get_gravisdma_data(void);
const unsigned char *abi_get_sub1279_data(void);
const unsigned char *abi_get_programdma_data(void);
const unsigned char *abi_get_memreallocx_data(void);
const unsigned char *abi_get_deinit125b9_data(void);
const unsigned char *abi_get_rtcclock_data(void);
const unsigned char *abi_get_loadcfg_data(void);
const unsigned char *abi_get_dosexec_data(void);
const unsigned char *abi_get_callsubx_data(void);
const unsigned char *abi_get_f2waves_data(void);
const unsigned char *abi_get_initvga_data(void);
const unsigned char *abi_get_f2draw_data(void);
const unsigned char *abi_get_readallmoules_data(void);
const unsigned char *abi_get_readmodule_data(void);
const unsigned char *abi_get_moduleread_data(void);
const unsigned char *abi_get_modread10311_data(void);
const unsigned char *abi_get_modnt_data(void);
const unsigned char *abi_get_formatloader_data(void);
const unsigned char *abi_get_modulessearch_data(void);
const unsigned char *abi_get_start_data(void);
const unsigned char *abi_get_keyb19efd_data(void);
const unsigned char *abi_get_spectr1bce9_data(void);
const unsigned char *abi_get_spectr1bc2d_data(void);
const unsigned char *abi_get_spectr1bbc1_data(void);
void abi_set_videoprp_inputs(unsigned char first, unsigned char second, unsigned char third);
const unsigned char *abi_get_videoprp_data(void);
void _2stm_module(void);
void e669_module(void);
void mtm_module(void);
void psm_module(void);
void far_module(void);
void ult_module(void);
void s3m_module(void);
void inr_module(void);
void keyb_19EFD(void);
void spectr_1BCE9(void);
void spectr_1BC2D(void);
void spectr_1BBC1(void);
void video_prp_mtr_positn(void);
void u4tox(void);
void u8tox(void);
void u16tox(void);
void my_u8toa_10(void);
void my_u16toa_10(void);
void my_i8toa10_0(void);
void my_i16toa10_0(void);
void my_u32toa_0(void);
void abi_set_playsettings_state(unsigned char value);
void abi_set_setplaysettings_state(unsigned char value, unsigned char config_hi, unsigned short freq, unsigned char shift);
const unsigned char *abi_get_setplaysettings_data(void);
void set_playsettings(void);
void abi_set_sub12afd_state(unsigned short value, unsigned char channel_count, unsigned char channel_index, unsigned char flags);
unsigned char abi_get_sub12afd_flag(void);
void sub_12AFD(void);
void abi_set_sub12b18_state(unsigned char channels, const unsigned char *src);
const unsigned char *abi_get_sub12b18_data(void);
void sub_12B18(void);
void abi_set_sub12b83_state(unsigned char value, const unsigned char *types, unsigned char sound_mode);
const unsigned char *abi_get_sub12b83_data(void);
unsigned char abi_get_sub12b83_count(void);
void sub_12B83(void);
void abi_set_someplaymode_state(unsigned char playsettings, unsigned short freq, unsigned short channels, unsigned char shift, unsigned char sndflags);
const unsigned char *abi_get_someplaymode_data(void);
void someplaymode(void);
const unsigned char *abi_get_sub12d05_data(void);
void sub_12D05(void);
void abi_set_sub1ab8c_state(unsigned char note_byte, unsigned char transpose);
void sub_1AB8C(void);
const unsigned char *abi_get_sub13d95_data(void);
void sub_13D95(void);
void abi_set_sub13cf6_state(unsigned short freq, unsigned short buffer_size);
const unsigned char *abi_get_sub13cf6_data(void);
void sub_13CF6(void);
void abi_set_sub13044_state(unsigned char mode, unsigned short divisor, unsigned short amplification, unsigned char high_amplif);
const unsigned char *abi_get_sub13044_data(void);
void sub_13044(void);
const unsigned char *abi_get_drawframe_data(void);
void draw_frame(void);
void txt_draw_top_title(void);
void abi_set_txtdrawbottom_state(unsigned char byte_1de72, unsigned char byte_1de73, unsigned char byte_1de74, unsigned char byte_1de75, unsigned char byte_1de76, unsigned char flags, unsigned short volume, unsigned short amplif);
void txt_draw_bottom(void);
void abi_set_message1be77_state(const unsigned char *text, unsigned char y, unsigned char attr);
unsigned short abi_get_message1be77_si(void);
unsigned short abi_get_message1be77_di(void);
void message_1BE77(void);
const unsigned char *abi_get_recolortxt_data(void);
unsigned short abi_get_recolortxt_ax(void);
unsigned short abi_get_recolortxt_bx(void);
void recolortxt(void);
void abi_set_mouse1c7cf_records(const unsigned char *records, unsigned short count);
void abi_set_mouse1c7cf_inputs(unsigned short ax, unsigned short bp, unsigned short bx);
unsigned short abi_get_mouse1c7cf_ax(void);
unsigned short abi_get_mouse1c7cf_bp(void);
unsigned short abi_get_mouse1c7cf_bx(void);
unsigned short abi_get_mouse1c7cf_cx(void);
unsigned short abi_get_mouse1c7cf_dx(void);
unsigned short abi_get_mouse1c7cf_si(void);
unsigned short abi_get_mouse1c7cf_di(void);
void mouse_1C7CF(void);
void sub_13E9B(void);
void ems_restore_mapctx(void);
unsigned char abi_get_ems_enabled_byte(void);
void ems_init(void);
void ems_release(void);
void ems_realloc(void);
void ems_deinit(void);
void ems_save_mapctx(void);
void ems_mapmem(void);
void ems_mapmem2(void);
void abi_set_emsmapcopy_source(const unsigned char *payload);
const unsigned char *abi_get_emsmapcopy_data(void);
void ems_mapmemx(void);
void ems_mapmemy(void);
void abi_set_emsrealloc2_state(unsigned char initial_count, unsigned long requested_size);
unsigned char abi_get_emsrealloc2_count(void);
void ems_realloc2(void);
void abi_set_clean11c43_state(unsigned char flag, unsigned char byte_2461e, unsigned char byte_2461f);
const unsigned char *abi_get_clean11c43_data(void);
void clean_11C43(void);
void mod_sub_delta(void);
void sub_11BA6(void);
void mod_102F5(void);
void ult_read(void);
void volume_prep(void);
void sub_135CA(void);
void memfree_125DA(void);
void mouse_getpos(void);
void mouse_showcur(void);
void mouse_hide2(void);
void mouse_show(void);
void mouse_hide(void);
void mouse_deinit(void);
void mouse_init(void);
void get_comspec(void);
void getexename(void);
void spectr_1C4F8(void);
void spectr_1B406(void);
const unsigned char *abi_get_myasmsprintf_data(void);
void myasmsprintf(void);
const unsigned char *abi_get_textsetup_data(void);
void text_init(void);
void text_init2(void);
void f1_help(void);
void f3_textmetter(void);
void f4_patternnae(void);
void f6_undoc(void);
void abi_set_playstate_state(unsigned char value);
void abi_set_get12f7c_state(unsigned short word_245f0, unsigned short word_245f6);
void abi_set_volume12a66_state(unsigned short channels);
void abi_set_vlm141df_state(unsigned short channels);
unsigned char abi_get_vlm141df_byte_24671(void);
void abi_set_memclean_state(unsigned short size);
void abi_set_sub131ef_state(unsigned short volume, unsigned char max_volume);
void abi_set_sub13177_state(unsigned long dword_245bc, unsigned long dword_245c0, unsigned char shift);
void abi_set_midi154ac_state(unsigned char max_volume);
void abi_set_midi15413_state(unsigned char last_status);
unsigned char abi_get_midi15413_last_status(void);
void abi_set_midi_port_state(unsigned short base_port, unsigned char last_status, unsigned char byte_24678);
unsigned char abi_get_midi_port_last_status(void);
unsigned char abi_get_midi_port_byte_24678(void);
void abi_set_sub12d35_state(unsigned char code_byte);
unsigned char abi_get_sub12d35_code_byte(void);
void abi_set_sub12da8_state(void);
const unsigned char *abi_get_sub12da8_data(void);
void abi_set_sub197f2_configword(unsigned short value);
const unsigned char *abi_get_sub197f2_labels(void);
const unsigned char *abi_get_doswrite_header(void);
void abi_set_sub1415e_state(unsigned short index, unsigned short total, unsigned char segment_index, unsigned char pending);
const unsigned char *abi_get_sub1415e_data(void);
void abi_set_sub154f4_state(unsigned short buffer_size2, unsigned char flag_playsettings);
const unsigned char *abi_get_sub154f4_data(void);
void abi_set_sub1609f_state(unsigned short buffer_size2);
void abi_set_sub13826_state(unsigned short table_word);
void abi_set_sub140b6_state(unsigned char byte_24671, unsigned char byte_24668);
unsigned char abi_get_sub140b6_byte_24671(void);
unsigned char abi_get_sub140b6_byte_24668(void);
void abi_set_sub14087_state(unsigned char byte_24668);
void abi_set_calc14043_state(unsigned char byte_2467b, unsigned char byte_2467c);
unsigned char abi_get_calc14043_byte_2467b(void);
unsigned char abi_get_calc14043_byte_2467c(void);
void abi_set_eff14030_state(unsigned char byte_2467c, unsigned short freq, unsigned short buffer_size);
const unsigned char *abi_get_eff14030_data(void);
void abi_set_change_volume_state(unsigned short channels, unsigned char channel0);
const unsigned char *abi_get_change_volume_data(void);
void abi_set_amplif_state(unsigned char sound_mode);
const unsigned char *abi_get_amplif_data(void);
void abi_set_eff14067_state(unsigned char byte_2467b, unsigned char byte_2467c, unsigned short freq, unsigned short buffer_size);
const unsigned char *abi_get_eff14067_data(void);
void abi_set_eff13ce8_state(unsigned char byte_24667, unsigned char byte_24668);
const unsigned char *abi_get_eff13ce8_data(void);
void abi_set_eff13cdd_state(unsigned char flag_playsettings, unsigned char byte_24667, unsigned char byte_24668);
void abi_set_eff13a43_state(unsigned char sndflags);
void abi_set_eff13a94_state(unsigned char byte_2461a);
void abi_set_eff13ad7_state(unsigned char max_volume);
void abi_set_eff13b06_state(void);
const unsigned char *abi_get_eff13b06_data(void);
void abi_set_eff13b78_state(unsigned char max_volume);
void abi_set_eff13b88_state(unsigned char byte_24669, unsigned char byte_2466a);
const unsigned char *abi_get_eff13b88_data(void);
void abi_set_effect_slide_state(unsigned char byte_24668);
void abi_set_eff1392f_state(unsigned char flag_playsettings);
void abi_set_eff139ac_state(unsigned char max_volume);
void abi_set_eff139b2_state(unsigned char max_volume, unsigned char flag_playsettings);
void abi_set_eff139b9_state(unsigned char max_volume);
void abi_set_eff13bc8_state(unsigned char byte_2461a);
void abi_set_eff13c02_state(unsigned char byte_24668, unsigned short word_245f6);
const unsigned char *abi_get_eff13c02_globals(void);
void abi_set_eff13c3f_state(unsigned char byte_24668, unsigned char sndflags);
void abi_set_eff13c64_state(unsigned char byte_24668);
void abi_set_eff13c88_state(unsigned char byte_24668, unsigned char max_volume);
void abi_set_eff13c95_state(unsigned char byte_24668);
void abi_set_eff13ca2_state(unsigned char byte_24668);
void abi_set_eff13cb3_state(unsigned char byte_24668);
void abi_set_eff13cc9_state(unsigned char byte_24668, unsigned char byte_2466d, unsigned char byte_2466c);
unsigned char abi_get_eff13cc9_byte_2466c(void);
void abi_set_eff13e32_state(unsigned char byte_24668, unsigned char max_volume);
void abi_set_eff13e8c_state(unsigned short freq, unsigned short buffer_size);
const unsigned char *abi_get_eff13e8c_data(void);
void abi_set_eff13f05_state(unsigned char byte_24668);
void abi_set_eff13f3b_state(unsigned char byte_24668, unsigned char max_volume);
void abi_set_eff13fbe_state(unsigned char byte_24668);

static unsigned long parse_u32(const char *s) { return strtoul(s, 0, 0); }

static int streq(const char *a, const char *b) { return strcmp(a, b) == 0; }

static int hex_nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static size_t parse_hex_bytes(const char *s, db *out, size_t max) {
    size_t n = 0;
    while (s[0] != 0 && s[1] != 0 && n < max) {
        int hi = hex_nibble(s[0]);
        int lo = hex_nibble(s[1]);
        if (hi < 0 || lo < 0) break;
        out[n++] = (db)((hi << 4) | lo);
        s += 2;
    }
    return n;
}

static void print_bytes(const db *p, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

int main(int argc, char **argv) {
    const char *op;
    memset(mem, 0, sizeof(mem));
    if (argc < 2) return 2;
    op = argv[1];

    if (streq(op, "abinoop")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "nullsub_5")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call nullsub_5
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "eff_nullsub")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call eff_nullsub
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "nullsub_2")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call nullsub_2
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "nullsub_4")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call nullsub_4
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

    if (streq(op, "abigetplaysettings")) {
        unsigned value;
        unsigned ax_after;
        if (argc != 3) return 2;
        value = (unsigned)parse_u32(argv[2]);
        abi_set_playsettings_state((unsigned char)value);
        _asm {
            xor ax, ax
            call get_playsettings
            mov ax_after, ax
        }
        printf("ax=%04x\n", ax_after);
        return 0;
    }

    if (streq(op, "abisetplaysettings")) {
        unsigned value;
        unsigned config_hi;
        unsigned freq;
        unsigned channels;
        unsigned shift;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 7) return 2;
        value = (unsigned)parse_u32(argv[2]);
        config_hi = (unsigned)parse_u32(argv[3]);
        freq = (unsigned)parse_u32(argv[4]);
        channels = (unsigned)parse_u32(argv[5]);
        shift = (unsigned)parse_u32(argv[6]);
        (void)channels;
        abi_set_setplaysettings_state((unsigned char)value, (unsigned char)config_hi, (unsigned short)freq, (unsigned char)shift);
        _asm {
            mov ax, value
            call set_playsettings
            mov ax_after, ax
        }
        data = abi_get_setplaysettings_data();
        printf("ax=%04x data=", ax_after);
        print_bytes(data, 12);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub12afd")) {
        unsigned value;
        unsigned channels;
        unsigned channel_index;
        unsigned flags;
        if (argc != 6) return 2;
        value = (unsigned)parse_u32(argv[2]);
        channels = (unsigned)parse_u32(argv[3]);
        channel_index = (unsigned)parse_u32(argv[4]);
        flags = (unsigned)parse_u32(argv[5]);
        abi_set_sub12afd_state((unsigned short)value, (unsigned char)channels, (unsigned char)channel_index, (unsigned char)flags);
        abi_set_eff13a43_state(0);
        _asm {
            mov ax, value
            mov cx, channel_index
            shl cx, 8
            call sub_12AFD
        }
        printf("data=%02x\n", (unsigned)abi_get_sub12afd_flag());
        return 0;
    }

    if (streq(op, "abisub12b18")) {
        unsigned channels;
        unsigned char src[32];
        const unsigned char *data;
        if (argc != 4) return 2;
        channels = (unsigned)parse_u32(argv[2]);
        memset(src, 0, sizeof(src));
        parse_hex_bytes(argv[3], src, sizeof(src));
        abi_set_sub12b18_state((unsigned char)channels, src);
        _asm {
            call sub_12B18
        }
        data = abi_get_sub12b18_data();
        printf("data=");
        print_bytes(data, 2 + channels * 2u);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub12b83")) {
        unsigned value;
        unsigned sound_mode;
        unsigned count;
        unsigned char types[32];
        const unsigned char *data;
        if (argc != 5) return 2;
        value = (unsigned)parse_u32(argv[2]);
        sound_mode = (unsigned)parse_u32(argv[4]);
        memset(types, 0, sizeof(types));
        parse_hex_bytes(argv[3], types, sizeof(types));
        abi_set_sub12b83_state((unsigned char)value, types, (unsigned char)sound_mode);
        _asm {
            mov ax, value
            call sub_12B83
        }
        count = (unsigned)abi_get_sub12b83_count();
        data = abi_get_sub12b83_data();
        printf("data=");
        print_bytes(data, 20 + count * 3u);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisomeplaymode")) {
        unsigned playsettings;
        unsigned freq;
        unsigned channels;
        unsigned shift;
        unsigned sndflags;
        const unsigned char *data;
        if (argc != 7) return 2;
        playsettings = (unsigned)parse_u32(argv[2]);
        freq = (unsigned)parse_u32(argv[3]);
        channels = (unsigned)parse_u32(argv[4]);
        shift = (unsigned)parse_u32(argv[5]);
        sndflags = (unsigned)parse_u32(argv[6]);
        abi_set_someplaymode_state((unsigned char)playsettings, (unsigned short)freq, (unsigned short)channels, (unsigned char)shift, (unsigned char)sndflags);
        _asm {
            call someplaymode
        }
        data = abi_get_someplaymode_data();
        printf("data=");
        print_bytes(data, 14);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub12d05")) {
        unsigned snd_init;
        unsigned sndcard_type;
        unsigned cx_after;
        const unsigned char *data;
        if (argc != 4) return 2;
        snd_init = (unsigned)parse_u32(argv[2]);
        sndcard_type = (unsigned)parse_u32(argv[3]);
        (void)snd_init;
        (void)sndcard_type;
        _asm {
            mov di, 2800h
            call sub_12D05
            mov cx_after, cx
        }
        data = abi_get_sub12d05_data();
        printf("cx=%04x data=", cx_after);
        print_bytes(data, 23);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub1ab8c")) {
        unsigned note_byte;
        unsigned transpose;
        unsigned ax_after;
        unsigned si_after;
        if (argc != 4) return 2;
        note_byte = (unsigned)parse_u32(argv[2]);
        transpose = (unsigned)parse_u32(argv[3]);
        abi_set_sub1ab8c_state((unsigned char)note_byte, (unsigned char)transpose);
        _asm {
            mov si, 2222h
            call sub_1AB8C
            mov ax_after, ax
            mov si_after, si
        }
        printf("ax=%04x si=%04x\n", ax_after, si_after);
        return 0;
    }

    if (streq(op, "abitextsetup")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "text_init")) {
            _asm { call text_init }
        } else if (streq(symbol, "text_init2")) {
            _asm { call text_init2 }
        } else if (streq(symbol, "f1_help")) {
            _asm { call f1_help }
        } else if (streq(symbol, "f3_textmetter")) {
            _asm { call f3_textmetter }
        } else if (streq(symbol, "f4_patternnae")) {
            _asm { call f4_patternnae }
        } else if (streq(symbol, "f6_undoc")) {
            _asm { call f6_undoc }
        } else {
            return 2;
        }
        _asm {
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        data = abi_get_textsetup_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(data, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub13d95")) {
        unsigned divisor;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        divisor = (unsigned)parse_u32(argv[2]);
        _asm {
            mov cx, divisor
            call sub_13D95
            mov ax_after, ax
        }
        data = mem + 0x0078u;
        printf("ax=%04x data=", ax_after);
        print_bytes(data, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub13cf6")) {
        unsigned value;
        unsigned freq;
        unsigned buffer_size;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 5) return 2;
        value = (unsigned)parse_u32(argv[2]);
        freq = (unsigned)parse_u32(argv[3]);
        buffer_size = (unsigned)parse_u32(argv[4]);
        abi_set_sub13cf6_state((unsigned short)freq, (unsigned short)buffer_size);
        _asm {
            mov ax, value
            call sub_13CF6
            mov ax_after, ax
        }
        data = abi_get_sub13cf6_data();
        printf("ax=%04x data=", ax_after);
        print_bytes(data, 9);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub13044")) {
        unsigned mode;
        unsigned divisor;
        unsigned amplification;
        unsigned high_amplif;
        const unsigned char *data;
        if (argc != 6) return 2;
        mode = (unsigned)parse_u32(argv[2]);
        divisor = (unsigned)parse_u32(argv[3]);
        amplification = (unsigned)parse_u32(argv[4]);
        high_amplif = (unsigned)parse_u32(argv[5]);
        abi_set_sub13044_state((unsigned char)mode, (unsigned short)divisor, (unsigned short)amplification, (unsigned char)high_amplif);
        _asm {
            call sub_13044
        }
        data = abi_get_sub13044_data();
        printf("data=");
        print_bytes(data, 38);
        printf("\n");
        return 0;
    }

    if (streq(op, "abidrawframe")) {
        unsigned style;
        unsigned attr;
        unsigned fill_attr;
        unsigned x;
        unsigned y;
        unsigned right;
        unsigned bottom;
        const unsigned char *data;
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
        data = abi_get_drawframe_data();
        printf("data=");
        print_bytes(data, 400);
        printf("\n");
        return 0;
    }

    if (streq(op, "abitxtdrawtoptitle")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call txt_draw_top_title
        }
        data = abi_get_drawframe_data();
        printf("data=");
        print_bytes(data, 0x500);
        printf("\n");
        return 0;
    }

    if (streq(op, "abitxtdrawbottom")) {
        unsigned byte_1de72;
        unsigned byte_1de73;
        unsigned byte_1de74;
        unsigned byte_1de75;
        unsigned byte_1de76;
        unsigned flags;
        unsigned volume;
        unsigned amplif;
        const unsigned char *data;
        if (argc != 10) return 2;
        byte_1de72 = (unsigned)parse_u32(argv[2]);
        byte_1de73 = (unsigned)parse_u32(argv[3]);
        byte_1de74 = (unsigned)parse_u32(argv[4]);
        byte_1de75 = (unsigned)parse_u32(argv[5]);
        byte_1de76 = (unsigned)parse_u32(argv[6]);
        flags = (unsigned)parse_u32(argv[7]);
        volume = (unsigned)parse_u32(argv[8]);
        amplif = (unsigned)parse_u32(argv[9]);
        abi_set_txtdrawbottom_state((unsigned char)byte_1de72, (unsigned char)byte_1de73, (unsigned char)byte_1de74, (unsigned char)byte_1de75, (unsigned char)byte_1de76, (unsigned char)flags, (unsigned short)volume, (unsigned short)amplif);
        _asm {
            call txt_draw_bottom
        }
        data = abi_get_drawframe_data();
        printf("data=");
        print_bytes(data, 0x600);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimessage1be77")) {
        char text[80];
        unsigned y;
        unsigned attr;
        const unsigned char *data;
        if (argc != 5) return 2;
        strncpy(text, argv[2], sizeof(text) - 1);
        text[sizeof(text) - 1] = 0;
        y = (unsigned)parse_u32(argv[3]);
        attr = (unsigned)parse_u32(argv[4]);
        abi_set_message1be77_state((const unsigned char *)text, (unsigned char)y, (unsigned char)attr);
        _asm {
            call message_1BE77
        }
        data = abi_get_drawframe_data();
        printf("si=%04x di=%04x data=", abi_get_message1be77_si(), abi_get_message1be77_di());
        print_bytes(data, 1000);
        printf("\n");
        return 0;
    }

    if (streq(op, "abirecolortxt")) {
        unsigned row;
        unsigned color;
        unsigned ax_after;
        unsigned bx_after;
        const unsigned char *data;
        if (argc != 4) return 2;
        row = (unsigned)parse_u32(argv[2]);
        color = (unsigned)parse_u32(argv[3]);
        _asm {
            mov ax, row
            mov bx, color
            call recolortxt
            mov ax_after, ax
            mov bx_after, bx
        }
        (void)ax_after;
        (void)bx_after;
        data = abi_get_recolortxt_data();
        printf("ax=%04x bx=%04x data=", abi_get_recolortxt_ax(), abi_get_recolortxt_bx());
        print_bytes(data, 64);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub13e9b")) {
        unsigned value;
        unsigned ax_after;
        unsigned dx_after;
        unsigned di_after;
        if (argc != 3) return 2;
        value = (unsigned)parse_u32(argv[2]);
        _asm {
            mov ax, value
            call sub_13E9B
            mov ax_after, ax
            mov dx_after, dx
            mov di_after, di
        }
        printf("ax=%04x dx=%04x di=%04x\n", ax_after, dx_after, di_after);
        return 0;
    }

    if (streq(op, "abimyasmsprintf")) {
        unsigned si_after;
        unsigned di_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov si, 2800h
            mov di, 2840h
            call myasmsprintf
            mov si_after, si
            mov di_after, di
        }
        data = abi_get_myasmsprintf_data();
        printf("si=%04x di=%04x data=", si_after, di_after);
        print_bytes(data, 20);
        printf("\n");
        return 0;
    }

    if (streq(op, "abigetsetplaystate")) {
        unsigned initial;
        unsigned request;
        unsigned ax_after;
        if (argc != 4) return 2;
        initial = (unsigned)parse_u32(argv[2]);
        request = (unsigned)parse_u32(argv[3]);
        abi_set_playstate_state((unsigned char)initial);
        _asm {
            mov ax, request
            call getset_playstate
            mov ax_after, ax
        }
        printf("ax=%04x\n", ax_after);
        return 0;
    }

    if (streq(op, "abiget12f7c")) {
        unsigned word_245f0;
        unsigned word_245f6;
        unsigned ax_after;
        unsigned bx_after;
        if (argc != 4) return 2;
        word_245f0 = (unsigned)parse_u32(argv[2]);
        word_245f6 = (unsigned)parse_u32(argv[3]);
        abi_set_get12f7c_state((unsigned short)word_245f0, (unsigned short)word_245f6);
        _asm {
            call get_12F7C
            mov ax_after, ax
            mov bx_after, bx
        }
        printf("ax=%04x bx=%04x\n", ax_after, bx_after);
        return 0;
    }

    if (streq(op, "abivolume12a66")) {
        unsigned channels;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        if (argc != 3) return 2;
        channels = (unsigned)parse_u32(argv[2]);
        abi_set_volume12a66_state((unsigned short)channels);
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

    if (streq(op, "abivlm141df")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        abi_set_vlm141df_state(1);
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
               ax_after, bx_after, cx_after, dx_after, abi_get_vlm141df_byte_24671());
        return 0;
    }

    if (streq(op, "abimemclean")) {
        unsigned size;
        unsigned fill_count;
        unsigned di_after;
        if (argc != 4) return 2;
        size = (unsigned)parse_u32(argv[2]);
        fill_count = (unsigned)parse_u32(argv[3]);
        memset(mem + 0x9000u, 0xa5, fill_count);
        abi_set_memclean_state((unsigned short)size);
        _asm {
            mov di, offset mem
            add di, 9000h
            call memclean
            mov di_after, di
        }
        di_after = (unsigned)(0x2800u + (di_after - ((unsigned)mem + 0x9000u)));
        printf("di=%04x data=", di_after);
        print_bytes(mem + 0x9000u, fill_count);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub131da")) {
        unsigned channel_type;
        unsigned flags;
        unsigned note_byte;
        if (argc != 5) return 2;
        channel_type = (unsigned)parse_u32(argv[2]);
        flags = (unsigned)parse_u32(argv[3]);
        note_byte = (unsigned)parse_u32(argv[4]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x1du] = (unsigned char)channel_type;
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        mem[0x9000u + 0x35u] = (unsigned char)note_byte;
        _asm {
            mov bx, offset mem
            add bx, 9000h
            call sub_131DA
        }
        printf("data=%02x%02x\n", mem[0x9000u + 0x17u], mem[0x9000u + 0x35u]);
        return 0;
    }

    if (streq(op, "abisub131ef")) {
        unsigned value;
        unsigned volume;
        unsigned max_volume;
        unsigned old_fine;
        unsigned flags_3d;
        if (argc != 7) return 2;
        value = (unsigned)parse_u32(argv[2]);
        volume = (unsigned)parse_u32(argv[3]);
        max_volume = (unsigned)parse_u32(argv[4]);
        old_fine = (unsigned)parse_u32(argv[5]);
        flags_3d = (unsigned)parse_u32(argv[6]);
        memset(mem + 0x9000u, 0, 0x42u);
        mem[0x9000u + 0x23u] = (unsigned char)old_fine;
        mem[0x9000u + 0x3du] = (unsigned char)flags_3d;
        abi_set_sub131ef_state((unsigned short)volume, (unsigned char)max_volume);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call sub_131EF
        }
        printf("data=%02x%02x%02x%02x%02x\n",
               mem[0x9000u + 0x22u],
               mem[0x9000u + 0x23u],
               mem[0x9000u + 0x36u],
               mem[0x9000u + 0x37u],
               mem[0x9000u + 0x3du]);
        return 0;
    }

    if (streq(op, "abisub13177")) {
        unsigned period;
        unsigned long dword_245bc;
        unsigned long dword_245c0;
        unsigned shift;
        unsigned flags_3d;
        if (argc != 7) return 2;
        period = (unsigned)parse_u32(argv[2]);
        dword_245bc = parse_u32(argv[3]);
        dword_245c0 = parse_u32(argv[4]);
        shift = (unsigned)parse_u32(argv[5]);
        flags_3d = (unsigned)parse_u32(argv[6]);
        memset(mem + 0x9000u, 0, 0x42u);
        mem[0x9000u + 0x3du] = (unsigned char)flags_3d;
        abi_set_sub13177_state(dword_245bc, dword_245c0, (unsigned char)shift);
        _asm {
            mov ax, period
            mov bx, offset mem
            add bx, 9000h
            call sub_13177
        }
        printf("data=%02x%02x%02x%02x%02x%02x%02x\n",
               mem[0x9000u + 0x1eu],
               mem[0x9000u + 0x1fu],
               mem[0x9000u + 0x20u],
               mem[0x9000u + 0x21u],
               mem[0x9000u + 0x3du],
               mem[0x9000u + 0x3eu],
               mem[0x9000u + 0x3fu]);
        return 0;
    }

    if (streq(op, "abimidi154da")) {
        unsigned value;
        unsigned ax_after;
        if (argc != 3) return 2;
        value = (unsigned)parse_u32(argv[2]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x18u] = (unsigned char)value;
        _asm {
            mov bx, offset mem
            add bx, 9000h
            call midi_154DA
            mov ax_after, ax
        }
        printf("ax=%04x\n", ax_after);
        return 0;
    }

    if (streq(op, "abimidi154de")) {
        unsigned packed;
        unsigned ax_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        packed = (unsigned)parse_u32(argv[2]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x35u] = (unsigned char)packed;
        _asm {
            mov bx, offset mem
            add bx, 9000h
            call midi_154DE
            mov ax_after, ax
            mov dx_after, dx
        }
        printf("ax=%04x dx=%04x\n", ax_after, dx_after);
        return 0;
    }

    if (streq(op, "abimidi154ac")) {
        unsigned value;
        unsigned max_volume;
        unsigned current_volume;
        unsigned ax_after;
        unsigned di_after;
        if (argc != 5) return 2;
        value = (unsigned)parse_u32(argv[2]);
        max_volume = (unsigned)parse_u32(argv[3]);
        current_volume = (unsigned)parse_u32(argv[4]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x1bu] = (unsigned char)current_volume;
        abi_set_midi154ac_state((unsigned char)max_volume);
        _asm {
            mov ax, value
            mov di, 1000h
            mov bx, offset mem
            add bx, 9000h
            call midi_154AC
            mov ax_after, ax
            mov di_after, di
        }
        printf("ax=%04x di=%04x data=2e\n", ax_after, di_after);
        return 0;
    }

    if (streq(op, "abimidi15413guard")) {
        unsigned value;
        unsigned short ax_in;
        unsigned ax_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        value = (unsigned)parse_u32(argv[2]);
        ax_in = (unsigned short)(((value & 0xffu) << 8) | 0x34u);
        abi_set_midi15413_state((unsigned char)value);
        _asm {
            mov ax, ax_in
            mov dx, 5678h
            call midi_15413
            mov ax_after, ax
            mov dx_after, dx
        }
        printf("ax=%04x dx=%04x data=%02x\n", ax_after, dx_after, abi_get_midi15413_last_status());
        return 0;
    }

    if (streq(op, "abimidiport")) {
        const char *symbol;
        unsigned base_port;
        unsigned ax_value;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 5) return 2;
        symbol = argv[2];
        base_port = (unsigned)parse_u32(argv[3]);
        ax_value = (unsigned)parse_u32(argv[4]);
        abi_set_midi_port_state((unsigned short)base_port, 0x55, 0xa0);
        if (streq(symbol, "midi_clean")) {
            _asm {
                mov ax, ax_value
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "midi_sndoff")) {
            _asm {
                mov ax, ax_value
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_sndoff
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "midi_153C0")) {
            _asm {
                mov ax, ax_value
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_153C0
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "midi_153D6")) {
            _asm {
                mov ax, ax_value
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_153D6
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "midi_153F1")) {
            _asm {
                mov ax, ax_value
                mov bx, 5678h
                mov cx, 0003h
                mov dx, 0def0h
                call midi_153F1
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "midi_15442")) {
            _asm {
                mov ax, ax_value
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
               abi_get_midi_port_last_status(),
               abi_get_midi_port_byte_24678());
        return 0;
    }

    if (streq(op, "abimidichannelport")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned last_status_after;
        unsigned byte_24678_after;
        if (argc != 3) return 2;
        symbol = argv[2];
        memset(mem + 0x9000u, 0, 0x42u);
        mem[0x9000u + 0x02u] = 0x05;
        mem[0x9000u + 0x03u] = 0x02;
        mem[0x9000u + 0x08u] = 0x20;
        mem[0x9000u + 0x17u] = streq(symbol, "midi_1544D") ? 0x83 : 0x00;
        mem[0x9000u + 0x18u] = 0x04;
        mem[0x9000u + 0x1bu] = 0x20;
        mem[0x9000u + 0x35u] = 0x31;
        abi_set_midi_port_state(0x0330u, 0x55, 0xa0);
        if (streq(symbol, "midi_1544D")) {
            _asm {
                mov ax, 1234h
                mov bx, offset mem
                add bx, 9000h
                mov cx, 9abch
                mov dx, 0def0h
                call midi_1544D
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "midi_15466")) {
            _asm {
                mov ax, 1234h
                mov bx, offset mem
                add bx, 9000h
                mov cx, 9abch
                mov dx, 0def0h
                call midi_15466
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        last_status_after = abi_get_midi_port_last_status();
        byte_24678_after = abi_get_midi_port_byte_24678();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               (unsigned)(0x9000u + (bx_after - ((unsigned)mem + 0x9000u))),
               cx_after,
               dx_after);
        print_bytes(mem + 0x9000u, 0x40);
        printf("%02x%02x\n", last_status_after, byte_24678_after);
        return 0;
    }

    if (streq(op, "abisub12d35disable")) {
        unsigned config_word;
        unsigned ax_after;
        unsigned bx_after;
        if (argc != 3) return 2;
        config_word = (unsigned)parse_u32(argv[2]);
        (void)config_word;
        abi_set_sub12d35_state(0xff);
        _asm {
            xor ax, ax
            xor bx, bx
            call sub_12D35
            mov ax_after, ax
            mov bx_after, bx
        }
        printf("ax=%04x bx=%04x data=%02x\n", ax_after, bx_after, abi_get_sub12d35_code_byte());
        return 0;
    }

    if (streq(op, "abisub12da8guard")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        abi_set_sub12da8_state();
        _asm {
            mov ax, 1603h
            mov bx, 7856h
            mov cx, 0907h
            mov dx, 0220h
            mov si, 0084h
            mov di, 1234h
            call sub_12DA8
        }
        data = abi_get_sub12da8_data();
        printf("data=");
        print_bytes(data, 14);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisettimerint")) {
        unsigned target;
        unsigned bx_after;
        unsigned dx_after;
        unsigned cs_after;
        unsigned char flags[4];
        if (argc != 3) return 2;
        target = (unsigned)parse_u32(argv[2]);
        _asm {
            mov dx, target
            call set_timer_int
            mov ax, 8
            call getint_vect
            mov bx_after, bx
            mov dx_after, dx
            mov ax, cs
            mov cs_after, ax
        }
        flags[0] = 1;
        flags[1] = 1;
        flags[2] = (unsigned char)(bx_after != target);
        flags[3] = (unsigned char)(dx_after != cs_after);
        printf("data=");
        print_bytes(flags, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisndvector")) {
        unsigned irq;
        unsigned old_off;
        unsigned old_seg;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned si_after;
        const unsigned char *data;
        if (argc != 5) return 2;
        irq = (unsigned)parse_u32(argv[2]);
        old_off = (unsigned)parse_u32(argv[3]);
        old_seg = (unsigned)parse_u32(argv[4]);
        abi_set_sndvector_state((unsigned short)old_off, (unsigned short)old_seg);
        _asm {
            mov ax, irq
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            mov si, 4321h
            call setsnd_handler
            call restore_intvector
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov si_after, si
        }
        data = abi_get_sndvector_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               si_after);
        print_bytes(data, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisb16probe")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        abi_set_sb16_probe_state();
        if (streq(symbol, "sb16_detect_port")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call sb16_detect_port
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "sb16_sound_on")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call sb16_sound_on
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        data = abi_get_sb16_probe_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(data, 10);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisb16initfail")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call sb16_init
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        data = abi_get_sb16_init_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(data, 15);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisb16int")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 2) return 2;
        abi_set_sb16_int_state(5);
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call sb16_handler_int
            mov bx, 802ah
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x data=%02x\n",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               0x156a,
               abi_get_sb16_int_counter());
        return 0;
    }

    if (streq(op, "abisb16dmafail")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call sb16_18540
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        data = abi_get_sb16_dma_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(data, 10);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiinrread119b7")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned di_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            mov di, 0bf68h
            call inr_read_119B7
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov di_after, di
        }
        data = abi_get_inr_read_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               di_after);
        print_bytes(data, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimodread12247eof")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned si_after;
        unsigned di_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 0000h
            mov cx, 0010h
            mov dx, 0ffffh
            mov si, 2222h
            mov di, 3333h
            call mod_readfile_12247
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov si_after, si
            mov di_after, di
        }
        data = abi_get_modread12247_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               si_after,
               di_after);
        print_bytes(data, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimodread11f4eguard")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            push ax
            push bx
            push cx
            push dx
            call mod_readfile_11F4E
            pop dx
            pop cx
            pop bx
            pop ax
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        data = abi_get_modread11f4e_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(data, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisb16off")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "sb16_sound_off")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call sb16_sound_off
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "sb16_off")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call sb16_off
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "sb16_deinit")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call sb16_deinit
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "sb_clean")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call sb_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "sb_sndoff")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call sb_sndoff
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "sbpro_clean")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call sbpro_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "sbpro_sndoff")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call sbpro_sndoff
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        data = abi_get_sb16off_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(data, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abicleandeinit")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "clean_int8_mem_timr")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call clean_int8_mem_timr
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "covox_deinit")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call covox_deinit
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "stereo_deinit")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call stereo_deinit
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "adlib_clean")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call adlib_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "pcspeaker_clean")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call pcspeaker_clean
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else {
            return 2;
        }
        data = abi_get_cleandeinit_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(data, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abidosdir")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned si_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "dosgetcurdir")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                mov si, 2800h
                call dosgetcurdir
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
                mov si_after, si
            }
        } else if (streq(symbol, "doschdir")) {
            _asm {
                mov ax, 1234h
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                mov si, 2800h
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
        data = abi_get_dosdir_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               si_after);
        print_bytes(data, 70);
        printf("\n");
        return 0;
    }

    if (streq(op, "abidosfindnext")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call dosfindnext
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        data = abi_get_dosfindnext_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(data, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abidosfread")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 0010h
            mov dx, 0bf68h
            call dosfread
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        data = abi_get_dosfread_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(data, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "abidosseeksuccess")) {
        unsigned ax_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            xor ax, ax
            xor dx, dx
            xor cx, cx
            mov dx, 0bf68h
            call dosseek
            mov ax_after, ax
            mov cx_after, cx
            mov dx_after, dx
        }
        data = abi_get_dosseek_data();
        printf("ax=%04x cx=%04x dx=%04x data=",
               ax_after,
               cx_after,
               dx_after);
        print_bytes(data, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiinrread118b0fail")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 2) return 2;
        _asm {
            mov dx, 0002h
            call inr_read_118B0
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x\n",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               0x156a);
        return 0;
    }

    if (streq(op, "abiread2buffer")) {
        unsigned si_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            mov si, 1111h
            mov di, 2222h
            call read2buffer
            mov si_after, si
        }
        data = abi_get_read2buffer_data();
        printf("si=%04x data=", si_after);
        print_bytes(data, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimemlimit")) {
        const char *symbol;
        unsigned long size;
        unsigned ax_after;
        unsigned bx_after;
        if (argc != 4) return 2;
        symbol = argv[2];
        size = parse_u32(argv[3]);
        if (streq(symbol, "memalloc")) {
            _asm {
                mov ax, 2345h
                mov ebx, size
                call memalloc
                mov ax_after, ax
                mov bx_after, bx
            }
        } else if (streq(symbol, "memrealloc")) {
            _asm {
                mov ax, 2345h
                mov ebx, size
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

    if (streq(op, "abiallocdmafail")) {
        unsigned long size;
        unsigned channel;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        const unsigned char *data;
        if (argc != 4) return 2;
        size = parse_u32(argv[2]);
        channel = (unsigned)parse_u32(argv[3]);
        _asm {
            mov eax, size
            mov cx, channel
            call alloc_dma_buf
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
        }
        data = abi_get_allocdma_data();
        printf("ax=%04x bx=%04x cx=%04x data=",
               ax_after,
               bx_after,
               cx_after);
        print_bytes(data, 0x19);
        printf("\n");
        return 0;
    }

    if (streq(op, "abigravisdma")) {
        const char *symbol;
        unsigned ax_after;
        unsigned cx_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "sub_182DB")) {
            _asm {
                mov ax, 0100h
                mov cx, 0020h
                call sub_182DB
                mov ax_after, ax
                mov cx_after, cx
            }
        } else if (streq(symbol, "nongravis_dma")) {
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
        data = abi_get_gravisdma_data();
        printf("ax=%04x cx=%04x data=", ax_after, cx_after);
        print_bytes(data, 11);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub1279dma")) {
        unsigned ax_after;
        unsigned cx_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov si, 2800h
            mov eax, 12345000h
            call sub_1279A
            mov ax_after, ax
            mov cx_after, cx
        }
        data = abi_get_sub1279_data();
        printf("ax=%04x cx=%04x data=", ax_after, cx_after);
        print_bytes(data, 9);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiprogramdma")) {
        unsigned ax_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov cx, 0001h
            call program_dma
            mov ax_after, ax
            mov cx_after, cx
            mov dx_after, dx
        }
        data = abi_get_programdma_data();
        printf("ax=%04x cx=%04x dx=%04x data=", ax_after, cx_after, dx_after);
        print_bytes(data, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimemreallocx")) {
        unsigned size;
        unsigned di_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        size = (unsigned)parse_u32(argv[2]);
        _asm {
            mov di, size
            call mem_reallocx
            mov di_after, di
        }
        data = abi_get_memreallocx_data();
        printf("di=%04x data=", di_after);
        print_bytes(data, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "abideinit125b9idle")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            push cs
            call deinit_125B9
        }
        data = abi_get_deinit125b9_data();
        printf("ds=%04x data=", 0x156a);
        print_bytes(data, 11);
        printf("\n");
        return 0;
    }

    if (streq(op, "abirtcclock")) {
        const char *symbol;
        unsigned ax_after;
        unsigned dx_after;
        unsigned es_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "initclockfromrtc")) {
            _asm {
                call initclockfromrtc
                mov ax_after, ax
                mov dx_after, dx
                mov es_after, es
            }
        } else if (streq(symbol, "rereadrtc_settmr")) {
            _asm {
                call rereadrtc_settmr
                mov ax_after, ax
                mov dx_after, dx
                mov es_after, es
            }
        } else {
            return 2;
        }
        data = abi_get_rtcclock_data();
        printf("ax=%04x dx=%04x es=%04x data=", ax_after, dx_after, es_after);
        print_bytes(data, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiloadcfgsuccess")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call loadcfg
        }
        data = abi_get_loadcfg_data();
        printf("ds=%04x data=", 0x0d8f);
        print_bytes(data, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "abidosexecnocomspec")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call dosexec
        }
        data = abi_get_dosexec_data();
        printf("ds=%04x data=", 0x0a15);
        print_bytes(data, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abicallsubxfail")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call callsubx
        }
        data = abi_get_callsubx_data();
        printf("data=");
        print_bytes(data, 17);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimemalloc12kbounded")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned di_after;
        unsigned es_after;
        if (argc != 2) return 2;
        _asm {
            call memalloc12k
            mov ax_after, ax
            mov bx_after, bx
            mov di_after, di
            mov es_after, es
        }
        printf("ax=%04x bx=%04x di=%04x es=%04x\n",
               ax_after,
               bx_after,
               di_after,
               es_after);
        return 0;
    }

    if (streq(op, "abigraphsetup")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (!streq(symbol, "f2_waves")) return 2;
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
        data = abi_get_f2waves_data();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(data, 9);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiinitvgabounded")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call init_vga_waves
        }
        data = abi_get_initvga_data();
        printf("data=");
        print_bytes(data, 5);
        printf("\n");
        return 0;
    }

    if (streq(op, "abif2drawbounded")) {
        const char *symbol;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "f2_draw_waves")) {
            _asm {
                call f2_draw_waves
            }
        } else if (streq(symbol, "f2_draw_waves2")) {
            _asm {
                call f2_draw_waves2
            }
        } else {
            return 2;
        }
        data = abi_get_f2draw_data();
        printf("data=");
        print_bytes(data, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abireadallmoulesbounded")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call readallmoules
        }
        data = abi_get_readallmoules_data();
        printf("flags=%04x data=", 0x7246);
        print_bytes(data, 3);
        printf("\n");
        return 0;
    }

    if (streq(op, "abireadmodulefail")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call read_module
        }
        data = abi_get_readmodule_data();
        printf("data=");
        print_bytes(data, 19);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimodulereadfail")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            push cs
            call moduleread
        }
        data = abi_get_moduleread_data();
        printf("data=");
        print_bytes(data, 7);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimodread10311bounded")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call mod_read_10311
        }
        data = abi_get_modread10311_data();
        printf("data=");
        print_bytes(data, 64);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimodntbounded")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call mod_n_t_module
        }
        data = abi_get_modnt_data();
        printf("data=");
        print_bytes(data, 10);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiformatloaderheader")) {
        const char *symbol;
        const unsigned char *data;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "_2stm_module")) {
            _asm { call _2stm_module }
        } else if (streq(symbol, "e669_module")) {
            _asm { call e669_module }
        } else if (streq(symbol, "mtm_module")) {
            _asm { call mtm_module }
        } else if (streq(symbol, "psm_module")) {
            _asm { call psm_module }
        } else if (streq(symbol, "far_module")) {
            _asm { call far_module }
        } else if (streq(symbol, "ult_module")) {
            _asm { call ult_module }
        } else if (streq(symbol, "s3m_module")) {
            _asm { call s3m_module }
        } else if (streq(symbol, "inr_module")) {
            _asm { call inr_module }
        } else {
            return 2;
        }
        data = abi_get_formatloader_data();
        printf("data=");
        print_bytes(data, 20);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimodulessearchbounded")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call modules_search
        }
        data = abi_get_modulessearch_data();
        printf("data=");
        print_bytes(data, 6);
        printf("\n");
        return 0;
    }

    if (streq(op, "abistartbounded")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call start
        }
        data = abi_get_start_data();
        printf("ds=%04x data=", 0x0d8f);
        print_bytes(data, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abikeybbounded")) {
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            call keyb_19EFD
        }
        data = abi_get_keyb19efd_data();
        printf("data=");
        print_bytes(data, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abispectr1bce9equal")) {
        unsigned value;
        unsigned bx_after;
        unsigned bp_after;
        const unsigned char *data;
        if (argc != 3) return 2;
        value = (unsigned)parse_u32(argv[2]) & 0xffu;
        (void)value;
        _asm {
            mov bx, 2800h
            push bp
            mov bp, 3800h
            call spectr_1BCE9
            mov ax, bx
            mov dx, bp
            pop bp
            mov bx_after, ax
            mov bp_after, dx
        }
        data = abi_get_spectr1bce9_data();
        printf("bx=%04x bp=%04x data=", bx_after, bp_after);
        print_bytes(data, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "abispectr1bc2dequal")) {
        unsigned bx_after;
        unsigned bp_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov bx, 2800h
            push bp
            mov bp, 3800h
            mov cx, 1234h
            call spectr_1BC2D
            mov ax, bx
            mov dx, bp
            pop bp
            mov bx_after, ax
            mov bp_after, dx
        }
        data = abi_get_spectr1bc2d_data();
        printf("bx=%04x bp=%04x data=", bx_after, bp_after);
        print_bytes(data, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "abispectr1bbc1zero")) {
        unsigned si_after;
        unsigned di_after;
        unsigned cx_after;
        const unsigned char *data;
        if (argc != 2) return 2;
        _asm {
            mov si, 2800h
            mov di, 2900h
            mov cx, 0001h
            call spectr_1BBC1
            mov si_after, si
            mov di_after, di
            mov cx_after, cx
        }
        data = abi_get_spectr1bbc1_data();
        printf("si=%04x di=%04x cx=%04x data=", si_after, di_after, cx_after);
        print_bytes(data, 3);
        printf("\n");
        return 0;
    }

    if (streq(op, "abivideoprp")) {
        unsigned first;
        unsigned second;
        unsigned third;
        const unsigned char *data;
        if (argc != 5) return 2;
        first = (unsigned)strtoul(argv[2], 0, 16) & 0xffu;
        second = (unsigned)strtoul(argv[3], 0, 16) & 0xffu;
        third = (unsigned)strtoul(argv[4], 0, 16) & 0xffu;
        abi_set_videoprp_inputs((unsigned char)first, (unsigned char)second, (unsigned char)third);
        _asm {
            call video_prp_mtr_positn
        }
        data = abi_get_videoprp_data();
        printf("data=");
        print_bytes(data, 9);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisetvideomode")) {
        unsigned mode;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        mode = (unsigned)parse_u32(argv[2]) & 0xffu;
        mem[0x1680u] = (db)mode;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call setvideomode
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x\n",
               ax_after, bx_after, cx_after, dx_after, mem[0x1680u]);
        return 0;
    }

    if (streq(op, "abihex1be39")) {
        unsigned value;
        unsigned attr;
        unsigned ax_after;
        unsigned di_after;
        if (argc != 4) return 2;
        value = (unsigned)parse_u32(argv[2]) & 0xffu;
        attr = (unsigned)parse_u32(argv[3]) & 0xffu;
        _asm {
            push ds
            pop es
            mov ax, attr
            shl ax, 8
            or ax, value
            mov di, offset mem
            add di, 9100h
            call hex_1BE39
            mov ax_after, ax
            mov di_after, di
        }
        di_after = (unsigned)(0x2800u + (di_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("ax=%04x di=%04x data=", ax_after, di_after);
        print_bytes(mem + ORIG_DST_OFF, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimouse1c7a9")) {
        unsigned x;
        unsigned y;
        unsigned left;
        unsigned top;
        unsigned right;
        unsigned bottom;
        unsigned ax_after;
        unsigned bp_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned si_after;
        unsigned di_after;
        if (argc != 8) return 2;
        x = (unsigned)parse_u32(argv[2]);
        y = (unsigned)parse_u32(argv[3]);
        left = (unsigned)parse_u32(argv[4]);
        top = (unsigned)parse_u32(argv[5]);
        right = (unsigned)parse_u32(argv[6]);
        bottom = (unsigned)parse_u32(argv[7]);
        _asm {
            mov ax, x
            mov cx, left
            mov dx, top
            mov si, right
            mov di, bottom
            push bp
            mov bp, y
            call mouse_1C7A9
            mov bx, bp
            pop bp
            mov ax_after, ax
            mov bp_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov si_after, si
            mov di_after, di
        }
        printf("ax=%04x bp=%04x cx=%04x dx=%04x si=%04x di=%04x\n",
               ax_after, bp_after, cx_after, dx_after, si_after, di_after);
        return 0;
    }

    if (streq(op, "abimouse1c7cf")) {
        unsigned x;
        unsigned y;
        size_t count;
        unsigned ax_after;
        unsigned bp_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned si_after;
        unsigned di_after;
        if (argc != 5) return 2;
        x = (unsigned)parse_u32(argv[2]);
        y = (unsigned)parse_u32(argv[3]);
        count = parse_hex_bytes(argv[4], mem, 0x400u);
        abi_set_mouse1c7cf_records(mem, (unsigned short)count);
        abi_set_mouse1c7cf_inputs((unsigned short)x, (unsigned short)y, 0x2800u);
        _asm {
            call mouse_1C7CF
        }
        ax_after = abi_get_mouse1c7cf_ax();
        bx_after = abi_get_mouse1c7cf_bx();
        bp_after = abi_get_mouse1c7cf_bp();
        cx_after = abi_get_mouse1c7cf_cx();
        dx_after = abi_get_mouse1c7cf_dx();
        si_after = abi_get_mouse1c7cf_si();
        di_after = abi_get_mouse1c7cf_di();
        printf("ax=%04x bx=%04x bp=%04x cx=%04x dx=%04x si=%04x di=%04x\n",
               ax_after, bx_after, bp_after, cx_after, dx_after, si_after, di_after);
        return 0;
    }

    if (streq(op, "abiint24")) {
        unsigned ah_value;
        unsigned ax_after;
        if (argc != 3) return 2;
        ah_value = (unsigned)parse_u32(argv[2]) & 0xffu;
        _asm {
            mov ax, ah_value
            shl ax, 8
            call int24
            mov ax_after, ax
        }
        printf("ax=%04x\n", ax_after);
        return 0;
    }

    if (streq(op, "abiemsrestore")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 4) return 2;
        (void)parse_u32(argv[2]);
        (void)parse_u32(argv[3]);
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

    if (streq(op, "abiemsinit")) {
        unsigned config_word;
        unsigned ax_after;
        if (argc != 3) return 2;
        config_word = (unsigned)parse_u32(argv[2]);
        (void)config_word;
        _asm {
            call ems_init
            mov ax_after, ax
        }
        printf("ax=%04x ems=%02x\n", ax_after, abi_get_ems_enabled_byte());
        return 0;
    }

    if (streq(op, "abiemsguard")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        symbol = argv[2];
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
        }
        if (!(streq(symbol, "ems_release") ||
              streq(symbol, "ems_realloc") ||
              streq(symbol, "ems_deinit") ||
              streq(symbol, "ems_save_mapctx") ||
              streq(symbol, "ems_mapmem") ||
              streq(symbol, "ems_mapmem2"))) {
            return 2;
        }
        ax_after = 0x1234u;
        bx_after = 0x5678u;
        cx_after = 0x9abcu;
        dx_after = 0xdef0u;
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(op, "abiemsmapcopy")) {
        db payload[16];
        unsigned i;
        const unsigned char *data;
        if (argc != 3) return 2;
        for (i = 0; i < sizeof(payload); ++i) payload[i] = (db)(0x31u + i);
        abi_set_emsmapcopy_source(payload);
        if (streq(argv[2], "ems_mapmemx")) {
            _asm { call ems_mapmemx }
        } else if (streq(argv[2], "ems_mapmemy")) {
            _asm { call ems_mapmemy }
        } else {
            return 2;
        }
        data = abi_get_emsmapcopy_data();
        printf("data=");
        print_bytes(data, sizeof(payload));
        printf("\n");
        return 0;
    }

    if (streq(op, "abiemsrealloc2limit")) {
        unsigned initial_count;
        unsigned long requested_size;
        unsigned ax_after;
        unsigned cx_after;
        if (argc != 4) return 2;
        initial_count = (unsigned)parse_u32(argv[2]);
        requested_size = parse_u32(argv[3]);
        abi_set_emsrealloc2_state((unsigned char)initial_count, requested_size);
        _asm {
            call ems_realloc2
            mov ax_after, ax
            mov cx_after, cx
        }
        printf("ax=%04x cx=%04x data=%02x\n", ax_after, cx_after, abi_get_emsrealloc2_count());
        return 0;
    }

    if (streq(op, "abiclean11c43")) {
        unsigned flag;
        unsigned byte_2461e;
        unsigned byte_2461f;
        const unsigned char *data;
        if (argc != 5) return 2;
        flag = (unsigned)parse_u32(argv[2]);
        byte_2461e = (unsigned)parse_u32(argv[3]);
        byte_2461f = (unsigned)parse_u32(argv[4]);
        abi_set_clean11c43_state((unsigned char)flag, (unsigned char)byte_2461e, (unsigned char)byte_2461f);
        _asm {
            call clean_11C43
        }
        data = abi_get_clean11c43_data();
        printf("data=");
        print_bytes(data, 57);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimodsubdelta")) {
        db flag;
        db reset;
        db previous;
        size_t len;
        unsigned si_after;
        unsigned cx_after;
        unsigned old_flag;
        unsigned old_reset;
        unsigned old_previous;
        if (argc < 6) return 2;
        flag = (db)parse_u32(argv[2]);
        reset = (db)parse_u32(argv[3]);
        previous = (db)parse_u32(argv[4]);
        len = strlen(argv[5]);
        memcpy(mem + 0x2800u, argv[5], len);
        _asm {
            mov al, byte ptr ds:00d4h
            mov old_flag, ax
            mov al, byte ptr ds:00d5h
            mov old_reset, ax
            mov al, byte ptr ds:00d6h
            mov old_previous, ax
            mov al, flag
            mov byte ptr ds:00d4h, al
            mov al, reset
            mov byte ptr ds:00d5h, al
            mov al, previous
            mov byte ptr ds:00d6h, al
            mov si, offset mem
            add si, 2800h
            mov cx, len
            call mod_sub_delta
            mov si_after, si
            mov cx_after, cx
            mov ax, old_flag
            mov byte ptr ds:00d4h, al
            mov ax, old_reset
            mov byte ptr ds:00d5h, al
            mov ax, old_previous
            mov byte ptr ds:00d6h, al
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + 0x2800u)));
        printf("si=%04x cx=%04x data=", si_after, cx_after);
        print_bytes(mem + 0x2800u, len);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub11ba6")) {
        unsigned ch;
        unsigned cl;
        unsigned bx;
        unsigned dx;
        unsigned current_max;
        unsigned short old_current_max;
        unsigned short di_reg_after;
        unsigned di_after;
        unsigned short ch16;
        unsigned short cl16;
        unsigned short bx16;
        unsigned short dx16;
        unsigned char current_max8;
        if (argc != 7) return 2;
        ch = (unsigned)parse_u32(argv[2]);
        cl = (unsigned)parse_u32(argv[3]);
        bx = (unsigned)parse_u32(argv[4]);
        dx = (unsigned)parse_u32(argv[5]);
        current_max = (unsigned)parse_u32(argv[6]);
        ch16 = (unsigned short)ch;
        cl16 = (unsigned short)cl;
        bx16 = (unsigned short)bx;
        dx16 = (unsigned short)dx;
        current_max8 = (unsigned char)current_max;
        memset(mem + 0x2800u, 0x2e, 8);
        memset(mem + ORIG_DST_OFF, 0x2e, 8);
        _asm {
            mov al, byte ptr ds:007bh
            mov old_current_max, ax
            mov al, current_max8
            mov byte ptr ds:007bh, al
            mov cx, ch16
            shl cx, 8
            or cx, cl16
            mov bx, bx16
            mov dx, dx16
            mov di, offset mem
            add di, 2800h
            call sub_11BA6
            mov di_reg_after, di
            mov ax, old_current_max
            mov byte ptr ds:007bh, al
        }
        di_after = (unsigned)(0x2800u + (di_reg_after - ((unsigned)mem + 0x2800u)));
        printf("di=%04x data=", di_after);
        print_bytes(mem + ORIG_DST_OFF, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimod102f5")) {
        char hexbuf[300];
        const char *hex;
        FILE *fp;
        size_t count;
        unsigned i;
        unsigned ax_after;
        if (argc != 3) return 2;
        hex = argv[2];
        if (hex[0] == '@') {
            fp = fopen(hex + 1, "r");
            if (fp == 0) return 2;
            count = fread(hexbuf, 1, sizeof(hexbuf) - 1u, fp);
            fclose(fp);
            hexbuf[count] = 0;
            hex = hexbuf;
        }
        memset(mem, 0, 128);
        count = parse_hex_bytes(hex, mem, 128);
        (void)count;
        for (i = 0; i < 128u; ++i) {
            ((db *)0x3a48u)[i] = mem[i];
        }
        _asm {
            call mod_102F5
            mov ax_after, ax
        }
        (void)ax_after;
        printf("data=%02x%02x\n", (unsigned)((db *)0x0052u)[0], (unsigned)((db *)0x0052u)[1]);
        return 0;
    }

    if (streq(op, "abiultreadfast")) {
        unsigned word_value;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        db old_data[6];
        db data[6];
        unsigned i;
        if (argc != 3) return 2;
        word_value = (unsigned)parse_u32(argv[2]);
        for (i = 0; i < 6u; ++i) old_data[i] = ((db *)0xc09bu)[i];
        ((db *)0xc09bu)[0] = (db)word_value;
        ((db *)0xc09bu)[1] = (db)(word_value >> 8);
        ((db *)0xc09bu)[2] = 0xa5;
        ((db *)0xc09bu)[3] = 0xa5;
        ((db *)0xc09bu)[4] = 0xa5;
        ((db *)0xc09bu)[5] = 0xa5;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call ult_read
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        for (i = 0; i < 6u; ++i) data[i] = ((db *)0xc09bu)[i];
        for (i = 0; i < 6u; ++i) ((db *)0xc09bu)[i] = old_data[i];
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=", ax_after, bx_after, cx_after, dx_after);
        print_bytes(data, 6);
        printf("\n");
        return 0;
    }

    if (streq(op, "abivolumeprepinactive")) {
        unsigned word_value;
        unsigned size;
        unsigned cx_after;
        unsigned di_after;
        db old_globals[4];
        db data[12];
        unsigned i;
        if (argc != 4) return 2;
        word_value = (unsigned)parse_u32(argv[2]);
        size = (unsigned)parse_u32(argv[3]);
        for (i = 0; i < 4u; ++i) old_globals[i] = ((db *)0x0070u)[i];
        memset(mem + 0x2900u, 0xa5, size);
        _asm {
            push es
            mov ax, ds
            mov es, ax
            mov ax, word_value
            mov cx, size
            mov di, offset mem
            add di, 2900h
            call volume_prep
            mov cx_after, cx
            mov di_after, di
            pop es
        }
        for (i = 0; i < 4u; ++i) data[i] = ((db *)0x0070u)[i];
        for (i = 0; i < 8u; ++i) data[4u + i] = mem[0x2900u + i];
        for (i = 0; i < 4u; ++i) ((db *)0x0070u)[i] = old_globals[i];
        printf("cx=%04x di=%04x data=", cx_after, di_after);
        print_bytes(data, 12);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub135ca")) {
        db old_data[6];
        db data[6];
        unsigned i;
        if (argc != 2) return 2;
        old_data[0] = ((db *)0x0014u)[0];
        old_data[1] = ((db *)0x0014u)[1];
        old_data[2] = ((db *)0x1372u)[0];
        old_data[3] = ((db *)0x1372u)[1];
        old_data[4] = ((db *)0x137fu)[0];
        old_data[5] = ((db *)0x13a5u)[0];
        ((db *)0x0014u)[0] = 0x00;
        ((db *)0x0014u)[1] = 0x28;
        ((db *)0x2800u)[0] = 0x00;
        ((db *)0x1372u)[0] = 0xef;
        ((db *)0x1372u)[1] = 0xbe;
        ((db *)0x137fu)[0] = 0x00;
        ((db *)0x13a5u)[0] = 0xaa;
        _asm {
            call sub_135CA
        }
        data[0] = ((db *)0x0014u)[0];
        data[1] = ((db *)0x0014u)[1];
        data[2] = ((db *)0x1372u)[0];
        data[3] = ((db *)0x1372u)[1];
        data[4] = ((db *)0x137fu)[0];
        data[5] = ((db *)0x13a5u)[0];
        ((db *)0x0014u)[0] = old_data[0];
        ((db *)0x0014u)[1] = old_data[1];
        ((db *)0x1372u)[0] = old_data[2];
        ((db *)0x1372u)[1] = old_data[3];
        ((db *)0x137fu)[0] = old_data[4];
        ((db *)0x13a5u)[0] = old_data[5];
        printf("data=");
        print_bytes(data, 6);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimemfree125da")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
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

    if (streq(op, "abimousegetpos")) {
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        db old_data[7];
        db data[7];
        unsigned i;
        if (argc != 2) return 2;
        for (i = 0; i < 7u; ++i) old_data[i] = ((db *)0x169cu)[i];
        ((db *)0x169cu)[0] = 0xaa;
        ((db *)0x169cu)[1] = 0xaa;
        ((db *)0x169cu)[2] = 0xbb;
        ((db *)0x169cu)[3] = 0xbb;
        ((db *)0x169cu)[4] = 0xcc;
        ((db *)0x169cu)[5] = 0x00;
        ((db *)0x169cu)[6] = 0x01;
        _asm {
            mov bx, 1111h
            mov cx, 2222h
            mov dx, 3333h
            call mouse_getpos
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        for (i = 0; i < 7u; ++i) data[i] = ((db *)0x169cu)[i];
        for (i = 0; i < 7u; ++i) ((db *)0x169cu)[i] = old_data[i];
        printf("bx=%04x cx=%04x dx=%04x data=", bx_after, cx_after, dx_after);
        print_bytes(data, 7);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimousecursor")) {
        const char *symbol;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        db old_data[7];
        db data[7];
        unsigned i;
        if (argc != 3) return 2;
        symbol = argv[2];
        for (i = 0; i < 7u; ++i) old_data[i] = ((db *)0x169cu)[i];
        ((db *)0x169cu)[0] = 0xaa;
        ((db *)0x169cu)[1] = 0xaa;
        ((db *)0x169cu)[2] = 0xbb;
        ((db *)0x169cu)[3] = 0xbb;
        ((db *)0x169cu)[4] = 0xcc;
        ((db *)0x169cu)[5] = 0x00;
        ((db *)0x169cu)[6] = streq(symbol, "mouse_showcur") ? 0x00 : 0x01;
        if (streq(symbol, "mouse_showcur")) {
            _asm {
                mov bx, 1111h
                mov cx, 2222h
                mov dx, 3333h
                call mouse_showcur
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "mouse_hide2")) {
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
        for (i = 0; i < 7u; ++i) data[i] = ((db *)0x169cu)[i];
        for (i = 0; i < 7u; ++i) ((db *)0x169cu)[i] = old_data[i];
        printf("bx=%04x cx=%04x dx=%04x data=", bx_after, cx_after, dx_after);
        print_bytes(data, 7);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimousewrapper")) {
        const char *symbol;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        db old_data[7];
        db data[7];
        unsigned i;
        if (argc != 3) return 2;
        symbol = argv[2];
        for (i = 0; i < 7u; ++i) old_data[i] = ((db *)0x169cu)[i];
        ((db *)0x169cu)[0] = 0xaa;
        ((db *)0x169cu)[1] = 0xaa;
        ((db *)0x169cu)[2] = 0xbb;
        ((db *)0x169cu)[3] = 0xbb;
        ((db *)0x169cu)[4] = 0xcc;
        ((db *)0x169cu)[5] = 0x00;
        ((db *)0x169cu)[6] = streq(symbol, "mouse_show") ? 0x00 : 0x01;
        if (streq(symbol, "mouse_show")) {
            _asm {
                mov bx, 1111h
                mov cx, 2222h
                mov dx, 3333h
                call mouse_show
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "mouse_hide")) {
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
        for (i = 0; i < 7u; ++i) data[i] = ((db *)0x169cu)[i];
        for (i = 0; i < 7u; ++i) ((db *)0x169cu)[i] = old_data[i];
        printf("bx=%04x cx=%04x dx=%04x data=", bx_after, cx_after, dx_after);
        print_bytes(data, 7);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimousedeinit")) {
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        db old_data[7];
        db data[7];
        unsigned i;
        if (argc != 2) return 2;
        for (i = 0; i < 7u; ++i) old_data[i] = ((db *)0x169cu)[i];
        ((db *)0x169cu)[0] = 0xaa;
        ((db *)0x169cu)[1] = 0xaa;
        ((db *)0x169cu)[2] = 0xbb;
        ((db *)0x169cu)[3] = 0xbb;
        ((db *)0x169cu)[4] = 0xcc;
        ((db *)0x169cu)[5] = 0x00;
        ((db *)0x169cu)[6] = 0x01;
        _asm {
            mov bx, 1111h
            mov cx, 2222h
            mov dx, 3333h
            call mouse_deinit
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        for (i = 0; i < 7u; ++i) data[i] = ((db *)0x169cu)[i];
        for (i = 0; i < 7u; ++i) ((db *)0x169cu)[i] = old_data[i];
        printf("bx=%04x cx=%04x dx=%04x data=", bx_after, cx_after, dx_after);
        print_bytes(data, 7);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimouseinit")) {
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        db old_data[7];
        db data[7];
        unsigned i;
        if (argc != 2) return 2;
        for (i = 0; i < 7u; ++i) old_data[i] = ((db *)0x169cu)[i];
        ((db *)0x169cu)[0] = 0xaa;
        ((db *)0x169cu)[1] = 0xaa;
        ((db *)0x169cu)[2] = 0xbb;
        ((db *)0x169cu)[3] = 0xbb;
        ((db *)0x169cu)[4] = 0xcc;
        ((db *)0x169cu)[5] = 0x01;
        ((db *)0x169cu)[6] = 0x01;
        _asm {
            mov bx, 1111h
            mov cx, 2222h
            mov dx, 3333h
            call mouse_init
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        for (i = 0; i < 7u; ++i) data[i] = ((db *)0x169cu)[i];
        for (i = 0; i < 7u; ++i) ((db *)0x169cu)[i] = old_data[i];
        printf("bx=%04x cx=%04x dx=%04x data=", bx_after, cx_after, dx_after);
        print_bytes(data, 7);
        printf("\n");
        return 0;
    }

    if (streq(op, "abigetcomspec")) {
        unsigned di_after;
        unsigned ds_value;
        db old_env[16];
        db old_2c[2];
        db old_164a[2];
        unsigned i;
        if (argc != 2) return 2;
        for (i = 0; i < 16u; ++i) old_env[i] = ((db *)0x0000u)[i];
        old_2c[0] = ((db *)0x002cu)[0];
        old_2c[1] = ((db *)0x002cu)[1];
        old_164a[0] = ((db *)0x164au)[0];
        old_164a[1] = ((db *)0x164au)[1];
        _asm {
            mov ds_value, ds
        }
        memcpy((void *)0x0000u, "COMSPEC=X", 10);
        ((db *)0x000au)[0] = 0;
        ((db *)0x000bu)[0] = 0;
        ((db *)0x002cu)[0] = (db)ds_value;
        ((db *)0x002cu)[1] = (db)(ds_value >> 8);
        ((db *)0x164au)[0] = (db)ds_value;
        ((db *)0x164au)[1] = (db)(ds_value >> 8);
        _asm {
            mov di, 0aaaah
            call get_comspec
            mov di_after, di
        }
        for (i = 0; i < 16u; ++i) ((db *)0x0000u)[i] = old_env[i];
        ((db *)0x002cu)[0] = old_2c[0];
        ((db *)0x002cu)[1] = old_2c[1];
        ((db *)0x164au)[0] = old_164a[0];
        ((db *)0x164au)[1] = old_164a[1];
        printf("di=%04x\n", di_after);
        return 0;
    }

    if (streq(op, "abigetexename")) {
        static const db env[] = {'A', '=', 'B', 0, 0, 1, 0, 'C', ':', '\\', 'I', 'P', 'L', 'A', 'Y', '.', 'E', 'X', 'E', 0};
        unsigned si_after;
        unsigned ds_value;
        db old_env[32];
        db old_dst[32];
        db old_2c[2];
        db old_164a[2];
        unsigned i;
        if (argc != 2) return 2;
        for (i = 0; i < 32u; ++i) old_env[i] = ((db *)0x0000u)[i];
        for (i = 0; i < 32u; ++i) old_dst[i] = ((db *)0x2800u)[i];
        old_2c[0] = ((db *)0x002cu)[0];
        old_2c[1] = ((db *)0x002cu)[1];
        old_164a[0] = ((db *)0x164au)[0];
        old_164a[1] = ((db *)0x164au)[1];
        _asm {
            mov ds_value, ds
        }
        memcpy((void *)0x0000u, env, sizeof(env));
        memset((void *)0x2800u, 0x2e, 32);
        ((db *)0x002cu)[0] = (db)ds_value;
        ((db *)0x002cu)[1] = (db)(ds_value >> 8);
        ((db *)0x164au)[0] = (db)ds_value;
        ((db *)0x164au)[1] = (db)(ds_value >> 8);
        _asm {
            mov si, 2800h
            call getexename
            mov si_after, si
        }
        printf("si=%04x data=", si_after);
        print_bytes((db *)0x2800u, 13);
        printf("\n");
        for (i = 0; i < 32u; ++i) ((db *)0x0000u)[i] = old_env[i];
        for (i = 0; i < 32u; ++i) ((db *)0x2800u)[i] = old_dst[i];
        ((db *)0x002cu)[0] = old_2c[0];
        ((db *)0x002cu)[1] = old_2c[1];
        ((db *)0x164au)[0] = old_164a[0];
        ((db *)0x164au)[1] = old_164a[1];
        return 0;
    }

    if (streq(op, "abispectrsqrt")) {
        unsigned long value;
        unsigned ax_after;
        unsigned bx_after;
        if (argc != 3) return 2;
        value = parse_u32(argv[2]);
        _asm {
            mov ebx, value
            call spectr_1C4F8
            mov ax_after, ax
            mov bx_after, bx
        }
        printf("ax=%04x bx=%04x\n", ax_after, bx_after);
        return 0;
    }

    if (streq(op, "abispectr1b406small")) {
        db old_payload[8];
        db old_state[0x18];
        db data[0x20];
        size_t n;
        unsigned i;
        if (argc != 3) return 2;
        for (i = 0; i < 8u; ++i) old_payload[i] = ((db *)0x2800u)[i];
        for (i = 0; i < 0x18u; ++i) old_state[i] = ((db *)0x7d1cu)[i];
        n = parse_hex_bytes(argv[2], (db *)0x2800u, 8);
        while (n < 8u) ((db *)0x2800u)[n++] = 0;
        ((db *)0x7d30u)[0] = 1;
        ((db *)0x7d30u)[1] = 0;
        _asm {
            mov di, 2800h
            call spectr_1B406
        }
        for (i = 0; i < 8u; ++i) data[i] = ((db *)0x2800u)[i];
        for (i = 0; i < 0x18u; ++i) data[8u + i] = ((db *)0x7d1cu)[i];
        for (i = 0; i < 8u; ++i) ((db *)0x2800u)[i] = old_payload[i];
        for (i = 0; i < 0x18u; ++i) ((db *)0x7d1cu)[i] = old_state[i];
        printf("data=");
        print_bytes(data, 0x20);
        printf("\n");
        return 0;
    }

    if (streq(op, "abireadsb")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 2) return 2;
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
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(op, "abireadmixersb")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 2) return 2;
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
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(op, "abiwritesb")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 2) return 2;
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
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(op, "abiwritemixersb")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 2) return 2;
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
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(op, "abichecksb")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 2) return 2;
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
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(op, "abisbdetectirq")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
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

    if (streq(op, "abiadlib18389")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
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

    if (streq(op, "abiadlib18395")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
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

    if (streq(op, "abisetegasequencer")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
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

    if (streq(op, "abigraph1c070")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
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

    if (streq(op, "abiuselessunsetegaseq")) {
        unsigned mode_bits;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        mode_bits = (unsigned)parse_u32(argv[2]) & 0xffu;
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

    if (streq(op, "abitxtblink")) {
        const char *symbol;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        symbol = argv[2];
        if (streq(symbol, "txt_blinkingoff")) {
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
        } else if (streq(symbol, "txt_enableblink")) {
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

    if (streq(op, "abitimerport")) {
        const char *symbol;
        unsigned ax_value;
        unsigned timer_word;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 4) return 2;
        symbol = argv[2];
        ax_value = (unsigned)parse_u32(argv[3]);
        if (streq(symbol, "set_timer")) {
            _asm {
                mov ax, ax_value
                mov bx, 5678h
                mov cx, 9abch
                mov dx, 0def0h
                call set_timer
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "clean_timer")) {
            _asm {
                mov ax, ax_value
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
        timer_word = abi_get_timer_word_14f6e();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x%02x\n",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               timer_word & 0xffu,
               (timer_word >> 8) & 0xffu);
        return 0;
    }

    if (streq(op, "abimemstrat")) {
        const char *symbol;
        unsigned config_word;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 4) return 2;
        symbol = argv[2];
        config_word = (unsigned)parse_u32(argv[3]);
        abi_set_config_word((unsigned short)config_word);
        if (streq(symbol, "setmemalloc1")) {
            _asm {
                mov ax, 0aaaah
                mov bx, 0bbbbh
                mov cx, 0cccch
                mov dx, 0ddddh
                call setmemalloc1
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "setmemalloc2")) {
            _asm {
                mov ax, 0aaaah
                mov bx, 0bbbbh
                mov cx, 0cccch
                mov dx, 0ddddh
                call setmemalloc2
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "setmemallocstrat")) {
            _asm {
                mov ax, 0aaaah
                mov bx, 0bbbbh
                mov cx, 0cccch
                mov dx, 0ddddh
                call setmemallocstrat
                mov ax_after, ax
                mov bx_after, bx
                mov cx_after, cx
                mov dx_after, dx
            }
        } else if (streq(symbol, "getmemallocstrat")) {
            _asm {
                mov ax, 0aaaah
                mov bx, 0bbbbh
                mov cx, 0cccch
                mov dx, 0ddddh
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

    if (streq(op, "abimemfree18a28")) {
        unsigned memflag;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        memflag = (unsigned)parse_u32(argv[2]);
        abi_set_memfree_18a28_state((unsigned char)memflag, 0x4321u);
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call memfree_18A28
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(op, "abimemfree")) {
        unsigned segment;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        segment = (unsigned)parse_u32(argv[2]);
        _asm {
            mov ax, segment
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call memfree
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n", ax_after, bx_after, cx_after, dx_after);
        return 0;
    }

    if (streq(op, "abimidiset")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
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

    if (streq(op, "abiintvect")) {
        unsigned int_number;
        unsigned vector_off;
        unsigned vector_seg;
        unsigned ax_after;
        unsigned bx_after;
        unsigned dx_after;
        unsigned ds_after;
        unsigned es_after;
        if (argc != 5) return 2;
        int_number = (unsigned)parse_u32(argv[2]);
        vector_off = (unsigned)parse_u32(argv[3]);
        vector_seg = (unsigned)parse_u32(argv[4]);
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
            mov ds_after, ds
            mov es_after, es
        }
        printf("ax=%04x bx=%04x dx=%04x ds=%04x es=%04x\n",
               ax_after,
               bx_after,
               dx_after,
               ds_after,
               es_after);
        return 0;
    }

    if (streq(op, "abikeybsw")) {
        const char *mode;
        unsigned value;
        unsigned word_after;
        if (argc != 4) return 2;
        mode = argv[2];
        value = (unsigned)parse_u32(argv[3]);
        if (streq(mode, "get")) {
            abi_set_keybsw_state(0x0600u, (unsigned short)value);
            _asm {
                call get_keybsw
            }
            word_after = abi_get_keyb_switches();
        } else if (streq(mode, "set")) {
            (void)value;
            abi_set_keybsw_state(0x0600u, 0x0600u);
            _asm {
                call set_keybsw
            }
            word_after = abi_get_bios_keyb_flags();
        } else {
            return 2;
        }
        printf("data=%02x%02x\n", word_after & 0xffu, (word_after >> 8) & 0xffu);
        return 0;
    }

    if (streq(op, "abisub197f2")) {
        unsigned configword;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        const unsigned char *labels;
        if (argc != 3) return 2;
        configword = (unsigned)parse_u32(argv[2]);
        abi_set_sub197f2_configword((unsigned short)configword);
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            call sub_197F2
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        labels = abi_get_sub197f2_labels();
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after);
        print_bytes(labels, 6);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiuseless11787zero")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned di_after;
        if (argc != 2) return 2;
        memset(mem + 0x9000u, 0, 0x60u);
        mem[0x9000u + 0x30u] = 0x55;
        mem[0x9000u + 0x31u] = 0x55;
        mem[0x9000u + 0x32u] = 0x66;
        mem[0x9000u + 0x33u] = 0x66;
        _asm {
            mov eax, 87654321h
            mov ebx, 11112222h
            mov ecx, 12345678h
            mov edx, 33334444h
            mov di, offset mem
            add di, 9000h
            call useless_11787
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov di_after, di
        }
        di_after = (unsigned)(0x1368u + (di_after - ((unsigned)mem + 0x9000u)));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
               ax_after,
               bx_after,
               cx_after,
               dx_after,
               di_after);
        print_bytes(mem + 0x9000u + 0x20u, 4);
        print_bytes(mem + 0x9000u + 0x30u, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiuselessdoswrite2")) {
        const unsigned char *header;
        if (argc != 2) return 2;
        _asm {
            mov eax, 504d4153h
            mov ecx, 12345678h
            mov dx, 2222h
            mov bx, 3333h
            call useless_doswrite2
        }
        header = abi_get_doswrite_header();
        printf("data=");
        print_bytes(header, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiuselessdoswrite")) {
        const unsigned char *header;
        unsigned dx_after;
        if (argc != 2) return 2;
        _asm {
            mov eax, 54534c50h
            mov ecx, 00000080h
            mov dx, 7fe8h
            mov bx, 3333h
            call useless_doswrite
            mov dx_after, dx
        }
        header = abi_get_doswrite_header();
        printf("dx=%04x data=", dx_after);
        print_bytes(header, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiult1150b")) {
        unsigned value;
        unsigned ax_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        value = (unsigned)parse_u32(argv[2]);
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

    if (streq(op, "abisub11c0c")) {
        unsigned count;
        unsigned ax_after;
        unsigned si_after;
        if (argc != 4) return 2;
        count = (unsigned)parse_u32(argv[2]) & 0xffu;
        memset(mem, 0, 0x100u);
        parse_hex_bytes(argv[3], mem, 0x100u);
        _asm {
            push ds
            pop es
            mov ax, count
            call sub_11C0C
            mov ax_after, ax
            mov si_after, si
        }
        printf("ax=%04x si=%04x\n", ax_after, si_after);
        return 0;
    }

    if (streq(op, "abisub1415e")) {
        unsigned index;
        unsigned total;
        unsigned segment_index;
        unsigned pending;
        unsigned si_after;
        const unsigned char *data;
        if (argc != 6) return 2;
        index = (unsigned)parse_u32(argv[2]);
        total = (unsigned)parse_u32(argv[3]);
        segment_index = (unsigned)parse_u32(argv[4]);
        pending = (unsigned)parse_u32(argv[5]);
        abi_set_sub1415e_state((unsigned short)index,
                               (unsigned short)total,
                               (unsigned char)segment_index,
                               (unsigned char)pending);
        _asm {
            call sub_1415E
            mov si_after, si
        }
        data = abi_get_sub1415e_data();
        printf("si=%04x data=", si_after);
        print_bytes(data, 20);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub12f56")) {
        unsigned index;
        unsigned total;
        unsigned segment_index;
        unsigned pending;
        unsigned bh;
        unsigned bx_value;
        unsigned si_after;
        const unsigned char *data;
        if (argc != 7) return 2;
        index = (unsigned)parse_u32(argv[2]);
        total = (unsigned)parse_u32(argv[3]);
        segment_index = (unsigned)parse_u32(argv[4]);
        pending = (unsigned)parse_u32(argv[5]);
        bh = (unsigned)parse_u32(argv[6]);
        bx_value = ((bh & 0xffu) << 8) | (pending & 0xffu);
        abi_set_sub1415e_state((unsigned short)index,
                               (unsigned short)total,
                               (unsigned char)segment_index,
                               (unsigned char)pending);
        _asm {
            mov ax, index
            mov bx, bx_value
            call sub_12F56
            mov si_after, si
        }
        data = abi_get_sub1415e_data();
        printf("si=%04x data=", si_after);
        print_bytes(data, 20);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub154f4")) {
        unsigned buffer_size2;
        unsigned flag;
        unsigned long sample_ptr;
        unsigned period;
        unsigned volume_index;
        unsigned seg_base;
        unsigned interp_word;
        const unsigned char *data;
        if (argc != 9) return 2;
        buffer_size2 = (unsigned)parse_u32(argv[2]);
        flag = (unsigned)parse_u32(argv[3]);
        sample_ptr = parse_u32(argv[4]);
        period = (unsigned)parse_u32(argv[5]);
        volume_index = (unsigned)parse_u32(argv[6]);
        seg_base = (unsigned)parse_u32(argv[7]);
        interp_word = (unsigned)parse_u32(argv[8]);
        memset(mem + 0x9000u, 0, 0x60u);
        mem[0x9000u + 0x04u] = (unsigned char)sample_ptr;
        mem[0x9000u + 0x05u] = (unsigned char)(sample_ptr >> 8);
        mem[0x9000u + 0x06u] = (unsigned char)(sample_ptr >> 16);
        mem[0x9000u + 0x07u] = (unsigned char)(sample_ptr >> 24);
        mem[0x9000u + 0x20u] = (unsigned char)period;
        mem[0x9000u + 0x21u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x23u] = (unsigned char)volume_index;
        mem[0x9000u + 0x24u] = (unsigned char)seg_base;
        mem[0x9000u + 0x25u] = (unsigned char)(seg_base >> 8);
        mem[0x9000u + 0x26u] = 0xff;
        mem[0x9000u + 0x27u] = 0xff;
        mem[0x9000u + 0x36u] = (unsigned char)interp_word;
        mem[0x9000u + 0x37u] = (unsigned char)(interp_word >> 8);
        abi_set_sub154f4_state((unsigned short)buffer_size2, (unsigned char)flag);
        _asm {
            mov si, offset mem
            add si, 9000h
            call sub_154F4
        }
        data = abi_get_sub154f4_data();
        printf("data=");
        print_bytes(data, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub15577guard")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned si_after;
        unsigned di_after;
        if (argc != 2) return 2;
        memset(mem + 0x9000u, 0, 0x50u);
        mem[0x9000u + 0x17u] = 0;
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            mov dx, 0def0h
            mov si, offset mem
            add si, 9000h
            mov di, 2468h
            call sub_15577
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov si_after, si
            mov di_after, di
        }
        si_after = (unsigned)(0x9000u + (si_after - ((unsigned)mem + 0x9000u)));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x data=%02x\n",
               ax_after, bx_after, cx_after, dx_after, si_after, di_after,
               mem[0x9000u + 0x17u]);
        return 0;
    }

    if (streq(op, "abisub13429guard")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 2) return 2;
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x03u] = 0x55;
        mem[0x9000u + 0x17u] = 0;
        _asm {
            mov ax, 1234h
            mov bx, offset mem
            add bx, 9000h
            mov cx, 9abch
            mov dx, 0def0h
            call sub_13429
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
        }
        bx_after = (unsigned)(0x9000u + (bx_after - ((unsigned)mem + 0x9000u)));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x%02x\n",
               ax_after, bx_after, cx_after, dx_after,
               mem[0x9000u + 0x03u], mem[0x9000u + 0x17u]);
        return 0;
    }

    if (streq(op, "abisub137d5guard")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned di_after;
        unsigned flags_3d;
        if (argc != 3) return 2;
        flags_3d = (unsigned)parse_u32(argv[2]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x0au] = 33;
        mem[0x9000u + 0x0bu] = 0x77;
        mem[0x9000u + 0x3du] = (unsigned char)flags_3d;
        _asm {
            mov ax, 1234h
            mov bx, offset mem
            add bx, 9000h
            mov cx, 9abch
            mov dx, 0def0h
            call sub_137D5
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov di_after, di
        }
        bx_after = (unsigned)(0x9000u + (bx_after - ((unsigned)mem + 0x9000u)));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=%02x%02x\n",
               ax_after, bx_after, cx_after, dx_after, di_after,
               mem[0x9000u + 0x0au], mem[0x9000u + 0x3du]);
        return 0;
    }

    if (streq(op, "abisub13826")) {
        unsigned value;
        unsigned table_word;
        unsigned ax_after;
        unsigned cx_after;
        unsigned di_after;
        if (argc != 4) return 2;
        value = (unsigned)parse_u32(argv[2]);
        table_word = (unsigned)parse_u32(argv[3]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x14u] = 0;
        mem[0x9000u + 0x15u] = 0;
        abi_set_sub13826_state((unsigned short)table_word);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call sub_13826
            mov ax_after, ax
            mov cx_after, cx
            mov di_after, di
        }
        printf("ax=%04x cx=%04x di=%04x\n", ax_after, cx_after, di_after);
        return 0;
    }

    if (streq(op, "abisub13813")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        unsigned di_after;
        if (argc != 2) return 2;
        memset(mem + 0x2800u, 0, 0x40u);
        mem[0x2800u + 0x0au] = 33;
        mem[0x2800u + 0x0bu] = 0x7c;
        _asm {
            mov ax, 1234h
            mov bx, offset mem
            add bx, 2800h
            mov cx, 5678h
            mov dx, 9abch
            mov di, 0def0h
            call sub_13813
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov dx_after, dx
            mov di_after, di
        }
        bx_after = (unsigned)(0x2800u + (bx_after - ((unsigned)mem + 0x2800u)));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=%02x%02x\n",
               ax_after, bx_after, cx_after, dx_after, di_after,
               mem[0x2800u + 0x0au], mem[0x2800u + 0x0bu]);
        return 0;
    }

    if (streq(op, "abisub140b6guard")) {
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        if (argc != 2) return 2;
        abi_set_sub140b6_state(1, 0);
        _asm {
            mov ax, 1234h
            mov bx, 5678h
            mov cx, 9abch
            call sub_140B6
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
        }
        printf("ax=%04x bx=%04x cx=%04x data=%02x%02x\n",
               ax_after, bx_after, cx_after,
               abi_get_sub140b6_byte_24671(), abi_get_sub140b6_byte_24668());
        return 0;
    }

    if (streq(op, "abisub14087")) {
        unsigned value;
        unsigned stored;
        unsigned byte_24668;
        unsigned ax_after;
        unsigned dx_after;
        if (argc != 5) return 2;
        value = (unsigned)parse_u32(argv[2]);
        stored = (unsigned)parse_u32(argv[3]);
        byte_24668 = (unsigned)parse_u32(argv[4]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x34u] = (unsigned char)stored;
        mem[0x00c8u] = (unsigned char)byte_24668;
        abi_set_sub14087_state((unsigned char)byte_24668);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            mov dx, 0100h
            call sub_14087
            mov ax_after, ax
            mov dx_after, dx
        }
        printf("ax=%04x dx=%04x data=%02x\n", ax_after, dx_after, mem[0x9000u + 0x34u]);
        return 0;
    }

    if (streq(op, "abicalc14043")) {
        unsigned byte_2467b;
        unsigned byte_2467c;
        unsigned ax_after;
        if (argc != 4) return 2;
        byte_2467b = (unsigned)parse_u32(argv[2]);
        byte_2467c = (unsigned)parse_u32(argv[3]);
        mem[0x00dbu] = (unsigned char)byte_2467b;
        mem[0x00dcu] = (unsigned char)byte_2467c;
        abi_set_calc14043_state((unsigned char)byte_2467b, (unsigned char)byte_2467c);
        _asm {
            call calc_14043
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x%02x\n",
               ax_after,
               mem[0x00dbu], mem[0x00dcu]);
        return 0;
    }

    if (streq(op, "abieff14030")) {
        unsigned value;
        unsigned byte_2467c;
        unsigned freq;
        unsigned buffer_size;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 6) return 2;
        value = (unsigned)parse_u32(argv[2]);
        byte_2467c = (unsigned)parse_u32(argv[3]);
        freq = (unsigned)parse_u32(argv[4]);
        buffer_size = (unsigned)parse_u32(argv[5]);
        mem[0x00dcu] = (unsigned char)byte_2467c;
        mem[0x00beu] = (unsigned char)freq;
        mem[0x00bfu] = (unsigned char)(freq >> 8);
        mem[0x0048u] = (unsigned char)buffer_size;
        mem[0x0049u] = (unsigned char)(buffer_size >> 8);
        abi_set_eff14030_state((unsigned char)byte_2467c, (unsigned short)freq, (unsigned short)buffer_size);
        _asm {
            mov ax, value
            call eff_14030
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x00dbu, 2);
        print_bytes(mem + 0x004au, 6);
        print_bytes(mem + 0x0044u, 2);
        print_bytes(mem + 0x00c6u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abichangevolume")) {
        unsigned value;
        unsigned channels;
        unsigned channel_volume;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 5) return 2;
        value = (unsigned)parse_u32(argv[2]);
        channels = (unsigned)parse_u32(argv[3]);
        channel_volume = (unsigned)parse_u32(argv[4]);
        abi_set_change_volume_state((unsigned short)channels, (unsigned char)channel_volume);
        _asm {
            mov ax, value
            call change_volume
            mov ax_after, ax
        }
        data = abi_get_change_volume_data();
        printf("ax=%04x data=", ax_after);
        print_bytes(data, 3);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiamplif")) {
        const char *symbol;
        unsigned value;
        unsigned sound_mode;
        unsigned channels;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 6) return 2;
        symbol = argv[2];
        value = (unsigned)parse_u32(argv[3]);
        sound_mode = (unsigned)parse_u32(argv[4]);
        channels = (unsigned)parse_u32(argv[5]);
        (void)channels;
        abi_set_amplif_state((unsigned char)sound_mode);
        if (streq(symbol, "eff_14020")) {
            _asm {
                mov ax, value
                call eff_14020
                mov ax_after, ax
            }
        } else if (streq(symbol, "change_amplif")) {
            _asm {
                mov ax, value
                call change_amplif
                mov ax_after, ax
            }
        } else {
            return 2;
        }
        data = abi_get_amplif_data();
        printf("ax=%04x data=", ax_after);
        print_bytes(data, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff14067")) {
        unsigned value;
        unsigned byte_2467b;
        unsigned byte_2467c;
        unsigned freq;
        unsigned buffer_size;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 7) return 2;
        value = (unsigned)parse_u32(argv[2]);
        byte_2467b = (unsigned)parse_u32(argv[3]);
        byte_2467c = (unsigned)parse_u32(argv[4]);
        freq = (unsigned)parse_u32(argv[5]);
        buffer_size = (unsigned)parse_u32(argv[6]);
        mem[0x00dbu] = (unsigned char)byte_2467b;
        mem[0x00dcu] = (unsigned char)byte_2467c;
        mem[0x00beu] = (unsigned char)freq;
        mem[0x00bfu] = (unsigned char)(freq >> 8);
        mem[0x0048u] = (unsigned char)buffer_size;
        mem[0x0049u] = (unsigned char)(buffer_size >> 8);
        abi_set_eff14067_state((unsigned char)byte_2467b, (unsigned char)byte_2467c, (unsigned short)freq, (unsigned short)buffer_size);
        _asm {
            mov ax, value
            call eff_14067
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x00dbu, 2);
        print_bytes(mem + 0x004au, 6);
        print_bytes(mem + 0x0044u, 2);
        print_bytes(mem + 0x00c6u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13ce8")) {
        unsigned initial_24667;
        unsigned initial_24668;
        unsigned value;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 5) return 2;
        initial_24667 = (unsigned)parse_u32(argv[2]);
        initial_24668 = (unsigned)parse_u32(argv[3]);
        value = (unsigned)parse_u32(argv[4]);
        abi_set_eff13ce8_state((unsigned char)initial_24667, (unsigned char)initial_24668);
        _asm {
            mov ax, value
            call eff_13CE8
            mov ax_after, ax
        }
        data = abi_get_eff13ce8_data();
        printf("ax=%04x data=", ax_after);
        print_bytes(data, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13a43")) {
        unsigned flags;
        unsigned sndflags;
        unsigned value;
        unsigned ax_after;
        if (argc != 5) return 2;
        flags = (unsigned)parse_u32(argv[2]);
        sndflags = (unsigned)parse_u32(argv[3]);
        value = (unsigned)parse_u32(argv[4]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        abi_set_eff13a43_state((unsigned char)sndflags);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13A43
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x9000u + 0x17u]);
        return 0;
    }

    if (streq(op, "abieff13a94")) {
        unsigned byte_16;
        unsigned sample_end;
        unsigned byte_2461a;
        unsigned flags;
        unsigned value;
        unsigned ax_after;
        if (argc != 7) return 2;
        byte_16 = (unsigned)parse_u32(argv[2]);
        sample_end = (unsigned)parse_u32(argv[3]);
        byte_2461a = (unsigned)parse_u32(argv[4]);
        flags = (unsigned)parse_u32(argv[5]);
        value = (unsigned)parse_u32(argv[6]);
        memset(mem + 0x9000u, 0, 0x50u);
        mem[0x9000u + 0x16u] = (unsigned char)byte_16;
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        mem[0x9000u + 0x30u] = (unsigned char)sample_end;
        mem[0x9000u + 0x31u] = (unsigned char)(sample_end >> 8);
        mem[0x9000u + 0x32u] = (unsigned char)(sample_end >> 16);
        mem[0x9000u + 0x33u] = (unsigned char)(sample_end >> 24);
        mem[0x9000u + 0x4cu] = 0xaa;
        mem[0x9000u + 0x4du] = 0xaa;
        abi_set_eff13a94_state((unsigned char)byte_2461a);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13A94
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u + 0x16u, 2);
        print_bytes(mem + 0x9000u + 0x4cu, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13bb2")) {
        unsigned flags;
        unsigned value;
        unsigned ax_after;
        if (argc != 4) return 2;
        flags = (unsigned)parse_u32(argv[2]);
        value = (unsigned)parse_u32(argv[3]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13BB2
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x9000u + 0x17u]);
        return 0;
    }

    if (streq(op, "abieff13ad7")) {
        unsigned volume;
        unsigned max_volume;
        unsigned value;
        unsigned ax_after;
        if (argc != 5) return 2;
        volume = (unsigned)parse_u32(argv[2]);
        max_volume = (unsigned)parse_u32(argv[3]);
        value = (unsigned)parse_u32(argv[4]);
        memset(mem + 0x9000u, 0, 0x10u);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        mem[0x00ddu] = (unsigned char)max_volume;
        abi_set_eff13ad7_state((unsigned char)max_volume);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13AD7
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13b06")) {
        unsigned playsettings;
        unsigned value;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 4) return 2;
        playsettings = (unsigned)parse_u32(argv[2]);
        value = (unsigned)parse_u32(argv[3]);
        (void)playsettings;
        abi_set_eff13b06_state();
        _asm {
            mov ax, value
            call eff_13B06
            mov ax_after, ax
        }
        data = abi_get_eff13b06_data();
        printf("ax=%04x data=", ax_after);
        print_bytes(data, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13b78")) {
        unsigned volume;
        unsigned max_volume;
        unsigned ax_after;
        if (argc != 4) return 2;
        volume = (unsigned)parse_u32(argv[2]);
        max_volume = (unsigned)parse_u32(argv[3]);
        memset(mem + 0x9000u, 0, 0x10u);
        abi_set_eff13b78_state((unsigned char)max_volume);
        _asm {
            mov ax, volume
            mov bx, offset mem
            add bx, 9000h
            call eff_13B78
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13b88")) {
        unsigned initial_24669;
        unsigned initial_2466a;
        unsigned value;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 5) return 2;
        initial_24669 = (unsigned)parse_u32(argv[2]);
        initial_2466a = (unsigned)parse_u32(argv[3]);
        value = (unsigned)parse_u32(argv[4]);
        mem[0x00c9u] = (unsigned char)initial_24669;
        mem[0x00cau] = (unsigned char)initial_2466a;
        abi_set_eff13b88_state((unsigned char)initial_24669, (unsigned char)initial_2466a);
        _asm {
            mov ax, value
            call eff_13B88
            mov ax_after, ax
        }
        data = mem + 0x00c9u;
        printf("ax=%04x data=", ax_after);
        print_bytes(data, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13ba3")) {
        unsigned flags;
        unsigned value;
        unsigned ax_after;
        if (argc != 4) return 2;
        flags = (unsigned)parse_u32(argv[2]);
        value = (unsigned)parse_u32(argv[3]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13BA3
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x9000u + 0x17u]);
        return 0;
    }

    if (streq(op, "abieff13bc0")) {
        unsigned initial;
        unsigned value;
        unsigned ax_after;
        if (argc != 4) return 2;
        initial = (unsigned)parse_u32(argv[2]);
        value = (unsigned)parse_u32(argv[3]);
        memset(mem + 0x9000u, 0, 0x10u);
        mem[0x9000u + 0x09u] = (unsigned char)initial;
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13BC0
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x9000u + 0x09u]);
        return 0;
    }

    if (streq(op, "abieff13c34")) {
        unsigned initial;
        unsigned value;
        unsigned ax_after;
        if (argc != 4) return 2;
        initial = (unsigned)parse_u32(argv[2]);
        value = (unsigned)parse_u32(argv[3]);
        memset(mem + 0x9000u, 0, 0x10u);
        mem[0x9000u + 0x09u] = (unsigned char)initial;
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13C34
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x9000u + 0x09u]);
        return 0;
    }

    if (streq(op, "abieff13bc8")) {
        unsigned byte_2461a;
        unsigned dx_value;
        unsigned value;
        unsigned ax_after;
        unsigned dx_after;
        if (argc != 5) return 2;
        byte_2461a = (unsigned)parse_u32(argv[2]);
        dx_value = (unsigned)parse_u32(argv[3]);
        value = (unsigned)parse_u32(argv[4]);
        memset(mem + 0x9000u, 0, 0x40u);
        abi_set_eff13bc8_state((unsigned char)byte_2461a);
        _asm {
            mov ax, value
            mov dx, dx_value
            mov bx, offset mem
            add bx, 9000h
            call eff_13BC8
            mov ax_after, ax
            mov dx_after, dx
        }
        printf("ax=%04x dx=%04x data=", ax_after, dx_after);
        print_bytes(mem + 0x9000u + 0x14u, 2);
        print_bytes(mem + 0x9000u + 0x38u, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13c3f")) {
        unsigned byte_24668;
        unsigned flags;
        unsigned sndflags;
        unsigned value;
        unsigned ax_after;
        if (argc != 6) return 2;
        byte_24668 = (unsigned)parse_u32(argv[2]);
        flags = (unsigned)parse_u32(argv[3]);
        sndflags = (unsigned)parse_u32(argv[4]);
        value = (unsigned)parse_u32(argv[5]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        abi_set_eff13c3f_state((unsigned char)byte_24668, (unsigned char)sndflags);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13C3F
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x9000u + 0x17u]);
        return 0;
    }

    if (streq(op, "abieff13c64")) {
        unsigned byte_24668;
        unsigned flags_3d;
        unsigned value;
        unsigned ax_after;
        if (argc != 5) return 2;
        byte_24668 = (unsigned)parse_u32(argv[2]);
        flags_3d = (unsigned)parse_u32(argv[3]);
        value = (unsigned)parse_u32(argv[4]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x3du] = (unsigned char)flags_3d;
        abi_set_eff13c64_state((unsigned char)byte_24668);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13C64
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x9000u + 0x3du]);
        return 0;
    }

    if (streq(op, "abieff13c88")) {
        unsigned volume;
        unsigned byte_24668;
        unsigned max_volume;
        unsigned value;
        unsigned ax_after;
        if (argc != 6) return 2;
        volume = (unsigned)parse_u32(argv[2]);
        byte_24668 = (unsigned)parse_u32(argv[3]);
        max_volume = (unsigned)parse_u32(argv[4]);
        value = (unsigned)parse_u32(argv[5]);
        memset(mem + 0x9000u, 0, 0x10u);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        abi_set_eff13c88_state((unsigned char)byte_24668, (unsigned char)max_volume);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13C88
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x9000u + 0x08u]);
        return 0;
    }

    if (streq(op, "abieff13c95")) {
        unsigned volume;
        unsigned byte_24668;
        unsigned value;
        unsigned ax_after;
        if (argc != 5) return 2;
        volume = (unsigned)parse_u32(argv[2]);
        byte_24668 = (unsigned)parse_u32(argv[3]);
        value = (unsigned)parse_u32(argv[4]);
        memset(mem + 0x9000u, 0, 0x10u);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        abi_set_eff13c95_state((unsigned char)byte_24668);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13C95
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x9000u + 0x08u]);
        return 0;
    }

    if (streq(op, "abieff13ca2")) {
        unsigned byte_24668;
        unsigned value;
        unsigned ax_after;
        if (argc != 4) return 2;
        byte_24668 = (unsigned)parse_u32(argv[2]);
        value = (unsigned)parse_u32(argv[3]);
        abi_set_eff13ca2_state((unsigned char)byte_24668);
        _asm {
            mov ax, value
            call eff_13CA2
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, (unsigned char)byte_24668);
        return 0;
    }

    if (streq(op, "abieff13cb3")) {
        unsigned period;
        unsigned byte_0a;
        unsigned byte_0b;
        unsigned byte_24668;
        unsigned value;
        unsigned ax_after;
        if (argc != 7) return 2;
        period = (unsigned)parse_u32(argv[2]);
        byte_0a = (unsigned)parse_u32(argv[3]);
        byte_0b = (unsigned)parse_u32(argv[4]);
        byte_24668 = (unsigned)parse_u32(argv[5]);
        value = (unsigned)parse_u32(argv[6]);
        memset(mem + 0x9000u, 0, 0x10u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9000u + 1u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x0au] = (unsigned char)byte_0a;
        mem[0x9000u + 0x0bu] = (unsigned char)byte_0b;
        abi_set_eff13cb3_state((unsigned char)byte_24668);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13CB3
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u + 0x0au, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13cc9")) {
        unsigned byte_24668;
        unsigned byte_2466d;
        unsigned initial_2466c;
        unsigned value;
        unsigned ax_after;
        if (argc != 6) return 2;
        byte_24668 = (unsigned)parse_u32(argv[2]);
        byte_2466d = (unsigned)parse_u32(argv[3]);
        initial_2466c = (unsigned)parse_u32(argv[4]);
        value = (unsigned)parse_u32(argv[5]);
        mem[0x00c8u] = (unsigned char)byte_24668;
        mem[0x00cdu] = (unsigned char)byte_2466d;
        mem[0x00ccu] = (unsigned char)initial_2466c;
        abi_set_eff13cc9_state((unsigned char)byte_24668, (unsigned char)byte_2466d, (unsigned char)initial_2466c);
        _asm {
            mov ax, value
            call eff_13CC9
            mov ax_after, ax
        }
        printf("ax=%04x data=%02x\n", ax_after, mem[0x00ccu]);
        return 0;
    }

    if (streq(op, "abieff13cdd")) {
        unsigned playsettings;
        unsigned initial_24667;
        unsigned initial_24668;
        unsigned value;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 6) return 2;
        playsettings = (unsigned)parse_u32(argv[2]);
        initial_24667 = (unsigned)parse_u32(argv[3]);
        initial_24668 = (unsigned)parse_u32(argv[4]);
        value = (unsigned)parse_u32(argv[5]);
        abi_set_eff13cdd_state((unsigned char)playsettings, (unsigned char)initial_24667, (unsigned char)initial_24668);
        _asm {
            mov ax, value
            call eff_13CDD
            mov ax_after, ax
        }
        data = abi_get_eff13ce8_data();
        printf("ax=%04x data=", ax_after);
        print_bytes(data, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13c02")) {
        unsigned byte_24668;
        unsigned word_245f6;
        unsigned byte_3b;
        unsigned byte_3c;
        unsigned value;
        unsigned ax_after;
        const unsigned char *globals;
        if (argc != 7) return 2;
        byte_24668 = (unsigned)parse_u32(argv[2]);
        word_245f6 = (unsigned)parse_u32(argv[3]);
        byte_3b = (unsigned)parse_u32(argv[4]);
        byte_3c = (unsigned)parse_u32(argv[5]);
        value = (unsigned)parse_u32(argv[6]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x3bu] = (unsigned char)byte_3b;
        mem[0x9000u + 0x3cu] = (unsigned char)byte_3c;
        abi_set_eff13c02_state((unsigned char)byte_24668, (unsigned short)word_245f6);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13C02
            mov ax_after, ax
        }
        globals = abi_get_eff13c02_globals();
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u + 0x3bu, 2);
        printf(" globals=");
        print_bytes(globals, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieffslide")) {
        const char *symbol;
        unsigned initial;
        unsigned value;
        unsigned active;
        unsigned which;
        unsigned ax_after;
        if (argc != 5 && argc != 6) return 2;
        symbol = argv[2];
        initial = (unsigned)parse_u32(argv[3]);
        value = (unsigned)parse_u32(argv[4]);
        active = argc == 6 ? (unsigned)parse_u32(argv[5]) : 0;
        if (streq(symbol, "eff_1387F")) which = 1;
        else if (streq(symbol, "eff_13886")) which = 2;
        else if (streq(symbol, "eff_1389D")) which = 3;
        else if (streq(symbol, "eff_138A4")) which = 4;
        else return 2;
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)initial;
        mem[0x9001u] = (unsigned char)(initial >> 8);
        abi_set_effect_slide_state((unsigned char)active);
        if (which == 1) {
            _asm {
                mov ax, value
                mov bx, offset mem
                add bx, 9000h
                call eff_1387F
                mov ax_after, ax
            }
        } else if (which == 2) {
            _asm {
                mov ax, value
                mov bx, offset mem
                add bx, 9000h
                call eff_13886
                mov ax_after, ax
            }
        } else if (which == 3) {
            _asm {
                mov ax, value
                mov bx, offset mem
                add bx, 9000h
                call eff_1389D
                mov ax_after, ax
            }
        } else {
            _asm {
                mov ax, value
                mov bx, offset mem
                add bx, 9000h
                call eff_138A4
                mov ax_after, ax
            }
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff1392f")) {
        unsigned period;
        unsigned byte_09;
        unsigned byte_0c;
        unsigned byte_0d;
        unsigned playsettings;
        unsigned value;
        unsigned ax_after;
        if (argc != 8) return 2;
        period = (unsigned)parse_u32(argv[2]);
        byte_09 = (unsigned)parse_u32(argv[3]);
        byte_0c = (unsigned)parse_u32(argv[4]);
        byte_0d = (unsigned)parse_u32(argv[5]);
        playsettings = (unsigned)parse_u32(argv[6]);
        value = (unsigned)parse_u32(argv[7]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x09u] = (unsigned char)byte_09;
        mem[0x9000u + 0x0cu] = (unsigned char)byte_0c;
        mem[0x9000u + 0x0du] = (unsigned char)byte_0d;
        mem[0x005eu] = (unsigned char)playsettings;
        abi_set_eff1392f_state(0);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_1392F
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x0cu, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff139ac")) {
        unsigned period;
        unsigned target;
        unsigned step;
        unsigned flags;
        unsigned volume;
        unsigned max_volume;
        unsigned value;
        unsigned ax_after;
        if (argc != 9) return 2;
        period = (unsigned)parse_u32(argv[2]);
        target = (unsigned)parse_u32(argv[3]);
        step = (unsigned)parse_u32(argv[4]);
        flags = (unsigned)parse_u32(argv[5]);
        volume = (unsigned)parse_u32(argv[6]);
        max_volume = (unsigned)parse_u32(argv[7]);
        value = (unsigned)parse_u32(argv[8]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        mem[0x9000u + 0x10u] = (unsigned char)target;
        mem[0x9000u + 0x11u] = (unsigned char)(target >> 8);
        mem[0x9000u + 0x12u] = (unsigned char)step;
        mem[0x9000u + 0x13u] = (unsigned char)(step >> 8);
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        mem[0x00ddu] = (unsigned char)max_volume;
        abi_set_eff139ac_state((unsigned char)max_volume);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_139AC
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        print_bytes(mem + 0x9000u + 0x10u, 4);
        print_bytes(mem + 0x9000u + 0x17u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff139b2")) {
        unsigned period;
        unsigned byte_09;
        unsigned byte_0c;
        unsigned byte_0d;
        unsigned playsettings;
        unsigned volume;
        unsigned max_volume;
        unsigned value;
        unsigned ax_after;
        if (argc != 10) return 2;
        period = (unsigned)parse_u32(argv[2]);
        byte_09 = (unsigned)parse_u32(argv[3]);
        byte_0c = (unsigned)parse_u32(argv[4]);
        byte_0d = (unsigned)parse_u32(argv[5]);
        playsettings = (unsigned)parse_u32(argv[6]);
        volume = (unsigned)parse_u32(argv[7]);
        max_volume = (unsigned)parse_u32(argv[8]);
        value = (unsigned)parse_u32(argv[9]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        mem[0x9000u + 0x09u] = (unsigned char)byte_09;
        mem[0x9000u + 0x0cu] = (unsigned char)byte_0c;
        mem[0x9000u + 0x0du] = (unsigned char)byte_0d;
        mem[0x005eu] = (unsigned char)playsettings;
        mem[0x00ddu] = (unsigned char)max_volume;
        abi_set_eff139b2_state((unsigned char)max_volume, (unsigned char)playsettings);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_139B2
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        print_bytes(mem + 0x9000u + 0x0cu, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff139b9")) {
        unsigned volume;
        unsigned byte_09;
        unsigned byte_0e;
        unsigned byte_0f;
        unsigned max_volume;
        unsigned value;
        unsigned ax_after;
        if (argc != 8) return 2;
        volume = (unsigned)parse_u32(argv[2]);
        byte_09 = (unsigned)parse_u32(argv[3]);
        byte_0e = (unsigned)parse_u32(argv[4]);
        byte_0f = (unsigned)parse_u32(argv[5]);
        max_volume = (unsigned)parse_u32(argv[6]);
        value = (unsigned)parse_u32(argv[7]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        mem[0x9000u + 0x09u] = (unsigned char)byte_09;
        mem[0x9000u + 0x0eu] = (unsigned char)byte_0e;
        mem[0x9000u + 0x0fu] = (unsigned char)byte_0f;
        mem[0x00ddu] = (unsigned char)max_volume;
        abi_set_eff139b9_state((unsigned char)max_volume);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_139B9
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        print_bytes(mem + 0x9000u + 0x0eu, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff138d2")) {
        unsigned period;
        unsigned target;
        unsigned step;
        unsigned flags;
        unsigned value;
        unsigned ax_after;
        if (argc != 7) return 2;
        period = (unsigned)parse_u32(argv[2]);
        target = (unsigned)parse_u32(argv[3]);
        step = (unsigned)parse_u32(argv[4]);
        flags = (unsigned)parse_u32(argv[5]);
        value = (unsigned)parse_u32(argv[6]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x10u] = (unsigned char)target;
        mem[0x9000u + 0x11u] = (unsigned char)(target >> 8);
        mem[0x9000u + 0x12u] = (unsigned char)step;
        mem[0x9000u + 0x13u] = (unsigned char)(step >> 8);
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_138D2
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x10u, 4);
        print_bytes(mem + 0x9000u + 0x17u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13de")) {
        const char *symbol;
        unsigned period;
        unsigned byte_24668;
        unsigned stored_34;
        unsigned value;
        unsigned ax_after;
        unsigned which;
        if (argc != 7) return 2;
        symbol = argv[2];
        period = (unsigned)parse_u32(argv[3]);
        byte_24668 = (unsigned)parse_u32(argv[4]);
        stored_34 = (unsigned)parse_u32(argv[5]);
        value = (unsigned)parse_u32(argv[6]);
        if (streq(symbol, "eff_13DE5")) which = 1;
        else if (streq(symbol, "eff_13DEF")) which = 2;
        else return 2;
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x34u] = (unsigned char)stored_34;
        mem[0x00c8u] = (unsigned char)byte_24668;
        abi_set_sub14087_state((unsigned char)byte_24668);
        if (which == 1) {
            _asm {
                mov ax, value
                mov bx, offset mem
                add bx, 9000h
                call eff_13DE5
                mov ax_after, ax
            }
        } else {
            _asm {
                mov ax, value
                mov bx, offset mem
                add bx, 9000h
                call eff_13DEF
                mov ax_after, ax
            }
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x34u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13e1e")) {
        unsigned period;
        unsigned target;
        unsigned step;
        unsigned flags;
        unsigned value;
        unsigned ax_after;
        if (argc != 7) return 2;
        period = (unsigned)parse_u32(argv[2]);
        target = (unsigned)parse_u32(argv[3]);
        step = (unsigned)parse_u32(argv[4]);
        flags = (unsigned)parse_u32(argv[5]);
        value = (unsigned)parse_u32(argv[6]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x10u] = (unsigned char)target;
        mem[0x9000u + 0x11u] = (unsigned char)(target >> 8);
        mem[0x9000u + 0x12u] = (unsigned char)step;
        mem[0x9000u + 0x13u] = (unsigned char)(step >> 8);
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13E1E
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x10u, 4);
        print_bytes(mem + 0x9000u + 0x17u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13e2d")) {
        unsigned period;
        unsigned byte_09;
        unsigned byte_0c;
        unsigned byte_0d;
        unsigned playsettings;
        unsigned value;
        unsigned ax_after;
        if (argc != 8) return 2;
        period = (unsigned)parse_u32(argv[2]);
        byte_09 = (unsigned)parse_u32(argv[3]);
        byte_0c = (unsigned)parse_u32(argv[4]);
        byte_0d = (unsigned)parse_u32(argv[5]);
        playsettings = (unsigned)parse_u32(argv[6]);
        value = (unsigned)parse_u32(argv[7]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x09u] = (unsigned char)byte_09;
        mem[0x9000u + 0x0cu] = (unsigned char)byte_0c;
        mem[0x9000u + 0x0du] = (unsigned char)byte_0d;
        mem[0x005eu] = (unsigned char)playsettings;
        abi_set_eff1392f_state(0);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13E2D
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x0cu, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13e32")) {
        unsigned volume;
        unsigned byte_24668;
        unsigned max_volume;
        unsigned stored_34;
        unsigned value;
        unsigned ax_after;
        if (argc != 7) return 2;
        volume = (unsigned)parse_u32(argv[2]);
        byte_24668 = (unsigned)parse_u32(argv[3]);
        max_volume = (unsigned)parse_u32(argv[4]);
        stored_34 = (unsigned)parse_u32(argv[5]);
        value = (unsigned)parse_u32(argv[6]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        mem[0x9000u + 0x34u] = (unsigned char)stored_34;
        mem[0x00c8u] = (unsigned char)byte_24668;
        mem[0x00ddu] = (unsigned char)max_volume;
        abi_set_eff13e32_state((unsigned char)byte_24668, (unsigned char)max_volume);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13E32
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        print_bytes(mem + 0x9000u + 0x34u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13e7f")) {
        unsigned period;
        unsigned target;
        unsigned step;
        unsigned flags;
        unsigned volume;
        unsigned byte_24668;
        unsigned max_volume;
        unsigned stored_34;
        unsigned value;
        unsigned ax_after;
        if (argc != 11) return 2;
        period = (unsigned)parse_u32(argv[2]);
        target = (unsigned)parse_u32(argv[3]);
        step = (unsigned)parse_u32(argv[4]);
        flags = (unsigned)parse_u32(argv[5]);
        volume = (unsigned)parse_u32(argv[6]);
        byte_24668 = (unsigned)parse_u32(argv[7]);
        max_volume = (unsigned)parse_u32(argv[8]);
        stored_34 = (unsigned)parse_u32(argv[9]);
        value = (unsigned)parse_u32(argv[10]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        mem[0x9000u + 0x10u] = (unsigned char)target;
        mem[0x9000u + 0x11u] = (unsigned char)(target >> 8);
        mem[0x9000u + 0x12u] = (unsigned char)step;
        mem[0x9000u + 0x13u] = (unsigned char)(step >> 8);
        mem[0x9000u + 0x17u] = (unsigned char)flags;
        mem[0x9000u + 0x34u] = (unsigned char)stored_34;
        mem[0x00c8u] = (unsigned char)byte_24668;
        mem[0x00ddu] = (unsigned char)max_volume;
        abi_set_eff13e32_state((unsigned char)byte_24668, (unsigned char)max_volume);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13E7F
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        print_bytes(mem + 0x9000u + 0x10u, 4);
        print_bytes(mem + 0x9000u + 0x17u, 1);
        print_bytes(mem + 0x9000u + 0x34u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13e84")) {
        unsigned period;
        unsigned byte_09;
        unsigned byte_0c;
        unsigned byte_0d;
        unsigned playsettings;
        unsigned volume;
        unsigned byte_24668;
        unsigned max_volume;
        unsigned stored_34;
        unsigned value;
        unsigned ax_after;
        if (argc != 12) return 2;
        period = (unsigned)parse_u32(argv[2]);
        byte_09 = (unsigned)parse_u32(argv[3]);
        byte_0c = (unsigned)parse_u32(argv[4]);
        byte_0d = (unsigned)parse_u32(argv[5]);
        playsettings = (unsigned)parse_u32(argv[6]);
        volume = (unsigned)parse_u32(argv[7]);
        byte_24668 = (unsigned)parse_u32(argv[8]);
        max_volume = (unsigned)parse_u32(argv[9]);
        stored_34 = (unsigned)parse_u32(argv[10]);
        value = (unsigned)parse_u32(argv[11]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        mem[0x9000u + 0x09u] = (unsigned char)byte_09;
        mem[0x9000u + 0x0cu] = (unsigned char)byte_0c;
        mem[0x9000u + 0x0du] = (unsigned char)byte_0d;
        mem[0x9000u + 0x34u] = (unsigned char)stored_34;
        mem[0x005eu] = (unsigned char)playsettings;
        mem[0x00c8u] = (unsigned char)byte_24668;
        mem[0x00ddu] = (unsigned char)max_volume;
        abi_set_eff1392f_state(0);
        abi_set_eff13e32_state((unsigned char)byte_24668, (unsigned char)max_volume);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13E84
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        print_bytes(mem + 0x9000u + 0x0cu, 2);
        print_bytes(mem + 0x9000u + 0x34u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13e8c")) {
        unsigned value;
        unsigned freq;
        unsigned buffer_size;
        unsigned ax_after;
        const unsigned char *data;
        if (argc != 5) return 2;
        value = (unsigned)parse_u32(argv[2]);
        freq = (unsigned)parse_u32(argv[3]);
        buffer_size = (unsigned)parse_u32(argv[4]);
        mem[0x00beu] = (unsigned char)freq;
        mem[0x00bfu] = (unsigned char)(freq >> 8);
        mem[0x0048u] = (unsigned char)buffer_size;
        mem[0x0049u] = (unsigned char)(buffer_size >> 8);
        abi_set_eff13e8c_state((unsigned short)freq, (unsigned short)buffer_size);
        _asm {
            mov ax, value
            call eff_13E8C
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x004au, 6);
        print_bytes(mem + 0x0044u, 2);
        print_bytes(mem + 0x00c6u, 3);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13f05")) {
        unsigned volume;
        unsigned byte_24668;
        unsigned stored_34;
        unsigned value;
        unsigned ax_after;
        if (argc != 6) return 2;
        volume = (unsigned)parse_u32(argv[2]);
        byte_24668 = (unsigned)parse_u32(argv[3]);
        stored_34 = (unsigned)parse_u32(argv[4]);
        value = (unsigned)parse_u32(argv[5]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        mem[0x9000u + 0x34u] = (unsigned char)stored_34;
        mem[0x00c8u] = (unsigned char)byte_24668;
        abi_set_eff13f05_state((unsigned char)byte_24668);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13F05
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        print_bytes(mem + 0x9000u + 0x34u, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13f3b")) {
        unsigned volume;
        unsigned byte_24668;
        unsigned max_volume;
        unsigned flags_3d;
        unsigned stored_34;
        unsigned value;
        unsigned ax_after;
        if (argc != 8) return 2;
        volume = (unsigned)parse_u32(argv[2]);
        byte_24668 = (unsigned)parse_u32(argv[3]);
        max_volume = (unsigned)parse_u32(argv[4]);
        flags_3d = (unsigned)parse_u32(argv[5]);
        stored_34 = (unsigned)parse_u32(argv[6]);
        value = (unsigned)parse_u32(argv[7]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u + 0x08u] = (unsigned char)volume;
        mem[0x9000u + 0x34u] = (unsigned char)stored_34;
        mem[0x9000u + 0x3du] = (unsigned char)flags_3d;
        mem[0x00c8u] = (unsigned char)byte_24668;
        mem[0x00ddu] = (unsigned char)max_volume;
        abi_set_eff13f3b_state((unsigned char)byte_24668, (unsigned char)max_volume);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13F3B
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u + 0x08u, 1);
        print_bytes(mem + 0x9000u + 0x34u, 1);
        print_bytes(mem + 0x9000u + 0x3du, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13fbe")) {
        unsigned period;
        unsigned byte_0b;
        unsigned byte_24668;
        unsigned stored_34;
        unsigned byte_35;
        unsigned value;
        unsigned ax_after;
        if (argc != 8) return 2;
        period = (unsigned)parse_u32(argv[2]);
        byte_0b = (unsigned)parse_u32(argv[3]);
        byte_24668 = (unsigned)parse_u32(argv[4]);
        stored_34 = (unsigned)parse_u32(argv[5]);
        byte_35 = (unsigned)parse_u32(argv[6]);
        value = (unsigned)parse_u32(argv[7]);
        memset(mem + 0x9000u, 0, 0x40u);
        mem[0x9000u] = (unsigned char)period;
        mem[0x9001u] = (unsigned char)(period >> 8);
        mem[0x9000u + 0x0bu] = (unsigned char)byte_0b;
        mem[0x9000u + 0x34u] = (unsigned char)stored_34;
        mem[0x9000u + 0x35u] = (unsigned char)byte_35;
        mem[0x00c8u] = (unsigned char)byte_24668;
        abi_set_eff13fbe_state((unsigned char)byte_24668);
        _asm {
            mov ax, value
            mov bx, offset mem
            add bx, 9000h
            call eff_13FBE
            mov ax_after, ax
        }
        printf("ax=%04x data=", ax_after);
        print_bytes(mem + 0x9000u, 2);
        print_bytes(mem + 0x9000u + 0x0bu, 1);
        print_bytes(mem + 0x9000u + 0x34u, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub1609fdisabled")) {
        unsigned buffer_size;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned si_after;
        unsigned di_after;
        if (argc != 3) return 2;
        buffer_size = (unsigned)parse_u32(argv[2]);
        memset(mem + 0x9000u, 0, 0x50u);
        memset(mem + 0x2800u, 0xa5, buffer_size * 8u);
        mem[0x9000u + 0x17u] = 0;
        abi_set_sub1609f_state((unsigned short)buffer_size);
        _asm {
            mov si, offset mem
            add si, 9000h
            mov di, offset mem
            add di, 2800h
            call sub_1609F
            mov ax_after, ax
            mov bx_after, bx
            mov cx_after, cx
            mov si_after, si
            mov di_after, di
        }
        si_after = (unsigned)(0x9000u + (si_after - ((unsigned)mem + 0x9000u)));
        di_after = (unsigned)(0x2800u + (di_after - ((unsigned)mem + 0x2800u)));
        printf("ax=%04x bx=%04x cx=%04x si=%04x di=%04x data=",
               ax_after, bx_after, cx_after, si_after, di_after);
        print_bytes(mem + 0x2800u, buffer_size * 8u);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisetdmamask")) {
        unsigned channel;
        unsigned ax_after;
        unsigned bx_after;
        unsigned cx_after;
        unsigned dx_after;
        if (argc != 3) return 2;
        channel = (unsigned)parse_u32(argv[2]);
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

    if (streq(op, "abihex16") || streq(op, "abihex8") || streq(op, "abihex4")) {
        unsigned long value;
        unsigned si_after;
        unsigned mem_off;
        unsigned count;
        if (argc != 3) return 2;
        value = parse_u32(argv[2]);
        count = streq(op, "abihex16") ? 4 : streq(op, "abihex8") ? 2 : 1;
        if (streq(op, "abihex16")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call u16tox
                mov si_after, si
                mov mem_off, offset mem
            }
        } else if (streq(op, "abihex8")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call u8tox
                mov si_after, si
                mov mem_off, offset mem
            }
        } else {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call u4tox
                mov si_after, si
                mov mem_off, offset mem
            }
        }
        si_after = (si_after - mem_off) & 0xffffu;
        printf("si=%04x data=", si_after);
        print_bytes(mem + ORIG_DST_OFF, count);
        printf("\n");
        return 0;
    }

    if (streq(op, "abihex32")) {
        unsigned long value;
        unsigned si_after;
        if (argc != 3) return 2;
        value = parse_u32(argv[2]);
        _asm {
            mov eax, value
            mov si, offset mem
            add si, 9100h
            call u32tox
            mov si_after, si
        }
        si_after = (unsigned)(ORIG_DST_OFF + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("si=%04x data=", si_after);
        print_bytes(mem + ORIG_DST_OFF, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiputdigit")) {
        unsigned si_after;
        unsigned cx_after;
        unsigned dx_value;
        if (argc != 4) return 2;
        cx_after = (unsigned)parse_u32(argv[2]);
        dx_value = (unsigned)parse_u32(argv[3]);
        _asm {
            mov cx, cx_after
            mov dx, dx_value
            mov si, offset mem
            add si, 9100h
            call my_putdigit
            mov si_after, si
            mov cx_after, cx
        }
        si_after = (unsigned)(ORIG_DST_OFF + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimyputdigit")) {
        unsigned si_after;
        unsigned cx_after;
        unsigned dx_value;
        if (argc != 4) return 2;
        cx_after = (unsigned)parse_u32(argv[2]);
        dx_value = (unsigned)parse_u32(argv[3]);
        _asm {
            mov cx, cx_after
            mov dx, dx_value
            mov si, offset mem
            add si, 9100h
            call myputdigit
            mov si_after, si
            mov cx_after, cx
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimyhex")) {
        const char *symbol;
        unsigned long value;
        unsigned ax_after;
        unsigned si_after;
        unsigned count;
        if (argc != 4) return 2;
        symbol = argv[2];
        value = parse_u32(argv[3]);
        count = streq(symbol, "my_u32tox") ? 8 : streq(symbol, "my_u16tox") ? 4 : streq(symbol, "my_u8tox") ? 2 : 1;
        if (streq(symbol, "my_u32tox")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_u32tox
                mov ax_after, ax
                mov si_after, si
            }
        } else if (streq(symbol, "my_u16tox")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_u16tox
                mov ax_after, ax
                mov si_after, si
            }
        } else if (streq(symbol, "my_u8tox")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_u8tox
                mov ax_after, ax
                mov si_after, si
            }
        } else if (streq(symbol, "my_u4tox")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_u4tox
                mov ax_after, ax
                mov si_after, si
            }
        } else {
            return 2;
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("ax=%04x si=%04x data=", ax_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, count);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiu32toa10") || streq(op, "abii32toa10")) {
        unsigned long value;
        unsigned si_after;
        unsigned cx_after;
        if (argc != 3) return 2;
        value = parse_u32(argv[2]);
        if (streq(op, "abiu32toa10")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_u32toa10_0
                mov si_after, si
                mov cx_after, cx
            }
        } else {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_i32toa10_0
                mov si_after, si
                mov cx_after, cx
            }
        }
        si_after = (unsigned)(ORIG_DST_OFF + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, cx_after);
        printf("\n");
        return 0;
    }

    if (streq(op, "abidecimal16")) {
        const char *symbol;
        unsigned value;
        unsigned si_after;
        unsigned cx_after;
        if (argc != 4) return 2;
        symbol = argv[2];
        value = (unsigned)parse_u32(argv[3]);
        if (streq(symbol, "my_u8toa_10")) {
            _asm {
                mov ax, value
                mov si, offset mem
                add si, 9100h
                call my_u8toa_10
                mov si_after, si
                mov cx_after, cx
            }
        } else if (streq(symbol, "my_u16toa_10")) {
            _asm {
                mov ax, value
                mov si, offset mem
                add si, 9100h
                call my_u16toa_10
                mov si_after, si
                mov cx_after, cx
            }
        } else if (streq(symbol, "my_i8toa10_0")) {
            _asm {
                mov ax, value
                mov si, offset mem
                add si, 9100h
                call my_i8toa10_0
                mov si_after, si
                mov cx_after, cx
            }
        } else if (streq(symbol, "my_i16toa10_0")) {
            _asm {
                mov ax, value
                mov si, offset mem
                add si, 9100h
                call my_i16toa10_0
                mov si_after, si
                mov cx_after, cx
            }
        } else {
            return 2;
        }
        si_after = (unsigned)(ORIG_DST_OFF + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, cx_after);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimyutoa10")) {
        const char *symbol;
        unsigned long value;
        unsigned si_after;
        unsigned cx_after;
        if (argc != 4) return 2;
        symbol = argv[2];
        value = parse_u32(argv[3]);
        if (streq(symbol, "my_u32toa10")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_u32toa10
                mov si_after, si
                mov cx_after, cx
            }
        } else if (streq(symbol, "my_u16toa10")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_u16toa10
                mov si_after, si
                mov cx_after, cx
            }
        } else if (streq(symbol, "my_u8toa10")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_u8toa10
                mov si_after, si
                mov cx_after, cx
            }
        } else {
            return 2;
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, cx_after);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimyu32toa")) {
        unsigned long value;
        unsigned long base;
        unsigned si_after;
        unsigned cx_after;
        if (argc != 4) return 2;
        value = parse_u32(argv[2]);
        base = parse_u32(argv[3]);
        _asm {
            mov eax, value
            mov ebx, base
            xor cx, cx
            mov si, offset mem
            add si, 9100h
            call my_u32toa
            mov si_after, si
            mov cx_after, cx
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, cx_after);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimyu32toa0")) {
        unsigned long value;
        unsigned long base;
        unsigned si_after;
        unsigned cx_after;
        if (argc != 4) return 2;
        value = parse_u32(argv[2]);
        base = parse_u32(argv[3]);
        _asm {
            mov eax, value
            mov ebx, base
            xor cx, cx
            mov si, offset mem
            add si, 9100h
            call my_u32toa_0
            mov si_after, si
            mov cx_after, cx
        }
        si_after = (unsigned)(ORIG_DST_OFF + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, cx_after);
        printf("\n");
        return 0;
    }

    if (streq(op, "abifill")) {
        const char *symbol;
        unsigned long value;
        unsigned count;
        unsigned di_after;
        unsigned out_len;
        if (argc != 5) return 2;
        symbol = argv[2];
        value = parse_u32(argv[3]);
        count = (unsigned)parse_u32(argv[4]);
        out_len = count + (streq(symbol, "my_pnt_u32toa_fill") ? 2u : 0u);
        if (streq(symbol, "my_pnt_u32toa_fill")) {
            _asm {
                mov eax, value
                push bp
                mov bp, count
                mov di, offset mem
                add di, 9100h
                call my_pnt_u32toa_fill
                pop bp
                mov di_after, di
            }
        } else if (streq(symbol, "my_u32toa_fill")) {
            _asm {
                mov eax, value
                push bp
                mov bp, count
                mov di, offset mem
                add di, 9100h
                call my_u32toa_fill
                pop bp
                mov di_after, di
            }
        } else {
            return 2;
        }
        di_after = (unsigned)(0x2800u + (di_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("di=%04x data=", di_after);
        print_bytes(mem + ORIG_DST_OFF, out_len);
        printf("\n");
        return 0;
    }

    if (streq(op, "abidmafillbuf")) {
        const char *symbol;
        unsigned count;
        unsigned si_after;
        unsigned di_after;
        unsigned i;
        if (argc != 4) return 2;
        symbol = argv[2];
        count = (unsigned)parse_u32(argv[3]);
        for (i = 0; i < 64; ++i) mem[ORIG_SRC_OFF + i] = (unsigned char)(0x10u + i);
        for (i = 0; i < 8; ++i) mem[ORIG_DST_OFF + i] = 0xa5;
        if (streq(symbol, "fill_dmabuf8")) {
            _asm {
                mov si, offset mem
                add si, 9000h
                mov di, offset mem
                add di, 9100h
                mov cx, count
                call fill_dmabuf8
                mov si_after, si
                mov di_after, di
            }
        } else if (streq(symbol, "fill_dmabuf8stereo")) {
            _asm {
                mov si, offset mem
                add si, 9000h
                mov di, offset mem
                add di, 9100h
                mov cx, count
                call fill_dmabuf8stereo
                mov si_after, si
                mov di_after, di
            }
        } else if (streq(symbol, "fill_dmabuf16stereo")) {
            _asm {
                mov si, offset mem
                add si, 9000h
                mov di, offset mem
                add di, 9100h
                mov cx, count
                call fill_dmabuf16stereo
                mov si_after, si
                mov di_after, di
            }
        } else {
            return 2;
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + ORIG_SRC_OFF)));
        di_after = (unsigned)(0x2900u + (di_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("si=%04x di=%04x data=", si_after, di_after);
        print_bytes(mem + ORIG_DST_OFF, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "abistrlen")) {
        const char *symbol;
        const char *text;
        unsigned si_after;
        unsigned ax_after;
        unsigned base;
        size_t len;
        if (argc != 3 && argc != 4) return 2;
        symbol = argv[2];
        text = argc == 4 ? argv[3] : "";
        len = strlen(text);
        if (len > 255u) return 2;
        memcpy(mem + ORIG_DST_OFF, text, len + 1u);
        if (streq(symbol, "mystrlen")) {
            _asm {
                mov si, offset mem
                add si, 9100h
                call mystrlen
                mov si_after, si
                mov ax_after, ax
            }
            base = 0x2800u;
        } else if (streq(symbol, "mystrlen_0")) {
            _asm {
                mov si, offset mem
                add si, 9100h
                call mystrlen_0
                mov si_after, si
                mov ax_after, ax
            }
            base = 0x9000u;
        } else {
            return 2;
        }
        si_after = (unsigned)(base + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("ax=%04x si=%04x\n", ax_after, si_after);
        return 0;
    }

    if (streq(op, "abistrcpy")) {
        const char *symbol;
        const char *text;
        unsigned si_after;
        unsigned di_after;
        unsigned cx_after;
        unsigned ax_after;
        unsigned src_base;
        unsigned dst_base;
        unsigned dump_len;
        size_t len;
        if (argc != 3 && argc != 4) return 2;
        symbol = argv[2];
        text = argc == 4 ? argv[3] : "";
        len = strlen(text);
        if (len > 255u) return 2;
        memcpy(mem + ORIG_DST_OFF, text, len + 1u);
        memset(mem + ORIG_DST_OFF + 0x100u, '.', len + 1u);
        if (streq(symbol, "strcpy_count")) {
            _asm {
                push ds
                pop es
                mov si, offset mem
                add si, 9100h
                mov di, offset mem
                add di, 9200h
                call strcpy_count
                mov si_after, si
                mov di_after, di
                mov cx_after, cx
                mov ax_after, ax
            }
            src_base = 0x2800u;
            dst_base = 0x2840u;
            dump_len = (unsigned)len;
        } else if (streq(symbol, "strcpy_count_0")) {
            _asm {
                push ds
                pop es
                mov si, offset mem
                add si, 9100h
                mov di, offset mem
                add di, 9200h
                call strcpy_count_0
                mov si_after, si
                mov di_after, di
                mov cx_after, cx
                mov ax_after, ax
            }
            src_base = 0x9000u;
            dst_base = 0x9100u;
            dump_len = (unsigned)(len + 1u);
        } else {
            return 2;
        }
        si_after = (unsigned)(src_base + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        di_after = (unsigned)(dst_base + (di_after - ((unsigned)mem + ORIG_DST_OFF + 0x100u)));
        printf("ax=%04x cx=%04x si=%04x di=%04x data=", ax_after, cx_after, si_after, di_after);
        print_bytes(mem + ORIG_DST_OFF + 0x100u, dump_len);
        printf("\n");
        return 0;
    }

    if (streq(op, "abicopyprint")) {
        const char *symbol;
        unsigned count;
        unsigned si_after;
        unsigned di_after;
        unsigned cx_after;
        unsigned ax_after;
        unsigned src_base;
        unsigned dst_base;
        if (argc != 5) return 2;
        symbol = argv[2];
        parse_hex_bytes(argv[3], mem + 0x9000u, 0x100u);
        count = (unsigned)parse_u32(argv[4]);
        memset(mem + 0x9200u, '.', count);
        if (streq(symbol, "copy_printable")) {
            _asm {
                mov si, offset mem
                add si, 9000h
                mov di, offset mem
                add di, 9200h
                mov cx, count
                call copy_printable
                mov si_after, si
                mov di_after, di
                mov cx_after, cx
                mov ax_after, ax
            }
            src_base = 0x9000u;
            dst_base = 0x9100u;
        } else if (streq(symbol, "cpy_printable")) {
            _asm {
                push ds
                pop es
                mov si, offset mem
                add si, 9000h
                mov di, offset mem
                add di, 9200h
                mov cx, count
                call cpy_printable
                mov si_after, si
                mov di_after, di
                mov cx_after, cx
                mov ax_after, ax
            }
            src_base = 0x2800u;
            dst_base = 0x2840u;
        } else {
            return 2;
        }
        si_after = (unsigned)(src_base + (si_after - ((unsigned)mem + 0x9000u)));
        di_after = (unsigned)(dst_base + (di_after - ((unsigned)mem + 0x9200u)));
        printf("ax=%04x cx=%04x si=%04x di=%04x data=", ax_after, cx_after, si_after, di_after);
        print_bytes(mem + 0x9200u, count);
        printf("\n");
        return 0;
    }

    if (streq(op, "abitxt1abae")) {
        const char *text;
        unsigned si_after;
        unsigned di_after;
        unsigned ax_after;
        size_t len;
        if (argc != 3) return 2;
        text = argv[2];
        len = strlen(text);
        if (len < 0x16u) return 2;
        memcpy(mem + 0x9000u, text, 0x16u);
        memset(mem + 0x9200u, 0, 0x2cu);
        _asm {
            push ds
            pop es
            push ds
            pop fs
            mov si, offset mem
            add si, 9000h
            mov di, offset mem
            add di, 9200h
            call txt_1ABAE
            mov si_after, si
            mov di_after, di
            mov ax_after, ax
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + 0x9000u)));
        di_after = (unsigned)(0x2840u + (di_after - ((unsigned)mem + 0x9200u)));
        printf("ax=%04x si=%04x di=%04x data=", ax_after, si_after, di_after);
        print_bytes(mem + 0x9200u, 0x2cu);
        printf("\n");
        return 0;
    }

    if (streq(op, "abiputmessage")) {
        const char *symbol;
        const char *text;
        unsigned attr;
        unsigned si_after;
        unsigned di_after;
        unsigned ax_after;
        size_t len;
        if (argc != 5) return 2;
        symbol = argv[2];
        text = argv[3];
        attr = (unsigned)parse_u32(argv[4]) & 0xffu;
        len = strlen(text);
        if (len == 0 || len > 64u) return 2;
        memset(mem + 0x9200u, 0, len * 2u);
        if (streq(symbol, "put_message")) {
            memcpy(mem + 0x9000u, text, len + 1u);
            _asm {
                push ds
                pop es
                mov ax, attr
                shl ax, 8
                mov si, offset mem
                add si, 9000h
                mov di, offset mem
                add di, 9200h
                call put_message
                mov si_after, si
                mov di_after, di
                mov ax_after, ax
            }
        } else if (streq(symbol, "put_message2")) {
            memcpy(mem + 0x9000u, text + 1u, len);
            _asm {
                push ds
                pop es
                push ds
                pop fs
                mov ax, attr
                shl ax, 8
                mov si, offset mem
                add si, 9000h
                mov di, offset mem
                add di, 9200h
                mov bx, text
                mov al, [bx]
                call put_message2
                mov si_after, si
                mov di_after, di
                mov ax_after, ax
            }
        } else {
            return 2;
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + 0x9000u)));
        di_after = (unsigned)(0x2840u + (di_after - ((unsigned)mem + 0x9200u)));
        printf("ax=%04x si=%04x di=%04x data=", ax_after, si_after, di_after);
        print_bytes(mem + 0x9200u, len * 2u);
        printf("\n");
        return 0;
    }

    if (streq(op, "abitext1bf69")) {
        unsigned attr;
        unsigned count;
        unsigned si_after;
        unsigned di_after;
        unsigned ax_after;
        if (argc != 5) return 2;
        count = (unsigned)parse_hex_bytes(argv[2], mem + 0x9000u, 0x100u);
        mem[0x9000u + count] = 0;
        attr = (unsigned)parse_u32(argv[3]) & 0xffu;
        memset(mem + 0x9200u, 0, 0x100u);
        _asm {
            push bp
            push ds
            pop es
            mov ax, attr
            shl ax, 8
            xor bp, bp
            mov si, offset mem
            add si, 9000h
            mov di, offset mem
            add di, 9200h
            call text_1BF69
            pop bp
            mov si_after, si
            mov di_after, di
            mov ax_after, ax
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + 0x9000u)));
        di_after = (unsigned)(0x2840u + (di_after - ((unsigned)mem + 0x9200u)));
        printf("ax=%04x si=%04x di=%04x data=", ax_after, si_after, di_after);
        print_bytes(mem + 0x9200u, (size_t)parse_u32(argv[4]));
        printf("\n");
        return 0;
    }

    if (streq(op, "abiwritescr")) {
        const char *text;
        unsigned attr;
        unsigned delta;
        unsigned si_after;
        unsigned di_after;
        unsigned ax_after;
        size_t len;
        if (argc != 5) return 2;
        text = argv[2];
        attr = (unsigned)parse_u32(argv[3]) & 0xffu;
        delta = (unsigned)parse_u32(argv[4]) & 0xffffu;
        len = strlen(text);
        if (len == 0 || len > 64u) return 2;
        mem[0x9000u] = (db)delta;
        mem[0x9001u] = (db)(delta >> 8);
        mem[0x9002u] = (db)attr;
        memcpy(mem + 0x9003u, text, len + 1u);
        memset(mem + 0x9200u, 0, 0x200u);
        _asm {
            push bp
            push ds
            pop es
            mov si, offset mem
            add si, 9000h
            mov di, offset mem
            add di, 9200h
            call write_scr
            pop bp
            mov si_after, si
            mov di_after, di
            mov ax_after, ax
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + 0x9000u)));
        di_after = (unsigned)(0x2840u + (di_after - ((unsigned)mem + 0x9200u)));
        printf("ax=%04x si=%04x di=%04x data=", ax_after, si_after, di_after);
        print_bytes(mem + 0x9200u + delta, len * 2u);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimyitoa10")) {
        const char *symbol;
        unsigned long value;
        unsigned si_after;
        unsigned cx_after;
        if (argc != 4) return 2;
        symbol = argv[2];
        value = parse_u32(argv[3]);
        if (streq(symbol, "my_i32toa10")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_i32toa10
                mov si_after, si
                mov cx_after, cx
            }
        } else if (streq(symbol, "my_i16toa10")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_i16toa10
                mov si_after, si
                mov cx_after, cx
            }
        } else if (streq(symbol, "my_i8toa10")) {
            _asm {
                mov eax, value
                mov si, offset mem
                add si, 9100h
                call my_i8toa10
                mov si_after, si
                mov cx_after, cx
            }
        } else {
            return 2;
        }
        si_after = (unsigned)(0x2800u + (si_after - ((unsigned)mem + ORIG_DST_OFF)));
        printf("cx=%04x si=%04x data=", cx_after, si_after);
        print_bytes(mem + ORIG_DST_OFF, cx_after);
        printf("\n");
        return 0;
    }

    return 2;
}
