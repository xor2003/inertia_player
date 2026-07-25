#include <stdio.h>
#include <string.h>

#define main iplay_player_original_main
#include "iplay_player.c"
#undef main

typedef struct PlayerHwCapture {
    dw ports[48];
    db values[48];
    unsigned writes;
    dw last_port;
    db last_value;
    unsigned read_status_reads;
    unsigned write_ready_reads;
    unsigned long physical;
    unsigned copy_seg;
    unsigned copy_off;
    db copied[16];
    db copied_tail[2];
    dw copied_bytes;
    unsigned audio_copies;
    unsigned long audio_bytes;
    unsigned long audio_checksum;
    db audio_first[2];
    db audio_tail[2];
    unsigned text_copies;
    unsigned text_seg;
    unsigned text_off;
    dw text_bytes;
    unsigned long text_checksum;
    unsigned text_nonblank;
    db text_first[2];
    db text_tail[2];
    db text_level_left[32];
    db text_level_right[32];
    unsigned text_has_module;
    unsigned text_has_sb16;
    unsigned text_has_playback;
    unsigned text_has_title;
    unsigned long manual_ticks;
    int keyboard_hit;
    unsigned keyboard_after_audio_copies;
} PlayerHwCapture;

static PlayerHwCapture hw_capture;
static dw hw_mock_sb_base = IPLAY_SB16_DEFAULT_BASE;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void hw_capture_write(dw port, db value) {
    hw_capture.last_port = port;
    hw_capture.last_value = value;
    if (hw_capture.writes < sizeof(hw_capture.values)) {
        hw_capture.ports[hw_capture.writes] = port;
        hw_capture.values[hw_capture.writes] = value;
    }
    ++hw_capture.writes;
}

static db hw_mock_port_read(dw port) {
    if (port == sb16_dsp_read_status_port(hw_mock_sb_base)) {
        ++hw_capture.read_status_reads;
        return IPLAY_SB16_DSP_READ_READY_MASK;
    }
    if (port == sb16_dsp_write_data_port(hw_mock_sb_base)) {
        ++hw_capture.write_ready_reads;
        return 0;
    }
    if (port == sb16_dsp_read_data_port(hw_mock_sb_base)) return IPLAY_SB16_DSP_RESET_ACK;
    return 0;
}

static int hw_text_contains(const db *cells, dw byte_count, const char *needle) {
    dw pos;
    unsigned matched;
    if (!needle || !needle[0]) return 1;
    for (pos = 0; (dw)(pos + 1u) < byte_count; pos = (dw)(pos + 2u)) {
        matched = 0;
        while (needle[matched]) {
            dw cell_pos = (dw)(pos + (dw)(matched * 2u));
            if ((dw)(cell_pos + 1u) >= byte_count) break;
            if (cells[cell_pos] != (db)needle[matched]) break;
            ++matched;
        }
        if (!needle[matched]) return 1;
    }
    return 0;
}

static void hw_mock_port_write(dw port, db value) {
    hw_capture_write(port, value);
}

static unsigned long hw_mock_far_physical(const void far *ptr) {
    (void)ptr;
    hw_capture.physical = 0x12340ul;
    return hw_capture.physical;
}

static void hw_mock_copy_to_far(void far *dst, const void *src, dw byte_count) {
    dw i;
    const db *bytes = (const db *)src;
    hw_capture.copy_seg = FP_SEG(dst);
    hw_capture.copy_off = FP_OFF(dst);
    hw_capture.copied_bytes = byte_count;
    for (i = 0; i < byte_count && i < sizeof(hw_capture.copied); ++i) hw_capture.copied[i] = bytes[i];
    if (byte_count >= 2u) {
        hw_capture.copied_tail[0] = bytes[(dw)(byte_count - 2u)];
        hw_capture.copied_tail[1] = bytes[(dw)(byte_count - 1u)];
    }
    if (hw_capture.copy_seg == IPLAY_DOS_TEXT_COLOR_SEG || hw_capture.copy_seg == IPLAY_DOS_TEXT_MONO_SEG) {
        ++hw_capture.text_copies;
        hw_capture.text_seg = hw_capture.copy_seg;
        hw_capture.text_off = hw_capture.copy_off;
        hw_capture.text_bytes = byte_count;
        hw_capture.text_checksum = iplay_text_cells_checksum(bytes, byte_count);
        hw_capture.text_nonblank = iplay_text_cells_nonblank_count(bytes, byte_count);
        if (byte_count >= 2u) {
            hw_capture.text_first[0] = bytes[0];
            hw_capture.text_first[1] = bytes[1];
            hw_capture.text_tail[0] = bytes[(dw)(byte_count - 2u)];
            hw_capture.text_tail[1] = bytes[(dw)(byte_count - 1u)];
        }
        if (byte_count >= iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X50)) {
            memcpy(hw_capture.text_level_left,
                   bytes + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 48, 62),
                   sizeof(hw_capture.text_level_left));
            memcpy(hw_capture.text_level_right,
                   bytes + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 62),
                   sizeof(hw_capture.text_level_right));
        }
        hw_capture.text_has_module = (hw_text_contains(bytes, byte_count, "HWPATH.MOD") || hw_text_contains(bytes, byte_count, "LEVELS.MOD")) ? 1u : 0u;
        hw_capture.text_has_sb16 = hw_text_contains(bytes, byte_count, "sb16-stereo") ? 1u : 0u;
        hw_capture.text_has_playback = hw_text_contains(bytes, byte_count, "Playback enabled") ? 1u : 0u;
        hw_capture.text_has_title = hw_text_contains(bytes, byte_count, "Inertia Player") ? 1u : 0u;
    } else {
        if (hw_capture.audio_copies == 0u && byte_count >= 2u) {
            hw_capture.audio_first[0] = bytes[0];
            hw_capture.audio_first[1] = bytes[1];
        }
        for (i = 0; i < byte_count; ++i) hw_capture.audio_checksum += bytes[i];
        hw_capture.audio_bytes += byte_count;
        if (byte_count >= 2u) {
            hw_capture.audio_tail[0] = bytes[(dw)(byte_count - 2u)];
            hw_capture.audio_tail[1] = bytes[(dw)(byte_count - 1u)];
        }
        ++hw_capture.audio_copies;
    }
}

