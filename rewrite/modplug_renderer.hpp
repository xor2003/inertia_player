#ifndef IPLAY_MODPLUG_RENDERER_HPP
#define IPLAY_MODPLUG_RENDERER_HPP

#include <cstdint>

#define IPLAY_MODPLUG_UI_MAX_CHANNELS 32
#define IPLAY_MODPLUG_UI_MAX_SAMPLES 64
#define IPLAY_ORIGINAL_CHANNEL_METER_RAW_MAX 60u
#define IPLAY_ORIGINAL_CHANNEL_METER_CELLS 30u
#define IPLAY_ORIGINAL_CHANNEL_METER_DECAY 3u
#define IPLAY_ORIGINAL_CHANNEL_METER_DECAY_MASK 31u
#define IPLAY_PAL_MIX_RATE 44100u
#define IPLAY_NTSC_MIX_RATE 43698u

struct IplayModplugChannelState {
    unsigned note;
    unsigned instrument;
    unsigned volume_effect;
    unsigned effect;
    unsigned row_effect;
    unsigned volume;
    unsigned parameter;
    unsigned row_parameter;
    unsigned level;
    unsigned pan;
    unsigned pan_valid;
    char sample_name[23];
};

struct IplayTrackerSampleInfo {
    unsigned length;
    unsigned volume;
    unsigned c2_speed;
    unsigned loop_start;
    unsigned loop_end;
    unsigned flags;
    unsigned valid;
};

struct IplayModplugUiSnapshot {
    unsigned order;
    unsigned pattern;
    unsigned row;
    unsigned rows;
    unsigned pattern_count;
    unsigned channel_count;
    unsigned playing_channels;
    unsigned speed;
    unsigned tempo;
    unsigned sample_count;
    unsigned used_samples;
    unsigned channel_vu_available;
    char title[21];
    char sample_names[IPLAY_MODPLUG_UI_MAX_SAMPLES][23];
    IplayTrackerSampleInfo sample_info[IPLAY_MODPLUG_UI_MAX_SAMPLES];
    IplayModplugChannelState channels[IPLAY_MODPLUG_UI_MAX_CHANNELS];
};

struct IplayModplugPcmStats {
    int frames;
    int samples;
    int bytes;
    std::uint64_t checksum;
    int peak;
    IplayModplugUiSnapshot ui;
};

struct IplayModplugRenderer;
struct IplayModplugPcmSource;
struct IplayMikmodPcmSource;

unsigned iplay_original_channel_meter_envelope_step(unsigned current_raw, unsigned target_raw, unsigned draw_counter);
unsigned iplay_original_meter_raw_from_mikmod(unsigned real_volume, unsigned voice_volume, unsigned channel_count);
unsigned iplay_modplug_pan_from_stereo_vu(float left, float right, unsigned fallback);
IplayModplugRenderer *iplay_modplug_renderer_open_file(const char *path);
bool iplay_modplug_renderer_read(IplayModplugRenderer *renderer, std::int16_t *pcm, int frame_capacity, IplayModplugPcmStats *stats);
bool iplay_modplug_renderer_read_until_end(IplayModplugRenderer *renderer, std::int16_t *pcm, int frame_capacity, int max_blocks, IplayModplugPcmStats *stats);
void iplay_modplug_renderer_close(IplayModplugRenderer *renderer);
bool iplay_modplug_render_file_stats(const char *path, IplayModplugPcmStats *stats);
bool iplay_modplug_render_file_pcm(const char *path, std::int16_t *pcm, int frame_capacity, IplayModplugPcmStats *stats);

IplayModplugPcmSource *iplay_modplug_pcm_source_open_file(const char *path);
bool iplay_modplug_pcm_source_read(IplayModplugPcmSource *source, std::int16_t *pcm, int frame_capacity, IplayModplugPcmStats *stats);
bool iplay_modplug_pcm_source_ended(const IplayModplugPcmSource *source);
bool iplay_modplug_pcm_source_restart(IplayModplugPcmSource *source);
bool iplay_modplug_pcm_source_seek_position(IplayModplugPcmSource *source, unsigned order, unsigned row);
bool iplay_modplug_pcm_source_set_channel_muted(IplayModplugPcmSource *source, unsigned channel, bool muted);
bool iplay_modplug_pcm_source_channel_muted(const IplayModplugPcmSource *source, unsigned channel);
bool iplay_modplug_pcm_source_set_channel_pan(IplayModplugPcmSource *source, unsigned channel, unsigned pan);
void iplay_modplug_pcm_source_set_master_volume(IplayModplugPcmSource *source, unsigned percent);
void iplay_modplug_pcm_source_set_interpolation(IplayModplugPcmSource *source, bool enabled);
void iplay_modplug_pcm_source_set_pal_timing(IplayModplugPcmSource *source, bool enabled);
void iplay_modplug_pcm_source_set_mod_compatibility(IplayModplugPcmSource *source, bool protracker, bool ignore_bpm);
void iplay_modplug_pcm_source_close(IplayModplugPcmSource *source);

IplayMikmodPcmSource *iplay_mikmod_pcm_source_open_file(const char *path);
bool iplay_mikmod_pcm_source_read(IplayMikmodPcmSource *source, std::int16_t *pcm, int frame_capacity, IplayModplugPcmStats *stats);
bool iplay_mikmod_pcm_source_seek_position(IplayMikmodPcmSource *source, unsigned order, unsigned row);
bool iplay_mikmod_pcm_source_set_channel_muted(IplayMikmodPcmSource *source, unsigned channel, bool muted);
bool iplay_mikmod_pcm_source_channel_muted(const IplayMikmodPcmSource *source, unsigned channel);
bool iplay_mikmod_pcm_source_set_channel_pan(IplayMikmodPcmSource *source, unsigned channel, unsigned pan);
void iplay_mikmod_pcm_source_set_interpolation(IplayMikmodPcmSource *source, bool enabled);
void iplay_mikmod_pcm_source_set_pal_timing(IplayMikmodPcmSource *source, bool enabled);
unsigned iplay_mikmod_pcm_source_channel_level_raw(const IplayMikmodPcmSource *source, unsigned channel, unsigned channel_count);
unsigned iplay_mikmod_pcm_source_order(const IplayMikmodPcmSource *source);
unsigned iplay_mikmod_pcm_source_row(const IplayMikmodPcmSource *source);
void iplay_mikmod_pcm_source_close(IplayMikmodPcmSource *source);

#endif
