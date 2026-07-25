#include "modplug_audio_bridge.hpp"

#include "modplug_renderer.hpp"

#include <cstdint>
#include <cmath>
#include <complex>
#include <cstring>
#include <string>
#include <vector>

static void bridge_clear_stats(IplayModplugAudioBridgeStats *stats) {
    stats->blocks = 0ul;
    stats->source_frames = 0ul;
    stats->accepted_bytes = 0ul;
    stats->frames_written = 0ul;
    stats->dropped_frames = 0ul;
    stats->source_checksum = 0ul;
    stats->last_left_level = 0u;
    stats->last_right_level = 0u;
    stats->max_left_level = 0u;
    stats->max_right_level = 0u;
    stats->active = 0u;
    stats->source_ended = 0u;
    stats->volume_percent = 100u;
    stats->loop_enabled = 0u;
    stats->interpolation_enabled = 1u;
    stats->protracker_enabled = 1u;
    stats->ignore_bpm_enabled = 0u;
    stats->pattern_loop_enabled = 0u;
    stats->pattern_loop_order = 0u;
    stats->pal_enabled = 1u;
    stats->amplification_percent = 100u;
    stats->selected_channel = 0u;
    stats->channel_muted_mask = 0u;
    std::memset(stats->channel_pan, 0, sizeof(stats->channel_pan));
    stats->channel_pan_valid_mask = 0u;
    stats->channel_generation = 0u;
    std::memset(stats->scope, 0, sizeof(stats->scope));
    std::memset(stats->spectrum, 0, sizeof(stats->spectrum));
    stats->stop_reason = "running";
    std::memset(&stats->ui, 0, sizeof(stats->ui));
}

static void bridge_update_visualization(IplayModplugAudioBridgeStats *stats, const std::int16_t *pcm, int frames) {
    const double two_pi = 6.28318530717958647692;
    std::complex<double> fft[IPLAY_MODPLUG_FFT_SAMPLES];
    unsigned i;
    unsigned band;
    if (!stats || !pcm || frames <= 0) return;
    for (i = 0u; i < IPLAY_MODPLUG_SCOPE_SAMPLES; ++i) {
        int frame = (int)(((unsigned long)i * (unsigned long)frames) / IPLAY_MODPLUG_SCOPE_SAMPLES);
        int mono;
        if (frame >= frames) frame = frames - 1;
        mono = ((int)pcm[(std::size_t)frame * 2u] + (int)pcm[(std::size_t)frame * 2u + 1u]) / 2;
        mono /= 2048;
        if (mono < -15) mono = -15;
        if (mono > 15) mono = 15;
        stats->scope[i] = (signed char)mono;
    }
    for (i = 0u; i < IPLAY_MODPLUG_FFT_SAMPLES; ++i) {
        double mono = i < (unsigned)frames
            ? ((double)pcm[(std::size_t)i * 2u] + (double)pcm[(std::size_t)i * 2u + 1u]) * 0.5
            : 0.0;
        unsigned reversed = 0u;
        unsigned value = i;
        unsigned bit;
        for (bit = 0u; bit < 9u; ++bit) {
            reversed = (reversed << 1u) | (value & 1u);
            value >>= 1u;
        }
        fft[reversed] = std::complex<double>(mono, 0.0);
    }
    for (unsigned length = 2u; length <= IPLAY_MODPLUG_FFT_SAMPLES; length <<= 1u) {
        double angle = -two_pi / (double)length;
        std::complex<double> root(std::cos(angle), std::sin(angle));
        for (unsigned start = 0u; start < IPLAY_MODPLUG_FFT_SAMPLES; start += length) {
            std::complex<double> factor(1.0, 0.0);
            for (unsigned offset = 0u; offset < length / 2u; ++offset) {
                std::complex<double> even = fft[start + offset];
                std::complex<double> odd = fft[start + offset + length / 2u] * factor;
                fft[start + offset] = even + odd;
                fft[start + offset + length / 2u] = even - odd;
                factor *= root;
            }
        }
    }
    for (band = 0u; band < IPLAY_MODPLUG_SPECTRUM_BANDS; ++band) {
        {
            double magnitude = std::abs(fft[band + 1u]) / (double)IPLAY_MODPLUG_FFT_SAMPLES;
            unsigned level = (unsigned)(std::log10(1.0 + magnitude) * 20.0);
            if (level > IPLAY_MODPLUG_SPECTRUM_MAX_LEVEL) level = IPLAY_MODPLUG_SPECTRUM_MAX_LEVEL;
            stats->spectrum[band] = (unsigned char)level;
        }
    }
}

