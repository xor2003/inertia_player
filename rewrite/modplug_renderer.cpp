#include "modplug_renderer.hpp"

#include <libmodplug/modplug.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <vector>

typedef void *(*OpenMptCreateFromMemoryFn)(const void *, std::size_t, void *, void *, const void *);
typedef void (*OpenMptDestroyFn)(void *);
typedef std::size_t (*OpenMptReadStereoFn)(void *, std::int32_t, std::size_t, std::int16_t *);
typedef float (*OpenMptChannelVuFn)(void *, std::int32_t);
typedef double (*OpenMptSetPositionOrderRowFn)(void *, std::int32_t, std::int32_t);

struct IplayModplugRenderer {
    std::vector<unsigned char> module;
    std::vector<unsigned char> decode_module;
    ModPlugFile *file;
    IplayModplugChannelState active_channels[IPLAY_MODPLUG_UI_MAX_CHANNELS];
    unsigned meter_raw[IPLAY_MODPLUG_UI_MAX_CHANNELS];
    unsigned meter_draw_counter;
    void *openmpt_library;
    void *openmpt_module;
    OpenMptDestroyFn openmpt_destroy;
    OpenMptReadStereoFn openmpt_read_stereo;
    OpenMptChannelVuFn openmpt_vu_left;
    OpenMptChannelVuFn openmpt_vu_right;
    OpenMptSetPositionOrderRowFn openmpt_set_position_order_row;
    std::vector<std::int16_t> openmpt_pcm;
    unsigned telemetry_rate;
    bool compatibility_initialized;
    bool protracker_enabled;
    bool ignore_bpm_enabled;
};

struct IplayModplugPcmSource {
    IplayModplugRenderer *renderer;
    IplayMikmodPcmSource *mikmod;
    std::vector<std::int16_t> telemetry_pcm;
    bool ended;
    unsigned volume_percent;
};

struct IplayMikmodPcmSource {
    void *library;
    void *module;
    bool initialized;
    void (*exit_library)(void);
    void (*player_stop)(void);
    void (*player_free)(void *);
    int (*player_active)(void);
    void (*player_set_position)(unsigned short);
    int (*player_get_order)(void);
    int (*player_get_row)(void);
    void (*player_mute)(std::int32_t, ...);
    void (*player_unmute)(std::int32_t, ...);
    int (*player_muted)(unsigned char);
    int (*player_get_channel_voice)(unsigned char);
    void (*voice_set_panning)(signed char, unsigned int);
    unsigned short (*voice_get_volume)(signed char);
    unsigned int (*voice_real_volume)(signed char);
    unsigned int (*write_bytes)(signed char *, unsigned int);
    unsigned short *mode;
    unsigned short *mixfreq;
    unsigned channel_pan[64];
    bool channel_pan_valid[64];
};

struct IplayMikmodModulePlaybackPrefix {
    char *songname;
    char *modtype;
    char *comment;
    unsigned short flags;
    unsigned char numchn;
    unsigned char numvoices;
    unsigned short numpos;
    unsigned short numpat;
    unsigned short numins;
    unsigned short numsmp;
    void *instruments;
    void *samples;
    unsigned char realchn;
    unsigned char totalchn;
    unsigned short reppos;
    unsigned char initspeed;
    unsigned short inittempo;
    unsigned char initvolume;
    unsigned short panning[64];
    unsigned char chanvol[64];
    unsigned short bpm;
    unsigned short sngspd;
    signed short volume;
    int extspd;
    int panflag;
    int wrap;
    int loop;
};

static bool read_file(const char *path, std::vector<unsigned char> *data) {
    FILE *fp = std::fopen(path, "rb");
    long size;
    if (!fp) return false;
    if (std::fseek(fp, 0, SEEK_END) != 0) {
        std::fclose(fp);
        return false;
    }
    size = std::ftell(fp);
    if (size <= 0) {
        std::fclose(fp);
        return false;
    }
    if (std::fseek(fp, 0, SEEK_SET) != 0) {
        std::fclose(fp);
        return false;
    }
    data->resize((std::size_t)size);
    if (std::fread(data->data(), 1, data->size(), fp) != data->size()) {
        std::fclose(fp);
        return false;
    }
    std::fclose(fp);
    return true;
}

template <typename Function>
static Function openmpt_symbol(void *library, const char *name) {
    return reinterpret_cast<Function>(dlsym(library, name));
}

static void clear_stats(IplayModplugPcmStats *stats);
static void set_stats_from_pcm(IplayModplugPcmStats *stats, const std::int16_t *pcm, int bytes);
static bool modplug_is_four_channel_mod(const std::vector<unsigned char> &module);

static void mikmod_source_apply_pans(IplayMikmodPcmSource *source) {
    unsigned channel;
    if (!source || !source->player_get_channel_voice || !source->voice_set_panning) return;
    for (channel = 0u; channel < 64u; ++channel) {
        int voice;
        if (!source->channel_pan_valid[channel]) continue;
        voice = source->player_get_channel_voice((unsigned char)channel);
        if (voice >= 0 && voice <= 127) {
            unsigned pan = source->channel_pan[channel];
            source->voice_set_panning((signed char)voice, pan >= 166u ? 512u : pan * 255u / 128u);
        }
    }
}