static unsigned long hw_mock_timer_ticks(void) {
    return hw_capture.manual_ticks + hw_capture.audio_copies;
}

static int hw_mock_keyboard_hit(void) {
    if (hw_capture.keyboard_after_audio_copies && hw_capture.audio_copies >= hw_capture.keyboard_after_audio_copies) return 1;
    return hw_capture.keyboard_hit;
}

#if IPLAY_PLAYER_ENABLE_TEXT_UI
static db far *hw_mock_text_color_memory(void) {
    return (db far *)MK_FP(IPLAY_DOS_TEXT_COLOR_SEG, 0);
}

static db far *hw_mock_text_mono_memory(void) {
    return (db far *)MK_FP(IPLAY_DOS_TEXT_MONO_SEG, 0);
}
#endif

static const DosHardwareIo hw_mock_io = {
    hw_mock_port_read,
    hw_mock_port_write,
    hw_mock_far_physical,
    hw_mock_copy_to_far,
    hw_mock_timer_ticks,
    hw_mock_keyboard_hit,
#if IPLAY_PLAYER_ENABLE_TEXT_UI
    hw_mock_text_color_memory,
    hw_mock_text_mono_memory
#endif
};

static void print_bytes(const db *p, unsigned count) {
    unsigned i;
    for (i = 0; i < count; ++i) printf("%02x", (unsigned)p[i]);
}

static void print_port_writes(void) {
    unsigned i;
    for (i = 0; i < hw_capture.writes && i < sizeof(hw_capture.values); ++i) {
        if (i != 0) putchar(',');
        printf("%03x:%02x", (unsigned)hw_capture.ports[i], (unsigned)hw_capture.values[i]);
    }
}

static void run_sb16_hw_block(void) {
    DosSb16Hardware hw = {
        IPLAY_SB16_DEFAULT_BASE,
        IPLAY_SB16_DEFAULT_IRQ,
        IPLAY_SB16_DEFAULT_DMA16,
        IPLAY_SB16_DEFAULT_RATE,
        0,
        0,
        0,
        0,
        0,
        0
    };
    static const db pcm[10] = {
        0x10, 0x00, 0x20, 0x00,
        0x30, 0x00, 0x40, 0x00,
        0x50, 0x00
    };
    memset(&hw_capture, 0, sizeof(hw_capture));
    hw_mock_sb_base = player_sb16_base_port(&hw);
    dos_hw_use_io(&hw_mock_io);
    sb16_audio_write(&hw, pcm, sizeof(pcm));
    sb16_shutdown(&hw);
    printf("detected=%u active=%u dma=%u last=%u blocks=%lu bytes=%lu copy=%u reads=%u,%u physical=%lu data=",
           (unsigned)hw.detected,
           (unsigned)hw.active,
           (unsigned)hw.dma_programmed,
           (unsigned)hw.last_block_bytes,
           (unsigned long)hw.blocks_started,
           (unsigned long)hw.bytes_written,
           (unsigned)hw_capture.copied_bytes,
           hw_capture.read_status_reads,
           hw_capture.write_ready_reads,
           hw_capture.physical);
    print_bytes(hw_capture.copied, hw_capture.copied_bytes);
    printf(" writes=");
    print_port_writes();
    printf("\n");
}

