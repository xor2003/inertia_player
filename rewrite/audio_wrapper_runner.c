#include <stdio.h>
#include <string.h>

#include "iplay_rewrite.h"

typedef struct AudioCapture {
    db data[32];
    dw bytes;
} AudioCapture;

static db cells[IPLAY_TEXT_FALLBACK_SCREEN_BYTES];

static void capture_audio_write(void *user, const db *pcm, dw byte_count) {
    AudioCapture *capture = (AudioCapture *)user;
    dw i;
    for (i = 0; i < byte_count && capture->bytes < sizeof(capture->data); ++i) {
        capture->data[capture->bytes++] = pcm[i];
    }
}

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned count) {
    unsigned i;
    for (i = 0; i < count; ++i) printf("%02x", (unsigned)p[i]);
}

int main(int argc, char **argv) {
    if (argc != 2) return 2;

    if (streq(argv[1], "sdlaudioinitformat")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        printf("backend=%u format=%s rate=%u bits=%u channels=%u signed=%u framebytes=%u samples=%u sb16=%u sdl=%u hw=%u active=%u paused=%u capacity=%lu frames10=%u bytes3=%u captured=%u\n",
               (unsigned)iplay_sdl_audio_device_backend(&device),
               iplay_audio_format_name(iplay_sdl_audio_device_format(&device)),
               (unsigned)iplay_sdl_audio_device_sample_rate(&device),
               (unsigned)iplay_sdl_audio_device_bits_per_sample(&device),
               (unsigned)iplay_sdl_audio_device_channels(&device),
               (unsigned)iplay_sdl_audio_device_signed_samples(&device),
               (unsigned)iplay_sdl_audio_device_bytes_per_frame(&device),
               (unsigned)iplay_sdl_audio_device_samples(&device),
               (unsigned)iplay_sdl_audio_device_is_sb16_compatible(&device),
               (unsigned)iplay_sdl_audio_device_is_sdl_compatible(&device),
               (unsigned)iplay_sdl_audio_device_is_sb16_hardware(&device),
               (unsigned)iplay_sdl_audio_device_active(&device),
               (unsigned)iplay_sdl_audio_device_paused(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device),
               (unsigned)iplay_sdl_audio_device_frames_for_bytes(&device, 10),
               (unsigned)iplay_sdl_audio_device_bytes_for_frames(&device, 3),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "sdlaudiowriteaccepted")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        dw accepted;
        static const db pcm[12] = {
            0x01,0x00, 0x02,0x00,
            0x03,0x00, 0x04,0x00,
            0x05,0x00, 0x06,0x00
        };
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 2);
        accepted = iplay_sdl_audio_device_write_sb16_frames(&device, pcm, 3);
        printf("accepted=%u captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "sdlaudiowritenull")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        dw accepted;
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 2);
        accepted = iplay_sdl_audio_device_write_sb16_frames(&device, 0, 2);
        printf("accepted=%u captured=%u frames=%lu dropped=%lu capacity=%lu\n",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device));
        return 0;
    }

    if (streq(argv[1], "sdlaudiowritesignedlevels")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        dw accepted;
        static const db pcm[8] = {
            0x00,0xc0, 0x00,0xf8,
            0x00,0x80, 0xff,0x7f
        };
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 1);
        accepted = iplay_sdl_audio_device_write_sb16_frames(&device, pcm, 2);
        printf("accepted=%u captured=%u frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device),
               (unsigned)iplay_sdl_audio_device_levels(&device)->left_16,
               (unsigned)iplay_sdl_audio_device_levels(&device)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "sdlaudiocallbackaccepted")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        dw accepted;
        static db stream[10] = {
            0x31,0x00, 0x32,0x00,
            0x33,0x00, 0x34,0x00,
            0xaa,0xbb
        };
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 1);
        accepted = iplay_sdl_audio_device_callback(&device, stream, sizeof(stream));
        printf("accepted=%u captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "sdlaudiocallbackpartial")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        dw accepted;
        dw frames;
        static db stream[6] = {
            0x61,0x00, 0x62,0x00,
            0xcc,0xdd
        };
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 2);
        frames = iplay_sdl_audio_device_frames_for_bytes(&device, sizeof(stream));
        accepted = iplay_sdl_audio_device_callback(&device, stream, sizeof(stream));
        printf("frames_for_bytes=%u accepted=%u captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)frames,
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "sdlaudiocallbacksignedlevels")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        dw accepted;
        static db stream[8] = {
            0x00,0xc0, 0x00,0xf8,
            0x00,0x80, 0xff,0x7f
        };
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 1);
        accepted = iplay_sdl_audio_device_callback(&device, stream, sizeof(stream));
        printf("accepted=%u captured=%u frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device),
               (unsigned)iplay_sdl_audio_device_levels(&device)->left_16,
               (unsigned)iplay_sdl_audio_device_levels(&device)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "sdlaudiocallbackpaused")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        dw accepted;
        static db stream[8] = {
            0x41,0x00, 0x42,0x00,
            0x43,0x00, 0x44,0x00
        };
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_pause(&device, 1);
        iplay_sdl_audio_device_set_capacity(&device, 2);
        accepted = iplay_sdl_audio_device_callback(&device, stream, sizeof(stream));
        printf("accepted=%u captured=%u paused=%u frames=%lu dropped=%lu capacity=%lu\n",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned)iplay_sdl_audio_device_paused(&device),
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device));
        return 0;
    }

    if (streq(argv[1], "sdlaudiocallbacknull")) {
        dw accepted;
        static db stream[8] = {
            0x51,0x00, 0x52,0x00,
            0x53,0x00, 0x54,0x00
        };
        accepted = iplay_sdl_audio_device_callback(0, stream, sizeof(stream));
        printf("accepted=%u\n", (unsigned)accepted);
        return 0;
    }

    if (streq(argv[1], "sdlaudiocallbacknullstream")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        dw accepted;
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 2);
        accepted = iplay_sdl_audio_device_callback(&device, 0, 8);
        printf("accepted=%u captured=%u frames=%lu dropped=%lu capacity=%lu\n",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device));
        return 0;
    }

    if (streq(argv[1], "sdlaudioopenrejectnonsb16")) {
        IplaySdlAudioDevice device;
        IplaySdlAudioDeviceConfig config;
        AudioCapture capture;
        int bad_u8;
        int bad_mono;
        int good;
        dw accepted;
        static const db pcm[4] = {0x44,0x33, 0x22,0x11};
        memset(&device, 0, sizeof(device));
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_config_sb16_stereo(&config, &device, IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);
        iplay_sdl_audio_device_config_set_format(&config, &IPLAY_AUDIO_U8_MONO);
        bad_u8 = iplay_sdl_audio_device_open(&device, &config, capture_audio_write, &capture);
        iplay_sdl_audio_device_config_set_format(&config, &IPLAY_AUDIO_S16_MONO);
        bad_mono = iplay_sdl_audio_device_open(&device, &config, capture_audio_write, &capture);
        iplay_sdl_audio_device_config_set_format(&config, &IPLAY_AUDIO_SB16_STEREO_16);
        good = iplay_sdl_audio_device_open(&device, &config, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 1);
        accepted = iplay_sdl_audio_device_write_sb16_frames(&device, pcm, 1);
        printf("bad_u8=%u bad_mono=%u good=%u format=%s bits=%u channels=%u signed=%u sb16=%u accepted=%u captured=%u frames=%lu data=",
               (unsigned)bad_u8,
               (unsigned)bad_mono,
               (unsigned)good,
               iplay_audio_format_name(iplay_sdl_audio_device_format(&device)),
               (unsigned)iplay_sdl_audio_device_bits_per_sample(&device),
               (unsigned)iplay_sdl_audio_device_channels(&device),
               (unsigned)iplay_sdl_audio_device_signed_samples(&device),
               (unsigned)iplay_sdl_audio_device_is_sb16_compatible(&device),
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "sdlaudioopenpreservesactive")) {
        IplaySdlAudioDevice device;
        IplaySdlAudioDeviceConfig config;
        AudioCapture capture;
        int good;
        int bad_reopen;
        dw accepted;
        static const db pcm[4] = {0x7a,0x56, 0x34,0x12};
        memset(&device, 0, sizeof(device));
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_config_sb16_stereo(&config, &device, IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);
        good = iplay_sdl_audio_device_open(&device, &config, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 1);
        iplay_sdl_audio_device_config_set_format(&config, &IPLAY_AUDIO_U8_MONO);
        bad_reopen = iplay_sdl_audio_device_open(&device, &config, capture_audio_write, &capture);
        accepted = iplay_sdl_audio_device_write_sb16_frames(&device, pcm, 1);
        printf("good=%u bad_reopen=%u format=%s bits=%u channels=%u signed=%u sb16=%u paused=%u accepted=%u captured=%u frames=%lu capacity=%lu data=",
               (unsigned)good,
               (unsigned)bad_reopen,
               iplay_audio_format_name(iplay_sdl_audio_device_format(&device)),
               (unsigned)iplay_sdl_audio_device_bits_per_sample(&device),
               (unsigned)iplay_sdl_audio_device_channels(&device),
               (unsigned)iplay_sdl_audio_device_signed_samples(&device),
               (unsigned)iplay_sdl_audio_device_is_sb16_compatible(&device),
               (unsigned)iplay_sdl_audio_device_paused(&device),
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimewriteaccepted")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[12] = {
            0x11,0x00, 0x12,0x00,
            0x13,0x00, 0x14,0x00,
            0x15,0x00, 0x16,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 3);
        printf("accepted=%u captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimeinitformat")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        printf("backend=%u format=%s rate=%u bits=%u channels=%u signed=%u framebytes=%u samples=%u sb16=%u sdl=%u sbhw=%u hw=%u active=%u paused=%u capacity=%lu frames10=%u bytes3=%u captured=%u\n",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               iplay_audio_format_name(iplay_runtime_audio_format(&runtime)),
               (unsigned)iplay_runtime_audio_sample_rate(&runtime),
               (unsigned)iplay_runtime_audio_bits_per_sample(&runtime),
               (unsigned)iplay_runtime_audio_channels(&runtime),
               (unsigned)iplay_runtime_audio_signed_samples(&runtime),
               (unsigned)iplay_runtime_audio_bytes_per_frame(&runtime),
               (unsigned)iplay_runtime_audio_samples(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_compatible(&runtime),
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_frames_for_bytes(&runtime, 10),
               (unsigned)iplay_runtime_audio_bytes_for_frames(&runtime, 3),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimeinitcounters")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        printf("sdl=%u hw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimeaudiostartclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        printf("sdl=%u hw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimeaudiostopclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_stop(&runtime);
        printf("sdl=%u hw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimeaudiopauseclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_pause(&runtime, 1);
        printf("sdl=%u hw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimeaudiopauseresumeclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_pause(&runtime, 1);
        iplay_runtime_audio_pause(&runtime, 0);
        printf("sdl=%u hw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimeaudiocapacityclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        printf("sdl=%u hw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimeaudioaddcapacityclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_audio_add_capacity(&runtime, 3);
        printf("sdl=%u hw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimeaudioclearqueuedclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 4);
        iplay_runtime_audio_clear_queued(&runtime);
        printf("sdl=%u hw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimeaudioresetcountersclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 4);
        iplay_runtime_audio_reset_counters(&runtime);
        printf("sdl=%u hw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimewritenull")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, 0, 2);
        printf("accepted=%u captured=%u frames=%lu dropped=%lu capacity=%lu\n",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        return 0;
    }

    if (streq(argv[1], "runtimeaudioopenpreservesactive")) {
        IplayRuntime runtime;
        IplayRuntimeConfig runtime_config;
        IplaySdlAudioDeviceConfig bad_config;
        AudioCapture capture;
        int bad_reopen;
        dw accepted;
        static const db pcm[4] = {0x22,0x11, 0x44,0x33};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&runtime_config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &runtime_config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        iplay_sdl_audio_device_config_sb16_stereo(&bad_config, iplay_runtime_audio(&runtime), IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);
        iplay_sdl_audio_device_config_set_format(&bad_config, &IPLAY_AUDIO_U8_MONO);
        bad_reopen = iplay_sdl_audio_device_open(iplay_runtime_audio(&runtime), &bad_config, capture_audio_write, &capture);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 1);
        printf("bad_reopen=%u format=%s bits=%u channels=%u signed=%u sb16=%u sdl=%u hw=%u active=%u paused=%u accepted=%u captured=%u frames=%lu capacity=%lu data=",
               (unsigned)bad_reopen,
               iplay_audio_format_name(iplay_runtime_audio_format(&runtime)),
               (unsigned)iplay_runtime_audio_bits_per_sample(&runtime),
               (unsigned)iplay_runtime_audio_channels(&runtime),
               (unsigned)iplay_runtime_audio_signed_samples(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_compatible(&runtime),
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimesignedlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[8] = {
            0x00,0x80, 0x00,0x10,
            0x00,0x20, 0x00,0x90
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 2);
        printf("sdl=%u hw=%u accepted=%u active=%u captured=%u frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimequeuepartial")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw frames;
        dw queued;
        static const db stream[6] = {
            0x71,0x00, 0x72,0x00,
            0xee,0xff
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        frames = iplay_runtime_audio_frames_for_bytes(&runtime, sizeof(stream));
        queued = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        printf("frames_for_bytes=%u queued=%u captured=%u queued_frames=%lu queued_bytes=%lu frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)frames,
               (unsigned)queued,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimequeuesignedlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw queued;
        static const db stream[8] = {
            0x00,0xc0, 0x00,0xf8,
            0x00,0x80, 0xff,0x7f
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        queued = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        printf("queued=%u captured=%u queued_frames=%lu queued_bytes=%lu frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)queued,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimequeuepaused")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw paused_queue;
        dw live_queue;
        static const db stream[4] = {0x91,0x00, 0x92,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_audio_pause(&runtime, 1);
        paused_queue = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        iplay_runtime_audio_pause(&runtime, 0);
        live_queue = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        printf("paused_queue=%u live_queue=%u paused=%u captured=%u queued_frames=%lu queued_bytes=%lu frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)paused_queue,
               (unsigned)live_queue,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimepausepreserveslevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw first;
        dw paused;
        static const db first_pcm[4] = {0xff,0x7f, 0x00,0x80};
        static const db paused_pcm[4] = {0x00,0x00, 0x00,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        first = iplay_runtime_write_sb16_frames(&runtime, first_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("sdl=%u hw=%u first=%u before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)first,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_pause(&runtime, 1);
        paused = iplay_runtime_write_sb16_frames(&runtime, paused_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" paused=%u paused_flag=%u active=%u after=%u,%u after_l=",
               (unsigned)paused,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimepauseresumelevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw first;
        dw paused;
        dw live;
        static const db first_pcm[4] = {0xff,0x7f, 0x00,0x80};
        static const db paused_pcm[4] = {0x11,0x11, 0x22,0x22};
        static const db live_pcm[4] = {0x00,0x00, 0x00,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        first = iplay_runtime_write_sb16_frames(&runtime, first_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("sdl=%u hw=%u first=%u before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)first,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_pause(&runtime, 1);
        paused = iplay_runtime_write_sb16_frames(&runtime, paused_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" paused=%u paused_levels=%u,%u paused_l=",
               (unsigned)paused,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" paused_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_pause(&runtime, 0);
        live = iplay_runtime_write_sb16_frames(&runtime, live_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" live=%u paused_flag=%u active=%u after=%u,%u after_l=",
               (unsigned)live,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimequeuestopstart")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw stopped_queue;
        dw live_queue;
        static const db stream[4] = {0xb1,0x00, 0xb2,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_audio_stop(&runtime);
        stopped_queue = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        iplay_runtime_audio_start(&runtime);
        live_queue = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        printf("stopped_queue=%u live_queue=%u active=%u captured=%u queued_frames=%lu queued_bytes=%lu frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)stopped_queue,
               (unsigned)live_queue,
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimestoppreserveslevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw first;
        dw stopped;
        static const db first_pcm[4] = {0xff,0x7f, 0x00,0x80};
        static const db stopped_pcm[4] = {0x00,0x00, 0x00,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        first = iplay_runtime_write_sb16_frames(&runtime, first_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("sdl=%u hw=%u first=%u before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)first,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_stop(&runtime);
        stopped = iplay_runtime_write_sb16_frames(&runtime, stopped_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" stopped=%u paused_flag=%u active=%u after=%u,%u after_l=",
               (unsigned)stopped,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimestopstartlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw first;
        dw stopped;
        dw live;
        static const db first_pcm[4] = {0xff,0x7f, 0x00,0x80};
        static const db stopped_pcm[4] = {0x11,0x11, 0x22,0x22};
        static const db live_pcm[4] = {0x00,0x00, 0x00,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        first = iplay_runtime_write_sb16_frames(&runtime, first_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("sdl=%u hw=%u first=%u before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)first,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_stop(&runtime);
        stopped = iplay_runtime_write_sb16_frames(&runtime, stopped_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" stopped=%u stopped_levels=%u,%u stopped_l=",
               (unsigned)stopped,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" stopped_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_start(&runtime);
        live = iplay_runtime_write_sb16_frames(&runtime, live_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" live=%u paused_flag=%u active=%u after=%u,%u after_l=",
               (unsigned)live,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimeclearqueuedlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw queued;
        dd queued_before_frames;
        dd queued_before_bytes;
        dd queued_after_frames;
        dd queued_after_bytes;
        static const db pcm[4] = {0xff,0x7f, 0x00,0x80};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        queued = iplay_runtime_audio_queue(&runtime, pcm, sizeof(pcm));
        queued_before_frames = iplay_runtime_audio_queued_frames(&runtime);
        queued_before_bytes = iplay_runtime_audio_queued_bytes(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("sdl=%u hw=%u queued=%u before=%lu,%lu levels_before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)queued,
               (unsigned long)queued_before_frames,
               (unsigned long)queued_before_bytes,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_clear_queued(&runtime);
        queued_after_frames = iplay_runtime_audio_queued_frames(&runtime);
        queued_after_bytes = iplay_runtime_audio_queued_bytes(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" after_clear=%lu,%lu levels_clear=%u,%u clear_l=",
               (unsigned long)queued_after_frames,
               (unsigned long)queued_after_bytes,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" clear_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_reset_levels(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" after_reset=%u,%u reset_l=",
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" reset_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimeresetcounterslevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[12] = {
            0xff,0x7f, 0x00,0x80,
            0x00,0x40, 0x00,0xc0,
            0x11,0x00, 0x22,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 3);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("sdl=%u hw=%u accepted=%u before=%lu,%lu,%lu,%u,%u before_l=",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_reset_counters(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" after=%lu,%lu,%lu,%u,%u after_l=",
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" active=%u captured=%u data=",
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimeresetunderrunlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_write_silence(&runtime, 2);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("sdl=%u hw=%u before=%lu,%lu,%lu,%u,%u before_l=",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_reset_counters(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" after=%lu,%lu,%lu,%u,%u after_l=",
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" active=%u captured=%u data=",
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwwriteaccepted")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[12] = {
            0x21,0x00, 0x22,0x00,
            0x23,0x00, 0x24,0x00,
            0x25,0x00, 0x26,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 3);
        printf("accepted=%u captured=%u backend=%u hw=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwinitformat")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        printf("backend=%u format=%s rate=%u bits=%u channels=%u signed=%u framebytes=%u samples=%u sb16=%u sdl=%u sbhw=%u hw=%u active=%u paused=%u capacity=%lu frames10=%u bytes3=%u captured=%u\n",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               iplay_audio_format_name(iplay_runtime_audio_format(&runtime)),
               (unsigned)iplay_runtime_audio_sample_rate(&runtime),
               (unsigned)iplay_runtime_audio_bits_per_sample(&runtime),
               (unsigned)iplay_runtime_audio_channels(&runtime),
               (unsigned)iplay_runtime_audio_signed_samples(&runtime),
               (unsigned)iplay_runtime_audio_bytes_per_frame(&runtime),
               (unsigned)iplay_runtime_audio_samples(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_compatible(&runtime),
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_frames_for_bytes(&runtime, 10),
               (unsigned)iplay_runtime_audio_bytes_for_frames(&runtime, 3),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwstatustext")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        const char *initial_status;
        const char *started_status;
        const char *paused_status;
        const char *resumed_status;
        const char *stopped_status;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        initial_status = iplay_runtime_audio_status_text(&runtime);
        iplay_runtime_audio_start(&runtime);
        started_status = iplay_runtime_audio_status_text(&runtime);
        iplay_runtime_audio_pause(&runtime, 1);
        paused_status = iplay_runtime_audio_status_text(&runtime);
        iplay_runtime_audio_pause(&runtime, 0);
        resumed_status = iplay_runtime_audio_status_text(&runtime);
        iplay_runtime_audio_stop(&runtime);
        stopped_status = iplay_runtime_audio_status_text(&runtime);
        printf("backend=%u hw=%u sbhw=%u active=%u paused=%u initial=%s started=%s paused_text=%s resumed=%s stopped=%s captured=%u\n",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               initial_status,
               started_status,
               paused_status,
               resumed_status,
               stopped_status,
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwinitcounters")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        printf("sdl=%u hw=%u sbhw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwaudiostartclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        printf("sdl=%u hw=%u sbhw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwaudiostopclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_stop(&runtime);
        printf("sdl=%u hw=%u sbhw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwaudiopauseclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_pause(&runtime, 1);
        printf("sdl=%u hw=%u sbhw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwaudiopauseresumeclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_pause(&runtime, 1);
        iplay_runtime_audio_pause(&runtime, 0);
        printf("sdl=%u hw=%u sbhw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwaudiocapacityclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        printf("sdl=%u hw=%u sbhw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwaudioaddcapacityclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_audio_add_capacity(&runtime, 3);
        printf("sdl=%u hw=%u sbhw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwaudioclearqueuedclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 4);
        iplay_runtime_audio_clear_queued(&runtime);
        printf("sdl=%u hw=%u sbhw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwaudioresetcountersclean")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 4);
        iplay_runtime_audio_reset_counters(&runtime);
        printf("sdl=%u hw=%u sbhw=%u active=%u paused=%u can_queue=%u frames=%lu dropped=%lu underrun=%lu queued_frames=%lu queued_bytes=%lu capacity=%lu captured=%u\n",
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_can_queue(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "runtimehwwritenull")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, 0, 2);
        printf("accepted=%u captured=%u backend=%u hw=%u frames=%lu dropped=%lu capacity=%lu\n",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        return 0;
    }

    if (streq(argv[1], "runtimehwaudioopenpreservesactive")) {
        IplayRuntime runtime;
        IplayRuntimeConfig runtime_config;
        IplaySdlAudioDeviceConfig bad_config;
        AudioCapture capture;
        int bad_reopen;
        dw accepted;
        static const db pcm[4] = {0x66,0x55, 0x88,0x77};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&runtime_config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &runtime_config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        iplay_sdl_audio_device_config_sb16_stereo(&bad_config, iplay_runtime_audio(&runtime), IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);
        iplay_sdl_audio_device_config_set_format(&bad_config, &IPLAY_AUDIO_U8_MONO);
        bad_reopen = iplay_sdl_audio_device_open(iplay_runtime_audio(&runtime), &bad_config, capture_audio_write, &capture);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 1);
        printf("bad_reopen=%u format=%s bits=%u channels=%u signed=%u sb16=%u sdl=%u sbhw=%u backend=%u hw=%u active=%u paused=%u accepted=%u captured=%u frames=%lu capacity=%lu data=",
               (unsigned)bad_reopen,
               iplay_audio_format_name(iplay_runtime_audio_format(&runtime)),
               (unsigned)iplay_runtime_audio_bits_per_sample(&runtime),
               (unsigned)iplay_runtime_audio_channels(&runtime),
               (unsigned)iplay_runtime_audio_signed_samples(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_compatible(&runtime),
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_hardware(&runtime),
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwqueuepartial")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw frames;
        dw queued;
        static const db stream[6] = {
            0x81,0x00, 0x82,0x00,
            0xde,0xad
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        frames = iplay_runtime_audio_frames_for_bytes(&runtime, sizeof(stream));
        queued = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        printf("backend=%u hw=%u frames_for_bytes=%u queued=%u captured=%u queued_frames=%lu queued_bytes=%lu frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)frames,
               (unsigned)queued,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwqueuesignedlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw queued;
        static const db stream[8] = {
            0x00,0xc0, 0x00,0xf8,
            0x00,0x80, 0xff,0x7f
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        queued = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        printf("backend=%u hw=%u queued=%u captured=%u queued_frames=%lu queued_bytes=%lu frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)queued,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwqueuepaused")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw paused_queue;
        dw live_queue;
        static const db stream[4] = {0xa1,0x00, 0xa2,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_audio_pause(&runtime, 1);
        paused_queue = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        iplay_runtime_audio_pause(&runtime, 0);
        live_queue = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        printf("backend=%u hw=%u paused_queue=%u live_queue=%u paused=%u captured=%u queued_frames=%lu queued_bytes=%lu frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)paused_queue,
               (unsigned)live_queue,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwwritepaused")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[8] = {
            0xd1,0x00, 0xd2,0x00,
            0xd3,0x00, 0xd4,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_audio_pause(&runtime, 1);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 2);
        printf("backend=%u hw=%u accepted=%u active=%u paused=%u captured=%u frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwpausepreserveslevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw first;
        dw paused;
        static const db first_pcm[4] = {0xff,0x7f, 0x00,0x80};
        static const db paused_pcm[4] = {0x00,0x00, 0x00,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        first = iplay_runtime_write_sb16_frames(&runtime, first_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("backend=%u hw=%u first=%u before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)first,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_pause(&runtime, 1);
        paused = iplay_runtime_write_sb16_frames(&runtime, paused_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" paused=%u paused_flag=%u active=%u after=%u,%u after_l=",
               (unsigned)paused,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwpauseresumelevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw first;
        dw paused;
        dw live;
        static const db first_pcm[4] = {0xff,0x7f, 0x00,0x80};
        static const db paused_pcm[4] = {0x11,0x11, 0x22,0x22};
        static const db live_pcm[4] = {0x00,0x00, 0x00,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        first = iplay_runtime_write_sb16_frames(&runtime, first_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("backend=%u hw=%u first=%u before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)first,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_pause(&runtime, 1);
        paused = iplay_runtime_write_sb16_frames(&runtime, paused_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" paused=%u paused_levels=%u,%u paused_l=",
               (unsigned)paused,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" paused_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_pause(&runtime, 0);
        live = iplay_runtime_write_sb16_frames(&runtime, live_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" live=%u paused_flag=%u active=%u after=%u,%u after_l=",
               (unsigned)live,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwwritestopped")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[8] = {
            0xe1,0x00, 0xe2,0x00,
            0xe3,0x00, 0xe4,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_audio_stop(&runtime);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 2);
        printf("backend=%u hw=%u accepted=%u active=%u paused=%u captured=%u frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwstoppreserveslevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw first;
        dw stopped;
        static const db first_pcm[4] = {0xff,0x7f, 0x00,0x80};
        static const db stopped_pcm[4] = {0x00,0x00, 0x00,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        first = iplay_runtime_write_sb16_frames(&runtime, first_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("backend=%u hw=%u first=%u before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)first,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_stop(&runtime);
        stopped = iplay_runtime_write_sb16_frames(&runtime, stopped_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" stopped=%u paused_flag=%u active=%u after=%u,%u after_l=",
               (unsigned)stopped,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwstopstartlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw first;
        dw stopped;
        dw live;
        static const db first_pcm[4] = {0xff,0x7f, 0x00,0x80};
        static const db stopped_pcm[4] = {0x11,0x11, 0x22,0x22};
        static const db live_pcm[4] = {0x00,0x00, 0x00,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        first = iplay_runtime_write_sb16_frames(&runtime, first_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("backend=%u hw=%u first=%u before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)first,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_stop(&runtime);
        stopped = iplay_runtime_write_sb16_frames(&runtime, stopped_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" stopped=%u stopped_levels=%u,%u stopped_l=",
               (unsigned)stopped,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" stopped_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_start(&runtime);
        live = iplay_runtime_write_sb16_frames(&runtime, live_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" live=%u paused_flag=%u active=%u after=%u,%u after_l=",
               (unsigned)live,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwwritepauseresume")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw paused_accepted;
        dw live_accepted;
        static const db paused_pcm[4] = {0xf1,0x00, 0xf2,0x00};
        static const db live_pcm[4] = {0xf3,0x00, 0xf4,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        iplay_runtime_audio_pause(&runtime, 1);
        paused_accepted = iplay_runtime_write_sb16_frames(&runtime, paused_pcm, 1);
        iplay_runtime_audio_pause(&runtime, 0);
        live_accepted = iplay_runtime_write_sb16_frames(&runtime, live_pcm, 1);
        printf("backend=%u hw=%u paused_accepted=%u live_accepted=%u active=%u paused=%u captured=%u frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)paused_accepted,
               (unsigned)live_accepted,
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwqueuestopstart")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw stopped_queue;
        dw live_queue;
        static const db stream[4] = {0xc1,0x00, 0xc2,0x00};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_audio_stop(&runtime);
        stopped_queue = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        iplay_runtime_audio_start(&runtime);
        live_queue = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));
        printf("backend=%u hw=%u stopped_queue=%u live_queue=%u active=%u captured=%u queued_frames=%lu queued_bytes=%lu frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)stopped_queue,
               (unsigned)live_queue,
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_queued_frames(&runtime),
               (unsigned long)iplay_runtime_audio_queued_bytes(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwshutdown")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        const IplayAudioLevels *levels;
        dw accepted;
        static const db pcm[12] = {
            0xff,0x7f, 0x00,0x80,
            0x00,0x40, 0x00,0xc0,
            0x11,0x00, 0x22,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 3);
        levels = iplay_runtime_audio_levels(&runtime);
        printf("accepted=%u captured=%u active_before=%u levels_before=%u,%u",
               (unsigned)accepted,
               (unsigned)capture.bytes,
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)levels->left_16,
               (unsigned)levels->right_16);
        iplay_runtime_shutdown(&runtime);
        levels = iplay_runtime_audio_levels(&runtime);
        printf(" active_after=%u levels_after=%u,%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)levels->left_16,
               (unsigned)levels->right_16,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimelevelsdisplay80x50")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        int ok;
        static const db high_pcm[4] = {0xff,0x7f, 0x00,0x80};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        accepted = iplay_runtime_write_sb16_frames(&runtime, high_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 48, 62, 16);
        printf("ok=%u flag=%u sdl=%u hw=%u accepted=%u levels=%u,%u high_l=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 48, 62), 32);
        printf(" high_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 62), 32);
        printf(" guard=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 78), 4);
        printf(" captured=%u data=",
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimelevelsreset80x50")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        int ok;
        static const db pcm[4] = {0xff,0x7f, 0x00,0x80};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sdl_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 48, 62, 16);
        printf("ok=%u flag=%u sdl=%u hw=%u accepted=%u before=%u,%u before_l=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               (unsigned)iplay_runtime_audio_is_sdl_compatible(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 48, 62), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 62), 32);
        iplay_runtime_audio_reset_levels(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 48, 62, 16);
        printf(" after=%u,%u after_l=",
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 48, 62), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 62), 32);
        printf(" guard=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 78), 4);
        printf(" captured=%u data=",
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwlevelsdisplay")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted_low;
        dw accepted_high;
        static const db low_pcm[4] = {0x00,0x00, 0x00,0x00};
        static const db high_pcm[4] = {0xff,0x7f, 0x00,0x80};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        accepted_low = iplay_runtime_write_sb16_frames(&runtime, low_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        accepted_high = iplay_runtime_write_sb16_frames(&runtime, high_pcm, 1);
        printf("accepted=%u,%u levels=%u,%u low_l=",
               (unsigned)accepted_low,
               (unsigned)accepted_high,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" low_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" high_l=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" high_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u data=",
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwlevelsdisplay80x50")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        int ok;
        static const db high_pcm[4] = {0xff,0x7f, 0x00,0x80};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        accepted = iplay_runtime_write_sb16_frames(&runtime, high_pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 48, 62, 16);
        printf("ok=%u flag=%u accepted=%u levels=%u,%u high_l=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               (unsigned)accepted,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 48, 62), 32);
        printf(" high_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 62), 32);
        printf(" guard=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 78), 4);
        printf(" captured=%u data=",
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwresetlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[4] = {0xff,0x7f, 0x00,0x80};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("backend=%u hw=%u accepted=%u before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_reset_levels(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" after=%u,%u after_l=",
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" active=%u captured=%u frames=%lu dropped=%lu data=",
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwlevelsreset80x50")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        int ok;
        static const db pcm[4] = {0xff,0x7f, 0x00,0x80};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 1);
        iplay_runtime_draw_audio_levels(&runtime, 48, 62, 16);
        printf("ok=%u flag=%u backend=%u hw=%u accepted=%u before=%u,%u before_l=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 48, 62), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 62), 32);
        iplay_runtime_audio_reset_levels(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 48, 62, 16);
        printf(" after=%u,%u after_l=",
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 48, 62), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 62), 32);
        printf(" guard=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 78), 4);
        printf(" active=%u captured=%u frames=%lu dropped=%lu data=",
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwsignedlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[8] = {
            0x00,0x80, 0x00,0x10,
            0x00,0x20, 0x00,0x90
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 2);
        printf("backend=%u hw=%u accepted=%u active=%u captured=%u frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwwritesilence")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        iplay_runtime_write_silence(&runtime, 3);
        printf("backend=%u hw=%u active=%u captured=%u frames=%lu underrun=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwstoppedsilence")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        iplay_runtime_audio_stop(&runtime);
        iplay_runtime_write_silence(&runtime, 3);
        printf("backend=%u hw=%u active=%u captured=%u frames=%lu underrun=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwpausedsilence")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        iplay_runtime_audio_pause(&runtime, 1);
        iplay_runtime_write_silence(&runtime, 3);
        printf("backend=%u hw=%u active=%u paused=%u captured=%u frames=%lu underrun=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwcapacityrefill")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw first;
        dw second;
        static const db pcm1[12] = {
            0x01,0x00, 0x02,0x00,
            0x03,0x00, 0x04,0x00,
            0x05,0x00, 0x06,0x00
        };
        static const db pcm2[8] = {
            0x11,0x00, 0x12,0x00,
            0x13,0x00, 0x14,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        first = iplay_runtime_write_sb16_frames(&runtime, pcm1, 3);
        iplay_runtime_audio_add_capacity(&runtime, 1);
        second = iplay_runtime_write_sb16_frames(&runtime, pcm2, 2);
        printf("accepted=%u,%u captured=%u frames=%lu dropped=%lu capacity=%lu levels=%u,%u data=",
               (unsigned)first,
               (unsigned)second,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwclearqueued")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw queued;
        dd queued_before_frames;
        dd queued_before_bytes;
        dd queued_after_frames;
        dd queued_after_bytes;
        static const db pcm[8] = {
            0x31,0x00, 0x32,0x00,
            0x33,0x00, 0x34,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        queued = iplay_runtime_audio_queue(&runtime, pcm, sizeof(pcm));
        queued_before_frames = iplay_runtime_audio_queued_frames(&runtime);
        queued_before_bytes = iplay_runtime_audio_queued_bytes(&runtime);
        iplay_runtime_audio_clear_queued(&runtime);
        queued_after_frames = iplay_runtime_audio_queued_frames(&runtime);
        queued_after_bytes = iplay_runtime_audio_queued_bytes(&runtime);
        printf("backend=%u hw=%u queued=%u before=%lu,%lu after=%lu,%lu captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)queued,
               (unsigned long)queued_before_frames,
               (unsigned long)queued_before_bytes,
               (unsigned long)queued_after_frames,
               (unsigned long)queued_after_bytes,
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwclearqueuedlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw queued;
        dd queued_before_frames;
        dd queued_before_bytes;
        dd queued_after_frames;
        dd queued_after_bytes;
        static const db pcm[4] = {0xff,0x7f, 0x00,0x80};
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        queued = iplay_runtime_audio_queue(&runtime, pcm, sizeof(pcm));
        queued_before_frames = iplay_runtime_audio_queued_frames(&runtime);
        queued_before_bytes = iplay_runtime_audio_queued_bytes(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("backend=%u hw=%u queued=%u before=%lu,%lu levels_before=%u,%u before_l=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)queued,
               (unsigned long)queued_before_frames,
               (unsigned long)queued_before_bytes,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_clear_queued(&runtime);
        queued_after_frames = iplay_runtime_audio_queued_frames(&runtime);
        queued_after_bytes = iplay_runtime_audio_queued_bytes(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" after_clear=%lu,%lu levels_clear=%u,%u clear_l=",
               (unsigned long)queued_after_frames,
               (unsigned long)queued_after_bytes,
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" clear_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_reset_levels(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" after_reset=%u,%u reset_l=",
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" reset_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" captured=%u frames=%lu dropped=%lu capacity=%lu data=",
               (unsigned)capture.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwresetcounters")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[12] = {
            0x41,0x00, 0x42,0x00,
            0x43,0x00, 0x44,0x00,
            0x45,0x00, 0x46,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 3);
        printf("backend=%u hw=%u accepted=%u before=%lu,%lu,%lu,%u,%u",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        iplay_runtime_audio_reset_counters(&runtime);
        printf(" after=%lu,%lu,%lu,%u,%u active=%u captured=%u data=",
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16,
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwresetcounterslevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        dw accepted;
        static const db pcm[12] = {
            0xff,0x7f, 0x00,0x80,
            0x00,0x40, 0x00,0xc0,
            0x11,0x00, 0x22,0x00
        };
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 3);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("backend=%u hw=%u accepted=%u before=%lu,%lu,%lu,%u,%u before_l=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)accepted,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_reset_counters(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" after=%lu,%lu,%lu,%u,%u after_l=",
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" active=%u captured=%u data=",
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwresetunderrun")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_write_silence(&runtime, 2);
        printf("backend=%u hw=%u before=%lu,%lu,%lu,%u",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes);
        iplay_runtime_audio_reset_counters(&runtime);
        printf(" after=%lu,%lu,%lu,%u active=%u data=",
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)capture.bytes,
               (unsigned)iplay_runtime_audio_active(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimehwresetunderrunlevels")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        memset(&capture, 0, sizeof(capture));
        memset(cells, 0, sizeof(cells));
        iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_write_silence(&runtime, 2);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf("backend=%u hw=%u before=%lu,%lu,%lu,%u,%u before_l=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" before_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        iplay_runtime_audio_reset_counters(&runtime);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        printf(" after=%lu,%lu,%lu,%u,%u after_l=",
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" after_r=");
        print_bytes(cells + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" active=%u captured=%u data=",
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    return 2;
}