IplayMikmodPcmSource *iplay_mikmod_pcm_source_open_file(const char *path) {
    typedef void (*RegisterDriverFn)(void *);
    typedef void (*RegisterLoadersFn)(void);
    typedef int (*InitFn)(const char *);
    typedef void *(*LoadFn)(const char *, int, int);
    typedef void (*StartFn)(void *);
    IplayMikmodPcmSource *source;
    RegisterDriverFn register_driver;
    RegisterLoadersFn register_loaders;
    InitFn init;
    LoadFn load;
    StartFn start;
    unsigned short *md_mode;
    unsigned short *md_mixfreq;
    unsigned short *md_device;
    unsigned char *md_volume;
    unsigned char *md_musicvolume;
    unsigned char *md_sndfxvolume;
    unsigned char *md_pansep;
    unsigned char *md_reverb;
    void *driver;
    if (!path) return 0;
    source = new IplayMikmodPcmSource;
    std::memset(source, 0, sizeof(*source));
    source->library = dlopen("libmikmod.so.3", RTLD_NOW | RTLD_LOCAL);
    if (!source->library) {
        delete source;
        return 0;
    }
    register_driver = openmpt_symbol<RegisterDriverFn>(source->library, "MikMod_RegisterDriver");
    register_loaders = openmpt_symbol<RegisterLoadersFn>(source->library, "MikMod_RegisterAllLoaders");
    init = openmpt_symbol<InitFn>(source->library, "MikMod_Init");
    load = openmpt_symbol<LoadFn>(source->library, "Player_Load");
    start = openmpt_symbol<StartFn>(source->library, "Player_Start");
    source->exit_library = openmpt_symbol<void (*)(void)>(source->library, "MikMod_Exit");
    source->player_stop = openmpt_symbol<void (*)(void)>(source->library, "Player_Stop");
    source->player_free = openmpt_symbol<void (*)(void *)>(source->library, "Player_Free");
    source->player_active = openmpt_symbol<int (*)(void)>(source->library, "Player_Active");
    source->player_set_position = openmpt_symbol<void (*)(unsigned short)>(source->library, "Player_SetPosition");
    source->player_get_order = openmpt_symbol<int (*)(void)>(source->library, "Player_GetOrder");
    source->player_get_row = openmpt_symbol<int (*)(void)>(source->library, "Player_GetRow");
    source->player_mute = openmpt_symbol<void (*)(std::int32_t, ...)>(source->library, "Player_Mute");
    source->player_unmute = openmpt_symbol<void (*)(std::int32_t, ...)>(source->library, "Player_Unmute");
    source->player_muted = openmpt_symbol<int (*)(unsigned char)>(source->library, "Player_Muted");
    source->player_get_channel_voice = openmpt_symbol<int (*)(unsigned char)>(source->library, "Player_GetChannelVoice");
    source->voice_set_panning = openmpt_symbol<void (*)(signed char, unsigned int)>(source->library, "Voice_SetPanning");
    source->voice_get_volume = openmpt_symbol<unsigned short (*)(signed char)>(source->library, "Voice_GetVolume");
    source->voice_real_volume = openmpt_symbol<unsigned int (*)(signed char)>(source->library, "Voice_RealVolume");
    source->write_bytes = openmpt_symbol<unsigned int (*)(signed char *, unsigned int)>(source->library, "VC_WriteBytes");
    md_mode = openmpt_symbol<unsigned short *>(source->library, "md_mode");
    source->mode = md_mode;
    md_mixfreq = openmpt_symbol<unsigned short *>(source->library, "md_mixfreq");
    source->mixfreq = md_mixfreq;
    md_device = openmpt_symbol<unsigned short *>(source->library, "md_device");
    md_volume = openmpt_symbol<unsigned char *>(source->library, "md_volume");
    md_musicvolume = openmpt_symbol<unsigned char *>(source->library, "md_musicvolume");
    md_sndfxvolume = openmpt_symbol<unsigned char *>(source->library, "md_sndfxvolume");
    md_pansep = openmpt_symbol<unsigned char *>(source->library, "md_pansep");
    md_reverb = openmpt_symbol<unsigned char *>(source->library, "md_reverb");
    driver = dlsym(source->library, "drv_nos");
    if (!register_driver || !register_loaders || !init || !load || !start ||
        !source->exit_library || !source->player_stop || !source->player_free ||
        !source->player_active || !source->player_set_position || !source->player_get_order ||
        !source->player_get_row || !source->player_mute || !source->player_unmute ||
        !source->player_muted || !source->player_get_channel_voice || !source->voice_set_panning ||
        !source->voice_get_volume || !source->voice_real_volume || !source->write_bytes ||
        !md_mode || !md_mixfreq || !md_device || !md_volume ||
        !md_musicvolume || !md_sndfxvolume || !md_pansep || !md_reverb || !driver) {
        dlclose(source->library);
        delete source;
        return 0;
    }
    *md_mode = 0x0001u | 0x0002u | 0x0008u | 0x0200u;
    *md_mixfreq = 44100u;
    *md_device = 0u;
    *md_volume = 128u;
    *md_musicvolume = 128u;
    *md_sndfxvolume = 128u;
    *md_pansep = 128u;
    *md_reverb = 0u;
    register_driver(driver);
    register_loaders();
    if (init(0) != 0) {
        dlclose(source->library);
        delete source;
        return 0;
    }
    source->initialized = true;
    source->module = load(path, 64, 0);
    if (!source->module) {
        source->exit_library();
        dlclose(source->library);
        delete source;
        return 0;
    }
    reinterpret_cast<IplayMikmodModulePlaybackPrefix *>(source->module)->extspd = 1;
    reinterpret_cast<IplayMikmodModulePlaybackPrefix *>(source->module)->panflag = 1;
    reinterpret_cast<IplayMikmodModulePlaybackPrefix *>(source->module)->wrap = 0;
    reinterpret_cast<IplayMikmodModulePlaybackPrefix *>(source->module)->loop = 0;
    start(source->module);
    return source;
}

bool iplay_mikmod_pcm_source_read(IplayMikmodPcmSource *source, std::int16_t *pcm, int frame_capacity, IplayModplugPcmStats *stats) {
    unsigned int requested;
    unsigned int written;
    if (!source || !pcm || !stats || frame_capacity <= 0 || !source->player_active()) return false;
    requested = (unsigned int)frame_capacity * 2u * (unsigned int)sizeof(std::int16_t);
    mikmod_source_apply_pans(source);
    written = source->write_bytes(reinterpret_cast<signed char *>(pcm), requested);
    if (written == 0u) return false;
    clear_stats(stats);
    set_stats_from_pcm(stats, pcm, (int)written);
    mikmod_source_apply_pans(source);
    return true;
}

bool iplay_mikmod_pcm_source_seek_position(IplayMikmodPcmSource *source, unsigned order, unsigned row) {
    std::vector<std::int16_t> discard(128u * 2u);
    IplayModplugPcmStats stats;
    unsigned attempts = 0u;
    if (!source || !source->player_set_position) return false;
    source->player_set_position((unsigned short)order);
    while (row > 0u && attempts < 32768u) {
        if (!iplay_mikmod_pcm_source_read(source, discard.data(), 128, &stats)) return false;
        if (iplay_mikmod_pcm_source_order(source) > order ||
            (iplay_mikmod_pcm_source_order(source) == order && iplay_mikmod_pcm_source_row(source) >= row)) break;
        ++attempts;
    }
    return attempts < 32768u;
}

