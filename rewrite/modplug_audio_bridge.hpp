#ifndef IPLAY_MODPLUG_AUDIO_BRIDGE_HPP
#define IPLAY_MODPLUG_AUDIO_BRIDGE_HPP

#include "iplay_rewrite.h"
#include "modplug_renderer.hpp"

#ifndef IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER
#define IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER 0
#endif

#define IPLAY_MODPLUG_SCOPE_SAMPLES 80u
#define IPLAY_MODPLUG_FFT_SAMPLES 512u
#define IPLAY_MODPLUG_SPECTRUM_BANDS 100u
#define IPLAY_MODPLUG_SPECTRUM_MAX_LEVEL 90u
#define IPLAY_MODPLUG_CONTROL_MAX_CHANNELS 32u

struct IplayModplugAudioBridgeStats {
    unsigned long blocks;
    unsigned long source_frames;
    unsigned long accepted_bytes;
    unsigned long frames_written;
    unsigned long dropped_frames;
    unsigned long source_checksum;
    unsigned last_left_level;
    unsigned last_right_level;
    unsigned max_left_level;
    unsigned max_right_level;
    unsigned active;
    unsigned source_ended;
    unsigned volume_percent;
    unsigned loop_enabled;
    unsigned interpolation_enabled;
    unsigned protracker_enabled;
    unsigned ignore_bpm_enabled;
    unsigned pattern_loop_enabled;
    unsigned pattern_loop_order;
    unsigned pal_enabled;
    unsigned amplification_percent;
    unsigned selected_channel;
    unsigned channel_muted_mask;
    unsigned channel_pan[IPLAY_MODPLUG_UI_MAX_CHANNELS];
    unsigned channel_pan_valid_mask;
    unsigned channel_generation;
    signed char scope[IPLAY_MODPLUG_SCOPE_SAMPLES];
    unsigned char spectrum[IPLAY_MODPLUG_SPECTRUM_BANDS];
    const char *stop_reason;
    IplayModplugUiSnapshot ui;
};

typedef bool (*IplayModplugAudioStopFn)(void *user, const IplayModplugAudioBridgeStats *stats);

struct IplayModplugPlaybackControls {
    unsigned volume_percent;
    unsigned volume_256;
    unsigned loop_enabled;
    unsigned interpolation_enabled;
    unsigned protracker_enabled;
    unsigned ignore_bpm_enabled;
    unsigned pattern_loop_enabled;
    unsigned pattern_loop_order;
    unsigned pal_enabled;
    unsigned amplification_percent;
    unsigned selected_channel;
    unsigned channel_muted_mask;
    unsigned channel_pan[IPLAY_MODPLUG_UI_MAX_CHANNELS];
    unsigned channel_pan_valid_mask;
    unsigned channel_generation;
    unsigned seek_order;
    unsigned seek_row;
    unsigned seek_generation;
    unsigned generation;
};

void iplay_modplug_playback_controls_init(IplayModplugPlaybackControls *controls);

bool iplay_modplug_audio_play_file_to_sdl_sb16(const char *path,
                                               IplayAudioWriteFn write,
                                               void *write_user,
                                               int frames_per_block,
                                               int max_blocks,
                                               IplayModplugAudioBridgeStats *stats);
bool iplay_modplug_audio_play_file_to_sdl_sb16_until(const char *path,
                                                     IplayAudioWriteFn write,
                                                     void *write_user,
                                                     int frames_per_block,
                                                     int max_blocks,
                                                     IplayModplugAudioStopFn stop,
                                                     void *stop_user,
                                                     IplayModplugAudioBridgeStats *stats);
bool iplay_modplug_audio_play_file_to_sdl_sb16_controlled(const char *path,
                                                          IplayAudioWriteFn write,
                                                          void *write_user,
                                                          int frames_per_block,
                                                          int max_blocks,
                                                          IplayModplugAudioStopFn stop,
                                                          void *stop_user,
                                                          IplayModplugPlaybackControls *controls,
                                                          IplayModplugAudioBridgeStats *stats);

#if IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER
struct IplayModplugExternalDecoder;
struct IplayModplugHookAudioStats {
    unsigned long blocks;
    unsigned long frames;
    unsigned long accepted_bytes;
    unsigned long capture_calls;
    unsigned long capture_bytes;
    unsigned long capture_checksum;
    unsigned last_left_level;
    unsigned last_right_level;
    unsigned max_left_level;
    unsigned max_right_level;
    int source_end;
    int block_limit;
    int last_status;
};
IplayModplugExternalDecoder *iplay_modplug_external_decoder_create(void);
void iplay_modplug_external_decoder_destroy(IplayModplugExternalDecoder *decoder);
int iplay_modplug_external_decoder_render(void *user, const PlayerModuleInfo *module, PlayerPlaybackBlock *block);
void iplay_modplug_external_decoder_install(IplayModplugExternalDecoder *decoder);
void iplay_modplug_external_decoder_uninstall(void);
bool iplay_modplug_external_decoder_play_module_to_sdl_sb16(IplayModplugExternalDecoder *decoder,
                                                            const PlayerModuleInfo *module,
                                                            PlayerPlaybackBlock *block,
                                                            IplayAudioWriteFn write,
                                                            void *write_user,
                                                            dw frames_per_block,
                                                            unsigned long max_blocks,
                                                            int require_source_end,
                                                            IplayModplugHookAudioStats *stats);
#endif

#endif