static void bridge_update_level_stats(IplayModplugAudioBridgeStats *stats, const IplaySdlAudioDevice *device) {
    const IplayAudioLevels *levels = iplay_sdl_audio_device_levels(device);
    stats->last_left_level = (unsigned)levels->left_16;
    stats->last_right_level = (unsigned)levels->right_16;
    if (stats->last_left_level > stats->max_left_level) stats->max_left_level = stats->last_left_level;
    if (stats->last_right_level > stats->max_right_level) stats->max_right_level = stats->last_right_level;
}

void iplay_modplug_playback_controls_init(IplayModplugPlaybackControls *controls) {
    if (!controls) return;
    controls->volume_percent = 100u;
    controls->volume_256 = 256u;
    controls->loop_enabled = 0u;
    controls->interpolation_enabled = 1u;
    controls->protracker_enabled = 1u;
    controls->ignore_bpm_enabled = 0u;
    controls->pattern_loop_enabled = 0u;
    controls->pattern_loop_order = 0u;
    controls->pal_enabled = 1u;
    controls->amplification_percent = 100u;
    controls->selected_channel = 0u;
    controls->channel_muted_mask = 0u;
    std::memset(controls->channel_pan, 0, sizeof(controls->channel_pan));
    controls->channel_pan_valid_mask = 0u;
    controls->channel_generation = 0u;
    controls->seek_order = 0u;
    controls->seek_row = 0u;
    controls->seek_generation = 0u;
    controls->generation = 0u;
}

static void bridge_apply_controls(IplayModplugPcmSource *source, const IplayModplugPlaybackControls *controls) {
    unsigned effective_percent;
    if (!source || !controls) return;
    effective_percent = (controls->volume_percent * controls->amplification_percent + 50u) / 100u;
    iplay_modplug_pcm_source_set_master_volume(source, effective_percent);
    iplay_modplug_pcm_source_set_interpolation(source, controls->interpolation_enabled != 0u);
    iplay_modplug_pcm_source_set_pal_timing(source, controls->pal_enabled != 0u);
    iplay_modplug_pcm_source_set_mod_compatibility(source, controls->protracker_enabled != 0u, controls->ignore_bpm_enabled != 0u);
}

static void bridge_apply_channel_controls(IplayModplugPcmSource *source, const IplayModplugPlaybackControls *controls) {
    unsigned channel;
    if (!source || !controls) return;
    for (channel = 0u; channel < IPLAY_MODPLUG_CONTROL_MAX_CHANNELS; ++channel) {
        (void)iplay_modplug_pcm_source_set_channel_muted(
            source,
            channel,
            (controls->channel_muted_mask & (1u << channel)) != 0u);
        if (channel < IPLAY_MODPLUG_UI_MAX_CHANNELS &&
            (controls->channel_pan_valid_mask & (1u << channel)) != 0u) {
            (void)iplay_modplug_pcm_source_set_channel_pan(source, channel, controls->channel_pan[channel]);
        }
    }
}

static void bridge_copy_controls(IplayModplugAudioBridgeStats *stats, const IplayModplugPlaybackControls *controls) {
    if (!stats || !controls) return;
    stats->volume_percent = controls->volume_percent;
    stats->loop_enabled = controls->loop_enabled;
    stats->interpolation_enabled = controls->interpolation_enabled;
    stats->protracker_enabled = controls->protracker_enabled;
    stats->ignore_bpm_enabled = controls->ignore_bpm_enabled;
    stats->pattern_loop_enabled = controls->pattern_loop_enabled;
    stats->pattern_loop_order = controls->pattern_loop_order;
    stats->pal_enabled = controls->pal_enabled;
    stats->amplification_percent = controls->amplification_percent;
    stats->selected_channel = controls->selected_channel;
    stats->channel_muted_mask = controls->channel_muted_mask;
    std::memcpy(stats->channel_pan, controls->channel_pan, sizeof(stats->channel_pan));
    stats->channel_pan_valid_mask = controls->channel_pan_valid_mask;
    stats->channel_generation = controls->channel_generation;
}

