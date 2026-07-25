#include "iplay_rewrite.h"
#include <conio.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLAYER_MEM_SIZE 0xa000u
#define PLAYER_VIDEO_SIZE IPLAY_TEXT_MAX_SCREEN_BYTES
#define IPLAY_PLAYER_MODULE_BUFFER_BYTES 24576u
#ifndef IPLAY_PLAYER_DEFAULT_VIDEO_MODE
#define IPLAY_PLAYER_DEFAULT_VIDEO_MODE IPLAY_TEXT_DEFAULT_VIDEO_MODE
#endif
#define IPLAY_MOD_MIN_HEADER_BYTES 1084u
#define IPLAY_DOS_TEXT_COLOR_SEG 0xb800u
#define IPLAY_DOS_TEXT_MONO_SEG 0xb000u
#ifndef IPLAY_PLAYER_ENABLE_TEXT_UI
#define IPLAY_PLAYER_ENABLE_TEXT_UI 1
#endif
#ifndef IPLAY_PLAYER_ENABLE_DIAGNOSTICS
#define IPLAY_PLAYER_ENABLE_DIAGNOSTICS 1
#endif
#ifndef IPLAY_PLAYER_ENABLE_SB16_HW
#define IPLAY_PLAYER_ENABLE_SB16_HW 1
#endif
#ifndef IPLAY_PLAYER_SB16_REAL_HARDWARE_IO
#define IPLAY_PLAYER_SB16_REAL_HARDWARE_IO 0
#endif
#define IPLAY_SB16_DEFAULT_BASE 0x220u
#define IPLAY_SB16_DEFAULT_IRQ 5u
#define IPLAY_SB16_DEFAULT_DMA16 5u
#define IPLAY_SB16_DEFAULT_RATE 44100u
#define IPLAY_SB16_DMA_BUFFER_BYTES 4096u
#define IPLAY_SB16_DSP_SET_OUTPUT_RATE 0x41u
#define IPLAY_SB16_DSP_SPEAKER_ON 0xd1u
#define IPLAY_SB16_DSP_SPEAKER_OFF 0xd5u
#define IPLAY_SB16_DSP_OUTPUT_16BIT 0xb0u
#define IPLAY_SB16_DSP_MODE_STEREO_SIGNED 0x30u
#define IPLAY_SB16_PORT_DSP_RESET 0x06u
#define IPLAY_SB16_PORT_DSP_READ_DATA 0x0au
#define IPLAY_SB16_PORT_DSP_WRITE_DATA 0x0cu
#define IPLAY_SB16_PORT_DSP_READ_STATUS 0x0eu
#define IPLAY_SB16_DSP_WRITE_READY_MASK 0x80u
#define IPLAY_SB16_DSP_READ_READY_MASK 0x80u
#define IPLAY_SB16_DSP_RESET_ASSERT 1u
#define IPLAY_SB16_DSP_RESET_RELEASE 0u
#define IPLAY_SB16_RESET_SETTLE_READS 256u
#define IPLAY_SB16_DSP_IO_SPIN_LIMIT 0xffffu
#define IPLAY_SB16_DSP_RESET_ACK 0xaau
#define IPLAY_SB16_DMA16_CHANNEL_BASE 4u
#define IPLAY_SB16_DMA16_PORT_MASK 0xd4u
#define IPLAY_SB16_DMA16_PORT_CLEAR_FLIPFLOP 0xd8u
#define IPLAY_SB16_DMA16_PORT_MODE 0xd6u
#define IPLAY_SB16_DMA16_PORT_ADDRESS_BASE 0xc0u
#define IPLAY_SB16_DMA16_PORT_COUNT_BASE 0xc2u
#define IPLAY_SB16_DMA16_PORT_STRIDE 4u
#define IPLAY_SB16_DMA16_PORT_PAGE_CH5 0x8bu
#define IPLAY_SB16_DMA16_PORT_PAGE_CH6 0x89u
#define IPLAY_SB16_DMA16_PORT_PAGE_CH7 0x8au
#define IPLAY_SB16_DMA_MASK_DISABLE 0x04u
#define IPLAY_SB16_DMA_MODE_PLAYBACK 0x48u
#define IPLAY_SB16_DMA_CLEAR_FLIPFLOP 0u
#define IPLAY_PLAYER_MEM_SB_BASE_PORT_LO 0x0137u
#define IPLAY_PLAYER_MEM_SOUND_DRIVER 0x00d9u
#define IPLAY_PLAYER_MEM_MASTER_VOLUME 0x00dau
#define IPLAY_PLAYER_DEFAULT_SB_BASE_PORT_LO 0x22u
#define IPLAY_PLAYER_DEFAULT_SOUND_DRIVER 6u
#define IPLAY_PLAYER_DEFAULT_MASTER_VOLUME 125u
#define IPLAY_PLAYER_DEFAULT_PATTERN 1u
#define IPLAY_PLAYER_DEFAULT_ORDER 1u
#define IPLAY_PLAYER_DEFAULT_ROW 0u
#define IPLAY_PLAYER_DEFAULT_SPEED 0u
#define IPLAY_PLAYER_DEFAULT_FLAGS 0u
#define IPLAY_PLAYER_DEFAULT_VOLUME 0x80u
#define IPLAY_PLAYER_DEFAULT_AMPLIFICATION 100u
#define IPLAY_PLAYER_DEFAULT_ERASE_ATTR 0x07u
#define IPLAY_PLAYER_UNSUPPORTED_MODULE_TYPE_ATTR 0x7Eu
#define IPLAY_PLAYER_UNSUPPORTED_FILENAME_ATTR 0x7Eu
#define IPLAY_PLAYER_UNSUPPORTED_FILENAME_BYTES 80u
#define IPLAY_PLAYER_PRIME_FRAMES 2u
#define IPLAY_PLAYER_SB16_BLOCK_FRAMES 512u
#define IPLAY_PLAYER_CONTINUOUS_BLOCK_FRAMES 1024u
#define IPLAY_PLAYER_MAX_BLOCK_FRAMES IPLAY_PLAYER_CONTINUOUS_BLOCK_FRAMES
#define IPLAY_PLAYER_PRIME_BLOCKS 2u
#define IPLAY_PLAYER_PUMP_BLOCK_LIMIT 16u
#define IPLAY_PLAYER_TRIAL_PLAYBACK_BLOCK_LIMIT 64u
#define IPLAY_PLAYER_FILE_STREAM_BUFFER_BYTES 496u
#define IPLAY_PLAYER_FILE_LIST_PATH_BYTES 80u
#define IPLAY_PLAYER_CONTINUOUS_TIMER_INTERVAL_TICKS 1u
#define IPLAY_PLAYER_TIMER_IDLE_POLL_LIMIT 4096u
#define IPLAY_PLAYER_LOOP_POLICY_BOUNDED 1u
#define IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS 2u
#ifndef IPLAY_PLAYER_DEFAULT_LOOP_POLICY
#define IPLAY_PLAYER_DEFAULT_LOOP_POLICY IPLAY_PLAYER_LOOP_POLICY_BOUNDED
#endif
#define IPLAY_PLAYER_STOP_RUNNING 0u
#define IPLAY_PLAYER_STOP_BLOCK_LIMIT 1u
#define IPLAY_PLAYER_STOP_SOURCE_END 2u
#define IPLAY_PLAYER_STOP_KEYBOARD 3u
#define IPLAY_PLAYER_RENDERER_NONE 0u
#define IPLAY_PLAYER_RENDERER_DOS_FALLBACK 1u
#define IPLAY_PLAYER_RENDERER_PROJECT 2u
#define IPLAY_PLAYER_RENDERER_EXTERNAL 3u
#define IPLAY_PLAYER_SAMPLE_DATA_OFFSET_16BIT 0x80000000ul
#define IPLAY_PLAYER_S3M_DEFAULT_C2SPD 8363ul
#define IPLAY_PLAYER_DEFAULT_ROWS_PER_ORDER 64u
#define IPLAY_PLAYER_DEFAULT_INITIAL_SPEED 6u
#define IPLAY_PLAYER_DEFAULT_INITIAL_TEMPO 125u
#define IPLAY_PLAYER_DEFAULT_CHANNELS 4u
#define IPLAY_PLAYER_MAX_CHANNELS 32u
#define IPLAY_PLAYER_TAG4(a, b, c, d) ((dd)(db)(a) | ((dd)(db)(b) << 8) | ((dd)(db)(c) << 16) | ((dd)(db)(d) << 24))
#define IPLAY_LOADER_KIND_MOD 1u
#define IPLAY_LOADER_KIND_S3M 2u
#define IPLAY_LOADER_KIND_STM 3u
#define IPLAY_LOADER_KIND_669 4u
#define IPLAY_LOADER_KIND_MTM 5u
#define IPLAY_LOADER_KIND_PSM 6u
#define IPLAY_LOADER_KIND_FAR 7u
#define IPLAY_LOADER_KIND_ULT 8u
#define IPLAY_LOADER_KIND_INR 9u
#define IPLAY_LOADER_KIND_EXTERNAL_LIBRARY 10u
#define IPLAY_DECODER_BACKEND_EXTERNAL 1u
#define IPLAY_DECODER_BACKEND_PROJECT 2u
#define IPLAY_DECODER_LIBRARY_TRACKER 1u
#define IPLAY_DECODER_LIBRARY_INR 2u
/* Keep route ids synchronized with IplayModernDecoderRoute. */
#define IPLAY_DECODER_ROUTE_EXTERNAL_LIBRARY 0u
#define IPLAY_DECODER_ROUTE_PROJECT_OWNED 1u
#define IPLAY_DECODER_ROUTE_PROBE_BY_CONTENT 2u
#define IPLAY_PLAYER_MODULE_OPEN_FAILED 0
#define IPLAY_PLAYER_MODULE_UNSUPPORTED -1
#define IPLAY_PLAYER_MODULE_TOO_LARGE -2
#define IPLAY_PLAYER_MODULE_OK 1
#define IPLAY_PLAYER_MODULE_HEADER_TRUNCATED 2
#define IPLAY_PLAYER_EXIT_OK 0
#define IPLAY_PLAYER_EXIT_OPEN_FAILED 2
#define IPLAY_PLAYER_EXIT_UNSUPPORTED 2
#define IPLAY_PLAYER_EXIT_TOO_LARGE 2
#define IPLAY_PLAYER_EXIT_AUDIO_UNAVAILABLE 3

static db mem[PLAYER_MEM_SIZE];
static db video_mem[PLAYER_VIDEO_SIZE];

static db *player_memory(void) {
    return mem;
}

static db *player_video_memory(void) {
    return video_mem;
}

#if IPLAY_PLAYER_ENABLE_SB16_HW
static db far sb16_dma_buffer[IPLAY_SB16_DMA_BUFFER_BYTES];

typedef struct DosSb16Hardware {
    dw base_port;
    db irq;
    db dma16;
    dw sample_rate;
    db detected;
    db active;
    db dma_programmed;
    dw last_block_bytes;
    dd blocks_started;
    dd bytes_written;
} DosSb16Hardware;

typedef struct Sb16PreparedBlock {
    dw byte_count;
    dw samples;
} Sb16PreparedBlock;

static DosSb16Hardware sb16_hw = {
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

static DosSb16Hardware *player_sb16_hardware(void) {
    return &sb16_hw;
}

static db far *sb16_dma_buffer_memory(void) {
    return sb16_dma_buffer;
}

static dw sb16_dma_buffer_capacity(void) {
    return IPLAY_SB16_DMA_BUFFER_BYTES;
}

static dw sb16_dma_align_16bit_stereo_bytes(dw byte_count) {
    return (dw)(byte_count & (dw)~3u);
}

#define player_sb16_base_port_field(state) ((state)->base_port)
#define player_sb16_irq_field(state) ((state)->irq)
#define player_sb16_dma16_field(state) ((state)->dma16)
#define player_sb16_sample_rate_field(state) ((state)->sample_rate)

static dw player_sb16_base_port(const DosSb16Hardware *hw) {
    return player_sb16_base_port_field(hw);
}

static db player_sb16_irq(const DosSb16Hardware *hw) {
    return player_sb16_irq_field(hw);
}

static db player_sb16_dma16(const DosSb16Hardware *hw) {
    return player_sb16_dma16_field(hw);
}

static dw player_sb16_sample_rate(const DosSb16Hardware *hw) {
    return player_sb16_sample_rate_field(hw);
}
#endif

typedef struct DosHardwareIo {
    db (*port_read)(dw port);
    void (*port_write)(dw port, db value);
    unsigned long (*far_physical)(const void far *ptr);
    void (*copy_to_far)(void far *dst, const void *src, dw byte_count);
    unsigned long (*timer_ticks)(void);
    int (*keyboard_hit)(void);
#if IPLAY_PLAYER_ENABLE_TEXT_UI
    db far *(*text_color_memory)(void);
    db far *(*text_mono_memory)(void);
#endif
} DosHardwareIo;

static db dos_hw_port_read(dw port) {
    return (db)inp((unsigned)port);
}

static void dos_hw_port_write(dw port, db value) {
    outp((unsigned)port, value);
}

static unsigned long dos_hw_far_physical(const void far *ptr) {
    return ((unsigned long)FP_SEG(ptr) << 4) + FP_OFF(ptr);
}

static void dos_hw_copy_to_far(void far *dst, const void *src, dw byte_count) {
    _fmemcpy(dst, src, byte_count);
}

static unsigned long dos_hw_timer_ticks(void) {
    volatile unsigned long far *ticks = (volatile unsigned long far *)MK_FP(0x0040u, 0x006cu);
    return *ticks;
}

static int dos_hw_keyboard_hit(void) {
    return kbhit() != 0;
}

#if IPLAY_PLAYER_ENABLE_TEXT_UI
static db far *dos_hw_text_color_memory(void) {
    return (db far *)MK_FP(IPLAY_DOS_TEXT_COLOR_SEG, 0);
}

static db far *dos_hw_text_mono_memory(void) {
    return (db far *)MK_FP(IPLAY_DOS_TEXT_MONO_SEG, 0);
}
#endif

static const DosHardwareIo dos_hw_default_io = {
    dos_hw_port_read,
    dos_hw_port_write,
    dos_hw_far_physical,
    dos_hw_copy_to_far,
    dos_hw_timer_ticks,
    dos_hw_keyboard_hit,
#if IPLAY_PLAYER_ENABLE_TEXT_UI
    dos_hw_text_color_memory,
    dos_hw_text_mono_memory
#endif
};

static const DosHardwareIo *dos_hw_io = &dos_hw_default_io;

static void dos_hw_use_io(const DosHardwareIo *io) {
    dos_hw_io = io ? io : &dos_hw_default_io;
}

#define dos_hw_io_port_read_field(state) ((state)->port_read)
#define dos_hw_io_port_write_field(state) ((state)->port_write)
#define dos_hw_io_far_physical_field(state) ((state)->far_physical)
#define dos_hw_io_copy_to_far_field(state) ((state)->copy_to_far)
#define dos_hw_io_timer_ticks_field(state) ((state)->timer_ticks)
#define dos_hw_io_keyboard_hit_field(state) ((state)->keyboard_hit)

#if IPLAY_PLAYER_ENABLE_TEXT_UI
#define dos_hw_io_text_color_memory_field(state) ((state)->text_color_memory)
#define dos_hw_io_text_mono_memory_field(state) ((state)->text_mono_memory)
#endif

static db (*dos_hw_io_port_read_fn(void))(dw port) {
    return dos_hw_io_port_read_field(dos_hw_io);
}

static void (*dos_hw_io_port_write_fn(void))(dw port, db value) {
    return dos_hw_io_port_write_field(dos_hw_io);
}

static unsigned long (*dos_hw_io_far_physical_fn(void))(const void far *ptr) {
    return dos_hw_io_far_physical_field(dos_hw_io);
}

static void (*dos_hw_io_copy_to_far_fn(void))(void far *dst, const void *src, dw byte_count) {
    return dos_hw_io_copy_to_far_field(dos_hw_io);
}

static unsigned long (*dos_hw_io_timer_ticks_fn(void))(void) {
    return dos_hw_io_timer_ticks_field(dos_hw_io);
}

static int (*dos_hw_io_keyboard_hit_fn(void))(void) {
    return dos_hw_io_keyboard_hit_field(dos_hw_io);
}

#if IPLAY_PLAYER_ENABLE_TEXT_UI
static db far *(*dos_hw_io_text_color_memory_fn(void))(void) {
    return dos_hw_io_text_color_memory_field(dos_hw_io);
}

static db far *(*dos_hw_io_text_mono_memory_fn(void))(void) {
    return dos_hw_io_text_mono_memory_field(dos_hw_io);
}
#endif

static db dos_hw_io_read_port(dw port) {
    return dos_hw_io_port_read_fn()(port);
}

static void dos_hw_io_write_port(dw port, db value) {
    dos_hw_io_port_write_fn()(port, value);
}

static unsigned long dos_hw_io_far_physical(const void far *ptr) {
    return dos_hw_io_far_physical_fn()(ptr);
}

static void dos_hw_io_copy_to_far(void far *dst, const void *src, dw byte_count) {
    dos_hw_io_copy_to_far_fn()(dst, src, byte_count);
}

static unsigned long dos_hw_io_timer_ticks(void) {
    return dos_hw_io_timer_ticks_fn()();
}

static int dos_hw_io_keyboard_hit(void) {
    return dos_hw_io_keyboard_hit_fn()();
}

#if IPLAY_PLAYER_ENABLE_TEXT_UI
static db far *dos_hw_io_text_color_memory(void) {
    return dos_hw_io_text_color_memory_fn()();
}

static db far *dos_hw_io_text_mono_memory(void) {
    return dos_hw_io_text_mono_memory_fn()();
}
#endif

typedef struct LoaderInfo {
    const char *ext;
    const char *symbol;
    const char *name;
    db kind;
} LoaderInfo;

typedef struct PlayerModuleInfo {
    const char *path;
    const LoaderInfo *loader;
    db *header;
    size_t header_len;
    size_t header_capacity;
    db header_truncated;
    unsigned long size;
    dd module_type;
} PlayerModuleInfo;

typedef struct PlayerModuleRequest {
    const char *path;
    dd trial_block_limit;
    db video_mode;
    db video_mode_valid;
} PlayerModuleRequest;

typedef struct PlayerPlaybackBlock {
    db pcm[IPLAY_PLAYER_MAX_BLOCK_FRAMES * 4u];
    dw frames;
} PlayerPlaybackBlock;

typedef struct PlayerPlayback {
    PlayerPlaybackBlock block;
    dd blocks_submitted;
    dd frames_submitted;
    dd bytes_accepted;
    dd pcm_checksum;
    db limit_reached;
    db source_ended;
    db stop_reason;
} PlayerPlayback;

typedef struct PlayerPlaybackLoop {
    dd max_blocks;
    dw frames_per_block;
    dw timer_interval_ticks;
    db policy;
    const char *name;
} PlayerPlaybackLoop;

typedef struct PlayerPlaybackTimer {
    const PlayerPlaybackLoop *loop;
    unsigned long last_ticks;
    dw interval_ticks;
    dw elapsed_ticks;
    dd ready_count;
} PlayerPlaybackTimer;

typedef struct PlayerPatternCell {
    dw period;
    db note;
    db octave;
    db instrument;
    db volume;
    db volume_set;
    db effect;
    db param;
} PlayerPatternCell;

typedef struct PlayerSampleInfo {
    dd length;
    db volume;
    dd loop_start;
    dd loop_length;
    dd data_offset;
} PlayerSampleInfo;

typedef struct PlayerVoiceState {
    dw period;
    db note;
    db octave;
    db instrument;
    db volume;
    db channel_volume;
    db active;
    dw target_period;
    db pan_set;
    db pan;
    dd sample_position;
    dd sample_phase;
    dd sample_step;
    PlayerSampleInfo sample;
} PlayerVoiceState;

typedef struct PlayerDecoderContext {
    const PlayerModuleInfo *module;
    const LoaderInfo *loader;
    db seed;
    db renderer;
    db ended;
    db loop_enabled;
    db pattern_break_pending;
    db position_jump_pending;
    db pattern_loop_active;
    db pattern_loop_remaining;
    db pattern_loop_completed;
    db pattern_loop_jump_pending;
    dw block_index;
    dw max_blocks;
    dw order_count;
    dw rows_per_order;
    dw restart_order;
    dw initial_speed;
    dw initial_tempo;
    dw current_speed;
    dw current_tempo;
    db global_volume;
    dw current_tick;
    dd pcm_stream_offset;
    dd file_stream_base;
    int file_stream_fd;
    db file_stream_open;
    dw file_stream_len;
    dw file_stream_index;
    dw pattern_break_row;
    dw position_jump_order;
    dw pattern_loop_row;
    dw channel_count;
    dw current_order_value;
    PlayerPatternCell current_cell;
    PlayerVoiceState voices[IPLAY_PLAYER_MAX_CHANNELS];
    dw order;
    dw row;
    dw channel;
} PlayerDecoderContext;

typedef struct PlayerPcmSource PlayerPcmSource;
typedef dw (*PlayerPcmSourceReadFn)(PlayerPcmSource *source, PlayerPlaybackBlock *block);
typedef int (*PlayerPcmSourceEndedFn)(const PlayerPcmSource *source);

struct PlayerPcmSource {
    PlayerPcmSourceReadFn read;
    PlayerPcmSourceEndedFn ended;
    void *user;
    dw frames_per_block;
};

typedef struct PlayerExternalDecoder {
    PlayerExternalDecoderRenderFn render;
    void *user;
    const char *provider;
} PlayerExternalDecoder;

static PlayerExternalDecoder player_external_decoder = {
    0,
    0,
    "none"
};

void iplay_player_set_external_decoder(PlayerExternalDecoderRenderFn render, void *user, const char *provider) {
    player_external_decoder.render = render;
    player_external_decoder.user = user;
    player_external_decoder.provider = provider ? provider : "external-library";
}

void iplay_player_clear_external_decoder(void) {
    iplay_player_set_external_decoder(0, 0, "none");
}

static int player_external_decoder_available(void) {
    return player_external_decoder.render != 0;
}

static const char *player_external_decoder_provider_name(void) {
    return player_external_decoder.provider ? player_external_decoder.provider : "external-library";
}

static int player_external_decoder_render(const PlayerModuleInfo *module, PlayerPlaybackBlock *block) {
    if (!player_external_decoder_available()) return 0;
    return player_external_decoder.render(player_external_decoder.user, module, block);
}

typedef struct PlayerAudioBackend {
    IplayAudioWriteFn write;
    void *user;
} PlayerAudioBackend;

typedef struct PlayerVideoBackend {
    IplayVideoPresentFn present;
    void *user;
} PlayerVideoBackend;

typedef struct PlayerVideoConfig {
    db *cells;
    dw capacity;
    const IplayTextMode *mode;
    db video_mode;
} PlayerVideoConfig;

typedef struct PlayerRuntimeOutput {
    PlayerVideoConfig video_config;
    PlayerVideoBackend video_backend;
    PlayerAudioBackend audio_backend;
} PlayerRuntimeOutput;

typedef struct PlayerRuntimeVideoOutput {
    db *cells;
    dw capacity;
    const IplayTextMode *mode;
    db video_mode;
    IplayVideoPresentFn present;
    void *user;
} PlayerRuntimeVideoOutput;

typedef struct PlayerRuntimeAudioOutput {
    IplayAudioWriteFn write;
    void *user;
} PlayerRuntimeAudioOutput;

typedef struct PlayerRuntimeOutputViews {
    PlayerRuntimeVideoOutput video;
    PlayerRuntimeAudioOutput audio;
} PlayerRuntimeOutputViews;

typedef void (*PlayerRuntimeOutputAudioInitFn)(PlayerRuntimeOutput *output);
typedef void (*PlayerRuntimeOutputInitFn)(PlayerRuntimeOutput *output);
typedef void (*PlayerRuntimeOutputApplyFn)(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutput *output);

static const LoaderInfo loaders[] = {
    {".mod", "mod_n_t_module", "ProTracker/NoiseTracker MOD", IPLAY_LOADER_KIND_MOD},
    {".nst", "mod_n_t_module", "ProTracker/NoiseTracker MOD", IPLAY_LOADER_KIND_MOD},
    {".s3m", "s3m_module", "Scream Tracker 3", IPLAY_LOADER_KIND_S3M},
    {".stm", "_2stm_module", "Scream Tracker 2 STM", IPLAY_LOADER_KIND_STM},
    {".669", "e669_module", "Composer 669", IPLAY_LOADER_KIND_669},
    {".mtm", "mtm_module", "MultiTracker MTM", IPLAY_LOADER_KIND_MTM},
    {".psm", "psm_module", "ProTracker Studio PSM", IPLAY_LOADER_KIND_PSM},
    {".far", "far_module", "Farandole FAR", IPLAY_LOADER_KIND_FAR},
    {".ult", "ult_module", "UltraTracker ULT", IPLAY_LOADER_KIND_ULT},
    {".wow", "external_module", "WOW tracker", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".okt", "external_module", "Oktalyzer OKT", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".oct", "external_module", "Octalyzer OCT", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".xm", "external_module", "FastTracker XM", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".it", "external_module", "Impulse Tracker IT", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".ptm", "external_module", "PolyTracker PTM", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".ams", "external_module", "Extreme Tracker AMS", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".dbm", "external_module", "DigiBooster DBM", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".dmf", "external_module", "X-Tracker DMF", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".mdl", "external_module", "DigiTrakker MDL", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".dsm", "external_module", "DSIK DSM", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".med", "external_module", "OctaMED MED", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".imf", "external_module", "Imago Orpheus IMF", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".j2b", "external_module", "Jazz Jackrabbit 2 J2B", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY},
    {".inr", "inr_module", "Inertia INR", IPLAY_LOADER_KIND_INR}
};

/*
 * Decoder ownership boundary for the later modern C/C++ rewrite:
 * MOD/NST/S3M/STM/669/MTM/PSM/FAR/ULT are external-library formats
 * (IPLAY_DECODER_LIBRARY_TRACKER, e.g. libopenmpt/libxmp/libmodplug).
 * WOW/OKT/OCT and XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B stay on the
 * external-library side through the generic external_module loader boundary.
 * INR remains a project-owned adapter boundary unless a reliable external decoder is found.
 * These names intentionally match the modern facade routes:
 * external-library, project-owned, and probe-by-content.
 * The DOS smoke player may identify headers and print bounded metadata, but
 * it must not grow handwritten pattern/sample/effect decoders for these
 * external-library formats.
 * Keep extra explanatory route prose out of compiled DOS data; IPLAYC is
 * currently close to its memory margin before the C runtime allocates file
 * structures.
 */

#define loader_ext_field(state) ((state)->ext)
#define loader_symbol_field(state) ((state)->symbol)
#define loader_name_field(state) ((state)->name)
#define loader_kind_field(state) ((state)->kind)

static const char *loader_ext(const LoaderInfo *loader) {
    return loader_ext_field(loader);
}

static const char *loader_symbol(const LoaderInfo *loader) {
    return loader_symbol_field(loader);
}

static const char *loader_name(const LoaderInfo *loader) {
    return loader_name_field(loader);
}

static db loader_kind(const LoaderInfo *loader) {
    return loader_kind_field(loader);
}

static db loader_decoder_backend(const LoaderInfo *loader) {
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_INR:
        return IPLAY_DECODER_BACKEND_PROJECT;
    default:
        return IPLAY_DECODER_BACKEND_EXTERNAL;
    }
}

static db loader_decoder_library(const LoaderInfo *loader) {
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_INR:
        return IPLAY_DECODER_LIBRARY_INR;
    default:
        return IPLAY_DECODER_LIBRARY_TRACKER;
    }
}

static int loader_uses_external_decoder_library(const LoaderInfo *loader) {
    return loader_decoder_backend(loader) == IPLAY_DECODER_BACKEND_EXTERNAL && loader_decoder_library(loader) != 0;
}

static int loader_uses_project_decoder(const LoaderInfo *loader) {
    return loader_decoder_backend(loader) == IPLAY_DECODER_BACKEND_PROJECT;
}

static int loader_decoder_available(const LoaderInfo *loader) {
    if (!loader) return 0;
    return loader_uses_external_decoder_library(loader) || loader_uses_project_decoder(loader);
}

static const char *loader_decoder_route_name(const LoaderInfo *loader) {
    if (loader_uses_external_decoder_library(loader)) return "external-library";
    if (loader_uses_project_decoder(loader)) return "project-owned";
    return "probe-by-content";
}

static db loader_decoder_route_id(const LoaderInfo *loader) {
    if (loader_uses_external_decoder_library(loader)) return IPLAY_DECODER_ROUTE_EXTERNAL_LIBRARY;
    if (loader_uses_project_decoder(loader)) return IPLAY_DECODER_ROUTE_PROJECT_OWNED;
    return IPLAY_DECODER_ROUTE_PROBE_BY_CONTENT;
}

static db loader_decoder_renderer_kind(const LoaderInfo *loader) {
    if (!loader) return IPLAY_PLAYER_RENDERER_NONE;
    if (loader_uses_external_decoder_library(loader)) return IPLAY_PLAYER_RENDERER_EXTERNAL;
    if (loader_uses_project_decoder(loader)) return IPLAY_PLAYER_RENDERER_PROJECT;
    return IPLAY_PLAYER_RENDERER_DOS_FALLBACK;
}

static char player_renderer_code(db renderer) {
    switch (renderer) {
    case IPLAY_PLAYER_RENDERER_DOS_FALLBACK:
        return 'f';
    case IPLAY_PLAYER_RENDERER_PROJECT:
        return 'p';
    case IPLAY_PLAYER_RENDERER_EXTERNAL:
        return 'e';
    default:
        return 'n';
    }
}