static void run_sb16_hw_two_blocks(void) {
    DosSb16Hardware hw = {
        IPLAY_SB16_DEFAULT_BASE,
        IPLAY_SB16_DEFAULT_IRQ,
        IPLAY_SB16_DEFAULT_DMA16,
        IPLAY_SB16_DEFAULT_RATE,
        0,
        0,
        0,
        0,
        0,
        0
    };
    static const db pcm_a[8] = {
        0x10, 0x00, 0x20, 0x00,
        0x30, 0x00, 0x40, 0x00
    };
    static const db pcm_b[8] = {
        0x50, 0x00, 0x60, 0x00,
        0x70, 0x00, 0x80, 0x00
    };
    memset(&hw_capture, 0, sizeof(hw_capture));
    hw_mock_sb_base = player_sb16_base_port(&hw);
    dos_hw_use_io(&hw_mock_io);
    sb16_audio_write(&hw, pcm_a, sizeof(pcm_a));
    sb16_audio_write(&hw, pcm_b, sizeof(pcm_b));
    sb16_shutdown(&hw);
    printf("detected=%u active=%u dma=%u last=%u blocks=%lu bytes=%lu copy=%u reads=%u,%u physical=%lu writes=",
           (unsigned)hw.detected,
           (unsigned)hw.active,
           (unsigned)hw.dma_programmed,
           (unsigned)hw.last_block_bytes,
           (unsigned long)hw.blocks_started,
           (unsigned long)hw.bytes_written,
           (unsigned)hw_capture.copied_bytes,
           hw_capture.read_status_reads,
           hw_capture.write_ready_reads,
           hw_capture.physical);
    print_port_writes();
    printf(" tail=");
    print_bytes(hw_capture.copied_tail, 2);
    printf("\n");
}

static void run_sb16_hw_dma6(void) {
    DosSb16Hardware hw = {
        IPLAY_SB16_DEFAULT_BASE,
        IPLAY_SB16_DEFAULT_IRQ,
        6,
        IPLAY_SB16_DEFAULT_RATE,
        0,
        0,
        0,
        0,
        0,
        0
    };
    static const db pcm[8] = {
        0x10, 0x00, 0x20, 0x00,
        0x30, 0x00, 0x40, 0x00
    };
    memset(&hw_capture, 0, sizeof(hw_capture));
    hw_mock_sb_base = player_sb16_base_port(&hw);
    dos_hw_use_io(&hw_mock_io);
    sb16_audio_write(&hw, pcm, sizeof(pcm));
    sb16_shutdown(&hw);
    printf("dma16=%u detected=%u active=%u dma=%u last=%u writes=",
           (unsigned)player_sb16_dma16(&hw),
           (unsigned)hw.detected,
           (unsigned)hw.active,
           (unsigned)hw.dma_programmed,
           (unsigned)hw.last_block_bytes);
    print_port_writes();
    printf("\n");
}

static void run_sb16_hw_dma7(void) {
    DosSb16Hardware hw = {
        IPLAY_SB16_DEFAULT_BASE,
        IPLAY_SB16_DEFAULT_IRQ,
        7,
        IPLAY_SB16_DEFAULT_RATE,
        0,
        0,
        0,
        0,
        0,
        0
    };
    static const db pcm[8] = {
        0x10, 0x00, 0x20, 0x00,
        0x30, 0x00, 0x40, 0x00
    };
    memset(&hw_capture, 0, sizeof(hw_capture));
    hw_mock_sb_base = player_sb16_base_port(&hw);
    dos_hw_use_io(&hw_mock_io);
    sb16_audio_write(&hw, pcm, sizeof(pcm));
    sb16_shutdown(&hw);
    printf("dma16=%u detected=%u active=%u dma=%u last=%u writes=",
           (unsigned)player_sb16_dma16(&hw),
           (unsigned)hw.detected,
           (unsigned)hw.active,
           (unsigned)hw.dma_programmed,
           (unsigned)hw.last_block_bytes);
    print_port_writes();
    printf("\n");
}

static void run_sb16_hw_base240(void) {
    DosSb16Hardware hw = {
        0x240u,
        IPLAY_SB16_DEFAULT_IRQ,
        IPLAY_SB16_DEFAULT_DMA16,
        IPLAY_SB16_DEFAULT_RATE,
        0,
        0,
        0,
        0,
        0,
        0
    };
    static const db pcm[8] = {
        0x10, 0x00, 0x20, 0x00,
        0x30, 0x00, 0x40, 0x00
    };
    memset(&hw_capture, 0, sizeof(hw_capture));
    hw_mock_sb_base = player_sb16_base_port(&hw);
    dos_hw_use_io(&hw_mock_io);
    sb16_audio_write(&hw, pcm, sizeof(pcm));
    sb16_shutdown(&hw);
    printf("base=%03x detected=%u active=%u dma=%u last=%u reads=%u,%u writes=",
           (unsigned)player_sb16_base_port(&hw),
           (unsigned)hw.detected,
           (unsigned)hw.active,
           (unsigned)hw.dma_programmed,
           (unsigned)hw.last_block_bytes,
           hw_capture.read_status_reads,
           hw_capture.write_ready_reads);
    print_port_writes();
    printf("\n");
}