bool iplay_mikmod_pcm_source_set_channel_muted(IplayMikmodPcmSource *source, unsigned channel, bool muted) {
    if (!source || channel >= 64u) return false;
    if (muted) source->player_mute((std::int32_t)channel);
    else source->player_unmute((std::int32_t)channel);
    return source->player_muted((unsigned char)channel) == (muted ? 1 : 0);
}

bool iplay_mikmod_pcm_source_channel_muted(const IplayMikmodPcmSource *source, unsigned channel) {
    return source && channel < 64u && source->player_muted((unsigned char)channel) != 0;
}

bool iplay_mikmod_pcm_source_set_channel_pan(IplayMikmodPcmSource *source, unsigned channel, unsigned pan) {
    if (!source || channel >= 64u) return false;
    if (pan > 166u) pan = 166u;
    source->channel_pan[channel] = pan;
    source->channel_pan_valid[channel] = true;
    mikmod_source_apply_pans(source);
    return true;
}

void iplay_mikmod_pcm_source_set_interpolation(IplayMikmodPcmSource *source, bool enabled) {
    if (!source || !source->mode) return;
    if (enabled) *source->mode |= 0x0200u;
    else *source->mode &= (unsigned short)~0x0200u;
}

void iplay_mikmod_pcm_source_set_pal_timing(IplayMikmodPcmSource *source, bool enabled) {
    if (!source || !source->mixfreq) return;
    *source->mixfreq = (unsigned short)(enabled ? IPLAY_PAL_MIX_RATE : IPLAY_NTSC_MIX_RATE);
}

unsigned iplay_original_meter_raw_from_mikmod(unsigned real_volume, unsigned voice_volume, unsigned channel_count) {
    std::uint64_t numerator;
    std::uint64_t denominator;
    unsigned divisor;
    unsigned raw;
    if (channel_count == 0u || real_volume == 0u || voice_volume == 0u) return 0u;
    if (channel_count > IPLAY_MODPLUG_UI_MAX_CHANNELS) channel_count = IPLAY_MODPLUG_UI_MAX_CHANNELS;
    if (real_volume > 65535u) real_volume = 65535u;
    if (voice_volume > 256u) voice_volume = 256u;
    divisor = 317u / channel_count;
    if (divisor == 0u) divisor = 1u;
    numerator = (std::uint64_t)real_volume * voice_volume * 80u;
    denominator = (std::uint64_t)262144u * divisor;
    raw = (unsigned)((numerator + denominator / 2u) / denominator);
    return raw > IPLAY_ORIGINAL_CHANNEL_METER_RAW_MAX ? IPLAY_ORIGINAL_CHANNEL_METER_RAW_MAX : raw;
}

unsigned iplay_mikmod_pcm_source_channel_level_raw(const IplayMikmodPcmSource *source, unsigned channel, unsigned channel_count) {
    int voice;
    if (!source || channel >= 64u) return 0u;
    voice = source->player_get_channel_voice((unsigned char)channel);
    if (voice < 0 || voice > 127) return 0u;
    return iplay_original_meter_raw_from_mikmod(
        source->voice_real_volume((signed char)voice),
        source->voice_get_volume((signed char)voice),
        channel_count);
}

unsigned iplay_mikmod_pcm_source_order(const IplayMikmodPcmSource *source) {
    int order = source && source->player_get_order ? source->player_get_order() : 0;
    return order > 0 ? (unsigned)order : 0u;
}

unsigned iplay_mikmod_pcm_source_row(const IplayMikmodPcmSource *source) {
    int row = source && source->player_get_row ? source->player_get_row() : 0;
    return row > 0 ? (unsigned)row : 0u;
}

void iplay_mikmod_pcm_source_close(IplayMikmodPcmSource *source) {
    if (!source) return;
    if (source->module && source->player_stop) source->player_stop();
    if (source->module && source->player_free) source->player_free(source->module);
    if (source->initialized && source->exit_library) source->exit_library();
    if (source->library) dlclose(source->library);
    delete source;
}

static std::uint64_t pcm_checksum(const std::vector<std::int16_t> &pcm, int sample_count) {
    std::uint64_t checksum = 0;
    int i;
    for (i = 0; i < sample_count; ++i) checksum += (std::uint16_t)pcm[(std::size_t)i];
    return checksum;
}

static std::uint64_t pcm_checksum_raw(const std::int16_t *pcm, int sample_count) {
    std::uint64_t checksum = 0;
    int i;
    for (i = 0; i < sample_count; ++i) checksum += (std::uint16_t)pcm[i];
    return checksum;
}

static int pcm_peak(const std::vector<std::int16_t> &pcm, int sample_count) {
    int peak = 0;
    int i;
    for (i = 0; i < sample_count; ++i) {
        int sample = pcm[(std::size_t)i];
        int mag = sample < 0 ? -sample : sample;
        if (mag > peak) peak = mag;
    }
    return peak;
}

static int pcm_peak_raw(const std::int16_t *pcm, int sample_count) {
    int peak = 0;
    int i;
    for (i = 0; i < sample_count; ++i) {
        int sample = pcm[i];
        int mag = sample < 0 ? -sample : sample;
        if (mag > peak) peak = mag;
    }
    return peak;
}

static void configure_modplug_stereo_16bit_44100(void) {
    ModPlug_Settings settings;
    ModPlug_GetSettings(&settings);
    settings.mFlags = MODPLUG_ENABLE_OVERSAMPLING;
    settings.mChannels = 2;
    settings.mBits = 16;
    settings.mFrequency = 44100;
    settings.mResamplingMode = MODPLUG_RESAMPLE_LINEAR;
    settings.mLoopCount = 0;
    ModPlug_SetSettings(&settings);
}

static void clear_stats(IplayModplugPcmStats *stats) {
    stats->frames = 0;
    stats->samples = 0;
    stats->bytes = 0;
    stats->checksum = 0;
    stats->peak = 0;
    std::memset(&stats->ui, 0, sizeof(stats->ui));
}