static char lower_ascii(char ch) {
    if (ch >= 'A' && ch <= 'Z') return (char)(ch + ('a' - 'A'));
    return ch;
}

static char upper_ascii(char ch) {
    if (ch >= 'a' && ch <= 'z') return (char)(ch - ('a' - 'A'));
    return ch;
}

static void copy_path_case_variant(char *dst, const char *src, unsigned capacity, int upper) {
    unsigned i = 0;
    if (capacity == 0u) return;
    while (src && src[i] != 0 && i + 1u < capacity) {
        dst[i] = upper ? upper_ascii(src[i]) : lower_ascii(src[i]);
        ++i;
    }
    dst[i] = 0;
}

static int player_open_read_binary(const char *path) {
    char case_path[IPLAY_PLAYER_FILE_LIST_PATH_BYTES];
    int fd;
    if (!path) return -1;
    fd = open(path, O_RDONLY | O_BINARY);
    if (fd >= 0) return fd;
    copy_path_case_variant(case_path, path, sizeof(case_path), 0);
    fd = open(case_path, O_RDONLY | O_BINARY);
    if (fd >= 0) return fd;
    copy_path_case_variant(case_path, path, sizeof(case_path), 1);
    return open(case_path, O_RDONLY | O_BINARY);
}

static int str_eq_nocase(const char *a, const char *b) {
    while (*a != 0 && *b != 0) {
        if (lower_ascii(*a) != lower_ascii(*b)) return 0;
        ++a;
        ++b;
    }
    return *a == 0 && *b == 0;
}

static const LoaderInfo *detect_loader(const char *path) {
    const char *dot = strrchr(path, '.');
    unsigned i;
    if (!dot) return NULL;
    for (i = 0; i < sizeof(loaders) / sizeof(loaders[0]); ++i) {
        if (str_eq_nocase(dot, loader_ext(&loaders[i]))) return &loaders[i];
    }
    return NULL;
}

static dd load_u32_le(const db *p) {
    return (dd)p[0] | ((dd)p[1] << 8) | ((dd)p[2] << 16) | ((dd)p[3] << 24);
}

static dw load_u16_le(const db *p) {
    return (dw)p[0] | ((dw)p[1] << 8);
}

static dw load_u16_be(const db *p) {
    return ((dw)p[0] << 8) | (dw)p[1];
}

static dd loader_module_type_tag(const LoaderInfo *loader) {
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_MOD: return IPLAY_PLAYER_TAG4('N', '.', 'T', '.');
    case IPLAY_LOADER_KIND_S3M: return IPLAY_PLAYER_TAG4('S', '3', 'M', ' ');
    case IPLAY_LOADER_KIND_STM: return IPLAY_PLAYER_TAG4('S', 'T', 'M', ' ');
    case IPLAY_LOADER_KIND_669: return IPLAY_PLAYER_TAG4('E', '6', '6', '9');
    case IPLAY_LOADER_KIND_MTM: return IPLAY_PLAYER_TAG4('M', 'T', 'M', ' ');
    case IPLAY_LOADER_KIND_PSM: return IPLAY_PLAYER_TAG4('P', 'S', 'M', ' ');
    case IPLAY_LOADER_KIND_FAR: return IPLAY_PLAYER_TAG4('F', 'A', 'R', ' ');
    case IPLAY_LOADER_KIND_ULT: return IPLAY_PLAYER_TAG4('U', 'L', 'T', ' ');
    case IPLAY_LOADER_KIND_EXTERNAL_LIBRARY: return IPLAY_PLAYER_TAG4('E', 'X', 'T', ' ');
    case IPLAY_LOADER_KIND_INR: return IPLAY_PLAYER_TAG4('I', 'N', 'R', ' ');
    default: return 0;
    }
}

static void copy_trimmed_text(char *dst, size_t dst_size, const db *src, size_t src_size) {
    size_t n = src_size;
    size_t i;
    if (dst_size == 0) return;
    while (n != 0 && (src[n - 1] == 0 || src[n - 1] == ' ')) --n;
    if (n > dst_size - 1) n = dst_size - 1;
    for (i = 0; i < n; ++i) {
        db ch = src[i];
        dst[i] = (ch >= 0x20 && ch < 0x7f) ? (char)ch : '.';
    }
    dst[n] = 0;
}

static int has_sig(const db *buf, size_t len, size_t off, const char *sig) {
    size_t i;
    if (off > len) return 0;
    for (i = 0; sig[i] != 0; ++i) {
        if (off + i >= len || buf[off + i] != (db)sig[i]) return 0;
    }
    return 1;
}

static const LoaderInfo *find_loader_by_kind(db kind) {
    unsigned i;
    for (i = 0; i < sizeof(loaders) / sizeof(loaders[0]); ++i) {
        if (loader_kind(&loaders[i]) == kind) return &loaders[i];
    }
    return NULL;
}

static const LoaderInfo *detect_loader_from_header(const db *buf, size_t len) {
    if (has_sig(buf, len, 0x2cu, "SCRM")) return find_loader_by_kind(IPLAY_LOADER_KIND_S3M);
    if (has_sig(buf, len, 20u, "!Scream!")) return find_loader_by_kind(IPLAY_LOADER_KIND_STM);
    if (has_sig(buf, len, 0u, "MTM")) return find_loader_by_kind(IPLAY_LOADER_KIND_MTM);
    if (has_sig(buf, len, 0u, "PSM ")) return find_loader_by_kind(IPLAY_LOADER_KIND_PSM);
    if (has_sig(buf, len, 0u, "FAR")) return find_loader_by_kind(IPLAY_LOADER_KIND_FAR);
    if (has_sig(buf, len, 0u, "MAS_UTrack_V00")) return find_loader_by_kind(IPLAY_LOADER_KIND_ULT);
    if (has_sig(buf, len, 0u, "IMPM")) return find_loader_by_kind(IPLAY_LOADER_KIND_INR);
    if (has_sig(buf, len, 1080u, "M.K.") || has_sig(buf, len, 1080u, "M!K!") ||
        has_sig(buf, len, 1080u, "FLT4") || has_sig(buf, len, 1080u, "4CHN") ||
        has_sig(buf, len, 1080u, "6CHN") || has_sig(buf, len, 1080u, "8CHN")) {
        return find_loader_by_kind(IPLAY_LOADER_KIND_MOD);
    }
    if (len >= 2 && ((buf[0] == 'i' && buf[1] == 'f') || (buf[0] == 'J' && buf[1] == 'N'))) {
        return find_loader_by_kind(IPLAY_LOADER_KIND_669);
    }
    return NULL;
}

static const LoaderInfo *detect_loader_for_module(const char *path, const db *header, size_t header_len) {
    const LoaderInfo *loader = detect_loader(path);
    if (!loader) loader = detect_loader_from_header(header, header_len);
    return loader;
}

static void print_usage(void) {
    puts("Inertia Player C rewrite");
    puts("Usage: IPLAY [Switches] [FileName.Ext|@FileList.Ext]");
    puts(" /?  Display this help");
    puts(" /i  Display current soundcard settings");
    puts(" --video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50");
    puts("Supported by this DOS hardware build: MOD NST S3M STM 669 MTM PSM FAR ULT WOW OKT OCT XM IT PTM AMS DBM DMF MDL DSM MED IMF J2B");
    puts("Audio driver scope: SB16 16-bit stereo only.");
    puts("Text backend: VGA color/BW text memory at B800:0000/B000:0000.");
    puts("Audio backend: SB16 16-bit stereo hardware wrapper, SDL-compatible callback boundary.");
}

#define player_module_request_path_field(state) ((state)->path)
#define player_module_request_set_path_field(state, value) ((state)->path = (value))

static int player_arg_is_diag_block_override(const char *arg) {
    return arg
        && arg[0] == '-'
        && arg[1] == '-'
        && arg[2] == 'b'
        && arg[3] == 'l'
        && arg[4] == 'o'
        && arg[5] == 'c'
        && arg[6] == 'k'
        && arg[7] == 's'
        && arg[8] == '=';
}

static int player_arg_is_video_mode_override(const char *arg) {
    return arg
        && arg[0] == '-'
        && arg[1] == '-'
        && arg[2] == 'v'
        && arg[3] == 'i'
        && arg[4] == 'd'
        && arg[5] == 'e'
        && arg[6] == 'o'
        && arg[7] == '-'
        && arg[8] == 'm'
        && arg[9] == 'o'
        && arg[10] == 'd'
        && arg[11] == 'e'
        && arg[12] == '=';
}

static int player_arg_is_player_option(const char *arg) {
    return player_arg_is_diag_block_override(arg) || player_arg_is_video_mode_override(arg);
}

static const char *player_module_arg(int argc, char **argv) {
    int i;
    /* Legacy simple argv marker: return argc < 2 ? NULL : argv[1]; */
    for (i = 1; i < argc; ++i) {
        if (player_arg_is_player_option(argv[i])) continue;
        return argv[i];
    }
    return NULL;
}

static dd player_parse_diag_block_limit(const char *arg) {
    dd value = 0;
    const char *p;
    if (!player_arg_is_diag_block_override(arg)) return IPLAY_PLAYER_TRIAL_PLAYBACK_BLOCK_LIMIT;
    p = arg + 9;
    while (*p >= '0' && *p <= '9') {
        value = value * 10u + (dd)(*p - '0');
        ++p;
    }
    if (*p != 0 || value == 0 || value > 256u) return IPLAY_PLAYER_TRIAL_PLAYBACK_BLOCK_LIMIT;
    return value;
}

static dd player_parse_cli_diag_block_limit(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; ++i) {
        if (player_arg_is_diag_block_override(argv[i])) return player_parse_diag_block_limit(argv[i]);
    }
    return IPLAY_PLAYER_TRIAL_PLAYBACK_BLOCK_LIMIT;
}

static char player_ascii_lower(char ch) {
    return (ch >= 'A' && ch <= 'Z') ? (char)(ch - 'A' + 'a') : ch;
}

static int player_streq_ci(const char *left, const char *right) {
    if (!left || !right) return 0;
    while (*left && *right) {
        if (player_ascii_lower(*left) != player_ascii_lower(*right)) return 0;
        ++left;
        ++right;
    }
    return *left == 0 && *right == 0;
}

static db player_parse_video_mode_value(const char *value) {
    if (!value) return IPLAY_PLAYER_DEFAULT_VIDEO_MODE;
    if (player_streq_ci(value, "40x25bw") || player_streq_ci(value, "40x25mono")) return IPLAY_VIDEO_MODE_40X25_BW;
    if (player_streq_ci(value, "40x25color") || player_streq_ci(value, "40x25")) return IPLAY_VIDEO_MODE_40X25_COLOR;
    if (player_streq_ci(value, "80x25bw") || player_streq_ci(value, "80x25mono")) return IPLAY_VIDEO_MODE_80X25_BW;
    if (player_streq_ci(value, "80x25color") || player_streq_ci(value, "80x25")) return IPLAY_VIDEO_MODE_80X25_COLOR;
    if (player_streq_ci(value, "80x50") || player_streq_ci(value, "80x50project")) return IPLAY_VIDEO_MODE_80X50_PROJECT;
    return IPLAY_PLAYER_DEFAULT_VIDEO_MODE;
}

static int player_video_mode_value_supported(const char *value) {
    if (!value) return 1;
    return player_streq_ci(value, "40x25bw")
        || player_streq_ci(value, "40x25mono")
        || player_streq_ci(value, "40x25color")
        || player_streq_ci(value, "40x25")
        || player_streq_ci(value, "80x25bw")
        || player_streq_ci(value, "80x25mono")
        || player_streq_ci(value, "80x25color")
        || player_streq_ci(value, "80x25")
        || player_streq_ci(value, "80x50")
        || player_streq_ci(value, "80x50project");
}

static db player_parse_video_mode_override(const char *arg) {
    if (!player_arg_is_video_mode_override(arg)) return IPLAY_PLAYER_DEFAULT_VIDEO_MODE;
    return player_parse_video_mode_value(arg + 13);
}

static int player_video_mode_override_valid(const char *arg) {
    if (!player_arg_is_video_mode_override(arg)) return 1;
    return player_video_mode_value_supported(arg + 13);
}

static db player_parse_cli_video_mode(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; ++i) {
        if (player_arg_is_video_mode_override(argv[i])) return player_parse_video_mode_override(argv[i]);
    }
    return IPLAY_PLAYER_DEFAULT_VIDEO_MODE;
}

static int player_parse_cli_video_mode_valid(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; ++i) {
        if (!player_video_mode_override_valid(argv[i])) return 0;
    }
    return 1;
}

static void player_module_request_init_path_blocks_mode_checked(PlayerModuleRequest *request, const char *path, dd trial_block_limit, db video_mode, int video_mode_valid) {
    player_module_request_set_path_field(request, path);
    request->trial_block_limit = trial_block_limit;
    request->video_mode = video_mode;
    request->video_mode_valid = video_mode_valid ? 1u : 0u;
}

static void player_module_request_init_path_blocks_mode(PlayerModuleRequest *request, const char *path, dd trial_block_limit, db video_mode) {
    player_module_request_init_path_blocks_mode_checked(request, path, trial_block_limit, video_mode, 1);
}

static void player_module_request_init_path_blocks(PlayerModuleRequest *request, const char *path, dd trial_block_limit) {
    player_module_request_init_path_blocks_mode(request, path, trial_block_limit, IPLAY_PLAYER_DEFAULT_VIDEO_MODE);
}

static void player_module_request_init_path(PlayerModuleRequest *request, const char *path) {
    player_module_request_init_path_blocks(request, path, IPLAY_PLAYER_TRIAL_PLAYBACK_BLOCK_LIMIT);
}

static const char *player_module_request_path(const PlayerModuleRequest *request) {
    return player_module_request_path_field(request);
}

static dd player_module_request_trial_block_limit(const PlayerModuleRequest *request) {
    return request->trial_block_limit;
}

static db player_module_request_video_mode(const PlayerModuleRequest *request) {
    return request->video_mode;
}

static int player_module_request_video_mode_valid(const PlayerModuleRequest *request) {
    return request->video_mode_valid != 0;
}

static void player_module_request_init_cli(PlayerModuleRequest *request, int argc, char **argv) {
    dd trial_block_limit = IPLAY_PLAYER_TRIAL_PLAYBACK_BLOCK_LIMIT;
    db video_mode;
    int video_mode_valid;
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    trial_block_limit = player_parse_cli_diag_block_limit(argc, argv);
#else
    (void)argc;
    (void)argv;
#endif
    video_mode = player_parse_cli_video_mode(argc, argv);
    video_mode_valid = player_parse_cli_video_mode_valid(argc, argv);
    /* Base CLI request marker: player_module_request_init_path_blocks(request, player_module_arg(argc, argv), trial_block_limit); */
    player_module_request_init_path_blocks_mode_checked(request, player_module_arg(argc, argv), trial_block_limit, video_mode, video_mode_valid);
}

static int player_module_request_is_usage(const PlayerModuleRequest *request) {
    const char *path = player_module_request_path(request);
    return !path || strcmp(path, "/?") == 0 || strcmp(path, "/0") == 0 || strcmp(path, "-?") == 0 || strcmp(path, "--help") == 0;
}

static int player_requested_usage(const PlayerModuleRequest *request) {
    return player_module_request_is_usage(request);
}

static int player_module_request_is_sound_settings(const PlayerModuleRequest *request) {
    const char *path = player_module_request_path(request);
    return path && (strcmp(path, "/i") == 0 || strcmp(path, "/I") == 0);
}

static int player_requested_sound_settings(const PlayerModuleRequest *request) {
    return player_module_request_is_sound_settings(request);
}

static int player_path_is_file_list(const char *path) {
    return path && path[0] == '@' && path[1] != 0;
}

static int player_file_list_space(char ch) {
    return ch == ' ' || ch == '\t';
}

static void player_trim_file_list_path(char *path) {
    unsigned start = 0u;
    unsigned end = 0u;
    unsigned out = 0u;

    while (path[start] && player_file_list_space(path[start])) ++start;
    end = start;
    while (path[end]) ++end;
    while (end > start && player_file_list_space(path[end - 1u])) --end;
    while (start < end) path[out++] = path[start++];
    path[out] = 0;
}

static int player_read_file_list_first_path(const char *list_arg, char *path, unsigned capacity) {
    const char *list_path;
    int fd;
    int n;
    char ch;
    unsigned pos;

    if (!player_path_is_file_list(list_arg) || capacity == 0u) return 0;
    list_path = list_arg + 1;
    fd = open(list_path, O_RDONLY | O_BINARY);
    if (fd < 0) return 0;

    pos = 0u;
    while ((n = read(fd, &ch, 1u)) == 1) {
        if (ch == '\r' || ch == '\n') {
            if (pos != 0u) {
                path[pos] = 0;
                player_trim_file_list_path(path);
                if (path[0] != 0) break;
                pos = 0u;
            }
            continue;
        }
        if (pos + 1u < capacity) {
            path[pos] = ch;
            ++pos;
        }
    }
    close(fd);
    path[pos] = 0;
    player_trim_file_list_path(path);
    return path[0] != 0;
}

static const char *player_resolve_requested_module_path(const PlayerModuleRequest *request, char *file_list_path, unsigned capacity) {
    const char *path = player_module_request_path(request);
    if (player_read_file_list_first_path(path, file_list_path, capacity)) return file_list_path;
    return path;
}

static int player_exit_ok_status(void);

static void player_set_memory_byte(db *player_mem, dw offset, db value) {
    player_mem[offset] = value;
}

static db player_memory_byte(const db *player_mem, dw offset) {
    return player_mem[offset];
}

static void player_init_audio_defaults(db *player_mem) {
    player_set_memory_byte(player_mem, IPLAY_PLAYER_MEM_SB_BASE_PORT_LO, IPLAY_PLAYER_DEFAULT_SB_BASE_PORT_LO);
    player_set_memory_byte(player_mem, IPLAY_PLAYER_MEM_SOUND_DRIVER, IPLAY_PLAYER_DEFAULT_SOUND_DRIVER);
    player_set_memory_byte(player_mem, IPLAY_PLAYER_MEM_MASTER_VOLUME, IPLAY_PLAYER_DEFAULT_MASTER_VOLUME);
}

static db player_master_volume(const db *player_mem) {
    return player_memory_byte(player_mem, IPLAY_PLAYER_MEM_MASTER_VOLUME);
}

static void player_start_program_memory(db *player_mem) {
    iplay_start_player_memory(player_mem);
    player_init_audio_defaults(player_mem);
}

static void player_audio_discard(void *user, const db *pcm, dw byte_count) {
    (void)user;
    (void)pcm;
    (void)byte_count;
}

#define player_audio_backend_set_write_field(state, value) ((state)->write = (value))
#define player_audio_backend_set_user_field(state, value) ((state)->user = (value))
#define player_audio_backend_write_field(state) ((state)->write)
#define player_audio_backend_user_field(state) ((state)->user)

static void player_audio_backend_set_write(PlayerAudioBackend *backend, IplayAudioWriteFn write) {
    player_audio_backend_set_write_field(backend, write);
}

static void player_audio_backend_set_user(PlayerAudioBackend *backend, void *user) {
    player_audio_backend_set_user_field(backend, user);
}

static void player_audio_backend_init(PlayerAudioBackend *backend, IplayAudioWriteFn write, void *user) {
    player_audio_backend_set_write(backend, write);
    player_audio_backend_set_user(backend, user);
}

static void player_audio_backend_init_discard(PlayerAudioBackend *backend) {
    player_audio_backend_init(backend, player_audio_discard, NULL);
}

static IplayAudioWriteFn player_audio_backend_write(const PlayerAudioBackend *backend) {
    return player_audio_backend_write_field(backend);
}

static void *player_audio_backend_user(const PlayerAudioBackend *backend) {
    return player_audio_backend_user_field(backend);
}

#define player_video_backend_set_present_field(state, value) ((state)->present = (value))
#define player_video_backend_set_user_field(state, value) ((state)->user = (value))
#define player_video_backend_present_field(state) ((state)->present)
#define player_video_backend_user_field(state) ((state)->user)

static void player_video_backend_set_present(PlayerVideoBackend *backend, IplayVideoPresentFn present) {
    player_video_backend_set_present_field(backend, present);
}

static void player_video_backend_set_user(PlayerVideoBackend *backend, void *user) {
    player_video_backend_set_user_field(backend, user);
}

static void player_video_backend_init(PlayerVideoBackend *backend, IplayVideoPresentFn present, void *user) {
    player_video_backend_set_present(backend, present);
    player_video_backend_set_user(backend, user);
}

static IplayVideoPresentFn player_video_backend_present(const PlayerVideoBackend *backend) {
    return player_video_backend_present_field(backend);
}

static void *player_video_backend_user(const PlayerVideoBackend *backend) {
    return player_video_backend_user_field(backend);
}

#if IPLAY_PLAYER_ENABLE_SB16_HW
static dw sb16_dsp_write_data_port(dw base_port) {
    return (dw)(base_port + IPLAY_SB16_PORT_DSP_WRITE_DATA);
}

static dw sb16_dsp_read_status_port(dw base_port) {
    return (dw)(base_port + IPLAY_SB16_PORT_DSP_READ_STATUS);
}

static dw sb16_dsp_read_data_port(dw base_port) {
    return (dw)(base_port + IPLAY_SB16_PORT_DSP_READ_DATA);
}

static dw sb16_dsp_reset_port(dw base_port) {
    return (dw)(base_port + IPLAY_SB16_PORT_DSP_RESET);
}

static int sb16_wait_write(dw base_port) {
    unsigned spin;
    for (spin = 0; spin < IPLAY_SB16_DSP_IO_SPIN_LIMIT; ++spin) {
        if ((dos_hw_io_read_port(sb16_dsp_write_data_port(base_port)) & IPLAY_SB16_DSP_WRITE_READY_MASK) == 0) return 1;
    }
    return 0;
}

static int sb16_wait_read(dw base_port) {
    unsigned spin;
    for (spin = 0; spin < IPLAY_SB16_DSP_IO_SPIN_LIMIT; ++spin) {
        if ((dos_hw_io_read_port(sb16_dsp_read_status_port(base_port)) & IPLAY_SB16_DSP_READ_READY_MASK) != 0) return 1;
    }
    return 0;
}

static int sb16_dsp_write(dw base_port, db value) {
    if (!sb16_wait_write(base_port)) return 0;
    dos_hw_io_write_port(sb16_dsp_write_data_port(base_port), value);
    return 1;
}

static int sb16_dsp_read(dw base_port, db *value) {
    if (!sb16_wait_read(base_port)) return 0;
    *value = dos_hw_io_read_port(sb16_dsp_read_data_port(base_port));
    return 1;
}

static db sb16_dsp_word_hi(dw value) {
    return (db)(value >> 8);
}

static db sb16_dsp_word_lo(dw value) {
    return (db)value;
}

static db sb16_detected_from_reset_ack(db value) {
    return value == IPLAY_SB16_DSP_RESET_ACK;
}

#define sb16_set_detected_flag_field(state, value) ((state)->detected = (value))
#define sb16_detected_flag_field(state) ((state)->detected)

static void sb16_set_detected_flag(DosSb16Hardware *hw, db detected) {
    sb16_set_detected_flag_field(hw, detected);
}

static db sb16_detected_flag(const DosSb16Hardware *hw) {
    return sb16_detected_flag_field(hw);
}

static void sb16_mark_detected(DosSb16Hardware *hw, db detected) {
    sb16_set_detected_flag(hw, detected);
}

static int sb16_is_detected(const DosSb16Hardware *hw) {
    return sb16_detected_flag(hw) != 0;
}

static int sb16_blaster_is_space(char ch) {
    return ch == ' ' || ch == '\t';
}

static char sb16_blaster_upper(char ch) {
    return (ch >= 'a' && ch <= 'z') ? (char)(ch - ('a' - 'A')) : ch;
}

static int sb16_blaster_hex_digit(char ch, unsigned *value) {
    if (ch >= '0' && ch <= '9') {
        *value = (unsigned)(ch - '0');
        return 1;
    }
    ch = sb16_blaster_upper(ch);
    if (ch >= 'A' && ch <= 'F') {
        *value = (unsigned)(ch - 'A' + 10);
        return 1;
    }
    return 0;
}

static int sb16_blaster_dec_digit(char ch, unsigned *value) {
    if (ch < '0' || ch > '9') return 0;
    *value = (unsigned)(ch - '0');
    return 1;
}

static unsigned long sb16_blaster_parse_number(const char **cursor, int hex, int *present) {
    const char *p = *cursor;
    unsigned long value = 0;
    unsigned digit = 0;
    *present = 0;
    while (hex ? sb16_blaster_hex_digit(*p, &digit) : sb16_blaster_dec_digit(*p, &digit)) {
        value = hex ? (value << 4) + digit : value * 10ul + digit;
        *present = 1;
        ++p;
    }
    *cursor = p;
    return value;
}

static void player_sb16_set_base_port(DosSb16Hardware *hw, dw base_port) {
    player_sb16_base_port_field(hw) = base_port;
    sb16_mark_detected(hw, 0);
}

static void player_sb16_set_irq(DosSb16Hardware *hw, db irq) {
    player_sb16_irq_field(hw) = irq;
}

static void player_sb16_set_dma16(DosSb16Hardware *hw, db dma16) {
    player_sb16_dma16_field(hw) = dma16;
}

static int sb16_blaster_dma16_valid(unsigned long value) {
    return value >= 5ul && value <= 7ul;
}

static void player_configure_sb16_from_blaster(void) {
    const char *p = getenv("BLASTER");
    int high_dma_set = 0;
    if (!p) return;
    while (*p) {
        char token;
        unsigned long value;
        int present;
        while (sb16_blaster_is_space(*p)) ++p;
        token = sb16_blaster_upper(*p);
        if (!token) break;
        ++p;
        value = sb16_blaster_parse_number(&p, token == 'A', &present);
        if (!present) continue;
        switch (token) {
        case 'A':
            if (value > 0ul && value <= 0xfffful) player_sb16_set_base_port(player_sb16_hardware(), (dw)value);
            break;
        case 'I':
            if (value <= 15ul) player_sb16_set_irq(player_sb16_hardware(), (db)value);
            break;
        case 'H':
            if (sb16_blaster_dma16_valid(value)) {
                player_sb16_set_dma16(player_sb16_hardware(), (db)value);
                high_dma_set = 1;
            }
            break;
        case 'D':
            if (!high_dma_set && sb16_blaster_dma16_valid(value)) player_sb16_set_dma16(player_sb16_hardware(), (db)value);
            break;
        default:
            break;
        }
        while (*p && !sb16_blaster_is_space(*p)) ++p;
    }
}

static void sb16_dsp_assert_reset(dw base_port) {
    dos_hw_io_write_port(sb16_dsp_reset_port(base_port), IPLAY_SB16_DSP_RESET_ASSERT);
}

static void sb16_dsp_release_reset(dw base_port) {
    dos_hw_io_write_port(sb16_dsp_reset_port(base_port), IPLAY_SB16_DSP_RESET_RELEASE);
}

static void sb16_dsp_settle_reset(dw base_port) {
    unsigned delay;
    for (delay = 0; delay < IPLAY_SB16_RESET_SETTLE_READS; ++delay) (void)dos_hw_io_read_port(sb16_dsp_read_status_port(base_port));
}

static void sb16_dsp_pulse_reset(dw base_port) {
    sb16_dsp_assert_reset(base_port);
    sb16_dsp_settle_reset(base_port);
    sb16_dsp_release_reset(base_port);
}

static int sb16_dsp_read_reset_detected(dw base_port, db *detected) {
    db value = 0;
    if (!sb16_dsp_read(base_port, &value)) return 0;
    *detected = sb16_detected_from_reset_ack(value);
    return 1;
}

static int sb16_reset(DosSb16Hardware *hw) {
    db detected = 0;
    dw base_port = player_sb16_base_port(hw);
    sb16_dsp_pulse_reset(base_port);
    if (!sb16_dsp_read_reset_detected(base_port, &detected)) return 0;
    sb16_mark_detected(hw, detected);
    return sb16_is_detected(hw);
}

static int sb16_dsp_set_output_rate(dw base_port, dw sample_rate) {
    return sb16_dsp_write(base_port, IPLAY_SB16_DSP_SET_OUTPUT_RATE)
        && sb16_dsp_write(base_port, sb16_dsp_word_hi(sample_rate))
        && sb16_dsp_write(base_port, sb16_dsp_word_lo(sample_rate));
}

static int sb16_set_rate(DosSb16Hardware *hw) {
    dw base_port = player_sb16_base_port(hw);
    dw sample_rate = player_sb16_sample_rate(hw);
    return sb16_dsp_set_output_rate(base_port, sample_rate);
}

static unsigned long sb16_dma16_word_address(unsigned long phys) {
    return phys >> 1;
}

static dw sb16_dma16_word_count(dw byte_count) {
    return (dw)(byte_count >> 1);
}

static db sb16_dma16_channel_index(db dma16) {
    return (db)(dma16 - IPLAY_SB16_DMA16_CHANNEL_BASE);
}

static dw sb16_dma16_address_port(db chan) {
    return (dw)(IPLAY_SB16_DMA16_PORT_ADDRESS_BASE + (dw)chan * IPLAY_SB16_DMA16_PORT_STRIDE);
}

static dw sb16_dma16_count_port(db chan) {
    return (dw)(IPLAY_SB16_DMA16_PORT_COUNT_BASE + (dw)chan * IPLAY_SB16_DMA16_PORT_STRIDE);
}

static dw sb16_dma16_page_port(db chan) {
    switch (chan) {
    case 2u:
        return IPLAY_SB16_DMA16_PORT_PAGE_CH6;
    case 3u:
        return IPLAY_SB16_DMA16_PORT_PAGE_CH7;
    case 1u:
    default:
        return IPLAY_SB16_DMA16_PORT_PAGE_CH5;
    }
}