static void run_playback_timer_hw(void) {
    PlayerPlaybackLoop loop;
    PlayerPlaybackTimer timer;
    int prime;
    int ready0;
    int ready1;
    int ready2;
    int ready3;
    memset(&hw_capture, 0, sizeof(hw_capture));
    dos_hw_use_io(&hw_mock_io);
    loop.max_blocks = 0;
    loop.frames_per_block = IPLAY_PLAYER_SB16_BLOCK_FRAMES;
    loop.timer_interval_ticks = 3;
    loop.policy = IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS;
    loop.name = "hw-timer";
    hw_capture.manual_ticks = 100ul;
    player_playback_timer_init(&timer, &loop);
    prime = player_playback_timer_ready(&timer);
    ready0 = player_playback_timer_ready(&timer);
    hw_capture.manual_ticks = 102ul;
    ready1 = player_playback_timer_ready(&timer);
    hw_capture.manual_ticks = 103ul;
    ready2 = player_playback_timer_ready(&timer);
    hw_capture.manual_ticks = 109ul;
    ready3 = player_playback_timer_ready(&timer);
    printf("prime=%d ready0=%d ready1=%d ready2=%d ready3=%d last=%lu elapsed=%u count=%lu ticks=%lu interval=%u\n",
           prime,
           ready0,
           ready1,
           ready2,
           ready3,
           timer.last_ticks,
           (unsigned)timer.elapsed_ticks,
           (unsigned long)timer.ready_count,
           hw_capture.manual_ticks,
           (unsigned)timer.interval_ticks);
}

static void run_playback_continuous_loop_hw(void) {
    PlayerPlaybackLoop loop;
    player_playback_loop_init_continuous(&loop);
    printf("policy=%u max=%lu frames=%u interval=%u cadence=%s name=%s\n",
           (unsigned)player_playback_loop_policy(&loop),
           (unsigned long)player_playback_loop_max_blocks(&loop),
           (unsigned)player_playback_loop_frames_per_block(&loop),
           (unsigned)player_playback_loop_timer_interval_ticks(&loop),
           player_playback_loop_cadence_name(&loop),
           player_playback_loop_name(&loop));
}

static void run_playback_keyboard_hw(void) {
    PlayerPlaybackLoop loop;
    int before;
    int after;
    memset(&hw_capture, 0, sizeof(hw_capture));
    dos_hw_use_io(&hw_mock_io);
    player_playback_loop_init_continuous(&loop);
    before = player_playback_loop_keyboard_requested(&loop);
    hw_capture.keyboard_hit = 1;
    after = player_playback_loop_keyboard_requested(&loop);
    printf("before=%d after=%d policy=%u interval=%u\n",
           before,
           after,
           (unsigned)player_playback_loop_policy(&loop),
           (unsigned)player_playback_loop_timer_interval_ticks(&loop));
}

static dw hw_keyboard_pcm_source_read(PlayerPcmSource *source, PlayerPlaybackBlock *block) {
    player_playback_prepare_block_frames(block, player_pcm_source_frames_per_block(source));
    return player_playback_block_frames(block);
}

static void run_playback_keyboard_stop_hw(void) {
    IplayRuntime runtime;
    IplayRuntimeConfig runtime_config;
    PlayerPlayback playback;
    PlayerPcmSource source;
    PlayerPlaybackLoop loop;
    memset(&hw_capture, 0, sizeof(hw_capture));
    dos_hw_use_io(&hw_mock_io);
    player_init_core_state();
    player_init_text_presenter();
    player_configure_runtime(&runtime_config);
    player_start_runtime(&runtime, &runtime_config);
    iplay_runtime_audio_start(&runtime);
    player_playback_init(&playback);
    player_pcm_source_init(&source, hw_keyboard_pcm_source_read, 0, 0);
    player_playback_loop_init_continuous(&loop);
    hw_capture.keyboard_after_audio_copies = 1u;
    player_pump_playback_loop(&runtime, &playback, &source, &loop);
    printf("submitted=%lu submitted_frames=%lu submitted_bytes=%lu stopcode=%u audio_copies=%u audio_bytes=%lu interval=%u\n",
           (unsigned long)playback.blocks_submitted,
           (unsigned long)playback.frames_submitted,
           (unsigned long)playback.bytes_accepted,
           (unsigned)playback.stop_reason,
           hw_capture.audio_copies,
           hw_capture.audio_bytes,
           (unsigned)player_playback_loop_timer_interval_ticks(&loop));
}