static void bridge_overlay_channel_controls(IplayModplugAudioBridgeStats *stats, const IplayModplugPlaybackControls *controls) {
    unsigned channel;
    if (!stats || !controls) return;
    for (channel = 0u; channel < stats->ui.channel_count && channel < IPLAY_MODPLUG_UI_MAX_CHANNELS; ++channel) {
        if ((controls->channel_pan_valid_mask & (1u << channel)) != 0u) {
            stats->ui.channels[channel].pan = controls->channel_pan[channel];
            stats->ui.channels[channel].pan_valid = 1u;
        }
    }
}

#if IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER
struct IplayModplugExternalDecoder {
    IplayModplugPcmSource *source;
    std::string path;
    const PlayerModuleInfo *module;
};

static void modplug_external_decoder_close_source(IplayModplugExternalDecoder *decoder) {
    if (!decoder || !decoder->source) return;
    iplay_modplug_pcm_source_close(decoder->source);
    decoder->source = 0;
    decoder->path.clear();
    decoder->module = 0;
}

IplayModplugExternalDecoder *iplay_modplug_external_decoder_create(void) {
    IplayModplugExternalDecoder *decoder = new IplayModplugExternalDecoder;
    decoder->source = 0;
    decoder->module = 0;
    return decoder;
}

void iplay_modplug_external_decoder_destroy(IplayModplugExternalDecoder *decoder) {
    if (!decoder) return;
    modplug_external_decoder_close_source(decoder);
    delete decoder;
}

void iplay_modplug_external_decoder_install(IplayModplugExternalDecoder *decoder) {
    iplay_player_set_external_decoder(iplay_modplug_external_decoder_render, decoder, "libmikmod");
}

void iplay_modplug_external_decoder_uninstall(void) {
    iplay_player_clear_external_decoder();
}

static void modplug_hook_audio_clear_stats(IplayModplugHookAudioStats *stats) {
    stats->blocks = 0ul;
    stats->frames = 0ul;
    stats->accepted_bytes = 0ul;
    stats->capture_calls = 0ul;
    stats->capture_bytes = 0ul;
    stats->capture_checksum = 0ul;
    stats->last_left_level = 0u;
    stats->last_right_level = 0u;
    stats->max_left_level = 0u;
    stats->max_right_level = 0u;
    stats->source_end = 0;
    stats->block_limit = 0;
    stats->last_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
}

typedef struct IplayModplugHookAudioWriteCapture {
    IplayAudioWriteFn write;
    void *user;
    IplayModplugHookAudioStats *stats;
} IplayModplugHookAudioWriteCapture;

static void modplug_hook_audio_write_capture(void *user, const db *pcm, dw byte_count) {
    IplayModplugHookAudioWriteCapture *capture = (IplayModplugHookAudioWriteCapture *)user;
    dw i;
    if (!capture || !capture->stats) return;
    capture->stats->capture_calls += 1ul;
    capture->stats->capture_bytes += (unsigned long)byte_count;
    for (i = 0; i < byte_count; ++i) capture->stats->capture_checksum += (unsigned long)pcm[i];
    if (capture->write) capture->write(capture->user, pcm, byte_count);
}

static bool modplug_external_decoder_open_source(IplayModplugExternalDecoder *decoder, const PlayerModuleInfo *module, const char *path) {
    if (!decoder || !path || !*path) return false;
    if (decoder->source && decoder->module == module && decoder->path == path) return true;
    modplug_external_decoder_close_source(decoder);
    decoder->source = iplay_modplug_pcm_source_open_file(path);
    if (!decoder->source) return false;
    decoder->path = path;
    decoder->module = module;
    return true;
}

int iplay_modplug_external_decoder_render(void *user, const PlayerModuleInfo *module, PlayerPlaybackBlock *block) {
    IplayModplugExternalDecoder *decoder = static_cast<IplayModplugExternalDecoder *>(user);
    IplayModplugPcmStats stats;
    const char *path;
    db *pcm;
    dw frames;

    if (!decoder || !module || !block) return IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    path = iplay_player_module_path(module);
    if (!modplug_external_decoder_open_source(decoder, module, path)) return IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    if (iplay_modplug_pcm_source_ended(decoder->source)) return IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED;

    pcm = iplay_player_playback_block_pcm(block);
    frames = iplay_player_playback_block_frames(block);
    if (!pcm || frames == 0) return IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    if (!iplay_modplug_pcm_source_read(decoder->source, reinterpret_cast<std::int16_t *>(pcm), (int)frames, &stats)) {
        if (iplay_modplug_pcm_source_ended(decoder->source)) return IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED;
        modplug_external_decoder_close_source(decoder);
        return IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    }
    iplay_player_playback_block_set_frames(block, (dw)stats.frames);
    return stats.frames > 0 ? IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED : IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED;
}

