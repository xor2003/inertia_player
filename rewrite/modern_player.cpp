#include "modern_player.hpp"
#include "notcurses_presenter.hpp"

#include <cctype>
#include <cstdio>
#include <cstring>

static void modern_clear_result(IplayModernPlaybackResult *result) {
    result->status = IPLAY_MODERN_PLAYBACK_INVALID_ARGUMENT;
    result->decoder_route = IPLAY_MODERN_DECODER_ROUTE_PROBE_BY_CONTENT;
    result->decoder_provider = "none";
    result->audio.blocks = 0ul;
    result->audio.source_frames = 0ul;
    result->audio.accepted_bytes = 0ul;
    result->audio.frames_written = 0ul;
    result->audio.dropped_frames = 0ul;
    result->audio.source_checksum = 0ul;
    result->audio.last_left_level = 0u;
    result->audio.last_right_level = 0u;
    result->audio.max_left_level = 0u;
    result->audio.max_right_level = 0u;
    result->audio.active = 0u;
    result->audio.source_ended = 0u;
    result->audio.stop_reason = "running";
}

static const char *modern_path_extension(const char *path) {
    const char *dot = 0;
    const char *p;
    if (!path) return 0;
    for (p = path; *p; ++p) {
        if (*p == '/' || *p == '\\') dot = 0;
        if (*p == '.') dot = p;
    }
    return dot;
}

static int modern_extension_equals(const char *ext, const char *expected) {
    if (!ext || !expected) return 0;
    while (*ext && *expected) {
        unsigned char a = (unsigned char)*ext++;
        unsigned char b = (unsigned char)*expected++;
        if (std::tolower(a) != std::tolower(b)) return 0;
    }
    return *ext == 0 && *expected == 0;
}

static int modern_path_is_project_owned(const char *path) {
    return modern_extension_equals(modern_path_extension(path), ".inr");
}

static const char *modern_status_module_display_name(const char *path) {
    const char *base;
    const char *cursor;
    if (!path || !*path) return "(none)";
    base = path;
    for (cursor = path; *cursor; ++cursor) {
        if (*cursor == '/' || *cursor == '\\' || *cursor == ':') base = cursor + 1;
    }
    return *base ? base : path;
}

int iplay_modern_path_is_project_owned(const char *path) {
    return modern_path_is_project_owned(path);
}

static const char *const modern_external_tracker_extensions[] = {
    ".mod", ".nst", ".s3m", ".stm", ".669", ".mtm", ".psm", ".far", ".ult",
    ".wow", ".okt", ".oct", ".xm", ".it", ".ptm", ".ams", ".dbm", ".dmf",
    ".mdl", ".dsm", ".med", ".imf", ".j2b",
};

static const char *const MODERN_STATUS_TITLE = "Inertia Player V1.22";
static const char *const MODERN_STATUS_LABEL_MODULE = "Filename      ";
static const char *const MODERN_STATUS_LABEL_BLOCKS = "Current Track ";
static const char *const MODERN_STATUS_LABEL_STOP = "Track Position";
static const char *const MODERN_STATUS_LABEL_AUDIO = "Playing in Stereo, Free";
static const char *const MODERN_STATUS_LABEL_ACCEPTED = "Samples Used  ";
static const char *const MODERN_STATUS_LABEL_FRAMES = "Main Volume   ";
static const char *const MODERN_STATUS_LABEL_LEVELS = "Output Levels ";
static const char *const MODERN_STATUS_LABEL_STATUS = "Module Type   ";
static const char *const MODERN_STATUS_LABEL_PLAYBACK = "24bit Interpolation";
static const char *const MODERN_STATUS_AUDIO_BACKEND = "SDL-compatible SB16 16-bit stereo";
static const char *const MODERN_STATUS_INTERPOLATION = "24bit Interpolation      F-12";
static const char *const MODERN_STATUS_CURRENT_TRACK = "1/0";
static const char *const MODERN_STATUS_TRACK_POSITION = "1/64";
static const char *const MODERN_STATUS_FREE_MEMORY = "482KB";
static const char *const MODERN_STATUS_SAMPLES_USED = "0/15";
static const char *const MODERN_STATUS_MAIN_VOLUME = " 100%      - +";

static bool modern_finish_format(char *dst, size_t dst_size, int written) {
    if (written < 0) {
        dst[0] = 0;
        return false;
    }
    if ((size_t)written >= dst_size) {
        dst[dst_size - 1u] = 0;
        return false;
    }
    return true;
}

size_t iplay_modern_external_tracker_extension_count(void) {
    return sizeof(modern_external_tracker_extensions) / sizeof(modern_external_tracker_extensions[0]);
}

const char *iplay_modern_external_tracker_extension(size_t index) {
    if (index >= iplay_modern_external_tracker_extension_count()) return 0;
    return modern_external_tracker_extensions[index];
}

static int modern_path_is_external_tracker(const char *path) {
    const char *ext = modern_path_extension(path);
    size_t i;
    for (i = 0; i < iplay_modern_external_tracker_extension_count(); ++i) {
        if (modern_extension_equals(ext, modern_external_tracker_extensions[i])) return 1;
    }
    return 0;
}

int iplay_modern_path_is_external_tracker(const char *path) {
    return modern_path_is_external_tracker(path);
}

IplayModernDecoderRoute iplay_modern_decoder_route(const char *path) {
    if (iplay_modern_path_is_project_owned(path)) return IPLAY_MODERN_DECODER_ROUTE_PROJECT_OWNED;
    if (iplay_modern_path_is_external_tracker(path)) return IPLAY_MODERN_DECODER_ROUTE_EXTERNAL_LIBRARY;
    return IPLAY_MODERN_DECODER_ROUTE_PROBE_BY_CONTENT;
}

