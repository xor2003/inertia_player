#define IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER 1
#include "modplug_audio_bridge.hpp"

#include <cstdio>
#include <cstring>

struct PlayerModuleInfo {
    const char *path;
};

struct PlayerPlaybackBlock {
    db pcm[1024u * 4u];
    dw frames;
};

struct AudioCapture {
    unsigned long calls;
    unsigned long bytes;
    unsigned long checksum;
};

static PlayerExternalDecoderRenderFn installed_render;
static void *installed_user;
static const char *installed_provider;

static void capture_audio_write(void *user, const db *pcm, dw byte_count) {
    AudioCapture *capture = static_cast<AudioCapture *>(user);
    dw i;
    if (!capture) return;
    capture->calls += 1ul;
    capture->bytes += byte_count;
    for (i = 0; i < byte_count; ++i) capture->checksum += pcm[i];
}

extern "C" void iplay_player_set_external_decoder(PlayerExternalDecoderRenderFn render, void *user, const char *provider) {
    installed_render = render;
    installed_user = user;
    installed_provider = provider;
}

extern "C" void iplay_player_clear_external_decoder(void) {
    installed_render = 0;
    installed_user = 0;
    installed_provider = "none";
}

extern "C" const char *iplay_player_module_path(const PlayerModuleInfo *module) {
    return module ? module->path : 0;
}

extern "C" unsigned long iplay_player_module_size(const PlayerModuleInfo *module) {
    (void)module;
    return 0ul;
}

extern "C" int iplay_player_module_header_truncated(const PlayerModuleInfo *module) {
    (void)module;
    return 0;
}

extern "C" const char *iplay_player_module_decoder_input_name(const PlayerModuleInfo *module) {
    (void)module;
    return "memory";
}

extern "C" db *iplay_player_playback_block_pcm(PlayerPlaybackBlock *block) {
    return block ? block->pcm : 0;
}

extern "C" dw iplay_player_playback_block_frames(const PlayerPlaybackBlock *block) {
    return block ? block->frames : 0;
}

extern "C" dw iplay_player_playback_block_capacity_frames(void) {
    return 1024u;
}

extern "C" void iplay_player_playback_block_set_frames(PlayerPlaybackBlock *block, dw frames) {
    if (frames > iplay_player_playback_block_capacity_frames()) frames = iplay_player_playback_block_capacity_frames();
    if (block) block->frames = frames;
}

extern "C" dw iplay_player_playback_block_active_bytes(const PlayerPlaybackBlock *block) {
    return block ? (dw)(block->frames * 4u) : 0;
}

static unsigned long checksum_block(const PlayerPlaybackBlock *block) {
    unsigned long checksum = 0ul;
    dw bytes = iplay_player_playback_block_active_bytes(block);
    dw i;
    for (i = 0; i < bytes; ++i) checksum += block->pcm[i];
    return checksum;
}

static void reset_block(PlayerPlaybackBlock *block) {
    std::memset(block, 0, sizeof(*block));
    block->frames = 512u;
}

static void reset_block_frames(PlayerPlaybackBlock *block, dw frames) {
    std::memset(block, 0, sizeof(*block));
    block->frames = frames;
}

static int run_until_end(PlayerExternalDecoderRenderFn render, void *user, const PlayerModuleInfo *module,
                         unsigned long *blocks, unsigned long *frames, unsigned long *checksum,
                         unsigned long *active_bytes, unsigned long *partial_blocks, int *last_status, dw frames_per_block) {
    PlayerPlaybackBlock block;
    unsigned long guard = 16384ul;
    *blocks = 0ul;
    *frames = 0ul;
    *checksum = 0ul;
    *active_bytes = 0ul;
    *partial_blocks = 0ul;
    *last_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    while (guard-- != 0ul) {
        reset_block_frames(&block, frames_per_block);
        *last_status = render(user, module, &block);
        if (*last_status == IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED) return 1;
        if (*last_status != IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED) return 0;
        *blocks += 1ul;
        *frames += block.frames;
        *checksum += checksum_block(&block);
        *active_bytes += iplay_player_playback_block_active_bytes(&block);
        if (block.frames != frames_per_block) *partial_blocks += 1ul;
    }
    return 0;
}