static void openmpt_telemetry_open(IplayModplugRenderer *renderer) {
    OpenMptCreateFromMemoryFn create;
    if (std::getenv("IPLAY_DISABLE_OPENMPT_TELEMETRY")) return;
    renderer->openmpt_library = dlopen("libopenmpt.so.0", RTLD_NOW | RTLD_LOCAL);
    if (!renderer->openmpt_library) return;
    create = openmpt_symbol<OpenMptCreateFromMemoryFn>(renderer->openmpt_library, "openmpt_module_create_from_memory");
    renderer->openmpt_destroy = openmpt_symbol<OpenMptDestroyFn>(renderer->openmpt_library, "openmpt_module_destroy");
    renderer->openmpt_read_stereo = openmpt_symbol<OpenMptReadStereoFn>(renderer->openmpt_library, "openmpt_module_read_interleaved_stereo");
    renderer->openmpt_vu_left = openmpt_symbol<OpenMptChannelVuFn>(renderer->openmpt_library, "openmpt_module_get_current_channel_vu_left");
    renderer->openmpt_vu_right = openmpt_symbol<OpenMptChannelVuFn>(renderer->openmpt_library, "openmpt_module_get_current_channel_vu_right");
    renderer->openmpt_set_position_order_row = openmpt_symbol<OpenMptSetPositionOrderRowFn>(renderer->openmpt_library, "openmpt_module_set_position_order_row");
    if (!create || !renderer->openmpt_destroy || !renderer->openmpt_read_stereo ||
        !renderer->openmpt_vu_left || !renderer->openmpt_vu_right) return;
    renderer->openmpt_module = create(renderer->module.data(), renderer->module.size(), 0, 0, 0);
}

static void openmpt_telemetry_close(IplayModplugRenderer *renderer) {
    if (renderer->openmpt_module && renderer->openmpt_destroy) renderer->openmpt_destroy(renderer->openmpt_module);
    if (renderer->openmpt_library) dlclose(renderer->openmpt_library);
    renderer->openmpt_module = 0;
    renderer->openmpt_library = 0;
    renderer->openmpt_destroy = 0;
    renderer->openmpt_read_stereo = 0;
    renderer->openmpt_vu_left = 0;
    renderer->openmpt_vu_right = 0;
    renderer->openmpt_set_position_order_row = 0;
    renderer->compatibility_initialized = false;
    renderer->protracker_enabled = false;
    renderer->ignore_bpm_enabled = false;
    renderer->openmpt_pcm.clear();
}

unsigned iplay_original_channel_meter_envelope_step(unsigned current_raw, unsigned target_raw, unsigned draw_counter) {
    if (current_raw > IPLAY_ORIGINAL_CHANNEL_METER_RAW_MAX) current_raw = IPLAY_ORIGINAL_CHANNEL_METER_RAW_MAX;
    if (target_raw > IPLAY_ORIGINAL_CHANNEL_METER_RAW_MAX) target_raw = IPLAY_ORIGINAL_CHANNEL_METER_RAW_MAX;
    if (target_raw >= current_raw) return target_raw;
    if ((draw_counter & IPLAY_ORIGINAL_CHANNEL_METER_DECAY_MASK) != 0u) return current_raw;
    return current_raw > IPLAY_ORIGINAL_CHANNEL_METER_DECAY
        ? current_raw - IPLAY_ORIGINAL_CHANNEL_METER_DECAY
        : 0u;
}

unsigned iplay_modplug_pan_from_stereo_vu(float left, float right, unsigned fallback) {
    float sum;
    if (fallback > 128u) fallback = 128u;
    if (left < 0.0f) left = 0.0f;
    if (right < 0.0f) right = 0.0f;
    sum = left + right;
    if (sum <= 0.000001f) return fallback;
    return (unsigned)(right * 128.0f / sum + 0.5f);
}

static void openmpt_telemetry_read(IplayModplugRenderer *renderer, int frames) {
    unsigned channel;
    if (!renderer->openmpt_module || frames <= 0) return;
    renderer->meter_draw_counter += 1u;
    renderer->openmpt_pcm.resize((std::size_t)frames * 2u);
    (void)renderer->openmpt_read_stereo(renderer->openmpt_module, (std::int32_t)renderer->telemetry_rate, (std::size_t)frames, renderer->openmpt_pcm.data());
    for (channel = 0u; channel < IPLAY_MODPLUG_UI_MAX_CHANNELS; ++channel) {
        float left = renderer->openmpt_vu_left(renderer->openmpt_module, (std::int32_t)channel);
        float right = renderer->openmpt_vu_right(renderer->openmpt_module, (std::int32_t)channel);
        float peak = left > right ? left : right;
        unsigned target_raw;
        if (peak < 0.0f) peak = 0.0f;
        if (peak > 1.0f) peak = 1.0f;
        target_raw = (unsigned)(peak * (float)IPLAY_ORIGINAL_CHANNEL_METER_RAW_MAX + 0.5f);
        renderer->meter_raw[channel] = iplay_original_channel_meter_envelope_step(
            renderer->meter_raw[channel],
            target_raw,
            renderer->meter_draw_counter);
        renderer->active_channels[channel].level = renderer->meter_raw[channel] >> 1u;
    }
}

static unsigned modplug_mod_used_samples(const std::vector<unsigned char> &module) {
    unsigned used = 0u;
    unsigned i;
    if (module.size() < 1084u) return 0u;
    for (i = 0u; i < 31u; ++i) {
        std::size_t offset = 20u + (std::size_t)i * 30u + 22u;
        if (module[offset] != 0u || module[offset + 1u] != 0u) ++used;
    }
    return used;
}

static bool modplug_is_four_channel_mod(const std::vector<unsigned char> &module) {
    if (module.size() < 1084u) return false;
    return std::memcmp(module.data() + 1080u, "M.K.", 4u) == 0
        || std::memcmp(module.data() + 1080u, "M!K!", 4u) == 0
        || std::memcmp(module.data() + 1080u, "4CHN", 4u) == 0
        || std::memcmp(module.data() + 1080u, "FLT4", 4u) == 0;
}