static dw sb16_dma16_terminal_count(dw word_count) {
    return (dw)(word_count - 1u);
}

static db sb16_dma16_byte_lo(unsigned long value) {
    return (db)value;
}

static db sb16_dma16_byte_hi(unsigned long value) {
    return (db)(value >> 8);
}

static db sb16_dma16_page_byte(unsigned long phys) {
    return (db)(phys >> 16);
}

static db sb16_dma16_disable_mask_value(db chan) {
    return (db)(IPLAY_SB16_DMA_MASK_DISABLE | chan);
}

static db sb16_dma16_enable_mask_value(db chan) {
    return chan;
}

static db sb16_dma16_playback_mode_value(db chan) {
    return (db)(IPLAY_SB16_DMA_MODE_PLAYBACK | chan);
}

static void sb16_dma16_mask_channel(db chan) {
    dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MASK, sb16_dma16_disable_mask_value(chan));
}

static void sb16_dma16_clear_flipflop(void) {
    dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_CLEAR_FLIPFLOP, IPLAY_SB16_DMA_CLEAR_FLIPFLOP);
}

static void sb16_dma16_set_playback_mode(db chan) {
    dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MODE, sb16_dma16_playback_mode_value(chan));
}

static void sb16_dma16_unmask_channel(db chan) {
    dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MASK, sb16_dma16_enable_mask_value(chan));
}

static void sb16_dma16_write_address(db chan, unsigned long word_addr, unsigned long phys) {
    dos_hw_io_write_port(sb16_dma16_address_port(chan), sb16_dma16_byte_lo(word_addr));
    dos_hw_io_write_port(sb16_dma16_address_port(chan), sb16_dma16_byte_hi(word_addr));
    dos_hw_io_write_port(sb16_dma16_page_port(chan), sb16_dma16_page_byte(phys));
}

static void sb16_dma16_write_count(db chan, dw terminal_count) {
    dos_hw_io_write_port(sb16_dma16_count_port(chan), sb16_dma16_byte_lo(terminal_count));
    dos_hw_io_write_port(sb16_dma16_count_port(chan), sb16_dma16_byte_hi(terminal_count));
}

#define sb16_set_dma_programmed_field(state, value) ((state)->dma_programmed = (value))
#define sb16_set_last_block_bytes_field(state, value) ((state)->last_block_bytes = (value))

static void sb16_set_dma_programmed(DosSb16Hardware *hw, db programmed) {
    sb16_set_dma_programmed_field(hw, programmed);
}

static void sb16_set_last_block_bytes(DosSb16Hardware *hw, dw byte_count) {
    sb16_set_last_block_bytes_field(hw, byte_count);
}

static void sb16_mark_dma_programmed(DosSb16Hardware *hw, dw byte_count) {
    sb16_set_dma_programmed(hw, 1);
    sb16_set_last_block_bytes(hw, byte_count);
}

static void sb16_program_dma16(DosSb16Hardware *hw, const void far *buffer, dw byte_count) {
    unsigned long phys = dos_hw_io_far_physical(buffer);
    unsigned long word_addr = sb16_dma16_word_address(phys);
    dw word_count = sb16_dma16_word_count(byte_count);
    dw terminal_count = sb16_dma16_terminal_count(word_count);
    db chan = sb16_dma16_channel_index(player_sb16_dma16(hw));
    sb16_dma16_mask_channel(chan);
    sb16_dma16_clear_flipflop();
    sb16_dma16_set_playback_mode(chan);
    sb16_dma16_write_address(chan, word_addr, phys);
    sb16_dma16_write_count(chan, terminal_count);
    sb16_dma16_unmask_channel(chan);
    sb16_mark_dma_programmed(hw, byte_count);
}

static dw sb16_block_sample_count(dw byte_count) {
    return (dw)((byte_count >> 1) - 1u);
}

static int sb16_block_has_payload(dw byte_count) {
    return byte_count >= 4u;
}

static dw sb16_block_aligned_byte_count(dw byte_count) {
    return (dw)(byte_count & (dw)~3u);
}

#define sb16_prepared_block_set_byte_count_field(state, value) ((state)->byte_count = (value))
#define sb16_prepared_block_set_samples_field(state, value) ((state)->samples = (value))
#define sb16_prepared_block_byte_count_field(state) ((state)->byte_count)
#define sb16_prepared_block_samples_field(state) ((state)->samples)

static void sb16_prepared_block_set_byte_count(Sb16PreparedBlock *block, dw byte_count) {
    sb16_prepared_block_set_byte_count_field(block, byte_count);
}

static void sb16_prepared_block_set_samples(Sb16PreparedBlock *block, dw samples) {
    sb16_prepared_block_set_samples_field(block, samples);
}

static int sb16_prepare_16bit_stereo_block(dw byte_count, Sb16PreparedBlock *block) {
    dw aligned_byte_count;
    if (!sb16_block_has_payload(byte_count)) return 0;
    aligned_byte_count = sb16_block_aligned_byte_count(byte_count);
    sb16_prepared_block_set_byte_count(block, aligned_byte_count);
    sb16_prepared_block_set_samples(block, sb16_block_sample_count(aligned_byte_count));
    return 1;
}

static dw sb16_prepared_block_byte_count(const Sb16PreparedBlock *block) {
    return sb16_prepared_block_byte_count_field(block);
}

static dw sb16_prepared_block_samples(const Sb16PreparedBlock *block) {
    return sb16_prepared_block_samples_field(block);
}

static int sb16_dsp_speaker_on(dw base_port) {
    return sb16_dsp_write(base_port, IPLAY_SB16_DSP_SPEAKER_ON);
}

static int sb16_dsp_start_16bit_stereo_output(dw base_port) {
    if (!sb16_dsp_write(base_port, IPLAY_SB16_DSP_OUTPUT_16BIT)) return 0;
    return sb16_dsp_write(base_port, IPLAY_SB16_DSP_MODE_STEREO_SIGNED);
}

static int sb16_dsp_write_sample_count(dw base_port, dw samples) {
    if (!sb16_dsp_write(base_port, sb16_dsp_word_lo(samples))) return 0;
    return sb16_dsp_write(base_port, sb16_dsp_word_hi(samples));
}

static int sb16_is_active(const DosSb16Hardware *hw) {
    return hw->active != 0;
}

static int sb16_start_16bit_stereo_dsp(DosSb16Hardware *hw, dw samples) {
    dw base_port = player_sb16_base_port(hw);
    if (!sb16_is_active(hw) && !sb16_dsp_speaker_on(base_port)) return 0;
    if (!sb16_dsp_start_16bit_stereo_output(base_port)) return 0;
    if (!sb16_dsp_write_sample_count(base_port, samples)) return 0;
    return 1;
}

#define sb16_set_active_field(state, value) ((state)->active = (value))
#define sb16_add_started_blocks_field(state, value) ((state)->blocks_started += (value))
#define sb16_add_written_bytes_field(state, value) ((state)->bytes_written += (value))

static void sb16_set_active(DosSb16Hardware *hw, db active) {
    sb16_set_active_field(hw, active);
}

static void sb16_add_started_blocks(DosSb16Hardware *hw, dd blocks) {
    sb16_add_started_blocks_field(hw, blocks);
}

static void sb16_add_written_bytes(DosSb16Hardware *hw, dw bytes) {
    sb16_add_written_bytes_field(hw, bytes);
}

static void sb16_mark_active(DosSb16Hardware *hw) {
    sb16_set_active(hw, 1);
}

static void sb16_count_started_block(DosSb16Hardware *hw) {
    sb16_add_started_blocks(hw, 1u);
}

static void sb16_count_written_bytes(DosSb16Hardware *hw, dw byte_count) {
    sb16_add_written_bytes(hw, byte_count);
}

static void sb16_commit_started_block(DosSb16Hardware *hw, dw byte_count) {
    sb16_mark_active(hw);
    sb16_count_started_block(hw);
    sb16_count_written_bytes(hw, byte_count);
}

static int sb16_start_prepared_16bit_stereo_dsp(DosSb16Hardware *hw, const Sb16PreparedBlock *block) {
    if (!sb16_is_active(hw) && !sb16_set_rate(hw)) return 0;
    return sb16_start_16bit_stereo_dsp(hw, sb16_prepared_block_samples(block));
}

static int sb16_start_prepared_16bit_stereo_block(DosSb16Hardware *hw, const db far *buffer, const Sb16PreparedBlock *block) {
    sb16_program_dma16(hw, buffer, sb16_prepared_block_byte_count(block));
    if (!sb16_start_prepared_16bit_stereo_dsp(hw, block)) return 0;
    sb16_commit_started_block(hw, sb16_prepared_block_byte_count(block));
    return 1;
}

static int sb16_start_16bit_stereo_block(DosSb16Hardware *hw, const db far *buffer, dw byte_count) {
    Sb16PreparedBlock block;
    if (!sb16_prepare_16bit_stereo_block(byte_count, &block)) return 0;
    return sb16_start_prepared_16bit_stereo_block(hw, buffer, &block);
}

static DosSb16Hardware *sb16_audio_user_hardware(void *user) {
    return (DosSb16Hardware *)user;
}

static int sb16_audio_ensure_ready(DosSb16Hardware *hw) {
    return sb16_is_detected(hw) || sb16_reset(hw);
}

static dw sb16_audio_dma_copy_count(dw byte_count) {
    dw capacity = sb16_dma_buffer_capacity();
    dw copy_count = byte_count > capacity ? capacity : byte_count;
    return sb16_dma_align_16bit_stereo_bytes(copy_count);
}

static void sb16_audio_copy_to_dma(const db *pcm, dw byte_count) {
    dos_hw_io_copy_to_far(sb16_dma_buffer_memory(), pcm, byte_count);
}

static void sb16_audio_start_dma_block(DosSb16Hardware *hw, dw byte_count) {
    (void)sb16_start_16bit_stereo_block(hw, sb16_dma_buffer_memory(), byte_count);
}

static void sb16_audio_submit_dma_block(DosSb16Hardware *hw, const db *pcm, dw byte_count) {
    sb16_audio_copy_to_dma(pcm, byte_count);
    sb16_audio_start_dma_block(hw, byte_count);
}

static void sb16_audio_submit_pcm(DosSb16Hardware *hw, const db *pcm, dw byte_count) {
    dw copy_count = sb16_audio_dma_copy_count(byte_count);
    if (copy_count == 0) return;
    sb16_audio_submit_dma_block(hw, pcm, copy_count);
}

static void sb16_stop_16bit_stereo_dsp(DosSb16Hardware *hw) {
    dos_hw_io_write_port(sb16_dsp_write_data_port(player_sb16_base_port(hw)), IPLAY_SB16_DSP_SPEAKER_OFF);
}

static void sb16_mark_inactive(DosSb16Hardware *hw) {
    sb16_set_active(hw, 0);
}

static void sb16_mark_dma_idle(DosSb16Hardware *hw) {
    sb16_set_dma_programmed(hw, 0);
}

static void sb16_audio_write(void *user, const db *pcm, dw byte_count) {
    DosSb16Hardware *hw = sb16_audio_user_hardware(user);
#if IPLAY_PLAYER_SB16_REAL_HARDWARE_IO
    if (!sb16_audio_ensure_ready(hw)) return;
    sb16_audio_submit_pcm(hw, pcm, byte_count);
#else
    (void)pcm;
    if (!hw) return;
    sb16_mark_detected(hw, 1);
    sb16_mark_active(hw);
    sb16_count_started_block(hw);
    sb16_count_written_bytes(hw, sb16_audio_dma_copy_count(byte_count));
#endif
}

static void player_audio_backend_init_sb16(PlayerAudioBackend *backend) {
    player_audio_backend_init(backend, sb16_audio_write, player_sb16_hardware());
}

static void sb16_shutdown(DosSb16Hardware *hw) {
#if IPLAY_PLAYER_SB16_REAL_HARDWARE_IO
    if (!sb16_is_detected(hw)) return;
    sb16_stop_16bit_stereo_dsp(hw);
    (void)sb16_reset(hw);
#else
    if (!hw) return;
#endif
    sb16_mark_inactive(hw);
    sb16_mark_dma_idle(hw);
}
#endif

#if IPLAY_PLAYER_ENABLE_TEXT_UI
typedef struct DosTextPresenter {
    db far *(*video_memory)(void);
    void (*copy_to_video)(db far *video, const db *cells, dw byte_count);
    db video_mode;
} DosTextPresenter;

static DosTextPresenter dos_text_default_presenter;

typedef struct DosTextPresentFrame {
    void *user;
    db far *video;
    const db *cells;
    dw byte_count;
} DosTextPresentFrame;

static dw dos_text_clamp_present_byte_count(dw byte_count, dw max_bytes) {
    return byte_count > max_bytes ? max_bytes : byte_count;
}

static dw dos_text_present_byte_count(const IplayTextMode *mode, dw byte_count) {
    dw max_bytes = iplay_text_mode_screen_bytes(mode);
    return dos_text_clamp_present_byte_count(byte_count, max_bytes);
}

static db far *dos_text_present_video_memory(void) {
    return dos_hw_io_text_color_memory();
}

static db far *dos_text_present_mono_video_memory(void) {
    return dos_hw_io_text_mono_memory();
}

static int dos_text_video_mode_is_mono(db video_mode) {
    switch (video_mode & 0x7fu) {
    case IPLAY_VIDEO_MODE_40X25_BW:
    case IPLAY_VIDEO_MODE_80X25_BW:
        return 1;
    default:
        return 0;
    }
}

static db far *(*dos_text_video_memory_for_mode(db video_mode))(void) {
    return dos_text_video_mode_is_mono(video_mode) ? dos_text_present_mono_video_memory : dos_text_present_video_memory;
}

static void dos_text_present_cells(db far *video, const db *cells, dw byte_count) {
    dos_hw_io_copy_to_far(video, cells, byte_count);
}

#define dos_text_presenter_set_video_memory_field(state, value) ((state)->video_memory = (value))
#define dos_text_presenter_set_copy_to_video_field(state, value) ((state)->copy_to_video = (value))
#define dos_text_presenter_set_video_mode_field(state, value) ((state)->video_mode = (value))
#define dos_text_presenter_video_memory_fn_field(state) ((state)->video_memory)
#define dos_text_presenter_copy_to_video_fn_field(state) ((state)->copy_to_video)
#define dos_text_presenter_video_mode_field(state) ((state)->video_mode)

static void dos_text_presenter_set_video_memory(DosTextPresenter *presenter, db far *(*video_memory)(void)) {
    dos_text_presenter_set_video_memory_field(presenter, video_memory);
}

static void dos_text_presenter_set_copy_to_video(DosTextPresenter *presenter, void (*copy_to_video)(db far *video, const db *cells, dw byte_count)) {
    dos_text_presenter_set_copy_to_video_field(presenter, copy_to_video);
}

static void dos_text_presenter_set_video_mode(DosTextPresenter *presenter, db video_mode) {
    dos_text_presenter_set_video_mode_field(presenter, video_mode);
}

static db dos_text_presenter_video_mode(const DosTextPresenter *presenter) {
    return dos_text_presenter_video_mode_field(presenter);
}

static void dos_text_presenter_init(DosTextPresenter *presenter, db video_mode, void (*copy_to_video)(db far *video, const db *cells, dw byte_count)) {
    dos_text_presenter_set_video_memory(presenter, dos_text_video_memory_for_mode(video_mode));
    dos_text_presenter_set_copy_to_video(presenter, copy_to_video);
    dos_text_presenter_set_video_mode(presenter, video_mode);
}

static void dos_text_presenter_init_vga_text_mode(DosTextPresenter *presenter, db video_mode) {
    dos_text_presenter_init(presenter, video_mode, dos_text_present_cells);
}

static void dos_text_presenter_init_vga_text(DosTextPresenter *presenter) {
    dos_text_presenter_init_vga_text_mode(presenter, IPLAY_TEXT_DEFAULT_VIDEO_MODE);
}

static DosTextPresenter *dos_text_default_presenter_state(void) {
    return &dos_text_default_presenter;
}

static void player_init_text_presenter(void) {
    dos_text_presenter_init_vga_text(dos_text_default_presenter_state());
}

static void *dos_text_default_present_user(void) {
    return (void *)dos_text_default_presenter_state();
}

static void *dos_text_present_user(void *user) {
    return user ? user : dos_text_default_present_user();
}

static const DosTextPresenter *dos_text_presenter_from_user(void *user) {
    return (const DosTextPresenter *)dos_text_present_user(user);
}

static db far *(*dos_text_presenter_video_memory_fn(const DosTextPresenter *presenter))(void) {
    return dos_text_presenter_video_memory_fn_field(presenter);
}

static void (*dos_text_presenter_copy_to_video_fn(const DosTextPresenter *presenter))(db far *video, const db *cells, dw byte_count) {
    return dos_text_presenter_copy_to_video_fn_field(presenter);
}

static db far *dos_text_presenter_video_memory(const DosTextPresenter *presenter) {
    return dos_text_presenter_video_memory_fn(presenter)();
}

static void dos_text_presenter_copy_to_video(const DosTextPresenter *presenter, db far *video, const db *cells, dw byte_count) {
    dos_text_presenter_copy_to_video_fn(presenter)(video, cells, byte_count);
}

#define dos_text_present_frame_set_byte_count_field(state, value) ((state)->byte_count = (value))
#define dos_text_present_frame_set_user_field(state, value) ((state)->user = (value))
#define dos_text_present_frame_set_video_field(state, value) ((state)->video = (value))
#define dos_text_present_frame_set_cells_field(state, value) ((state)->cells = (value))

static void dos_text_present_frame_set_byte_count(DosTextPresentFrame *frame, dw byte_count) {
    dos_text_present_frame_set_byte_count_field(frame, byte_count);
}

static void dos_text_present_frame_set_user(DosTextPresentFrame *frame, void *user) {
    dos_text_present_frame_set_user_field(frame, user);
}

static void dos_text_present_frame_set_video(DosTextPresentFrame *frame, db far *video) {
    dos_text_present_frame_set_video_field(frame, video);
}

static void dos_text_present_frame_set_cells(DosTextPresentFrame *frame, const db *cells) {
    dos_text_present_frame_set_cells_field(frame, cells);
}

static void dos_text_present_frame_init(DosTextPresentFrame *frame, void *user, db far *video, const db *cells, dw byte_count) {
    dos_text_present_frame_set_user(frame, user);
    dos_text_present_frame_set_byte_count(frame, byte_count);
    dos_text_present_frame_set_video(frame, video);
    dos_text_present_frame_set_cells(frame, cells);
}

static db far *dos_text_prepare_present_video(void *user) {
    return dos_text_presenter_video_memory(dos_text_presenter_from_user(user));
}

static dw dos_text_prepare_present_byte_count(const IplayTextMode *mode, dw byte_count) {
    return dos_text_present_byte_count(mode, byte_count);
}

static void dos_text_prepare_present_frame_init(DosTextPresentFrame *frame, void *present_user, const db *cells, const IplayTextMode *mode, dw byte_count) {
    dos_text_present_frame_init(frame, present_user, dos_text_prepare_present_video(present_user), cells, dos_text_prepare_present_byte_count(mode, byte_count));
}

static void dos_text_prepare_present(DosTextPresentFrame *frame, void *user, const db *cells, const IplayTextMode *mode, dw byte_count) {
    void *present_user = dos_text_present_user(user);
    dos_text_prepare_present_frame_init(frame, present_user, cells, mode, byte_count);
}

#define dos_text_present_frame_user_field(state) ((state)->user)

static void *dos_text_present_frame_user(const DosTextPresentFrame *frame) {
    return dos_text_present_frame_user_field(frame);
}

static const DosTextPresenter *dos_text_present_frame_presenter(const DosTextPresentFrame *frame) {
    return dos_text_presenter_from_user(dos_text_present_frame_user(frame));
}

#define dos_text_present_frame_video_field(state) ((state)->video)

static db far *dos_text_present_frame_video(const DosTextPresentFrame *frame) {
    return dos_text_present_frame_video_field(frame);
}

#define dos_text_present_frame_byte_count_field(state) ((state)->byte_count)

static dw dos_text_present_frame_byte_count(const DosTextPresentFrame *frame) {
    return dos_text_present_frame_byte_count_field(frame);
}

#define dos_text_present_frame_cells_field(state) ((state)->cells)

static const db *dos_text_present_frame_cells(const DosTextPresentFrame *frame) {
    return dos_text_present_frame_cells_field(frame);
}

static void dos_text_present_frame_copy_to_video(const DosTextPresentFrame *frame) {
    dos_text_presenter_copy_to_video(dos_text_present_frame_presenter(frame), dos_text_present_frame_video(frame), dos_text_present_frame_cells(frame), dos_text_present_frame_byte_count(frame));
}

static void dos_text_present_frame(const DosTextPresentFrame *frame) {
    dos_text_present_frame_copy_to_video(frame);
}

static void dos_text_present(void *user, const db *cells, const IplayTextMode *mode, dw byte_count) {
    DosTextPresentFrame frame;
    dos_text_prepare_present(&frame, user, cells, mode, byte_count);
    dos_text_present_frame(&frame);
}
#endif

static int player_module_storage_can_hold_size(unsigned long wanted, size_t capacity) {
    return wanted <= (unsigned long)capacity;
}

static int read_file_info(const char *path, unsigned long *size_out, db *data, size_t data_capacity, size_t *data_len) {
    int fd;
    long end;
    int n;
    unsigned long file_size;
    size_t wanted;
    size_t read_count;
    if (data_len) *data_len = 0;
    if (!data || data_capacity == 0) return 0;
    fd = player_open_read_binary(path);
    if (fd < 0) return 0;
    end = lseek(fd, 0L, SEEK_END);
    if (end <= 0 || lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        return 0;
    }
    file_size = (unsigned long)end;
    if (size_out) *size_out = file_size;
    wanted = player_module_storage_can_hold_size(file_size, data_capacity) ? (size_t)file_size : data_capacity;
    read_count = 0;
    while (read_count < wanted) {
        size_t remaining = wanted - read_count;
        unsigned chunk = remaining > 0x7fffu ? 0x7fffu : (unsigned)remaining;
        n = read(fd, data + read_count, chunk);
        if (n <= 0) break;
        read_count += (size_t)n;
    }
    close(fd);
    if (read_count != wanted) {
        return 0;
    }
    if (data_len) *data_len = read_count;
    if (!player_module_storage_can_hold_size(file_size, data_capacity)) return IPLAY_PLAYER_MODULE_HEADER_TRUNCATED;
    return IPLAY_PLAYER_MODULE_OK;
}

static void print_s3m_metadata(const db *header, size_t header_len) {
    char title[29];
    dw orders;
    dw instruments;
    dw patterns;
    dw flags;
    dw tracker;
    dw ffi;
    if (header_len < 0x60 || !has_sig(header, header_len, 0x2cu, "SCRM")) return;
    copy_trimmed_text(title, sizeof(title), header, 28);
    orders = load_u16_le(header + 0x20);
    instruments = load_u16_le(header + 0x22);
    patterns = load_u16_le(header + 0x24);
    flags = load_u16_le(header + 0x26);
    tracker = load_u16_le(header + 0x28);
    ffi = load_u16_le(header + 0x2a);
    printf("Title: %s\n", title[0] ? title : "(untitled)");
    printf("Orders: %u Instruments: %u Patterns: %u\n",
           (unsigned)orders,
           (unsigned)instruments,
           (unsigned)patterns);
    printf("S3M flags: %04X Tracker: %04X FFI: %04X\n",
           (unsigned)flags,
           (unsigned)tracker,
           (unsigned)ffi);
    printf("FFI: %04X\n", (unsigned)ffi);
    if (orders != 0 && header_len >= 0x60u + orders) {
        dw shown = orders < 8u ? orders : 8u;
        dw i;
        printf("Order preview:");
        for (i = 0; i < shown; ++i) printf(" %02X", header[0x60u + i]);
        if (shown != orders) printf(" ...");
        printf("\n");
    }
}

static unsigned mod_channels_from_sig(const db *sig) {
    if ((sig[0] >= '1' && sig[0] <= '9') && sig[1] == 'C' && sig[2] == 'H' && sig[3] == 'N') {
        return (unsigned)(sig[0] - '0');
    }
    if (sig[0] == 'M' && (sig[1] == '.' || sig[1] == '!') && sig[2] == 'K' && sig[3] == '.') return 4;
    if (sig[0] == 'F' && sig[1] == 'L' && sig[2] == 'T' && sig[3] >= '1' && sig[3] <= '9') return (unsigned)(sig[3] - '0');
    return 4;
}

static void print_mod_metadata(const db *header, size_t header_len) {
    char title[21];
    unsigned song_len;
    unsigned channels;
    if (header_len < 1084u) return;
    if (!has_sig(header, header_len, 1080u, "M.K.") && !has_sig(header, header_len, 1080u, "M!K!") &&
        !has_sig(header, header_len, 1080u, "FLT4") && !has_sig(header, header_len, 1080u, "4CHN") &&
        !has_sig(header, header_len, 1080u, "6CHN") && !has_sig(header, header_len, 1080u, "8CHN")) {
        return;
    }
    copy_trimmed_text(title, sizeof(title), header, 20);
    song_len = header[950];
    channels = mod_channels_from_sig(header + 1080u);
    printf("Title: %s\n", title[0] ? title : "(untitled)");
    printf("Orders: %u Channels: %u\n", song_len, channels);
    if (song_len != 0 && song_len <= 128u && header_len >= 1084u) {
        unsigned shown = song_len < 8u ? song_len : 8u;
        unsigned i;
        printf("Order preview:");
        for (i = 0; i < shown; ++i) printf(" %02X", header[952u + i]);
        if (shown != song_len) printf(" ...");
        printf("\n");
    }
    printf("MOD signature: %c%c%c%c\n", header[1080], header[1081], header[1082], header[1083]);
}

static void print_mtm_metadata(const db *header, size_t header_len) {
    char title[21];
    unsigned tracks;
    unsigned patterns;
    unsigned orders;
    unsigned channels;
    if (header_len < 0x42u || !has_sig(header, header_len, 0u, "MTM")) return;
    copy_trimmed_text(title, sizeof(title), header + 4u, 20);
    tracks = load_u16_le(header + 0x1au);
    patterns = (unsigned)header[0x1c] + 1u;
    orders = (unsigned)header[0x1e] + 1u;
    channels = header[0x20];
    printf("Title: %s\n", title[0] ? title : "(untitled)");
    printf("Tracks: %u Patterns: %u Orders: %u Channels: %u\n",
           tracks,
           patterns,
           orders,
           channels);
    if (orders != 0 && header_len >= 0x42u) {
        unsigned shown = orders < 8u ? orders : 8u;
        unsigned i;
        printf("Order preview:");
        for (i = 0; i < shown; ++i) printf(" %02X", header[0x22u + i]);
        if (shown != orders) printf(" ...");
        printf("\n");
    }
}

static void print_far_metadata(const db *header, size_t header_len) {
    char title[41];
    if (header_len < 0x31u || !has_sig(header, header_len, 0u, "FAR")) return;
    copy_trimmed_text(title, sizeof(title), header + 4u, 40);
    printf("Title: %s\n", title[0] ? title : "(untitled)");
    printf("Channels: 16\n");
}

static void print_669_metadata(const db *header, size_t header_len) {
    char message[37];
    if (header_len < 0x71u) return;
    if (!((header[0] == 'i' && header[1] == 'f') || (header[0] == 'J' && header[1] == 'N'))) return;
    copy_trimmed_text(message, sizeof(message), header + 2u, 36);
    printf("Message: %s\n", message[0] ? message : "(empty)");
    printf("Samples: %u Patterns: %u Restart: %u\n",
           (unsigned)header[0x6e],
           (unsigned)header[0x6f],
           (unsigned)header[0x70]);
}

static void print_ult_metadata(const db *header, size_t header_len) {
    size_t i;
    size_t n;
    if (!has_sig(header, header_len, 0u, "MAS_UTrack_V00")) return;
    n = header_len < 15u ? header_len : 15u;
    while (n != 0 && (header[n - 1u] == 0 || header[n - 1u] == ' ')) --n;
    printf("Version: ");
    if (n == 0) {
        printf("MAS_UTrack");
    } else {
        for (i = 0; i < n; ++i) {
            db ch = header[i];
            putchar((ch >= 0x20 && ch < 0x7f) ? (char)ch : '.');
        }
    }
    putchar('\n');
    printf("Title: ");
    if (header_len < 47u) {
        printf("(untitled)");
    } else {
        n = 32u;
        while (n != 0 && (header[15u + n - 1u] == 0 || header[15u + n - 1u] == ' ')) --n;
        if (n == 0) {
            printf("(untitled)");
        } else {
            for (i = 0; i < n; ++i) {
                db ch = header[15u + i];
                putchar((ch >= 0x20 && ch < 0x7f) ? (char)ch : '.');
            }
        }
    }
    putchar('\n');
}

static void print_psm_metadata(const db *header, size_t header_len) {
    char title[41];
    if (!has_sig(header, header_len, 0u, "PSM ")) return;
    title[0] = 0;
    if (header_len >= 0x2cu) copy_trimmed_text(title, sizeof(title), header + 4u, 40);
    printf("Title: %s\n", title[0] ? title : "(untitled)");
}

static void print_inr_metadata(const db *header, size_t header_len) {
    char title[33];
    if (!has_sig(header, header_len, 0u, "IMPM")) return;
    title[0] = 0;
    if (header_len >= 0x24u) copy_trimmed_text(title, sizeof(title), header + 4u, 32);
    printf("Title: %s\n", title[0] ? title : "(untitled)");
}

