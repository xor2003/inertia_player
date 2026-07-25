#include "modplug_renderer.hpp"

#include <cstdint>
#include <cstdio>
#include <vector>

static std::uint64_t render_checksum(IplayMikmodPcmSource *source, unsigned blocks, std::uint64_t *left_energy, std::uint64_t *right_energy) {
    std::vector<std::int16_t> pcm(2048u * 2u);
    IplayModplugPcmStats stats;
    std::uint64_t checksum = 0u;
    unsigned block;
    *left_energy = 0u;
    *right_energy = 0u;
    for (block = 0u; block < blocks; ++block) {
        int frame;
        if (!iplay_mikmod_pcm_source_read(source, pcm.data(), 2048, &stats)) break;
        checksum += stats.checksum;
        for (frame = 0; frame < stats.frames; ++frame) {
            int left = pcm[(std::size_t)frame * 2u];
            int right = pcm[(std::size_t)frame * 2u + 1u];
            *left_energy += (std::uint64_t)(left < 0 ? -left : left);
            *right_energy += (std::uint64_t)(right < 0 ? -right : right);
        }
    }
    return checksum;
}

int main(int argc, char **argv) {
    IplayMikmodPcmSource *source;
    std::uint64_t baseline;
    std::uint64_t muted;
    std::uint64_t left_pan;
    std::uint64_t right_pan;
    std::uint64_t left_energy;
    std::uint64_t right_energy;
    std::uint64_t unused_left;
    std::uint64_t unused_right;
    unsigned baseline_level;
    unsigned channel;
    unsigned scale_zero;
    unsigned scale_full_one;
    unsigned scale_half_four;
    unsigned scale_full_four;
    if (argc != 2) return 2;
    source = iplay_mikmod_pcm_source_open_file(argv[1]);
    if (!source) return 3;
    baseline = render_checksum(source, 16u, &unused_left, &unused_right);
    baseline_level = 0u;
    for (channel = 0u; channel < 10u; ++channel) {
        unsigned level = iplay_mikmod_pcm_source_channel_level_raw(source, channel, 10u);
        if (level > baseline_level) baseline_level = level;
    }
    scale_zero = iplay_original_meter_raw_from_mikmod(0u, 256u, 4u);
    scale_full_one = iplay_original_meter_raw_from_mikmod(65535u, 256u, 1u);
    scale_half_four = iplay_original_meter_raw_from_mikmod(32768u, 128u, 4u);
    scale_full_four = iplay_original_meter_raw_from_mikmod(65535u, 256u, 4u);
    (void)iplay_mikmod_pcm_source_seek_position(source, 0u, 0u);
    if (!iplay_mikmod_pcm_source_set_channel_muted(source, 0u, true)) return 4;
    muted = render_checksum(source, 16u, &unused_left, &unused_right);
    if (!iplay_mikmod_pcm_source_set_channel_muted(source, 0u, false)) return 5;
    (void)iplay_mikmod_pcm_source_seek_position(source, 0u, 0u);
    (void)iplay_mikmod_pcm_source_set_channel_pan(source, 0u, 0u);
    left_pan = render_checksum(source, 16u, &left_energy, &unused_right);
    (void)iplay_mikmod_pcm_source_seek_position(source, 0u, 0u);
    (void)iplay_mikmod_pcm_source_set_channel_pan(source, 0u, 128u);
    right_pan = render_checksum(source, 16u, &unused_left, &right_energy);
    std::printf("baseline=%llu muted=%llu left_pan=%llu right_pan=%llu left_energy=%llu right_energy=%llu baseline_level=%u scale_zero=%u scale_full_one=%u scale_half_four=%u scale_full_four=%u muted_state=%u order=%u row=%u\n",
                (unsigned long long)baseline,
                (unsigned long long)muted,
                (unsigned long long)left_pan,
                (unsigned long long)right_pan,
                (unsigned long long)left_energy,
                (unsigned long long)right_energy,
                baseline_level,
                scale_zero,
                scale_full_one,
                scale_half_four,
                scale_full_four,
                iplay_mikmod_pcm_source_channel_muted(source, 0u) ? 1u : 0u,
                iplay_mikmod_pcm_source_order(source),
                iplay_mikmod_pcm_source_row(source));
    iplay_mikmod_pcm_source_close(source);
    return 0;
}