static void modplug_patch_loaded_mod_compatibility(ModPlugFile *file, const std::vector<unsigned char> &module, bool protracker, bool ignore_bpm) {
    static const unsigned MODPLUG_COMMAND_SPEED = 0x10u;
    unsigned pattern_count = 0u;
    unsigned order;
    unsigned pattern;
    std::size_t event;
    if (!file || !modplug_is_four_channel_mod(module)) return;
    for (order = 0u; order < module[950u] && order < 128u; ++order) {
        unsigned ordered_pattern = module[952u + order];
        if (ordered_pattern + 1u > pattern_count) pattern_count = ordered_pattern + 1u;
    }
    if (1084u + (std::size_t)pattern_count * 64u * 4u * 4u > module.size()) return;
    for (pattern = 0u; pattern < pattern_count; ++pattern) {
        unsigned rows = 0u;
        ModPlugNote *decoded = ModPlug_GetPattern(file, (int)pattern, &rows);
        if (!decoded) continue;
        for (event = 0u; event < (std::size_t)rows * 4u; ++event) {
            std::size_t source_event = (std::size_t)pattern * 64u * 4u + event;
            std::size_t offset = 1084u + source_event * 4u;
            unsigned effect = module[offset + 2u] & 0x0fu;
            unsigned parameter = module[offset + 3u];
            if (protracker && (effect == 4u || effect == 6u)) {
                unsigned depth = parameter & 0x0fu;
                decoded[event].Parameter = (unsigned char)((parameter & 0xf0u) | ((depth + 1u) >> 1u));
            }
            if (ignore_bpm && effect == 0x0fu && parameter > 0x20u) {
                decoded[event].Effect = (unsigned char)MODPLUG_COMMAND_SPEED;
                decoded[event].Parameter = (unsigned char)parameter;
            }
        }
    }
}

static bool modplug_renderer_reload_compatibility(IplayModplugRenderer *renderer, bool protracker, bool ignore_bpm) {
    int order;
    ModPlugFile *replacement;
    if (!renderer || !renderer->file || !modplug_is_four_channel_mod(renderer->module)) return false;
    order = ModPlug_GetCurrentOrder(renderer->file);
    renderer->decode_module = renderer->module;
    replacement = ModPlug_Load(renderer->decode_module.data(), (int)renderer->decode_module.size());
    if (!replacement) return false;
    modplug_patch_loaded_mod_compatibility(replacement, renderer->module, protracker, ignore_bpm);
    ModPlug_Unload(renderer->file);
    renderer->file = replacement;
    if (order > 0) ModPlug_SeekOrder(renderer->file, order);
    std::memset(renderer->active_channels, 0, sizeof(renderer->active_channels));
    std::memset(renderer->meter_raw, 0, sizeof(renderer->meter_raw));
    renderer->meter_draw_counter = 0u;
    return true;
}

static void copy_modplug_text(char *dst, std::size_t capacity, const char *src) {
    std::size_t i = 0u;
    if (!capacity) return;
    while (src && src[i] && i + 1u < capacity) {
        unsigned char ch = (unsigned char)src[i];
        dst[i] = ch >= 32u && ch < 127u ? (char)ch : ' ';
        ++i;
    }
    while (i > 0u && dst[i - 1u] == ' ') --i;
    dst[i] = 0;
}

static unsigned module_le16(const std::vector<unsigned char> &module, std::size_t offset) {
    if (offset + 2u > module.size()) return 0u;
    return (unsigned)module[offset] | ((unsigned)module[offset + 1u] << 8u);
}

static unsigned module_le32(const std::vector<unsigned char> &module, std::size_t offset) {
    if (offset + 4u > module.size()) return 0u;
    return (unsigned)module[offset] |
           ((unsigned)module[offset + 1u] << 8u) |
           ((unsigned)module[offset + 2u] << 16u) |
           ((unsigned)module[offset + 3u] << 24u);
}

static unsigned s3m_channel_pan(const std::vector<unsigned char> &module,
                                unsigned logical_channel,
                                unsigned fallback) {
    if (module.size() < 0x60u ||
        module[0x2cu] != 'S' || module[0x2du] != 'C' ||
        module[0x2eu] != 'R' || module[0x2fu] != 'M') return fallback;
    const unsigned orders = module_le16(module, 0x20u);
    const unsigned instruments = module_le16(module, 0x22u);
    const unsigned patterns = module_le16(module, 0x24u);
    const std::size_t pan_table = 0x60u + orders + instruments * 2u + patterns * 2u;
    unsigned logical = 0u;
    for (unsigned physical = 0u; physical < 32u; ++physical) {
        const unsigned setting = module[0x40u + physical];
        if (setting >= 16u) continue;
        if (logical++ != logical_channel) continue;
        if (module[0x35u] == 252u && pan_table + physical < module.size() &&
            (module[pan_table + physical] & 0x20u) != 0u) {
            const unsigned nibble = module[pan_table + physical] & 0x0fu;
            return (nibble * 128u + 7u) / 15u;
        }
        return setting < 8u ? 0u : 128u;
    }
    return fallback;
}

static unsigned s3m_channel_count(const std::vector<unsigned char> &module,
                                  unsigned fallback) {
    if (module.size() < 0x60u ||
        module[0x2cu] != 'S' || module[0x2du] != 'C' ||
        module[0x2eu] != 'R' || module[0x2fu] != 'M') return fallback;
    unsigned count = 0u;
    for (unsigned physical = 0u; physical < 32u; ++physical) {
        if (module[0x40u + physical] < 16u) ++count;
    }
    return count ? count : fallback;
}