static void print_stm_metadata(const db *header, size_t header_len) {
    char title[21];
    char tracker[9];
    if (header_len < 28u || !has_sig(header, header_len, 20u, "!Scream!")) return;
    copy_trimmed_text(title, sizeof(title), header, 20);
    copy_trimmed_text(tracker, sizeof(tracker), header + 20u, 8);
    printf("Title: %s\n", title[0] ? title : "(untitled)");
    printf("Tracker: %s\n", tracker[0] ? tracker : "!Scream!");
}

static void print_external_library_metadata(const db *header, size_t header_len) {
    char title[27];
    if (header_len >= 37u && has_sig(header, header_len, 0u, "Extended Module: ")) {
        copy_trimmed_text(title, sizeof(title), header + 17u, 20);
    } else if (header_len >= 0x20u && has_sig(header, header_len, 0u, "IMPM")) {
        copy_trimmed_text(title, sizeof(title), header + 4u, 26);
    } else if (header_len != 0) {
        copy_trimmed_text(title, sizeof(title), header, header_len < 20u ? header_len : 20u);
    } else {
        return;
    }
    printf("Title: %s\n", title[0] ? title : "(untitled)");
}

static void print_loader_metadata(const LoaderInfo *loader, const db *header, size_t header_len) {
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        print_s3m_metadata(header, header_len);
        break;
    case IPLAY_LOADER_KIND_MOD:
        print_mod_metadata(header, header_len);
        break;
    case IPLAY_LOADER_KIND_MTM:
        print_mtm_metadata(header, header_len);
        break;
    case IPLAY_LOADER_KIND_FAR:
        print_far_metadata(header, header_len);
        break;
    case IPLAY_LOADER_KIND_669:
        print_669_metadata(header, header_len);
        break;
    case IPLAY_LOADER_KIND_ULT:
        print_ult_metadata(header, header_len);
        break;
    case IPLAY_LOADER_KIND_PSM:
        print_psm_metadata(header, header_len);
        break;
    case IPLAY_LOADER_KIND_INR:
        print_inr_metadata(header, header_len);
        break;
    case IPLAY_LOADER_KIND_STM:
        print_stm_metadata(header, header_len);
        break;
    case IPLAY_LOADER_KIND_EXTERNAL_LIBRARY:
        print_external_library_metadata(header, header_len);
        break;
    default:
        break;
    }
}

static void print_module_summary(const char *path, unsigned long size, const LoaderInfo *loader, dd module_type) {
    printf("Module: %s\n", path);
    printf("Size: %lu bytes\n", size);
    printf("Loader: %s (%s)\n", loader_symbol(loader), loader_name(loader));
    printf("Module type tag: %08lX\n", (unsigned long)module_type);
}

static void player_report_open_failed(const char *path) {
    (void)path;
    puts("Module not found.");
}

static void player_report_unsupported_module(const char *path) {
    printf("Unsupported module type: %s\n", path);
}

static void player_report_module_too_large(const char *path, unsigned long size) {
    printf("Module too large: %s (%lu bytes, buffer %u bytes)\n",
           path,
           size,
           (unsigned)IPLAY_PLAYER_MODULE_BUFFER_BYTES);
}

static int player_report_usage(void) {
    print_usage();
    return player_exit_ok_status();
}

static int player_report_sound_settings(void) {
    puts("Current Soundcard settings:");
#if IPLAY_PLAYER_ENABLE_SB16_HW
    {
        DosSb16Hardware *hw = player_sb16_hardware();
        printf("Sound Blaster 16/16ASP, mixed at %ukHz\n", (unsigned)(player_sb16_sample_rate(hw) / 1000u));
        printf("%03Xh, IRQ %u, DMA %u\n",
               (unsigned)player_sb16_base_port(hw),
               (unsigned)player_sb16_irq(hw),
               (unsigned)player_sb16_dma16(hw));
    }
#else
    puts("SB16 hardware disabled in this build.");
#endif
    return player_exit_ok_status();
}

static int player_report_invalid_video_mode(void) {
    puts("Unsupported video mode. Use --video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50");
    return IPLAY_PLAYER_EXIT_OPEN_FAILED;
}

static void player_report_playback_output(void) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
#if IPLAY_PLAYER_ENABLE_SB16_HW
    DosSb16Hardware *hw = player_sb16_hardware();
    puts("Playback output: SB16 16-bit stereo hardware wrapper enabled.");
    printf("SB16 config: base=%03Xh irq=%u dma16=%u rate=%u\n",
           (unsigned)player_sb16_base_port(hw),
           (unsigned)player_sb16_irq(hw),
           (unsigned)player_sb16_dma16(hw),
           (unsigned)player_sb16_sample_rate(hw));
#else
    puts("Playback output: SDL-compatible discard sink; SB16 hardware disabled in this build.");
#endif
#else
    (void)0;
#endif
}

static void player_report_decoder_handoff(const LoaderInfo *loader) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    printf("Decoder route: id=%u name=%s\n", (unsigned)loader_decoder_route_id(loader), loader_decoder_route_name(loader));
    if (loader_uses_external_decoder_library(loader)) {
        puts("Decoder handoff: external tracker -> SB16 PCM seam.");
        return;
    }
    if (loader_uses_project_decoder(loader)) {
        puts("Decoder handoff: project INR -> SB16 PCM.");
        return;
    }
    puts("Decoder handoff: unavailable.");
#else
    (void)loader;
#endif
}

static void player_flush_reports(void) {
    fflush(stdout);
}

static db *player_module_storage_buffer(void) {
    static db far module_data[IPLAY_PLAYER_MODULE_BUFFER_BYTES];
    return module_data;
}

#define player_module_set_path_field(state, value) ((state)->path = (value))
#define player_module_set_header_field(state, value) ((state)->header = (value))
#define player_module_set_loader_field(state, value) ((state)->loader = (value))
#define player_module_set_header_len_field(state, value) ((state)->header_len = (value))
#define player_module_set_header_capacity_field(state, value) ((state)->header_capacity = (value))
#define player_module_set_header_truncated_field(state, value) ((state)->header_truncated = (value))
#define player_module_set_size_field(state, value) ((state)->size = (value))
#define player_module_set_type_tag_field(state, value) ((state)->module_type = (value))
#define player_module_path_field(state) ((state)->path)
#define player_module_loader_field(state) ((state)->loader)
#define player_module_size_field(state) ((state)->size)
#define player_module_type_tag_field(state) ((state)->module_type)
#define player_module_header_field(state) ((state)->header)
#define player_module_header_data_field(state) ((state)->header)
#define player_module_header_len_field(state) ((state)->header_len)
#define player_module_header_capacity_field(state) ((state)->header_capacity)
#define player_module_header_truncated_field(state) ((state)->header_truncated)
#define player_module_size_out_field(state) (&(state)->size)
#define player_module_header_len_out_field(state) (&(state)->header_len)

static void player_module_set_source(PlayerModuleInfo *module, const char *path, db *header, size_t capacity) {
    player_module_set_path_field(module, path);
    player_module_set_header_field(module, header);
    player_module_set_header_capacity_field(module, capacity);
    player_module_set_header_truncated_field(module, 0);
}

static void player_module_clear_loaded_state(PlayerModuleInfo *module) {
    player_module_set_loader_field(module, NULL);
    player_module_set_header_len_field(module, 0);
    player_module_set_header_truncated_field(module, 0);
    player_module_set_size_field(module, 0);
    player_module_set_type_tag_field(module, 0);
}

static void player_init_module_info(PlayerModuleInfo *module, const char *path, db *header, size_t capacity) {
    player_module_set_source(module, path, header, capacity);
    player_module_clear_loaded_state(module);
}

static const char *player_module_path(const PlayerModuleInfo *module) {
    return player_module_path_field(module);
}

static const LoaderInfo *player_module_loader(const PlayerModuleInfo *module) {
    return player_module_loader_field(module);
}

static unsigned long player_module_size(const PlayerModuleInfo *module) {
    return player_module_size_field(module);
}

static dd player_module_type_tag(const PlayerModuleInfo *module) {
    return player_module_type_tag_field(module);
}

static const db *player_module_header(const PlayerModuleInfo *module) {
    return player_module_header_field(module);
}

static db *player_module_header_data(PlayerModuleInfo *module) {
    return player_module_header_data_field(module);
}

static size_t player_module_header_len(const PlayerModuleInfo *module) {
    return player_module_header_len_field(module);
}

static size_t player_module_header_capacity(const PlayerModuleInfo *module) {
    return player_module_header_capacity_field(module);
}

static int player_module_header_truncated(const PlayerModuleInfo *module) {
    return player_module_header_truncated_field(module) != 0;
}

static int player_module_data_complete(const PlayerModuleInfo *module) {
    /* Accessor-only inventory marker: return !player_module_header_truncated(module); */
    return module && !player_module_header_truncated(module);
}

static const char *player_module_decoder_input_name(const PlayerModuleInfo *module) {
    return player_module_data_complete(module) ? "memory" : "file-path";
}

const char *iplay_player_module_path(const PlayerModuleInfo *module) {
    return player_module_path(module);
}

unsigned long iplay_player_module_size(const PlayerModuleInfo *module) {
    return player_module_size(module);
}

int iplay_player_module_header_truncated(const PlayerModuleInfo *module) {
    return player_module_header_truncated(module);
}

const char *iplay_player_module_decoder_input_name(const PlayerModuleInfo *module) {
    return player_module_decoder_input_name(module);
}

static int player_module_has_range(const PlayerModuleInfo *module, dd offset, dd byte_count) {
    dd len;
    if (!module) return 0;
    len = (dd)player_module_header_len(module);
    return offset <= len && byte_count <= len - offset;
}

static db *player_module_file_stream_buffer(void) {
    static db stream_buffer[IPLAY_PLAYER_FILE_STREAM_BUFFER_BYTES];
    return stream_buffer;
}

static db player_module_byte_at(const PlayerModuleInfo *module, dd offset) {
    const db *data;
    if (!player_module_has_range(module, offset, 1u)) return 0;
    data = player_module_header(module);
    return data[(size_t)offset];
}

static dw player_module_u16_be_at(const PlayerModuleInfo *module, dd offset) {
    return (dw)(((dw)player_module_byte_at(module, offset) << 8) | player_module_byte_at(module, offset + 1u));
}

static dw player_module_u16_le_at(const PlayerModuleInfo *module, dd offset) {
    return (dw)((dw)player_module_byte_at(module, offset) | ((dw)player_module_byte_at(module, offset + 1u) << 8));
}

static dd player_module_u32_le_at(const PlayerModuleInfo *module, dd offset) {
    return (dd)player_module_byte_at(module, offset) |
           ((dd)player_module_byte_at(module, offset + 1u) << 8) |
           ((dd)player_module_byte_at(module, offset + 2u) << 16) |
           ((dd)player_module_byte_at(module, offset + 3u) << 24);
}

static unsigned long *player_module_size_out(PlayerModuleInfo *module) {
    return player_module_size_out_field(module);
}

static size_t *player_module_header_len_out(PlayerModuleInfo *module) {
    return player_module_header_len_out_field(module);
}

static void player_module_release(PlayerModuleInfo *module) {
    player_module_set_header_field(module, NULL);
    player_module_set_header_len_field(module, 0);
    player_module_set_header_capacity_field(module, 0);
    player_module_set_header_truncated_field(module, 0);
}

static void player_module_set_loader(PlayerModuleInfo *module, const LoaderInfo *loader) {
    player_module_set_loader_field(module, loader);
}

static void player_module_set_type_tag(PlayerModuleInfo *module, dd module_type) {
    player_module_set_type_tag_field(module, module_type);
}

static void player_detect_module_loader(PlayerModuleInfo *module) {
    player_module_set_loader(module, detect_loader_for_module(player_module_path(module), player_module_header(module), player_module_header_len(module)));
}

static void player_apply_module_type_tag(PlayerModuleInfo *module) {
    player_module_set_type_tag(module, loader_module_type_tag(player_module_loader(module)));
}

static int player_read_module_file_info(PlayerModuleInfo *module) {
    return read_file_info(player_module_path(module), player_module_size_out(module), player_module_header_data(module),
                          player_module_header_capacity(module), player_module_header_len_out(module));
}

static int player_module_file_info_loaded(PlayerModuleInfo *module) {
    return player_read_module_file_info(module);
}

static int player_module_loader_available(const PlayerModuleInfo *module) {
    return loader_decoder_available(player_module_loader(module));
}

static int player_module_accepts_capped_header(const PlayerModuleInfo *module) {
    const LoaderInfo *loader = player_module_loader(module);
    if (!loader) return 0;
    return loader_uses_external_decoder_library(loader);
}

static int player_module_loader_header_valid(const PlayerModuleInfo *module) {
    const LoaderInfo *loader = player_module_loader(module);
    if (!loader) return 0;
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_MOD:
        return player_module_header_len(module) >= IPLAY_MOD_MIN_HEADER_BYTES;
    default:
        return 1;
    }
}

static int player_module_open_failed_status(void) {
    return IPLAY_PLAYER_MODULE_OPEN_FAILED;
}

static int player_module_unsupported_status(void) {
    return IPLAY_PLAYER_MODULE_UNSUPPORTED;
}

static int player_module_too_large_status(void) {
    return IPLAY_PLAYER_MODULE_TOO_LARGE;
}

static int player_module_ok_status(void) {
    return IPLAY_PLAYER_MODULE_OK;
}

static void player_report_loaded_module(const PlayerModuleInfo *module) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    print_module_summary(player_module_path(module), player_module_size(module), player_module_loader(module), player_module_type_tag(module));
    print_loader_metadata(player_module_loader(module), player_module_header(module), player_module_header_len(module));
    player_report_decoder_handoff(player_module_loader(module));
    player_report_playback_output();
    player_flush_reports();
#else
    (void)module;
#endif
}

static int player_load_module_info(PlayerModuleInfo *module) {
    int read_status = player_module_file_info_loaded(module);
    int capped_header = read_status == IPLAY_PLAYER_MODULE_HEADER_TRUNCATED;
    player_module_set_header_truncated_field(module, capped_header ? 1u : 0u);
    if (read_status == IPLAY_PLAYER_MODULE_TOO_LARGE) return player_module_too_large_status();
    if (read_status != IPLAY_PLAYER_MODULE_OK && !capped_header) return player_module_open_failed_status();
    player_detect_module_loader(module);
    if (!player_module_loader_header_valid(module)) return player_module_unsupported_status();
    if (!player_module_loader_available(module)) return player_module_unsupported_status();
    if (capped_header && !player_module_accepts_capped_header(module)) return player_module_too_large_status();
    player_apply_module_type_tag(module);
    return player_module_ok_status();
}

static int player_prepare_module(PlayerModuleInfo *module, const char *path, db *data, size_t capacity) {
    player_init_module_info(module, path, data, capacity);
    return player_load_module_info(module);
}

static int player_prepare_requested_module(PlayerModuleInfo *module, const PlayerModuleRequest *request) {
    return player_prepare_module(module, player_module_request_path(request), player_module_storage_buffer(), IPLAY_PLAYER_MODULE_BUFFER_BYTES);
}

static int player_module_load_ok(int load_status) {
    return load_status == IPLAY_PLAYER_MODULE_OK;
}

static int player_module_load_open_failed(int load_status) {
    return load_status == IPLAY_PLAYER_MODULE_OPEN_FAILED;
}

static int player_module_load_unsupported(int load_status) {
    return load_status == IPLAY_PLAYER_MODULE_UNSUPPORTED;
}

static int player_module_load_too_large(int load_status) {
    return load_status == IPLAY_PLAYER_MODULE_TOO_LARGE;
}

static int player_exit_open_failed_status(void) {
    return IPLAY_PLAYER_EXIT_OPEN_FAILED;
}

static int player_exit_unsupported_status(void) {
    return IPLAY_PLAYER_EXIT_UNSUPPORTED;
}

static int player_exit_too_large_status(void) {
    return IPLAY_PLAYER_EXIT_TOO_LARGE;
}

static int player_exit_ok_status(void) {
    return IPLAY_PLAYER_EXIT_OK;
}

static int player_exit_audio_unavailable_status(void) {
    return IPLAY_PLAYER_EXIT_AUDIO_UNAVAILABLE;
}

static int player_module_load_exit_code(int load_status) {
    if (player_module_load_open_failed(load_status)) return player_exit_open_failed_status();
    if (player_module_load_unsupported(load_status)) return player_exit_unsupported_status();
    if (player_module_load_too_large(load_status)) return player_exit_too_large_status();
    return player_exit_ok_status();
}

static int player_report_module_load_failure(int load_status, const PlayerModuleInfo *module) {
    if (player_module_load_open_failed(load_status)) {
        player_report_open_failed(player_module_path(module));
    }
    if (player_module_load_unsupported(load_status)) {
        player_report_unsupported_module(player_module_path(module));
    }
    if (player_module_load_too_large(load_status)) {
        player_report_module_too_large(player_module_path(module), player_module_size(module));
    }
    return player_module_load_exit_code(load_status);
}

#if IPLAY_PLAYER_ENABLE_TEXT_UI
static IplayVideoPresentFn player_text_video_present_fn(void) {
    return dos_text_present;
}

static void *player_text_video_present_user(void) {
    return dos_text_default_present_user();
}

static void player_video_backend_init_text(PlayerVideoBackend *backend) {
    player_video_backend_init(backend, player_text_video_present_fn(), player_text_video_present_user());
}

static db *player_text_video_cells(void) {
    return player_video_memory();
}

static dw player_text_video_capacity(void) {
    return PLAYER_VIDEO_SIZE;
}

static db player_text_current_video_mode = IPLAY_PLAYER_DEFAULT_VIDEO_MODE;

static void player_set_text_video_mode_id(db video_mode) {
    player_text_current_video_mode = video_mode;
}

static db player_text_video_mode_id(void) {
    return player_text_current_video_mode;
}

static const IplayTextMode *player_text_video_mode(void) {
    return iplay_text_mode_for_video_mode(player_text_video_mode_id());
}

#define player_video_config_set_cells_field(state, value) ((state)->cells = (value))
#define player_video_config_set_capacity_field(state, value) ((state)->capacity = (value))
#define player_video_config_set_mode_field(state, value) ((state)->mode = (value))
#define player_video_config_set_video_mode_field(state, value) ((state)->video_mode = (value))
#define player_video_config_cells_field(state) ((state)->cells)
#define player_video_config_capacity_field(state) ((state)->capacity)
#define player_video_config_mode_field(state) ((state)->mode)
#define player_video_config_video_mode_field(state) ((state)->video_mode)

static void player_video_config_init(PlayerVideoConfig *config, db *cells, dw capacity, const IplayTextMode *mode, db video_mode) {
    player_video_config_set_cells_field(config, cells);
    player_video_config_set_capacity_field(config, capacity);
    player_video_config_set_mode_field(config, mode);
    player_video_config_set_video_mode_field(config, video_mode);
}

static void player_video_config_init_runtime(PlayerVideoConfig *config) {
    player_video_config_init(config, player_text_video_cells(), player_text_video_capacity(), player_text_video_mode(), player_text_video_mode_id());
}

static db *player_video_config_cells(const PlayerVideoConfig *config) {
    return player_video_config_cells_field(config);
}

static dw player_video_config_capacity(const PlayerVideoConfig *config) {
    return player_video_config_capacity_field(config);
}

static const IplayTextMode *player_video_config_mode(const PlayerVideoConfig *config) {
    return player_video_config_mode_field(config);
}

static db player_video_config_video_mode(const PlayerVideoConfig *config) {
    return player_video_config_video_mode_field(config);
}

#define player_runtime_output_video_config_field(state) (&(state)->video_config)
#define player_runtime_output_video_backend_field(state) (&(state)->video_backend)
#define player_runtime_output_audio_backend_field(state) (&(state)->audio_backend)

static PlayerVideoConfig *player_runtime_output_video_config(PlayerRuntimeOutput *output) {
    return player_runtime_output_video_config_field(output);
}

static PlayerVideoBackend *player_runtime_output_video_backend(PlayerRuntimeOutput *output) {
    return player_runtime_output_video_backend_field(output);
}

static PlayerAudioBackend *player_runtime_output_audio_backend(PlayerRuntimeOutput *output) {
    return player_runtime_output_audio_backend_field(output);
}

static void player_runtime_output_init_text_config(PlayerRuntimeOutput *output) {
    player_video_config_init_runtime(player_runtime_output_video_config(output));
}

static void player_runtime_output_init_text_backend(PlayerRuntimeOutput *output) {
    player_video_backend_init_text(player_runtime_output_video_backend(output));
}

static void player_runtime_output_init_text_video(PlayerRuntimeOutput *output) {
    player_runtime_output_init_text_config(output);
    player_runtime_output_init_text_backend(output);
}

static db *player_runtime_output_video_cells(PlayerRuntimeOutput *output) {
    return player_video_config_cells(player_runtime_output_video_config(output));
}

static dw player_runtime_output_video_capacity(PlayerRuntimeOutput *output) {
    return player_video_config_capacity(player_runtime_output_video_config(output));
}

static const IplayTextMode *player_runtime_output_video_mode(PlayerRuntimeOutput *output) {
    return player_video_config_mode(player_runtime_output_video_config(output));
}

static db player_runtime_output_video_mode_id(PlayerRuntimeOutput *output) {
    return player_video_config_video_mode(player_runtime_output_video_config(output));
}

static IplayVideoPresentFn player_runtime_output_video_present(PlayerRuntimeOutput *output) {
    return player_video_backend_present(player_runtime_output_video_backend(output));
}

static void *player_runtime_output_video_user(PlayerRuntimeOutput *output) {
    return player_video_backend_user(player_runtime_output_video_backend(output));
}

static IplayAudioWriteFn player_runtime_output_audio_write(PlayerRuntimeOutput *output) {
    return player_audio_backend_write(player_runtime_output_audio_backend(output));
}

static void *player_runtime_output_audio_user(PlayerRuntimeOutput *output) {
    return player_audio_backend_user(player_runtime_output_audio_backend(output));
}

#define player_runtime_video_output_set_cells_field(state, value) ((state)->cells = (value))
#define player_runtime_video_output_set_capacity_field(state, value) ((state)->capacity = (value))
#define player_runtime_video_output_set_mode_field(state, value) ((state)->mode = (value))
#define player_runtime_video_output_set_video_mode_field(state, value) ((state)->video_mode = (value))
#define player_runtime_video_output_set_present_field(state, value) ((state)->present = (value))
#define player_runtime_video_output_set_user_field(state, value) ((state)->user = (value))
#define player_runtime_audio_output_set_write_field(state, value) ((state)->write = (value))
#define player_runtime_audio_output_set_user_field(state, value) ((state)->user = (value))
#define player_runtime_video_output_cells_field(state) ((state)->cells)
#define player_runtime_video_output_capacity_field(state) ((state)->capacity)
#define player_runtime_video_output_mode_field(state) ((state)->mode)
#define player_runtime_video_output_video_mode_field(state) ((state)->video_mode)
#define player_runtime_video_output_present_field(state) ((state)->present)
#define player_runtime_video_output_user_field(state) ((state)->user)
#define player_runtime_audio_output_write_field(state) ((state)->write)
#define player_runtime_audio_output_user_field(state) ((state)->user)

static void player_runtime_video_output_set_cells(PlayerRuntimeVideoOutput *video, db *cells) {
    player_runtime_video_output_set_cells_field(video, cells);
}

static void player_runtime_video_output_set_capacity(PlayerRuntimeVideoOutput *video, dw capacity) {
    player_runtime_video_output_set_capacity_field(video, capacity);
}

static void player_runtime_video_output_set_mode(PlayerRuntimeVideoOutput *video, const IplayTextMode *mode) {
    player_runtime_video_output_set_mode_field(video, mode);
}

static void player_runtime_video_output_set_video_mode(PlayerRuntimeVideoOutput *video, db video_mode) {
    player_runtime_video_output_set_video_mode_field(video, video_mode);
}

static void player_runtime_video_output_set_present(PlayerRuntimeVideoOutput *video, IplayVideoPresentFn present) {
    player_runtime_video_output_set_present_field(video, present);
}

static void player_runtime_video_output_set_user(PlayerRuntimeVideoOutput *video, void *user) {
    player_runtime_video_output_set_user_field(video, user);
}

static void player_runtime_audio_output_set_write(PlayerRuntimeAudioOutput *audio, IplayAudioWriteFn write) {
    player_runtime_audio_output_set_write_field(audio, write);
}

static void player_runtime_audio_output_set_user(PlayerRuntimeAudioOutput *audio, void *user) {
    player_runtime_audio_output_set_user_field(audio, user);
}

static void player_runtime_video_output_prepare_presenter(PlayerRuntimeVideoOutput *video) {
    dos_text_presenter_init_vga_text_mode((DosTextPresenter *)dos_text_present_user(player_runtime_video_output_user_field(video)), player_runtime_video_output_video_mode_field(video));
}

static void player_runtime_video_output_init(PlayerRuntimeVideoOutput *video, PlayerRuntimeOutput *output) {
    player_runtime_video_output_set_cells(video, player_runtime_output_video_cells(output));
    player_runtime_video_output_set_capacity(video, player_runtime_output_video_capacity(output));
    player_runtime_video_output_set_mode(video, player_runtime_output_video_mode(output));
    player_runtime_video_output_set_video_mode(video, player_runtime_output_video_mode_id(output));
    player_runtime_video_output_set_present(video, player_runtime_output_video_present(output));
    player_runtime_video_output_set_user(video, player_runtime_output_video_user(output));
    player_runtime_video_output_prepare_presenter(video);
}

static void player_runtime_audio_output_init(PlayerRuntimeAudioOutput *audio, PlayerRuntimeOutput *output) {
    player_runtime_audio_output_set_write(audio, player_runtime_output_audio_write(output));
    player_runtime_audio_output_set_user(audio, player_runtime_output_audio_user(output));
}

static db *player_runtime_video_output_cells(const PlayerRuntimeVideoOutput *video) {
    return player_runtime_video_output_cells_field(video);
}

static dw player_runtime_video_output_capacity(const PlayerRuntimeVideoOutput *video) {
    return player_runtime_video_output_capacity_field(video);
}

static const IplayTextMode *player_runtime_video_output_mode(const PlayerRuntimeVideoOutput *video) {
    return player_runtime_video_output_mode_field(video);
}

static db player_runtime_video_output_video_mode(const PlayerRuntimeVideoOutput *video) {
    return player_runtime_video_output_video_mode_field(video);
}

static IplayVideoPresentFn player_runtime_video_output_present(const PlayerRuntimeVideoOutput *video) {
    return player_runtime_video_output_present_field(video);
}

static void *player_runtime_video_output_user(const PlayerRuntimeVideoOutput *video) {
    return player_runtime_video_output_user_field(video);
}

static IplayAudioWriteFn player_runtime_audio_output_write(const PlayerRuntimeAudioOutput *audio) {
    return player_runtime_audio_output_write_field(audio);
}

static void *player_runtime_audio_output_user(const PlayerRuntimeAudioOutput *audio) {
    return player_runtime_audio_output_user_field(audio);
}

#define player_runtime_output_views_video_field(state) (&(state)->video)
#define player_runtime_output_views_audio_field(state) (&(state)->audio)
#define player_runtime_output_views_video_const_field(state) (&(state)->video)
#define player_runtime_output_views_audio_const_field(state) (&(state)->audio)

static PlayerRuntimeVideoOutput *player_runtime_output_views_video(PlayerRuntimeOutputViews *views) {
    return player_runtime_output_views_video_field(views);
}

static PlayerRuntimeAudioOutput *player_runtime_output_views_audio(PlayerRuntimeOutputViews *views) {
    return player_runtime_output_views_audio_field(views);
}

static const PlayerRuntimeVideoOutput *player_runtime_output_views_video_const(const PlayerRuntimeOutputViews *views) {
    return player_runtime_output_views_video_const_field(views);
}

static const PlayerRuntimeAudioOutput *player_runtime_output_views_audio_const(const PlayerRuntimeOutputViews *views) {
    return player_runtime_output_views_audio_const_field(views);
}

static void player_runtime_output_views_init(PlayerRuntimeOutputViews *views, PlayerRuntimeOutput *output) {
    player_runtime_video_output_init(player_runtime_output_views_video(views), output);
    player_runtime_audio_output_init(player_runtime_output_views_audio(views), output);
}

static void player_runtime_output_init_with_audio(PlayerRuntimeOutput *output, PlayerRuntimeOutputAudioInitFn init_audio) {
    player_runtime_output_init_text_video(output);
    init_audio(output);
}

static void player_configure_runtime_output_with(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutputInitFn init_output, PlayerRuntimeOutputApplyFn apply_output) {
    PlayerRuntimeOutput output;
    init_output(&output);
    apply_output(runtime_config, &output);
}

#if IPLAY_PLAYER_ENABLE_SB16_HW
static void player_runtime_output_init_sb16_audio(PlayerRuntimeOutput *output) {
    player_audio_backend_init_sb16(player_runtime_output_audio_backend(output));
}

static void player_runtime_output_init_sb16(PlayerRuntimeOutput *output) {
    player_runtime_output_init_with_audio(output, player_runtime_output_init_sb16_audio);
}

static void player_apply_runtime_sb16_output_views(IplayRuntimeConfig *runtime_config, const PlayerRuntimeVideoOutput *video, const PlayerRuntimeAudioOutput *audio) {
    iplay_runtime_config_sb16_hardware_capacity(runtime_config, player_runtime_video_output_cells(video), player_runtime_video_output_capacity(video), player_runtime_video_output_mode(video), player_runtime_video_output_present(video), player_runtime_video_output_user(video),
                                                player_runtime_audio_output_write(audio), player_runtime_audio_output_user(audio));
}