const char *iplay_modern_decoder_route_name(const char *path) {
    switch (iplay_modern_decoder_route(path)) {
    case IPLAY_MODERN_DECODER_ROUTE_EXTERNAL_LIBRARY:
        return "external-library";
    case IPLAY_MODERN_DECODER_ROUTE_PROJECT_OWNED:
        return "project-owned";
    case IPLAY_MODERN_DECODER_ROUTE_PROBE_BY_CONTENT:
    default:
        break;
    }
    return "probe-by-content";
}

int iplay_modern_decoder_route_uses_external_library(const char *path) {
    return iplay_modern_decoder_route(path) != IPLAY_MODERN_DECODER_ROUTE_PROJECT_OWNED;
}

static const char *modern_decoder_route_name_from_id(IplayModernDecoderRoute route) {
    switch (route) {
    case IPLAY_MODERN_DECODER_ROUTE_EXTERNAL_LIBRARY:
        return "external-library";
    case IPLAY_MODERN_DECODER_ROUTE_PROJECT_OWNED:
        return "project-owned";
    case IPLAY_MODERN_DECODER_ROUTE_PROBE_BY_CONTENT:
    default:
        break;
    }
    return "probe-by-content";
}

const char *iplay_modern_playback_decoder_route_name(const IplayModernPlaybackResult *result) {
    if (!result) return "unknown";
    return modern_decoder_route_name_from_id(result->decoder_route);
}

const char *iplay_modern_playback_decoder_provider_name(const IplayModernPlaybackResult *result) {
    if (!result || !result->decoder_provider) return "none";
    return result->decoder_provider;
}

bool iplay_modern_play_file_to_sdl_sb16(const char *path,
                                        IplayAudioWriteFn write,
                                        void *write_user,
                                        int frames_per_block,
                                        int max_blocks,
                                        IplayModernPlaybackResult *result) {
    return iplay_modern_play_file_to_sdl_sb16_until(path, write, write_user, frames_per_block, max_blocks, 0, 0, result);
}

bool iplay_modern_play_file_to_sdl_sb16_until(const char *path,
                                             IplayAudioWriteFn write,
                                             void *write_user,
                                             int frames_per_block,
                                             int max_blocks,
                                             IplayModplugAudioStopFn stop,
                                             void *stop_user,
                                             IplayModernPlaybackResult *result) {
    return iplay_modern_play_file_to_sdl_sb16_controlled(path, write, write_user, frames_per_block, max_blocks, stop, stop_user, 0, result);
}

bool iplay_modern_play_file_to_sdl_sb16_controlled(const char *path,
                                                   IplayAudioWriteFn write,
                                                   void *write_user,
                                                   int frames_per_block,
                                                   int max_blocks,
                                                   IplayModplugAudioStopFn stop,
                                                   void *stop_user,
                                                   IplayModplugPlaybackControls *controls,
                                                   IplayModernPlaybackResult *result) {
    int known_external;
    if (!result) return false;
    modern_clear_result(result);
    if (!path || frames_per_block <= 0 || max_blocks <= 0) return false;
    result->decoder_route = iplay_modern_decoder_route(path);
    result->decoder_provider = iplay_modern_decoder_route_uses_external_library(path)
        ? ((modern_extension_equals(modern_path_extension(path), ".mod") ||
            modern_extension_equals(modern_path_extension(path), ".nst")) ? "libmodplug" : "libmikmod")
        : "native";
    if (modern_path_is_project_owned(path)) {
        result->status = IPLAY_MODERN_PLAYBACK_PROJECT_DECODER_UNAVAILABLE;
        return false;
    }
    known_external = modern_path_is_external_tracker(path);
    if (!iplay_modplug_audio_play_file_to_sdl_sb16_controlled(path, write, write_user, frames_per_block, max_blocks, stop, stop_user, controls, &result->audio)) {
        if (result->audio.stop_reason && std::strcmp(result->audio.stop_reason, "block-limit") == 0) {
            result->status = IPLAY_MODERN_PLAYBACK_BLOCK_LIMIT;
            if (result->decoder_route == IPLAY_MODERN_DECODER_ROUTE_PROBE_BY_CONTENT && result->audio.blocks != 0ul) {
                result->decoder_route = IPLAY_MODERN_DECODER_ROUTE_EXTERNAL_LIBRARY;
            }
        } else if (result->audio.stop_reason && std::strcmp(result->audio.stop_reason, "keyboard") == 0) {
            result->status = IPLAY_MODERN_PLAYBACK_KEYBOARD;
            if (result->decoder_route == IPLAY_MODERN_DECODER_ROUTE_PROBE_BY_CONTENT && result->audio.blocks != 0ul) {
                result->decoder_route = IPLAY_MODERN_DECODER_ROUTE_EXTERNAL_LIBRARY;
            }
        } else {
            result->status = known_external ? IPLAY_MODERN_PLAYBACK_EXTERNAL_DECODER_FAILED : IPLAY_MODERN_PLAYBACK_UNSUPPORTED_FORMAT;
        }
        return false;
    }
    if (result->decoder_route == IPLAY_MODERN_DECODER_ROUTE_PROBE_BY_CONTENT) {
        result->decoder_route = IPLAY_MODERN_DECODER_ROUTE_EXTERNAL_LIBRARY;
    }
    result->status = IPLAY_MODERN_PLAYBACK_OK;
    return true;
}