bool iplay_modplug_external_decoder_play_module_to_sdl_sb16(IplayModplugExternalDecoder *decoder,
                                                            const PlayerModuleInfo *module,
                                                            PlayerPlaybackBlock *block,
                                                            IplayAudioWriteFn write,
                                                            void *write_user,
                                                            dw frames_per_block,
                                                            unsigned long max_blocks,
                                                            int require_source_end,
                                                            IplayModplugHookAudioStats *stats) {
    IplaySdlAudioDevice device;
    IplayModplugHookAudioWriteCapture capture;
    unsigned long i;
    if (stats) modplug_hook_audio_clear_stats(stats);
    if (!decoder || !module || !block || !stats || frames_per_block == 0 || max_blocks == 0) return false;
    capture.write = write;
    capture.user = write_user;
    capture.stats = stats;
    iplay_sdl_audio_device_init_sb16_compatible(&device, modplug_hook_audio_write_capture, &capture);
    iplay_sdl_audio_device_start(&device);
    for (i = 0; i < max_blocks; ++i) {
        iplay_player_playback_block_set_frames(block, frames_per_block);
        stats->last_status = iplay_modplug_external_decoder_render(decoder, module, block);
        if (stats->last_status == IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED) {
            stats->source_end = 1;
            break;
        }
        if (stats->last_status != IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED) break;
        iplay_sdl_audio_device_set_capacity(&device, (dd)iplay_player_playback_block_frames(block));
        stats->accepted_bytes += (unsigned long)iplay_sdl_audio_device_write_sb16_frames(&device, iplay_player_playback_block_pcm(block), iplay_player_playback_block_frames(block));
        stats->frames += (unsigned long)iplay_player_playback_block_frames(block);
        stats->blocks += 1ul;
        stats->last_left_level = (unsigned)iplay_sdl_audio_device_levels(&device)->left_16;
        stats->last_right_level = (unsigned)iplay_sdl_audio_device_levels(&device)->right_16;
        if (stats->last_left_level > stats->max_left_level) stats->max_left_level = stats->last_left_level;
        if (stats->last_right_level > stats->max_right_level) stats->max_right_level = stats->last_right_level;
    }
    if (!stats->source_end && stats->last_status == IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED && stats->blocks == max_blocks) {
        stats->block_limit = 1;
    }
    return stats->blocks != 0ul
        && stats->frames != 0ul
        && stats->accepted_bytes == stats->frames * 4ul
        && stats->capture_calls == stats->blocks
        && stats->capture_bytes == stats->accepted_bytes
        && stats->capture_checksum != 0ul
        && (stats->max_left_level != 0u || stats->max_right_level != 0u)
        && (!require_source_end || (stats->source_end && stats->last_status == IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED));
}
#endif

bool iplay_modplug_audio_play_file_to_sdl_sb16(const char *path,
                                               IplayAudioWriteFn write,
                                               void *write_user,
                                               int frames_per_block,
                                               int max_blocks,
                                               IplayModplugAudioBridgeStats *stats) {
    return iplay_modplug_audio_play_file_to_sdl_sb16_until(path, write, write_user, frames_per_block, max_blocks, 0, 0, stats);
}

bool iplay_modplug_audio_play_file_to_sdl_sb16_until(const char *path,
                                                     IplayAudioWriteFn write,
                                                     void *write_user,
                                                     int frames_per_block,
                                                     int max_blocks,
                                                     IplayModplugAudioStopFn stop,
                                                     void *stop_user,
                                                     IplayModplugAudioBridgeStats *stats) {
    return iplay_modplug_audio_play_file_to_sdl_sb16_controlled(path, write, write_user, frames_per_block, max_blocks, stop, stop_user, 0, stats);
}