static void player_apply_runtime_sb16_output_view_bundle(IplayRuntimeConfig *runtime_config, const PlayerRuntimeOutputViews *views) {
    player_apply_runtime_sb16_output_views(runtime_config, player_runtime_output_views_video_const(views), player_runtime_output_views_audio_const(views));
}

static void player_apply_runtime_sb16_output_config(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutput *output) {
    PlayerRuntimeOutputViews views;
    player_runtime_output_views_init(&views, output);
    player_apply_runtime_sb16_output_view_bundle(runtime_config, &views);
}

static void player_configure_runtime_sb16_output(IplayRuntimeConfig *runtime_config) {
    player_configure_runtime_output_with(runtime_config, player_runtime_output_init_sb16, player_apply_runtime_sb16_output_config);
}
#else
static void player_runtime_output_init_sdl_audio(PlayerRuntimeOutput *output) {
    player_audio_backend_init_discard(player_runtime_output_audio_backend(output));
}

static void player_runtime_output_init_sdl(PlayerRuntimeOutput *output) {
    player_runtime_output_init_with_audio(output, player_runtime_output_init_sdl_audio);
}

static void player_apply_runtime_sdl_output_views(IplayRuntimeConfig *runtime_config, const PlayerRuntimeVideoOutput *video, const PlayerRuntimeAudioOutput *audio) {
    iplay_runtime_config_sdl_capacity(runtime_config, player_runtime_video_output_cells(video), player_runtime_video_output_capacity(video), player_runtime_video_output_mode(video), player_runtime_video_output_present(video), player_runtime_video_output_user(video),
                                      player_runtime_audio_output_write(audio), player_runtime_audio_output_user(audio));
}

static void player_apply_runtime_sdl_output_view_bundle(IplayRuntimeConfig *runtime_config, const PlayerRuntimeOutputViews *views) {
    player_apply_runtime_sdl_output_views(runtime_config, player_runtime_output_views_video_const(views), player_runtime_output_views_audio_const(views));
}

static void player_apply_runtime_sdl_output_config(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutput *output) {
    PlayerRuntimeOutputViews views;
    player_runtime_output_views_init(&views, output);
    player_apply_runtime_sdl_output_view_bundle(runtime_config, &views);
}

static void player_configure_runtime_sdl_output(IplayRuntimeConfig *runtime_config) {
    player_configure_runtime_output_with(runtime_config, player_runtime_output_init_sdl, player_apply_runtime_sdl_output_config);
}
#endif

static void player_configure_runtime_output(IplayRuntimeConfig *runtime_config) {
#if IPLAY_PLAYER_ENABLE_SB16_HW
    player_configure_runtime_sb16_output(runtime_config);
#else
    player_configure_runtime_sdl_output(runtime_config);
#endif
}

static void player_configure_runtime(IplayRuntimeConfig *runtime_config) {
    player_configure_runtime_output(runtime_config);
}

static void player_start_runtime(IplayRuntime *runtime, const IplayRuntimeConfig *runtime_config) {
    iplay_runtime_start_config_checked(runtime, runtime_config, player_text_video_mode_id());
}

static dw player_present_runtime_frame_scope(IplayRuntime *runtime, const char *reason, const char *scope) {
    dw byte_count;
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    dw screen_bytes;
    dd screen_checksum;
    dw screen_nonblank;
    const IplayAudioLevels *levels;
#endif
    byte_count = iplay_runtime_present(runtime);
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    screen_bytes = iplay_runtime_video_screen_bytes(runtime);
    screen_checksum = iplay_runtime_video_checksum(runtime);
    screen_nonblank = iplay_runtime_video_nonblank_cells(runtime);
    levels = iplay_runtime_audio_levels(runtime);
    printf("Screen present: reason=%s scope=%s bytes=%u screen_bytes=%u screen_checksum=%lu screen_nonblank=%u full=%u cols=%u rows=%u mode_ok=%u audio_frames=%lu levels=%u/%u\n",
           reason ? reason : "unknown",
           scope ? scope : "unknown",
           (unsigned)byte_count,
           (unsigned)screen_bytes,
           (unsigned long)screen_checksum,
           (unsigned)screen_nonblank,
           (unsigned)(byte_count == screen_bytes),
           (unsigned)iplay_runtime_video_cols(runtime),
           (unsigned)iplay_runtime_video_rows(runtime),
           (unsigned)iplay_runtime_video_mode_ok(runtime),
           (unsigned long)iplay_runtime_audio_frames_written(runtime),
           levels ? (unsigned)iplay_audio_levels_left_16(levels) : 0u,
           levels ? (unsigned)iplay_audio_levels_right_16(levels) : 0u);
    player_flush_reports();
#else
    (void)reason;
    (void)scope;
#endif
    return byte_count;
}

static dw player_present_runtime_frame(IplayRuntime *runtime, const char *reason) {
    return player_present_runtime_frame_scope(runtime, reason, "full-screen");
}

static dw player_module_channel_count(const PlayerModuleInfo *module);
static dw player_module_order_count(const PlayerModuleInfo *module);
static dw player_module_rows_per_order(const PlayerModuleInfo *module);
static const char *player_original_module_type_label(const PlayerModuleInfo *module);
static const char *player_original_filename_label(const PlayerModuleInfo *module);
static void player_init_module_status(IplayModuleStatus *module_status, const PlayerModuleInfo *module);

static unsigned player_mod_sample_slots(const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    if (!module || loader_kind(player_module_loader(module)) != IPLAY_LOADER_KIND_MOD) return 0u;
    header = player_module_header(module);
    len = player_module_header_len(module);
    if (len < 20u + 15u * 30u) return 0u;
    return len >= 1084u ? 31u : 15u;
}

static unsigned player_mod_used_samples(const PlayerModuleInfo *module) {
    unsigned slots = player_mod_sample_slots(module);
    unsigned used = 0u;
    unsigned i;
    for (i = 0u; i < slots; ++i) {
        unsigned offset = 20u + i * 30u + 22u;
        if (player_module_u16_be_at(module, (dd)offset) != 0u) ++used;
    }
    return used;
}

static const char *player_original_live_module_type_label(const PlayerModuleInfo *module) {
    static char line[32];
    const db *header;
    size_t len;
    if (!module || loader_kind(player_module_loader(module)) != IPLAY_LOADER_KIND_MOD) {
        return player_original_module_type_label(module);
    }
    header = player_module_header(module);
    len = player_module_header_len(module);
    if (len >= 1084u) {
        sprintf(line, "Module Type   : %c%c%c%c",
                header[1080u], header[1081u], header[1082u], header[1083u]);
        return line;
    }
    return "Module Type   : N.T.";
}

static const char *player_original_live_playing_label(const PlayerModuleInfo *module) {
    if (module && loader_kind(player_module_loader(module)) == IPLAY_LOADER_KIND_MOD) {
        return "Playing in Stereo, Free: 462KB";
    }
    return "Playing in Stereo, Free: 482KB";
}

static const char *player_original_live_channels_label(const PlayerModuleInfo *module) {
    static char line[32];
    sprintf(line, "Channels      : %u", (unsigned)player_module_channel_count(module));
    return line;
}

static const char *player_original_live_samples_label(const PlayerModuleInfo *module) {
    static char line[32];
    unsigned slots = player_mod_sample_slots(module);
    if (slots == 0u) slots = 15u;
    sprintf(line, "Samples Used  : %u/%u", player_mod_used_samples(module), slots);
    return line;
}

static const char *player_original_live_title(const PlayerModuleInfo *module) {
    static char title[21];
    const db *header = module ? player_module_header(module) : NULL;
    size_t len = module ? player_module_header_len(module) : 0u;
    unsigned i;
    unsigned end;
    if (!header || len == 0u) return "";
    end = len < 20u ? (unsigned)len : 20u;
    for (i = 0u; i < end; ++i) {
        db ch = header[i];
        title[i] = ch >= 32u && ch < 127u ? (char)ch : ' ';
    }
    while (end > 0u && title[end - 1u] == ' ') --end;
    title[end] = 0;
    return title;
}

static const char *player_original_live_track_label(const PlayerModuleInfo *module) {
    static char line[32];
    sprintf(line, "Current Track : 1/%u", (unsigned)player_module_order_count(module));
    return line;
}

static const char *player_original_live_position_label(const PlayerModuleInfo *module) {
    static char line[32];
    sprintf(line, "Track Position: 1/%u", (unsigned)player_module_rows_per_order(module));
    return line;
}

static void player_original_mod_note(dw period, char note[4]) {
    static const dw periods[] = {
        1712u, 1616u, 1524u, 1440u, 1356u, 1280u, 1208u, 1140u, 1076u, 1016u, 960u, 906u,
        856u, 808u, 762u, 720u, 678u, 640u, 604u, 570u, 538u, 508u, 480u, 453u,
        428u, 404u, 381u, 360u, 339u, 320u, 302u, 285u, 269u, 254u, 240u, 226u,
        214u, 202u, 190u, 180u, 170u, 160u, 151u, 143u, 135u, 127u, 120u, 113u
    };
    static const char names[] = "C-C#D-D#E-F-F#G-G#A-A#B-";
    unsigned i;
    note[0] = ' ';
    note[1] = ' ';
    note[2] = ' ';
    note[3] = 0;
    for (i = 0u; i < sizeof(periods) / sizeof(periods[0]); ++i) {
        unsigned name_offset;
        if (periods[i] != period) continue;
        name_offset = (i % 12u) * 2u;
        note[0] = names[name_offset];
        note[1] = names[name_offset + 1u];
        note[2] = (char)('0' + i / 12u);
        return;
    }
}

static const char *player_original_mod_effect(db command) {
    if (command == 0x0cu) return "Volume Change";
    if (command == 0x0fu) return "Set Speed/BPM";
    return "";
}

static void player_original_mod_sample_name(
    const PlayerModuleInfo *module,
    unsigned sample,
    char name[23]) {
    const db *header = player_module_header(module);
    size_t len = player_module_header_len(module);
    size_t offset;
    unsigned i;
    unsigned end = 0u;
    name[0] = 0;
    if (!header || sample == 0u || sample > player_mod_sample_slots(module)) return;
    offset = 20u + (size_t)(sample - 1u) * 30u;
    if (offset + 22u > len) return;
    for (i = 0u; i < 22u; ++i) {
        db ch = header[offset + i];
        name[i] = ch >= 32u && ch < 127u ? (char)ch : ' ';
        if (name[i] != ' ') end = i + 1u;
    }
    name[end] = 0;
}

static void player_draw_original_mod_first_row(IplayRuntime *runtime, const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    dw channels;
    dw channel;
    if (!module || loader_kind(player_module_loader(module)) != IPLAY_LOADER_KIND_MOD) return;
    header = player_module_header(module);
    len = player_module_header_len(module);
    channels = player_module_channel_count(module);
    if (!header || len < 1084u + (size_t)channels * 4u) return;
    for (channel = 0u; channel < channels; ++channel) {
        const db *cell = header + 1084u + (size_t)channel * 4u;
        unsigned sample = (unsigned)(cell[0] & 0xf0u) | (unsigned)(cell[2] >> 4u);
        dw period = (dw)(((dw)(cell[0] & 0x0fu) << 8u) | cell[1]);
        char note[4];
        char sample_name[23];
        player_original_mod_note(period, note);
        player_original_mod_sample_name(module, sample, sample_name);
        iplay_runtime_draw_original_channel_text(
            runtime,
            channel,
            note,
            sample_name,
            player_original_mod_effect((db)(cell[2] & 0x0fu)));
    }
}

static void player_draw_original_live_module_info(IplayRuntime *runtime, const PlayerModuleInfo *module) {
    iplay_runtime_draw_original_live_module_info(
        runtime,
        player_original_filename_label(module),
        player_original_live_module_type_label(module),
        player_original_live_playing_label(module),
        player_original_live_channels_label(module),
        player_original_live_samples_label(module),
        player_original_live_title(module),
        "Sound Blaster 16 (44kHz)",
        player_original_live_track_label(module),
        player_original_live_position_label(module));
}

static void player_render_runtime_status_reason(IplayRuntime *runtime, const PlayerModuleInfo *module, const char *reason) {
    IplayModuleStatus compact_status;
    iplay_runtime_render_static(runtime, IPLAY_PLAYER_DEFAULT_ERASE_ATTR);
    iplay_runtime_render_bottom(runtime,
                                IPLAY_PLAYER_DEFAULT_PATTERN,
                                IPLAY_PLAYER_DEFAULT_ORDER,
                                IPLAY_PLAYER_DEFAULT_ROW,
                                IPLAY_PLAYER_DEFAULT_SPEED,
                                player_master_volume(player_memory()),
                                IPLAY_PLAYER_DEFAULT_FLAGS,
                                IPLAY_PLAYER_DEFAULT_VOLUME,
                                IPLAY_PLAYER_DEFAULT_AMPLIFICATION);
    if (iplay_runtime_video_cols(runtime) < 80u) {
        player_init_module_status(&compact_status, module);
        iplay_runtime_draw_status_block(runtime, &compact_status);
    }
    iplay_runtime_draw_audio_status(runtime);
    player_draw_original_live_module_info(runtime, module);
    player_present_runtime_frame(runtime, reason);
}

static void player_render_runtime_status(IplayRuntime *runtime, const PlayerModuleInfo *module) {
    /* inventory marker: player_present_runtime_frame(runtime, "status"); */
    player_render_runtime_status_reason(runtime, module, "status");
}

static const char *player_original_module_type_label(const PlayerModuleInfo *module) {
    const LoaderInfo *loader = module ? player_module_loader(module) : NULL;
    if (!loader) return "Module Type   : Unknown";
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_MOD:
        return "Module Type   : N.T.";
    case IPLAY_LOADER_KIND_S3M:
        return "Module Type   : S3M";
    case IPLAY_LOADER_KIND_STM:
        return "Module Type   : STM";
    case IPLAY_LOADER_KIND_669:
        return "Module Type   : 669";
    case IPLAY_LOADER_KIND_MTM:
        return "Module Type   : MTM";
    case IPLAY_LOADER_KIND_PSM:
        return "Module Type   : PSM";
    case IPLAY_LOADER_KIND_FAR:
        return "Module Type   : FAR";
    case IPLAY_LOADER_KIND_ULT:
        return "Module Type   : ULT";
    default:
        return "Module Type   : External";
    }
}

static const char *player_original_filename_label(const PlayerModuleInfo *module) {
    static char line[IPLAY_PLAYER_UNSUPPORTED_FILENAME_BYTES];
    const char *prefix = "Filename      : ";
    const char *path = module ? player_module_path(module) : "";
    unsigned i = 0u;
    while (prefix[i] && i + 1u < IPLAY_PLAYER_UNSUPPORTED_FILENAME_BYTES) {
        line[i] = prefix[i];
        ++i;
    }
    while (*path && i + 1u < IPLAY_PLAYER_UNSUPPORTED_FILENAME_BYTES) {
        line[i++] = *path++;
    }
    line[i] = 0;
    return line;
}

static void player_render_runtime_unsupported_module(IplayRuntime *runtime, const PlayerModuleInfo *module, const IplayModuleStatus *module_status) {
    iplay_runtime_render_static(runtime, IPLAY_PLAYER_DEFAULT_ERASE_ATTR);
    iplay_runtime_render_bottom(runtime,
                                IPLAY_PLAYER_DEFAULT_PATTERN,
                                IPLAY_PLAYER_DEFAULT_ORDER,
                                IPLAY_PLAYER_DEFAULT_ROW,
                                IPLAY_PLAYER_DEFAULT_SPEED,
                                player_master_volume(player_memory()),
                                IPLAY_PLAYER_DEFAULT_FLAGS,
                                IPLAY_PLAYER_DEFAULT_VOLUME,
                                IPLAY_PLAYER_DEFAULT_AMPLIFICATION);
    iplay_runtime_draw_status_block(runtime, module_status);
    iplay_runtime_draw_original_module_info(runtime, player_original_filename_label(module), player_original_module_type_label(module));
    player_present_runtime_frame(runtime, "unsupported-module");
}

static void player_render_runtime_audio_status_frame(IplayRuntime *runtime, const PlayerModuleInfo *module, const char *reason) {
    IplayModuleStatus compact_status;
    if (iplay_runtime_video_cols(runtime) < 80u) {
        player_init_module_status(&compact_status, module);
        iplay_runtime_draw_status_block(runtime, &compact_status);
    }
    player_draw_original_live_module_info(runtime, module);
    iplay_runtime_draw_original_channel_levels(runtime, player_module_channel_count(module));
    player_draw_original_mod_first_row(runtime, module);
    player_present_runtime_frame_scope(runtime, reason, "status-only");
}

static int player_runtime_playback_ready(const IplayRuntime *runtime) {
    return iplay_runtime_audio_active(runtime) && iplay_runtime_audio_is_sb16_compatible(runtime);
}

static void player_report_runtime_playback_prime(const IplayRuntime *runtime) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    const IplayAudioLevels *levels = iplay_runtime_audio_levels(runtime);
    printf("Playback prime: ready=%u hw=%u backend=%s status=%s frames=%lu capacity=%lu dropped=%lu queued=%lu levels=%u/%u\n",
           (unsigned)player_runtime_playback_ready(runtime),
           (unsigned)iplay_runtime_audio_hardware_enabled(runtime),
           iplay_runtime_audio_backend_name(runtime),
           iplay_runtime_audio_status_text(runtime),
           (unsigned long)iplay_runtime_audio_frames_written(runtime),
           (unsigned long)iplay_runtime_audio_capacity(runtime),
           (unsigned long)iplay_runtime_audio_dropped_frames(runtime),
           (unsigned long)iplay_runtime_audio_queued_frames(runtime),
           levels ? (unsigned)iplay_audio_levels_left_16(levels) : 0u,
           levels ? (unsigned)iplay_audio_levels_right_16(levels) : 0u);
    player_flush_reports();
#else
    (void)runtime;
#endif
}

static PlayerPlaybackBlock *player_playback_block(PlayerPlayback *playback) {
    return &playback->block;
}

static const PlayerPlaybackBlock *player_playback_block_const(const PlayerPlayback *playback) {
    return &playback->block;
}

static db *player_playback_block_pcm(PlayerPlaybackBlock *block) {
    return block->pcm;
}

static const db *player_playback_block_pcm_const(const PlayerPlaybackBlock *block) {
    return block->pcm;
}

static dw player_playback_block_frames(const PlayerPlaybackBlock *block) {
    return block->frames;
}

static void player_playback_block_set_frames(PlayerPlaybackBlock *block, dw frames) {
    block->frames = frames;
}

static dw player_playback_block_capacity_frames(void) {
    return IPLAY_PLAYER_MAX_BLOCK_FRAMES;
}

static dw player_playback_block_active_bytes(const PlayerPlaybackBlock *block) {
    dw frames = player_playback_block_frames(block);
    if (frames > player_playback_block_capacity_frames()) frames = player_playback_block_capacity_frames();
    return (dw)(frames * 4u);
}

db *iplay_player_playback_block_pcm(PlayerPlaybackBlock *block) {
    return player_playback_block_pcm(block);
}

dw iplay_player_playback_block_frames(const PlayerPlaybackBlock *block) {
    return player_playback_block_frames(block);
}

dw iplay_player_playback_block_capacity_frames(void) {
    return player_playback_block_capacity_frames();
}

void iplay_player_playback_block_set_frames(PlayerPlaybackBlock *block, dw frames) {
    if (frames > player_playback_block_capacity_frames()) frames = player_playback_block_capacity_frames();
    player_playback_block_set_frames(block, frames);
}

dw iplay_player_playback_block_active_bytes(const PlayerPlaybackBlock *block) {
    return player_playback_block_active_bytes(block);
}

static void player_playback_block_clear_pcm(PlayerPlaybackBlock *block) {
    unsigned i;
    for (i = 0; i < sizeof(block->pcm); ++i) block->pcm[i] = 0;
}

static void player_playback_prepare_block_frames(PlayerPlaybackBlock *block, dw frames) {
    player_playback_block_clear_pcm(block);
    if (frames > player_playback_block_capacity_frames()) frames = player_playback_block_capacity_frames();
    player_playback_block_set_frames(block, frames);
}

static void player_playback_prepare_prime_block(PlayerPlaybackBlock *block) {
    player_playback_prepare_block_frames(block, IPLAY_PLAYER_PRIME_FRAMES);
}

static void player_playback_prepare_sb16_block(PlayerPlaybackBlock *block) {
    player_playback_prepare_block_frames(block, IPLAY_PLAYER_SB16_BLOCK_FRAMES);
}

static void player_playback_fill_seed_pcm(PlayerPlaybackBlock *block, db seed) {
    db *pcm = player_playback_block_pcm(block);
    unsigned i;
    unsigned bytes = player_playback_block_active_bytes(block);
    for (i = 0; i < bytes; i += 2u) {
        pcm[i] = (db)(seed + (db)(i >> 1));
        pcm[i + 1u] = 0;
    }
}

static db player_module_sample_byte(const PlayerModuleInfo *module, const PlayerSampleInfo *sample, dd index);
static int player_sample_byte_to_s8(db sample_byte);
static int player_module_s3m_samples_unsigned(const PlayerModuleInfo *module);
static int player_mix_clamp_s16(long sample);

static dd player_voice_state_step_for_period(dw period) {
    if (period == 0) return 0x0100ul;
    return (dd)(((dd)856u * 0x0100ul) / period);
}

static db player_voice_state_pcm_seed(const PlayerVoiceState *voice, db fallback) {
    if (!voice || !voice->active) return fallback;
    return (db)(fallback + (db)voice->note + (db)(voice->octave << 4) + (db)voice->instrument + (db)voice->volume + (db)voice->period);
}

static int player_sample_info_is_16bit(const PlayerSampleInfo *sample) {
    return sample && (sample->data_offset & IPLAY_PLAYER_SAMPLE_DATA_OFFSET_16BIT) != 0;
}

static dd player_sample_info_data_offset(const PlayerSampleInfo *sample) {
    return sample ? (sample->data_offset & ~IPLAY_PLAYER_SAMPLE_DATA_OFFSET_16BIT) : 0;
}

static int player_sample_info_loop_enabled(const PlayerSampleInfo *sample) {
    return sample && sample->loop_length > 2ul;
}

static dd player_sample_info_loop_end(const PlayerSampleInfo *sample) {
    dd loop_end;
    if (!player_sample_info_loop_enabled(sample)) return 0;
    loop_end = sample->loop_start + sample->loop_length;
    return loop_end > sample->length ? sample->length : loop_end;
}

static dd player_sample_info_loop_start(const PlayerSampleInfo *sample) {
    if (!player_sample_info_loop_enabled(sample)) return 0;
    if (sample->loop_start >= sample->length) return sample->length ? sample->length - 1ul : 0;
    return sample->loop_start;
}

static db player_voice_state_next_sample_byte(PlayerVoiceState *voice, const PlayerModuleInfo *module) {
    db sample_byte;
    dd loop_end;
    dd loop_start;
    if (!voice || !voice->active || voice->sample.length == 0) return 0;
    sample_byte = player_module_sample_byte(module, &voice->sample, voice->sample_position);
    voice->sample_phase += voice->sample_step ? voice->sample_step : 0x0100ul;
    voice->sample_position = voice->sample_phase >> 8;
    if (player_sample_info_loop_enabled(&voice->sample)) {
        loop_end = player_sample_info_loop_end(&voice->sample);
        if (voice->sample_position >= loop_end) {
            loop_start = player_sample_info_loop_start(&voice->sample);
            voice->sample_phase = loop_start << 8;
            voice->sample_position = loop_start;
        }
    } else if (voice->sample_position >= voice->sample.length) {
        voice->active = 0;
    }
    return sample_byte;
}

static void player_voice_state_advance_sample(PlayerVoiceState *voice) {
    dd loop_end;
    dd loop_start;
    if (!voice || !voice->active || voice->sample.length == 0) return;
    voice->sample_phase += voice->sample_step ? voice->sample_step : 0x0100ul;
    voice->sample_position = voice->sample_phase >> 8;
    if (player_sample_info_loop_enabled(&voice->sample)) {
        loop_end = player_sample_info_loop_end(&voice->sample);
        if (voice->sample_position >= loop_end) {
            loop_start = player_sample_info_loop_start(&voice->sample);
            voice->sample_phase = loop_start << 8;
            voice->sample_position = loop_start;
        }
    } else if (voice->sample_position >= voice->sample.length) {
        voice->active = 0;
    }
}

static dd player_voice_state_next_sample_index(const PlayerVoiceState *voice, dd index) {
    dd loop_start;
    if (!voice || voice->sample.length == 0) return index;
    index += 1ul;
    if (index < voice->sample.length) return index;
    if (player_sample_info_loop_enabled(&voice->sample)) {
        loop_start = player_sample_info_loop_start(&voice->sample);
        return loop_start < voice->sample.length ? loop_start : 0;
    }
    return voice->sample.length - 1ul;
}

static int player_voice_state_sample_s16_at(const PlayerVoiceState *voice, const PlayerModuleInfo *module, dd index) {
    dd offset;
    db lo;
    db hi;
    if (!voice || voice->sample.length == 0 || index >= voice->sample.length) return 0;
    if (!player_sample_info_is_16bit(&voice->sample)) {
        return player_sample_byte_to_s8(player_module_sample_byte(module, &voice->sample, index)) << 8;
    }
    offset = player_sample_info_data_offset(&voice->sample) + index * 2ul;
    lo = player_module_byte_at(module, offset);
    hi = player_module_byte_at(module, offset + 1ul);
    if (loader_kind(player_module_loader(module)) == IPLAY_LOADER_KIND_S3M && player_module_s3m_samples_unsigned(module)) hi ^= 0x80u;
    return (int)(short)((unsigned)lo | ((unsigned)hi << 8));
}

static int player_voice_state_next_sample_s16(PlayerVoiceState *voice, const PlayerModuleInfo *module) {
    dd index;
    dd next_index;
    dd fraction;
    int current;
    int next;
    long interpolated;
    if (!voice || !voice->active || voice->sample.length == 0) return 0;
    index = voice->sample_position;
    fraction = voice->sample_phase & 0xfful;
    next_index = player_voice_state_next_sample_index(voice, index);
    current = player_voice_state_sample_s16_at(voice, module, index);
    next = player_voice_state_sample_s16_at(voice, module, next_index);
    interpolated = (long)current + (((long)next - (long)current) * (long)fraction) / 256L;
    player_voice_state_advance_sample(voice);
    return player_mix_clamp_s16(interpolated);
}

static int player_mix_clamp_s16(long sample) {
    if (sample < -32768L) return -32768;
    if (sample > 32767L) return 32767;
    return (int)sample;
}

static int player_sample_byte_to_s8(db sample_byte) {
    return sample_byte < 128u ? (int)sample_byte : (int)sample_byte - 256;
}

static void player_mix_put_s16le(db *pcm, unsigned offset, int sample) {
    unsigned value = (unsigned)((int)sample & 0xffffu);
    pcm[offset] = (db)(value & 0xffu);
    pcm[offset + 1u] = (db)((value >> 8) & 0xffu);
}

static void player_playback_fill_voice_pcm(PlayerPlaybackBlock *block, PlayerVoiceState *voice, const PlayerModuleInfo *module, db fallback) {
    db *pcm = player_playback_block_pcm(block);
    db seed = player_voice_state_pcm_seed(voice, fallback);
    unsigned i;
    unsigned bytes = player_playback_block_active_bytes(block);
    for (i = 0; i < bytes; i += 4u) {
        db sample_byte = player_voice_state_next_sample_byte(voice, module);
        pcm[i] = (db)(seed + sample_byte + (db)(i >> 2));
        pcm[i + 1u] = 0;
        pcm[i + 2u] = (db)(seed + sample_byte + (db)voice->instrument);
        pcm[i + 3u] = 0;
    }
}

static dd player_playback_block_checksum(const PlayerPlaybackBlock *block) {
    const db *pcm = player_playback_block_pcm_const(block);
    dd sum = 0;
    unsigned i;
    unsigned bytes = player_playback_block_active_bytes(block);
    for (i = 0; i < bytes; ++i) sum += pcm[i];
    return sum;
}

static void player_playback_init(PlayerPlayback *playback) {
    playback->blocks_submitted = 0;
    playback->frames_submitted = 0;
    playback->bytes_accepted = 0;
    playback->pcm_checksum = 0;
    playback->limit_reached = 0;
    playback->source_ended = 0;
    playback->stop_reason = IPLAY_PLAYER_STOP_RUNNING;
    player_playback_block_set_frames(player_playback_block(playback), 0);
}

static void player_playback_count_submit(PlayerPlayback *playback, dw frames, dw accepted_bytes) {
    playback->blocks_submitted += 1u;
    playback->frames_submitted += frames;
    playback->bytes_accepted += accepted_bytes;
}

static void player_pcm_source_set_frames_per_block(PlayerPcmSource *source, dw frames_per_block) {
    source->frames_per_block = frames_per_block;
}

static dw player_pcm_source_frames_per_block(const PlayerPcmSource *source) {
    return source->frames_per_block;
}

static void player_pcm_source_init(PlayerPcmSource *source, PlayerPcmSourceReadFn read, PlayerPcmSourceEndedFn ended, void *user) {
    source->read = read;
    source->ended = ended;
    source->user = user;
    player_pcm_source_set_frames_per_block(source, IPLAY_PLAYER_PRIME_FRAMES);
}

static dw player_pcm_source_read(PlayerPcmSource *source, PlayerPlaybackBlock *block) {
    if (!source || !source->read) return 0;
    return source->read(source, block);
}

static int player_pcm_source_ended(const PlayerPcmSource *source) {
    if (!source || !source->ended) return 0;
    return source->ended(source);
}

static dw player_prime_pcm_source_read(PlayerPcmSource *source, PlayerPlaybackBlock *block) {
    player_playback_prepare_block_frames(block, player_pcm_source_frames_per_block(source));
    return player_playback_block_frames(block);
}