static int run_switch_path(PlayerExternalDecoderRenderFn render, void *user, const char *first_path, const char *second_path,
                           int *first_status, int *second_status, unsigned long *first_checksum, unsigned long *second_checksum) {
    PlayerModuleInfo first_module;
    PlayerModuleInfo second_module;
    PlayerPlaybackBlock first_block;
    PlayerPlaybackBlock second_block;
    unsigned i;
    first_module.path = first_path;
    second_module.path = second_path;
    reset_block(&first_block);
    reset_block(&second_block);
    *first_status = render(user, &first_module, &first_block);
    *first_checksum = checksum_block(&first_block);
    *second_checksum = 0ul;
    *second_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    for (i = 0; i < 256u; ++i) {
        reset_block(&second_block);
        *second_status = render(user, &second_module, &second_block);
        if (*second_status != IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED) break;
        *second_checksum += checksum_block(&second_block);
        if (*second_checksum != 0ul) break;
    }
    return *first_status == IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED
        && *second_status == IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED
        && *first_checksum != 0ul
        && *second_checksum != 0ul
        && *first_checksum != *second_checksum;
}

static int run_bad_then_good(PlayerExternalDecoderRenderFn render, void *user, const char *bad_path, const char *good_path,
                             int *bad_status, int *good_status, unsigned long *good_checksum) {
    PlayerModuleInfo bad_module;
    PlayerModuleInfo good_module;
    PlayerPlaybackBlock bad_block;
    PlayerPlaybackBlock good_block;
    bad_module.path = bad_path;
    good_module.path = good_path;
    reset_block(&bad_block);
    reset_block(&good_block);
    *bad_status = render(user, &bad_module, &bad_block);
    *good_status = render(user, &good_module, &good_block);
    *good_checksum = checksum_block(&good_block);
    return *bad_status == IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE
        && *good_status == IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED
        && *good_checksum != 0ul;
}

static int run_hook_audio(IplayModplugExternalDecoder *decoder, const char *path,
                          unsigned long *blocks, unsigned long *frames, unsigned long *accepted,
                          unsigned long *capture_calls, unsigned long *capture_bytes, unsigned long *capture_checksum,
                          unsigned *last_left_level, unsigned *last_right_level,
                          unsigned *max_left_level, unsigned *max_right_level,
                          int *source_end, int *block_limit, int *last_status, unsigned long max_blocks, int require_source_end, int null_writer) {
    PlayerModuleInfo module;
    PlayerPlaybackBlock block;
    IplayModplugHookAudioStats stats;
    AudioCapture capture = {0ul, 0ul, 0ul};
    module.path = path;
    reset_block(&block);
    (void)iplay_modplug_external_decoder_play_module_to_sdl_sb16(decoder,
                                                                 &module,
                                                                 &block,
                                                                 null_writer ? 0 : capture_audio_write,
                                                                 null_writer ? 0 : &capture,
                                                                 512u,
                                                                 max_blocks,
                                                                 require_source_end,
                                                                 &stats);
    *blocks = stats.blocks;
    *frames = stats.frames;
    *accepted = stats.accepted_bytes;
    *capture_calls = stats.capture_calls;
    *capture_bytes = stats.capture_bytes;
    *capture_checksum = stats.capture_checksum;
    *last_left_level = stats.last_left_level;
    *last_right_level = stats.last_right_level;
    *max_left_level = stats.max_left_level;
    *max_right_level = stats.max_right_level;
    *source_end = stats.source_end;
    *block_limit = stats.block_limit;
    *last_status = stats.last_status;
    return *blocks != 0ul
        && *frames != 0ul
        && *accepted == *frames * 4ul
        && *capture_calls == *blocks
        && *capture_bytes == *accepted
        && *capture_checksum != 0ul
        && (!require_source_end || (*source_end && *last_status == IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED));
}

