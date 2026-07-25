#ifndef IPLAY_REWRITE_H
#define IPLAY_REWRITE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t  db;
typedef uint16_t dw;
typedef uint32_t dd;

typedef struct IplayRegs {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd ebp;
    dd esi;
    dd edi;
} IplayRegs;

typedef struct IplayNcPlane {
    db *cells;
    dw rows;
    dw cols;
    dw stride_cols;
    dw origin_y;
    dw origin_x;
    dw cursor_y;
    dw cursor_x;
} IplayNcPlane;

typedef struct IplayWindow {
    IplayNcPlane plane;
} IplayWindow;

typedef struct IplayTextMode {
    dw cols;
    dw rows;
} IplayTextMode;

typedef struct IplayTextScreen {
    db *cells;
    dw capacity_bytes;
    IplayTextMode mode;
    IplayNcPlane root;
} IplayTextScreen;

typedef void (*IplayVideoPresentFn)(void *user, const db *cells, const IplayTextMode *mode, dw byte_count);

typedef enum IplayTerminalBackend {
    IPLAY_TERMINAL_BACKEND_VGA_MEMORY = 0
} IplayTerminalBackend;

typedef struct IplayVideoSpec {
    IplayTerminalBackend backend;
    IplayTextMode mode;
    db present_enabled;
} IplayVideoSpec;

typedef struct IplayTerminal {
    IplayTextScreen screen;
    IplayTerminalBackend backend;
    IplayVideoPresentFn present;
    void *present_user;
} IplayTerminal;

typedef struct IplayNotcurses {
    IplayTerminal terminal;
} IplayNotcurses;

typedef struct IplayBottomLayout {
    dw module_y;
    dw pattern_y;
    dw timing_y;
    dw left_x;
    dw mode_x;
    dw value_x;
    dw flag_x;
    dw playstate_y;
    dw playstate_x;
    dw module_width;
    dw pattern_width;
    dw timing_width;
    dw mode_width;
    dw value_width;
    dw playstate_width;
} IplayBottomLayout;

typedef struct IplayAudioFormat {
    dw sample_rate;
    db bits_per_sample;
    db channels;
    db signed_samples;
} IplayAudioFormat;

typedef void (*IplayAudioWriteFn)(void *user, const db *pcm, dw byte_count);
typedef struct PlayerModuleInfo PlayerModuleInfo;
typedef struct PlayerPlaybackBlock PlayerPlaybackBlock;
#define IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE 0
#define IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED 1
#define IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED 2
typedef int (*PlayerExternalDecoderRenderFn)(void *user, const PlayerModuleInfo *module, PlayerPlaybackBlock *block);
void iplay_player_set_external_decoder(PlayerExternalDecoderRenderFn render, void *user, const char *provider);
void iplay_player_clear_external_decoder(void);
const char *iplay_player_module_path(const PlayerModuleInfo *module);
unsigned long iplay_player_module_size(const PlayerModuleInfo *module);
int iplay_player_module_header_truncated(const PlayerModuleInfo *module);
const char *iplay_player_module_decoder_input_name(const PlayerModuleInfo *module);
db *iplay_player_playback_block_pcm(PlayerPlaybackBlock *block);
dw iplay_player_playback_block_frames(const PlayerPlaybackBlock *block);
dw iplay_player_playback_block_capacity_frames(void);
void iplay_player_playback_block_set_frames(PlayerPlaybackBlock *block, dw frames);
dw iplay_player_playback_block_active_bytes(const PlayerPlaybackBlock *block);
typedef dw (*IplaySdlAudioCallback)(void *user, db *stream, dw byte_count);

typedef struct IplayAudioSink {
    IplayAudioFormat format;
    IplayAudioWriteFn write;
    void *user;
    dd frames_written;
    dd underrun_frames;
    dd dropped_frames;
    dd capacity_frames;
    db active;
} IplayAudioSink;

typedef struct IplayAudioLevels {
    dw left_peak;
    dw right_peak;
    db left_16;
    db right_16;
} IplayAudioLevels;

typedef struct IplayAudioOutput {
    IplayAudioSink sink;
    IplayAudioFormat source_format;
    db *scratch;
    dw scratch_bytes;
    IplayAudioLevels levels;
} IplayAudioOutput;

typedef enum IplayAudioBackend {
    IPLAY_AUDIO_BACKEND_SB16_STEREO = 0,
    IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE = 1
} IplayAudioBackend;

typedef struct IplaySdlAudioSpec {
    IplayAudioBackend backend;
    IplayAudioFormat format;
    db hardware_enabled;
} IplaySdlAudioSpec;

typedef struct IplayRuntimeOutputSpec {
    IplayTerminalBackend video_backend;
    IplayAudioBackend audio_backend;
    db audio_hardware_enabled;
} IplayRuntimeOutputSpec;

typedef struct IplaySdlAudioDeviceConfig {
    dw frequency;
    db bits_per_sample;
    db channels;
    db signed_samples;
    dw samples;
    IplaySdlAudioCallback callback;
    void *userdata;
    IplayAudioBackend backend;
    db hardware_enabled;
} IplaySdlAudioDeviceConfig;

typedef struct IplaySdlAudioDevice {
    IplayAudioOutput output;
    IplaySdlAudioDeviceConfig config;
    IplayAudioBackend backend;
    db hardware_enabled;
    db paused;
} IplaySdlAudioDevice;

typedef struct IplayRuntime {
    IplayNotcurses nc;
    IplaySdlAudioDevice audio;
    db video_mode_ok;
} IplayRuntime;

typedef struct IplayRuntimeConfig {
    db *cells;
    dw cell_capacity_bytes;
    const IplayTextMode *mode;
    IplayVideoPresentFn present;
    void *present_user;
    IplayAudioWriteFn audio_write;
    void *audio_user;
    IplayTerminalBackend video_backend;
    db video_present_enabled;
    IplayAudioBackend audio_backend;
    db audio_hardware_enabled;
} IplayRuntimeConfig;

typedef struct IplayModuleStatus {
    const char *title;
    const char *module_path;
    dd module_size;
    const char *loader_symbol;
    dd module_type;
} IplayModuleStatus;

typedef struct IplayDecimalResult {
    dd magnitude;
    dw count;
    dw offset;
    db last_digit;
} IplayDecimalResult;

typedef struct IplayStringCopyResult {
    dw src_offset;
    dw dst_offset;
    dw count;
    db last_byte;
    db copied_any;
} IplayStringCopyResult;

typedef struct IplayAttributedTextResult {
    dw src_offset;
    dw dst_offset;
    dw ax;
} IplayAttributedTextResult;

typedef struct IplayAsmSprintfResult {
    dw src_offset;
    dw dst_offset;
    dd eax;
    dd ecx;
    dd edx;
} IplayAsmSprintfResult;

typedef struct IplayScreenStreamResult {
    dw src_offset;
    dw dst_offset;
    dw ax;
} IplayScreenStreamResult;

typedef struct IplayRecolorResult {
    dw dst_offset;
    dw ax;
} IplayRecolorResult;

typedef struct IplaySb16ProbeResult {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
} IplaySb16ProbeResult;

typedef struct IplaySb16RegsResult {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
} IplaySb16RegsResult;

typedef struct IplayRegs6Result {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd esi;
    dd edi;
} IplayRegs6Result;

typedef struct IplayRegs3Result {
    dd eax;
    dd ecx;
    dd edx;
} IplayRegs3Result;

typedef struct IplaySndSettingsResult {
    dd eax;
    dd ebx;
    dd ecx;
    dd edx;
    dd ebp;
    dd esi;
} IplaySndSettingsResult;

typedef enum IplayTextColor {
    IPLAY_TEXT_BLACK = 0,
    IPLAY_TEXT_BLUE = 1,
    IPLAY_TEXT_GREEN = 2,
    IPLAY_TEXT_CYAN = 3,
    IPLAY_TEXT_RED = 4,
    IPLAY_TEXT_MAGENTA = 5,
    IPLAY_TEXT_BROWN = 6,
    IPLAY_TEXT_LIGHT_GRAY = 7,
    IPLAY_TEXT_DARK_GRAY = 8,
    IPLAY_TEXT_LIGHT_BLUE = 9,
    IPLAY_TEXT_LIGHT_GREEN = 10,
    IPLAY_TEXT_LIGHT_CYAN = 11,
    IPLAY_TEXT_LIGHT_RED = 12,
    IPLAY_TEXT_LIGHT_MAGENTA = 13,
    IPLAY_TEXT_YELLOW = 14,
    IPLAY_TEXT_WHITE = 15
} IplayTextColor;

#define IPLAY_TEXT_CELL_BYTES 2u
#define IPLAY_VIDEO_MODE_40X25_BW 0u
#define IPLAY_VIDEO_MODE_40X25_COLOR 1u
#define IPLAY_VIDEO_MODE_80X25_BW 2u
#define IPLAY_VIDEO_MODE_80X25_COLOR 3u
#define IPLAY_TEXT_COLS_40 40u
#define IPLAY_TEXT_COLS_80 80u
#define IPLAY_TEXT_ROWS_25 25u
#define IPLAY_TEXT_ROWS_28 28u
#define IPLAY_TEXT_ROWS_50 50u
#define IPLAY_TEXT_SUPPORTED_MODE_COUNT 4u
#define IPLAY_TEXT_FALLBACK_COLS IPLAY_TEXT_COLS_40
#define IPLAY_TEXT_FALLBACK_ROWS IPLAY_TEXT_ROWS_25
#define IPLAY_TEXT_DEFAULT_COLS IPLAY_TEXT_COLS_80
#define IPLAY_TEXT_DEFAULT_ROWS IPLAY_TEXT_ROWS_25
#define IPLAY_TEXT_MAX_COLS IPLAY_TEXT_COLS_80
#define IPLAY_TEXT_MAX_ROWS IPLAY_TEXT_ROWS_50
#define IPLAY_TEXT_PROJECT_MODE_80X50 0x50u
#define IPLAY_TEXT_PROJECT_MODE_80X28 0x28u
#define IPLAY_TEXT_DEFAULT_VIDEO_MODE IPLAY_VIDEO_MODE_80X25_COLOR
#define IPLAY_VIDEO_MODE_80X50_PROJECT IPLAY_TEXT_PROJECT_MODE_80X50
#define IPLAY_VIDEO_MODE_80X28_PROJECT IPLAY_TEXT_PROJECT_MODE_80X28
#define IPLAY_TEXT_ROW_BYTES(cols) ((dw)((cols) * IPLAY_TEXT_CELL_BYTES))
#define IPLAY_TEXT_OFFSET(cols, y, x) ((dw)(((dw)(y) * IPLAY_TEXT_ROW_BYTES(cols)) + ((dw)(x) * IPLAY_TEXT_CELL_BYTES)))
#define IPLAY_TEXT_DEFAULT_SCREEN_BYTES ((dw)(IPLAY_TEXT_DEFAULT_COLS * IPLAY_TEXT_DEFAULT_ROWS * IPLAY_TEXT_CELL_BYTES))
#define IPLAY_TEXT_FALLBACK_SCREEN_BYTES ((dw)(IPLAY_TEXT_FALLBACK_COLS * IPLAY_TEXT_FALLBACK_ROWS * IPLAY_TEXT_CELL_BYTES))
#define IPLAY_TEXT_MAX_SCREEN_BYTES ((dw)(IPLAY_TEXT_MAX_COLS * IPLAY_TEXT_MAX_ROWS * IPLAY_TEXT_CELL_BYTES))
#define IPLAY_VIDEO_METER_LEFT_X 1u
#define IPLAY_VIDEO_METER_CENTER_DIVISOR 4u
#define IPLAY_VIDEO_METER_RIGHT_DIVISOR 2u
#define IPLAY_VIDEO_METER_RIGHT_PAD 2u
#define IPLAY_RUNTIME_STATUS_TITLE_ROW 9u
#define IPLAY_RUNTIME_STATUS_MODULE_ROW 10u
#define IPLAY_RUNTIME_STATUS_SIZE_ROW 11u
#define IPLAY_RUNTIME_STATUS_LOADER_ROW 12u
#define IPLAY_RUNTIME_STATUS_AUDIO_ROW 13u
#define IPLAY_RUNTIME_STATUS_HARDWARE_ROW 14u
#define IPLAY_RUNTIME_STATUS_VIDEO_ROW 15u
#define IPLAY_RUNTIME_STATUS_LEVELS_ROW 16u
#define IPLAY_RUNTIME_STATUS_TAG_ROW 17u
#define IPLAY_RUNTIME_STATUS_PLAYBACK_ROW 18u
#define IPLAY_RUNTIME_STATUS_PANEL_ROW 8u
#define IPLAY_RUNTIME_STATUS_PANEL_HEIGHT 12u
#define IPLAY_RUNTIME_STATUS_PANEL_ATTR 0x1eu
#define IPLAY_RUNTIME_STATUS_PANEL_FILL_ATTR 0x17u
#define IPLAY_RUNTIME_STATUS_LEVELS_X 16u
#define IPLAY_RUNTIME_STATUS_LEVELS_WIDTH 24u
#define IPLAY_RUNTIME_STATUS_TITLE_ATTR 0x1fu
#define IPLAY_RUNTIME_STATUS_LABEL_ATTR 0x1eu
#define IPLAY_RUNTIME_STATUS_VALUE_ATTR 0x2fu
#define IPLAY_RUNTIME_STATUS_PLAYBACK_ATTR 0x4fu
#define IPLAY_RUNTIME_CONFIG_OK 0u
#define IPLAY_RUNTIME_CONFIG_MISSING_CELLS 1u
#define IPLAY_RUNTIME_CONFIG_MISSING_MODE 2u
#define IPLAY_RUNTIME_CONFIG_MISSING_AUDIO 3u
#define IPLAY_RUNTIME_CONFIG_SMALL_CELLS 4u

extern const IplayTextMode IPLAY_TEXT_MODE_40X25;
extern const IplayTextMode IPLAY_TEXT_MODE_80X25;
extern const IplayTextMode IPLAY_TEXT_MODE_80X28;
extern const IplayTextMode IPLAY_TEXT_MODE_80X50;
extern const IplayTextMode IPLAY_TEXT_DEFAULT_MODE;
extern const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_ORIGINAL;
extern const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_40COL;
extern const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_80COL;
extern const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_80X28;
extern const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_80X50;
extern const IplayAudioFormat IPLAY_AUDIO_SB16_STEREO_16;
extern const IplayAudioFormat IPLAY_AUDIO_U8_MONO;
extern const IplayAudioFormat IPLAY_AUDIO_U8_STEREO;
extern const IplayAudioFormat IPLAY_AUDIO_S16_MONO;
extern const IplayAudioFormat IPLAY_AUDIO_S16_STEREO;