static void run_playback_levels_hw(void) {
    IplayRuntime runtime;
    IplayRuntimeConfig runtime_config;
    PlayerModuleInfo module;
    db module_header[1084];
    PlayerPlayback playback;
    PlayerPcmSource source;
    PlayerPlaybackLoop loop;
    memset(&hw_capture, 0, sizeof(hw_capture));
    dos_hw_use_io(&hw_mock_io);
    player_init_core_state();
    player_init_text_presenter();
    player_configure_runtime(&runtime_config);
    player_start_runtime(&runtime, &runtime_config);
    iplay_runtime_audio_start(&runtime);
    memset(module_header, 0, sizeof(module_header));
    memcpy(module_header, "LEVELS", 6u);
    memcpy(module_header + 1080u, "M.K.", 4u);
    player_init_module_info(&module, "LEVELS.MOD", module_header, sizeof(module_header));
    player_module_set_header_len_field(&module, sizeof(module_header));
    player_module_set_size_field(&module, sizeof(module_header));
    player_detect_module_loader(&module);
    player_apply_module_type_tag(&module);
    player_render_runtime_status(&runtime, &module);
    player_playback_init(&playback);
    player_pcm_source_init(&source, hw_keyboard_pcm_source_read, 0, 0);
    player_playback_loop_init(&loop, "level-proof", IPLAY_PLAYER_LOOP_POLICY_BOUNDED, 1ul, IPLAY_PLAYER_SB16_BLOCK_FRAMES);
    player_pump_playback_loop(&runtime, &playback, &source, &loop);
    printf("levelproof blocks=%u frames=%u accepted=%u audio_copies=%u audio_bytes=%u audio_checksum=%lu audio_first=",
           (unsigned)playback.blocks_submitted,
           (unsigned)playback.frames_submitted,
           (unsigned)playback.bytes_accepted,
           hw_capture.audio_copies,
           (unsigned)hw_capture.audio_bytes,
           hw_capture.audio_checksum);
    print_bytes(hw_capture.audio_first, 2);
    printf(" audio_tail=");
    print_bytes(hw_capture.audio_tail, 2);
    printf(" text_copies=%u text_seg=%04x text_off=%04x text_bytes=%u text_checksum=%lu text_nonblank=%u ui_module=%u ui_sb16=%u ui_playback=%u stopcode=%u\n",
           hw_capture.text_copies,
           hw_capture.text_seg,
           hw_capture.text_off,
           (unsigned)hw_capture.text_bytes,
           hw_capture.text_checksum,
           hw_capture.text_nonblank,
           hw_capture.text_has_module,
           hw_capture.text_has_sb16,
           hw_capture.text_has_playback,
           (unsigned)playback.stop_reason);
    player_flush_reports();
}

static void fill_player_mod_header(db *header, unsigned header_size);

static void init_hw_mod_module(PlayerModuleInfo *module, db *header) {
    fill_player_mod_header(header, IPLAY_MOD_MIN_HEADER_BYTES);
    player_init_module_info(module, "HWPATH.MOD", header, IPLAY_MOD_MIN_HEADER_BYTES);
    module->loader = find_loader_by_kind(IPLAY_LOADER_KIND_MOD);
    module->header_len = IPLAY_MOD_MIN_HEADER_BYTES;
    module->size = IPLAY_MOD_MIN_HEADER_BYTES;
    module->module_type = loader_module_type_tag(module->loader);
}

static void run_module_keyboard_stop_hw(void) {
    IplayRuntime runtime;
    IplayRuntimeConfig runtime_config;
    PlayerPlayback playback;
    PlayerDecoderContext decoder;
    PlayerPcmSource source;
    PlayerPlaybackLoop loop;
    PlayerModuleInfo module;
    static db header[IPLAY_MOD_MIN_HEADER_BYTES];
    memset(&hw_capture, 0, sizeof(hw_capture));
    dos_hw_use_io(&hw_mock_io);
    player_init_core_state();
    player_init_text_presenter();
    init_hw_mod_module(&module, header);
    player_configure_runtime(&runtime_config);
    player_start_runtime(&runtime, &runtime_config);
    iplay_runtime_audio_start(&runtime);
    player_playback_init(&playback);
    player_decoder_context_init(&decoder, &module);
    player_module_pcm_source_init(&source, &decoder);
    player_playback_loop_init_continuous(&loop);
    hw_capture.keyboard_after_audio_copies = 1u;
    player_pump_playback_loop(&runtime, &playback, &source, &loop);
    player_decoder_context_close_file_stream(&decoder);
    printf("modulekbd blocks=%u frames=%u accepted=%u audio_copies=%u audio_bytes=%u audio_checksum=%lu audio_first=",
           (unsigned)playback.blocks_submitted,
           (unsigned)playback.frames_submitted,
           (unsigned)playback.bytes_accepted,
           hw_capture.audio_copies,
           (unsigned)hw_capture.audio_bytes,
           hw_capture.audio_checksum);
    print_bytes(hw_capture.audio_first, 2);
    printf(" audio_tail=");
    print_bytes(hw_capture.audio_tail, 2);
    printf(" text_copies=%u text_seg=%04x text_off=%04x text_bytes=%u text_checksum=%lu text_nonblank=%u checksum=%u stopcode=%u order=%u row=%u tick=%u interval=%u\n",
           hw_capture.text_copies,
           hw_capture.text_seg,
           hw_capture.text_off,
           (unsigned)hw_capture.text_bytes,
           hw_capture.text_checksum,
           hw_capture.text_nonblank,
           (unsigned)playback.pcm_checksum,
           (unsigned)playback.stop_reason,
           (unsigned)player_decoder_context_order(&decoder),
           (unsigned)player_decoder_context_row(&decoder),
           (unsigned)player_decoder_context_current_tick(&decoder),
           (unsigned)player_playback_loop_timer_interval_ticks(&loop));
    player_flush_reports();
}