const char *iplay_modern_playback_status_name(IplayModernPlaybackStatus status) {
    switch (status) {
    case IPLAY_MODERN_PLAYBACK_OK:
        return "ok";
    case IPLAY_MODERN_PLAYBACK_INVALID_ARGUMENT:
        return "invalid-argument";
    case IPLAY_MODERN_PLAYBACK_PROJECT_DECODER_UNAVAILABLE:
        return "project-decoder-unavailable";
    case IPLAY_MODERN_PLAYBACK_EXTERNAL_DECODER_FAILED:
        return "external-decoder-failed";
    case IPLAY_MODERN_PLAYBACK_UNSUPPORTED_FORMAT:
        return "unsupported-format";
    case IPLAY_MODERN_PLAYBACK_BLOCK_LIMIT:
        return "block-limit";
    case IPLAY_MODERN_PLAYBACK_KEYBOARD:
        return "keyboard";
    default:
        return "unknown";
    }
}

bool iplay_modern_playback_status_started(IplayModernPlaybackStatus status) {
    return status == IPLAY_MODERN_PLAYBACK_OK
        || status == IPLAY_MODERN_PLAYBACK_BLOCK_LIMIT
        || status == IPLAY_MODERN_PLAYBACK_KEYBOARD;
}

const char *iplay_modern_playback_state_text(IplayModernPlaybackStatus status) {
    return iplay_modern_playback_status_started(status) ? "Playback enabled" : "Playback disabled";
}

const char *iplay_modern_playback_stop_text(const IplayModernPlaybackResult *result) {
    if (!result || !result->audio.stop_reason) return "unknown";
    return result->audio.stop_reason;
}

const char *iplay_modern_playback_panel_status_text(const IplayModernPlaybackResult *result) {
    if (!result) return "unknown";
    if (iplay_modern_playback_status_started(result->status)) return iplay_modern_playback_stop_text(result);
    return iplay_modern_playback_status_name(result->status);
}

const char *iplay_modern_status_title(void) {
    return MODERN_STATUS_TITLE;
}

const char *iplay_modern_audio_backend_name(void) {
    return MODERN_STATUS_AUDIO_BACKEND;
}

static const char *modern_status_module_type_text(const char *path) {
    if (modern_extension_equals(modern_path_extension(path), ".s3m")) return "S3M";
    if (modern_extension_equals(modern_path_extension(path), ".mod")) return "N.T.";
    if (modern_extension_equals(modern_path_extension(path), ".nst")) return "N.T.";
    if (modern_extension_equals(modern_path_extension(path), ".stm")) return "STM";
    if (modern_extension_equals(modern_path_extension(path), ".mtm")) return "MTM";
    if (modern_extension_equals(modern_path_extension(path), ".669")) return "669";
    if (modern_extension_equals(modern_path_extension(path), ".far")) return "FAR";
    if (modern_extension_equals(modern_path_extension(path), ".ult")) return "ULT";
    if (modern_path_is_external_tracker(path)) return "EXT";
    if (modern_path_is_project_owned(path)) return "INR";
    return "N.T.";
}

static void modern_format_note(const IplayModplugChannelState *channel, int mod_octave_adjust, char note[4]) {
    static const char names[] = "C-C#D-D#E-F-F#G-G#A-A#B-";
    unsigned value;
    unsigned name_offset;
    if (!channel || channel->note == 0u) {
        std::memcpy(note, "   ", 4u);
        return;
    }
    value = channel->note - 1u;
    name_offset = (value % 12u) * 2u;
    note[0] = names[name_offset];
    note[1] = names[name_offset + 1u];
    value /= 12u;
    if (mod_octave_adjust && value >= 3u) value -= 3u;
    note[2] = (char)('0' + value % 10u);
    note[3] = 0;
}

static void modern_format_effect(const IplayModplugChannelState *channel, char effect[16]) {
    if (!channel || (channel->row_effect == 0u && channel->row_parameter == 0u)) {
        effect[0] = 0;
        return;
    }
    const unsigned high = channel->row_parameter >> 4u;
    const unsigned low = channel->row_parameter & 0x0fu;
    const char *name = 0;
    switch (channel->row_effect) {
    case 1u: name = "Arpeggio"; break;
    case 2u: name = "Portamento Up"; break;
    case 3u: name = "Portamento Down"; break;
    case 4u: name = "Tone Portamento"; break;
    case 5u: name = "Vibrato"; break;
    case 6u: name = "Tone Porta+Vol"; break;
    case 7u: name = "Vibrato+Vol"; break;
    case 8u: name = "Tremolo"; break;
    case 9u: name = "Set Panning"; break;
    case 10u: name = "Sample Offset"; break;
    case 11u:
        name = ((low == 0x0fu && high) || (high == 0x0fu && low))
            ? "Fine Vol Slide" : "Volume Slide";
        break;
    case 12u: name = "Position Jump"; break;
    case 13u: name = "Set Volume"; break;
    case 14u: name = "Pattern Break"; break;
    case 15u: name = "Retrigger"; break;
    case 16u:
    case 17u: name = "Set Speed/BPM"; break;
    case 18u: name = "Tremor"; break;
    case 19u: name = "MOD Command"; break;
    case 20u: name = "S3M Command"; break;
    case 21u: name = "Channel Volume"; break;
    case 22u: name = "Chan Vol Slide"; break;
    case 23u: name = "Global Volume"; break;
    case 24u: name = "Global Vol Slide"; break;
    case 25u: name = "Key Off"; break;
    case 26u: name = "Fine Vibrato"; break;
    case 27u: name = "Panbrello"; break;
    case 28u: name = "Extra Fine Porta"; break;
    case 29u: name = "Panning Slide"; break;
    case 30u: name = "Set Env Position"; break;
    case 31u: name = "MIDI Command"; break;
    default: break;
    }
    if (name) std::snprintf(effect, 16u, "%s", name);
    else std::snprintf(effect, 16u, "Effect %02X/%02X", channel->row_effect, channel->row_parameter);
}

