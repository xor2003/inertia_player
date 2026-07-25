#ifndef IPLAY_MODERN_PLAYER_HPP
#define IPLAY_MODERN_PLAYER_HPP

#include "modplug_audio_bridge.hpp"

#include <stddef.h>

enum IplayModernPlaybackStatus {
    IPLAY_MODERN_PLAYBACK_OK = 0,
    IPLAY_MODERN_PLAYBACK_INVALID_ARGUMENT = 1,
    IPLAY_MODERN_PLAYBACK_PROJECT_DECODER_UNAVAILABLE = 2,
    IPLAY_MODERN_PLAYBACK_EXTERNAL_DECODER_FAILED = 3,
    IPLAY_MODERN_PLAYBACK_UNSUPPORTED_FORMAT = 4,
    IPLAY_MODERN_PLAYBACK_BLOCK_LIMIT = 5,
    IPLAY_MODERN_PLAYBACK_KEYBOARD = 6
};

/* Keep route ids synchronized with the DOS IPLAY_DECODER_ROUTE_* constants. */
enum IplayModernDecoderRoute {
    IPLAY_MODERN_DECODER_ROUTE_EXTERNAL_LIBRARY = 0,
    IPLAY_MODERN_DECODER_ROUTE_PROJECT_OWNED = 1,
    IPLAY_MODERN_DECODER_ROUTE_PROBE_BY_CONTENT = 2
};

struct IplayModernPlaybackResult {
    IplayModernPlaybackStatus status;
    IplayModernDecoderRoute decoder_route;
    const char *decoder_provider;
    IplayModplugAudioBridgeStats audio;
};

#define IPLAY_MODERN_VIEW_HELP 1u
#define IPLAY_MODERN_VIEW_SCOPES 2u
#define IPLAY_MODERN_VIEW_VU 3u
#define IPLAY_MODERN_VIEW_SAMPLES 4u
#define IPLAY_MODERN_VIEW_SPECTRUM 5u
#define IPLAY_MODERN_VIEW_UNDOCUMENTED 6u

bool iplay_modern_play_file_to_sdl_sb16(const char *path,
                                        IplayAudioWriteFn write,
                                        void *write_user,
                                        int frames_per_block,
                                        int max_blocks,
                                        IplayModernPlaybackResult *result);
bool iplay_modern_play_file_to_sdl_sb16_until(const char *path,
                                             IplayAudioWriteFn write,
                                             void *write_user,
                                             int frames_per_block,
                                             int max_blocks,
                                             IplayModplugAudioStopFn stop,
                                             void *stop_user,
                                             IplayModernPlaybackResult *result);
bool iplay_modern_play_file_to_sdl_sb16_controlled(const char *path,
                                                   IplayAudioWriteFn write,
                                                   void *write_user,
                                                   int frames_per_block,
                                                   int max_blocks,
                                                   IplayModplugAudioStopFn stop,
                                                   void *stop_user,
                                                   IplayModplugPlaybackControls *controls,
                                                   IplayModernPlaybackResult *result);

const char *iplay_modern_playback_status_name(IplayModernPlaybackStatus status);
bool iplay_modern_playback_status_started(IplayModernPlaybackStatus status);
const char *iplay_modern_playback_state_text(IplayModernPlaybackStatus status);
const char *iplay_modern_playback_stop_text(const IplayModernPlaybackResult *result);
const char *iplay_modern_playback_panel_status_text(const IplayModernPlaybackResult *result);
const char *iplay_modern_status_title(void);
const char *iplay_modern_audio_backend_name(void);
bool iplay_modern_format_blocks(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);
bool iplay_modern_format_stop(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);
bool iplay_modern_format_accepted(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);
bool iplay_modern_format_frames(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);
bool iplay_modern_format_levels(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);
bool iplay_modern_format_playback_state(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);
bool iplay_modern_playback_summary(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);
bool iplay_modern_render_playback_status(IplayRuntime *runtime, const char *module_path, const IplayModernPlaybackResult *result);
bool iplay_modern_render_playback_view(IplayRuntime *runtime, const char *module_path, const IplayModernPlaybackResult *result, unsigned view);
size_t iplay_modern_external_tracker_extension_count(void);
const char *iplay_modern_external_tracker_extension(size_t index);
int iplay_modern_path_is_external_tracker(const char *path);
int iplay_modern_path_is_project_owned(const char *path);
IplayModernDecoderRoute iplay_modern_decoder_route(const char *path);
const char *iplay_modern_decoder_route_name(const char *path);
int iplay_modern_decoder_route_uses_external_library(const char *path);
const char *iplay_modern_playback_decoder_route_name(const IplayModernPlaybackResult *result);
const char *iplay_modern_playback_decoder_provider_name(const IplayModernPlaybackResult *result);

#endif