#if IPLAY_PLAYER_ENABLE_TEXT_UI
static void run_text_hw_present(void) {
    DosTextPresenter presenter;
    static db cells[IPLAY_TEXT_MAX_SCREEN_BYTES];
    dw color_tail;
    dw mono_tail;
    dw project_tail;
    memset(&hw_capture, 0, sizeof(hw_capture));
    memset(cells, 0, sizeof(cells));
    cells[0] = 'C';
    cells[1] = 0x1e;
    color_tail = (dw)(iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25) - 2u);
    cells[color_tail] = 'Z';
    cells[(dw)(color_tail + 1u)] = 0x2c;
    dos_hw_use_io(&hw_mock_io);
    dos_text_presenter_init_vga_text_mode(&presenter, IPLAY_VIDEO_MODE_80X25_COLOR);
    dos_text_present(&presenter, cells, &IPLAY_TEXT_MODE_80X25, IPLAY_TEXT_MAX_SCREEN_BYTES);
    printf("color_seg=%04x color_off=%04x color_bytes=%u color_first=",
           hw_capture.copy_seg,
           hw_capture.copy_off,
           (unsigned)hw_capture.copied_bytes);
    print_bytes(hw_capture.copied, 2);
    printf(" color_tail=");
    print_bytes(hw_capture.copied_tail, 2);
    memset(&hw_capture, 0, sizeof(hw_capture));
    memset(cells, 0, sizeof(cells));
    cells[0] = 'M';
    cells[1] = 0x70;
    mono_tail = (dw)(iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_40X25) - 2u);
    cells[mono_tail] = 'W';
    cells[(dw)(mono_tail + 1u)] = 0x07;
    dos_text_presenter_init_vga_text_mode(&presenter, IPLAY_VIDEO_MODE_40X25_BW);
    dos_text_present(&presenter, cells, &IPLAY_TEXT_MODE_40X25, IPLAY_TEXT_MAX_SCREEN_BYTES);
    printf(" mono_seg=%04x mono_off=%04x mono_bytes=%u mono_first=",
           hw_capture.copy_seg,
           hw_capture.copy_off,
           (unsigned)hw_capture.copied_bytes);
    print_bytes(hw_capture.copied, 2);
    printf(" mono_tail=");
    print_bytes(hw_capture.copied_tail, 2);
    memset(&hw_capture, 0, sizeof(hw_capture));
    memset(cells, 0, sizeof(cells));
    cells[0] = 'F';
    cells[1] = 0x3a;
    project_tail = (dw)(iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X50) - 2u);
    cells[project_tail] = 'Q';
    cells[(dw)(project_tail + 1u)] = 0x5e;
    dos_text_presenter_init_vga_text_mode(&presenter, IPLAY_VIDEO_MODE_80X50_PROJECT);
    dos_text_present(&presenter, cells, &IPLAY_TEXT_MODE_80X50, IPLAY_TEXT_MAX_SCREEN_BYTES);
    printf(" project_seg=%04x project_off=%04x project_bytes=%u project_first=",
           hw_capture.copy_seg,
           hw_capture.copy_off,
           (unsigned)hw_capture.copied_bytes);
    print_bytes(hw_capture.copied, 2);
    printf(" project_tail=");
    print_bytes(hw_capture.copied_tail, 2);
    printf("\n");
}
#endif

#if IPLAY_PLAYER_ENABLE_TEXT_UI
static void fill_player_mod_header(db *header, unsigned header_size) {
    (void)header_size;
    memset(header, 0, IPLAY_MOD_MIN_HEADER_BYTES);
    memcpy(header, "HW PATH MOD", 11);
    header[20u + 22u] = 0x00;
    header[20u + 23u] = 0x02;
    header[20u + 25u] = 0x40;
    header[20u + 28u] = 0x00;
    header[20u + 29u] = 0x02;
    header[950u] = 1;
    header[952u] = 0;
    memcpy(header + 1080u, "M.K.", 4);
}