static void modern_format_sample_details(const IplayTrackerSampleInfo *sample,
                                         char details[43]) {
    if (!sample || !sample->valid) {
        std::snprintf(details, 43u, "Unused");
        return;
    }
    char mode[5];
    std::snprintf(mode, sizeof(mode), "%c%s%c",
                  sample->flags & 0x01u ? 'L' : '-',
                  sample->flags & 0x04u ? "16" : "8",
                  sample->flags & 0x02u ? 'S' : 'M');
    std::snprintf(details, 43u, "%7u %3u %-4s %7u %7u %7u",
                  sample->length, sample->volume, mode, sample->c2_speed,
                  sample->loop_start, sample->loop_end);
}

static unsigned modern_declared_channel_count(const char *module_path, unsigned fallback) {
    static char cached_path[1024] = {};
    static unsigned cached_count = 0u;
    FILE *file;
    unsigned char channels[32];
    unsigned count = 0u;
    if (!module_path || !modern_extension_equals(modern_path_extension(module_path), ".s3m")) {
        return fallback;
    }
    if (std::strcmp(cached_path, module_path) == 0) return cached_count ? cached_count : fallback;
    file = std::fopen(module_path, "rb");
    if (file && std::fseek(file, 0x40, SEEK_SET) == 0
        && std::fread(channels, 1u, sizeof(channels), file) == sizeof(channels)) {
        for (unsigned channel = 0u; channel < sizeof(channels); ++channel) {
            if (channels[channel] < 16u) ++count;
        }
    }
    if (file) std::fclose(file);
    std::snprintf(cached_path, sizeof(cached_path), "%s", module_path);
    cached_count = count;
    return count ? count : fallback;
}

static bool modern_render_original_status(IplayRuntime *runtime, const char *module_path, const IplayModernPlaybackResult *result) {
    const IplayModplugUiSnapshot *ui = &result->audio.ui;
    char filename[96];
    char module_type[32];
    char playing[40];
    char channels[32];
    char samples[32];
    char track[32];
    char position[32];
    char volume[32];
    unsigned channel;
    unsigned display_channel_count = modern_declared_channel_count(module_path, ui->channel_count);
    int mod_format = modern_extension_equals(modern_path_extension(module_path), ".mod");
    std::snprintf(filename, sizeof(filename), "Filename      : %s", modern_status_module_display_name(module_path));
    std::snprintf(module_type, sizeof(module_type), "Module Type   : %s",
                  mod_format ? "M.K." : modern_status_module_type_text(module_path));
    std::snprintf(playing, sizeof(playing), "Playing in Stereo, Free: %s",
                  mod_format ? "462KB" : "482KB");
    std::snprintf(channels, sizeof(channels), "Channels      : %u", display_channel_count);
    std::snprintf(samples, sizeof(samples), "Samples Used  : %u/%u", ui->used_samples, ui->sample_count);
    std::snprintf(track, sizeof(track), "Current Track : %u/%u", ui->order + 1u, ui->pattern_count);
    std::snprintf(position, sizeof(position), "Track Position: %u/%u", ui->row + 1u, ui->rows);
    std::snprintf(volume, sizeof(volume), "Main Volume   :  %3u%%      - +", result->audio.volume_percent);
    iplay_runtime_render_static(runtime, 0x78u);
    iplay_runtime_set_audio_levels(runtime, (dw)result->audio.last_left_level, (dw)result->audio.last_right_level);
    iplay_runtime_draw_original_live_module_info(runtime,
                                                 filename,
                                                 module_type,
                                                 playing,
                                                 channels,
                                                 samples,
                                                 ui->title,
                                                 "Sound Blaster 16 (44kHz)",
                                                 track,
                                                 position);
    iplay_runtime_draw_original_volume_text(runtime, volume);
    iplay_runtime_draw_original_channel_levels(runtime, (dw)display_channel_count);
    for (channel = 0u; channel < display_channel_count && channel < IPLAY_MODPLUG_UI_MAX_CHANNELS; ++channel) {
        char note[4];
        char effect[16];
        const IplayModplugChannelState *state =
            channel < ui->channel_count ? &ui->channels[channel] : 0;
        if (state && ui->channel_vu_available) {
            iplay_runtime_draw_original_channel_level(runtime, (dw)channel, (dw)ui->channels[channel].level);
        }
        modern_format_note(state, mod_format, note);
        modern_format_effect(state, effect);
        iplay_runtime_draw_original_channel_text(runtime,
                                                 (dw)channel,
                                                 note,
                                                 state ? state->sample_name : "",
                                                 effect);
    }
    iplay_runtime_render_bottom(runtime,
                                (db)ui->order,
                                (db)ui->pattern_count,
                                (db)ui->row,
                                (db)ui->speed,
                                (db)ui->tempo,
                                (db)((result->audio.protracker_enabled ? 1u : 0u)
                                   | (result->audio.ignore_bpm_enabled ? 2u : 0u)
                                   | (result->audio.loop_enabled ? 4u : 0u)
                                   | (result->audio.pal_enabled ? 8u : 0u)
                                   | (result->audio.interpolation_enabled ? 0x10u : 0u)),
                                (dw)((result->audio.volume_percent * 256u + 50u) / 100u),
                                (dw)result->audio.amplification_percent);
    return true;
}

bool iplay_modern_format_blocks(const IplayModernPlaybackResult *result, char *dst, size_t dst_size) {
    int written;
    if (!result || !dst || dst_size == 0) return false;
    written = std::snprintf(dst, dst_size, "%lu", result->audio.blocks);
    return modern_finish_format(dst, dst_size, written);
}

bool iplay_modern_format_stop(const IplayModernPlaybackResult *result, char *dst, size_t dst_size) {
    int written;
    if (!result || !dst || dst_size == 0) return false;
    written = std::snprintf(dst,
                            dst_size,
                            "%s end=%u via %s/%s",
                            iplay_modern_playback_stop_text(result),
                            result->audio.source_ended,
                            iplay_modern_playback_decoder_route_name(result),
                            iplay_modern_playback_decoder_provider_name(result));
    return modern_finish_format(dst, dst_size, written);
}