static void capture_s3m_sample_info(IplayModplugRenderer *renderer,
                                    IplayModplugUiSnapshot *ui) {
    const std::vector<unsigned char> &module = renderer->module;
    if (module.size() < 0x60u ||
        module[0x2cu] != 'S' || module[0x2du] != 'C' ||
        module[0x2eu] != 'R' || module[0x2fu] != 'M') return;

    const unsigned order_count = module_le16(module, 0x20u);
    const unsigned instrument_count = module_le16(module, 0x22u);
    const std::size_t parapointers = 0x60u + order_count;
    const unsigned count = std::min<unsigned>(
        instrument_count, IPLAY_MODPLUG_UI_MAX_SAMPLES);
    if (parapointers + count * 2u > module.size()) return;

    ui->sample_count = count;
    for (unsigned sample = 0u; sample < count; ++sample) {
        const std::size_t header =
            (std::size_t)module_le16(module, parapointers + sample * 2u) * 16u;
        if (header + 0x50u > module.size() || module[header] != 1u) continue;

        IplayTrackerSampleInfo &info = ui->sample_info[sample];
        info.length = module_le32(module, header + 0x10u);
        info.loop_start = module_le32(module, header + 0x14u);
        info.loop_end = module_le32(module, header + 0x18u);
        info.volume = module[header + 0x1cu];
        info.flags = module[header + 0x1fu];
        info.c2_speed = module_le32(module, header + 0x20u);
        info.valid = 1u;
        if (!ui->sample_names[sample][0]) {
            char name[29];
            std::memcpy(name, &module[header + 0x30u], 28u);
            name[28] = 0;
            copy_modplug_text(ui->sample_names[sample],
                              sizeof(ui->sample_names[sample]), name);
        }
    }
}

static void capture_modplug_ui(IplayModplugRenderer *renderer, IplayModplugUiSnapshot *ui) {
    ModPlugNote *pattern_data;
    unsigned pattern_rows = 0u;
    unsigned channel_count;
    unsigned channel;
    unsigned sample;
    int order;
    int pattern;
    int row;
    std::memset(ui, 0, sizeof(*ui));
    order = ModPlug_GetCurrentOrder(renderer->file);
    pattern = ModPlug_GetCurrentPattern(renderer->file);
    row = ModPlug_GetCurrentRow(renderer->file);
    ui->order = order >= 0 ? (unsigned)order : 0u;
    ui->pattern = pattern >= 0 ? (unsigned)pattern : 0u;
    ui->row = row >= 0 ? (unsigned)row : 0u;
    ui->pattern_count = ModPlug_NumPatterns(renderer->file);
    ui->speed = ModPlug_GetCurrentSpeed(renderer->file);
    ui->tempo = ModPlug_GetCurrentTempo(renderer->file);
    ui->playing_channels = ModPlug_GetPlayingChannels(renderer->file);
    ui->sample_count = ModPlug_NumSamples(renderer->file);
    ui->used_samples = modplug_mod_used_samples(renderer->module);
    ui->channel_vu_available = renderer->openmpt_module ? 1u : 0u;
    copy_modplug_text(ui->title, sizeof(ui->title), ModPlug_GetName(renderer->file));
    for (sample = 0u; sample < ui->sample_count && sample < IPLAY_MODPLUG_UI_MAX_SAMPLES; ++sample) {
        char name[64];
        name[0] = 0;
        (void)ModPlug_SampleName(renderer->file, sample + 1u, name);
        copy_modplug_text(ui->sample_names[sample], sizeof(ui->sample_names[sample]), name);
    }
    capture_s3m_sample_info(renderer, ui);
    channel_count = ModPlug_NumChannels(renderer->file);
    if (channel_count > IPLAY_MODPLUG_UI_MAX_CHANNELS) channel_count = IPLAY_MODPLUG_UI_MAX_CHANNELS;
    ui->channel_count = s3m_channel_count(renderer->module, channel_count);
    if (ui->channel_count > IPLAY_MODPLUG_UI_MAX_CHANNELS)
        ui->channel_count = IPLAY_MODPLUG_UI_MAX_CHANNELS;
    for (channel = channel_count; channel < ui->channel_count; ++channel) {
        IplayModplugChannelState *state = &renderer->active_channels[channel];
        if (!state->pan_valid) {
            state->pan = s3m_channel_pan(renderer->module, channel, 64u);
            state->pan_valid = 1u;
        }
        ui->channels[channel] = *state;
    }
    pattern_data = ModPlug_GetPattern(renderer->file, pattern, &pattern_rows);
    ui->rows = pattern_rows;
    if (!pattern_data || row < 0 || (unsigned)row >= pattern_rows) return;
    for (channel = 0u; channel < channel_count; ++channel) {
        ModPlugNote note = pattern_data[(std::size_t)row * ModPlug_NumChannels(renderer->file) + channel];
        IplayModplugChannelState *state = &renderer->active_channels[channel];
        if (!state->pan_valid) {
            const unsigned fallback =
                (channel & 3u) == 0u || (channel & 3u) == 3u ? 0u : 128u;
            state->pan = s3m_channel_pan(renderer->module, channel, fallback);
            state->pan_valid = 1u;
        }
        if (note.Note != 0u) state->note = note.Note;
        if (note.Instrument != 0u) {
            char name[64];
            state->instrument = note.Instrument;
            name[0] = 0;
            (void)ModPlug_SampleName(renderer->file, note.Instrument, name);
            copy_modplug_text(state->sample_name, sizeof(state->sample_name), name);
        }
        if (note.VolumeEffect != 0u || note.Volume != 0u) {
            state->volume_effect = note.VolumeEffect;
            state->volume = note.Volume;
        }
        if (note.Effect != 0u || note.Parameter != 0u) {
            state->effect = note.Effect;
            state->parameter = note.Parameter;
        }
        ui->channels[channel] = *state;
        ui->channels[channel].row_effect = note.Effect;
        ui->channels[channel].row_parameter = note.Parameter;
    }
}

static void set_stats_from_pcm(IplayModplugPcmStats *stats, const std::int16_t *pcm, int bytes) {
    int samples = bytes / (int)sizeof(std::int16_t);
    std::vector<std::int16_t> pcm_view;
    pcm_view.assign(pcm, pcm + samples);
    stats->samples = samples;
    stats->frames = samples / 2;
    stats->bytes = bytes;
    stats->checksum = pcm_checksum(pcm_view, samples);
    stats->peak = pcm_peak(pcm_view, samples);
}