static void run_loaded_module_hw_path_mode(db video_mode) {
    IplayRuntime runtime;
    IplayRuntimeConfig runtime_config;
    IplayModuleStatus module_status;
    PlayerModuleInfo module;
    static db header[IPLAY_MOD_MIN_HEADER_BYTES];
    memset(&hw_capture, 0, sizeof(hw_capture));
    fill_player_mod_header(header, sizeof(header));
    player_init_core_state();
    player_init_text_presenter();
    player_set_text_video_mode_id(video_mode);
    dos_hw_use_io(&hw_mock_io);
    player_init_module_info(&module, "HWPATH.MOD", header, IPLAY_MOD_MIN_HEADER_BYTES);
    module.loader = find_loader_by_kind(IPLAY_LOADER_KIND_MOD);
    module.header_len = sizeof(header);
    module.size = sizeof(header);
    module.module_type = loader_module_type_tag(module.loader);
    (void)player_run_prepared_module(&module, &runtime, &runtime_config, &module_status, IPLAY_PLAYER_TRIAL_PLAYBACK_BLOCK_LIMIT);
    printf("audio_copies=%u audio_bytes=%lu audio_checksum=%lu audio_first=",
           hw_capture.audio_copies,
           hw_capture.audio_bytes,
           hw_capture.audio_checksum);
    print_bytes(hw_capture.audio_first, 2);
    printf(" audio_tail=");
    print_bytes(hw_capture.audio_tail, 2);
    printf(" text_copies=%u text_seg=%04x text_off=%04x text_bytes=%u text_checksum=%lu text_nonblank=%u text_first=",
           hw_capture.text_copies,
           hw_capture.text_seg,
           hw_capture.text_off,
           (unsigned)hw_capture.text_bytes,
           hw_capture.text_checksum,
           hw_capture.text_nonblank);
    print_bytes(hw_capture.text_first, 2);
    printf(" text_tail=");
    print_bytes(hw_capture.text_tail, 2);
    printf(" ui_module=%u ui_sb16=%u ui_playback=%u sb_detected=%u sb_active=%u sb_dma=%u sb_blocks=%lu sb_bytes=%lu writes=%u last_write=%03x:%02x\n",
           hw_capture.text_has_module,
           hw_capture.text_has_sb16,
           hw_capture.text_has_playback,
           (unsigned)player_sb16_hardware()->detected,
           (unsigned)player_sb16_hardware()->active,
           (unsigned)player_sb16_hardware()->dma_programmed,
           (unsigned long)player_sb16_hardware()->blocks_started,
           (unsigned long)player_sb16_hardware()->bytes_written,
           hw_capture.writes,
           (unsigned)hw_capture.last_port,
           (unsigned)hw_capture.last_value);
}

static void run_loaded_module_hw_path(void) {
    run_loaded_module_hw_path_mode(IPLAY_TEXT_DEFAULT_VIDEO_MODE);
}

static void run_loaded_module_hw_path_40x25_bw(void) {
    run_loaded_module_hw_path_mode(IPLAY_VIDEO_MODE_40X25_BW);
}

static void run_loaded_module_hw_path_80x25_bw(void) {
    run_loaded_module_hw_path_mode(IPLAY_VIDEO_MODE_80X25_BW);
}

static void run_loaded_module_hw_path_80x50(void) {
    run_loaded_module_hw_path_mode(IPLAY_VIDEO_MODE_80X50_PROJECT);
}

static void run_player_runtime_hw_80x50(void) {
    IplayRuntime runtime;
    IplayRuntimeConfig runtime_config;
    dw presented;
    memset(&hw_capture, 0, sizeof(hw_capture));
    player_init_core_state();
    player_init_text_presenter();
    player_set_text_video_mode_id(IPLAY_VIDEO_MODE_80X50_PROJECT);
    dos_hw_use_io(&hw_mock_io);
    player_configure_runtime(&runtime_config);
    player_start_runtime(&runtime, &runtime_config);
    iplay_runtime_render_static(&runtime, IPLAY_PLAYER_DEFAULT_ERASE_ATTR);
    presented = iplay_runtime_present(&runtime);
    printf("presented=%u text_copies=%u text_seg=%04x text_off=%04x text_bytes=%u text_checksum=%lu text_nonblank=%u text_first=",
           (unsigned)presented,
           hw_capture.text_copies,
           hw_capture.text_seg,
           hw_capture.text_off,
           (unsigned)hw_capture.text_bytes,
           hw_capture.text_checksum,
           hw_capture.text_nonblank);
    print_bytes(hw_capture.text_first, 2);
    printf(" text_tail=");
    print_bytes(hw_capture.text_tail, 2);
    printf(" title=%u sb16=%u playback=%u mode=%u\n",
           hw_capture.text_has_title,
           hw_capture.text_has_sb16,
           hw_capture.text_has_playback,
           (unsigned)player_text_video_mode_id());
}