int main(int argc, char **argv) {
    IplayModplugExternalDecoder *decoder;
    PlayerModuleInfo module;
    PlayerPlaybackBlock first;
    PlayerPlaybackBlock second;
    int first_status;
    int second_status;
    unsigned long first_checksum;
    unsigned long second_checksum;
    int until_end = 0;
    int switch_path = 0;
    int replay_same_path = 0;
    int bad_then_good = 0;
    int hook_audio = 0;
    int hook_audio_until_end = 0;
    int hook_audio_null_writer = 0;
    int hook_audio_one_block = 0;
    int unavailable_probe = 0;
    unsigned long end_blocks = 0ul;
    unsigned long end_frames = 0ul;
    unsigned long end_checksum = 0ul;
    unsigned long end_active_bytes = 0ul;
    unsigned long end_partial_blocks = 0ul;
    int end_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    int ended = 0;
    const char *provider_before_uninstall;
    int switched = 0;
    int switch_first_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    int switch_second_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    unsigned long switch_first_checksum = 0ul;
    unsigned long switch_second_checksum = 0ul;
    PlayerModuleInfo replay_module;
    unsigned long replay_blocks = 0ul;
    unsigned long replay_frames = 0ul;
    unsigned long replay_checksum = 0ul;
    unsigned long replay_active_bytes = 0ul;
    unsigned long replay_partial_blocks = 0ul;
    int replay_end_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    int replay_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    unsigned long replay_first_checksum = 0ul;
    int recovered = 0;
    int bad_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    int good_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    unsigned long good_checksum = 0ul;
    int audio_ok = 0;
    unsigned long audio_blocks = 0ul;
    unsigned long audio_frames = 0ul;
    unsigned long audio_accepted = 0ul;
    unsigned long audio_capture_calls = 0ul;
    unsigned long audio_capture_bytes = 0ul;
    unsigned long audio_capture_checksum = 0ul;
    unsigned audio_left = 0u;
    unsigned audio_right = 0u;
    unsigned audio_max_left = 0u;
    unsigned audio_max_right = 0u;
    int audio_source_end = 0;
    int audio_block_limit = 0;
    int audio_last_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    int unavailable_status = IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE;
    unsigned long unavailable_checksum = 0ul;
    unsigned unavailable_frames = 0u;
    unsigned unavailable_active_bytes = 0u;

    if (argc == 2 && std::strcmp(argv[1], "--frame-contract") == 0) {
        PlayerPlaybackBlock contract_block;
        reset_block_frames(&contract_block, 1024u);
        iplay_player_playback_block_set_frames(&contract_block, 123u);
        {
            dw small_frames = iplay_player_playback_block_frames(&contract_block);
            dw small_bytes = iplay_player_playback_block_active_bytes(&contract_block);
        std::printf("frames=%u active_bytes=%u",
                    (unsigned)small_frames,
                    (unsigned)small_bytes);
        iplay_player_playback_block_set_frames(&contract_block, 9000u);
        std::printf(" capacity=%u over_frames=%u over_active_bytes=%u\n",
                    (unsigned)iplay_player_playback_block_capacity_frames(),
                    (unsigned)iplay_player_playback_block_frames(&contract_block),
                    (unsigned)iplay_player_playback_block_active_bytes(&contract_block));
        return small_frames == 123u
            && small_bytes == 492u
            && iplay_player_playback_block_frames(&contract_block) == 1024u
            && iplay_player_playback_block_active_bytes(&contract_block) == 4096u
            ? 0 : 5;
        }
    }
    if (argc == 2 && std::strcmp(argv[1], "--invalid-helper-stats") == 0) {
        IplayModplugHookAudioStats stats;
        int ok;
        stats.blocks = 7ul;
        stats.frames = 9ul;
        stats.accepted_bytes = 11ul;
        stats.capture_calls = 13ul;
        stats.capture_bytes = 15ul;
        stats.capture_checksum = 17ul;
        stats.last_left_level = 19u;
        stats.last_right_level = 21u;
        stats.max_left_level = 23u;
        stats.max_right_level = 25u;
        stats.source_end = 1;
        stats.block_limit = 1;
        stats.last_status = IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED;
        ok = iplay_modplug_external_decoder_play_module_to_sdl_sb16(0, 0, 0, 0, 0, 0, 0ul, 0, &stats) ? 1 : 0;
        std::printf("ok=%d blocks=%lu frames=%lu accepted=%lu capture_calls=%lu capture_bytes=%lu capture_checksum=%lu levels=%u,%u maxlevels=%u,%u source_end=%d block_limit=%d last_status=%d\n",
                    ok,
                    stats.blocks,
                    stats.frames,
                    stats.accepted_bytes,
                    stats.capture_calls,
                    stats.capture_bytes,
                    stats.capture_checksum,
                    stats.last_left_level,
                    stats.last_right_level,
                    stats.max_left_level,
                    stats.max_right_level,
                    stats.source_end,
                    stats.block_limit,
                    stats.last_status);
        return stats.blocks == 0ul
            && stats.frames == 0ul
            && stats.accepted_bytes == 0ul
            && stats.capture_calls == 0ul
            && stats.capture_bytes == 0ul
            && stats.capture_checksum == 0ul
            && stats.last_left_level == 0u
            && stats.last_right_level == 0u
            && stats.max_left_level == 0u
            && stats.max_right_level == 0u
            && stats.source_end == 0
            && stats.block_limit == 0
            && stats.last_status == IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE
            ? 0 : 5;
    }

    if (argc != 2 && argc != 3 && argc != 4) return 2;
    if (argc == 3 && std::strcmp(argv[2], "--until-end") == 0) {
        until_end = 1;
    } else if (argc == 3 && std::strcmp(argv[2], "--replay-same-path") == 0) {
        replay_same_path = 1;
    } else if (argc == 3 && std::strcmp(argv[2], "--hook-audio") == 0) {
        hook_audio = 1;
    } else if (argc == 3 && std::strcmp(argv[2], "--hook-audio-until-end") == 0) {
        hook_audio_until_end = 1;
    } else if (argc == 3 && std::strcmp(argv[2], "--hook-audio-null-writer") == 0) {
        hook_audio_null_writer = 1;
    } else if (argc == 3 && std::strcmp(argv[2], "--hook-audio-one-block") == 0) {
        hook_audio_one_block = 1;
    } else if (argc == 3 && std::strcmp(argv[2], "--unavailable") == 0) {
        unavailable_probe = 1;
    } else if (argc == 4 && std::strcmp(argv[2], "--switch-path") == 0) {
        switch_path = 1;
    } else if (argc == 4 && std::strcmp(argv[2], "--bad-then-good") == 0) {
        bad_then_good = 1;
    } else if (argc != 2) {
        return 2;
    }
    reset_block(&first);
    reset_block(&second);
    module.path = argv[1];

    decoder = iplay_modplug_external_decoder_create();
    if (!decoder) return 3;
    iplay_modplug_external_decoder_install(decoder);
    if (!installed_render || !installed_user || !installed_provider) return 4;

    if (until_end) {
        ended = run_until_end(installed_render, installed_user, &module, &end_blocks, &end_frames, &end_checksum, &end_active_bytes, &end_partial_blocks, &end_status, 1024u);
    } else if (switch_path) {
        switched = run_switch_path(installed_render,
                                   installed_user,
                                   argv[1],
                                   argv[3],
                                   &switch_first_status,
                                   &switch_second_status,
                                   &switch_first_checksum,
                                   &switch_second_checksum);
    } else if (replay_same_path) {
        ended = run_until_end(installed_render, installed_user, &module, &replay_blocks, &replay_frames, &replay_checksum, &replay_active_bytes, &replay_partial_blocks, &replay_end_status, 1024u);
        replay_module.path = argv[1];
        reset_block(&first);
        replay_status = installed_render(installed_user, &replay_module, &first);
        replay_first_checksum = checksum_block(&first);
    } else if (bad_then_good) {
        recovered = run_bad_then_good(installed_render, installed_user, argv[1], argv[3], &bad_status, &good_status, &good_checksum);
    } else if (hook_audio || hook_audio_until_end || hook_audio_null_writer || hook_audio_one_block) {
        audio_ok = run_hook_audio(decoder,
                                  argv[1],
                                  &audio_blocks,
                                  &audio_frames,
                                  &audio_accepted,
                                  &audio_capture_calls,
                                  &audio_capture_bytes,
                                  &audio_capture_checksum,
                                  &audio_left,
                                  &audio_right,
                                  &audio_max_left,
                                  &audio_max_right,
                                  &audio_source_end,
                                  &audio_block_limit,
                                  &audio_last_status,
                                  hook_audio_until_end ? 16384ul : (hook_audio_one_block ? 1ul : 8ul),
                                  hook_audio_until_end,
                                  hook_audio_null_writer);
    } else if (unavailable_probe) {
        unavailable_status = installed_render(installed_user, &module, &first);
        unavailable_checksum = checksum_block(&first);
        unavailable_frames = first.frames;
        unavailable_active_bytes = iplay_player_playback_block_active_bytes(&first);
    } else {
        first_status = installed_render(installed_user, &module, &first);
        second_status = installed_render(installed_user, &module, &second);
        first_checksum = checksum_block(&first);
        second_checksum = checksum_block(&second);
    }
    provider_before_uninstall = installed_provider;
    iplay_modplug_external_decoder_uninstall();
    if (until_end) {
        std::printf("provider=libmodplug ended=%d blocks=%lu frames=%lu active_bytes=%lu checksum=%lu partial_blocks=%lu last_status=%d after_uninstall_render=%s after_uninstall_provider=%s\n",
                    ended,
                    end_blocks,
                    end_frames,
                    end_active_bytes,
                    end_checksum,
                    end_partial_blocks,
                    end_status,
                    installed_render ? "set" : "none",
                    installed_provider ? installed_provider : "null");
        iplay_modplug_external_decoder_destroy(decoder);
        return ended
            && end_status == IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED
            && end_blocks > 1ul
            && end_frames > 512ul
            && end_active_bytes == end_frames * 4ul
            && end_checksum != 0ul
            && installed_render == 0
            ? 0 : 5;
    }
    if (bad_then_good) {
        std::printf("provider=%s recovered=%d bad_status=%d good_status=%d good_checksum=%lu after_uninstall_render=%s after_uninstall_provider=%s\n",
                    provider_before_uninstall ? provider_before_uninstall : "null",
                    recovered,
                    bad_status,
                    good_status,
                    good_checksum,
                    installed_render ? "set" : "none",
                    installed_provider ? installed_provider : "null");
        iplay_modplug_external_decoder_destroy(decoder);
        return recovered && installed_render == 0 ? 0 : 5;
    }
    if (hook_audio || hook_audio_until_end || hook_audio_null_writer || hook_audio_one_block) {
        std::printf("provider=%s audio_ok=%d blocks=%lu frames=%lu accepted=%lu capture_calls=%lu capture_bytes=%lu capture_checksum=%lu levels=%u,%u maxlevels=%u,%u source_end=%d block_limit=%d last_status=%d after_uninstall_render=%s after_uninstall_provider=%s\n",
                    provider_before_uninstall ? provider_before_uninstall : "null",
                    audio_ok,
                    audio_blocks,
                    audio_frames,
                    audio_accepted,
                    audio_capture_calls,
                    audio_capture_bytes,
                    audio_capture_checksum,
                    audio_left,
                    audio_right,
                    audio_max_left,
                    audio_max_right,
                    audio_source_end,
                    audio_block_limit,
                    audio_last_status,
                    installed_render ? "set" : "none",
                    installed_provider ? installed_provider : "null");
        iplay_modplug_external_decoder_destroy(decoder);
        return audio_ok && installed_render == 0 ? 0 : 5;
    }
    if (unavailable_probe) {
        std::printf("provider=%s unavailable_status=%d frames=%u active_bytes=%u checksum=%lu after_uninstall_render=%s after_uninstall_provider=%s\n",
                    provider_before_uninstall ? provider_before_uninstall : "null",
                    unavailable_status,
                    unavailable_frames,
                    unavailable_active_bytes,
                    unavailable_checksum,
                    installed_render ? "set" : "none",
                    installed_provider ? installed_provider : "null");
        iplay_modplug_external_decoder_destroy(decoder);
        return unavailable_status == IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE
            && unavailable_frames == 512u
            && unavailable_active_bytes == 2048u
            && unavailable_checksum == 0ul
            && installed_render == 0
            ? 0 : 5;
    }
    if (replay_same_path) {
        std::printf("provider=%s ended=%d end_status=%d replay_status=%d replay_blocks=%lu replay_frames=%lu replay_active_bytes=%lu replay_checksum=%lu replay_partial_blocks=%lu replay_first_checksum=%lu after_uninstall_render=%s after_uninstall_provider=%s\n",
                    provider_before_uninstall ? provider_before_uninstall : "null",
                    ended,
                    replay_end_status,
                    replay_status,
                    replay_blocks,
                    replay_frames,
                    replay_active_bytes,
                    replay_checksum,
                    replay_partial_blocks,
                    replay_first_checksum,
                    installed_render ? "set" : "none",
                    installed_provider ? installed_provider : "null");
        iplay_modplug_external_decoder_destroy(decoder);
        return ended
            && replay_end_status == IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED
            && replay_status == IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED
            && replay_blocks > 1ul
            && replay_frames > 512ul
            && replay_active_bytes == replay_frames * 4ul
            && replay_checksum != 0ul
            && replay_first_checksum != 0ul
            && installed_render == 0
            ? 0 : 5;
    }
    if (switch_path) {
        std::printf("provider=%s switched=%d first_status=%d second_status=%d first_checksum=%lu second_checksum=%lu after_uninstall_render=%s after_uninstall_provider=%s\n",
                    provider_before_uninstall ? provider_before_uninstall : "null",
                    switched,
                    switch_first_status,
                    switch_second_status,
                    switch_first_checksum,
                    switch_second_checksum,
                    installed_render ? "set" : "none",
                    installed_provider ? installed_provider : "null");
        iplay_modplug_external_decoder_destroy(decoder);
        return switched && installed_render == 0 ? 0 : 5;
    }

    std::printf("provider=%s first_status=%d second_status=%d first_frames=%u second_frames=%u first_checksum=%lu second_checksum=%lu after_uninstall_render=%s after_uninstall_provider=%s\n",
                provider_before_uninstall ? provider_before_uninstall : "null",
                first_status,
                second_status,
                (unsigned)first.frames,
                (unsigned)second.frames,
                first_checksum,
                second_checksum,
                installed_render ? "set" : "none",
                installed_provider ? installed_provider : "null");

    iplay_modplug_external_decoder_destroy(decoder);
    return first_status == IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED
        && second_status == IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED
        && first.frames == 512u
        && second.frames == 512u
        && first_checksum != 0ul
        && second_checksum != 0ul
        && first_checksum != second_checksum
        && installed_render == 0
        ? 0 : 5;
}