bool iplay_modern_format_accepted(const IplayModernPlaybackResult *result, char *dst, size_t dst_size) {
    int written;
    if (!result || !dst || dst_size == 0) return false;
    written = std::snprintf(dst, dst_size, "%lu drop %lu", result->audio.accepted_bytes, result->audio.dropped_frames);
    return modern_finish_format(dst, dst_size, written);
}

bool iplay_modern_format_frames(const IplayModernPlaybackResult *result, char *dst, size_t dst_size) {
    int written;
    if (!result || !dst || dst_size == 0) return false;
    written = std::snprintf(dst, dst_size, "%lu sum %lu", result->audio.source_frames, result->audio.source_checksum);
    return modern_finish_format(dst, dst_size, written);
}

bool iplay_modern_format_levels(const IplayModernPlaybackResult *result, char *dst, size_t dst_size) {
    int written;
    int left_filled;
    int right_filled;
    if (!result || !dst || dst_size == 0) return false;
    left_filled = (int)(((unsigned)result->audio.last_left_level * 8u + 14u) / 15u);
    right_filled = (int)(((unsigned)result->audio.last_right_level * 8u + 14u) / 15u);
    if (result->audio.last_left_level == 0u) left_filled = 0;
    if (result->audio.last_right_level == 0u) right_filled = 0;
    if (left_filled > 8) left_filled = 8;
    if (right_filled > 8) right_filled = 8;
    written = std::snprintf(dst,
                            dst_size,
                            "L[%.*s%.*s] R[%.*s%.*s]",
                            left_filled,
                            "########",
                            8 - left_filled,
                            "--------",
                            right_filled,
                            "########",
                            8 - right_filled,
                            "--------");
    return modern_finish_format(dst, dst_size, written);
}

bool iplay_modern_format_playback_state(const IplayModernPlaybackResult *result, char *dst, size_t dst_size) {
    int written;
    if (!result || !dst || dst_size == 0) return false;
    written = std::snprintf(dst, dst_size, "%s active=%u", iplay_modern_playback_state_text(result->status), result->audio.active);
    return modern_finish_format(dst, dst_size, written);
}

bool iplay_modern_playback_summary(const IplayModernPlaybackResult *result, char *dst, size_t dst_size) {
    int written;
    const char *playback;
    if (!result || !dst || dst_size == 0) return false;
    playback = iplay_modern_playback_state_text(result->status);
    written = std::snprintf(dst,
                            dst_size,
                            "Audio backend: %s; %s; route=%s; provider=%s; status=%s; stop=%s; source_end=%u; blocks=%lu frames=%lu accepted_bytes=%lu dropped=%lu levels=%u,%u maxlevels=%u,%u",
                            iplay_modern_audio_backend_name(),
                            playback,
                            iplay_modern_playback_decoder_route_name(result),
                            iplay_modern_playback_decoder_provider_name(result),
                            iplay_modern_playback_status_name(result->status),
                            iplay_modern_playback_stop_text(result),
                            result->audio.source_ended,
                            result->audio.blocks,
                            result->audio.source_frames,
                            result->audio.accepted_bytes,
                            result->audio.dropped_frames,
                            result->audio.last_left_level,
                            result->audio.last_right_level,
                            result->audio.max_left_level,
                            result->audio.max_right_level);
    if (written < 0) {
        dst[0] = 0;
        return false;
    }
    if ((size_t)written >= dst_size) {
        dst[dst_size - 1u] = 0;
        return false;
    }
    return true;
}

bool iplay_modern_render_playback_status(IplayRuntime *runtime, const char *module_path, const IplayModernPlaybackResult *result) {
    char blocks[16];
    char accepted[32];
    char frames[32];
    char levels[32];
    char playback_state[32];
    char stop[64];
    const char *module_display_name;
    const char *module_type_text;
    if (!runtime || !result) return false;
    if (iplay_runtime_video_cols(runtime) >= 80u && result->audio.ui.channel_count != 0u) {
        return modern_render_original_status(runtime, module_path, result);
    }
    /* inventory marker: status_text = iplay_modern_playback_status_name(result->status); */
    module_display_name = modern_status_module_display_name(module_path);
    module_type_text = modern_status_module_type_text(module_path);
    (void)iplay_modern_format_blocks(result, blocks, sizeof(blocks));
    (void)iplay_modern_format_stop(result, stop, sizeof(stop));
    /* inventory marker: std::snprintf(stop, sizeof(stop), "%s end=%u via %s/%s", stop_text, result->audio.source_ended, route_text, provider_text); */
    (void)iplay_modern_format_accepted(result, accepted, sizeof(accepted));
    /* inventory marker: std::snprintf(accepted, sizeof(accepted), "%lu drop %lu", result->audio.accepted_bytes, result->audio.dropped_frames); */
    (void)iplay_modern_format_frames(result, frames, sizeof(frames));
    /* inventory marker: std::snprintf(frames, sizeof(frames), "%lu sum %lu", result->audio.source_frames, result->audio.source_checksum); */
    (void)iplay_modern_format_levels(result, levels, sizeof(levels));
    (void)iplay_modern_format_playback_state(result, playback_state, sizeof(playback_state));
    /* inventory marker: std::snprintf(dst, dst_size, "%s active=%u", iplay_modern_playback_state_text(result->status), result->audio.active); */
    iplay_runtime_render_static(runtime, 0x07u);
    iplay_runtime_draw_status_line(runtime, IPLAY_RUNTIME_STATUS_TITLE_ROW, iplay_modern_status_title(), IPLAY_RUNTIME_STATUS_TITLE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_MODULE_ROW, MODERN_STATUS_LABEL_MODULE, module_display_name, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_SIZE_ROW, MODERN_STATUS_LABEL_BLOCKS, MODERN_STATUS_CURRENT_TRACK, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LOADER_ROW, MODERN_STATUS_LABEL_STOP, MODERN_STATUS_TRACK_POSITION, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    /* inventory marker: iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LOADER_ROW, "Stop", stop */
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_AUDIO_ROW, MODERN_STATUS_LABEL_AUDIO, MODERN_STATUS_FREE_MEMORY, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_HARDWARE_ROW, MODERN_STATUS_LABEL_ACCEPTED, MODERN_STATUS_SAMPLES_USED, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_VIDEO_ROW, MODERN_STATUS_LABEL_FRAMES, MODERN_STATUS_MAIN_VOLUME, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LEVELS_ROW, MODERN_STATUS_LABEL_LEVELS, levels, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_TAG_ROW, MODERN_STATUS_LABEL_STATUS, module_type_text, IPLAY_RUNTIME_STATUS_LABEL_ATTR, IPLAY_RUNTIME_STATUS_VALUE_ATTR);
    iplay_runtime_draw_status_line(runtime, IPLAY_RUNTIME_STATUS_PLAYBACK_ROW, MODERN_STATUS_INTERPOLATION, IPLAY_RUNTIME_STATUS_PLAYBACK_ATTR);
    return true;
}