static void run_player_runtime_hw_80x50_levels(void) {
    IplayRuntime runtime;
    IplayRuntimeConfig runtime_config;
    dw accepted;
    dw presented;
    static const db high_pcm[4] = {0xff, 0x7f, 0x00, 0x80};
    memset(&hw_capture, 0, sizeof(hw_capture));
    player_init_core_state();
    player_init_text_presenter();
    player_set_text_video_mode_id(IPLAY_VIDEO_MODE_80X50_PROJECT);
    dos_hw_use_io(&hw_mock_io);
    player_configure_runtime(&runtime_config);
    player_start_runtime(&runtime, &runtime_config);
    iplay_runtime_audio_start(&runtime);
    iplay_runtime_audio_set_capacity(&runtime, 1);
    accepted = iplay_runtime_write_sb16_frames(&runtime, high_pcm, 1);
    iplay_runtime_render_static(&runtime, IPLAY_PLAYER_DEFAULT_ERASE_ATTR);
    iplay_runtime_draw_audio_levels(&runtime, 48, 62, 16);
    presented = iplay_runtime_present(&runtime);
    printf("accepted=%u levels=%u,%u presented=%u audio_copies=%u audio_bytes=%lu text_copies=%u text_seg=%04x text_off=%04x text_bytes=%u text_checksum=%lu text_nonblank=%u text_first=",
           (unsigned)accepted,
           (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
           (unsigned)iplay_runtime_audio_levels(&runtime)->right_16,
           (unsigned)presented,
           hw_capture.audio_copies,
           hw_capture.audio_bytes,
           hw_capture.text_copies,
           hw_capture.text_seg,
           hw_capture.text_off,
           (unsigned)hw_capture.text_bytes,
           hw_capture.text_checksum,
           hw_capture.text_nonblank);
    print_bytes(hw_capture.text_first, 2);
    printf(" text_tail=");
    print_bytes(hw_capture.text_tail, 2);
    printf(" level_l=");
    print_bytes(hw_capture.text_level_left, sizeof(hw_capture.text_level_left));
    printf(" level_r=");
    print_bytes(hw_capture.text_level_right, sizeof(hw_capture.text_level_right));
    printf(" title=%u mode=%u\n",
           hw_capture.text_has_title,
           (unsigned)player_text_video_mode_id());
}
#endif

int main(int argc, char **argv) {
    if (argc != 2) return 2;
    if (streq(argv[1], "playersb16hwblock")) {
        run_sb16_hw_block();
        return 0;
    }
    if (streq(argv[1], "playersb16hwtwoblocks")) {
        run_sb16_hw_two_blocks();
        return 0;
    }
    if (streq(argv[1], "playersb16hwdma6")) {
        run_sb16_hw_dma6();
        return 0;
    }
    if (streq(argv[1], "playersb16hwdma7")) {
        run_sb16_hw_dma7();
        return 0;
    }
    if (streq(argv[1], "playersb16hwbase240")) {
        run_sb16_hw_base240();
        return 0;
    }
    if (streq(argv[1], "playerplaybacktimerhw")) {
        run_playback_timer_hw();
        return 0;
    }
    if (streq(argv[1], "playercontinuousloophw")) {
        run_playback_continuous_loop_hw();
        return 0;
    }
    if (streq(argv[1], "playerkeyboardhw")) {
        run_playback_keyboard_hw();
        return 0;
    }
    if (streq(argv[1], "playerkeyboardstophw")) {
        run_playback_keyboard_stop_hw();
        return 0;
    }
    if (streq(argv[1], "playerplaybacklevelshw")) {
        run_playback_levels_hw();
        return 0;
    }
    if (streq(argv[1], "playermodulekeyboardstophw")) {
        run_module_keyboard_stop_hw();
        return 0;
    }
#if IPLAY_PLAYER_ENABLE_TEXT_UI
    if (streq(argv[1], "playertexthwpresent")) {
        run_text_hw_present();
        return 0;
    }
    if (streq(argv[1], "plhw25")) {
        run_loaded_module_hw_path();
        return 0;
    }
    if (streq(argv[1], "plhw40")) {
        run_loaded_module_hw_path_40x25_bw();
        return 0;
    }
    if (streq(argv[1], "plhw8b")) {
        run_loaded_module_hw_path_80x25_bw();
        return 0;
    }
    if (streq(argv[1], "plhw50")) {
        run_loaded_module_hw_path_80x50();
        return 0;
    }
    if (streq(argv[1], "playerruntimehw80x50")) {
        run_player_runtime_hw_80x50();
        return 0;
    }
    if (streq(argv[1], "playerruntimehw80x50levels")) {
        run_player_runtime_hw_80x50_levels();
        return 0;
    }
#endif
    return 2;
}