void iplay_ncplane_init(IplayNcPlane *plane, db *cells, dw rows, dw cols);
void iplay_ncplane_init_mode(IplayNcPlane *plane, db *cells, const IplayTextMode *mode);
db *iplay_ncplane_cells_at(db *cells, dw stride_cols, dw origin_y, dw origin_x);
void iplay_text_screen_init(IplayTextScreen *screen, db *cells, const IplayTextMode *mode);
void iplay_text_screen_init_capacity(IplayTextScreen *screen, db *cells, dw capacity_bytes, const IplayTextMode *mode);
void iplay_text_screen_set_cells(IplayTextScreen *screen, db *cells);
void iplay_text_screen_set_capacity(IplayTextScreen *screen, dw capacity_bytes);
void iplay_text_screen_set_mode(IplayTextScreen *screen, const IplayTextMode *mode);
void iplay_text_screen_reinit_root(IplayTextScreen *screen);
void iplay_text_screen_resize(IplayTextScreen *screen, const IplayTextMode *mode);
int iplay_text_screen_resize_checked(IplayTextScreen *screen, const IplayTextMode *mode);
void iplay_text_screen_resize_to_size(IplayTextScreen *screen, dw cols, dw rows);
int iplay_text_screen_resize_to_size_checked(IplayTextScreen *screen, dw cols, dw rows);
int iplay_text_screen_can_resize(const IplayTextScreen *screen, const IplayTextMode *mode);
const IplayTextMode *iplay_text_screen_set_video_mode(IplayTextScreen *screen, db video_mode);
int iplay_text_screen_set_video_mode_checked(IplayTextScreen *screen, db video_mode);
dw iplay_text_screen_capacity(const IplayTextScreen *screen);
db *iplay_text_screen_cells(IplayTextScreen *screen);
const db *iplay_text_screen_cells_const(const IplayTextScreen *screen);
dd iplay_text_cells_checksum(const db *cells, dw byte_count);
dw iplay_text_cells_nonblank_count(const db *cells, dw byte_count);
dd iplay_text_screen_checksum(const IplayTextScreen *screen);
dw iplay_text_screen_nonblank_count(const IplayTextScreen *screen);
dw iplay_text_screen_bytes(const IplayTextScreen *screen);
IplayNcPlane *iplay_text_screen_root(IplayTextScreen *screen);
const IplayTextMode *iplay_text_screen_mode(const IplayTextScreen *screen);
const IplayBottomLayout *iplay_text_screen_bottom_layout(const IplayTextScreen *screen);
int iplay_text_screen_bottom_layout_fits(const IplayTextScreen *screen);
void iplay_terminal_init_vga_memory(IplayTerminal *terminal, db *cells, const IplayTextMode *mode);
void iplay_terminal_init_vga_memory_capacity(IplayTerminal *terminal, db *cells, dw capacity_bytes, const IplayTextMode *mode);
void iplay_terminal_set_backend(IplayTerminal *terminal, IplayTerminalBackend backend);
void iplay_terminal_set_present_fn(IplayTerminal *terminal, IplayVideoPresentFn present);
void iplay_terminal_set_present_user(IplayTerminal *terminal, void *user);
void iplay_terminal_set_present_callback(IplayTerminal *terminal, IplayVideoPresentFn present, void *user);
void iplay_terminal_clear_present_callback(IplayTerminal *terminal);
IplayTerminalBackend iplay_terminal_backend(const IplayTerminal *terminal);
int iplay_terminal_has_present(const IplayTerminal *terminal);
IplayTextScreen *iplay_terminal_screen(IplayTerminal *terminal);
const IplayTextScreen *iplay_terminal_screen_const(const IplayTerminal *terminal);
db *iplay_terminal_cells(IplayTerminal *terminal);
const db *iplay_terminal_cells_const(const IplayTerminal *terminal);
IplayVideoPresentFn iplay_terminal_present_callback(const IplayTerminal *terminal);
void *iplay_terminal_present_user(const IplayTerminal *terminal);
dw iplay_terminal_capacity(const IplayTerminal *terminal);
int iplay_terminal_bottom_layout_fits(const IplayTerminal *terminal);
IplayNcPlane *iplay_terminal_root(IplayTerminal *terminal);
const IplayTextMode *iplay_terminal_mode(const IplayTerminal *terminal);
const IplayTextMode *iplay_terminal_resize(IplayTerminal *terminal, const IplayTextMode *mode);
int iplay_terminal_resize_checked(IplayTerminal *terminal, const IplayTextMode *mode);
const IplayTextMode *iplay_terminal_resize_to_size(IplayTerminal *terminal, dw cols, dw rows);
int iplay_terminal_resize_to_size_checked(IplayTerminal *terminal, dw cols, dw rows);
const IplayTextMode *iplay_terminal_set_video_mode(IplayTerminal *terminal, db video_mode);
int iplay_terminal_set_video_mode_checked(IplayTerminal *terminal, db video_mode);
dw iplay_terminal_present(IplayTerminal *terminal);
void iplay_terminal_erase(IplayTerminal *terminal, db attr);
void iplay_terminal_draw_top_title(IplayTerminal *terminal);
void iplay_terminal_draw_bottom(IplayTerminal *terminal, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif);
void iplay_terminal_draw_audio_output_levels(IplayTerminal *terminal, dw y, dw x, const IplayAudioOutput *output, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr);
IplayTerminalBackend iplay_video_spec_backend(const IplayVideoSpec *spec);
const IplayTextMode *iplay_video_spec_mode(const IplayVideoSpec *spec);
dw iplay_video_spec_cols(const IplayVideoSpec *spec);
dw iplay_video_spec_rows(const IplayVideoSpec *spec);
int iplay_video_spec_present_enabled(const IplayVideoSpec *spec);
void iplay_notcurses_init_vga_memory(IplayNotcurses *nc, db *cells, const IplayTextMode *mode);
void iplay_notcurses_init_vga_memory_capacity(IplayNotcurses *nc, db *cells, dw capacity_bytes, const IplayTextMode *mode);
IplayTerminal *iplay_notcurses_terminal(IplayNotcurses *nc);
const IplayTerminal *iplay_notcurses_terminal_const(const IplayNotcurses *nc);
IplayNcPlane *iplay_notcurses_stdplane(IplayNotcurses *nc);
const IplayTextMode *iplay_notcurses_mode(const IplayNotcurses *nc);
dw iplay_notcurses_capacity(const IplayNotcurses *nc);
dw iplay_notcurses_cols(const IplayNotcurses *nc);
dw iplay_notcurses_rows(const IplayNotcurses *nc);
dw iplay_notcurses_row_bytes(const IplayNotcurses *nc);
dw iplay_notcurses_screen_bytes(const IplayNotcurses *nc);
int iplay_notcurses_bottom_layout_fits(const IplayNotcurses *nc);
IplayVideoSpec iplay_notcurses_video_spec(const IplayNotcurses *nc);
IplayTerminalBackend iplay_notcurses_backend(const IplayNotcurses *nc);
int iplay_notcurses_present_enabled(const IplayNotcurses *nc);
int iplay_notcurses_has_present(const IplayNotcurses *nc);
IplayVideoPresentFn iplay_notcurses_present_callback(const IplayNotcurses *nc);
void *iplay_notcurses_present_user(const IplayNotcurses *nc);
void iplay_notcurses_set_present_fn(IplayNotcurses *nc, IplayVideoPresentFn present);
void iplay_notcurses_set_present_user(IplayNotcurses *nc, void *user);
void iplay_notcurses_set_present_callback(IplayNotcurses *nc, IplayVideoPresentFn present, void *user);
void iplay_notcurses_clear_present_callback(IplayNotcurses *nc);
const IplayTextMode *iplay_notcurses_resize(IplayNotcurses *nc, const IplayTextMode *mode);
int iplay_notcurses_resize_checked(IplayNotcurses *nc, const IplayTextMode *mode);
const IplayTextMode *iplay_notcurses_resize_to_size(IplayNotcurses *nc, dw cols, dw rows);
int iplay_notcurses_resize_to_size_checked(IplayNotcurses *nc, dw cols, dw rows);
const IplayTextMode *iplay_notcurses_set_video_mode(IplayNotcurses *nc, db video_mode);
int iplay_notcurses_set_video_mode_checked(IplayNotcurses *nc, db video_mode);
void iplay_notcurses_render_static(IplayNotcurses *nc, db erase_attr);
void iplay_notcurses_render_bottom(IplayNotcurses *nc, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif);
void iplay_notcurses_draw_audio_output_levels(IplayNotcurses *nc, dw y, dw x, const IplayAudioOutput *output, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr);
dw iplay_notcurses_present(IplayNotcurses *nc);
const IplayTextMode *iplay_text_mode_for_video_mode(db mode);
const IplayTextMode *iplay_text_mode_for_size(dw cols, dw rows);
const IplayTextMode *iplay_text_current_mode(void);
const IplayTextMode *iplay_set_current_text_video_mode(db video_mode);
const IplayTextMode *iplay_text_default_mode(void);
const IplayTextMode *iplay_text_fallback_mode(void);
const IplayTextMode *iplay_text_supported_mode(dw index);
dw iplay_text_supported_mode_count(void);
int iplay_text_size_is_supported(dw cols, dw rows);
int iplay_text_mode_is_supported(const IplayTextMode *mode);
dw iplay_text_mode_cols(const IplayTextMode *mode);
dw iplay_text_mode_rows(const IplayTextMode *mode);
dw iplay_text_mode_row_bytes(const IplayTextMode *mode);
dw iplay_text_mode_cells(const IplayTextMode *mode);
dw iplay_text_mode_screen_bytes(const IplayTextMode *mode);
int iplay_text_mode_fits_capacity(const IplayTextMode *mode, dw capacity_bytes);
dw iplay_text_max_screen_bytes(void);
int iplay_text_mode_equals(const IplayTextMode *a, const IplayTextMode *b);
const IplayBottomLayout *iplay_bottom_layout(void);
const IplayBottomLayout *iplay_bottom_layout_for_mode(const IplayTextMode *mode);
int iplay_bottom_layout_fits(const IplayBottomLayout *layout, const IplayTextMode *mode);
int iplay_ncplane_visible_region(const IplayNcPlane *plane, dw y, dw x, dw rows, dw cols, dw *visible_rows, dw *visible_cols);
void iplay_ncplane_origin_yx(const IplayNcPlane *plane, dw *y, dw *x);
dw iplay_ncplane_rows(const IplayNcPlane *plane);
dw iplay_ncplane_cols(const IplayNcPlane *plane);
dw iplay_ncplane_stride_cols(const IplayNcPlane *plane);
db *iplay_ncplane_cells(const IplayNcPlane *plane);
void iplay_ncplane_set_cells(IplayNcPlane *plane, db *cells);
void iplay_ncplane_set_size(IplayNcPlane *plane, dw rows, dw cols);
void iplay_ncplane_set_stride_cols(IplayNcPlane *plane, dw stride_cols);
void iplay_ncplane_set_origin_yx(IplayNcPlane *plane, dw y, dw x);
int iplay_ncplane_is_empty(const IplayNcPlane *plane);
dw iplay_ncplane_cursor_y(const IplayNcPlane *plane);
dw iplay_ncplane_cursor_x(const IplayNcPlane *plane);
void iplay_ncplane_set_cursor_yx_raw(IplayNcPlane *plane, dw y, dw x);
void iplay_ncplane_advance_cursor_x(IplayNcPlane *plane);
dw iplay_ncplane_cell_offset(const IplayNcPlane *plane, dw y, dw x);
db iplay_ncplane_cell_ch(const IplayNcPlane *plane, dw offset);
db iplay_ncplane_cell_attr(const IplayNcPlane *plane, dw offset);
void iplay_ncplane_put_cell_offset(IplayNcPlane *plane, dw offset, db ch, db attr);
void iplay_ncplane_copy_cell_offset(IplayNcPlane *plane, dw dst_offset, dw src_offset);
dw iplay_audio_bytes_per_frame(const IplayAudioFormat *format);
dw iplay_audio_frames_for_bytes(const IplayAudioFormat *format, dw byte_count);
dw iplay_audio_format_sample_rate(const IplayAudioFormat *format);
db iplay_audio_format_bits_per_sample(const IplayAudioFormat *format);
db iplay_audio_format_channels(const IplayAudioFormat *format);
db iplay_audio_format_signed_samples(const IplayAudioFormat *format);
void iplay_audio_format_set(IplayAudioFormat *format, dw sample_rate, db bits_per_sample, db channels, db signed_samples);
int iplay_audio_format_equals(const IplayAudioFormat *a, const IplayAudioFormat *b);
int iplay_audio_format_is_sb16_stereo_16(const IplayAudioFormat *format);
const char *iplay_audio_format_name(const IplayAudioFormat *format);
const char *iplay_audio_backend_name(IplayAudioBackend backend);
int iplay_audio_backend_is_sb16_scope(IplayAudioBackend backend);
int iplay_audio_backend_is_sb16_hardware(IplayAudioBackend backend);
int iplay_audio_backend_is_sdl_compatible(IplayAudioBackend backend);
IplayAudioBackend iplay_sdl_audio_spec_backend(const IplaySdlAudioSpec *spec);
const char *iplay_sdl_audio_spec_backend_name(const IplaySdlAudioSpec *spec);
const IplayAudioFormat *iplay_sdl_audio_spec_format(const IplaySdlAudioSpec *spec);
dw iplay_sdl_audio_spec_sample_rate(const IplaySdlAudioSpec *spec);
db iplay_sdl_audio_spec_bits_per_sample(const IplaySdlAudioSpec *spec);
db iplay_sdl_audio_spec_channels(const IplaySdlAudioSpec *spec);
db iplay_sdl_audio_spec_signed_samples(const IplaySdlAudioSpec *spec);
int iplay_sdl_audio_spec_hardware_enabled(const IplaySdlAudioSpec *spec);
int iplay_sdl_audio_spec_is_sb16_compatible(const IplaySdlAudioSpec *spec);
int iplay_sdl_audio_spec_is_sb16_hardware(const IplaySdlAudioSpec *spec);
int iplay_sdl_audio_spec_is_sdl_compatible(const IplaySdlAudioSpec *spec);
int iplay_audio_rates_match(const IplayAudioFormat *src_format, const IplayAudioFormat *dst_format);
const IplayAudioFormat *iplay_audio_source_format(db bits_per_sample, db channels, db signed_samples);
int iplay_audio_make_source_format(IplayAudioFormat *format, dw sample_rate, db bits_per_sample, db channels, db signed_samples);
void iplay_audio_sink_init(IplayAudioSink *sink, const IplayAudioFormat *format, IplayAudioWriteFn write, void *user);
void iplay_audio_sink_set_format(IplayAudioSink *sink, const IplayAudioFormat *format);
void iplay_audio_sink_set_write_callback(IplayAudioSink *sink, IplayAudioWriteFn write, void *user);
void iplay_audio_sink_start(IplayAudioSink *sink);
void iplay_audio_sink_stop(IplayAudioSink *sink);
void iplay_audio_sink_set_active(IplayAudioSink *sink, int active);
void iplay_audio_sink_reset_counters(IplayAudioSink *sink);
void iplay_audio_sink_set_capacity(IplayAudioSink *sink, dd capacity_frames);
void iplay_audio_sink_add_capacity(IplayAudioSink *sink, dd capacity_frames);
const IplayAudioFormat *iplay_audio_sink_format(const IplayAudioSink *sink);
dw iplay_audio_sink_bytes_per_frame(const IplayAudioSink *sink);
dd iplay_audio_sink_capacity(const IplayAudioSink *sink);
dd iplay_audio_sink_frames_written(const IplayAudioSink *sink);
dd iplay_audio_sink_underrun_frames(const IplayAudioSink *sink);
dd iplay_audio_sink_dropped_frames(const IplayAudioSink *sink);
int iplay_audio_sink_is_active(const IplayAudioSink *sink);
IplayAudioWriteFn iplay_audio_sink_write_callback(const IplayAudioSink *sink);
void *iplay_audio_sink_write_user(const IplayAudioSink *sink);
void iplay_audio_sink_set_frames_written(IplayAudioSink *sink, dd frames);
void iplay_audio_sink_set_underrun_frames(IplayAudioSink *sink, dd frames);
void iplay_audio_sink_set_dropped_frames(IplayAudioSink *sink, dd frames);
void iplay_audio_sink_clear_frames_written(IplayAudioSink *sink);
void iplay_audio_sink_clear_underrun_frames(IplayAudioSink *sink);
void iplay_audio_sink_clear_dropped_frames(IplayAudioSink *sink);
void iplay_audio_sink_add_frames_written(IplayAudioSink *sink, dd frames);
void iplay_audio_sink_add_underrun_frames(IplayAudioSink *sink, dd frames);
void iplay_audio_sink_add_dropped_frames(IplayAudioSink *sink, dd frames);
void iplay_audio_sink_consume_capacity(IplayAudioSink *sink, dd frames);
void iplay_audio_sink_write(IplayAudioSink *sink, const db *pcm, dw byte_count);
void iplay_audio_sink_write_silence(IplayAudioSink *sink, dw frame_count);
dw iplay_audio_u8_to_s16_stereo(const db *src, dw src_frames, db src_channels, db *dst, dw dst_bytes);
dw iplay_audio_s16_to_s16_stereo(const db *src, dw src_frames, db src_channels, db *dst, dw dst_bytes);
dw iplay_audio_convert_to_sink_format(const IplayAudioFormat *src_format, const db *src, dw src_frames, const IplayAudioFormat *dst_format, db *dst, dw dst_bytes);
dw iplay_audio_sink_write_converted(IplayAudioSink *sink, const IplayAudioFormat *src_format, const db *src, dw src_frames, db *scratch, dw scratch_bytes);
IplayAudioSink *iplay_audio_output_sink(IplayAudioOutput *output);
const IplayAudioSink *iplay_audio_output_sink_const(const IplayAudioOutput *output);
IplayAudioFormat *iplay_audio_output_source_format_mut(IplayAudioOutput *output);
void iplay_audio_output_set_source_format(IplayAudioOutput *output, const IplayAudioFormat *source_format);
void iplay_audio_output_set_scratch_buffer(IplayAudioOutput *output, db *scratch);
void iplay_audio_output_set_scratch_bytes(IplayAudioOutput *output, dw scratch_bytes);
void iplay_audio_output_set_scratch(IplayAudioOutput *output, db *scratch, dw scratch_bytes);
void iplay_audio_output_init(IplayAudioOutput *output, const IplayAudioFormat *source_format, IplayAudioWriteFn write, void *user, db *scratch, dw scratch_bytes);
void iplay_audio_output_init_sb16_stereo(IplayAudioOutput *output, IplayAudioWriteFn write, void *user);
void iplay_audio_output_start(IplayAudioOutput *output);
void iplay_audio_output_stop(IplayAudioOutput *output);
int iplay_audio_output_is_active(const IplayAudioOutput *output);
void iplay_audio_output_reset_counters(IplayAudioOutput *output);
void iplay_audio_output_set_capacity(IplayAudioOutput *output, dd capacity_frames);
void iplay_audio_output_add_capacity(IplayAudioOutput *output, dd capacity_frames);
dd iplay_audio_output_capacity(const IplayAudioOutput *output);
dw iplay_audio_output_accepted_frames(const IplayAudioOutput *output, dw frame_count);
dd iplay_audio_output_frames_written(const IplayAudioOutput *output);
dd iplay_audio_output_underrun_frames(const IplayAudioOutput *output);
dd iplay_audio_output_dropped_frames(const IplayAudioOutput *output);
const IplayAudioFormat *iplay_audio_output_source_format(const IplayAudioOutput *output);
const IplayAudioFormat *iplay_audio_output_sink_format(const IplayAudioOutput *output);
dw iplay_audio_output_bytes_per_frame(const IplayAudioOutput *output);
dw iplay_audio_output_frames_for_bytes(const IplayAudioOutput *output, dw byte_count);
dw iplay_audio_output_bytes_for_frames(const IplayAudioOutput *output, dw frame_count);
int iplay_audio_output_is_sb16_stereo(const IplayAudioOutput *output);
const IplayAudioLevels *iplay_audio_output_levels(const IplayAudioOutput *output);
IplayAudioLevels *iplay_audio_output_levels_mut(IplayAudioOutput *output);
db *iplay_audio_output_scratch(IplayAudioOutput *output);
dw iplay_audio_output_scratch_bytes(const IplayAudioOutput *output);
void iplay_audio_levels_set(IplayAudioLevels *levels, dw left_peak, dw right_peak);
void iplay_audio_levels_clear(IplayAudioLevels *levels);
db iplay_audio_levels_left_16(const IplayAudioLevels *levels);
db iplay_audio_levels_right_16(const IplayAudioLevels *levels);
void iplay_audio_output_reset_levels(IplayAudioOutput *output);
dw iplay_audio_output_write_mixer_frames(IplayAudioOutput *output, const db *src, dw src_frames);
dw iplay_audio_output_write_sb16_frames(IplayAudioOutput *output, const db *pcm, dw frame_count);
void iplay_audio_output_write_silence(IplayAudioOutput *output, dw frame_count);
db iplay_audio_level_to_16(dw peak);
void iplay_audio_sb16_stereo_levels(IplayAudioLevels *levels, const db *pcm, dw frame_count);
void iplay_audio_output_draw_levels_yx(IplayNcPlane *plane, dw y, dw x, const IplayAudioOutput *output, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr);
void iplay_sdl_audio_device_config_sb16_stereo(IplaySdlAudioDeviceConfig *config, void *userdata, IplayAudioBackend backend, db hardware_enabled);
void iplay_sdl_audio_device_config_set_format(IplaySdlAudioDeviceConfig *config, const IplayAudioFormat *format);
void iplay_sdl_audio_device_config_set_samples(IplaySdlAudioDeviceConfig *config, dw samples);
void iplay_sdl_audio_device_config_set_callback(IplaySdlAudioDeviceConfig *config, IplaySdlAudioCallback callback, void *userdata);
void iplay_sdl_audio_device_config_set_backend(IplaySdlAudioDeviceConfig *config, IplayAudioBackend backend, db hardware_enabled);
int iplay_sdl_audio_device_config_format(const IplaySdlAudioDeviceConfig *config, IplayAudioFormat *format);
dw iplay_sdl_audio_device_config_frequency(const IplaySdlAudioDeviceConfig *config);
db iplay_sdl_audio_device_config_bits_per_sample(const IplaySdlAudioDeviceConfig *config);
db iplay_sdl_audio_device_config_channels(const IplaySdlAudioDeviceConfig *config);
db iplay_sdl_audio_device_config_signed_samples(const IplaySdlAudioDeviceConfig *config);
dw iplay_sdl_audio_device_config_samples(const IplaySdlAudioDeviceConfig *config);
IplaySdlAudioCallback iplay_sdl_audio_device_config_callback(const IplaySdlAudioDeviceConfig *config);
void *iplay_sdl_audio_device_config_userdata(const IplaySdlAudioDeviceConfig *config);
IplayAudioBackend iplay_sdl_audio_device_config_backend(const IplaySdlAudioDeviceConfig *config);
int iplay_sdl_audio_device_config_hardware_enabled(const IplaySdlAudioDeviceConfig *config);
int iplay_sdl_audio_device_config_is_sb16_stereo(const IplaySdlAudioDeviceConfig *config);
int iplay_sdl_audio_device_open(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config, IplayAudioWriteFn write, void *write_user);
IplaySdlAudioDeviceConfig *iplay_sdl_audio_device_config_mut(IplaySdlAudioDevice *device);
const IplaySdlAudioDeviceConfig *iplay_sdl_audio_device_config(const IplaySdlAudioDevice *device);
void iplay_sdl_audio_device_set_config(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config);
void iplay_sdl_audio_device_apply_config(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config);
void iplay_sdl_audio_device_finish_open(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config);
void iplay_sdl_audio_device_init_sb16_compatible(IplaySdlAudioDevice *device, IplayAudioWriteFn write, void *user);
void iplay_sdl_audio_device_init_sb16_hardware(IplaySdlAudioDevice *device, IplayAudioWriteFn write, void *user);
IplaySdlAudioSpec iplay_sdl_audio_device_spec(const IplaySdlAudioDevice *device);
IplayAudioBackend iplay_sdl_audio_device_backend_raw(const IplaySdlAudioDevice *state);
void iplay_sdl_audio_device_set_backend_raw(IplaySdlAudioDevice *state, IplayAudioBackend backend);
IplayAudioBackend iplay_sdl_audio_device_backend(const IplaySdlAudioDevice *device);
void iplay_sdl_audio_device_set_backend(IplaySdlAudioDevice *device, IplayAudioBackend backend);
const char *iplay_sdl_audio_device_backend_name(const IplaySdlAudioDevice *device);
IplayAudioOutput *iplay_sdl_audio_device_output(IplaySdlAudioDevice *device);
const IplayAudioOutput *iplay_sdl_audio_device_output_const(const IplaySdlAudioDevice *device);
const IplayAudioFormat *iplay_sdl_audio_device_format(const IplaySdlAudioDevice *device);
dw iplay_sdl_audio_device_sample_rate(const IplaySdlAudioDevice *device);
db iplay_sdl_audio_device_bits_per_sample(const IplaySdlAudioDevice *device);
db iplay_sdl_audio_device_channels(const IplaySdlAudioDevice *device);
db iplay_sdl_audio_device_signed_samples(const IplaySdlAudioDevice *device);
dw iplay_sdl_audio_device_bytes_per_frame(const IplaySdlAudioDevice *device);
dw iplay_sdl_audio_device_samples(const IplaySdlAudioDevice *device);
IplaySdlAudioCallback iplay_sdl_audio_device_audio_callback(const IplaySdlAudioDevice *device);
void *iplay_sdl_audio_device_audio_userdata(const IplaySdlAudioDevice *device);
int iplay_sdl_audio_device_is_sb16_compatible(const IplaySdlAudioDevice *device);
int iplay_sdl_audio_device_is_sb16_hardware(const IplaySdlAudioDevice *device);
int iplay_sdl_audio_device_is_sdl_compatible(const IplaySdlAudioDevice *device);
db iplay_sdl_audio_device_hardware_enabled_flag(const IplaySdlAudioDevice *state);
void iplay_sdl_audio_device_set_hardware_enabled_flag(IplaySdlAudioDevice *state, db enabled);
int iplay_sdl_audio_device_hardware_enabled(const IplaySdlAudioDevice *device);
void iplay_sdl_audio_device_set_hardware_enabled(IplaySdlAudioDevice *device, int enabled);
const char *iplay_sdl_audio_device_status_text(const IplaySdlAudioDevice *device);
void iplay_sdl_audio_device_start(IplaySdlAudioDevice *device);
void iplay_sdl_audio_device_stop(IplaySdlAudioDevice *device);
int iplay_sdl_audio_device_active(const IplaySdlAudioDevice *device);
void iplay_sdl_audio_device_pause(IplaySdlAudioDevice *device, db paused);
db iplay_sdl_audio_device_paused_flag(const IplaySdlAudioDevice *state);
void iplay_sdl_audio_device_set_paused_flag(IplaySdlAudioDevice *state, db paused);
int iplay_sdl_audio_device_paused(const IplaySdlAudioDevice *device);
void iplay_sdl_audio_device_set_paused(IplaySdlAudioDevice *device, int paused);
void iplay_sdl_audio_device_reset_counters(IplaySdlAudioDevice *device);
void iplay_sdl_audio_device_set_capacity(IplaySdlAudioDevice *device, dd capacity_frames);
void iplay_sdl_audio_device_add_capacity(IplaySdlAudioDevice *device, dd capacity_frames);
void iplay_sdl_audio_device_clear_queued(IplaySdlAudioDevice *device);
dd iplay_sdl_audio_device_capacity(const IplaySdlAudioDevice *device);
dd iplay_sdl_audio_device_frames_written(const IplaySdlAudioDevice *device);
dd iplay_sdl_audio_device_underrun_frames(const IplaySdlAudioDevice *device);
dd iplay_sdl_audio_device_dropped_frames(const IplaySdlAudioDevice *device);
dd iplay_sdl_audio_device_queued_frames(const IplaySdlAudioDevice *device);
dd iplay_sdl_audio_device_queued_bytes(const IplaySdlAudioDevice *device);
dw iplay_sdl_audio_device_write_sb16_frames(IplaySdlAudioDevice *device, const db *pcm, dw frame_count);
int iplay_sdl_audio_device_can_queue(const IplaySdlAudioDevice *device);
dw iplay_sdl_audio_device_frames_for_bytes(const IplaySdlAudioDevice *device, dw byte_count);
dw iplay_sdl_audio_device_bytes_for_frames(const IplaySdlAudioDevice *device, dw frame_count);
dw iplay_sdl_audio_device_callback(void *user, db *stream, dw byte_count);
dw iplay_sdl_audio_device_queue(IplaySdlAudioDevice *device, const db *stream, dw byte_count);
dw iplay_sdl_audio_device_queue_frames(IplaySdlAudioDevice *device, const db *stream, dw frame_count);
void iplay_sdl_audio_device_write_silence(IplaySdlAudioDevice *device, dw frame_count);
const IplayAudioLevels *iplay_sdl_audio_device_levels(const IplaySdlAudioDevice *device);
void iplay_sdl_audio_device_reset_levels(IplaySdlAudioDevice *device);
void iplay_runtime_init_vga_sb16(IplayRuntime *runtime, db *cells, const IplayTextMode *mode, IplayAudioWriteFn write, void *user);
void iplay_runtime_init_vga_sdl_audio(IplayRuntime *runtime, db *cells, const IplayTextMode *mode, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_init_vga_sb16_present(IplayRuntime *runtime, db *cells, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn write, void *audio_user);
void iplay_runtime_init_callbacks(IplayRuntime *runtime, db *cells, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_init_callbacks_capacity(IplayRuntime *runtime, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_init_config(IplayRuntime *runtime, const IplayRuntimeConfig *config);
const IplayTextMode *iplay_runtime_start_config(IplayRuntime *runtime, const IplayRuntimeConfig *config, db video_mode);
int iplay_runtime_start_config_checked(IplayRuntime *runtime, const IplayRuntimeConfig *config, db video_mode);
void iplay_runtime_output_spec_init(IplayRuntimeOutputSpec *spec, IplayTerminalBackend video_backend, IplayAudioBackend audio_backend, db audio_hardware_enabled);
IplayTerminalBackend iplay_runtime_output_spec_video_backend(const IplayRuntimeOutputSpec *spec);
IplayAudioBackend iplay_runtime_output_spec_audio_backend(const IplayRuntimeOutputSpec *spec);
int iplay_runtime_output_spec_audio_hardware_enabled(const IplayRuntimeOutputSpec *spec);
void iplay_runtime_output_spec_sdl(IplayRuntimeOutputSpec *spec);
void iplay_runtime_output_spec_sb16_hardware(IplayRuntimeOutputSpec *spec);
void iplay_runtime_config_output_capacity(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user, const IplayRuntimeOutputSpec *output);
void iplay_runtime_config_sb16_hardware(IplayRuntimeConfig *config, db *cells, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_config_sb16_hardware_capacity(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_config_no_hardware(IplayRuntimeConfig *config, db *cells, const IplayTextMode *mode, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_config_no_hardware_capacity(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_config_sdl(IplayRuntimeConfig *config, db *cells, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_config_sdl_capacity(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayVideoPresentFn present, void *present_user, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_config_set_video_memory(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode);
void iplay_runtime_config_set_video_present(IplayRuntimeConfig *config, IplayVideoPresentFn present, void *present_user);
void iplay_runtime_config_set_video_backend(IplayRuntimeConfig *config, IplayTerminalBackend backend);
void iplay_runtime_config_set_video(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayTerminalBackend backend, IplayVideoPresentFn present, void *present_user);
void iplay_runtime_config_set_audio_sink(IplayRuntimeConfig *config, IplayAudioWriteFn audio_write, void *audio_user);
void iplay_runtime_config_set_audio_backend(IplayRuntimeConfig *config, IplayAudioBackend backend, db hardware_enabled);
void iplay_runtime_config_set_audio(IplayRuntimeConfig *config, IplayAudioWriteFn audio_write, void *audio_user, IplayAudioBackend backend, db hardware_enabled);
int iplay_runtime_config_has_video_present(const IplayRuntimeConfig *config);
int iplay_runtime_config_has_audio_sink(const IplayRuntimeConfig *config);
int iplay_runtime_config_has_cell_capacity(const IplayRuntimeConfig *config);
db *iplay_runtime_config_cells(const IplayRuntimeConfig *config);
dw iplay_runtime_config_cell_capacity(const IplayRuntimeConfig *config);
const IplayTextMode *iplay_runtime_config_mode(const IplayRuntimeConfig *config);
IplayTerminalBackend iplay_runtime_config_video_backend(const IplayRuntimeConfig *config);
IplayVideoPresentFn iplay_runtime_config_present(const IplayRuntimeConfig *config);
void *iplay_runtime_config_present_user(const IplayRuntimeConfig *config);
int iplay_runtime_config_video_present_enabled(const IplayRuntimeConfig *config);
IplayAudioWriteFn iplay_runtime_config_audio_write(const IplayRuntimeConfig *config);
void *iplay_runtime_config_audio_user(const IplayRuntimeConfig *config);
IplayAudioBackend iplay_runtime_config_audio_backend(const IplayRuntimeConfig *config);
int iplay_runtime_config_audio_hardware_enabled(const IplayRuntimeConfig *config);
int iplay_runtime_config_uses_sb16_hardware(const IplayRuntimeConfig *config);
db iplay_runtime_config_error(const IplayRuntimeConfig *config);
const char *iplay_runtime_config_error_name(db error);
int iplay_runtime_config_is_valid(const IplayRuntimeConfig *config);
void iplay_runtime_shutdown(IplayRuntime *runtime);
IplayNotcurses *iplay_runtime_notcurses(IplayRuntime *runtime);
const IplayNotcurses *iplay_runtime_notcurses_const(const IplayRuntime *runtime);
IplayTerminal *iplay_runtime_terminal(IplayRuntime *runtime);
const IplayTerminal *iplay_runtime_terminal_const(const IplayRuntime *runtime);
IplaySdlAudioDevice *iplay_runtime_audio(IplayRuntime *runtime);
const IplaySdlAudioDevice *iplay_runtime_audio_const(const IplayRuntime *runtime);
IplayNcPlane *iplay_runtime_stdplane(IplayRuntime *runtime);
IplayVideoSpec iplay_runtime_video_spec(const IplayRuntime *runtime);
IplayTerminalBackend iplay_runtime_video_backend(const IplayRuntime *runtime);
int iplay_runtime_video_present_enabled(const IplayRuntime *runtime);
int iplay_runtime_video_has_present(const IplayRuntime *runtime);
IplayVideoPresentFn iplay_runtime_video_present_callback(const IplayRuntime *runtime);
void *iplay_runtime_video_present_user(const IplayRuntime *runtime);
void iplay_runtime_video_set_present_fn(IplayRuntime *runtime, IplayVideoPresentFn present);
void iplay_runtime_video_set_present_user(IplayRuntime *runtime, void *user);
void iplay_runtime_video_set_present_callback(IplayRuntime *runtime, IplayVideoPresentFn present, void *user);
void iplay_runtime_video_clear_present_callback(IplayRuntime *runtime);
const IplayTextMode *iplay_runtime_video_mode(const IplayRuntime *runtime);
const db *iplay_runtime_video_cells_const(const IplayRuntime *runtime);
dd iplay_runtime_video_checksum(const IplayRuntime *runtime);
dw iplay_runtime_video_nonblank_cells(const IplayRuntime *runtime);
dw iplay_runtime_video_capacity(const IplayRuntime *runtime);
dw iplay_runtime_video_cols(const IplayRuntime *runtime);
dw iplay_runtime_video_rows(const IplayRuntime *runtime);
dw iplay_runtime_video_row_bytes(const IplayRuntime *runtime);
dw iplay_runtime_video_screen_bytes(const IplayRuntime *runtime);
int iplay_runtime_bottom_layout_fits(const IplayRuntime *runtime);
IplaySdlAudioSpec iplay_runtime_audio_spec(const IplayRuntime *runtime);
IplayAudioBackend iplay_runtime_audio_backend(const IplayRuntime *runtime);
const IplayAudioFormat *iplay_runtime_audio_format(const IplayRuntime *runtime);
dw iplay_runtime_audio_sample_rate(const IplayRuntime *runtime);
db iplay_runtime_audio_bits_per_sample(const IplayRuntime *runtime);
db iplay_runtime_audio_channels(const IplayRuntime *runtime);
db iplay_runtime_audio_signed_samples(const IplayRuntime *runtime);
dw iplay_runtime_audio_samples(const IplayRuntime *runtime);
const char *iplay_runtime_audio_backend_name(const IplayRuntime *runtime);
int iplay_runtime_audio_hardware_enabled(const IplayRuntime *runtime);
const char *iplay_runtime_audio_status_text(const IplayRuntime *runtime);
dw iplay_runtime_audio_bytes_per_frame(const IplayRuntime *runtime);
int iplay_runtime_audio_is_sb16_compatible(const IplayRuntime *runtime);
int iplay_runtime_audio_is_sb16_hardware(const IplayRuntime *runtime);
int iplay_runtime_audio_is_sdl_compatible(const IplayRuntime *runtime);
const IplayTextMode *iplay_runtime_resize(IplayRuntime *runtime, const IplayTextMode *mode);
int iplay_runtime_resize_checked(IplayRuntime *runtime, const IplayTextMode *mode);
const IplayTextMode *iplay_runtime_resize_to_size(IplayRuntime *runtime, dw cols, dw rows);
int iplay_runtime_resize_to_size_checked(IplayRuntime *runtime, dw cols, dw rows);
const IplayTextMode *iplay_runtime_set_video_mode(IplayRuntime *runtime, db video_mode);
int iplay_runtime_set_video_mode_checked(IplayRuntime *runtime, db video_mode);
void iplay_runtime_set_video_mode_ok_flag(IplayRuntime *runtime, db ok);
void iplay_runtime_set_video_mode_ok(IplayRuntime *runtime, int ok);
db iplay_runtime_video_mode_ok_flag(const IplayRuntime *runtime);
int iplay_runtime_video_mode_ok(const IplayRuntime *runtime);
const char *iplay_runtime_video_status_text(const IplayRuntime *runtime);
const char *iplay_runtime_video_status_token(const IplayRuntime *runtime);
void iplay_runtime_render_static(IplayRuntime *runtime, db erase_attr);
void iplay_runtime_render_bottom(IplayRuntime *runtime, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif);
void iplay_runtime_audio_start(IplayRuntime *runtime);
void iplay_runtime_audio_stop(IplayRuntime *runtime);
int iplay_runtime_audio_active(const IplayRuntime *runtime);
void iplay_runtime_audio_pause(IplayRuntime *runtime, db paused);
int iplay_runtime_audio_paused(const IplayRuntime *runtime);
void iplay_runtime_audio_reset_counters(IplayRuntime *runtime);
void iplay_runtime_audio_set_capacity(IplayRuntime *runtime, dd capacity_frames);
void iplay_runtime_audio_add_capacity(IplayRuntime *runtime, dd capacity_frames);
void iplay_runtime_audio_clear_queued(IplayRuntime *runtime);
dd iplay_runtime_audio_capacity(const IplayRuntime *runtime);
dd iplay_runtime_audio_frames_written(const IplayRuntime *runtime);
dd iplay_runtime_audio_underrun_frames(const IplayRuntime *runtime);
dd iplay_runtime_audio_dropped_frames(const IplayRuntime *runtime);
dd iplay_runtime_audio_queued_frames(const IplayRuntime *runtime);
dd iplay_runtime_audio_queued_bytes(const IplayRuntime *runtime);
int iplay_runtime_audio_can_queue(const IplayRuntime *runtime);
dw iplay_runtime_audio_frames_for_bytes(const IplayRuntime *runtime, dw byte_count);
dw iplay_runtime_audio_bytes_for_frames(const IplayRuntime *runtime, dw frame_count);
dw iplay_runtime_audio_queue(IplayRuntime *runtime, const db *pcm, dw byte_count);
dw iplay_runtime_audio_queue_frames(IplayRuntime *runtime, const db *pcm, dw frame_count);
dw iplay_runtime_write_sb16_frames(IplayRuntime *runtime, const db *pcm, dw frame_count);
void iplay_runtime_write_silence(IplayRuntime *runtime, dw frame_count);
const IplayAudioLevels *iplay_runtime_audio_levels(const IplayRuntime *runtime);
void iplay_runtime_audio_reset_levels(IplayRuntime *runtime);
void iplay_runtime_draw_audio_levels(IplayRuntime *runtime, dw y, dw x, dw width);
void iplay_runtime_draw_live_audio_levels(IplayRuntime *runtime);
void iplay_runtime_draw_original_channel_levels(IplayRuntime *runtime, dw channel_count);
void iplay_runtime_draw_original_channel_level(IplayRuntime *runtime, dw channel, dw level);
void iplay_runtime_set_audio_levels(IplayRuntime *runtime, dw left_peak, dw right_peak);
void iplay_runtime_draw_original_channel_text(IplayRuntime *runtime, dw channel, const char *note, const char *sample, const char *effect);
void iplay_runtime_draw_original_volume_text(IplayRuntime *runtime, const char *text);
void iplay_runtime_draw_audio_status(IplayRuntime *runtime);
dw iplay_runtime_refresh_audio_status(IplayRuntime *runtime);
void iplay_runtime_draw_video_status(IplayRuntime *runtime);
void iplay_module_status_init(IplayModuleStatus *status, const char *title, const char *module_path, dd module_size, const char *loader_symbol, dd module_type);
void iplay_module_status_set_title(IplayModuleStatus *status, const char *title);
void iplay_module_status_set_path(IplayModuleStatus *status, const char *module_path);
void iplay_module_status_set_size(IplayModuleStatus *status, dd module_size);
void iplay_module_status_set_loader(IplayModuleStatus *status, const char *loader_symbol);
const char *iplay_module_status_title(const IplayModuleStatus *status);
const char *iplay_module_status_path(const IplayModuleStatus *status);
dd iplay_module_status_size(const IplayModuleStatus *status);
const char *iplay_module_status_loader(const IplayModuleStatus *status);
dd iplay_module_status_type(const IplayModuleStatus *status);
void iplay_module_status_set_type(IplayModuleStatus *status, dd module_type);
void iplay_module_status_clear_type(IplayModuleStatus *status);
void iplay_module_status_type_hex(const IplayModuleStatus *status, char *dst);
void iplay_runtime_draw_module_status_struct(IplayRuntime *runtime, const IplayModuleStatus *status);
void iplay_runtime_draw_module_tag_struct(IplayRuntime *runtime, const IplayModuleStatus *status);
void iplay_runtime_draw_status_block(IplayRuntime *runtime, const IplayModuleStatus *status);
void iplay_runtime_draw_module_status(IplayRuntime *runtime, const char *title, const char *module_path, dd module_size, const char *loader_symbol);
void iplay_runtime_draw_module_tag(IplayRuntime *runtime, dd module_type);
void iplay_runtime_draw_original_filename_status(IplayRuntime *runtime, const char *text, db attr);
void iplay_runtime_draw_original_module_info(IplayRuntime *runtime, const char *filename_line, const char *module_type_line);
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
    const char *position_line);
void iplay_runtime_draw_status_line(IplayRuntime *runtime, dw y, const char *text, db attr);
void iplay_runtime_draw_status_field(IplayRuntime *runtime, dw y, const char *label, const char *value, db label_attr, db value_attr);
void iplay_runtime_draw_status_u32(IplayRuntime *runtime, dw y, const char *label, dd value, db label_attr, db value_attr);
void iplay_runtime_draw_status_hex32(IplayRuntime *runtime, dw y, const char *label, dd value, db label_attr, db value_attr);
void iplay_window_draw_status_line(IplayWindow *window, dw y, const char *text, db attr);
void iplay_window_draw_status_field(IplayWindow *window, dw y, const char *label, const char *value, db label_attr, db value_attr);
void iplay_window_draw_status_u32(IplayWindow *window, dw y, const char *label, dd value, db label_attr, db value_attr);
void iplay_window_draw_status_hex32(IplayWindow *window, dw y, const char *label, dd value, db label_attr, db value_attr);
dw iplay_runtime_present(IplayRuntime *runtime);
db iplay_text_attr(IplayTextColor fg, IplayTextColor bg, int blink);
IplayTextColor iplay_text_attr_fg(db attr);
IplayTextColor iplay_text_attr_bg(db attr);
int iplay_text_attr_blink(db attr);
db *iplay_ncplane_cells_at(db *cells, dw stride_cols, dw origin_y, dw origin_x);
void iplay_ncplane_init_at(IplayNcPlane *plane, db *cells, dw rows, dw cols, dw origin_y, dw origin_x, dw stride_cols);
void iplay_ncplane_subplane(IplayNcPlane *child, const IplayNcPlane *parent, dw y, dw x, dw rows, dw cols);
void iplay_ncplane_resize(IplayNcPlane *plane, dw rows, dw cols);
void iplay_ncplane_origin_yx(const IplayNcPlane *plane, dw *y, dw *x);
dw iplay_ncplane_rows(const IplayNcPlane *plane);
dw iplay_ncplane_cols(const IplayNcPlane *plane);
dw iplay_ncplane_cell_offset(const IplayNcPlane *plane, dw y, dw x);
db iplay_ncplane_cell_ch(const IplayNcPlane *plane, dw offset);
db iplay_ncplane_cell_attr(const IplayNcPlane *plane, dw offset);
void iplay_ncplane_put_cell_offset(IplayNcPlane *plane, dw offset, db ch, db attr);
void iplay_ncplane_copy_cell_offset(IplayNcPlane *plane, dw dst_offset, dw src_offset);
void iplay_ncplane_cursor_yx(const IplayNcPlane *plane, dw *y, dw *x);
void iplay_ncplane_cursor_move_yx(IplayNcPlane *plane, dw y, dw x);
void iplay_ncplane_putc_yx(IplayNcPlane *plane, dw y, dw x, db ch, db attr);
void iplay_ncplane_putc(IplayNcPlane *plane, db ch, db attr);
void iplay_ncplane_hline_yx(IplayNcPlane *plane, dw y, dw x, db ch, db attr, dw count);
void iplay_ncplane_vline_yx(IplayNcPlane *plane, dw y, dw x, db ch, db attr, dw count);
void iplay_ncplane_meter16_yx(IplayNcPlane *plane, dw y, dw x, db level, dw width, db fill_ch, db empty_ch, db fill_attr, db empty_attr);
void iplay_audio_levels_draw_yx(IplayNcPlane *plane, dw y, dw x, const IplayAudioLevels *levels, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr);
void iplay_ncplane_fill_yx(IplayNcPlane *plane, dw y, dw x, dw rows, dw cols, db ch, db attr);
void iplay_ncplane_erase(IplayNcPlane *plane, db attr);
void iplay_ncplane_box_yx(IplayNcPlane *plane, dw y, dw x, dw rows, dw cols, db attr, db fill_attr);
void iplay_window_init_root(IplayWindow *window, IplayNcPlane *root);
void iplay_window_init_subwindow(IplayWindow *window, const IplayWindow *parent, dw y, dw x, dw rows, dw cols);
IplayNcPlane *iplay_window_plane(IplayWindow *window);
const IplayNcPlane *iplay_window_plane_const(const IplayWindow *window);
void iplay_window_resize(IplayWindow *window, dw rows, dw cols);
void iplay_window_origin_yx(const IplayWindow *window, dw *y, dw *x);
dw iplay_window_rows(const IplayWindow *window);
dw iplay_window_cols(const IplayWindow *window);
void iplay_window_erase(IplayWindow *window, db attr);
void iplay_window_fill_yx(IplayWindow *window, dw y, dw x, dw rows, dw cols, db ch, db attr);
void iplay_window_box_yx(IplayWindow *window, dw y, dw x, dw rows, dw cols, db attr, db fill_attr);
void iplay_window_cursor_yx(const IplayWindow *window, dw *y, dw *x);
void iplay_window_cursor_move_yx(IplayWindow *window, dw y, dw x);
void iplay_window_putc(IplayWindow *window, db ch, db attr);
void iplay_window_putstr(IplayWindow *window, const char *text, db attr);
void iplay_window_putnstr(IplayWindow *window, const char *text, db attr, dw width);
void iplay_window_putnstr_fill_yx(IplayWindow *window, dw y, dw x, const char *text, db attr, dw width);
void iplay_window_scroll_up(IplayWindow *window, dw top, dw left, dw rows, dw cols, dw count, db fill_attr);
void iplay_window_scroll_down(IplayWindow *window, dw top, dw left, dw rows, dw cols, dw count, db fill_attr);
void iplay_window_draw_audio_levels(IplayWindow *window, dw y, dw x, const IplayAudioLevels *levels, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr);
void iplay_ncplane_scroll_up(IplayNcPlane *plane, dw top, dw left, dw rows, dw cols, dw count, db fill_attr);
void iplay_ncplane_scroll_down(IplayNcPlane *plane, dw top, dw left, dw rows, dw cols, dw count, db fill_attr);
void iplay_ncplane_putstr_yx(IplayNcPlane *plane, dw y, dw x, const char *text, db attr);
void iplay_ncplane_putstr(IplayNcPlane *plane, const char *text, db attr);
void iplay_ncplane_putnstr_yx(IplayNcPlane *plane, dw y, dw x, const char *text, db attr, dw width);
void iplay_ncplane_putnstr(IplayNcPlane *plane, const char *text, db attr, dw width);
void iplay_ncplane_putnstr_fill_yx(IplayNcPlane *plane, dw y, dw x, const char *text, db attr, dw width);
void iplay_ncplane_putnstr_fill(IplayNcPlane *plane, const char *text, db attr, dw width);

db iplay_hex4_to_buffer(db *mem, dw *offset, db value);
db iplay_hex8_to_buffer(db *mem, dw *offset, db value);
db iplay_hex16_to_buffer(db *mem, dw *offset, dw value);
db iplay_hex32_to_buffer(db *mem, dw *offset, dd value);
IplayDecimalResult iplay_u8_decimal_to_buffer(db *mem, dw *offset, db value);
IplayDecimalResult iplay_u16_decimal_to_buffer(db *mem, dw *offset, dw value);
IplayDecimalResult iplay_u32_decimal_to_buffer(db *mem, dw *offset, dd value);
IplayDecimalResult iplay_u32_base_to_buffer(db *mem, dw *offset, dd value, unsigned base, dw initial_count);
IplayDecimalResult iplay_i8_decimal_to_buffer(db *mem, dw *offset, db value);
IplayDecimalResult iplay_i16_decimal_to_buffer(db *mem, dw *offset, dw value);
IplayDecimalResult iplay_i32_decimal_to_buffer(db *mem, dw *offset, dd value);
dw iplay_put_counted_char_to_buffer(db *mem, dw *offset, dw count, db value);
dw iplay_u32_decimal_fill_to_buffer(db *mem, dw *offset, dd value, dw count, int with_pointer_prefix);
dw iplay_string_length_at(const db *mem, dw offset);
IplayStringCopyResult iplay_strcpy_count_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset);
IplayStringCopyResult iplay_copy_printable_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, dw count);
IplayStringCopyResult iplay_copy_printable_padded_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, dw count);
IplayAttributedTextResult iplay_copy_attributed_fixed_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, dw count, db attr);
IplayAttributedTextResult iplay_put_attributed_message_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, db attr, int initial, db initial_ch);
IplayAttributedTextResult iplay_put_controlled_attributed_text_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw dst_offset, db attr);
IplayAttributedTextResult iplay_message_1be77_to_buffer(db *mem, dw video_base, dw src_offset, db y, db attr);
IplayAsmSprintfResult iplay_myasmsprintf_to_buffer(db *mem, dw src_offset, dw dst_offset, dd eax, dd ecx, dd edx);
IplayScreenStreamResult iplay_write_screen_stream_to_buffer(const db *src_mem, db *dst_mem, dw src_offset, dw base_offset);
IplayRecolorResult iplay_recolor_text_row(db *mem, const IplayTextMode *mode, dw row, db color);
void iplay_u4tox(IplayRegs *r, db *mem);
void iplay_u8tox(IplayRegs *r, db *mem);
void iplay_u16tox(IplayRegs *r, db *mem);
void iplay_u32tox(IplayRegs *r, db *mem);
void iplay_hex_1be39(IplayRegs *r, db *dst);
void iplay_my_putdigit(IplayRegs *r, db *mem);
void iplay_my_u32toa(IplayRegs *r, db *mem, unsigned base);
void iplay_my_u8toa_10(IplayRegs *r, db *mem);
void iplay_my_u16toa_10(IplayRegs *r, db *mem);
void iplay_my_u32toa10(IplayRegs *r, db *mem);
void iplay_my_i8toa10(IplayRegs *r, db *mem);
void iplay_my_i16toa10(IplayRegs *r, db *mem);
void iplay_my_i32toa10(IplayRegs *r, db *mem);
void iplay_my_u32toa_fill(IplayRegs *r, db *mem, dw count, int with_pointer_prefix);
void iplay_myasmsprintf(IplayRegs *r, db *mem);
void iplay_mystrlen(IplayRegs *r, const db *mem);
void iplay_interpolation_patch(db *code, db value);
void iplay_strcpy_count(IplayRegs *r, const db *src_mem, db *dst_mem);
void iplay_copy_printable(IplayRegs *r, const db *src_mem, db *dst_mem);
void iplay_seg1_copy_printable(IplayRegs *r, const db *src_mem, db *dst_mem);
void iplay_txt_1abae(IplayRegs *r, const db *src_mem, db *dst_mem);
void iplay_put_message(IplayRegs *r, const db *src_mem, db *dst_mem, int initial_ax);
void iplay_text_1bf69(IplayRegs *r, const db *src_mem, db *dst_mem);
void iplay_message_1be77(IplayRegs *r, db *mem, dw video_base);
void iplay_draw_frame_plane(IplayNcPlane *plane, db style, db attr, db fill_attr, db x, db y, db right, db bottom);
void iplay_draw_frame(db *mem, db style, db attr, db fill_attr, db x, db y, db right, db bottom);
void iplay_write_scr(IplayRegs *r, const db *src_mem, db *dst_mem);
void iplay_txt_draw_top_title_plane(IplayNcPlane *plane);
void iplay_txt_draw_top_title(db *mem);
void iplay_txt_draw_bottom_plane(IplayNcPlane *plane, const IplayBottomLayout *layout, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif);
void iplay_txt_draw_bottom(db *mem, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif);
void iplay_text_screen_draw_top_title(IplayTextScreen *screen);
void iplay_text_screen_draw_bottom(IplayTextScreen *screen, db byte_1de72, db byte_1de73, db byte_1de74, db byte_1de75, db byte_1de76, db flags, dw volume, dw amplif);
void iplay_text_screen_draw_audio_output_levels(IplayTextScreen *screen, dw y, dw x, const IplayAudioOutput *output, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr);
void iplay_filelist_row(db *row, db entry_type, db flags, dw time_word, dw date_word, dd size, const char *name);
void iplay_find_mods_no_nul_guard(IplayRegs *r, db *mem, dw dseg);
void iplay_recolor_txt(IplayRegs *r, db *mem);
int iplay_mouse_1c7a9(IplayRegs *r);
int iplay_mouse_1c7cf(IplayRegs *r, const db *mem);
void iplay_int24(IplayRegs *r);
void iplay_ems_restore_mapctx_guard(IplayRegs *r, db ems_enabled, db mapctx_saved);
void iplay_ems_init_config(IplayRegs *r, db *globals, dw config_word);
void iplay_ems_disabled_guard(IplayRegs *r, db ems_enabled);
void iplay_ems_local_mapcopy(db *mem, const char *symbol, dw base);
void iplay_ems_realloc2_fallback(IplayRegs *r, db *mem, dw di);
void iplay_clean_11c43(db *mem, db flag_playsettings, db byte_2461e, db byte_2461f);
void iplay_mod_sub_delta(IplayRegs *r, db *mem, db flag, db reset, db *previous);
void iplay_sub_11ba6(IplayRegs *r, db *mem, db *current_max);
dw iplay_mod_102f5(const db *orders);
void iplay_sub_126a9(IplayRegs *r, dw word_245fa, dw size1, dw channels, db realloc_count, dd module_type);
void iplay_ult_read_fast(IplayRegs *r, db *mem);
void iplay_sub_1265d(IplayRegs *r, dw volume, db sndcard, db byte_24666, db byte_24667, db sndflags, db byte_24628, db stereo, db byte_24671, dw word_245f6, dw word_245f0);
void iplay_memfree_125da_guard(IplayRegs *r);
void iplay_mod_1021e(db *out, db first, db second, const db *pattern, const db *title);
void iplay_mod_1024a(db *out, dw sample_count, const db *headers, dw freq);
void iplay_memfree_18a28_guard(IplayRegs *r, db memflag);
void iplay_sub_11c0c(IplayRegs *r, const db *mem);
void iplay_sub_1415e(IplayRegs *r, db *mem, dw index, dw total, db segment_index, db pending);
void iplay_sub_12f56(IplayRegs *r, db *mem, dw index, dw total, db segment_index, db pending, db bh);
void iplay_sub_135ca_zero_event(IplayRegs *r, db *mem);
void iplay_spectr_1b084_len2(IplayRegs *r, db *mem, dw buf);
void iplay_f5_draw_spectr_inactive(IplayRegs *r, db *mem);
IplayRegs6Result iplay_fill_dma_small_result(db *mem, const char *symbol, dw src, dw dst, dw count, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_fill_dma_small(IplayRegs *r, db *mem, const char *symbol, dw src, dw dst, dw count);
IplayRegs6Result iplay_fill_dma_inactive_mono_result(db *mem, dw dma_off);
void iplay_fill_dma_inactive_mono(IplayRegs *r, db *mem, dw dma_off);
void iplay_get_keybsw(IplayRegs *r, db *mem, dw value);
void iplay_set_keybsw(IplayRegs *r, db *mem, dw value);
void iplay_int9_keyb_no_scancode(IplayRegs *r, db *mem);
void iplay_sub_197f2_labels(IplayRegs *r, db *mem, dw configword);
void iplay_useless_11787_zero(IplayRegs *r, db *mem, dw channel);
void iplay_useless_11787_zero_public_layout(IplayRegs *r, db *mem, dw channel);
void iplay_timer_port_no_device(IplayRegs *r, db *mem, const char *symbol, dw ax_value);
void iplay_set_timer_no_device(IplayRegs *r, db *mem, dw ax_value);
void iplay_clean_timer_no_device(IplayRegs *r, dw ax_value);
void iplay_set_timer_int_alloc_fail(IplayRegs *r, db *mem);
void iplay_useless_doswrite2_header(IplayRegs *r, db *mem);
void iplay_useless_doswrite_header(IplayRegs *r, db *mem);
void iplay_ult_1150b(IplayRegs *r, dw value);
void iplay_ega_seq_no_device(IplayRegs *r, int set_mode);
void iplay_useless_unset_egaseq(IplayRegs *r, db mode_bits);
void iplay_useless_strange_short(IplayRegs *r, db *mem);
void iplay_useless_writeinr_118_header(IplayRegs *r, db *mem);
void iplay_useless_writeinr_fail(IplayRegs *r);
void iplay_useless_12d61_no_device(IplayRegs *r, db *mem);
void iplay_txt_blink_no_device(IplayRegs *r, int enable);
void iplay_useless_sprintf_chunk(IplayRegs *r, db *mem, const char *symbol, dd value);
void iplay_useless_mysprintf(IplayRegs *r, db *mem);
size_t iplay_useless_sprint_numeric(IplayRegs *r, const db *mem, db *dst, unsigned kind);
void iplay_snd_on_parnt_bounded(db *mem);
void iplay_memfree_invalid(IplayRegs *r);
void iplay_midi_port_no_device(IplayRegs *r, db *mem, const char *symbol);
void iplay_midi_port_public(IplayRegs *r, const char *symbol);
void iplay_midi_153f1_public(IplayRegs *r);
void iplay_midi_set_no_device(IplayRegs *r);
void iplay_midi_channel_event_no_device(IplayRegs *r, db *globals, db *channel, int note_off);
void iplay_int_vector_roundtrip(IplayRegs *r, db int_number, dw vector_off, dw vector_seg);
void iplay_snd_vector_roundtrip(IplayRegs *r, db *mem, db irq, dw old_off, dw old_seg);
IplaySb16ProbeResult iplay_sb16_probe_no_device_to_buffer(db *mem, const char *symbol);
void iplay_sb16_probe_no_device(IplayRegs *r, db *mem, const char *symbol);
void iplay_sb16_probe_public(IplayRegs *r, const char *symbol);
IplaySb16RegsResult iplay_sb16_init_fail_to_buffer(db *mem);
void iplay_sb16_init_fail(IplayRegs *r, db *mem);
IplaySb16RegsResult iplay_sb16_int_ack_to_buffer(db *mem);
void iplay_sb16_int_ack(IplayRegs *r, db *mem);
IplaySb16RegsResult iplay_sb16_dma_fail_to_buffer(db *mem);
void iplay_sb16_dma_fail(IplayRegs *r, db *mem);
void iplay_sb16_dma_public(IplayRegs *r);
IplaySb16RegsResult iplay_sb16_off_no_device_to_buffer(db *mem, const char *symbol);
void iplay_sb16_off_public(IplayRegs *r, const char *symbol);
void iplay_int1a_passthrough(IplayRegs *r);
void iplay_inr_read_119b7_eof(IplayRegs *r, db *mem);
void iplay_mod_readfile_11f4e_guard(IplayRegs *r, db *mem);
void iplay_mod_readfile_11f4e_public_layout(IplayRegs *r, db *mem);
void iplay_mod_readfile_12247_eof(IplayRegs *r, db *mem);
void iplay_stereo_timer_int_snapshot(IplayRegs *r, db *mem);
void iplay_timer_int_end_disabled(IplayRegs *r, db *mem);
void iplay_sb16_off_no_device(IplayRegs *r, db *mem, const char *symbol);
void iplay_clean_deinit_no_device(IplayRegs *r, db *mem, const char *symbol);
void iplay_clean_deinit_public(IplayRegs *r, const char *symbol);
void iplay_dos_dir_stub(IplayRegs *r, db *mem, int chdir_mode);
void iplay_dos_findnext_fail(IplayRegs *r, db *mem);
void iplay_dos_fread_eof(IplayRegs *r, db *mem);
void iplay_dos_seek_success(IplayRegs *r, db *mem);
void iplay_inr_read_118b0_fail(IplayRegs *r);
void iplay_read2buffer_empty(IplayRegs *r, db *mem);
void iplay_read2buffer_public_layout(IplayRegs *r, db *mem);
void iplay_mem_limit(IplayRegs *r, dd size);
void iplay_alloc_dma_fail(IplayRegs *r, db *mem, dd size, dw channel);
void iplay_gravis_dma_control(IplayRegs *r, db *mem, const char *symbol);
void iplay_sub_1279a_dma(IplayRegs *r, db *mem);
void iplay_program_dma_channel1(IplayRegs *r, db *mem);
void iplay_mem_strategy(IplayRegs *r, const char *symbol, dw config_word);
void iplay_mem_reallocx_bookkeeping(db *mem, dw size);
void iplay_deinit_125b9_idle(IplayRegs *r, db *mem);
void iplay_deinit_125b9_public_layout(IplayRegs *r, db *mem);
void iplay_rtc_clock_bcd_123456(IplayRegs *r, db *mem);
void iplay_loadcfg_success(IplayRegs *r, db *mem);
void iplay_dosexec_no_comspec(IplayRegs *r, db *mem);
void iplay_callsubx_fail(db *mem);
void iplay_memalloc12k_bounded(IplayRegs *r);
void iplay_init_vga_bounded(db *mem);
void iplay_init_vga_public_layout(db *mem);
void iplay_f2_draw_bounded(db *mem);
void iplay_f2_draw_public_layout(db *mem, dw data_seg);
void iplay_readallmoules_bounded(IplayRegs *r, db *mem);
void iplay_readmodule_fail(db *mem);
void iplay_moduleread_fail(db *mem);
void iplay_modread_10311_bounded(db *mem);
void iplay_modnt_bounded(db *mem);
void iplay_format_loader_header(db *mem, const char *symbol);
void iplay_modules_search_bounded(db *mem);
void iplay_readallmoules_public_layout(db *mem);
void iplay_readmodule_public_layout(db *mem);
void iplay_moduleread_public_layout(db *mem);
void iplay_modnt_public_layout(db *mem);
void iplay_modules_search_public_layout(db *mem);
void iplay_start_bounded(IplayRegs *r, db *mem);
void iplay_start_player_memory(db *mem);
void iplay_keyb_bounded(db *mem);
void iplay_noop(IplayRegs *r);
void iplay_setvideomode_no_hw(IplayRegs *r, db *globals);
int iplay_text_setup_small(IplayRegs *r, db *globals, const char *symbol);
int iplay_graph_setup_bounded(IplayRegs *r, db *globals, const char *symbol);
void iplay_sub_1ab8c(IplayRegs *r, const db *channel);
void iplay_spectr_1bce9_equal(IplayRegs *r, db *frame);
void iplay_spectr_1bc2d_equal(IplayRegs *r, db *frame);
void iplay_spectr_1bbc1_zero(IplayRegs *r, db *bins);
void iplay_video_prp_mtr_positn(db *globals, const db *channels, dw count);
void iplay_parse_cmdline(IplayRegs *r, db *mem);
void iplay_parse_cmdline_from_psp(IplayRegs *r, db *globals, const db *psp);
void iplay_get_comspec(IplayRegs *r, const db *env);
void iplay_getexename(IplayRegs *r, const db *env, db *dst);
void iplay_int2f_checkmyself(IplayRegs *r, db *globals);
void iplay_spectr_1b406_small(db *mem, dw di);
void iplay_spectr_1c4f8(IplayRegs *r);
dd iplay_get_playsettings_eax(dd eax, db flag_playsettings);
void iplay_get_playsettings(IplayRegs *r, db flag_playsettings);
IplaySb16RegsResult iplay_set_playsettings_result(db *globals, db *channels, dw channel_count, dw channel_stride, dd eax, dd ebx, dd ecx, dd edx);
void iplay_set_playsettings(IplayRegs *r, db *globals, db *channels, dw channel_count, dw channel_stride);
IplaySb16RegsResult iplay_volume_12a66_result(dw channel_count, dd eax, dd ebx, dd ecx, dd edx);
void iplay_volume_12a66(IplayRegs *r, dw channel_count);
IplaySb16RegsResult iplay_vlm_141df_result(db *globals, dw channel_count, dd eax, dd ebx, dd ecx, dd edx);
void iplay_vlm_141df(IplayRegs *r, db *globals, dw channel_count);
IplaySb16RegsResult iplay_change_volume_result(db *globals, db *channels, dw channel_count, dd eax, dd ebx, dd ecx, dd edx);
void iplay_change_volume(IplayRegs *r, db *globals, db *channels, dw channel_count);
dd iplay_getset_playstate_eax(dd eax, db play_state);
db iplay_getset_playstate(IplayRegs *r, db play_state);
IplaySb16RegsResult iplay_get_12f7c_result(dw word_245f0, dw word_245f6, dd eax, dd ebx, dd ecx, dd edx);
void iplay_get_12f7c(IplayRegs *r, dw word_245f0, dw word_245f6);
IplayRegs6Result iplay_memclean_result(db *mem, dw di, dw size, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_memclean(IplayRegs *r, db *mem, dw size);
void iplay_sub_12afd(IplayRegs *r, db *channels, dw channel_count, db channel_index, db sndflags);
void iplay_sub_12b18(db *globals, db *channels, const db *src, dw channel_count, db sndflags);
void iplay_sub_12b83_state(db *globals, db *channels, dw channel_stride, const db *types, db requested_count);
void iplay_sub_12b83(IplayRegs *r, db *globals, db *channels, dw channel_stride, const db *types);
IplayRegs6Result iplay_sub_12cad_guard_result(db *event_store, dw channel_count, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_sub_12cad_guard(IplayRegs *r, db *event_store, dw channel_count);
IplayRegs6Result iplay_sub_12d05_to_buffer(db *dst, db snd_init, db sndcard_type, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_sub_12d05(IplayRegs *r, db *dst, db snd_init, db sndcard_type);
db iplay_sub_12d35_disable_code(dd eax);
void iplay_sub_12d35_disable(IplayRegs *r, db *code_byte);
void iplay_sub_12da8_guard_state(db *globals, dd eax, dd ebx, dd ecx, dd edx, dd esi);
void iplay_sub_12da8_guard(IplayRegs *r, db *globals);
IplayRegs6Result iplay_sub_1281a_small_result(db *dst, const db *samples, const db *vlm_table, const db *channel, dw word_24610, dw size, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_sub_1281a_small(IplayRegs *r, db *dst, const db *samples, const db *vlm_table, const db *channel, dw word_24610, dw size);
void iplay_sub_13017_bounded(db *globals, db *samples, dw sample_count);
IplayRegs6Result iplay_configure_timer_bounded_result(db *globals, db *samples, dw sample_count, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_configure_timer_bounded(IplayRegs *r, db *globals, db *samples, dw sample_count);
IplayRegs6Result iplay_sub_13623_guard_result(dw channel_count, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_sub_13623_guard(IplayRegs *r, dw channel_count);
void iplay_sub_13044(db *globals, db *vlm_table);
void iplay_someplaymode(db *globals, db *channels, dw channel_count, dw channel_stride);
void iplay_sub_131da(db *channel);
void iplay_sub_131ef(db *channel, db value, dw volume, db max_volume);
void iplay_sub_13177(db *channel, dw period, dd dword_245bc, dd dword_245c0, db shift);
void iplay_sub_13429_guard(IplayRegs *r, db *channel);
void iplay_sub_137d5_guard(IplayRegs *r, db *channel);
void iplay_sub_13813_guard(IplayRegs *r, db *channel);
void iplay_sub_140b6_guard(IplayRegs *r, db *globals);
void iplay_eff_13bc0(db *channel, db value);
void iplay_eff_13c34(db *channel, db value);
void iplay_eff_13a43_state(db *channel, db input, db sndflags);
void iplay_eff_13a43(IplayRegs *r, db *channel, db sndflags);
void iplay_eff_13a94(IplayRegs *r, db *channel, db byte_2461a);
IplayRegs3Result iplay_eff_13ad7_result(db *channel, db input, db max_volume, dd eax, dd ecx, dd edx);
void iplay_eff_13ad7(IplayRegs *r, db *channel, db max_volume);
dw iplay_eff_13b06_ax(db *globals, db input);
void iplay_eff_13b06(IplayRegs *r, db *globals, db flag_playsettings);
db iplay_eff_13b78_al(db *channel, db input, db max_volume);
void iplay_eff_13b78(IplayRegs *r, db *channel, db max_volume);
IplayRegs3Result iplay_eff_13b88_result(db *globals, db input, dd eax, dd ecx, dd edx);
void iplay_eff_13b88(IplayRegs *r, db *globals);
void iplay_eff_13bb2_state(db *channel, db input);
void iplay_eff_13bb2(IplayRegs *r, db *channel);
IplayRegs6Result iplay_eff_13ba3_result(db *channel, db input, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_eff_13ba3(IplayRegs *r, db *channel);
IplayRegs6Result iplay_eff_13bc8_result(db *channel, db input, dw dx, db byte_2461a, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_eff_13bc8(IplayRegs *r, db *channel, db byte_2461a);
dd iplay_eff_13c02_eax(db *channel, db *globals, db input, dw word_245f6, dd eax);
void iplay_eff_13c02(IplayRegs *r, db *channel, db *globals, dw word_245f6);
IplayRegs6Result iplay_eff_13c3f_result(db *channel, db input, db byte_24668, db sndflags, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_eff_13c3f(IplayRegs *r, db *channel, db byte_24668, db sndflags);
IplayRegs3Result iplay_eff_13c64_result(const db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx);
void iplay_eff_13c64(IplayRegs *r, db *channel, db byte_24668);
IplayRegs3Result iplay_eff_13c88_result(db *channel, db input, db byte_24668, db max_volume, dd eax, dd ecx, dd edx);
void iplay_eff_13c88(IplayRegs *r, db *channel, db byte_24668, db max_volume);
IplayRegs3Result iplay_eff_13c95_result(db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx);
void iplay_eff_13c95(IplayRegs *r, db *channel, db byte_24668);
dd iplay_eff_13ca2_eax(db input, db byte_24668, dd eax);
void iplay_eff_13ca2(IplayRegs *r, db *globals, db byte_24668);
void iplay_eff_13cb3_state(db *channel, db input, db byte_24668);
void iplay_eff_13cb3(IplayRegs *r, db *channel, db byte_24668);
dd iplay_eff_13cc9_eax(db *globals, db input, db byte_24668, db byte_2466d, dd eax);
void iplay_eff_13cc9(IplayRegs *r, db *globals, db byte_24668, db byte_2466d);
void iplay_eff_13cdd_state(db *globals, db input, db flag_playsettings);
void iplay_eff_13cdd(IplayRegs *r, db *globals, db flag_playsettings);
void iplay_eff_13ce8_state(db *globals, db input);
void iplay_eff_13ce8(IplayRegs *r, db *globals);
IplayRegs3Result iplay_sub_13cf6_result(db *globals, db tempo, dw freq, dw buffer_size, dd eax, dd ecx, dd edx);
void iplay_sub_13cf6(IplayRegs *r, db *globals, dw freq, dw buffer_size);
IplayRegs3Result iplay_sub_13d95_result(db *globals, dd eax, dd ecx, dd edx);
void iplay_sub_13d95(IplayRegs *r, db *globals);
void iplay_sub_13e9b_public(IplayRegs *r);
IplayRegs6Result iplay_sub_13826_result(db *channel, db input, db byte_2461a, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_sub_13826(IplayRegs *r, db *channel, db byte_2461a);
void iplay_sub_13826_full(IplayRegs *r, db *globals, db *channel);
dd iplay_eff_138d2_eax(db *channel, db input, dd eax);
void iplay_eff_138d2(IplayRegs *r, db *channel);
dd iplay_eff_1392f_eax(db *channel, db input, db flag_playsettings, dd eax);
void iplay_eff_1392f(IplayRegs *r, db *channel, db flag_playsettings);
IplayRegs3Result iplay_eff_139ac_result(db *channel, db input, db max_volume, dd eax, dd ecx, dd edx);
void iplay_eff_139ac(IplayRegs *r, db *channel, db max_volume);
IplayRegs3Result iplay_eff_139b2_result(db *channel, db input, db max_volume, db flag_playsettings, dd eax, dd ecx, dd edx);
void iplay_eff_139b2(IplayRegs *r, db *channel, db max_volume, db flag_playsettings);
dd iplay_eff_139b9_eax(db *channel, db input, db max_volume, dd eax);
void iplay_eff_139b9(IplayRegs *r, db *channel, db max_volume);
dd iplay_eff_slide_step_eax(db *channel, db input, db shift, dd eax);
dd iplay_eff_13e1e_eax(db *channel, db input, dd eax);
void iplay_eff_13e1e(IplayRegs *r, db *channel);
dd iplay_vibrato_eax(db *channel, db input, db flag_playsettings, db base_shift, db update_memory, dd eax);
dd iplay_eff_13e2d_eax(db *channel, db input, db flag_playsettings, dd eax);
void iplay_eff_13e2d(IplayRegs *r, db *channel, db flag_playsettings);
IplayRegs3Result iplay_eff_13e7f_result(db *channel, db input, db byte_24668, db max_volume, dd eax, dd ecx, dd edx);
void iplay_eff_13e7f(IplayRegs *r, db *channel, db byte_24668, db max_volume);
IplayRegs3Result iplay_eff_13e84_result(db *channel, db input, db byte_24668, db max_volume, db flag_playsettings, dd eax, dd ecx, dd edx);
void iplay_eff_13e84(IplayRegs *r, db *channel, db byte_24668, db max_volume, db flag_playsettings);
IplayRegs6Result iplay_eff_13fbe_result(db *channel, db input, db byte_24668, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_eff_13fbe(IplayRegs *r, db *channel, db byte_24668);
IplayRegs3Result iplay_eff_13de5_result(db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx);
void iplay_eff_13de5(IplayRegs *r, db *channel, db byte_24668);
IplayRegs3Result iplay_eff_13def_result(db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx);
void iplay_eff_13def(IplayRegs *r, db *channel, db byte_24668);
IplayRegs3Result iplay_eff_13e32_result(db *channel, db input, db byte_24668, db max_volume, dd eax, dd ecx, dd edx);
void iplay_eff_13e32(IplayRegs *r, db *channel, db byte_24668, db max_volume);
IplayRegs3Result iplay_eff_13e8c_result(db *globals, db input, dw freq, dw buffer_size, dd eax, dd ecx, dd edx);
void iplay_eff_13e8c(IplayRegs *r, db *globals, dw freq, dw buffer_size);
dd iplay_eff_13f05_eax(db *channel, db input, db byte_24668, dd eax);
void iplay_eff_13f05(IplayRegs *r, db *channel, db byte_24668);
IplayRegs3Result iplay_eff_13f3b_result(db *channel, db input, db byte_24668, db max_volume, dd eax, dd ecx, dd edx);
void iplay_eff_13f3b(IplayRegs *r, db *channel, db byte_24668, db max_volume);
dd iplay_change_amplif_eax(db *globals, db sound_mode, dd eax);
void iplay_change_amplif(IplayRegs *r, db *globals, db sound_mode);
dd iplay_eff_14020_eax(db *globals, db sound_mode, dd eax);
void iplay_eff_14020(IplayRegs *r, db *globals, db sound_mode);
dd iplay_eff_13886_eax(db *channel, db input, dd eax);
void iplay_eff_13886(IplayRegs *r, db *channel);
dd iplay_eff_138a4_eax(db *channel, db input, dd eax);
void iplay_eff_138a4(IplayRegs *r, db *channel);
dd iplay_eff_1387f_eax(db *channel, db input, db active_channel, dd eax);
void iplay_eff_1387f(IplayRegs *r, db *channel, db active_channel);
dd iplay_eff_1389d_eax(db *channel, db input, db active_channel, dd eax);
void iplay_eff_1389d(IplayRegs *r, db *channel, db active_channel);
dw iplay_calc_14043_ax(db byte_2467b, db byte_2467c);
void iplay_calc_14043(IplayRegs *r, db byte_2467b, db byte_2467c);
IplayRegs3Result iplay_sub_14087_result(db *channel, db input, db byte_24668, dd eax, dd ecx, dd edx);
void iplay_sub_14087(IplayRegs *r, db *channel, db byte_24668);
IplayRegs3Result iplay_eff_14030_result(db *globals, db input, db byte_2467c, dw freq, dw buffer_size, dd eax, dd ecx, dd edx);
void iplay_eff_14030(IplayRegs *r, db *globals, db byte_2467c, dw freq, dw buffer_size);
IplayRegs3Result iplay_eff_14067_result(db *globals, db input, db byte_2467b, db byte_2467c, dw freq, dw buffer_size, dd eax, dd ecx, dd edx);
void iplay_eff_14067(IplayRegs *r, db *globals, db byte_2467b, db byte_2467c, dw freq, dw buffer_size);
void iplay_snd_guard_state(db *globals, unsigned op);
void iplay_snd_guard(IplayRegs *r, db *globals, unsigned op);
void iplay_midi_154da(IplayRegs *r, const db *channel);
void iplay_midi_154de(IplayRegs *r, const db *channel);
void iplay_midi_154ac(IplayRegs *r, db *channel, db max_volume);
void iplay_midi_15413_guard(IplayRegs *r, db *last_status);
void iplay_sub_154f4(IplayRegs *r, db *globals, const db *channel);
void iplay_mix_channel_8bit(
    db *channel,
    const db *samples,
    const int16_t *volume_table,
    db *mix_buffer,
    dw frame_count,
    int interpolation,
    int wide_accumulator);
void iplay_sub_15577_disabled(IplayRegs *r, db *channel);
IplayRegs6Result iplay_sub_1609f_disabled_result(db *dst, dw buffer_size, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_sub_1609f_disabled(IplayRegs *r, db *dst, dw buffer_size);
void iplay_sub_1609f_disabled(IplayRegs *r, db *dst, dw buffer_size);
IplayRegs6Result iplay_volume_prep_inactive_result(db *globals, db *dst, dw word_24610, dw size, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_volume_prep_inactive(IplayRegs *r, db *globals, db *dst, dw word_24610, dw size);
IplaySb16RegsResult iplay_sb_helper_no_device_result(const char *symbol, dw base_port, dd eax, dd ebx, dd ecx, dd edx);
void iplay_sb_helper_no_device(IplayRegs *r, const char *symbol, dw base_port);
void iplay_sb_write_no_device_state(void);
void iplay_sb_write_no_device(IplayRegs *r);
IplaySb16RegsResult iplay_set_dmachn_mask_no_device_result(dw channel, dd eax, dd ebx, dd ecx, dd edx);
void iplay_set_dmachn_mask_no_device(IplayRegs *r, dw channel);
IplaySb16RegsResult iplay_adlib_delay_no_device_result(const char *symbol, dd eax, dd ebx, dd ecx, dd edx);
void iplay_adlib_delay_no_device(IplayRegs *r, const char *symbol);
void iplay_adlib_delay_public(IplayRegs *r);
IplayRegs6Result iplay_sb_legacy_init_no_device_result(db *globals, int sbpro_mode, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_sb_legacy_init_no_device(IplayRegs *r, db *globals, int sbpro_mode);
IplaySb16RegsResult iplay_sb_detect_irq_no_device_result(dd eax, dd ebx, dd ecx, dd edx);
void iplay_sb_detect_irq_no_device(IplayRegs *r);
void iplay_sb_detect_irq_public(IplayRegs *r);
IplayRegs6Result iplay_sb_test_interrupt_no_device_result(db *counter, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_sb_test_interrupt_no_device(IplayRegs *r, db *counter);
size_t iplay_sb16_start_commands(
    db *commands,
    size_t capacity,
    dw output_frequency,
    db bit_mode,
    db stereo_flag,
    dw dma_block_bytes);
size_t iplay_sb16_dma_channel5_events(
    db *events,
    size_t capacity,
    dw buffer_segment,
    dw buffer_offset,
    dd buffer_addend,
    dw dma_block_bytes,
    db dma_mode,
    dw config_word);
void iplay_sb_on_bounded(db *globals, const char *symbol);
void iplay_sb_handler_int_bounded_state(db *globals);
void iplay_sb_handler_int_bounded(IplayRegs *r, db *globals);
IplaySb16RegsResult iplay_sub_19050_bounded_result(dd eax, dd ebx, dd ecx, dd edx);
void iplay_sub_19050_bounded(IplayRegs *r, db *globals);
IplayRegs6Result iplay_memfill8080_result(db *dma, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_memfill8080(IplayRegs *r, db *dma);
IplayRegs6Result iplay_sndoff_fill_result(db *dma, const char *symbol, dd eax, dd ebx, dd ecx, dd edx, dd esi, dd edi);
void iplay_sndoff_fill(IplayRegs *r, db *dma, const char *symbol);
db iplay_audio_init_failure(db *globals, db *text, const char *symbol);
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
    db sndflags);
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
    db sndflags);

#ifdef __WATCOMC__
/*
 * Final 16-bit DOS ABI targets. These names mirror the original entry points;
 * the C cores above are used by the test adapter while the direct DOS-callable
 * wrappers are filled in function-by-function. The pragmas document the live
 * register contract expected by the assembly tests.
 */
void u4tox(void);
#pragma aux u4tox          __parm __caller [] __modify __exact [__ax __si]
void u8tox(void);
#pragma aux u8tox          __parm __caller [] __modify __exact [__ax __si]
void u16tox(void);
#pragma aux u16tox         __parm __caller [] __modify __exact [__ax __si]
void u32tox(void);
#pragma aux u32tox         __parm __caller [] __modify __exact [__ax __dx __si]
void my_u4tox(void);
#pragma aux my_u4tox       __parm __caller [] __modify __exact [__ax __si]
void my_u8tox(void);
#pragma aux my_u8tox       __parm __caller [] __modify __exact [__ax __si]
void my_u16tox(void);
#pragma aux my_u16tox      __parm __caller [] __modify __exact [__ax __si]
void my_u32tox(void);
#pragma aux my_u32tox      __parm __caller [] __modify __exact [__ax __dx __si]
void my_putdigit(void);
#pragma aux my_putdigit    __parm __caller [] __modify __exact [__cx __dx __si]
void myputdigit(void);
#pragma aux myputdigit     __parm __caller [] __modify __exact [__cx __dx __si]
void my_u32toa(void);
#pragma aux my_u32toa      __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void my_u8toa_10(void);
#pragma aux my_u8toa_10    __parm __caller [] __modify __exact [__ax __cx __dx __si]
void my_u16toa_10(void);
#pragma aux my_u16toa_10   __parm __caller [] __modify __exact [__ax __cx __dx __si]
void my_u32toa10_0(void);
#pragma aux my_u32toa10_0  __parm __caller [] __modify __exact [__ax __dx __cx __si]
void my_u8toa10(void);
#pragma aux my_u8toa10     __parm __caller [] __modify __exact [__ax __cx __dx __si]
void my_u16toa10(void);
#pragma aux my_u16toa10    __parm __caller [] __modify __exact [__ax __cx __dx __si]
void my_u32toa10(void);
#pragma aux my_u32toa10    __parm __caller [] __modify __exact [__ax __dx __cx __si]
void my_i8toa10_0(void);
#pragma aux my_i8toa10_0   __parm __caller [] __modify __exact [__ax __cx __dx __si]
void my_i16toa10_0(void);
#pragma aux my_i16toa10_0  __parm __caller [] __modify __exact [__ax __cx __dx __si]
void my_i32toa10_0(void);
#pragma aux my_i32toa10_0  __parm __caller [] __modify __exact [__ax __dx __cx __si]
void my_i8toa10(void);
#pragma aux my_i8toa10     __parm __caller [] __modify __exact [__ax __cx __dx __si]
void my_i16toa10(void);
#pragma aux my_i16toa10    __parm __caller [] __modify __exact [__ax __cx __dx __si]
void my_i32toa10(void);
#pragma aux my_i32toa10    __parm __caller [] __modify __exact [__ax __dx __cx __si]
void my_u32toa_fill(void);
#pragma aux my_u32toa_fill __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void my_pnt_u32toa_fill(void);
#pragma aux my_pnt_u32toa_fill __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void myasmsprintf(void);
#pragma aux myasmsprintf   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void useless_mysprintf(void);
#pragma aux useless_mysprintf __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void mystrlen_0(void);
#pragma aux mystrlen_0     __parm __caller [] __modify __exact [__ax __si]
void mystrlen(void);
#pragma aux mystrlen       __parm __caller [] __modify __exact [__ax __si]
void strcpy_count_0(void);
#pragma aux strcpy_count_0 __parm __caller [] __modify __exact [__ax __cx __si __di __es]
void copy_printable(void);
#pragma aux copy_printable __parm __caller [] __modify __exact [__ax __cx __si __di]
void cpy_printable(void);
#pragma aux cpy_printable  __parm __caller [] __modify __exact [__ax __cx __si __di __es]
void strcpy_count(void);
#pragma aux strcpy_count   __parm __caller [] __modify __exact [__ax __cx __si __di __es]
void txt_1ABAE(void);
#pragma aux txt_1ABAE      __parm __caller [] __modify __exact [__ax __cx __si __di __fs __es]
void put_message(void);
#pragma aux put_message    __parm __caller [] __modify __exact [__ax __si __di __es]
void put_message2(void);
#pragma aux put_message2   __parm __caller [] __modify __exact [__ax __si __di __fs __es]
void text_1BF69(void);
#pragma aux text_1BF69     __parm __caller [] __modify __exact [__ax __si __di __bp __es]
void message_1BE77(void);
#pragma aux message_1BE77  __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void draw_frame(void);
#pragma aux draw_frame     __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void write_scr(void);
#pragma aux write_scr      __parm __caller [] __modify __exact [__ax __bp __si __di __es]
void txt_draw_top_title(void);
#pragma aux txt_draw_top_title __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void txt_draw_bottom(void);
#pragma aux txt_draw_bottom __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void filelist_198B8(void);
#pragma aux filelist_198B8 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void recolortxt(void);
#pragma aux recolortxt     __parm __caller [] __modify __exact [__ax __cx __di]
void mouse_1C7A9(void);
#pragma aux mouse_1C7A9    __parm __caller [] __modify __exact [__ax __bp __cx __dx __si __di]
void mouse_1C7CF(void);
#pragma aux mouse_1C7CF    __parm __caller [] __modify __exact [__ax __bx __bp __cx __dx __si __di]
void int24(void);
#pragma aux int24          __parm __caller [] __modify __exact [__ax]
void ems_init(void);
#pragma aux ems_init       __parm __caller [] __modify __exact [__ax __bx __dx __es]
void ems_release(void);
#pragma aux ems_release    __parm __caller [] __modify __exact [__ax __bx __dx]
void ems_realloc(void);
#pragma aux ems_realloc    __parm __caller [] __modify __exact [__ax __bx __dx]
void ems_deinit(void);
#pragma aux ems_deinit     __parm __caller [] __modify __exact [__ax __bx __dx]
void ems_save_mapctx(void);
#pragma aux ems_save_mapctx __parm __caller [] __modify __exact [__ax __cx __dx]
void ems_restore_mapctx(void);
#pragma aux ems_restore_mapctx __parm __caller [] __modify __exact [__ax __cx __dx]
void ems_mapmem(void);
#pragma aux ems_mapmem     __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void ems_mapmem2(void);
#pragma aux ems_mapmem2    __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void ems_realloc2(void);
#pragma aux ems_realloc2   __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void ems_mapmemx(void);
#pragma aux ems_mapmemx    __parm __caller [] __modify __exact []
void ems_mapmemy(void);
#pragma aux ems_mapmemy    __parm __caller [] __modify __exact []
void clean_11C43(void);
#pragma aux clean_11C43    __parm __caller [] __modify __exact [__ax __cx __dx __di __es]
void mod_sub_delta(void);
#pragma aux mod_sub_delta  __parm __caller [] __modify __exact [__ax __cx __si]
void sub_11BA6(void);
#pragma aux sub_11BA6      __parm __caller [] __modify __exact [__ax __cx __dx __di]
void mod_102F5(void);
#pragma aux mod_102F5      __parm __caller [] __modify __exact [__ax __bx __cx __si]
void sub_126A9(void);
#pragma aux sub_126A9      __parm __caller [] __modify __exact [__ax __bx __cx __si __di __es]
void ult_read(void);
#pragma aux ult_read       __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sub_1265D(void);
#pragma aux sub_1265D      __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si __di __es]
void memfree_125DA(void);
#pragma aux memfree_125DA  __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void mod_1021E(void);
#pragma aux mod_1021E      __parm __caller [] __modify __exact [__ax __cx __si __di __es]
void mod_1024A(void);
#pragma aux mod_1024A      __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void memfree_18A28(void);
#pragma aux memfree_18A28  __parm __caller [] __modify __exact [__ax]
void sub_11C0C(void);
#pragma aux sub_11C0C      __parm __caller [] __modify __exact [__ax __bx __si]
void sub_1415E(void);
#pragma aux sub_1415E      __parm __caller [] __modify __exact [__ax __bx __si __es]
void sub_12F56(void);
#pragma aux sub_12F56      __parm __caller [] __modify __exact [__ax __cx __si]
void sub_135CA(void);
#pragma aux sub_135CA      __parm __caller [] __modify __exact [__ax __bx __cx __si __es]
void spectr_1B084(void);
#pragma aux spectr_1B084   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp]
void f5_draw_spectr(void);
#pragma aux f5_draw_spectr __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void fill_dmabuf8(void);
#pragma aux fill_dmabuf8   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void fill_dmabuf8stereo(void);
#pragma aux fill_dmabuf8stereo __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void fill_dmabuf16stereo(void);
#pragma aux fill_dmabuf16stereo __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void fill_dma(void);
#pragma aux fill_dma     __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void get_keybsw(void);
#pragma aux get_keybsw   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void set_keybsw(void);
#pragma aux set_keybsw   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void int9_keyb(void);
#pragma aux int9_keyb    __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void sub_197F2(void);
#pragma aux sub_197F2    __parm __caller [] __modify __exact [__cx __si __di]
void useless_11787(void);
#pragma aux useless_11787 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void set_timer(void);
#pragma aux set_timer    __parm __caller [] __modify __exact [__ax __cx __si __di]
void clean_timer(void);
#pragma aux clean_timer  __parm __caller [] __modify __exact [__ax __cx __si __di]
void set_timer_int(void);
#pragma aux set_timer_int __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void useless_doswrite2(void);
#pragma aux useless_doswrite2 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void useless_doswrite(void);
#pragma aux useless_doswrite __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void ult_1150B(void);
#pragma aux ult_1150B    __parm __caller [] __modify __exact [__ax __cx]
void set_egasequencer(void);
#pragma aux set_egasequencer __parm __caller [] __modify __exact [__ax __dx]
void graph_1C070(void);
#pragma aux graph_1C070  __parm __caller [] __modify __exact [__ax __dx]
void useless_unset_egaseq(void);
#pragma aux useless_unset_egaseq __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void useless_strange(void);
#pragma aux useless_strange __parm __caller [] __modify __exact [__ax __cx __dx __di __bp]
void useless_writeinr_118(void);
#pragma aux useless_writeinr_118 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void useless_writeinr(void);
#pragma aux useless_writeinr __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void useless_12D61(void);
#pragma aux useless_12D61 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void txt_blinkingoff(void);
#pragma aux txt_blinkingoff __parm __caller [] __modify __exact [__ax __bx]
void txt_enableblink(void);
#pragma aux txt_enableblink __parm __caller [] __modify __exact [__ax __bx]
void useless_sprint_6(void);
#pragma aux useless_sprint_6 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void useless_sprint_7(void);
#pragma aux useless_sprint_7 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void useless_sprint_8(void);
#pragma aux useless_sprint_8 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void useless_sprint_9(void);
#pragma aux useless_sprint_9 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void useless_sprint_10(void);
#pragma aux useless_sprint_10 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void useless_sprint_11(void);
#pragma aux useless_sprint_11 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void useless_sprint_12(void);
#pragma aux useless_sprint_12 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void snd_on_parnt(void);
#pragma aux snd_on_parnt __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void memfree(void);
#pragma aux memfree      __parm __caller [] __modify __exact [__ax]
void midi_clean(void);
#pragma aux midi_clean   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void midi_sndoff(void);
#pragma aux midi_sndoff  __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void midi_153C0(void);
#pragma aux midi_153C0   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void midi_153D6(void);
#pragma aux midi_153D6   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void midi_153F1(void);
#pragma aux midi_153F1   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void midi_15442(void);
#pragma aux midi_15442   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void midi_set(void);
#pragma aux midi_set     __parm __caller [] __modify __exact [__ax __bx __dx]
void midi_1544D(void);
#pragma aux midi_1544D   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void midi_15466(void);
#pragma aux midi_15466   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void setint_vect(void);
#pragma aux setint_vect  __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void getint_vect(void);
#pragma aux getint_vect  __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void setsnd_handler(void);
#pragma aux setsnd_handler __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void restore_intvector(void);
#pragma aux restore_intvector __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sb16_detect_port(void);
#pragma aux sb16_detect_port __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sb16_sound_on(void);
#pragma aux sb16_sound_on __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sb16_init(void);
#pragma aux sb16_init    __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sb16_handler_int(void);
#pragma aux sb16_handler_int __parm __caller [] __modify __exact [__ax __cx __dx]
void sb16_18540(void);
#pragma aux sb16_18540   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void int1a_timer(void);
#pragma aux int1a_timer  __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void inr_read_119B7(void);
#pragma aux inr_read_119B7 __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void mod_readfile_11F4E(void);
#pragma aux mod_readfile_11F4E __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void mod_readfile_12247(void);
#pragma aux mod_readfile_12247 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void stereo_timer_int(void);
#pragma aux stereo_timer_int __parm __caller [] __modify __exact [__ax __bx __cx __dx __ds __es]
void timer_int_end(void);
#pragma aux timer_int_end __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void sb16_sound_off(void);
#pragma aux sb16_sound_off __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sb16_off(void);
#pragma aux sb16_off     __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sb16_deinit(void);
#pragma aux sb16_deinit  __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sb_clean(void);
#pragma aux sb_clean     __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sb_sndoff(void);
#pragma aux sb_sndoff    __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sbpro_clean(void);
#pragma aux sbpro_clean  __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sbpro_sndoff(void);
#pragma aux sbpro_sndoff __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void clean_int8_mem_timr(void);
#pragma aux clean_int8_mem_timr __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void covox_deinit(void);
#pragma aux covox_deinit __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void stereo_deinit(void);
#pragma aux stereo_deinit __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void adlib_clean(void);
#pragma aux adlib_clean  __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void pcspeaker_clean(void);
#pragma aux pcspeaker_clean __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void dosgetcurdir(void);
#pragma aux dosgetcurdir __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void doschdir(void);
#pragma aux doschdir    __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void dosfindnext(void);
#pragma aux dosfindnext __parm __caller [] __modify __exact [__ax __cx __dx __si __di]
void dosfread(void);
#pragma aux dosfread    __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void dosseek(void);
#pragma aux dosseek     __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void inr_read_118B0(void);
#pragma aux inr_read_118B0 __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void read2buffer(void);
#pragma aux read2buffer __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void memalloc(void);
#pragma aux memalloc    __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void memrealloc(void);
#pragma aux memrealloc  __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void alloc_dma_buf(void);
#pragma aux alloc_dma_buf __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sub_182DB(void);
#pragma aux sub_182DB   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp]
void nongravis_dma(void);
#pragma aux nongravis_dma __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp]
void sub_1279A(void);
#pragma aux sub_1279A   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp]
void program_dma(void);
#pragma aux program_dma __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void setmemalloc1(void);
#pragma aux setmemalloc1 __parm __caller [] __modify __exact [__ax __bx]
void setmemalloc2(void);
#pragma aux setmemalloc2 __parm __caller [] __modify __exact [__ax __bx]
void setmemallocstrat(void);
#pragma aux setmemallocstrat __parm __caller [] __modify __exact [__ax __bx]
void getmemallocstrat(void);
#pragma aux getmemallocstrat __parm __caller [] __modify __exact [__ax __bx]
void mem_reallocx(void);
#pragma aux mem_reallocx __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void deinit_125B9(void);
#pragma aux deinit_125B9 __parm __caller [] __modify __exact []
void initclockfromrtc(void);
#pragma aux initclockfromrtc __parm __caller [] __modify __exact [__ax __bx __cx __dx __es]
void rereadrtc_settmr(void);
#pragma aux rereadrtc_settmr __parm __caller [] __modify __exact [__ax __bx __cx __dx __es]
void loadcfg(void);
#pragma aux loadcfg     __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void dosexec(void);
#pragma aux dosexec     __parm __caller [] __modify __exact []
void callsubx(void);
#pragma aux callsubx    __parm __caller [] __modify __exact []
void memalloc12k(void);
#pragma aux memalloc12k __parm __caller [] __modify __exact [__ax __bx __di __es]
void init_vga_waves(void);
#pragma aux init_vga_waves __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void f2_draw_waves(void);
#pragma aux f2_draw_waves __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp]
void f2_draw_waves2(void);
#pragma aux f2_draw_waves2 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp]
void readallmoules(void);
#pragma aux readallmoules __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void read_module(void);
#pragma aux read_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void moduleread(void);
#pragma aux moduleread  __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void mod_read_10311(void);
#pragma aux mod_read_10311 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void mod_n_t_module(void);
#pragma aux mod_n_t_module __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void modules_search(void);
#pragma aux modules_search __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void start(void);
#pragma aux start       __parm __caller [] __modify __exact []
void keyb_19EFD(void);
#pragma aux keyb_19EFD  __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void nullsub_5(void);
#pragma aux nullsub_5       __parm __caller [] __modify __exact []
void eff_nullsub(void);
#pragma aux eff_nullsub     __parm __caller [] __modify __exact []
void nullsub_2(void);
#pragma aux nullsub_2       __parm __caller [] __modify __exact []
void nullsub_4(void);
#pragma aux nullsub_4       __parm __caller [] __modify __exact []
void nullsub_3(void);
#pragma aux nullsub_3       __parm __caller [] __modify __exact []
void setvideomode(void);
#pragma aux setvideomode   __parm __caller [] __modify __exact [__ax __bx __dx]
void hex_1BE39(void);
#pragma aux hex_1BE39     __parm __caller [] __modify __exact [__ax __di __es]
void text_init(void);
#pragma aux text_init      __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void text_init2(void);
#pragma aux text_init2     __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void f1_help(void);
#pragma aux f1_help        __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void f3_textmetter(void);
#pragma aux f3_textmetter  __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void f4_patternnae(void);
#pragma aux f4_patternnae  __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void f6_undoc(void);
#pragma aux f6_undoc       __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void f2_waves(void);
#pragma aux f2_waves       __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void init_vga_waves(void);
#pragma aux init_vga_waves __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void f5_graphspectr(void);
#pragma aux f5_graphspectr __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void init_f5_spectr(void);
#pragma aux init_f5_spectr __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sub_1AB8C(void);
#pragma aux sub_1AB8C      __parm __caller [] __modify __exact [__ax]
void spectr_1BCE9(void);
#pragma aux spectr_1BCE9   __parm __caller [] __modify __exact [__ax __dx __di]
void spectr_1BC2D(void);
#pragma aux spectr_1BC2D   __parm __caller [] __modify __exact [__ax __cx __dx __di __bx __bp]
void spectr_1BBC1(void);
#pragma aux spectr_1BBC1   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void video_prp_mtr_positn(void);
#pragma aux video_prp_mtr_positn __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp]
void parse_cmdline(void);
#pragma aux parse_cmdline  __parm __caller [] __modify __exact [__ax __cx __dx __si __di __bp __ds __es]
void get_comspec(void);
#pragma aux get_comspec    __parm __caller [] __modify __exact [__di __es]
void getexename(void);
#pragma aux getexename     __parm __caller [] __modify __exact [__ax __cx __di __si __es]
void int2f_checkmyself(void);
#pragma aux int2f_checkmyself __parm __caller [] __modify __exact [__ax]
void spectr_1B406(void);
#pragma aux spectr_1B406   __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp]
void spectr_1C4F8(void);
#pragma aux spectr_1C4F8   __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void get_playsettings(void);
#pragma aux get_playsettings __parm __caller [] __modify __exact [__ax]
void set_playsettings(void);
#pragma aux set_playsettings __parm __caller [] __modify __exact [__ax __cx __dx __di]
void volume_12A66(void);
#pragma aux volume_12A66    __parm __caller [] __modify __exact [__ax __bx __cx]
void change_volume(void);
#pragma aux change_volume   __parm __caller [] __modify __exact [__ax __bx __cx]
void vlm_141DF(void);
#pragma aux vlm_141DF       __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void getset_playstate(void);
#pragma aux getset_playstate __parm __caller [] __modify __exact [__ax __bx]
void get_12F7C(void);
#pragma aux get_12F7C       __parm __caller [] __modify __exact [__ax __bx]
void memclean(void);
#pragma aux memclean        __parm __caller [] __modify __exact [__ax __cx __di]
void read_sndsettings(void);
#pragma aux read_sndsettings __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si]
void sub_12AFD(void);
#pragma aux sub_12AFD      __parm __caller [] __modify __exact [__ax __bx]
void sub_12B18(void);
#pragma aux sub_12B18      __parm __caller [] __modify __exact [__ax __bx __cx __si __di]
void sub_12B83(void);
#pragma aux sub_12B83      __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void sub_12CAD(void);
#pragma aux sub_12CAD      __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sub_12D05(void);
#pragma aux sub_12D05      __parm __caller [] __modify __exact [__ax __cx __si __di]
void sub_12D35(void);
#pragma aux sub_12D35      __parm __caller [] __modify __exact [__ax __bx]
void someplaymode(void);
#pragma aux someplaymode   __parm __caller [] __modify __exact [__ax __cx __dx __di]
void sub_1281A(void);
#pragma aux sub_1281A      __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si __di]
void sub_13044(void);
#pragma aux sub_13044      __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si __di]
void sub_13017(void);
#pragma aux sub_13017      __parm __caller [] __modify __exact [__ax __cx __di]
void configure_timer(void);
#pragma aux configure_timer __parm __caller [] __modify __exact [__ax __cx __dx __di]
void sub_12DA8(void);
#pragma aux sub_12DA8      __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si __di]
void snd_initialze(void);
#pragma aux snd_initialze   __parm __caller [] __modify __exact [__bx]
void snd_on(void);
#pragma aux snd_on          __parm __caller [] __modify __exact [__bx]
void snd_off(void);
#pragma aux snd_off         __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void snd_deinit(void);
#pragma aux snd_deinit      __parm __caller [] __modify __exact [__bx]
void snd_offx(void);
#pragma aux snd_offx        __parm __caller [] __modify __exact [__ax __bx __cx __dx __di]
void sub_131DA(void);
#pragma aux sub_131DA      __parm __caller [] __modify __exact []
void sub_131EF(void);
#pragma aux sub_131EF      __parm __caller [] __modify __exact [__ax]
void sub_13177(void);
#pragma aux sub_13177      __parm __caller [] __modify __exact [__ax __cx __dx __di]
void sub_13429(void);
#pragma aux sub_13429      __parm __caller [] __modify __exact []
void sub_13623(void);
#pragma aux sub_13623      __parm __caller [] __modify __exact [__ax __dx __si __di]
void sub_137D5(void);
#pragma aux sub_137D5      __parm __caller [] __modify __exact [__di]
void sub_13813(void);
#pragma aux sub_13813      __parm __caller [] __modify __exact [__di]
void sub_140B6(void);
#pragma aux sub_140B6      __parm __caller [] __modify __exact []
void eff_13BC0(void);
#pragma aux eff_13BC0      __parm __caller [] __modify __exact []
void eff_13C34(void);
#pragma aux eff_13C34      __parm __caller [] __modify __exact []
void eff_13A43(void);
#pragma aux eff_13A43      __parm __caller [] __modify __exact [__ax]
void eff_13A94(void);
#pragma aux eff_13A94      __parm __caller [] __modify __exact [__ax]
void eff_13AD7(void);
#pragma aux eff_13AD7      __parm __caller [] __modify __exact [__ax __dx]
void eff_13B06(void);
#pragma aux eff_13B06      __parm __caller [] __modify __exact [__ax __cx __di]
void eff_13B78(void);
#pragma aux eff_13B78      __parm __caller [] __modify __exact [__ax]
void eff_13B88(void);
#pragma aux eff_13B88      __parm __caller [] __modify __exact [__ax __dx]
void eff_13BB2(void);
#pragma aux eff_13BB2      __parm __caller [] __modify __exact [__ax]
void eff_13BA3(void);
#pragma aux eff_13BA3      __parm __caller [] __modify __exact [__ax __di]
void eff_13BC8(void);
#pragma aux eff_13BC8      __parm __caller [] __modify __exact [__ax __di]
void eff_13C02(void);
#pragma aux eff_13C02      __parm __caller [] __modify __exact [__ax]
void eff_13C3F(void);
#pragma aux eff_13C3F      __parm __caller [] __modify __exact [__ax __di]
void eff_13C64(void);
#pragma aux eff_13C64      __parm __caller [] __modify __exact [__ax __dx]
void eff_13C88(void);
#pragma aux eff_13C88      __parm __caller [] __modify __exact [__ax __dx]
void eff_13C95(void);
#pragma aux eff_13C95      __parm __caller [] __modify __exact [__ax __dx]
void eff_13CA2(void);
#pragma aux eff_13CA2      __parm __caller [] __modify __exact [__ax]
void eff_13CB3(void);
#pragma aux eff_13CB3      __parm __caller [] __modify __exact [__ax]
void eff_13CC9(void);
#pragma aux eff_13CC9      __parm __caller [] __modify __exact [__ax]
void eff_13CDD(void);
#pragma aux eff_13CDD      __parm __caller [] __modify __exact [__ax]
void eff_13CE8(void);
#pragma aux eff_13CE8      __parm __caller [] __modify __exact [__ax]
void sub_13CF6(void);
#pragma aux sub_13CF6      __parm __caller [] __modify __exact [__ax __cx __dx]
void sub_13D95(void);
#pragma aux sub_13D95      __parm __caller [] __modify __exact [__ax __cx __dx]
void sub_13E9B(void);
#pragma aux sub_13E9B      __parm __caller [] __modify __exact [__ax __dx __di]
void sub_13826(void);
#pragma aux sub_13826      __parm __caller [] __modify __exact [__ax __cx __di]
void eff_138D2(void);
#pragma aux eff_138D2      __parm __caller [] __modify __exact [__ax __dx]
void eff_1392F(void);
#pragma aux eff_1392F      __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_139AC(void);
#pragma aux eff_139AC      __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_139B2(void);
#pragma aux eff_139B2      __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_139B9(void);
#pragma aux eff_139B9      __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_13E1E(void);
#pragma aux eff_13E1E      __parm __caller [] __modify __exact [__ax __dx]
void eff_13E2D(void);
#pragma aux eff_13E2D      __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_13E7F(void);
#pragma aux eff_13E7F      __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_13E84(void);
#pragma aux eff_13E84      __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_13FBE(void);
#pragma aux eff_13FBE      __parm __caller [] __modify __exact [__ax __dx]
void eff_13DE5(void);
#pragma aux eff_13DE5      __parm __caller [] __modify __exact [__ax __dx]
void eff_13DEF(void);
#pragma aux eff_13DEF      __parm __caller [] __modify __exact [__ax __dx]
void eff_13E32(void);
#pragma aux eff_13E32      __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_13E8C(void);
#pragma aux eff_13E8C      __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_13F05(void);
#pragma aux eff_13F05      __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_13F3B(void);
#pragma aux eff_13F3B      __parm __caller [] __modify __exact [__ax __cx __dx]
void eff_14020(void);
#pragma aux eff_14020      __parm __caller [] __modify __exact [__ax __bx __si]
void change_amplif(void);
#pragma aux change_amplif  __parm __caller [] __modify __exact [__ax __cx __di]
void eff_1387F(void);
#pragma aux eff_1387F      __parm __caller [] __modify __exact [__ax]
void eff_13886(void);
#pragma aux eff_13886      __parm __caller [] __modify __exact [__ax]
void eff_1389D(void);
#pragma aux eff_1389D      __parm __caller [] __modify __exact [__ax]
void eff_138A4(void);
#pragma aux eff_138A4      __parm __caller [] __modify __exact [__ax]
void calc_14043(void);
#pragma aux calc_14043     __parm __caller [] __modify __exact [__ax]
void sub_14087(void);
#pragma aux sub_14087      __parm __caller [] __modify __exact [__ax __dx]
void eff_14030(void);
#pragma aux eff_14030      __parm __caller [] __modify __exact [__ax __cx __dx __di]
void eff_14067(void);
#pragma aux eff_14067      __parm __caller [] __modify __exact [__ax __cx __dx]
void midi_154DA(void);
#pragma aux midi_154DA     __parm __caller [] __modify __exact [__ax]
void midi_154DE(void);
#pragma aux midi_154DE     __parm __caller [] __modify __exact [__ax __dx]
void midi_154AC(void);
#pragma aux midi_154AC     __parm __caller [] __modify __exact [__ax __di]
void midi_15413(void);
#pragma aux midi_15413     __parm __caller [] __modify __exact [__ax __cx __dx]
void sub_154F4(void);
#pragma aux sub_154F4      __parm __caller [] __modify __exact [__ax __bx __cx __bp __si]
void sub_15577(void);
#pragma aux sub_15577      __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void sub_1609F(void);
#pragma aux sub_1609F      __parm __caller [] __modify __exact [__ax __bx __cx __si __di]
void volume_prep(void);
#pragma aux volume_prep    __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void WriteMixerSB(void);
#pragma aux WriteMixerSB   __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void ReadMixerSB(void);
#pragma aux ReadMixerSB    __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void WriteSB(void);
#pragma aux WriteSB        __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void ReadSB(void);
#pragma aux ReadSB         __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void CheckSB(void);
#pragma aux CheckSB        __parm __caller [] __modify __exact [__ax __bx __cx __dx]
void set_dmachn_mask(void);
#pragma aux set_dmachn_mask __parm __caller [] __modify __exact [__ax]
void adlib_18395(void);
#pragma aux adlib_18395    __parm __caller [] __modify __exact []
void adlib_18389(void);
#pragma aux adlib_18389    __parm __caller [] __modify __exact [__ax]
void sbpro_init(void);
#pragma aux sbpro_init     __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void sb_init(void);
#pragma aux sb_init        __parm __caller [] __modify __exact [__ax __bx __cx __dx __si]
void sb_detect_irq(void);
#pragma aux sb_detect_irq  __parm __caller [] __modify __exact [__ax __dx]
void sb_test_interrupt(void);
#pragma aux sb_test_interrupt __parm __caller [] __modify __exact [__ax __bx __cx __si]
void sb_on(void);
#pragma aux sb_on          __parm __caller [] __modify __exact [__ax __cx __dx __si]
void sb16_on(void);
#pragma aux sb16_on        __parm __caller [] __modify __exact [__ax __cx __dx __si]
void sb_handler_int(void);
#pragma aux sb_handler_int __parm __caller [] __modify __exact []
void sub_19050(void);
#pragma aux sub_19050      __parm __caller [] __modify __exact [__ax __dx]
void memfill8080(void);
#pragma aux memfill8080    __parm __caller [] __modify __exact [__ax __bx __cx __es __di]
void covox_sndoff(void);
#pragma aux covox_sndoff   __parm __caller [] __modify __exact [__ax __bx __cx __es __di]
void stereo_sndoff(void);
#pragma aux stereo_sndoff  __parm __caller [] __modify __exact [__ax __bx __cx __es __di]
void adlib_sndoff(void);
#pragma aux adlib_sndoff   __parm __caller [] __modify __exact [__ax __bx __cx __es __di]
void pcspeaker_sndoff(void);
#pragma aux pcspeaker_sndoff __parm __caller [] __modify __exact [__ax __bx __cx __es __di]
void covox_init(void);
#pragma aux covox_init     __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void stereo_init(void);
#pragma aux stereo_init    __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void adlib_init(void);
#pragma aux adlib_init     __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void pcspeaker_init(void);
#pragma aux pcspeaker_init __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
#endif

#ifdef __cplusplus
}
#endif

#endif