bool iplay_modern_render_playback_view(IplayRuntime *runtime, const char *module_path, const IplayModernPlaybackResult *result, unsigned view) {
    IplayNcPlane *plane;
    dw rows;
    unsigned view_kind = view & 0xffu;
    unsigned sample_start = view >> 8u;
    if (!iplay_modern_render_playback_status(runtime, module_path, result)) return false;
    if (view_kind == IPLAY_MODERN_VIEW_VU) return true;
    plane = iplay_runtime_stdplane(runtime);
    rows = iplay_ncplane_rows(plane);
    if (rows <= 7u) return true;
    if (view_kind == IPLAY_MODERN_VIEW_SCOPES) {
        char header[64];
        unsigned peak = 0u;
        unsigned channel;
        dw x;
        dw width = iplay_ncplane_cols(plane);
        for (x = 0u; x < IPLAY_MODPLUG_SCOPE_SAMPLES; ++x) {
            unsigned magnitude = result->audio.scope[x] < 0
                ? (unsigned)(-(int)result->audio.scope[x])
                : (unsigned)result->audio.scope[x];
            if (magnitude > peak) peak = magnitude;
        }
        std::snprintf(header, sizeof(header), "F-2  Graphical scopes, live stereo PCM peak=%u", peak);
        iplay_ncplane_fill_yx(plane, 5u, 0u, rows > 18u ? 13u : (dw)(rows - 5u), width, ' ', 0x78u);
        iplay_ncplane_putnstr_fill_yx(plane, 5u, 2u, header, 0x7fu, width > 4u ? (dw)(width - 4u) : 0u);
        for (channel = 0u;
             channel < result->audio.ui.channel_count && channel < 10u && (dw)(6u + channel) < rows;
             ++channel) {
            dw y = (dw)(6u + channel);
            db channel_label = channel < 9u ? (db)('1' + channel) : (db)('A' + channel - 9u);
            unsigned level = result->audio.ui.channels[channel].level;
            dw trace_width = width > 8u ? (dw)(width - 8u) : 0u;
            iplay_ncplane_putc_yx(plane, y, 2u, ' ', 0x7eu);
            iplay_ncplane_putc_yx(plane, y, 3u, channel_label, 0x7eu);
            iplay_ncplane_putc_yx(plane, y, 4u, ' ', 0x7eu);
            for (x = 0u; x < trace_width; ++x) {
                unsigned sample_index = (unsigned)((x * IPLAY_MODPLUG_SCOPE_SAMPLES) / trace_width);
                int value = (int)result->audio.scope[sample_index];
                unsigned magnitude;
                db glyph;
                db attr;
                value = (value * (int)level) / 30;
                magnitude = value < 0 ? (unsigned)(-value) : (unsigned)value;
                glyph = value > 0 ? 0xdfu : (value < 0 ? 0xdcu : 0xc4u);
                attr = magnitude > 2u ? 0x7eu : (magnitude > 0u ? 0x7au : 0x78u);
                iplay_ncplane_putc_yx(plane, y, (dw)(6u + x), glyph, attr);
            }
        }
    } else if (view_kind == IPLAY_MODERN_VIEW_SPECTRUM) {
        char header[64];
        unsigned peak = 0u;
        dw band;
        dw width = iplay_ncplane_cols(plane);
        dw base = rows > 18u ? 17u : (dw)(rows - 1u);
        for (band = 0u; band < IPLAY_MODPLUG_SPECTRUM_BANDS; ++band) {
            if (result->audio.spectrum[band] > peak) peak = result->audio.spectrum[band];
        }
        std::snprintf(header, sizeof(header), "F-5  FastFourier Frequency Analysis peak=%u", peak);
        iplay_ncplane_fill_yx(plane, 5u, 0u, rows > 18u ? 13u : (dw)(rows - 5u), width, ' ', 0x78u);
        iplay_ncplane_putnstr_fill_yx(plane, 5u, 2u, header, 0x7fu, width > 4u ? (dw)(width - 4u) : 0u);
        for (dw x = 0u; x < width; ++x) {
            unsigned first_band = (unsigned)((x * IPLAY_MODPLUG_SPECTRUM_BANDS) / width);
            unsigned end_band = (unsigned)(((x + 1u) * IPLAY_MODPLUG_SPECTRUM_BANDS) / width);
            dw height = 0u;
            dw y;
            if (end_band <= first_band) end_band = first_band + 1u;
            for (band = first_band; band < end_band && band < IPLAY_MODPLUG_SPECTRUM_BANDS; ++band) {
                if (result->audio.spectrum[band] > height) height = result->audio.spectrum[band];
            }
            if (height > base - 5u) height = (dw)(base - 5u);
            for (y = 0u; y < height; ++y) {
                iplay_ncplane_putc_yx(plane, (dw)(base - y), x, 0xdbu, y < 6u ? 0x7au : (y < 10u ? 0x7eu : 0x7cu));
            }
        }
    } else if (view_kind == IPLAY_MODERN_VIEW_HELP) {
        dw width = iplay_ncplane_cols(plane);
        if (rows > 28u) {
            iplay_ncplane_fill_yx(
                plane, 28u, 2u, (dw)(rows - 28u),
                width > 4u ? (dw)(width - 4u) : 0u, ' ', 0x78u);
        }
        if (width >= 80u) {
            struct HelpField {
                dw y;
                dw x;
                const char *text;
                db attr;
            };
            static const HelpField fields[] = {
                {6u, 28u, "So you wanted some help?", 0x7fu},
                {7u, 4u, "F-2", 0x7fu},
                {7u, 7u, "  Graphical scopes, one for each channel", 0x7eu},
                {8u, 4u, "F-3", 0x7fu},
                {8u, 7u, "  Realtime VU meters", 0x7eu},
                {9u, 4u, "F-4", 0x7fu},
                {9u, 7u, "  View sample names (twice for more)", 0x7eu},
                {10u, 4u, "F-5", 0x7fu},
                {10u, 7u, "  FastFourier Frequency Analysis", 0x7eu},
                {12u, 4u, "F-9", 0x7fu},
                {12u, 7u, "  ProTracker 1.0 compatibility on/off", 0x7eu},
                {13u, 4u, "F-10", 0x7fu},
                {13u, 8u, " Disable BPM on/off", 0x7eu},
                {14u, 4u, "F-11", 0x7fu},
                {14u, 8u, " Loop module", 0x7eu},
                {15u, 4u, "F-12", 0x7fu},
                {15u, 8u, " Toggle 24bit Interpolation", 0x7eu},
                {7u, 50u, "Gray - +", 0x7fu},
                {7u, 58u, "  Dec/Inc volume", 0x7eu},
                {8u, 55u, "[ ]", 0x7fu},
                {8u, 58u, "  Dec/Inc amplify", 0x7eu},
                {9u, 48u, "Cursor \x1a \x18", 0x7fu},
                {9u, 58u, "  Fast(er) forward", 0x7eu},
                {10u, 48u, "Cursor \x1b \x19", 0x7fu},
                {10u, 58u, "  Fast(er) rewind", 0x7eu},
                {11u, 50u, "1 Thru 0", 0x7fu},
                {11u, 58u, "  Mute channel", 0x7eu},
                {12u, 48u, "ScrollLock", 0x7fu},
                {12u, 58u, "  Loop pattern", 0x7eu},
                {13u, 53u, "Pause", 0x7fu},
                {13u, 58u, "  Guess...", 0x7eu},
                {14u, 55u, "End", 0x7fu},
                {14u, 58u, "  End pattern", 0x7eu},
                {15u, 55u, "Tab", 0x7fu},
                {15u, 58u, "  Toggle PAL/NTSC", 0x7eu}
            };
            unsigned field;
            dw y;
            for (y = 6u; y <= 15u; ++y) {
                iplay_ncplane_putnstr_fill_yx(plane, y, 2u, "", 0x78u, 76u);
            }
            for (field = 0u; field < sizeof(fields) / sizeof(fields[0]); ++field) {
                iplay_ncplane_putnstr_yx(
                    plane,
                    fields[field].y,
                    fields[field].x,
                    fields[field].text,
                    fields[field].attr,
                    (dw)std::strlen(fields[field].text));
            }
        } else {
            static const char *const compact_help[] = {
                "So you wanted some help?",
                "F2 scopes  F3 VU  F4 samples",
                "F5 spectrum",
                "F9/F10/F11/F12 options",
                "-/+ volume  [/] amplify",
                "Arrows seek  1..0 mute",
                "Pause playback  End pattern",
                "Tab PAL/NTSC"
            };
            dw y;
            iplay_ncplane_fill_yx(plane, 5u, 0u, rows > 18u ? 13u : (dw)(rows - 5u), width, ' ', 0x78u);
            for (y = 0u; y < (dw)(sizeof(compact_help) / sizeof(compact_help[0])) && (dw)(5u + y) < rows; ++y) {
                iplay_ncplane_putnstr_yx(plane, (dw)(5u + y), 1u, compact_help[y], y == 0u ? 0x7fu : 0x7eu, width > 2u ? (dw)(width - 2u) : 0u);
            }
        }
    } else if (view_kind == IPLAY_MODERN_VIEW_SAMPLES) {
        unsigned sample;
        unsigned display_rows = iplay_notcurses_presenter_rows();
        unsigned sample_page_size;
        if (display_rows == 0u) display_rows = rows;
        sample_page_size = display_rows > 19u
            ? display_rows - 19u
            : (display_rows > 7u ? display_rows - 7u : 0u);
        if (sample_page_size > 31u) sample_page_size = 31u;
        unsigned sample_end = sample_start + sample_page_size;
        dw width = iplay_ncplane_cols(plane);
        if (sample_start >= result->audio.ui.sample_count) sample_start = 0u;
        if (sample_end > result->audio.ui.sample_count) sample_end = result->audio.ui.sample_count;
        iplay_ncplane_fill_yx(plane, 6u, 2u, 10u, width > 4u ? (dw)(width - 4u) : 0u, ' ', 0x78u);
        if (rows > 28u) {
            iplay_ncplane_fill_yx(
                plane,
                28u,
                2u,
                (dw)(rows - 28u),
                width > 4u ? (dw)(width - 4u) : 0u,
                ' ',
                0x78u);
        }
        iplay_ncplane_putnstr_yx(plane, 6u, 3u, "# SampleName   ", 0x7eu, 15u);
        iplay_ncplane_putnstr_yx(plane, 6u, 18u, "Press F-4 for more", 0x78u, 18u);
        iplay_ncplane_putnstr_yx(plane, 6u, 36u, "Size Vol Mode  C-2 Tune LoopPos LoopEnd", 0x7eu, 42u);
        for (sample = sample_start; sample < sample_end && sample < IPLAY_MODPLUG_UI_MAX_SAMPLES; ++sample) {
            char number[4];
            char details[43];
            unsigned page_row = sample - sample_start;
            dw y = page_row < 9u
                ? (dw)(7u + page_row)
                : (dw)(28u + page_row - 9u);
            std::snprintf(number, sizeof(number), "%2u ", sample + 1u);
            iplay_ncplane_putnstr_fill_yx(plane, y, 2u, number, 0x7fu, 3u);
            iplay_ncplane_putnstr_fill_yx(
                plane,
                y,
                5u,
                result->audio.ui.sample_names[sample],
                0x7bu,
                32u);
            modern_format_sample_details(&result->audio.ui.sample_info[sample], details);
            iplay_ncplane_putnstr_fill_yx(plane, y, 37u, details, 0x7fu, 42u);
        }
    } else if (view_kind == IPLAY_MODERN_VIEW_UNDOCUMENTED) {
        static const unsigned F6_PRIMARY_CHANNEL_ROWS = 10u;
        static const dw F6_FIRST_ROW = 6u;
        static const dw F6_OVERFLOW_FIRST_ROW = 28u;
        static const dw F6_RULER_X = 7u;
        static const unsigned F6_RULER_POSITIONS = 17u;
        static const unsigned F6_PAN_CENTER = 64u;
        static const unsigned F6_PAN_MAX = 128u;
        static const unsigned F6_PAN_UNITS_PER_POSITION = 8u;
        static const dw F6_NUMBER_X = 24u;
        static const IplayModernPlaybackResult *pan_source = 0;
        static unsigned base_pan_count = 0u;
        static unsigned base_pan[IPLAY_MODPLUG_UI_MAX_CHANNELS] = {};
        unsigned channel;
        dw width = iplay_ncplane_cols(plane);
        dw y;
        if (pan_source != result || base_pan_count != result->audio.ui.channel_count) {
            pan_source = result;
            base_pan_count = result->audio.ui.channel_count < IPLAY_MODPLUG_UI_MAX_CHANNELS
                ? result->audio.ui.channel_count
                : (unsigned)IPLAY_MODPLUG_UI_MAX_CHANNELS;
            for (channel = 0u; channel < base_pan_count; ++channel) {
                base_pan[channel] = result->audio.ui.channels[channel].pan;
            }
        }
        for (y = 6u; y <= 15u && y < rows; ++y) {
            iplay_ncplane_putnstr_fill_yx(plane, y, 2u, "", 0x78u, width > 4u ? (dw)(width - 4u) : 0u);
        }
        for (y = 28u; y < rows; ++y) {
            iplay_ncplane_putnstr_fill_yx(plane, y, 2u, "", 0x78u, width > 4u ? (dw)(width - 4u) : 0u);
        }
        for (channel = 0u; channel < result->audio.ui.channel_count &&
                           channel < IPLAY_MODPLUG_UI_MAX_CHANNELS; ++channel) {
            char offset_text[5];
            unsigned pan = (result->audio.channel_pan_valid_mask & (1u << channel))
                ? result->audio.channel_pan[channel]
                : base_pan[channel];
            unsigned meter_pan = pan > F6_PAN_MAX ? F6_PAN_MAX : pan;
            unsigned marker = meter_pan / F6_PAN_UNITS_PER_POSITION;
            int offset = (int)meter_pan - (int)F6_PAN_CENTER;
            char side = meter_pan < F6_PAN_CENTER
                ? 'L' : (meter_pan == F6_PAN_CENTER ? 'M' : 'R');
            db label_attr = channel == result->audio.selected_channel ? 0x1eu : 0x7eu;
            db channel_label = channel < 9u ? (db)('1' + channel) : (db)('A' + channel - 9u);
            unsigned x;
            y = channel < F6_PRIMARY_CHANNEL_ROWS
                ? (dw)(F6_FIRST_ROW + channel)
                : (dw)(F6_OVERFLOW_FIRST_ROW +
                       channel - F6_PRIMARY_CHANNEL_ROWS);
            if (y >= rows) break;
            iplay_ncplane_putc_yx(plane, y, 2u, ' ', label_attr);
            iplay_ncplane_putc_yx(plane, y, 3u, channel_label, label_attr);
            iplay_ncplane_putc_yx(plane, y, 4u, ' ', label_attr);
            iplay_ncplane_putc_yx(plane, y, 5u, ' ', 0x7eu);
            iplay_ncplane_putc_yx(plane, y, 6u, ' ', 0x7eu);
            for (x = 0u; x < F6_RULER_POSITIONS; ++x) {
                iplay_ncplane_putc_yx(
                    plane, y, (dw)(F6_RULER_X + x),
                    x == marker ? (db)side : 0xc4u, 0x7eu);
            }
            std::snprintf(offset_text, sizeof(offset_text), "%4d", offset);
            iplay_ncplane_putnstr_yx(
                plane, y, F6_NUMBER_X, offset_text, 0x7eu, 4u);
        }
    }
    return true;
}