IplayModplugRenderer *iplay_modplug_renderer_open_file(const char *path) {
    IplayModplugRenderer *renderer = new IplayModplugRenderer;
    renderer->file = 0;
    renderer->openmpt_library = 0;
    renderer->openmpt_module = 0;
    renderer->openmpt_destroy = 0;
    renderer->openmpt_read_stereo = 0;
    renderer->openmpt_vu_left = 0;
    renderer->openmpt_vu_right = 0;
    renderer->openmpt_set_position_order_row = 0;
    renderer->telemetry_rate = IPLAY_PAL_MIX_RATE;
    std::memset(renderer->active_channels, 0, sizeof(renderer->active_channels));
    if (!read_file(path, &renderer->module)) {
        delete renderer;
        return 0;
    }
    configure_modplug_stereo_16bit_44100();
    renderer->file = ModPlug_Load(renderer->module.data(), (int)renderer->module.size());
    if (!renderer->file) {
        delete renderer;
        return 0;
    }
    renderer->decode_module = renderer->module;
    openmpt_telemetry_open(renderer);
    return renderer;
}

bool iplay_modplug_renderer_read(IplayModplugRenderer *renderer, std::int16_t *pcm, int frame_capacity, IplayModplugPcmStats *stats) {
    int bytes;
    if (!stats || !pcm || frame_capacity <= 0) return false;
    clear_stats(stats);
    if (!renderer || !renderer->file) return false;
    bytes = ModPlug_Read(renderer->file, pcm, frame_capacity * 2 * (int)sizeof(std::int16_t));
    if (bytes <= 0) return false;
    set_stats_from_pcm(stats, pcm, bytes);
    openmpt_telemetry_read(renderer, stats->frames);
    capture_modplug_ui(renderer, &stats->ui);
    return true;
}

bool iplay_modplug_renderer_read_until_end(IplayModplugRenderer *renderer, std::int16_t *pcm, int frame_capacity, int max_blocks, IplayModplugPcmStats *stats) {
    IplayModplugPcmStats block;
    int blocks = 0;
    bool ended = false;
    if (!stats || !pcm || frame_capacity <= 0 || max_blocks <= 0) return false;
    clear_stats(stats);
    while (blocks < max_blocks) {
        if (!iplay_modplug_renderer_read(renderer, pcm, frame_capacity, &block)) {
            ended = true;
            break;
        }
        stats->frames += block.frames;
        stats->samples += block.samples;
        stats->bytes += block.bytes;
        stats->checksum += pcm_checksum_raw(pcm, block.samples);
        if (pcm_peak_raw(pcm, block.samples) > stats->peak) stats->peak = pcm_peak_raw(pcm, block.samples);
        ++blocks;
        if (block.frames < frame_capacity) {
            ended = true;
            break;
        }
    }
    return stats->frames > 0 && ended;
}

void iplay_modplug_renderer_close(IplayModplugRenderer *renderer) {
    if (!renderer) return;
    openmpt_telemetry_close(renderer);
    if (renderer->file) ModPlug_Unload(renderer->file);
    delete renderer;
}

bool iplay_modplug_render_file_pcm(const char *path, std::int16_t *pcm, int frame_capacity, IplayModplugPcmStats *stats) {
    IplayModplugRenderer *renderer = iplay_modplug_renderer_open_file(path);
    bool ok;
    if (!renderer) return false;
    ok = iplay_modplug_renderer_read(renderer, pcm, frame_capacity, stats);
    iplay_modplug_renderer_close(renderer);
    return ok;
}

bool iplay_modplug_render_file_stats(const char *path, IplayModplugPcmStats *stats) {
    std::vector<std::int16_t> pcm;
    pcm.resize(44100u * 2u);
    if (!stats) return false;
    clear_stats(stats);
    if (!iplay_modplug_render_file_pcm(path, pcm.data(), 44100, stats)) return false;
    return true;
}

IplayModplugPcmSource *iplay_modplug_pcm_source_open_file(const char *path) {
    IplayModplugPcmSource *source = new IplayModplugPcmSource;
    source->renderer = iplay_modplug_renderer_open_file(path);
    source->mikmod = 0;
    source->ended = false;
    source->volume_percent = 100u;
    if (!source->renderer) {
        delete source;
        return 0;
    }
    if (!modplug_is_four_channel_mod(source->renderer->module)) {
        source->mikmod = iplay_mikmod_pcm_source_open_file(path);
    }
    return source;
}

bool iplay_modplug_pcm_source_read(IplayModplugPcmSource *source, std::int16_t *pcm, int frame_capacity, IplayModplugPcmStats *stats) {
    int sample;
    IplayModplugPcmStats telemetry_stats;
    unsigned previous_meter_raw[IPLAY_MODPLUG_UI_MAX_CHANNELS];
    unsigned previous_meter_draw_counter;
    if (!source || !stats || !pcm || frame_capacity <= 0) return false;
    if (source->ended) {
        clear_stats(stats);
        return false;
    }
    if (source->mikmod) {
        std::memcpy(previous_meter_raw, source->renderer->meter_raw, sizeof(previous_meter_raw));
        previous_meter_draw_counter = source->renderer->meter_draw_counter;
        if (!iplay_mikmod_pcm_source_read(source->mikmod, pcm, frame_capacity, stats)) {
            source->ended = true;
            clear_stats(stats);
            return false;
        }
        source->telemetry_pcm.resize((std::size_t)frame_capacity * 2u);
        if (iplay_modplug_renderer_read(source->renderer, source->telemetry_pcm.data(), frame_capacity, &telemetry_stats)) {
            unsigned channel;
            if (source->renderer->meter_draw_counter == previous_meter_draw_counter) {
                source->renderer->meter_draw_counter += 1u;
            }
            for (channel = 0u; channel < telemetry_stats.ui.channel_count &&
                               channel < IPLAY_MODPLUG_UI_MAX_CHANNELS; ++channel) {
                unsigned target_raw = iplay_mikmod_pcm_source_channel_level_raw(
                    source->mikmod,
                    channel,
                    telemetry_stats.ui.channel_count);
                source->renderer->meter_raw[channel] = iplay_original_channel_meter_envelope_step(
                    previous_meter_raw[channel],
                    target_raw,
                    source->renderer->meter_draw_counter);
                source->renderer->active_channels[channel].level = source->renderer->meter_raw[channel] >> 1u;
                telemetry_stats.ui.channels[channel].level = source->renderer->active_channels[channel].level;
            }
            telemetry_stats.ui.channel_vu_available = 1u;
            stats->ui = telemetry_stats.ui;
        }
    } else if (!iplay_modplug_renderer_read(source->renderer, pcm, frame_capacity, stats)) {
        source->ended = true;
        clear_stats(stats);
        return false;
    }
    if (source->volume_percent != 100u) {
        for (sample = 0; sample < stats->samples; ++sample) {
            std::int64_t scaled = ((std::int64_t)pcm[sample] * (std::int64_t)source->volume_percent) / 100;
            if (scaled > 32767) scaled = 32767;
            if (scaled < -32768) scaled = -32768;
            pcm[sample] = (std::int16_t)scaled;
        }
        set_stats_from_pcm(stats, pcm, stats->bytes);
    }
    if (stats->frames < frame_capacity) source->ended = true;
    return true;
}