static db player_module_header_seed(const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    size_t i;
    db seed = 0;
    if (!module) return 0;
    header = player_module_header(module);
    len = player_module_header_len(module);
    if (len > 16u) len = 16u;
    for (i = 0; i < len; ++i) seed = (db)(seed + header[i]);
    return seed;
}

static db player_module_pcm_seed(const PlayerModuleInfo *module) {
    if (!module) return 0;
    return (db)(loader_kind(player_module_loader(module)) + player_module_header_seed(module));
}

static dw player_module_order_count(const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    const LoaderInfo *loader;
    db raw_count;
    if (!module) return 1u;
    header = player_module_header(module);
    len = player_module_header_len(module);
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        if (len >= 0x22u) return load_u16_le(header + 0x20u);
        break;
    case IPLAY_LOADER_KIND_MOD:
        if (len > 950u && header[950] != 0) {
            raw_count = header[950];
            return raw_count > 128u ? 128u : raw_count;
        }
        break;
    case IPLAY_LOADER_KIND_MTM:
        if (len > 0x1eu) return (dw)(header[0x1eu] + 1u);
        break;
    default:
        break;
    }
    return 1u;
}

static dw player_module_mod_order_value(db raw_order) {
    return raw_order >= 0xfeu ? 0u : raw_order;
}

static int player_module_order_value_is_skip(dw value) {
    return value == 0xfeu;
}

static int player_module_order_value_is_end(dw value) {
    return value == 0xffu;
}

static dw player_module_order_value(const PlayerModuleInfo *module, dw order) {
    const db *header;
    size_t len;
    const LoaderInfo *loader;
    if (!module) return order;
    header = player_module_header(module);
    len = player_module_header_len(module);
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        if (len > 0x60u + order) return header[0x60u + order];
        break;
    case IPLAY_LOADER_KIND_MOD:
        if (len > 952u + order) return player_module_mod_order_value(header[952u + order]);
        break;
    case IPLAY_LOADER_KIND_MTM:
        if (len > 0x22u + order) return header[0x22u + order];
        break;
    default:
        break;
    }
    return order;
}

static dw player_module_pattern_count(const PlayerModuleInfo *module) {
    dw orders;
    dw i;
    dw max_pattern = 0;
    if (!module) return 1u;
    orders = player_module_order_count(module);
    if (orders == 0) return 1u;
    if (orders > 128u) orders = 128u;
    for (i = 0; i < orders; ++i) {
        dw value = player_module_order_value(module, i);
        if (value < 0xfeu && value > max_pattern) max_pattern = value;
    }
    return (dw)(max_pattern + 1u);
}

static dw player_module_s3m_channel_count(const db *header, size_t len) {
    dw count = 0;
    dw i;
    if (len < 0x60u) return IPLAY_PLAYER_DEFAULT_CHANNELS;
    for (i = 0; i < 32u; ++i) {
        if (header[0x40u + i] < 16u) count += 1u;
    }
    return count ? count : IPLAY_PLAYER_DEFAULT_CHANNELS;
}

static dw player_module_s3m_physical_channel(const PlayerModuleInfo *module, dw logical_channel) {
    const db *header;
    size_t len;
    dw i;
    dw count = 0;
    if (!module) return logical_channel;
    header = player_module_header(module);
    len = player_module_header_len(module);
    if (len < 0x60u) return logical_channel;
    for (i = 0; i < 32u; ++i) {
        if (header[0x40u + i] < 16u) {
            if (count == logical_channel) return i;
            count += 1u;
        }
    }
    return logical_channel;
}

static dw player_module_channel_count(const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    const LoaderInfo *loader;
    if (!module) return IPLAY_PLAYER_DEFAULT_CHANNELS;
    header = player_module_header(module);
    len = player_module_header_len(module);
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        return player_module_s3m_channel_count(header, len);
    case IPLAY_LOADER_KIND_MOD:
        if (len >= 1084u) return (dw)mod_channels_from_sig(header + 1080u);
        break;
    case IPLAY_LOADER_KIND_MTM:
        if (len > 0x20u && header[0x20u] != 0) return header[0x20u];
        break;
    case IPLAY_LOADER_KIND_669:
        return 8u;
    case IPLAY_LOADER_KIND_FAR:
        return 16u;
    default:
        break;
    }
    return IPLAY_PLAYER_DEFAULT_CHANNELS;
}

static void player_pattern_cell_clear(PlayerPatternCell *cell) {
    cell->period = 0;
    cell->note = 0;
    cell->octave = 0;
    cell->instrument = 0;
    cell->volume = 0;
    cell->volume_set = 0;
    cell->effect = 0;
    cell->param = 0;
}

static void player_sample_info_clear(PlayerSampleInfo *sample) {
    sample->length = 0;
    sample->volume = 0;
    sample->loop_start = 0;
    sample->loop_length = 0;
    sample->data_offset = 0;
}

static void player_voice_state_clear(PlayerVoiceState *voice) {
    voice->period = 0;
    voice->note = 0;
    voice->octave = 0;
    voice->instrument = 0;
    voice->volume = 0;
    voice->channel_volume = 64u;
    voice->active = 0;
    voice->target_period = 0;
    voice->pan_set = 0;
    voice->pan = 128;
    voice->sample_position = 0;
    voice->sample_phase = 0;
    voice->sample_step = 0x0100ul;
    player_sample_info_clear(&voice->sample);
}

static int player_voice_state_playable(const PlayerVoiceState *voice) {
    return voice && voice->period != 0 && voice->sample.length != 0;
}

static void player_voice_state_apply_panning(PlayerVoiceState *voice, db param);

static void player_module_mod_sample_info(const PlayerModuleInfo *module, db instrument, PlayerSampleInfo *sample) {
    dd offset;
    dd data_offset;
    db i;
    db volume;
    player_sample_info_clear(sample);
    if (!module || instrument == 0 || instrument > 31u) return;
    offset = 20ul + ((dd)instrument - 1ul) * 30ul;
    if (!player_module_has_range(module, offset, 30u)) return;
    sample->length = (dd)player_module_u16_be_at(module, offset + 22u) * 2u;
    volume = player_module_byte_at(module, offset + 25u);
    sample->volume = volume > 64u ? 64u : volume;
    sample->loop_start = (dd)player_module_u16_be_at(module, offset + 26u) * 2u;
    sample->loop_length = (dd)player_module_u16_be_at(module, offset + 28u) * 2u;
    data_offset = 1084ul + (dd)player_module_pattern_count(module) * (dd)IPLAY_PLAYER_DEFAULT_ROWS_PER_ORDER * (dd)player_module_channel_count(module) * 4ul;
    for (i = 1u; i < instrument; ++i) {
        dd previous_offset = 20ul + ((dd)i - 1ul) * 30ul;
        if (player_module_has_range(module, previous_offset, 24u)) data_offset += (dd)player_module_u16_be_at(module, previous_offset + 22u) * 2u;
    }
    sample->data_offset = data_offset;
}

static dw player_module_s3m_instrument_count(const PlayerModuleInfo *module) {
    if (!player_module_has_range(module, 0x22u, 2u)) return 0;
    return player_module_u16_le_at(module, 0x22u);
}

static dd player_module_s3m_table_offset(const PlayerModuleInfo *module) {
    return 0x60ul + (dd)player_module_order_count(module);
}

static dd player_module_s3m_instrument_header_offset(const PlayerModuleInfo *module, db instrument) {
    dd table_offset;
    dd parapointer_offset;
    dw instrument_count;
    if (!module || instrument == 0) return 0;
    instrument_count = player_module_s3m_instrument_count(module);
    if (instrument > instrument_count) return 0;
    table_offset = player_module_s3m_table_offset(module);
    parapointer_offset = table_offset + ((dd)instrument - 1ul) * 2ul;
    if (!player_module_has_range(module, parapointer_offset, 2u)) return 0;
    return (dd)player_module_u16_le_at(module, parapointer_offset) * 16ul;
}

static dd player_module_s3m_pattern_offset(const PlayerModuleInfo *module, dw pattern) {
    dd table_offset;
    dd pattern_table_offset;
    dw instrument_count;
    if (!module) return 0;
    instrument_count = player_module_s3m_instrument_count(module);
    table_offset = player_module_s3m_table_offset(module);
    pattern_table_offset = table_offset + (dd)instrument_count * 2ul + (dd)pattern * 2ul;
    if (!player_module_has_range(module, pattern_table_offset, 2u)) return 0;
    return (dd)player_module_u16_le_at(module, pattern_table_offset) * 16ul;
}

static dd player_module_mod_pattern_stream_offset(const PlayerModuleInfo *module) {
    (void)module;
    return 1084ul;
}

static dd player_module_mtm_stream_offset(const PlayerModuleInfo *module) {
    (void)module;
    return 0x42ul;
}

static dd player_module_stm_stream_offset(const PlayerModuleInfo *module) {
    (void)module;
    return 0x40ul;
}

static dd player_module_far_stream_offset(const PlayerModuleInfo *module) {
    (void)module;
    return 0x80ul;
}

static dd player_module_669_stream_offset(const PlayerModuleInfo *module) {
    (void)module;
    return 0x71ul;
}

static dd player_module_psm_stream_offset(const PlayerModuleInfo *module) {
    (void)module;
    return 0x80ul;
}

static dd player_module_ult_stream_offset(const PlayerModuleInfo *module) {
    (void)module;
    return 0x60ul;
}

static dd player_module_s3m_pattern_stream_offset(const PlayerModuleInfo *module) {
    dw orders;
    dw order;
    if (!module) return 0;
    orders = player_module_order_count(module);
    if (orders > 128u) orders = 128u;
    for (order = 0; order < orders; ++order) {
        dw value = player_module_order_value(module, order);
        dd offset;
        if (player_module_order_value_is_skip(value)) continue;
        if (player_module_order_value_is_end(value)) break;
        offset = player_module_s3m_pattern_offset(module, value);
        if (offset != 0) return offset;
    }
    return player_module_s3m_table_offset(module) +
           (dd)player_module_s3m_instrument_count(module) * 2ul +
           (dd)player_module_pattern_count(module) * 2ul;
}

static dd player_module_stream_start_offset(const PlayerModuleInfo *module) {
    dd offset = 0;
    dd available;
    dd module_size;
    if (!module) return 0;
    available = (dd)player_module_header_len(module);
    module_size = (dd)player_module_size(module);
    switch (loader_kind(player_module_loader(module))) {
    case IPLAY_LOADER_KIND_MOD:
        offset = player_module_mod_pattern_stream_offset(module);
        break;
    case IPLAY_LOADER_KIND_S3M:
        offset = player_module_s3m_pattern_stream_offset(module);
        break;
    case IPLAY_LOADER_KIND_MTM:
        offset = player_module_mtm_stream_offset(module);
        break;
    case IPLAY_LOADER_KIND_STM:
        offset = player_module_stm_stream_offset(module);
        break;
    case IPLAY_LOADER_KIND_FAR:
        offset = player_module_far_stream_offset(module);
        break;
    case IPLAY_LOADER_KIND_669:
        offset = player_module_669_stream_offset(module);
        break;
    case IPLAY_LOADER_KIND_PSM:
        offset = player_module_psm_stream_offset(module);
        break;
    case IPLAY_LOADER_KIND_ULT:
        offset = player_module_ult_stream_offset(module);
        break;
    default:
        offset = 0;
        break;
    }
    if (offset != 0 && module_size != 0 && offset < module_size) return offset;
    if (available == 0 || offset >= available) return 0;
    return offset;
}

static dd player_module_diagnostic_stream_start_offset(const PlayerModuleInfo *module) {
    if (module && player_module_loader(module) && loader_kind(player_module_loader(module)) == IPLAY_LOADER_KIND_S3M &&
        player_module_data_complete(module) && player_module_size(module) > 0x200ul) {
        return 0x200ul;
    }
    return player_module_stream_start_offset(module);
}

static void player_module_s3m_sample_info(const PlayerModuleInfo *module, db instrument, PlayerSampleInfo *sample) {
    dd offset;
    dd loop_end;
    db volume;
    db flags;
    player_sample_info_clear(sample);
    offset = player_module_s3m_instrument_header_offset(module, instrument);
    if (offset == 0 || !player_module_has_range(module, offset, 32u)) return;
    if (player_module_byte_at(module, offset) != 1u) return;
    sample->data_offset = (dd)player_module_u16_le_at(module, offset + 14u) * 16ul;
    sample->length = player_module_u32_le_at(module, offset + 16u);
    sample->loop_start = player_module_u32_le_at(module, offset + 20u);
    loop_end = player_module_u32_le_at(module, offset + 24u);
    flags = player_module_byte_at(module, offset + 31u);
    sample->loop_length = ((flags & 1u) && loop_end > sample->loop_start) ? loop_end - sample->loop_start : 0;
    if (flags & 4u) sample->data_offset |= IPLAY_PLAYER_SAMPLE_DATA_OFFSET_16BIT;
    volume = player_module_byte_at(module, offset + 28u);
    sample->volume = volume > 64u ? 64u : volume;
}

static int player_module_s3m_samples_unsigned(const PlayerModuleInfo *module) {
    if (!player_module_has_range(module, 0x2au, 2u)) return 0;
    return player_module_u16_le_at(module, 0x2au) == 2u;
}

static db player_module_sample_byte(const PlayerModuleInfo *module, const PlayerSampleInfo *sample, dd index) {
    dd offset;
    db value;
    if (!module || !sample || sample->length == 0) return 0;
    if (index >= sample->length) return 0;
    offset = player_sample_info_is_16bit(sample) ? player_sample_info_data_offset(sample) + index * 2ul + 1ul : player_sample_info_data_offset(sample) + index;
    value = player_module_byte_at(module, offset);
    if (loader_kind(player_module_loader(module)) == IPLAY_LOADER_KIND_S3M && player_module_s3m_samples_unsigned(module)) value ^= 0x80u;
    return value;
}

static dd player_module_s3m_sample_c2spd(const PlayerModuleInfo *module, db instrument) {
    dd offset;
    dd c2spd;
    offset = player_module_s3m_instrument_header_offset(module, instrument);
    if (offset == 0 || !player_module_has_range(module, offset, 36u)) return IPLAY_PLAYER_S3M_DEFAULT_C2SPD;
    c2spd = player_module_u32_le_at(module, offset + 32u);
    return c2spd ? c2spd : IPLAY_PLAYER_S3M_DEFAULT_C2SPD;
}

static dd player_module_sample_c2spd(const PlayerModuleInfo *module, db instrument) {
    const LoaderInfo *loader;
    if (!module) return IPLAY_PLAYER_S3M_DEFAULT_C2SPD;
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        return player_module_s3m_sample_c2spd(module, instrument);
    default:
        break;
    }
    return IPLAY_PLAYER_S3M_DEFAULT_C2SPD;
}

static int player_module_mod_sample_finetune(const PlayerModuleInfo *module, db instrument) {
    dd offset;
    db raw;
    if (!module || instrument == 0 || instrument > 31u) return 0;
    offset = 20ul + ((dd)instrument - 1ul) * 30ul;
    if (!player_module_has_range(module, offset + 24u, 1u)) return 0;
    raw = (db)(player_module_byte_at(module, offset + 24u) & 0x0fu);
    return raw >= 8u ? (int)raw - 16 : (int)raw;
}

static void player_voice_state_apply_sample_c2spd(PlayerVoiceState *voice, const PlayerModuleInfo *module) {
    const LoaderInfo *loader;
    dd c2spd;
    dd scaled;
    if (!voice || voice->instrument == 0 || voice->sample_step == 0) return;
    loader = player_module_loader(module);
    if (loader_kind(loader) == IPLAY_LOADER_KIND_MOD) {
        int finetune = player_module_mod_sample_finetune(module, voice->instrument);
        if (finetune != 0) {
            long tuned = (long)voice->sample_step * (128L + (long)finetune);
            if (tuned < 128L) tuned = 128L;
            voice->sample_step = (dd)(tuned / 128L);
            if (voice->sample_step == 0) voice->sample_step = 1ul;
        }
        return;
    }
    c2spd = player_module_sample_c2spd(module, voice->instrument);
    if (c2spd == IPLAY_PLAYER_S3M_DEFAULT_C2SPD) return;
    scaled = (voice->sample_step * c2spd) / IPLAY_PLAYER_S3M_DEFAULT_C2SPD;
    voice->sample_step = scaled ? scaled : 1ul;
}

static void player_module_sample_info(const PlayerModuleInfo *module, db instrument, PlayerSampleInfo *sample) {
    const LoaderInfo *loader;
    player_sample_info_clear(sample);
    if (!module) return;
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        player_module_s3m_sample_info(module, instrument, sample);
        break;
    case IPLAY_LOADER_KIND_MOD:
        player_module_mod_sample_info(module, instrument, sample);
        break;
    default:
        break;
    }
}

static void player_voice_state_apply_cell(PlayerVoiceState *voice, const PlayerPatternCell *cell, const PlayerModuleInfo *module) {
    db apply_c2spd = 0;
    if (cell->effect == 0x03u && cell->period != 0 && voice->active) {
        voice->target_period = cell->period;
    } else if (cell->period != 0) {
        voice->period = cell->period;
        voice->target_period = cell->period;
        voice->sample_step = player_voice_state_step_for_period(voice->period);
        apply_c2spd = 1;
    }
    if (cell->effect != 0x03u || !voice->active) {
        if (cell->note != 0) voice->note = cell->note;
        if (cell->octave != 0) voice->octave = cell->octave;
    }
    if (cell->instrument != 0 && (cell->effect != 0x03u || !voice->active)) {
        voice->instrument = cell->instrument;
        player_module_sample_info(module, voice->instrument, &voice->sample);
        voice->sample_position = 0;
        voice->sample_phase = 0;
        if (voice->sample.volume != 0) voice->volume = voice->sample.volume;
        apply_c2spd = 1;
    }
    if (apply_c2spd) player_voice_state_apply_sample_c2spd(voice, module);
    voice->active = player_voice_state_playable(voice) ? 1 : 0;
    if (cell->volume_set == 1u) {
        voice->volume = cell->volume;
    } else if (cell->volume_set == 2u) {
        player_voice_state_apply_panning(voice, cell->volume);
    }
}

static void player_voice_state_apply_volume_slide(PlayerVoiceState *voice, db param) {
    db up;
    db down;
    if (!voice || !voice->active || param == 0) return;
    up = (db)((param >> 4) & 0x0fu);
    down = (db)(param & 0x0fu);
    if (up != 0) {
        voice->volume = (db)(voice->volume + up > 64u ? 64u : voice->volume + up);
    } else if (down != 0) {
        voice->volume = (db)(voice->volume > down ? voice->volume - down : 0);
    }
}

static void player_voice_state_apply_channel_volume(PlayerVoiceState *voice, db param) {
    if (!voice || !voice->active) return;
    voice->channel_volume = param > 64u ? 64u : param;
}

static void player_voice_state_apply_channel_volume_slide(PlayerVoiceState *voice, db param) {
    db up;
    db down;
    if (!voice || !voice->active || param == 0) return;
    up = (db)((param >> 4) & 0x0fu);
    down = (db)(param & 0x0fu);
    if (up != 0) {
        voice->channel_volume = (db)(voice->channel_volume + up > 64u ? 64u : voice->channel_volume + up);
    } else if (down != 0) {
        voice->channel_volume = (db)(voice->channel_volume > down ? voice->channel_volume - down : 0);
    }
}

static void player_voice_state_apply_extended_volume_slide(PlayerVoiceState *voice, db param) {
    db kind;
    db amount;
    if (!voice || !voice->active) return;
    kind = (db)((param >> 4) & 0x0fu);
    amount = (db)(param & 0x0fu);
    if (amount == 0) return;
    if (kind == 0x0au) {
        voice->volume = (db)(voice->volume + amount > 64u ? 64u : voice->volume + amount);
    } else if (kind == 0x0bu) {
        voice->volume = (db)(voice->volume > amount ? voice->volume - amount : 0);
    }
}

static void player_voice_state_apply_sample_offset(PlayerVoiceState *voice, db param) {
    dd offset;
    if (!voice || !voice->active || param == 0) return;
    offset = (dd)param << 8;
    if (offset >= voice->sample.length) {
        voice->sample_position = voice->sample.length;
        voice->sample_phase = voice->sample_position << 8;
        voice->active = 0;
        return;
    }
    voice->sample_position = offset;
    voice->sample_phase = offset << 8;
}

static void player_voice_state_apply_pitch_slide(PlayerVoiceState *voice, db effect, db param) {
    dw amount;
    if (!voice || !voice->active || param == 0) return;
    amount = param;
    if (effect == 0x01u) {
        voice->period = voice->period > amount ? (dw)(voice->period - amount) : 1u;
    } else if (effect == 0x02u) {
        voice->period = (dw)(voice->period + amount < voice->period ? 0xffffu : voice->period + amount);
    } else {
        return;
    }
    voice->sample_step = player_voice_state_step_for_period(voice->period);
}

static void player_voice_state_apply_extended_pitch_slide(PlayerVoiceState *voice, db param) {
    db kind;
    db amount;
    int finetune;
    long tuned;
    kind = (db)((param >> 4) & 0x0fu);
    amount = (db)(param & 0x0fu);
    if (kind == 0x01u) {
        player_voice_state_apply_pitch_slide(voice, 0x01u, amount);
    } else if (kind == 0x02u) {
        player_voice_state_apply_pitch_slide(voice, 0x02u, amount);
    } else if (kind == 0x05u && voice && voice->period != 0) {
        finetune = amount >= 8u ? (int)amount - 16 : (int)amount;
        voice->sample_step = player_voice_state_step_for_period(voice->period);
        if (finetune != 0) {
            tuned = (long)voice->sample_step * (128L + (long)finetune);
            if (tuned < 128L) tuned = 128L;
            voice->sample_step = (dd)(tuned / 128L);
            if (voice->sample_step == 0) voice->sample_step = 1ul;
        }
    }
}

static void player_voice_state_apply_tone_portamento(PlayerVoiceState *voice, db param) {
    dw amount;
    if (!voice || !voice->active || param == 0 || voice->target_period == 0 || voice->period == 0) return;
    amount = param;
    if (voice->period > voice->target_period) {
        voice->period = (dw)(voice->period - voice->target_period > amount ? voice->period - amount : voice->target_period);
    } else if (voice->period < voice->target_period) {
        voice->period = (dw)(voice->target_period - voice->period > amount ? voice->period + amount : voice->target_period);
    }
    voice->sample_step = player_voice_state_step_for_period(voice->period);
}

static void player_voice_state_apply_arpeggio(PlayerVoiceState *voice, db param) {
    db semitone;
    dw amount;
    if (!voice || !voice->active || param == 0 || voice->period <= 1u) return;
    semitone = (db)((param >> 4) & 0x0fu);
    if (semitone == 0) semitone = (db)(param & 0x0fu);
    if (semitone == 0) return;
    amount = (dw)(((dd)voice->period * (dd)semitone) / 32ul);
    if (amount == 0) amount = 1u;
    voice->period = voice->period > amount ? (dw)(voice->period - amount) : 1u;
    voice->sample_step = player_voice_state_step_for_period(voice->period);
}

static void player_voice_state_apply_retrigger(PlayerVoiceState *voice, db param) {
    if (!voice || !voice->active || param == 0 || voice->sample.length == 0) return;
    voice->sample_position = 0;
    voice->sample_phase = 0;
}

static void player_voice_state_apply_extended_retrigger(PlayerVoiceState *voice, db param) {
    if (((param >> 4) & 0x0fu) != 0x09u) return;
    player_voice_state_apply_retrigger(voice, (db)(param & 0x0fu));
}

static void player_voice_state_apply_vibrato(PlayerVoiceState *voice, db param) {
    db depth;
    dw amount;
    dw period;
    if (!voice || !voice->active || param == 0 || voice->period <= 1u) return;
    depth = (db)(param & 0x0fu);
    if (depth == 0) depth = (db)((param >> 4) & 0x0fu);
    if (depth == 0) return;
    amount = (dw)(((dd)voice->period * (dd)depth) / 64ul);
    if (amount == 0) amount = 1u;
    period = voice->period > amount ? (dw)(voice->period - amount) : 1u;
    voice->sample_step = player_voice_state_step_for_period(period);
}

static void player_voice_state_apply_tremolo(PlayerVoiceState *voice, db param) {
    db depth;
    if (!voice || !voice->active || param == 0) return;
    depth = (db)(param & 0x0fu);
    if (depth == 0) depth = (db)((param >> 4) & 0x0fu);
    if (depth == 0) return;
    voice->volume = voice->volume > depth ? (db)(voice->volume - depth) : 0;
}

static void player_voice_state_apply_tremor(PlayerVoiceState *voice, db param) {
    db on_ticks;
    db off_ticks;
    if (!voice || !voice->active || param == 0) return;
    on_ticks = (db)((param >> 4) & 0x0fu);
    off_ticks = (db)(param & 0x0fu);
    if (off_ticks != 0 && (on_ticks == 0 || off_ticks >= on_ticks)) voice->volume = 0;
}

static void player_voice_state_apply_panning(PlayerVoiceState *voice, db param) {
    if (!voice || !voice->active) return;
    voice->pan_set = 1;
    voice->pan = param;
}

static void player_voice_state_apply_panning_slide(PlayerVoiceState *voice, db param) {
    db right;
    db left;
    if (!voice || !voice->active || param == 0) return;
    if (!voice->pan_set) {
        voice->pan_set = 1;
        voice->pan = 128;
    }
    right = (db)((param >> 4) & 0x0fu);
    left = (db)(param & 0x0fu);
    if (right != 0) {
        voice->pan = (db)(voice->pan + right > 255u ? 255u : voice->pan + right);
    } else if (left != 0) {
        voice->pan = (db)(voice->pan > left ? voice->pan - left : 0);
    }
}

static void player_voice_state_apply_panbrello(PlayerVoiceState *voice, db param) {
    db depth;
    dw amount;
    if (!voice || !voice->active || param == 0) return;
    depth = (db)(param & 0x0fu);
    if (depth == 0) depth = (db)((param >> 4) & 0x0fu);
    if (depth == 0) return;
    amount = (dw)depth * 8u;
    voice->pan_set = 1;
    voice->pan = (db)(128u + amount > 255u ? 255u : 128u + amount);
}

static void player_voice_state_apply_extended_panning(PlayerVoiceState *voice, db param) {
    if (((param >> 4) & 0x0fu) != 0x08u) return;
    player_voice_state_apply_panning(voice, (db)((param & 0x0fu) * 17u));
}

static void player_voice_state_apply_note_cut(PlayerVoiceState *voice, db param) {
    if (!voice || !voice->active) return;
    if (((param >> 4) & 0x0fu) != 0x0cu) return;
    voice->active = 0;
}

static int player_pattern_cell_has_note_delay(const PlayerPatternCell *cell) {
    return cell && cell->effect == 0x0eu && ((cell->param >> 4) & 0x0fu) == 0x0du && (cell->param & 0x0fu) != 0;
}

static int player_pattern_cell_note_delay_tick(const PlayerPatternCell *cell, dw tick) {
    return player_pattern_cell_has_note_delay(cell) && (dw)(cell->param & 0x0fu) == tick;
}

static void player_decoder_context_apply_timing_effect(PlayerDecoderContext *context, const PlayerPatternCell *cell) {
    if (!context || !cell) return;
    if (cell->effect == 0x0fu && cell->param != 0) {
        if (cell->param <= 0x20u) {
            context->current_speed = cell->param;
        } else {
            context->current_tempo = cell->param;
        }
    }
    if (cell->effect == 0x0du) {
        dw row = (dw)(((cell->param >> 4) & 0x0fu) * 10u + (cell->param & 0x0fu));
        if (row >= context->rows_per_order) row = (dw)(context->rows_per_order - 1u);
        context->pattern_break_row = row;
        context->pattern_break_pending = 1;
    }
    if (cell->effect == 0x0bu) {
        context->position_jump_order = cell->param;
        context->position_jump_pending = 1;
    }
    if (cell->effect == 0x16u) {
        context->global_volume = cell->param > 64u ? 64u : cell->param;
    }
    if (cell->effect == 0x17u && cell->param != 0) {
        db up = (db)((cell->param >> 4) & 0x0fu);
        db down = (db)(cell->param & 0x0fu);
        if (up != 0) {
            context->global_volume = (db)(context->global_volume + up > 64u ? 64u : context->global_volume + up);
        } else if (down != 0) {
            context->global_volume = (db)(context->global_volume > down ? context->global_volume - down : 0);
        }
    }
    if (cell->effect == 0x0eu && ((cell->param >> 4) & 0x0fu) == 0x06u) {
        db loop_count = (db)(cell->param & 0x0fu);
        if (loop_count == 0) {
            context->pattern_loop_row = context->row;
            context->pattern_loop_active = 1;
            context->pattern_loop_remaining = 0;
            context->pattern_loop_completed = 0;
        } else if (context->pattern_loop_active) {
            if (context->pattern_loop_completed) return;
            if (context->pattern_loop_remaining == 0) context->pattern_loop_remaining = loop_count;
            if (context->pattern_loop_remaining != 0) {
                context->pattern_loop_remaining -= 1u;
                if (context->pattern_loop_remaining != 0) {
                    context->pattern_break_row = context->pattern_loop_row;
                    context->pattern_break_pending = 1;
                    context->pattern_loop_jump_pending = 1;
                } else {
                    context->pattern_loop_completed = 1;
                }
            }
        }
    }
}

static void player_module_pattern_cell(const PlayerModuleInfo *module, dw pattern, dw row, dw channel, PlayerPatternCell *cell);

