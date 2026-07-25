#include "modplug_renderer.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>

static void print_stats(const IplayModplugPcmStats &stats) {
    std::printf("frames=%d samples=%d bytes=%d checksum=%llu peak=%d",
                stats.frames,
                stats.samples,
                stats.bytes,
                (unsigned long long)stats.checksum,
                stats.peak);
}

int main(int argc, char **argv) {
    IplayModplugPcmStats stats;
    int frames = 44100;
    std::vector<std::int16_t> pcm;
    if (argc == 2 && std::strcmp(argv[1], "--pan-ratio") == 0) {
        std::printf("left=%u center=%u right=%u silent=%u\n",
                    iplay_modplug_pan_from_stereo_vu(1.0f, 0.0f, 64u),
                    iplay_modplug_pan_from_stereo_vu(0.5f, 0.5f, 0u),
                    iplay_modplug_pan_from_stereo_vu(0.0f, 1.0f, 64u),
                    iplay_modplug_pan_from_stereo_vu(0.0f, 0.0f, 37u));
        return 0;
    }
    if (argc == 2 && std::strcmp(argv[1], "--meter-envelope") == 0) {
        unsigned level = 0u;
        unsigned counter;
        level = iplay_original_channel_meter_envelope_step(level, 60u, 1u);
        std::printf("rise=%u ", level);
        for (counter = 2u; counter <= 31u; ++counter) {
            level = iplay_original_channel_meter_envelope_step(level, 0u, counter);
        }
        std::printf("hold31=%u ", level);
        level = iplay_original_channel_meter_envelope_step(level, 0u, 32u);
        std::printf("decay32=%u ", level);
        for (counter = 33u; counter <= 64u; ++counter) {
            level = iplay_original_channel_meter_envelope_step(level, 0u, counter);
        }
        std::printf("decay64=%u ", level);
        level = iplay_original_channel_meter_envelope_step(level, 60u, 65u);
        std::printf("rerise=%u cells=%u\n", level, level >> 1u);
        return 0;
    }
    if (argc != 2 && argc != 3) return 2;
    if (argc == 3 && std::strcmp(argv[2], "--twoblocks") == 0) {
        IplayModplugRenderer *renderer;
        IplayModplugPcmStats second;
        pcm.resize(512u * 2u);
        renderer = iplay_modplug_renderer_open_file(argv[1]);
        if (!renderer) return 3;
        if (!iplay_modplug_renderer_read(renderer, pcm.data(), 512, &stats)) {
            iplay_modplug_renderer_close(renderer);
            return 4;
        }
        if (!iplay_modplug_renderer_read(renderer, pcm.data(), 512, &second)) {
            iplay_modplug_renderer_close(renderer);
            return 5;
        }
        iplay_modplug_renderer_close(renderer);
        std::printf("first_");
        print_stats(stats);
        std::printf(" second_frames=%d second_samples=%d second_bytes=%d second_checksum=%llu second_peak=%d\n",
                    second.frames,
                    second.samples,
                    second.bytes,
                    (unsigned long long)second.checksum,
                    second.peak);
        return 0;
    }
    if (argc == 3 && std::strcmp(argv[2], "--until-end") == 0) {
        IplayModplugRenderer *renderer;
        pcm.resize(512u * 2u);
        renderer = iplay_modplug_renderer_open_file(argv[1]);
        if (!renderer) return 3;
        if (!iplay_modplug_renderer_read_until_end(renderer, pcm.data(), 512, 16384, &stats)) {
            iplay_modplug_renderer_close(renderer);
            return 4;
        }
        iplay_modplug_renderer_close(renderer);
        print_stats(stats);
        std::printf("\n");
        return 0;
    }
    if (argc == 3 && std::strcmp(argv[2], "--source-loop") == 0) {
        IplayModplugPcmSource *source;
        IplayModplugPcmStats block;
        int blocks = 0;
        pcm.resize(512u * 2u);
        source = iplay_modplug_pcm_source_open_file(argv[1]);
        if (!source) return 3;
        stats.frames = 0;
        stats.samples = 0;
        stats.bytes = 0;
        stats.checksum = 0;
        stats.peak = 0;
        while (!iplay_modplug_pcm_source_ended(source) && blocks < 16384) {
            if (!iplay_modplug_pcm_source_read(source, pcm.data(), 512, &block)) break;
            stats.frames += block.frames;
            stats.samples += block.samples;
            stats.bytes += block.bytes;
            stats.checksum += block.checksum;
            if (block.peak > stats.peak) stats.peak = block.peak;
            ++blocks;
        }
        if (!iplay_modplug_pcm_source_ended(source)) {
            iplay_modplug_pcm_source_close(source);
            return 4;
        }
        iplay_modplug_pcm_source_close(source);
        std::printf("blocks=%d ended=1 ", blocks);
        print_stats(stats);
        std::printf("\n");
        return 0;
    }
    if (argc == 3 && std::strncmp(argv[2], "--ui-blocks=", 12) == 0) {
        IplayModplugRenderer *renderer;
        int blocks = std::atoi(argv[2] + 12);
        int block;
        if (blocks <= 0) return 2;
        pcm.resize(512u * 2u);
        renderer = iplay_modplug_renderer_open_file(argv[1]);
        if (!renderer) return 3;
        for (block = 0; block < blocks; ++block) {
            if (!iplay_modplug_renderer_read(renderer, pcm.data(), 512, &stats)) break;
            std::printf("ui_block=%d row=%u note=%u instrument=%u effect=%u parameter=%u sample=\"%s\" vu_available=%u level=%u pan=%u pan_valid=%u\n",
                        block,
                        stats.ui.row,
                        stats.ui.channels[0].note,
                        stats.ui.channels[0].instrument,
                        stats.ui.channels[0].effect,
                        stats.ui.channels[0].parameter,
                        stats.ui.channels[0].sample_name,
                        stats.ui.channel_vu_available,
                        stats.ui.channels[0].level,
                        stats.ui.channels[0].pan,
                        stats.ui.channels[0].pan_valid);
        }
        iplay_modplug_renderer_close(renderer);
        return block > 0 ? 0 : 4;
    }
    if (argc == 3) frames = std::atoi(argv[2]);
    if (frames <= 0) return 2;
    pcm.resize((std::size_t)frames * 2u);
    if (!iplay_modplug_render_file_pcm(argv[1], pcm.data(), frames, &stats)) return 3;
    print_stats(stats);
    std::printf("\n");
    return 0;
}