bool iplay_modplug_pcm_source_ended(const IplayModplugPcmSource *source) {
    return !source || source->ended;
}

bool iplay_modplug_pcm_source_restart(IplayModplugPcmSource *source) {
    if (!source || !source->renderer || !source->renderer->file) return false;
    ModPlug_Seek(source->renderer->file, 0);
    std::memset(source->renderer->active_channels, 0, sizeof(source->renderer->active_channels));
    std::memset(source->renderer->meter_raw, 0, sizeof(source->renderer->meter_raw));
    source->renderer->meter_draw_counter = 0u;
    openmpt_telemetry_close(source->renderer);
    openmpt_telemetry_open(source->renderer);
    if (source->mikmod) (void)iplay_mikmod_pcm_source_seek_position(source->mikmod, 0u, 0u);
    source->ended = false;
    return true;
}

bool iplay_modplug_pcm_source_seek_position(IplayModplugPcmSource *source, unsigned order, unsigned row) {
    std::vector<std::int16_t> discard(128u * 2u);
    IplayModplugPcmStats stats;
    unsigned attempts = 0u;
    if (!source || !source->renderer || !source->renderer->file) return false;
    ModPlug_SeekOrder(source->renderer->file, (int)order);
    if (source->mikmod && !iplay_mikmod_pcm_source_seek_position(source->mikmod, order, row)) return false;
    if (source->renderer->openmpt_module && source->renderer->openmpt_set_position_order_row) {
        (void)source->renderer->openmpt_set_position_order_row(
            source->renderer->openmpt_module,
            (std::int32_t)order,
            0);
    }
    source->ended = false;
    while (row > 0u && attempts < 32768u) {
        if (!iplay_modplug_renderer_read(source->renderer, discard.data(), 128, &stats)) {
            source->ended = true;
            return false;
        }
        if (stats.ui.order > order || (stats.ui.order == order && stats.ui.row >= row)) break;
        ++attempts;
    }
    std::memset(source->renderer->active_channels, 0, sizeof(source->renderer->active_channels));
    std::memset(source->renderer->meter_raw, 0, sizeof(source->renderer->meter_raw));
    source->renderer->meter_draw_counter = 0u;
    return attempts < 32768u;
}

bool iplay_modplug_pcm_source_set_channel_muted(IplayModplugPcmSource *source, unsigned channel, bool muted) {
    return source && source->mikmod &&
        iplay_mikmod_pcm_source_set_channel_muted(source->mikmod, channel, muted);
}

bool iplay_modplug_pcm_source_channel_muted(const IplayModplugPcmSource *source, unsigned channel) {
    return source && source->mikmod &&
        iplay_mikmod_pcm_source_channel_muted(source->mikmod, channel);
}

bool iplay_modplug_pcm_source_set_channel_pan(IplayModplugPcmSource *source, unsigned channel, unsigned pan) {
    return source && source->mikmod &&
        iplay_mikmod_pcm_source_set_channel_pan(source->mikmod, channel, pan);
}

void iplay_modplug_pcm_source_set_master_volume(IplayModplugPcmSource *source, unsigned percent) {
    if (!source || !source->renderer || !source->renderer->file) return;
    if (percent > 400u) percent = 400u;
    source->volume_percent = percent;
    ModPlug_SetMasterVolume(source->renderer->file, 128u);
}

void iplay_modplug_pcm_source_set_interpolation(IplayModplugPcmSource *source, bool enabled) {
    ModPlug_Settings settings;
    if (!source || !source->renderer || !source->renderer->file) return;
    ModPlug_GetSettings(&settings);
    settings.mResamplingMode = enabled ? MODPLUG_RESAMPLE_LINEAR : MODPLUG_RESAMPLE_NEAREST;
    ModPlug_SetSettings(&settings);
    iplay_mikmod_pcm_source_set_interpolation(source->mikmod, enabled);
}

void iplay_modplug_pcm_source_set_pal_timing(IplayModplugPcmSource *source, bool enabled) {
    unsigned rate;
    if (!source || !source->renderer) return;
    rate = enabled ? IPLAY_PAL_MIX_RATE : IPLAY_NTSC_MIX_RATE;
    source->renderer->telemetry_rate = rate;
    if (source->mikmod) {
        iplay_mikmod_pcm_source_set_pal_timing(source->mikmod, enabled);
    } else {
        ModPlug_Settings settings;
        ModPlug_GetSettings(&settings);
        settings.mFrequency = (int)rate;
        ModPlug_SetSettings(&settings);
    }
}

void iplay_modplug_pcm_source_set_mod_compatibility(IplayModplugPcmSource *source, bool protracker, bool ignore_bpm) {
    if (!source || !source->renderer) return;
    if (source->renderer->compatibility_initialized
        && source->renderer->protracker_enabled == protracker
        && source->renderer->ignore_bpm_enabled == ignore_bpm) return;
    if (modplug_renderer_reload_compatibility(source->renderer, protracker, ignore_bpm)) {
        source->renderer->compatibility_initialized = true;
        source->renderer->protracker_enabled = protracker;
        source->renderer->ignore_bpm_enabled = ignore_bpm;
        source->ended = false;
    }
}

void iplay_modplug_pcm_source_close(IplayModplugPcmSource *source) {
    if (!source) return;
    iplay_mikmod_pcm_source_close(source->mikmod);
    iplay_modplug_renderer_close(source->renderer);
    delete source;
}