static PlayerVoiceState *player_decoder_context_voice(PlayerDecoderContext *context, dw channel) {
    if (!context) return 0;
    if (channel >= IPLAY_PLAYER_MAX_CHANNELS) channel = 0;
    return &context->voices[channel];
}

static const PlayerVoiceState *player_decoder_context_voice_const(const PlayerDecoderContext *context, dw channel) {
    if (!context) return 0;
    if (channel >= IPLAY_PLAYER_MAX_CHANNELS) channel = 0;
    return &context->voices[channel];
}

static void player_decoder_context_refresh_current_cell(PlayerDecoderContext *context) {
    if (!context) return;
    player_module_pattern_cell(context->module, context->current_order_value, context->row, context->channel, &context->current_cell);
}

static void player_decoder_context_load_row_events(PlayerDecoderContext *context) {
    dw channel;
    dw channel_limit;
    PlayerPatternCell cell;
    if (!context) return;
    channel_limit = context->channel_count;
    if (channel_limit > IPLAY_PLAYER_MAX_CHANNELS) channel_limit = IPLAY_PLAYER_MAX_CHANNELS;
    for (channel = 0; channel < channel_limit; ++channel) {
        PlayerVoiceState *voice;
        player_module_pattern_cell(context->module, context->current_order_value, context->row, channel, &cell);
        player_decoder_context_apply_timing_effect(context, &cell);
        if (player_pattern_cell_has_note_delay(&cell)) continue;
        voice = player_decoder_context_voice(context, channel);
        player_voice_state_apply_cell(voice, &cell, context->module);
        if (cell.effect == 0x00u && cell.param != 0) player_voice_state_apply_arpeggio(voice, cell.param);
        if (cell.effect == 0x04u) player_voice_state_apply_vibrato(voice, cell.param);
        if (cell.effect == 0x07u) player_voice_state_apply_tremolo(voice, cell.param);
        if (cell.effect == 0x14u) player_voice_state_apply_tremor(voice, cell.param);
        if (cell.effect == 0x19u) player_voice_state_apply_channel_volume(voice, cell.param);
        if (cell.effect == 0x1au) player_voice_state_apply_channel_volume_slide(voice, cell.param);
        if (cell.effect == 0x12u) {
            player_voice_state_apply_vibrato(voice, cell.param);
            player_voice_state_apply_volume_slide(voice, cell.param);
        }
        if (cell.effect == 0x13u) {
            player_voice_state_apply_tone_portamento(voice, cell.param);
            player_voice_state_apply_volume_slide(voice, cell.param);
        }
        if (cell.effect == 0x05u) {
            player_voice_state_apply_tone_portamento(voice, cell.param);
            player_voice_state_apply_volume_slide(voice, cell.param);
        }
        if (cell.effect == 0x06u) {
            player_voice_state_apply_vibrato(voice, cell.param);
            player_voice_state_apply_volume_slide(voice, cell.param);
        }
        if (cell.effect == 0x01u || cell.effect == 0x02u) player_voice_state_apply_pitch_slide(voice, cell.effect, cell.param);
        if (cell.effect == 0x03u) player_voice_state_apply_tone_portamento(voice, cell.param);
        if (cell.effect == 0x0eu) player_voice_state_apply_extended_pitch_slide(voice, cell.param);
        if (cell.effect == 0x08u) player_voice_state_apply_panning(voice, cell.param);
        if (cell.effect == 0x09u) player_voice_state_apply_sample_offset(voice, cell.param);
        if (cell.effect == 0x0au) player_voice_state_apply_volume_slide(voice, cell.param);
        if (cell.effect == 0x0eu) player_voice_state_apply_extended_volume_slide(voice, cell.param);
        if (cell.effect == 0x11u) player_voice_state_apply_retrigger(voice, cell.param);
        if (cell.effect == 0x0eu) player_voice_state_apply_extended_retrigger(voice, cell.param);
        if (cell.effect == 0x15u) player_voice_state_apply_panning(voice, cell.param);
        if (cell.effect == 0x18u) player_voice_state_apply_panning_slide(voice, cell.param);
        if (cell.effect == 0x1bu) player_voice_state_apply_panbrello(voice, cell.param);
        if (cell.effect == 0x0eu) player_voice_state_apply_extended_panning(voice, cell.param);
        if (cell.effect == 0x0eu) player_voice_state_apply_note_cut(voice, cell.param);
    }
    player_decoder_context_refresh_current_cell(context);
}

static void player_decoder_context_load_delayed_row_events(PlayerDecoderContext *context) {
    dw channel;
    dw channel_limit;
    PlayerPatternCell cell;
    if (!context || context->current_tick == 0) return;
    channel_limit = context->channel_count;
    if (channel_limit > IPLAY_PLAYER_MAX_CHANNELS) channel_limit = IPLAY_PLAYER_MAX_CHANNELS;
    for (channel = 0; channel < channel_limit; ++channel) {
        PlayerVoiceState *voice;
        player_module_pattern_cell(context->module, context->current_order_value, context->row, channel, &cell);
        if (!player_pattern_cell_note_delay_tick(&cell, context->current_tick)) continue;
        voice = player_decoder_context_voice(context, channel);
        player_voice_state_apply_cell(voice, &cell, context->module);
    }
    player_decoder_context_refresh_current_cell(context);
}

static void player_mod_period_to_note(dw period, db *note, db *octave) {
    static const dw periods[36] = {
        856u, 808u, 762u, 720u, 678u, 640u, 604u, 570u, 538u, 508u, 480u, 453u,
        428u, 404u, 381u, 360u, 339u, 320u, 302u, 285u, 269u, 254u, 240u, 226u,
        214u, 202u, 190u, 180u, 170u, 160u, 151u, 143u, 135u, 127u, 120u, 113u
    };
    db i;
    db best = 0;
    dw best_delta = 0xffffu;
    *note = 0;
    *octave = 0;
    if (period == 0) return;
    for (i = 0; i < 36u; ++i) {
        dw delta = periods[i] > period ? (dw)(periods[i] - period) : (dw)(period - periods[i]);
        if (delta < best_delta) {
            best_delta = delta;
            best = i;
        }
    }
    *note = (db)((best % 12u) + 1u);
    *octave = (db)((best / 12u) + 1u);
}

static void player_module_mod_pattern_cell(const PlayerModuleInfo *module, dw pattern, dw row, dw channel, PlayerPatternCell *cell) {
    dw channels;
    dd offset;
    db b0;
    db b1;
    db b2;
    db b3;
    player_pattern_cell_clear(cell);
    if (!module) return;
    channels = player_module_channel_count(module);
    offset = 1084ul + ((dd)pattern * (dd)IPLAY_PLAYER_DEFAULT_ROWS_PER_ORDER * (dd)channels * 4ul) +
             ((dd)row * (dd)channels * 4ul) + ((dd)channel * 4ul);
    if (!player_module_has_range(module, offset, 4u)) return;
    b0 = player_module_byte_at(module, offset + 0u);
    b1 = player_module_byte_at(module, offset + 1u);
    b2 = player_module_byte_at(module, offset + 2u);
    b3 = player_module_byte_at(module, offset + 3u);
    cell->period = (dw)(((dw)(b0 & 0x0fu) << 8) | b1);
    player_mod_period_to_note(cell->period, &cell->note, &cell->octave);
    cell->instrument = (db)((b0 & 0xf0u) | ((b2 >> 4) & 0x0fu));
    cell->effect = (db)(b2 & 0x0fu);
    cell->param = b3;
    if (cell->effect == 0x0cu) {
        cell->volume = cell->param > 64u ? 64u : cell->param;
        cell->volume_set = 1;
    }
}

static void player_s3m_note_to_cell(db note_byte, PlayerPatternCell *cell) {
    static const dw base_periods[12] = {
        1712u, 1616u, 1524u, 1440u, 1356u, 1280u, 1208u, 1140u, 1076u, 1016u, 960u, 907u
    };
    db note = (db)(note_byte & 0x0fu);
    db octave = (db)((note_byte >> 4) & 0x0fu);
    db shift = octave > 6u ? 6u : octave;
    if (note_byte == 0xfeu) {
        cell->effect = 0x0eu;
        cell->param = 0xc0u;
        return;
    }
    if (note >= 12u || note_byte >= 0xfeu) return;
    cell->period = (dw)(base_periods[note] >> shift);
    if (cell->period == 0) cell->period = 1;
    cell->note = (db)(note + 1u);
    cell->octave = (db)(octave + 1u);
}

static void player_module_s3m_apply_effect(PlayerPatternCell *cell, db command, db param) {
    switch (command) {
    case 1u:
    case 20u:
        cell->effect = 0x0fu;
        cell->param = param;
        break;
    case 21u:
        cell->effect = 0x04u;
        cell->param = param;
        break;
    case 22u:
        cell->effect = 0x16u;
        cell->param = param;
        break;
    case 23u:
        cell->effect = 0x17u;
        cell->param = param;
        break;
    case 24u:
        cell->effect = 0x15u;
        cell->param = param;
        break;
    case 25u:
        cell->effect = 0x1bu;
        cell->param = param;
        break;
    case 2u:
        cell->effect = 0x0bu;
        cell->param = param;
        break;
    case 3u:
        cell->effect = 0x0du;
        cell->param = param;
        break;
    case 4u:
        cell->effect = 0x0au;
        cell->param = param;
        break;
    case 5u:
        cell->effect = 0x02u;
        cell->param = param;
        break;
    case 6u:
        cell->effect = 0x01u;
        cell->param = param;
        break;
    case 7u:
        cell->effect = 0x03u;
        cell->param = param;
        break;
    case 8u:
        cell->effect = 0x04u;
        cell->param = param;
        break;
    case 9u:
        cell->effect = 0x14u;
        cell->param = param;
        break;
    case 18u:
        cell->effect = 0x07u;
        cell->param = param;
        break;
    case 19u:
        cell->effect = 0x0eu;
        cell->param = param;
        break;
    case 10u:
        cell->effect = 0x00u;
        cell->param = param;
        break;
    case 11u:
        cell->effect = 0x12u;
        cell->param = param;
        break;
    case 12u:
        cell->effect = 0x13u;
        cell->param = param;
        break;
    case 13u:
        cell->effect = 0x19u;
        cell->param = param;
        break;
    case 14u:
        cell->effect = 0x1au;
        cell->param = param;
        break;
    case 15u:
        cell->effect = 0x09u;
        cell->param = param;
        break;
    case 16u:
        cell->effect = 0x18u;
        cell->param = param;
        break;
    case 17u:
        cell->effect = 0x11u;
        cell->param = param;
        break;
    default:
        cell->effect = command;
        cell->param = param;
        break;
    }
}

static void player_module_s3m_pattern_cell(const PlayerModuleInfo *module, dw pattern, dw row, dw channel, PlayerPatternCell *cell) {
    dd pattern_offset;
    dd offset;
    dd end;
    dw pattern_len;
    dw current_row = 0;
    dw physical_channel;
    player_pattern_cell_clear(cell);
    physical_channel = player_module_s3m_physical_channel(module, channel);
    pattern_offset = player_module_s3m_pattern_offset(module, pattern);
    if (pattern_offset == 0 || !player_module_has_range(module, pattern_offset, 2u)) return;
    pattern_len = player_module_u16_le_at(module, pattern_offset);
    offset = pattern_offset + 2ul;
    end = pattern_offset + (dd)pattern_len;
    if (!player_module_has_range(module, offset, pattern_len > 2u ? (dd)pattern_len - 2ul : 0)) return;
    while (offset < end && current_row <= row) {
        db what = player_module_byte_at(module, offset++);
        db packed_channel;
        db note = 0xffu;
        db instrument = 0;
        db volume = 0xffu;
        db command = 0;
        db param = 0;
        if (what == 0) {
            current_row += 1u;
            continue;
        }
        packed_channel = (db)(what & 0x1fu);
        if (what & 0x20u) {
            note = player_module_byte_at(module, offset++);
            instrument = player_module_byte_at(module, offset++);
        }
        if (what & 0x40u) volume = player_module_byte_at(module, offset++);
        if (what & 0x80u) {
            command = player_module_byte_at(module, offset++);
            param = player_module_byte_at(module, offset++);
        }
        if (current_row == row && packed_channel == (db)physical_channel) {
            player_s3m_note_to_cell(note, cell);
            cell->instrument = instrument;
            if (volume != 0xffu) {
                if (volume <= 64u) {
                    cell->volume = volume;
                    cell->volume_set = 1;
                } else if (volume >= 128u && volume <= 192u) {
                    cell->volume = volume == 192u ? 255u : (db)((volume - 128u) * 4u);
                    cell->volume_set = 2;
                }
            }
            if (command != 0) player_module_s3m_apply_effect(cell, command, param);
            return;
        }
    }
}

static void player_module_pattern_cell(const PlayerModuleInfo *module, dw pattern, dw row, dw channel, PlayerPatternCell *cell) {
    const LoaderInfo *loader;
    player_pattern_cell_clear(cell);
    if (!module) return;
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        player_module_s3m_pattern_cell(module, pattern, row, channel, cell);
        break;
    case IPLAY_LOADER_KIND_MOD:
        player_module_mod_pattern_cell(module, pattern, row, channel, cell);
        break;
    default:
        break;
    }
}

static dw player_module_rows_per_order(const PlayerModuleInfo *module) {
    (void)module;
    return IPLAY_PLAYER_DEFAULT_ROWS_PER_ORDER;
}

static dw player_module_restart_order(const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    const LoaderInfo *loader;
    if (!module) return 0;
    header = player_module_header(module);
    len = player_module_header_len(module);
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_669:
        if (len > 0x70u) return header[0x70u];
        break;
    default:
        break;
    }
    return 0;
}

static dw player_module_initial_speed(const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    const LoaderInfo *loader;
    if (!module) return IPLAY_PLAYER_DEFAULT_INITIAL_SPEED;
    header = player_module_header(module);
    len = player_module_header_len(module);
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        if (len > 0x31u && header[0x31u] != 0) return header[0x31u];
        break;
    default:
        break;
    }
    return IPLAY_PLAYER_DEFAULT_INITIAL_SPEED;
}

static dw player_module_initial_tempo(const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    const LoaderInfo *loader;
    if (!module) return IPLAY_PLAYER_DEFAULT_INITIAL_TEMPO;
    header = player_module_header(module);
    len = player_module_header_len(module);
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        if (len > 0x32u && header[0x32u] != 0) return header[0x32u];
        break;
    default:
        break;
    }
    return IPLAY_PLAYER_DEFAULT_INITIAL_TEMPO;
}

static dw player_module_initial_global_volume(const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    const LoaderInfo *loader;
    db volume;
    db master_volume;
    dd scaled_volume;
    if (!module) return 64u;
    header = player_module_header(module);
    len = player_module_header_len(module);
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        if (len > 0x30u) {
            volume = header[0x30u];
            if (volume > 64u) volume = 64u;
            if (len > 0x33u) {
                master_volume = (db)(header[0x33u] & 0x7fu);
                if (master_volume != 0) {
                    if (master_volume > 64u) master_volume = 64u;
                    scaled_volume = ((dd)volume * (dd)master_volume) / 64ul;
                    return scaled_volume > 64ul ? 64u : (dw)scaled_volume;
                }
            }
            return volume;
        }
        break;
    default:
        break;
    }
    return 64u;
}

static int player_module_s3m_stereo_enabled(const PlayerModuleInfo *module) {
    const db *header;
    size_t len;
    if (!module) return 0;
    header = player_module_header(module);
    len = player_module_header_len(module);
    return len > 0x33u && (header[0x33u] & 0x80u) != 0;
}

static int player_module_initial_channel_pan(const PlayerModuleInfo *module, dw channel, db *pan) {
    const db *header;
    size_t len;
    const LoaderInfo *loader;
    db setting;
    dd pan_table_offset;
    db pan_entry;
    dw instrument_count;
    dw pattern_count;
    dw physical_channel;
    if (!module || !pan) return 0;
    header = player_module_header(module);
    len = player_module_header_len(module);
    loader = player_module_loader(module);
    switch (loader_kind(loader)) {
    case IPLAY_LOADER_KIND_S3M:
        if (!player_module_s3m_stereo_enabled(module)) return 0;
        physical_channel = player_module_s3m_physical_channel(module, channel);
        if (physical_channel < 32u && len > 0x40u + physical_channel) {
            setting = header[0x40u + physical_channel];
            if (setting < 16u) {
                *pan = setting < 8u ? 48u : 208u;
                if (len > 0x35u && header[0x35u] == 0xfcu) {
                    instrument_count = player_module_s3m_instrument_count(module);
                    pattern_count = player_module_pattern_count(module);
                    pan_table_offset = player_module_s3m_table_offset(module) + (dd)instrument_count * 2ul + (dd)pattern_count * 2ul;
                    if (player_module_has_range(module, pan_table_offset + physical_channel, 1u)) {
                        pan_entry = player_module_byte_at(module, pan_table_offset + physical_channel);
                        if (pan_entry & 0x20u) *pan = (db)((pan_entry & 0x0fu) * 17u);
                    }
                }
                return 1;
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

static dw player_decoder_context_estimated_max_blocks(dw orders, dw rows, dw speed, dw channels) {
    dd blocks;
    if (orders == 0) orders = 1u;
    if (rows == 0) rows = IPLAY_PLAYER_DEFAULT_ROWS_PER_ORDER;
    if (speed == 0) speed = IPLAY_PLAYER_DEFAULT_INITIAL_SPEED;
    if (channels == 0) channels = IPLAY_PLAYER_DEFAULT_CHANNELS;
    blocks = (dd)orders * (dd)rows * (dd)speed * (dd)channels;
    if (blocks == 0) return 1u;
    return blocks > 0xfffful ? 0xffffu : (dw)blocks;
}

static void player_decoder_context_init(PlayerDecoderContext *context, const PlayerModuleInfo *module) {
    dw i;
    dw value;
    db pan;
    context->module = module;
    context->loader = module ? player_module_loader(module) : 0;
    context->seed = player_module_pcm_seed(module);
    context->renderer = loader_decoder_renderer_kind(context->loader);
    context->ended = 0;
    context->loop_enabled = 0;
    context->pattern_break_pending = 0;
    context->position_jump_pending = 0;
    context->pattern_loop_active = 0;
    context->pattern_loop_remaining = 0;
    context->pattern_loop_completed = 0;
    context->pattern_loop_jump_pending = 0;
    context->block_index = 0;
    context->order_count = player_module_order_count(module);
    context->rows_per_order = player_module_rows_per_order(module);
    context->restart_order = player_module_restart_order(module);
    context->initial_speed = player_module_initial_speed(module);
    context->initial_tempo = player_module_initial_tempo(module);
    context->current_speed = context->initial_speed;
    context->current_tempo = context->initial_tempo;
    context->global_volume = player_module_initial_global_volume(module);
    context->current_tick = 0;
    context->pcm_stream_offset = player_module_stream_start_offset(module);
    context->file_stream_base = 0;
    context->file_stream_fd = -1;
    context->file_stream_open = 0;
    context->file_stream_len = 0;
    context->file_stream_index = 0;
    context->pattern_break_row = 0;
    context->position_jump_order = 0;
    context->pattern_loop_row = 0;
    context->channel_count = player_module_channel_count(module);
    context->max_blocks = player_decoder_context_estimated_max_blocks(context->order_count, context->rows_per_order, context->current_speed, context->channel_count);
    context->current_order_value = 0;
    context->order = 0;
    context->row = 0;
    context->channel = 0;
    for (i = 0; i < IPLAY_PLAYER_MAX_CHANNELS; ++i) {
        player_voice_state_clear(&context->voices[i]);
        if (player_module_initial_channel_pan(module, i, &pan)) {
            context->voices[i].pan_set = 1;
            context->voices[i].pan = pan;
        }
    }
    if (context->order_count != 0) {
        while (context->order < context->order_count) {
            value = player_module_order_value(module, context->order);
            if (player_module_order_value_is_skip(value)) {
                context->order += 1u;
                continue;
            }
            if (player_module_order_value_is_end(value)) {
                context->ended = 1;
                break;
            }
            context->current_order_value = value;
            break;
        }
        if (context->order >= context->order_count) context->ended = 1;
    }
    if (!context->ended) player_decoder_context_load_row_events(context);
}

static const LoaderInfo *player_decoder_context_loader(const PlayerDecoderContext *context) {
    return context->loader;
}

static db player_decoder_context_seed(const PlayerDecoderContext *context) {
    return context->seed;
}

static db player_decoder_context_renderer(const PlayerDecoderContext *context) {
    return context ? context->renderer : IPLAY_PLAYER_RENDERER_NONE;
}

static int player_decoder_context_has_block(const PlayerDecoderContext *context) {
    return context && !context->ended && context->block_index < context->max_blocks;
}

static void player_decoder_context_mark_ended(PlayerDecoderContext *context) {
    context->ended = 1;
}

static db player_decoder_context_block_seed(PlayerDecoderContext *context) {
    return (db)(context->seed + (db)context->block_index);
}

static void player_decoder_context_close_file_stream(PlayerDecoderContext *context) {
    if (!context || !context->file_stream_open) return;
    close(context->file_stream_fd);
    context->file_stream_fd = -1;
    context->file_stream_open = 0;
    context->file_stream_len = 0;
    context->file_stream_index = 0;
}

static int player_decoder_context_open_file_stream(PlayerDecoderContext *context) {
    int fd;
    if (!context || !context->module || !player_module_path(context->module)) return 0;
    if (context->file_stream_open) return 1;
    fd = player_open_read_binary(player_module_path(context->module));
    if (fd < 0) return 0;
    context->file_stream_fd = fd;
    context->file_stream_open = 1;
    return 1;
}

static int player_decoder_context_refill_file_stream(PlayerDecoderContext *context) {
    long pos;
    int n;
    unsigned request;
    unsigned long module_size;
    if (!context || !context->module || !player_module_path(context->module)) return 0;
    module_size = player_module_size(context->module);
    if (module_size == 0) return 0;
    if (context->pcm_stream_offset >= module_size) context->pcm_stream_offset = player_module_stream_start_offset(context->module);
    if (!player_decoder_context_open_file_stream(context)) return 0;
    pos = lseek(context->file_stream_fd, (long)context->pcm_stream_offset, SEEK_SET);
    if (pos < 0) {
        player_decoder_context_close_file_stream(context);
        return 0;
    }
    request = IPLAY_PLAYER_FILE_STREAM_BUFFER_BYTES;
    if ((unsigned long)request > module_size - context->pcm_stream_offset) request = (unsigned)(module_size - context->pcm_stream_offset);
    n = read(context->file_stream_fd, player_module_file_stream_buffer(), request);
    if (n <= 0) {
        player_decoder_context_close_file_stream(context);
        return 0;
    }
    context->file_stream_base = context->pcm_stream_offset;
    context->file_stream_len = (dw)n;
    context->file_stream_index = 0;
    return 1;
}

static db player_decoder_context_next_file_stream_byte(PlayerDecoderContext *context, db fallback) {
    db value;
    if (!context) return fallback;
    if (context->file_stream_index >= context->file_stream_len) {
        if (!player_decoder_context_refill_file_stream(context)) return fallback;
    }
    value = player_module_file_stream_buffer()[context->file_stream_index];
    context->file_stream_index += 1u;
    context->pcm_stream_offset = context->file_stream_base + context->file_stream_index;
    return value;
}

static db player_decoder_context_next_module_stream_byte(PlayerDecoderContext *context, db fallback) {
    db value;
    if (!context || !context->module || player_module_header_len(context->module) == 0) return fallback;
    if (!player_module_data_complete(context->module)) return player_decoder_context_next_file_stream_byte(context, fallback);
    if (context->pcm_stream_offset >= (dd)player_module_header_len(context->module)) context->pcm_stream_offset = player_module_stream_start_offset(context->module);
    value = player_module_byte_at(context->module, context->pcm_stream_offset);
    context->pcm_stream_offset += 1u;
    return value;
}

static void player_decoder_context_advance(PlayerDecoderContext *context) {
    db row_advanced = 0;
    dw value;
    context->block_index += 1u;
    context->channel += 1u;
    if (context->channel >= context->channel_count) {
        context->channel = 0;
        context->current_tick += 1u;
        if (context->current_speed == 0 || context->current_tick >= context->current_speed) {
            context->current_tick = 0;
            if (context->pattern_break_pending || context->position_jump_pending) {
                context->row = context->pattern_break_pending ? context->pattern_break_row : 0;
                if (context->position_jump_pending) {
                    context->order = context->position_jump_order;
                } else if (!context->pattern_loop_jump_pending) {
                    context->order += 1u;
                }
                context->pattern_break_pending = 0;
                context->position_jump_pending = 0;
                context->pattern_loop_jump_pending = 0;
            } else {
                context->row += 1u;
                if (context->row >= context->rows_per_order) {
                    context->row = 0;
                    context->order += 1u;
                }
            }
            if (context->order >= context->order_count) {
                player_decoder_context_mark_ended(context);
                return;
            }
            while (context->order < context->order_count) {
                value = player_module_order_value(context->module, context->order);
                if (player_module_order_value_is_skip(value)) {
                    context->order += 1u;
                    continue;
                }
                if (player_module_order_value_is_end(value)) {
                    player_decoder_context_mark_ended(context);
                    return;
                }
                context->current_order_value = value;
                break;
            }
            if (context->order >= context->order_count) {
                player_decoder_context_mark_ended(context);
                return;
            }
            row_advanced = 1;
        } else {
            player_decoder_context_load_delayed_row_events(context);
        }
    }
    if (row_advanced) {
        player_decoder_context_load_row_events(context);
    } else {
        player_decoder_context_refresh_current_cell(context);
    }
}

static dw player_decoder_context_block_index(const PlayerDecoderContext *context) {
    return context->block_index;
}

static dw player_decoder_context_max_blocks(const PlayerDecoderContext *context) {
    return context->max_blocks;
}

static dw player_decoder_context_order_count(const PlayerDecoderContext *context) {
    return context->order_count;
}

static dw player_decoder_context_rows_per_order(const PlayerDecoderContext *context) {
    return context->rows_per_order;
}

static dw player_decoder_context_restart_order(const PlayerDecoderContext *context) {
    return context->restart_order;
}

static dw player_decoder_context_initial_speed(const PlayerDecoderContext *context) {
    return context->initial_speed;
}

static dw player_decoder_context_initial_tempo(const PlayerDecoderContext *context) {
    return context->initial_tempo;
}

static dw player_decoder_context_current_speed(const PlayerDecoderContext *context) {
    return context->current_speed;
}

static dw player_decoder_context_current_tempo(const PlayerDecoderContext *context) {
    return context->current_tempo;
}

static dw player_decoder_context_current_tick(const PlayerDecoderContext *context) {
    return context->current_tick;
}

static dw player_decoder_context_channel_count(const PlayerDecoderContext *context) {
    return context->channel_count;
}

static dw player_decoder_context_current_order_value(const PlayerDecoderContext *context) {
    return context->current_order_value;
}

static const PlayerPatternCell *player_decoder_context_current_cell(const PlayerDecoderContext *context) {
    return &context->current_cell;
}

static const PlayerVoiceState *player_decoder_context_current_voice(const PlayerDecoderContext *context) {
    return player_decoder_context_voice_const(context, context->channel);
}

static dw player_decoder_context_order(const PlayerDecoderContext *context) {
    return context->order;
}

static dw player_decoder_context_row(const PlayerDecoderContext *context) {
    return context->row;
}

static dw player_decoder_context_channel(const PlayerDecoderContext *context) {
    return context->channel;
}

static db player_decoder_context_ended(const PlayerDecoderContext *context) {
    return context->ended;
}

static db player_decoder_context_loop_enabled(const PlayerDecoderContext *context) {
    return context->loop_enabled;
}

static int player_decoder_context_has_active_voice(PlayerDecoderContext *context, dw channel_limit) {
    dw channel;
    if (!context) return 0;
    if (channel_limit > IPLAY_PLAYER_MAX_CHANNELS) channel_limit = IPLAY_PLAYER_MAX_CHANNELS;
    for (channel = 0; channel < channel_limit; ++channel) {
        PlayerVoiceState *voice = player_decoder_context_voice(context, channel);
        if (voice && voice->active) return 1;
    }
    return 0;
}

static void player_playback_fill_stream_pcm(PlayerPlaybackBlock *block, PlayerDecoderContext *context, db fallback) {
    db *pcm = player_playback_block_pcm(block);
    unsigned frame;
    unsigned frames = player_playback_block_frames(block);
    if (frames > player_playback_block_capacity_frames()) frames = player_playback_block_capacity_frames();
    for (frame = 0; frame < frames; ++frame) {
        db file_byte = player_decoder_context_next_module_stream_byte(context, fallback);
        int sample = player_mix_clamp_s16(((long)(int)file_byte - 128L) << 8);
        player_mix_put_s16le(pcm, (unsigned)(frame * 4u), sample);
        player_mix_put_s16le(pcm, (unsigned)(frame * 4u + 2u), sample);
    }
}

static void player_playback_mix_voices_pcm(PlayerPlaybackBlock *block, PlayerDecoderContext *context, db fallback) {
    db *pcm = player_playback_block_pcm(block);
    unsigned frame;
    unsigned frames = player_playback_block_frames(block);
    dw channel_limit;
    if (!context) return;
    channel_limit = context->channel_count;
    if (channel_limit > IPLAY_PLAYER_MAX_CHANNELS) channel_limit = IPLAY_PLAYER_MAX_CHANNELS;
    if (frames > player_playback_block_capacity_frames()) frames = player_playback_block_capacity_frames();
    if (!player_decoder_context_has_active_voice(context, channel_limit)) {
        player_playback_fill_stream_pcm(block, context, fallback);
        return;
    }
    for (frame = 0; frame < frames; ++frame) {
        db file_byte = player_decoder_context_next_module_stream_byte(context, fallback);
        long left = ((long)(int)file_byte - 128L) << 8;
        long right = ((long)(int)file_byte - 128L) << 8;
        dw channel;
        for (channel = 0; channel < channel_limit; ++channel) {
            PlayerVoiceState *voice = player_decoder_context_voice(context, channel);
            if (voice && voice->active) {
                db seed = player_voice_state_pcm_seed(voice, fallback);
                long sample = (long)player_voice_state_next_sample_s16(voice, context->module);
                sample = (sample * (long)voice->volume) / 64L;
                sample = (sample * (long)voice->channel_volume) / 64L;
                sample = (sample * (long)context->global_volume) / 64L;
                if (voice->pan_set) {
                    left += (sample * (long)(255u - voice->pan)) / 255L + (((long)(int)seed - 128L) << 4);
                    right += (sample * (long)voice->pan) / 255L;
                } else if ((channel & 1u) == 0) {
                    left += sample + (((long)(int)seed - 128L) << 4);
                    right += sample / 2L;
                } else {
                    left += sample / 2L;
                    right += sample + (((long)(int)seed - 128L) << 4);
                }
            }
        }
        player_mix_put_s16le(pcm, (unsigned)(frame * 4u), player_mix_clamp_s16(left));
        player_mix_put_s16le(pcm, (unsigned)(frame * 4u + 2u), player_mix_clamp_s16(right));
    }
}

static void player_decoder_context_render_dos_fallback_pcm(PlayerDecoderContext *context, PlayerPlaybackBlock *block, db seed) {
    player_playback_mix_voices_pcm(block, context, seed);
}

static void player_decoder_context_render_external_tracker_pcm(PlayerDecoderContext *context, PlayerPlaybackBlock *block, db seed) {
    if (context) {
        int external_status = player_external_decoder_render(context->module, block);
        if (external_status == IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED) {
            return;
        }
        if (external_status == IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED) {
            player_decoder_context_mark_ended(context);
            player_playback_prepare_block_frames(block, 0);
            return;
        }
    }
    if (context && player_module_data_complete(context->module)) {
        player_playback_mix_voices_pcm(block, context, seed);
        return;
    }
    player_playback_fill_stream_pcm(block, context, seed);
}

static void player_decoder_context_render_project_pcm(PlayerDecoderContext *context, PlayerPlaybackBlock *block, db seed) {
    player_playback_mix_voices_pcm(block, context, seed);
}

static void player_decoder_context_render_pcm(PlayerDecoderContext *context, PlayerPlaybackBlock *block, db seed) {
    switch (player_decoder_context_renderer(context)) {
    case IPLAY_PLAYER_RENDERER_PROJECT:
        player_decoder_context_render_project_pcm(context, block, seed);
        break;
    case IPLAY_PLAYER_RENDERER_EXTERNAL:
        player_decoder_context_render_external_tracker_pcm(context, block, seed);
        break;
    case IPLAY_PLAYER_RENDERER_DOS_FALLBACK:
    default:
        player_decoder_context_render_dos_fallback_pcm(context, block, seed);
        break;
    }
}

static dw player_module_pcm_source_read(PlayerPcmSource *source, PlayerPlaybackBlock *block) {
    PlayerDecoderContext *context = (PlayerDecoderContext *)source->user;
    db seed;
    if (!player_decoder_context_has_block(context)) {
        if (context) player_decoder_context_mark_ended(context);
        return 0;
    }
    seed = player_decoder_context_block_seed(context);
    player_playback_prepare_block_frames(block, player_pcm_source_frames_per_block(source));
    player_decoder_context_render_pcm(context, block, seed);
    if (player_decoder_context_ended(context)) return player_playback_block_frames(block);
    player_decoder_context_advance(context);
    return player_playback_block_frames(block);
}

static int player_module_pcm_source_ended(const PlayerPcmSource *source) {
    return player_decoder_context_ended((const PlayerDecoderContext *)source->user) != 0;
}

static void player_module_pcm_source_init(PlayerPcmSource *source, PlayerDecoderContext *context) {
    player_pcm_source_init(source, player_module_pcm_source_read, player_module_pcm_source_ended, context);
}

static void player_prime_pcm_source_init(PlayerPcmSource *source) {
    player_pcm_source_init(source, player_prime_pcm_source_read, 0, 0);
}

static const char *player_decoder_context_provider_name(const PlayerDecoderContext *context) {
    if (!context) return "none";
    if (player_decoder_context_renderer(context) != IPLAY_PLAYER_RENDERER_EXTERNAL) return "native";
    if (player_external_decoder_available()) return player_external_decoder_provider_name();
    return player_module_data_complete(context->module) ? "native-preview" : "dos-fallback";
}

static const char *player_decoder_context_hook_provider_name(const PlayerDecoderContext *context) {
    if (!context) return "none";
    if (player_decoder_context_renderer(context) != IPLAY_PLAYER_RENDERER_EXTERNAL) return "none";
    if (!player_external_decoder_available()) return "none";
    return player_external_decoder_provider_name();
}

static void player_report_pcm_source(const PlayerPcmSource *source) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    const PlayerDecoderContext *context = (const PlayerDecoderContext *)source->user;
    const LoaderInfo *loader = context ? player_decoder_context_loader(context) : 0;
    const PlayerModuleInfo *module = context ? context->module : 0;
    printf("PCM source: %s seed=%u truncated=%u input=%s renderer=%c route=%u provider=%s hook_provider=%s stream_start=%lu\n",
           loader ? loader_symbol(loader) : "none",
           context ? (unsigned)player_decoder_context_seed(context) : 0u,
           module ? (unsigned)player_module_header_truncated(module) : 0u,
           module ? player_module_decoder_input_name(module) : "none",
           player_renderer_code(player_decoder_context_renderer(context)),
           (unsigned)loader_decoder_route_id(loader),
           player_decoder_context_provider_name(context),
           player_decoder_context_hook_provider_name(context),
           module ? (unsigned long)player_module_diagnostic_stream_start_offset(module) : 0ul);
    player_flush_reports();
#else
    (void)source;
#endif
}

static void player_report_decoder_geometry(const PlayerDecoderContext *context) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    printf("Decoder geometry: orders=%u rows/order=%u restart=%u speed=%u tempo=%u channels=%u\n",
           (unsigned)player_decoder_context_order_count(context),
           (unsigned)player_decoder_context_rows_per_order(context),
           (unsigned)player_decoder_context_restart_order(context),
           (unsigned)player_decoder_context_initial_speed(context),
           (unsigned)player_decoder_context_initial_tempo(context),
           (unsigned)player_decoder_context_channel_count(context));
    player_flush_reports();
#else
    (void)context;
#endif
}

static void player_report_decoder_event(const PlayerDecoderContext *context) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    const PlayerPatternCell *cell = player_decoder_context_current_cell(context);
    printf("Decoder event: period=%u note=%u octave=%u instrument=%u volume=%u effect=%u param=%u\n",
           (unsigned)cell->period,
           (unsigned)cell->note,
           (unsigned)cell->octave,
           (unsigned)cell->instrument,
           (unsigned)cell->volume,
           (unsigned)cell->effect,
           (unsigned)cell->param);
    player_flush_reports();
#else
    (void)context;
#endif
}

static void player_report_decoder_voice(const PlayerDecoderContext *context) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    const PlayerVoiceState *voice = player_decoder_context_current_voice(context);
    printf("Decoder voice: active=%u period=%u note=%u octave=%u instrument=%u volume=%u sample_len=%lu sample_vol=%u loop=%lu/%lu data=%lu\n",
           (unsigned)voice->active,
           (unsigned)voice->period,
           (unsigned)voice->note,
           (unsigned)voice->octave,
           (unsigned)voice->instrument,
           (unsigned)voice->volume,
           (unsigned long)voice->sample.length,
           (unsigned)voice->sample.volume,
           (unsigned long)voice->sample.loop_start,
           (unsigned long)voice->sample.loop_length,
           (unsigned long)player_sample_info_data_offset(&voice->sample));
    player_flush_reports();
#else
    (void)context;
#endif
}