bool iplay_modplug_audio_play_file_to_sdl_sb16_controlled(const char *path,
                                                          IplayAudioWriteFn write,
                                                          void *write_user,
                                                          int frames_per_block,
                                                          int max_blocks,
                                                          IplayModplugAudioStopFn stop,
                                                          void *stop_user,
                                                          IplayModplugPlaybackControls *controls,
                                                          IplayModplugAudioBridgeStats *stats) {
    IplayModplugPcmSource *source;
    IplayModplugPcmStats block;
    IplaySdlAudioDevice device;
    std::vector<std::int16_t> pcm;
    int blocks_left;
    unsigned applied_generation = controls ? controls->generation : 0u;
    unsigned applied_seek_generation = controls ? controls->seek_generation : 0u;
    unsigned applied_channel_generation = controls ? controls->channel_generation : 0u;

    if (!stats || !path || frames_per_block <= 0 || max_blocks <= 0) return false;
    bridge_clear_stats(stats);

    source = iplay_modplug_pcm_source_open_file(path);
    if (!source) return false;
    bridge_apply_controls(source, controls);
    bridge_apply_channel_controls(source, controls);
    bridge_copy_controls(stats, controls);

    iplay_sdl_audio_device_init_sb16_compatible(&device, write, write_user);
    if (!iplay_sdl_audio_device_is_sb16_compatible(&device) || !iplay_sdl_audio_device_is_sdl_compatible(&device)) {
        iplay_modplug_pcm_source_close(source);
        return false;
    }
    iplay_sdl_audio_device_start(&device);

    pcm.resize((std::size_t)frames_per_block * 2u);
    blocks_left = max_blocks;
    while (blocks_left > 0) {
        dw accepted;
        if (iplay_modplug_pcm_source_ended(source)) {
            if (!controls || !controls->loop_enabled || !iplay_modplug_pcm_source_restart(source)) break;
            bridge_apply_controls(source, controls);
        }
        if (!iplay_modplug_pcm_source_read(source, pcm.data(), frames_per_block, &block)) continue;
        if (controls && controls->pattern_loop_enabled && block.ui.order != controls->pattern_loop_order) {
            if (!iplay_modplug_pcm_source_seek_position(source, controls->pattern_loop_order, 0u) ||
                !iplay_modplug_pcm_source_read(source, pcm.data(), frames_per_block, &block)) {
                continue;
            }
        }
        --blocks_left;
        bridge_update_visualization(stats, pcm.data(), block.frames);
        iplay_sdl_audio_device_set_capacity(&device, (dd)block.frames);
        accepted = iplay_sdl_audio_device_write_sb16_frames(&device, reinterpret_cast<const db *>(pcm.data()), (dw)block.frames);
        bridge_update_level_stats(stats, &device);
        stats->source_frames += (unsigned long)block.frames;
        stats->accepted_bytes += (unsigned long)accepted;
        stats->source_checksum += (unsigned long)block.checksum;
        stats->blocks += 1ul;
        stats->ui = block.ui;
        bridge_overlay_channel_controls(stats, controls);
        if (stop && stop(stop_user, stats)) {
            stats->frames_written = (unsigned long)iplay_sdl_audio_device_frames_written(&device);
            stats->dropped_frames = (unsigned long)iplay_sdl_audio_device_dropped_frames(&device);
            stats->active = (unsigned)iplay_sdl_audio_device_active(&device);
            stats->source_ended = 0u;
            stats->stop_reason = "keyboard";
            iplay_modplug_pcm_source_close(source);
            return false;
        }
        if (controls && controls->generation != applied_generation) {
            bridge_apply_controls(source, controls);
            bridge_copy_controls(stats, controls);
            applied_generation = controls->generation;
        }
        if (controls && controls->seek_generation != applied_seek_generation) {
            (void)iplay_modplug_pcm_source_seek_position(source, controls->seek_order, controls->seek_row);
            applied_seek_generation = controls->seek_generation;
        }
        if (controls && controls->channel_generation != applied_channel_generation) {
            bridge_apply_channel_controls(source, controls);
            bridge_copy_controls(stats, controls);
            applied_channel_generation = controls->channel_generation;
        }
    }

    if (!iplay_modplug_pcm_source_ended(source) || (controls && controls->loop_enabled)) {
        stats->stop_reason = "block-limit";
        stats->frames_written = (unsigned long)iplay_sdl_audio_device_frames_written(&device);
        stats->dropped_frames = (unsigned long)iplay_sdl_audio_device_dropped_frames(&device);
        stats->active = (unsigned)iplay_sdl_audio_device_active(&device);
        stats->source_ended = 0u;
        iplay_modplug_pcm_source_close(source);
        return false;
    }

    stats->frames_written = (unsigned long)iplay_sdl_audio_device_frames_written(&device);
    stats->dropped_frames = (unsigned long)iplay_sdl_audio_device_dropped_frames(&device);
    stats->active = (unsigned)iplay_sdl_audio_device_active(&device);
    stats->source_ended = 1u;
    stats->stop_reason = "source-end";
    iplay_modplug_pcm_source_close(source);
    return stats->blocks != 0ul;
}