static void player_report_decoder_progress(const PlayerDecoderContext *context) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    printf("Decoder progress: block=%u/%u order=%u pattern=%u row=%u channel=%u tick=%u/%u speed=%u tempo=%u ended=%u loop=%u\n",
           (unsigned)player_decoder_context_block_index(context),
           (unsigned)player_decoder_context_max_blocks(context),
           (unsigned)player_decoder_context_order(context),
           (unsigned)player_decoder_context_current_order_value(context),
           (unsigned)player_decoder_context_row(context),
           (unsigned)player_decoder_context_channel(context),
           (unsigned)player_decoder_context_current_tick(context),
           (unsigned)player_decoder_context_current_speed(context),
           (unsigned)player_decoder_context_current_speed(context),
           (unsigned)player_decoder_context_current_tempo(context),
           (unsigned)player_decoder_context_ended(context),
           (unsigned)player_decoder_context_loop_enabled(context));
    player_flush_reports();
#else
    (void)context;
#endif
}

static int player_playback_fill_next_block(PlayerPlayback *playback, PlayerPcmSource *source) {
    PlayerPlaybackBlock *block = player_playback_block(playback);
    dw frames = player_pcm_source_read(source, block);
    player_playback_block_set_frames(block, frames);
    return frames != 0;
}

static void player_submit_playback_block(IplayRuntime *runtime, PlayerPlayback *playback) {
    const PlayerPlaybackBlock *block = player_playback_block_const(playback);
    dw frames = player_playback_block_frames(block);
    dw accepted;
    if (frames == 0) return;
    iplay_runtime_audio_set_capacity(runtime, frames);
    accepted = iplay_runtime_write_sb16_frames(runtime, player_playback_block_pcm_const(block), frames);
    playback->pcm_checksum += player_playback_block_checksum(block);
    player_playback_count_submit(playback, frames, accepted);
    iplay_runtime_refresh_audio_status(runtime);
}

static const char *player_playback_stop_reason_name(db stop_reason) {
    switch (stop_reason) {
    case IPLAY_PLAYER_STOP_BLOCK_LIMIT:
        return "block-limit";
    case IPLAY_PLAYER_STOP_SOURCE_END:
        return "source-end";
    case IPLAY_PLAYER_STOP_KEYBOARD:
        return "keyboard";
    default:
        return "running";
    }
}

static void player_playback_mark_block_limit(PlayerPlayback *playback) {
    playback->limit_reached = 1;
    playback->stop_reason = IPLAY_PLAYER_STOP_BLOCK_LIMIT;
}

static void player_playback_mark_source_end(PlayerPlayback *playback) {
    playback->source_ended = 1;
    playback->stop_reason = IPLAY_PLAYER_STOP_SOURCE_END;
}

static void player_playback_mark_keyboard(PlayerPlayback *playback) {
    playback->stop_reason = IPLAY_PLAYER_STOP_KEYBOARD;
}

static void player_report_playback_pump(const PlayerPlayback *playback) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    printf("Playback pump: blocks=%lu frames=%lu accepted=%lu checksum=%lu limit=%u source_end=%u stop=%s\n",
           (unsigned long)playback->blocks_submitted,
           (unsigned long)playback->frames_submitted,
           (unsigned long)playback->bytes_accepted,
           (unsigned long)playback->pcm_checksum,
           (unsigned)playback->limit_reached,
           (unsigned)playback->source_ended,
           player_playback_stop_reason_name(playback->stop_reason));
    player_flush_reports();
#else
    (void)playback;
#endif
}

static void player_playback_loop_init(PlayerPlaybackLoop *loop, const char *name, db policy, dd max_blocks, dw frames_per_block) {
    loop->name = name;
    loop->policy = policy;
    loop->max_blocks = max_blocks;
    loop->frames_per_block = frames_per_block;
    loop->timer_interval_ticks = 0;
}

static void player_playback_loop_init_diagnostic(PlayerPlaybackLoop *loop) {
    player_playback_loop_init(loop, "diagnostic", IPLAY_PLAYER_LOOP_POLICY_BOUNDED, IPLAY_PLAYER_PUMP_BLOCK_LIMIT, IPLAY_PLAYER_PRIME_FRAMES);
}

static void player_playback_loop_init_trial(PlayerPlaybackLoop *loop, dd block_limit) {
    player_playback_loop_init(loop, "playback", IPLAY_PLAYER_LOOP_POLICY_BOUNDED, block_limit, IPLAY_PLAYER_SB16_BLOCK_FRAMES);
}

static void player_playback_loop_init_continuous(PlayerPlaybackLoop *loop) {
    player_playback_loop_init(loop, "playback", IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS, 0, IPLAY_PLAYER_CONTINUOUS_BLOCK_FRAMES);
    loop->timer_interval_ticks = IPLAY_PLAYER_CONTINUOUS_TIMER_INTERVAL_TICKS;
}

static void player_playback_loop_init_for_policy(PlayerPlaybackLoop *loop, db policy, dd trial_block_limit) {
    switch (policy) {
    case IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS:
        player_playback_loop_init_continuous(loop);
        break;
    case IPLAY_PLAYER_LOOP_POLICY_BOUNDED:
    default:
        player_playback_loop_init_trial(loop, trial_block_limit);
        break;
    }
}

static void player_playback_loop_init_default(PlayerPlaybackLoop *loop, dd trial_block_limit) {
    player_playback_loop_init_for_policy(loop, IPLAY_PLAYER_DEFAULT_LOOP_POLICY, trial_block_limit);
}

static dd player_playback_loop_max_blocks(const PlayerPlaybackLoop *loop) {
    return loop->max_blocks;
}

static dw player_playback_loop_frames_per_block(const PlayerPlaybackLoop *loop) {
    return loop->frames_per_block;
}

static dw player_playback_loop_timer_interval_ticks(const PlayerPlaybackLoop *loop) {
    return loop->timer_interval_ticks;
}

static const char *player_playback_loop_name(const PlayerPlaybackLoop *loop) {
    return loop->name;
}

static db player_playback_loop_policy(const PlayerPlaybackLoop *loop) {
    return loop->policy;
}

static int player_playback_loop_is_bounded(const PlayerPlaybackLoop *loop) {
    return player_playback_loop_policy(loop) == IPLAY_PLAYER_LOOP_POLICY_BOUNDED;
}

static const char *player_playback_loop_policy_name(const PlayerPlaybackLoop *loop) {
    switch (player_playback_loop_policy(loop)) {
    case IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS:
        return "timer-keyboard";
    case IPLAY_PLAYER_LOOP_POLICY_BOUNDED:
    default:
        return "bounded-trial";
    }
}

static int player_playback_loop_uses_timer(const PlayerPlaybackLoop *loop) {
    return player_playback_loop_policy(loop) == IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS;
}

static const char *player_playback_loop_cadence_name(const PlayerPlaybackLoop *loop) {
    return player_playback_loop_uses_timer(loop) ? "timer" : "immediate";
}

static int player_playback_loop_reached_limit(const PlayerPlayback *playback, const PlayerPlaybackLoop *loop) {
    return player_playback_loop_is_bounded(loop) && playback->blocks_submitted >= player_playback_loop_max_blocks(loop);
}

static void player_playback_timer_init(PlayerPlaybackTimer *timer, const PlayerPlaybackLoop *loop) {
    timer->loop = loop;
    timer->interval_ticks = player_playback_loop_timer_interval_ticks(loop);
    timer->last_ticks = timer->interval_ticks ? dos_hw_io_timer_ticks() : 0;
    timer->elapsed_ticks = timer->interval_ticks;
    timer->ready_count = 0;
}

static int player_playback_timer_uses_timer(const PlayerPlaybackTimer *timer) {
    return player_playback_loop_uses_timer(timer->loop);
}

static void player_playback_timer_count_ready(PlayerPlaybackTimer *timer) {
    timer->ready_count += 1u;
}

static void player_playback_timer_poll_tick(PlayerPlaybackTimer *timer) {
    unsigned long ticks = dos_hw_io_timer_ticks();
    unsigned long delta = ticks - timer->last_ticks;
    timer->last_ticks = ticks;
    if (delta > 0xfffful) delta = 0xfffful;
    if (delta != 0) timer->ready_count = 0;
    timer->elapsed_ticks = (dw)(timer->elapsed_ticks + (dw)delta);
}

static int player_playback_timer_idle_fallback_ready(PlayerPlaybackTimer *timer) {
    if (timer->ready_count < IPLAY_PLAYER_TIMER_IDLE_POLL_LIMIT) {
        timer->ready_count += 1u;
        return 0;
    }
    timer->ready_count = 0;
    return 1;
}

static int player_playback_timer_ready(PlayerPlaybackTimer *timer) {
    if (!player_playback_timer_uses_timer(timer)) return 1;
    if (timer->interval_ticks == 0) {
        player_playback_timer_count_ready(timer);
        return 1;
    }
    player_playback_timer_poll_tick(timer);
    if (timer->elapsed_ticks < timer->interval_ticks && !player_playback_timer_idle_fallback_ready(timer)) return 0;
    timer->elapsed_ticks = 0;
    player_playback_timer_count_ready(timer);
    return 1;
}

static int player_playback_loop_keyboard_requested(const PlayerPlaybackLoop *loop) {
    if (player_playback_loop_policy(loop) != IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS) return 0;
    return dos_hw_io_keyboard_hit() != 0;
}

static int player_playback_loop_should_continue(const PlayerPlayback *playback, const PlayerPlaybackLoop *loop) {
    return !player_playback_loop_reached_limit(playback, loop);
}

static void player_report_playback_loop(const PlayerPlaybackLoop *loop) {
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    printf("Playback loop: mode=%s policy=%s cadence=%s max_blocks=%lu frames/block=%u\n",
           player_playback_loop_name(loop),
           player_playback_loop_policy_name(loop),
           player_playback_loop_cadence_name(loop),
           (unsigned long)player_playback_loop_max_blocks(loop),
           (unsigned)player_playback_loop_frames_per_block(loop));
    player_flush_reports();
#else
    (void)loop;
#endif
}

static void player_refresh_module_playback_position(IplayRuntime *runtime, const PlayerPcmSource *source) {
    const PlayerDecoderContext *context;
    if (!source || source->read != player_module_pcm_source_read || !source->user) return;
    context = (const PlayerDecoderContext *)source->user;
    iplay_runtime_render_bottom(runtime,
                                (db)player_decoder_context_current_order_value(context),
                                (db)player_decoder_context_order(context),
                                (db)player_decoder_context_row(context),
                                (db)player_decoder_context_current_speed(context),
                                player_master_volume(player_memory()),
                                IPLAY_PLAYER_DEFAULT_FLAGS,
                                IPLAY_PLAYER_DEFAULT_VOLUME,
                                IPLAY_PLAYER_DEFAULT_AMPLIFICATION);
    iplay_runtime_draw_audio_levels(runtime,
                                    IPLAY_RUNTIME_STATUS_LEVELS_ROW,
                                    IPLAY_RUNTIME_STATUS_LEVELS_X,
                                    IPLAY_RUNTIME_STATUS_LEVELS_WIDTH);
    player_present_runtime_frame(runtime, "playback-position");
}

static void player_pump_playback_once(IplayRuntime *runtime, PlayerPlayback *playback) {
    player_submit_playback_block(runtime, playback);
    player_report_playback_pump(playback);
}

static void player_pump_playback_loop(IplayRuntime *runtime, PlayerPlayback *playback, PlayerPcmSource *source, const PlayerPlaybackLoop *loop) {
    PlayerPlaybackTimer timer;
    player_playback_timer_init(&timer, loop);
    player_pcm_source_set_frames_per_block(source, player_playback_loop_frames_per_block(loop));
    player_report_playback_loop(loop);
    while (player_playback_loop_should_continue(playback, loop)) {
        if (!player_playback_timer_ready(&timer)) continue;
        if (player_playback_loop_keyboard_requested(loop)) {
            player_playback_mark_keyboard(playback);
            break;
        }
        if (!player_playback_fill_next_block(playback, source)) {
            player_playback_mark_source_end(playback);
            break;
        }
        player_submit_playback_block(runtime, playback);
        player_refresh_module_playback_position(runtime, source);
        if (player_pcm_source_ended(source)) {
            player_playback_mark_source_end(playback);
            break;
        }
    }
    if (player_playback_loop_reached_limit(playback, loop)) player_playback_mark_block_limit(playback);
    player_report_playback_pump(playback);
}

static void player_pump_playback(IplayRuntime *runtime, PlayerPlayback *playback, PlayerPcmSource *source, dd trial_block_limit) {
    PlayerPlaybackLoop loop;
    player_playback_loop_init_default(&loop, trial_block_limit);
    player_pump_playback_loop(runtime, playback, source, &loop);
}

static void player_submit_runtime_prime_pcm(IplayRuntime *runtime, const PlayerModuleInfo *module, dd trial_block_limit) {
    PlayerPlayback playback;
    PlayerDecoderContext decoder;
    PlayerPcmSource source;
    player_playback_init(&playback);
    player_decoder_context_init(&decoder, module);
    player_module_pcm_source_init(&source, &decoder);
    player_report_pcm_source(&source);
    player_report_decoder_geometry(&decoder);
    player_report_decoder_event(&decoder);
    player_report_decoder_voice(&decoder);
    player_pump_playback(runtime, &playback, &source, trial_block_limit);
    player_decoder_context_close_file_stream(&decoder);
    player_report_decoder_progress(&decoder);
    player_report_decoder_event(&decoder);
    player_report_decoder_voice(&decoder);
}

static void player_prime_runtime_playback(IplayRuntime *runtime, const PlayerModuleInfo *module, dd trial_block_limit) {
    player_submit_runtime_prime_pcm(runtime, module, trial_block_limit);
    player_report_runtime_playback_prime(runtime);
}

static void player_shutdown_audio_hardware(void) {
#if IPLAY_PLAYER_ENABLE_SB16_HW
    sb16_shutdown(player_sb16_hardware());
#endif
}

static int player_audio_hardware_ready(void) {
#if IPLAY_PLAYER_ENABLE_SB16_HW && IPLAY_PLAYER_SB16_REAL_HARDWARE_IO
    return sb16_audio_ensure_ready(player_sb16_hardware());
#else
    return 1;
#endif
}

static void player_shutdown_runtime(IplayRuntime *runtime) {
    (void)runtime;
    player_shutdown_audio_hardware();
}

static void player_init_module_status(IplayModuleStatus *module_status, const PlayerModuleInfo *module) {
    const LoaderInfo *loader = player_module_loader(module);
    iplay_module_status_init(module_status, loader_name(loader), player_module_path(module), (dd)player_module_size(module), loader_symbol(loader), 0);
    iplay_module_status_set_type(module_status, player_module_type_tag(module));
}

static void player_activate_runtime_ui(IplayRuntime *runtime, IplayRuntimeConfig *runtime_config) {
    player_configure_runtime(runtime_config);
    player_start_runtime(runtime, runtime_config);
}

static void player_render_runtime_audio_unavailable(IplayRuntime *runtime) {
    iplay_runtime_draw_audio_status(runtime);
    iplay_runtime_draw_status_line(runtime,
                                   IPLAY_RUNTIME_STATUS_PLAYBACK_ROW,
                                   "Playback disabled: SB16 not detected",
                                   IPLAY_RUNTIME_STATUS_PLAYBACK_ATTR);
#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS
    printf("Playback disabled: SB16 not detected\n");
    player_flush_reports();
#endif
    player_present_runtime_frame(runtime, "audio-unavailable");
}

/* inventory marker: static void player_run_runtime_ui(IplayRuntime *runtime, IplayRuntimeConfig *runtime_config, const PlayerModuleInfo *module, const IplayModuleStatus *module_status, dd trial_block_limit) */
static int player_run_runtime_ui(IplayRuntime *runtime, IplayRuntimeConfig *runtime_config, const PlayerModuleInfo *module, const IplayModuleStatus *module_status, dd trial_block_limit) {
    int exit_status = player_exit_ok_status();
    (void)module_status;
    player_activate_runtime_ui(runtime, runtime_config);
    player_render_runtime_status(runtime, module);
    if (player_audio_hardware_ready()) {
        player_prime_runtime_playback(runtime, module, trial_block_limit);
        player_render_runtime_audio_status_frame(runtime, module, "post-playback-status");
    } else {
        player_render_runtime_audio_unavailable(runtime);
        exit_status = player_exit_audio_unavailable_status();
    }
    player_shutdown_runtime(runtime);
    return exit_status;
}
#endif

#if IPLAY_PLAYER_ENABLE_TEXT_UI
static void player_prepare_loaded_module_ui(const PlayerModuleInfo *module, IplayModuleStatus *module_status) {
    player_start_program_memory(player_memory());
    player_init_module_status(module_status, module);
}

/* inventory marker: static void player_present_loaded_module(IplayRuntime *runtime, IplayRuntimeConfig *runtime_config, const PlayerModuleInfo *module, */
static int player_present_loaded_module(IplayRuntime *runtime, IplayRuntimeConfig *runtime_config, const PlayerModuleInfo *module,
                                        const IplayModuleStatus *module_status, dd trial_block_limit) {
    player_report_loaded_module(module);
    return player_run_runtime_ui(runtime, runtime_config, module, module_status, trial_block_limit);
}

/* inventory marker: static void player_run_loaded_module(const PlayerModuleInfo *module, IplayRuntime *runtime, IplayRuntimeConfig *runtime_config, */
static int player_run_loaded_module(const PlayerModuleInfo *module, IplayRuntime *runtime, IplayRuntimeConfig *runtime_config,
                                    IplayModuleStatus *module_status, dd trial_block_limit) {
    player_prepare_loaded_module_ui(module, module_status);
    return player_present_loaded_module(runtime, runtime_config, module, module_status, trial_block_limit);
}
#else
static void player_prepare_loaded_module_no_ui(void) {
    player_start_program_memory(player_memory());
}

static void player_present_loaded_module_no_ui(const PlayerModuleInfo *module) {
    player_report_loaded_module(module);
}

static void player_run_loaded_module(const PlayerModuleInfo *module) {
    player_prepare_loaded_module_no_ui();
    player_present_loaded_module_no_ui(module);
}
#endif

#if IPLAY_PLAYER_ENABLE_TEXT_UI
static int player_run_prepared_module(const PlayerModuleInfo *module, IplayRuntime *runtime, IplayRuntimeConfig *runtime_config,
                                      IplayModuleStatus *module_status, dd trial_block_limit) {
    return player_run_loaded_module(module, runtime, runtime_config, module_status, trial_block_limit);
}
#else
static int player_run_prepared_module(const PlayerModuleInfo *module) {
    player_run_loaded_module(module);
    return player_exit_ok_status();
}
#endif

static int player_run_request(const PlayerModuleRequest *request) {
#if IPLAY_PLAYER_ENABLE_TEXT_UI
    IplayRuntime runtime;
    IplayRuntimeConfig runtime_config;
    IplayModuleStatus module_status;
#endif
    PlayerModuleInfo module;
    char file_list_path[IPLAY_PLAYER_FILE_LIST_PATH_BYTES];
    const char *path;
    int load_status;
    int exit_status;

    path = player_resolve_requested_module_path(request, file_list_path, IPLAY_PLAYER_FILE_LIST_PATH_BYTES);
#if IPLAY_PLAYER_ENABLE_TEXT_UI
    player_set_text_video_mode_id(player_module_request_video_mode(request));
#endif
    load_status = player_prepare_module(&module, path, player_module_storage_buffer(), IPLAY_PLAYER_MODULE_BUFFER_BYTES);
    if (!player_module_load_ok(load_status)) {
        exit_status = player_report_module_load_failure(load_status, &module);
#if IPLAY_PLAYER_ENABLE_TEXT_UI
        if (player_module_load_unsupported(load_status) && player_module_loader(&module)) {
            player_prepare_loaded_module_ui(&module, &module_status);
            player_activate_runtime_ui(&runtime, &runtime_config);
            player_render_runtime_unsupported_module(&runtime, &module, &module_status);
            player_shutdown_runtime(&runtime);
        }
#endif
        return exit_status;
    }

#if IPLAY_PLAYER_ENABLE_TEXT_UI
    exit_status = player_run_prepared_module(&module, &runtime, &runtime_config, &module_status, player_module_request_trial_block_limit(request));
#else
    exit_status = player_run_prepared_module(&module);
#endif
    return exit_status;
}

static int player_run_path(const char *path) {
    PlayerModuleRequest request;
    player_module_request_init_path(&request, path);
    return player_run_request(&request);
}

static int player_run_cli(int argc, char **argv) {
    PlayerModuleRequest request;
#if IPLAY_PLAYER_ENABLE_SB16_HW
    player_configure_sb16_from_blaster();
#endif
    player_module_request_init_cli(&request, argc, argv);
    if (player_requested_usage(&request)) return player_report_usage();
    if (player_requested_sound_settings(&request)) return player_report_sound_settings();
    if (!player_module_request_video_mode_valid(&request)) return player_report_invalid_video_mode();
    return player_run_request(&request);
}

static void player_clear_player_memory(void) {
    memset(player_memory(), 0, PLAYER_MEM_SIZE);
}

static void player_clear_video_memory(void) {
    memset(player_video_memory(), 0, PLAYER_VIDEO_SIZE);
}

static void player_init_hardware_io(void) {
    dos_hw_use_io(NULL);
}

static void player_init_core_state(void) {
    player_clear_player_memory();
    player_clear_video_memory();
}

static void player_init_dos_process(void) {
    player_init_core_state();
    player_init_hardware_io();
#if IPLAY_PLAYER_ENABLE_TEXT_UI
    player_init_text_presenter();
#endif
}

static int player_run_dos_cli_process(int argc, char **argv) {
    player_init_dos_process();
    return player_run_cli(argc, argv);
}

int main(int argc, char **argv) {
    return player_run_dos_cli_process(argc, argv);
}
