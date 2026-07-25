#include "iplay_rewrite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_OFF 0x100u
#define DST_OFF 0x120u
#define ORIG_DST_OFF 0x9100u
#define DSEG_SCRATCH 0x2800u
#define RUNNER_MEM_SIZE 0xa000u

static db *mem;

typedef struct AudioCapture {
    db data[32];
    dw bytes;
} AudioCapture;

typedef struct VideoCapture {
    dw cols;
    dw rows;
    dw bytes;
    db first[4];
} VideoCapture;

static void capture_audio_write(void *user, const db *pcm, dw byte_count) {
    AudioCapture *capture = (AudioCapture *)user;
    dw i;
    if (byte_count > sizeof(capture->data)) byte_count = sizeof(capture->data);
    for (i = 0; i < byte_count; ++i) capture->data[i] = pcm[i];
    capture->bytes = byte_count;
}

static void capture_video_present(void *user, const db *cells, const IplayTextMode *mode, dw byte_count) {
    VideoCapture *capture = (VideoCapture *)user;
    capture->cols = mode->cols;
    capture->rows = mode->rows;
    capture->bytes = byte_count;
    capture->first[0] = cells[0];
    capture->first[1] = cells[1];
    capture->first[2] = cells[2];
    capture->first[3] = cells[3];
}

static unsigned long parse_u32(const char *s) { return strtoul(s, 0, 0); }

static void print_bytes(const db *p, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

static void store_dword(db *p, dd value) {
    p[0] = (db)value;
    p[1] = (db)(value >> 8);
    p[2] = (db)(value >> 16);
    p[3] = (db)(value >> 24);
}

static int streq(const char *a, const char *b) { return strcmp(a, b) == 0; }

static int effop(const char *a, const char *b) {
    if (a[0] == 'a' && a[1] == 'b' && a[2] == 'i') a += 3;
    return strcmp(a, b) == 0;
}

static int hex_nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static size_t parse_hex_bytes(const char *s, db *out, size_t max) {
    size_t n = 0;
    while (s[0] != 0 && s[1] != 0 && n < max) {
        int hi = hex_nibble(s[0]);
        int lo = hex_nibble(s[1]);
        if (hi < 0 || lo < 0) break;
        out[n++] = (db)((hi << 4) | lo);
        s += 2;
    }
    return n;
}

static int read_text_file_arg(const char *arg, char *out, size_t out_size) {
    FILE *fp;
    size_t n;
    if (arg[0] != '@') return 0;
    fp = fopen(arg + 1, "rb");
    if (!fp) return -1;
    n = fread(out, 1, out_size - 1, fp);
    fclose(fp);
    while (n != 0 && (out[n - 1] == '\n' || out[n - 1] == '\r')) --n;
    out[n] = 0;
    return 1;
}

static void join_args(char *out, size_t out_size, int argc, char **argv, int first) {
    int i;
    size_t used = 0;
    if (out_size == 0) return;
    out[0] = 0;
    for (i = first; i < argc; ++i) {
        const char *part = argv[i];
        size_t len = strlen(part);
        if (i != first && used + 1 < out_size) out[used++] = ' ';
        if (len > out_size - used - 1) len = out_size - used - 1;
        memcpy(out + used, part, len);
        used += len;
        out[used] = 0;
    }
}

int main(int argc, char **argv) {
    IplayRegs r;
    const char *op;
    mem = (db *)calloc(RUNNER_MEM_SIZE, sizeof(mem[0]));
    if (!mem) return 2;
    memset(&r, 0, sizeof(r));
    if (argc < 2) return 2;
    op = argv[1];

    if (streq(op, "nullsub5") || streq(op, "effnullsub") || streq(op, "nullsub2") || streq(op, "nullsub4") || streq(op, "nullsub3")) {
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        iplay_noop(&r);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "setvideomodenoop") || streq(op, "abisetvideomode")) {
        db mode;
        dw ax = 0x1234;
        if (argc != 3) return 2;
        mode = (db)parse_u32(argv[2]);
        mem[0x1680] = mode;
        iplay_set_current_text_video_mode(mode);
        if (mode != 0 && mode != 1) {
            ax = 0x0003u;
            if (mode == 2) ax |= 0x0080u;
        }
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)ax,
               0x5678u,
               0x9abcu,
               0xdef0u);
        print_bytes(mem + 0x1680, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "textmodegeom")) {
        const IplayTextMode *mode;
        if (argc != 3) return 2;
        mode = iplay_text_mode_for_video_mode((db)parse_u32(argv[2]));
        printf("cols=%u rows=%u rowbytes=%u cells=%u screenbytes=%u\n",
               (unsigned)mode->cols,
               (unsigned)mode->rows,
               (unsigned)iplay_text_mode_row_bytes(mode),
               (unsigned)iplay_text_mode_cells(mode),
               (unsigned)iplay_text_mode_screen_bytes(mode));
        return 0;
    }

    if (streq(op, "settextmodegeom")) {
        const IplayTextMode *mode;
        db video_mode;
        if (argc != 3) return 2;
        video_mode = (db)parse_u32(argv[2]);
        mem[0x1680] = video_mode;
        mode = iplay_set_current_text_video_mode(video_mode);
        printf("cols=%u rows=%u rowbytes=%u cells=%u screenbytes=%u data=%02x\n",
               (unsigned)mode->cols,
               (unsigned)mode->rows,
               (unsigned)iplay_text_mode_row_bytes(mode),
               (unsigned)iplay_text_mode_cells(mode),
               (unsigned)iplay_text_mode_screen_bytes(mode),
               (unsigned)mem[0x1680]);
        return 0;
    }

    if (streq(op, "textmodesupported")) {
        const IplayTextMode *mode;
        dw index;
        if (argc != 3) return 2;
        index = (dw)parse_u32(argv[2]);
        mode = iplay_text_supported_mode(index);
        if (!mode) {
            printf("present=0\n");
            return 0;
        }
        printf("present=1 cols=%u rows=%u rowbytes=%u cells=%u screenbytes=%u\n",
               (unsigned)mode->cols,
               (unsigned)mode->rows,
               (unsigned)iplay_text_mode_row_bytes(mode),
               (unsigned)iplay_text_mode_cells(mode),
               (unsigned)iplay_text_mode_screen_bytes(mode));
        return 0;
    }

    if (streq(op, "textscreenresize")) {
        IplayTextScreen screen;
        IplayNcPlane *root;
        const IplayTextMode *mode;
        if (argc != 2) return 2;
        memset(mem, 0, RUNNER_MEM_SIZE);
        iplay_text_screen_init_capacity(&screen, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 24, 39, 'A', 0x1e);
        iplay_text_screen_resize(&screen, &IPLAY_TEXT_MODE_80X50);
        mode = iplay_text_screen_mode(&screen);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 49, 79, 'B', 0x2f);
        printf("cols=%u rows=%u stride=%u oldtail=",
               (unsigned)mode->cols,
               (unsigned)mode->rows,
               (unsigned)root->stride_cols);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" newtail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" screenbytes=%u\n", (unsigned)iplay_text_mode_screen_bytes(mode));
        return 0;
    }

    if (streq(op, "textscreenvideomode")) {
        IplayTextScreen screen;
        IplayNcPlane *root;
        const IplayTextMode *mode;
        const IplayBottomLayout *layout;
        if (argc != 3) return 2;
        iplay_text_screen_init_capacity(&screen, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25);
        mode = iplay_text_screen_set_video_mode(&screen, (db)parse_u32(argv[2]));
        root = iplay_text_screen_root(&screen);
        layout = iplay_text_screen_bottom_layout(&screen);
        printf("cols=%u rows=%u stride=%u rowbytes=%u screenbytes=%u layout_left=%u layout_play=%u fits=%u\n",
               (unsigned)mode->cols,
               (unsigned)mode->rows,
               (unsigned)root->stride_cols,
               (unsigned)iplay_text_mode_row_bytes(mode),
               (unsigned)iplay_text_mode_screen_bytes(mode),
               (unsigned)layout->left_x,
               (unsigned)layout->playstate_x,
               (unsigned)iplay_text_screen_bottom_layout_fits(&screen));
        return 0;
    }

    if (streq(op, "textscreenui")) {
        IplayTextScreen screen;
        IplayAudioOutput output;
        AudioCapture capture;
        static const db pcm[8] = {
            0x00,0xc0, 0x00,0x20,
            0xff,0x7f, 0x00,0x80
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_text_screen_init(&screen, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(iplay_text_screen_root(&screen), 0x07);
        iplay_text_screen_draw_top_title(&screen);
        iplay_text_screen_draw_bottom(&screen, 1, 9, 2, 6, 125, 0x1f, 0x80, 123);
        iplay_audio_output_init_sb16_stereo(&output, capture_audio_write, &capture);
        iplay_audio_output_start(&output);
        iplay_audio_output_write_sb16_frames(&output, pcm, 2);
        iplay_text_screen_draw_audio_output_levels(&screen, 10, 4, &output, 16, 0xdb, 0xb0, 0x2a, 0x4c, 0x08);
        printf("fits=%u mode=%u,%u top=",
               (unsigned)iplay_text_screen_bottom_layout_fits(&screen),
               (unsigned)iplay_text_screen_mode(&screen)->cols,
               (unsigned)iplay_text_screen_mode(&screen)->rows);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 2), 2);
        printf(" module=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 5, 9), 20);
        printf(" meter_l=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" meter_r=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf("\n");
        return 0;
    }

    if (streq(op, "terminalwrap")) {
        IplayTerminal terminal;
        IplayAudioOutput output;
        AudioCapture capture;
        const IplayTextMode *mode;
        static const db pcm[8] = {
            0x00,0xc0, 0x00,0x20,
            0xff,0x7f, 0x00,0x80
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_terminal_init_vga_memory(&terminal, mem, &IPLAY_TEXT_MODE_40X25);
        mode = iplay_terminal_set_video_mode(&terminal, IPLAY_VIDEO_MODE_40X25_COLOR);
        iplay_terminal_erase(&terminal, 0x07);
        iplay_terminal_draw_top_title(&terminal);
        iplay_terminal_draw_bottom(&terminal, 1, 9, 2, 6, 125, 0x1f, 0x80, 123);
        iplay_audio_output_init_sb16_stereo(&output, capture_audio_write, &capture);
        iplay_audio_output_start(&output);
        iplay_audio_output_write_sb16_frames(&output, pcm, 2);
        iplay_terminal_draw_audio_output_levels(&terminal, 10, 4, &output, 16, 0xdb, 0xb0, 0x2a, 0x4c, 0x08);
        printf("backend=%u mode=%u,%u stride=%u top=",
               (unsigned)iplay_terminal_backend(&terminal),
               (unsigned)mode->cols,
               (unsigned)mode->rows,
               (unsigned)iplay_terminal_root(&terminal)->stride_cols);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 2), 2);
        printf(" module=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 5, 9), 20);
        printf(" meter_l=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" meter_r=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf("\n");
        return 0;
    }

    if (streq(op, "notcurseswrap")) {
        IplayNotcurses nc;
        IplayNcPlane *stdplane;
        const IplayTextMode *mode;
        if (argc != 2) return 2;
        iplay_notcurses_init_vga_memory(&nc, mem, &IPLAY_TEXT_MODE_40X25);
        mode = iplay_notcurses_set_video_mode(&nc, IPLAY_VIDEO_MODE_40X25_COLOR);
        iplay_notcurses_render_static(&nc, 0x07);
        iplay_notcurses_render_bottom(&nc, 1, 9, 2, 6, 125, 0x1f, 0x80, 123);
        stdplane = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putnstr_fill_yx(stdplane, 12, 3, "NC", 0x1e, 5);
        printf("backend=%u mode=%u,%u stride=%u top=",
               (unsigned)iplay_terminal_backend(iplay_notcurses_terminal(&nc)),
               (unsigned)mode->cols,
               (unsigned)mode->rows,
               (unsigned)stdplane->stride_cols);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 2), 2);
        printf(" module=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 5, 9), 20);
        printf(" std=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 12, 3), 10);
        printf("\n");
        return 0;
    }

    if (streq(op, "sdlaudiofacade")) {
        IplaySdlAudioDevice device;
        IplaySdlAudioSpec spec;
        AudioCapture capture;
        const IplayAudioLevels *levels;
        const IplayAudioFormat *format;
        static const db pcm[12] = {
            0xff,0x7f, 0x00,0x80,
            0x00,0x40, 0x00,0xc0,
            1,2,3,4
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 2);
        iplay_sdl_audio_device_write_sb16_frames(&device, pcm, 3);
        levels = iplay_sdl_audio_device_levels(&device);
        format = iplay_sdl_audio_device_format(&device);
        spec = iplay_sdl_audio_device_spec(&device);
        printf("backend=%u backend_name=%s hw=%u spec_backend=%u spec_hw=%u spec_rate=%u audio_status=%s format=%s rate=%u bits=%u channels=%u signed=%u framebytes=%u sb16=%u frames=%lu dropped=%lu capacity=%lu bytes=%u levels=%u,%u data=",
               (unsigned)iplay_sdl_audio_device_backend(&device),
               iplay_sdl_audio_device_backend_name(&device),
               (unsigned)iplay_sdl_audio_device_hardware_enabled(&device),
               (unsigned)iplay_sdl_audio_spec_backend(&spec),
               (unsigned)iplay_sdl_audio_spec_hardware_enabled(&spec),
               (unsigned)iplay_sdl_audio_spec_sample_rate(&spec),
               iplay_sdl_audio_device_status_text(&device),
               iplay_audio_format_name(format),
               (unsigned)format->sample_rate,
               (unsigned)format->bits_per_sample,
               (unsigned)format->channels,
               (unsigned)format->signed_samples,
               (unsigned)iplay_sdl_audio_device_bytes_per_frame(&device),
               (unsigned)iplay_sdl_audio_device_is_sb16_compatible(&device),
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device),
               (unsigned)capture.bytes,
               (unsigned)levels->left_16,
               (unsigned)levels->right_16);
        print_bytes(capture.data, capture.bytes);
        iplay_sdl_audio_device_add_capacity(&device, 1);
        iplay_sdl_audio_device_write_silence(&device, 1);
        iplay_sdl_audio_device_reset_levels(&device);
        levels = iplay_sdl_audio_device_levels(&device);
        printf(" silence_frames=%lu underrun=%lu reset=%u,%u silence_bytes=%u\n",
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_underrun_frames(&device),
               (unsigned)levels->left_16,
               (unsigned)levels->right_16,
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(op, "sb16hardwarefacade")) {
        IplaySdlAudioDevice device;
        IplaySdlAudioSpec spec;
        AudioCapture capture;
        const IplayAudioFormat *format;
        static const db pcm[4] = { 0x34,0x12, 0xcd,0xab };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_hardware(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 1);
        iplay_sdl_audio_device_write_sb16_frames(&device, pcm, 1);
        spec = iplay_sdl_audio_device_spec(&device);
        format = iplay_sdl_audio_device_format(&device);
        printf("backend=%u backend_name=%s hw=%u spec_backend=%u spec_hw=%u status=%s format=%s rate=%u frames=%lu bytes=%u data=",
               (unsigned)iplay_sdl_audio_device_backend(&device),
               iplay_sdl_audio_device_backend_name(&device),
               (unsigned)iplay_sdl_audio_device_hardware_enabled(&device),
               (unsigned)iplay_sdl_audio_spec_backend(&spec),
               (unsigned)iplay_sdl_audio_spec_hardware_enabled(&spec),
               iplay_sdl_audio_device_status_text(&device),
               iplay_audio_format_name(format),
               (unsigned)format->sample_rate,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "sdlaudiocallback")) {
        IplaySdlAudioDevice device;
        IplaySdlAudioCallback callback = iplay_sdl_audio_device_callback;
        AudioCapture capture;
        const IplayAudioLevels *levels;
        dw paused_bytes;
        dw bytes;
        static db stream[10] = {
            0x01,0x00, 0xff,0x7f,
            0x00,0x80, 0x34,0x12,
            0xaa,0xbb
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        paused_bytes = iplay_sdl_audio_device_callback(&device, stream, sizeof(stream));
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 4);
        bytes = iplay_sdl_audio_device_callback(&device, stream, sizeof(stream));
        iplay_sdl_audio_device_pause(&device, 1);
        paused_bytes = (dw)(paused_bytes + iplay_sdl_audio_device_callback(&device, stream, sizeof(stream)));
        iplay_sdl_audio_device_pause(&device, 0);
        levels = iplay_sdl_audio_device_levels(&device);
        printf("bytes=%u paused_bytes=%u paused=%u frames=%lu dropped=%lu levels=%u,%u callback=%u data=",
               (unsigned)bytes,
               (unsigned)paused_bytes,
               (unsigned)iplay_sdl_audio_device_paused(&device),
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned)levels->left_16,
               (unsigned)levels->right_16,
               callback != 0 ? 1u : 0u);
        print_bytes(capture.data, capture.bytes);
        iplay_sdl_audio_device_stop(&device);
        printf(" stopped=%u", (unsigned)iplay_sdl_audio_device_paused(&device));
        printf("\n");
        return 0;
    }

    if (streq(op, "sdlaudioopen")) {
        IplaySdlAudioDevice device;
        IplaySdlAudioDeviceConfig config;
        const IplaySdlAudioDeviceConfig *obtained;
        AudioCapture capture;
        dw opened;
        dw bytes;
        static db stream[8] = {
            0x10,0x00, 0x20,0x00,
            0x30,0x00, 0x40,0x00
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_config_sb16_stereo(&config, &device, IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);
        opened = (dw)iplay_sdl_audio_device_open(&device, &config, capture_audio_write, &capture);
        obtained = iplay_sdl_audio_device_config(&device);
        iplay_sdl_audio_device_start(&device);
        iplay_sdl_audio_device_set_capacity(&device, 2);
        bytes = obtained->callback(obtained->userdata, stream, sizeof(stream));
        printf("opened=%u freq=%u bits=%u channels=%u signed=%u samples=%u obtained_freq=%u obtained_samples=%u backend=%u hw=%u cb=%u userdata=%u bytes=%u frames=%lu data=",
               (unsigned)opened,
               (unsigned)config.frequency,
               (unsigned)config.bits_per_sample,
               (unsigned)config.channels,
               (unsigned)config.signed_samples,
               (unsigned)config.samples,
               (unsigned)obtained->frequency,
               (unsigned)obtained->samples,
               (unsigned)iplay_sdl_audio_device_backend(&device),
               (unsigned)iplay_sdl_audio_device_hardware_enabled(&device),
               obtained->callback == iplay_sdl_audio_device_callback ? 1u : 0u,
               obtained->userdata == &device ? 1u : 0u,
               (unsigned)bytes,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device));
        print_bytes(capture.data, capture.bytes);
        config.frequency = 22050u;
        printf(" bad_open=%u\n", (unsigned)iplay_sdl_audio_device_open(&device, &config, capture_audio_write, &capture));
        return 0;
    }

    if (streq(op, "sdlaudioqueue")) {
        IplaySdlAudioDevice device;
        AudioCapture capture;
        dw queued;
        dw paused_queue;
        static const db stream[10] = {
            0x01,0x00, 0x02,0x00,
            0x03,0x00, 0x04,0x00,
            0xaa,0xbb
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);
        iplay_sdl_audio_device_start(&device);
        queued = iplay_sdl_audio_device_queue(&device, stream, sizeof(stream));
        iplay_sdl_audio_device_pause(&device, 1);
        paused_queue = iplay_sdl_audio_device_queue(&device, stream, sizeof(stream));
        printf("queued=%u frames=%lu capacity=%lu dropped=%lu paused_queue=%u data=",
               (unsigned)queued,
               (unsigned long)iplay_sdl_audio_device_frames_written(&device),
               (unsigned long)iplay_sdl_audio_device_capacity(&device),
               (unsigned long)iplay_sdl_audio_device_dropped_frames(&device),
               (unsigned)paused_queue);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimefacade")) {
        IplayRuntime runtime;
        AudioCapture capture;
        const IplayAudioFormat *format;
        IplaySdlAudioSpec audio_spec;
        static const db pcm[8] = {
            0x00,0xc0, 0x00,0x20,
            0xff,0x7f, 0x00,0x80
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_init_vga_sb16(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &capture);
        iplay_runtime_set_video_mode(&runtime, IPLAY_VIDEO_MODE_40X25_COLOR);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_runtime_render_bottom(&runtime, 1, 9, 2, 6, 125, 0x1f, 0x80, 123);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 2);
        iplay_runtime_write_sb16_frames(&runtime, pcm, 2);
        iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);
        iplay_runtime_draw_status_line(&runtime, 14, "READY", 0x1f);
        iplay_runtime_draw_status_field(&runtime, 15, "File", "DEMO.MOD", 0x1e, 0x2f);
        iplay_runtime_draw_status_u32(&runtime, 16, "Size", 123456u, 0x1e, 0x2f);
        iplay_runtime_draw_status_hex32(&runtime, 17, "Tag", 0x1234abcdUL, 0x1e, 0x2f);
        format = iplay_runtime_audio_format(&runtime);
        audio_spec = iplay_runtime_audio_spec(&runtime);
        printf("mode=%u,%u backend=%u spec_backend=%u backend_name=%s hw=%u spec_hw=%u audio_status=%s format=%s spec_format=%s rate=%u spec_rate=%u framebytes=%u sb16=%u frames=%lu bytes=%u top=",
               (unsigned)iplay_runtime_video_cols(&runtime),
               (unsigned)iplay_runtime_video_rows(&runtime),
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_sdl_audio_spec_backend(&audio_spec),
               iplay_runtime_audio_backend_name(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)iplay_sdl_audio_spec_hardware_enabled(&audio_spec),
               iplay_runtime_audio_status_text(&runtime),
               iplay_audio_format_name(format),
               iplay_audio_format_name(iplay_sdl_audio_spec_format(&audio_spec)),
               (unsigned)format->sample_rate,
               (unsigned)iplay_sdl_audio_spec_sample_rate(&audio_spec),
               (unsigned)iplay_runtime_audio_bytes_per_frame(&runtime),
               (unsigned)iplay_runtime_audio_is_sb16_compatible(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 2), 2);
        printf(" module=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 5, 9), 20);
        printf(" meter_l=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 10, 4), 32);
        printf(" meter_r=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 11, 4), 32);
        printf(" data=");
        print_bytes(capture.data, capture.bytes);
        printf(" status=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 14, 0), 12);
        printf(" field=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 15, 0), 28);
        printf(" size=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 16, 0), 24);
        printf(" tag=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 17, 0), 24);
        printf(" present=%u", (unsigned)iplay_runtime_present(&runtime));
        iplay_runtime_shutdown(&runtime);
        printf(" stopped=%u reset=%u,%u",
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_audio_levels(&runtime)->left_16,
               (unsigned)iplay_runtime_audio_levels(&runtime)->right_16);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimeaudioqueue")) {
        IplayRuntime runtime;
        AudioCapture capture;
        dw queued;
        dw stopped_queue;
        static const db pcm[10] = {
            0x11,0x00, 0x22,0x00,
            0x33,0x00, 0x44,0x00,
            0xaa,0xbb
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_init_vga_sb16(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &capture);
        iplay_runtime_audio_start(&runtime);
        queued = iplay_runtime_audio_queue(&runtime, pcm, sizeof(pcm));
        iplay_runtime_audio_stop(&runtime);
        stopped_queue = iplay_runtime_audio_queue(&runtime, pcm, sizeof(pcm));
        printf("queued=%u frames=%lu capacity=%lu underrun=%lu dropped=%lu stopped_queue=%u data=",
               (unsigned)queued,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned)stopped_queue);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimeaudiopause")) {
        IplayRuntime runtime;
        AudioCapture capture;
        dw paused_queue;
        dw resumed_queue;
        static const db pcm[8] = {
            0x21,0x00, 0x22,0x00,
            0x23,0x00, 0x24,0x00
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_init_vga_sb16(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &capture);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_pause(&runtime, 1);
        paused_queue = iplay_runtime_audio_queue(&runtime, pcm, sizeof(pcm));
        iplay_runtime_audio_pause(&runtime, 0);
        resumed_queue = iplay_runtime_audio_queue(&runtime, pcm, sizeof(pcm));
        printf("paused_queue=%u resumed_queue=%u paused=%u frames=%lu capacity=%lu data=",
               (unsigned)paused_queue,
               (unsigned)resumed_queue,
               (unsigned)iplay_runtime_audio_paused(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimeaudioresetcounters")) {
        IplayRuntime runtime;
        AudioCapture capture;
        dw queued;
        static const db pcm[4] = {0x31,0x00,0x32,0x00};
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_init_vga_sb16(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &capture);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 3);
        iplay_runtime_write_silence(&runtime, 2);
        printf("before=%lu,%lu,%lu,%lu",
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        iplay_runtime_audio_reset_counters(&runtime);
        printf(" reset=%lu,%lu,%lu,%lu",
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        queued = iplay_runtime_audio_queue(&runtime, pcm, sizeof(pcm));
        printf(" queued=%u after=%lu,%lu,%lu,%lu data=",
               (unsigned)queued,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned long)iplay_runtime_audio_underrun_frames(&runtime),
               (unsigned long)iplay_runtime_audio_dropped_frames(&runtime),
               (unsigned long)iplay_runtime_audio_capacity(&runtime));
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimemodulestatus")) {
        IplayRuntime runtime;
        AudioCapture capture;
        IplayModuleStatus status;
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_init_vga_sb16(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &capture);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_module_status_init(&status, "ProTracker MOD", "SONG.MOD", 123456u, "mod_n_t_module", 0x1234abcdUL);
        iplay_runtime_draw_module_status_struct(&runtime, &status);
        iplay_runtime_draw_module_tag_struct(&runtime, &status);
        printf("title=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_TITLE_ROW, 0), 28);
        printf(" module=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_MODULE_ROW, 0), 32);
        printf(" size=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_SIZE_ROW, 0), 24);
        printf(" loader=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_LOADER_ROW, 0), 44);
        printf(" tag=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_TAG_ROW, 0), 26);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimestatusblock")) {
        IplayRuntime runtime;
        AudioCapture capture;
        IplayModuleStatus status;
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_init_vga_sb16(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &capture);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_module_status_init(&status, "ProTracker MOD", "SONG.MOD", 123456u, "mod_n_t_module", 0x1234abcdUL);
        iplay_runtime_draw_status_block(&runtime, &status);
        printf("title=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_TITLE_ROW, 0), 28);
        printf(" audio=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_AUDIO_ROW, 0), 66);
        printf(" video=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_VIDEO_ROW, 0), 18);
        printf(" tag=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_TAG_ROW, 0), 26);
        printf("\n");
        return 0;
    }

    if (streq(op, "modulestatusapi")) {
        IplayModuleStatus status;
        char tag[9];
        if (argc != 2) return 2;
        iplay_module_status_init(&status, "ProTracker_MOD", "SONG.MOD", 123456u, "mod_n_t_module", 0);
        printf("title=%s path=%s size=%lu loader=%s type0=%08lX",
               iplay_module_status_title(&status),
               iplay_module_status_path(&status),
               (unsigned long)iplay_module_status_size(&status),
               iplay_module_status_loader(&status),
               (unsigned long)iplay_module_status_type(&status));
        iplay_module_status_set_type(&status, 0x1234abcdUL);
        iplay_module_status_type_hex(&status, tag);
        printf(" type1=%08lX tag=%s", (unsigned long)iplay_module_status_type(&status), tag);
        iplay_module_status_clear_type(&status);
        iplay_module_status_type_hex(&status, tag);
        printf(" clear=%08lX clear_tag=%s\n", (unsigned long)iplay_module_status_type(&status), tag);
        return 0;
    }

    if (streq(op, "runtimeaudiostatus")) {
        IplayRuntime runtime;
        AudioCapture capture;
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_init_vga_sb16(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &capture);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_runtime_draw_audio_status(&runtime);
        printf("audio=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_AUDIO_ROW, 0), 66);
        printf(" hardware=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_HARDWARE_ROW, 0), 22);
        printf(" video=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_VIDEO_ROW, 0), 18);
        printf(" playback=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_PLAYBACK_ROW, 0), 78);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimevideostatus")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture capture;
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_no_hardware_capacity(&config, mem, IPLAY_TEXT_FALLBACK_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &capture);
        iplay_runtime_start_config_checked(&runtime, &config, IPLAY_VIDEO_MODE_80X50_PROJECT);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_runtime_draw_video_status(&runtime);
        printf("ok=%u token=%s text=%s video=",
               (unsigned)iplay_runtime_video_mode_ok(&runtime),
               iplay_runtime_video_status_token(&runtime),
               iplay_runtime_video_status_text(&runtime));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, IPLAY_RUNTIME_STATUS_VIDEO_ROW, 0), 18);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimepresentcb")) {
        IplayRuntime runtime;
        AudioCapture audio;
        VideoCapture video;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        memset(&video, 0, sizeof(video));
        iplay_runtime_init_vga_sb16_present(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_video_present, &video, capture_audio_write, &audio);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_runtime_present(&runtime);
        printf("cols=%u rows=%u bytes=%u first=",
               (unsigned)video.cols,
               (unsigned)video.rows,
               (unsigned)video.bytes);
        print_bytes(video.first, sizeof(video.first));
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimepresentresize")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture audio;
        VideoCapture video;
        const IplayTextMode *mode;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        memset(&video, 0, sizeof(video));
        iplay_runtime_config_sb16_hardware_capacity(&config, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_video_present, &video, capture_audio_write, &audio);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_resize(&runtime, &IPLAY_TEXT_MODE_80X50);
        mode = iplay_runtime_video_mode(&runtime);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_runtime_present(&runtime);
        printf("ok=%u mode=%u,%u bytes=%u callback=%u,%u,%u first=",
               (unsigned)iplay_runtime_video_mode_ok(&runtime),
               (unsigned)mode->cols,
               (unsigned)mode->rows,
               (unsigned)iplay_runtime_video_screen_bytes(&runtime),
               (unsigned)video.cols,
               (unsigned)video.rows,
               (unsigned)video.bytes);
        print_bytes(video.first, sizeof(video.first));
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimeaudiocb")) {
        IplayRuntime runtime;
        AudioCapture audio;
        static const db pcm[4] = {0x01,0x02,0x03,0x04};
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        iplay_runtime_init_vga_sdl_audio(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        iplay_runtime_write_sb16_frames(&runtime, pcm, 1);
        printf("backend=%u frames=%lu bytes=%u data=",
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned)audio.bytes);
        print_bytes(audio.data, audio.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimecallbacks")) {
        IplayRuntime runtime;
        AudioCapture audio;
        VideoCapture video;
        static const db pcm[4] = {0x05,0x06,0x07,0x08};
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        memset(&video, 0, sizeof(video));
        iplay_runtime_init_callbacks(&runtime, mem, &IPLAY_TEXT_MODE_40X25, capture_video_present, &video, capture_audio_write, &audio);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        iplay_runtime_write_sb16_frames(&runtime, pcm, 1);
        iplay_runtime_present(&runtime);
        printf("video=%u,%u,%u audio=%lu,%u data=",
               (unsigned)video.cols,
               (unsigned)video.rows,
               (unsigned)video.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned)audio.bytes);
        print_bytes(audio.data, audio.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimeconfig")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture audio;
        VideoCapture video;
        static const db pcm[4] = {0x09,0x0a,0x0b,0x0c};
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        memset(&video, 0, sizeof(video));
        iplay_runtime_config_sdl_capacity(&config, mem, IPLAY_TEXT_FALLBACK_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_video_present, &video, capture_audio_write, &audio);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        iplay_runtime_write_sb16_frames(&runtime, pcm, 1);
        iplay_runtime_present(&runtime);
        printf("video=%u,%u,%u audio=%lu,%u data=",
               (unsigned)video.cols,
               (unsigned)video.rows,
               (unsigned)video.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned)audio.bytes);
        print_bytes(audio.data, audio.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimehwresizecallbacks")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture audio;
        VideoCapture video;
        static const db pcm[4] = {0x21,0x43,0x65,0x87};
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        memset(&video, 0, sizeof(video));
        iplay_runtime_config_sb16_hardware_capacity(&config, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_video_present, &video, capture_audio_write, &audio);
        iplay_runtime_init_config(&runtime, &config);
        iplay_runtime_resize(&runtime, &IPLAY_TEXT_MODE_80X50);
        iplay_runtime_render_static(&runtime, 0x07);
        iplay_runtime_audio_start(&runtime);
        iplay_runtime_audio_set_capacity(&runtime, 1);
        iplay_runtime_write_sb16_frames(&runtime, pcm, 1);
        iplay_runtime_present(&runtime);
        printf("ok=%u backend=%u hw=%u video=%u,%u,%u audio=%lu,%u framebytes=%u data=",
               (unsigned)iplay_runtime_video_mode_ok(&runtime),
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_hardware_enabled(&runtime),
               (unsigned)video.cols,
               (unsigned)video.rows,
               (unsigned)video.bytes,
               (unsigned long)iplay_runtime_audio_frames_written(&runtime),
               (unsigned)audio.bytes,
               (unsigned)iplay_runtime_audio_bytes_per_frame(&runtime));
        print_bytes(audio.data, audio.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "runtimevideospec")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayVideoSpec spec;
        AudioCapture audio;
        VideoCapture video;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        memset(&video, 0, sizeof(video));
        iplay_runtime_config_no_hardware(&config, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        iplay_runtime_init_config(&runtime, &config);
        spec = iplay_runtime_video_spec(&runtime);
        printf("nohw_backend=%u nohw_present=%u nohw_mode=%u,%u cfg_backend=%u cfg_present=%u",
               (unsigned)spec.backend,
               (unsigned)spec.present_enabled,
               (unsigned)spec.mode.cols,
               (unsigned)spec.mode.rows,
               (unsigned)config.video_backend,
               (unsigned)config.video_present_enabled);
        iplay_runtime_config_sdl(&config, mem, &IPLAY_TEXT_MODE_80X25, capture_video_present, &video, capture_audio_write, &audio);
        iplay_runtime_init_config(&runtime, &config);
        spec = iplay_runtime_video_spec(&runtime);
        printf(" cb_backend=%u cb_present=%u cb_mode=%u,%u cb_cfg_backend=%u cb_cfg_present=%u\n",
               (unsigned)spec.backend,
               (unsigned)spec.present_enabled,
               (unsigned)spec.mode.cols,
               (unsigned)spec.mode.rows,
               (unsigned)config.video_backend,
               (unsigned)config.video_present_enabled);
        return 0;
    }

    if (streq(op, "runtimestartconfig")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture audio;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        iplay_runtime_config_no_hardware_capacity(&config, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        iplay_runtime_start_config(&runtime, &config, IPLAY_VIDEO_MODE_80X50_PROJECT);
        printf("mode=%u,%u active=%u mode_ok=%u capacity=%u\n",
               (unsigned)iplay_runtime_video_cols(&runtime),
               (unsigned)iplay_runtime_video_rows(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_video_mode_ok(&runtime),
               (unsigned)iplay_runtime_video_capacity(&runtime));
        return 0;
    }

    if (streq(op, "runtimestartconfigchecked")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture audio;
        int ok;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        iplay_runtime_config_no_hardware_capacity(&config, mem, IPLAY_TEXT_FALLBACK_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        ok = iplay_runtime_start_config_checked(&runtime, &config, IPLAY_VIDEO_MODE_80X50_PROJECT);
        printf("ok=%u mode=%u,%u active=%u mode_ok=%u capacity=%u\n",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_cols(&runtime),
               (unsigned)iplay_runtime_video_rows(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_video_mode_ok(&runtime),
               (unsigned)iplay_runtime_video_capacity(&runtime));
        return 0;
    }

    if (streq(op, "runtimesdlconfig")) {
        IplayRuntimeConfig config;
        AudioCapture audio;
        VideoCapture video;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        memset(&video, 0, sizeof(video));
        iplay_runtime_config_sdl(&config, mem, &IPLAY_TEXT_MODE_40X25, capture_video_present, &video, capture_audio_write, &audio);
        printf("cells=%u capacity=%u mode=%u,%u present=%u puser=%u video_backend=%u video_present=%u audio=%u auser=%u has_video=%u has_audio=%u has_capacity=%u valid=%u\n",
               config.cells == mem ? 1u : 0u,
               (unsigned)config.cell_capacity_bytes,
               (unsigned)config.mode->cols,
               (unsigned)config.mode->rows,
               config.present == capture_video_present ? 1u : 0u,
               config.present_user == &video ? 1u : 0u,
               (unsigned)config.video_backend,
               (unsigned)config.video_present_enabled,
               config.audio_write == capture_audio_write ? 1u : 0u,
               config.audio_user == &audio ? 1u : 0u,
               (unsigned)iplay_runtime_config_has_video_present(&config),
               (unsigned)iplay_runtime_config_has_audio_sink(&config),
               (unsigned)iplay_runtime_config_has_cell_capacity(&config),
               (unsigned)iplay_runtime_config_is_valid(&config));
        return 0;
    }

    if (streq(op, "runtimenohwconfig")) {
        IplayRuntimeConfig config;
        AudioCapture audio;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        iplay_runtime_config_no_hardware(&config, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        printf("cells=%u capacity=%u mode=%u,%u present=%u video_backend=%u video_present=%u audio=%u has_video=%u has_audio=%u has_capacity=%u valid=%u\n",
               config.cells == mem ? 1u : 0u,
               (unsigned)config.cell_capacity_bytes,
               (unsigned)config.mode->cols,
               (unsigned)config.mode->rows,
               config.present == 0 ? 0u : 1u,
               (unsigned)config.video_backend,
               (unsigned)config.video_present_enabled,
               config.audio_write == capture_audio_write ? 1u : 0u,
               (unsigned)iplay_runtime_config_has_video_present(&config),
               (unsigned)iplay_runtime_config_has_audio_sink(&config),
               (unsigned)iplay_runtime_config_has_cell_capacity(&config),
               (unsigned)iplay_runtime_config_is_valid(&config));
        return 0;
    }

    if (streq(op, "runtimeinvalidconfig")) {
        IplayRuntimeConfig config;
        AudioCapture audio;
        db err;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        iplay_runtime_config_no_hardware(&config, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        err = iplay_runtime_config_error(&config);
        printf("valid=%u err=%u name=%s", (unsigned)iplay_runtime_config_is_valid(&config), (unsigned)err, iplay_runtime_config_error_name(err));
        config.cells = 0;
        err = iplay_runtime_config_error(&config);
        printf(" nocells=%u err_cells=%u name_cells=%s", (unsigned)iplay_runtime_config_is_valid(&config), (unsigned)err, iplay_runtime_config_error_name(err));
        iplay_runtime_config_no_hardware(&config, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        config.mode = 0;
        err = iplay_runtime_config_error(&config);
        printf(" nomode=%u err_mode=%u name_mode=%s", (unsigned)iplay_runtime_config_is_valid(&config), (unsigned)err, iplay_runtime_config_error_name(err));
        iplay_runtime_config_no_hardware(&config, mem, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        config.audio_write = 0;
        err = iplay_runtime_config_error(&config);
        printf(" noaudio=%u err_audio=%u name_audio=%s", (unsigned)iplay_runtime_config_is_valid(&config), (unsigned)err, iplay_runtime_config_error_name(err));
        iplay_runtime_config_no_hardware_capacity(&config, mem, (dw)(IPLAY_TEXT_FALLBACK_SCREEN_BYTES - 2u), &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        err = iplay_runtime_config_error(&config);
        printf(" small=%u err_small=%u name_small=%s unknown=%s\n", (unsigned)iplay_runtime_config_is_valid(&config), (unsigned)err, iplay_runtime_config_error_name(err), iplay_runtime_config_error_name(0xffu));
        return 0;
    }

    if (streq(op, "runtimecapacityresize")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture audio;
        int ok;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        iplay_runtime_config_no_hardware_capacity(&config, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_set_video_mode_checked(&runtime, IPLAY_VIDEO_MODE_80X50_PROJECT);
        printf("ok=%u wide=%u,%u bytes=%u capacity=%u",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_cols(&runtime),
               (unsigned)iplay_runtime_video_rows(&runtime),
               (unsigned)iplay_runtime_present(&runtime),
               (unsigned)iplay_runtime_video_capacity(&runtime));
        iplay_runtime_config_no_hardware_capacity(&config, mem, IPLAY_TEXT_FALLBACK_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_audio_write, &audio);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_set_video_mode_checked(&runtime, IPLAY_VIDEO_MODE_80X50_PROJECT);
        printf(" small_ok=%u small=%u,%u bytes_small=%u capacity_small=%u\n",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_cols(&runtime),
               (unsigned)iplay_runtime_video_rows(&runtime),
               (unsigned)iplay_runtime_present(&runtime),
               (unsigned)iplay_runtime_video_capacity(&runtime));
        return 0;
    }

    if (streq(op, "runtimeresizefacade")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        AudioCapture audio;
        VideoCapture video;
        IplayNcPlane *root;
        int ok_wide;
        int ok_small;
        dw mode_cols;
        dw mode_rows;
        dw root_cols;
        dw root_rows;
        dw root_stride;
        dw fits;
        dw present_cols;
        dw present_rows;
        dw present_bytes;
        if (argc != 2) return 2;
        memset(&audio, 0, sizeof(audio));
        memset(&video, 0, sizeof(video));
        iplay_runtime_config_sdl_capacity(&config, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_video_present, &video, capture_audio_write, &audio);
        iplay_runtime_init_config(&runtime, &config);
        ok_wide = iplay_runtime_resize_checked(&runtime, &IPLAY_TEXT_MODE_80X50);
        iplay_runtime_resize(&runtime, &IPLAY_TEXT_MODE_80X25);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 24, 79, 'R', 0x3e);
        iplay_runtime_present(&runtime);
        mode_cols = iplay_runtime_video_cols(&runtime);
        mode_rows = iplay_runtime_video_rows(&runtime);
        root_cols = iplay_runtime_video_cols(&runtime);
        root_rows = iplay_runtime_video_rows(&runtime);
        root_stride = iplay_runtime_video_cols(&runtime);
        fits = (dw)iplay_runtime_bottom_layout_fits(&runtime);
        present_cols = video.cols;
        present_rows = video.rows;
        present_bytes = video.bytes;
        iplay_runtime_config_sdl_capacity(&config, mem, IPLAY_TEXT_FALLBACK_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_video_present, &video, capture_audio_write, &audio);
        iplay_runtime_init_config(&runtime, &config);
        ok_small = iplay_runtime_resize_checked(&runtime, &IPLAY_TEXT_MODE_80X50);
        printf("ok_wide=%u mode=%u,%u root=%u,%u stride=%u fits=%u present=%u,%u,%u tail=",
               (unsigned)ok_wide,
               (unsigned)mode_cols,
               (unsigned)mode_rows,
               (unsigned)root_cols,
               (unsigned)root_rows,
               (unsigned)root_stride,
               (unsigned)fits,
               (unsigned)present_cols,
               (unsigned)present_rows,
               (unsigned)present_bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 24, 79), 2);
        printf(" small_ok=%u small_mode=%u,%u video_ok=%u\n",
               (unsigned)ok_small,
               (unsigned)iplay_runtime_video_cols(&runtime),
               (unsigned)iplay_runtime_video_rows(&runtime),
               (unsigned)iplay_runtime_video_mode_ok(&runtime));
        return 0;
    }

    if (streq(op, "runtimeinvalidinit")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        if (argc != 2) return 2;
        memset(&config, 0, sizeof(config));
        iplay_runtime_init_config(&runtime, &config);
        printf("mode=%u,%u backend=%u active=%u present=%u valid=%u\n",
               (unsigned)iplay_runtime_video_cols(&runtime),
               (unsigned)iplay_runtime_video_rows(&runtime),
               (unsigned)iplay_runtime_audio_backend(&runtime),
               (unsigned)iplay_runtime_audio_active(&runtime),
               (unsigned)iplay_runtime_present(&runtime),
               (unsigned)iplay_runtime_config_is_valid(&config));
        return 0;
    }

    if (streq(op, "textmodegeometry")) {
        const IplayTextMode *mode;
        const IplayTextMode *fallback;
        const IplayTextMode *by_size;
        dw i;
        if (argc != 2) return 2;
        mode = iplay_text_default_mode();
        fallback = iplay_text_fallback_mode();
        printf("count=%u default=%u,%u fallback=%u,%u max=%u",
               (unsigned)iplay_text_supported_mode_count(),
               (unsigned)iplay_text_mode_cols(mode),
               (unsigned)iplay_text_mode_rows(mode),
               (unsigned)iplay_text_mode_cols(fallback),
               (unsigned)iplay_text_mode_rows(fallback),
               (unsigned)iplay_text_max_screen_bytes());
        for (i = 0; i < iplay_text_supported_mode_count(); ++i) {
            mode = iplay_text_supported_mode(i);
            printf(" mode%u=%u,%u,%u,%u,%u",
                   (unsigned)i,
                   (unsigned)iplay_text_mode_cols(mode),
                   (unsigned)iplay_text_mode_rows(mode),
                   (unsigned)iplay_text_mode_row_bytes(mode),
                   (unsigned)iplay_text_mode_cells(mode),
                   (unsigned)iplay_text_mode_screen_bytes(mode));
        }
        by_size = iplay_text_mode_for_size(IPLAY_TEXT_COLS_80, IPLAY_TEXT_ROWS_50);
        printf(" bysize=%u,%u missing=%u\n",
               (unsigned)(by_size ? iplay_text_mode_cols(by_size) : 0),
               (unsigned)(by_size ? iplay_text_mode_rows(by_size) : 0),
               (unsigned)(iplay_text_mode_for_size(132u, 43u) == 0));
        return 0;
    }

    if (streq(op, "bottomlayout")) {
        const IplayBottomLayout *layout;
        db video_mode;
        if (argc != 3) return 2;
        video_mode = (db)parse_u32(argv[2]);
        mem[0x1680] = video_mode;
        iplay_set_current_text_video_mode(video_mode);
        layout = iplay_bottom_layout();
        printf("module_y=%u pattern_y=%u timing_y=%u left_x=%u mode_x=%u value_x=%u flag_x=%u play_y=%u play_x=%u module_w=%u pattern_w=%u timing_w=%u mode_w=%u value_w=%u play_w=%u\n",
               (unsigned)layout->module_y,
               (unsigned)layout->pattern_y,
               (unsigned)layout->timing_y,
               (unsigned)layout->left_x,
               (unsigned)layout->mode_x,
               (unsigned)layout->value_x,
               (unsigned)layout->flag_x,
               (unsigned)layout->playstate_y,
               (unsigned)layout->playstate_x,
               (unsigned)layout->module_width,
               (unsigned)layout->pattern_width,
               (unsigned)layout->timing_width,
               (unsigned)layout->mode_width,
               (unsigned)layout->value_width,
               (unsigned)layout->playstate_width);
        return 0;
    }

    if (streq(op, "bottomlayoutfits")) {
        const IplayTextMode *mode;
        const IplayBottomLayout *layout;
        db video_mode;
        if (argc != 3) return 2;
        video_mode = (db)parse_u32(argv[2]);
        mode = iplay_text_mode_for_video_mode(video_mode);
        mem[0x1680] = video_mode;
        iplay_set_current_text_video_mode(video_mode);
        layout = iplay_bottom_layout();
        printf("fits=%u cols=%u rows=%u\n",
               (unsigned)iplay_bottom_layout_fits(layout, mode),
               (unsigned)mode->cols,
               (unsigned)mode->rows);
        return 0;
    }

    if (streq(op, "txtdrawbottomplane")) {
        IplayNcPlane root;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_txt_draw_bottom_plane(&root, &IPLAY_BOTTOM_LAYOUT_40COL, 1, 9, 2, 6, 125, 0x1f, 0x80, 123);
        printf("module=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 5, 9), 20);
        printf(" pattern=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 6, 9), 14);
        printf(" timing=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 7, 9), 26);
        printf(" mode=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 7, 1), 12);
        printf(" values=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 6, 29), 10);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 7, 29), 10);
        printf(" flags=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 19), 2);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 19), 2);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 19), 2);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 5, 19), 2);
        printf(" play=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 24), 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "txtdrawtoptitleplane")) {
        IplayNcPlane root;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_80X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_txt_draw_top_title_plane(&root);
        printf("corner=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 1, 2), 2);
        printf(" title=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 1, 41), 28);
        printf(" copy=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 2, 10), 18);
        printf("\n");
        return 0;
    }

    if (streq(op, "ncplanesubresize")) {
        IplayNcPlane root;
        IplayNcPlane child;
        IplayNcPlane grandchild;
        dw origin_y = 0;
        dw origin_x = 0;
        dw nested_y = 0;
        dw nested_x = 0;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_subplane(&child, &root, 1, 2, 3, 4);
        iplay_ncplane_subplane(&grandchild, &child, 1, 1, 1, 2);
        iplay_ncplane_origin_yx(&child, &origin_y, &origin_x);
        iplay_ncplane_origin_yx(&grandchild, &nested_y, &nested_x);
        iplay_ncplane_putc_yx(&child, 0, 0, 'A', 0x1e);
        iplay_ncplane_putc_yx(&child, 2, 3, 'B', 0x2f);
        iplay_ncplane_putc_yx(&child, 3, 0, 'X', 0x7c);
        iplay_ncplane_resize(&child, 1, 1);
        iplay_ncplane_putc_yx(&child, 0, 0, 'C', 0x3a);
        iplay_ncplane_putc_yx(&child, 0, 1, 'Y', 0x4b);
        printf("rows=%u cols=%u origin=%u,%u nested=%u,%u stride=%u data=",
               (unsigned)iplay_ncplane_rows(&child),
               (unsigned)iplay_ncplane_cols(&child),
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)nested_y,
               (unsigned)nested_x,
               (unsigned)child.stride_cols);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 2), 2);
        printf(" tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 5), 2);
        printf(" clipped=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 2), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "ncplanefillrect")) {
        IplayNcPlane root;
        IplayNcPlane child;
        dw visible_rows;
        dw visible_cols;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_ncplane_subplane(&child, &root, 2, 3, 2, 5);
        iplay_ncplane_visible_region(&child, 0, 1, 3, 3, &visible_rows, &visible_cols);
        iplay_ncplane_fill_yx(&child, 0, 1, 3, 3, '#', 0x2e);
        printf("visible=%u,%u ", (unsigned)visible_rows, (unsigned)visible_cols);
        printf("origin=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 3), 10);
        printf(" row1=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 3), 10);
        printf(" clipped=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 4), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "windowwrapper")) {
        IplayNcPlane root_plane;
        IplayWindow root;
        IplayWindow child;
        IplayWindow nested;
        dw child_y = 0;
        dw child_x = 0;
        dw nested_y = 0;
        dw nested_x = 0;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root_plane, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root_plane, 0x07);
        iplay_window_init_root(&root, &root_plane);
        iplay_window_init_subwindow(&child, &root, 2, 4, 4, 6);
        iplay_window_init_subwindow(&nested, &child, 1, 2, 2, 3);
        iplay_window_origin_yx(&child, &child_y, &child_x);
        iplay_window_origin_yx(&nested, &nested_y, &nested_x);
        iplay_ncplane_putnstr_fill_yx(iplay_window_plane(&child), 0, 0, "WIN", 0x1e, 6);
        iplay_ncplane_putc_yx(iplay_window_plane(&nested), 1, 2, 'Z', 0x4f);
        iplay_window_resize(&child, 2, 3);
        printf("child=%u,%u,%u,%u nested=%u,%u rows=%u cols=%u data=",
               (unsigned)child_y,
               (unsigned)child_x,
               (unsigned)iplay_window_rows(&child),
               (unsigned)iplay_window_cols(&child),
               (unsigned)nested_y,
               (unsigned)nested_x,
               (unsigned)iplay_window_rows(&nested),
               (unsigned)iplay_window_cols(&nested));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 4), 12);
        printf(" nested_cell=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 8), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "windowstatus")) {
        IplayNcPlane root_plane;
        IplayWindow root;
        IplayWindow status;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root_plane, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root_plane, 0x07);
        iplay_window_init_root(&root, &root_plane);
        iplay_window_init_subwindow(&status, &root, 5, 3, 4, 20);
        iplay_window_draw_status_line(&status, 0, "READY", 0x1f);
        iplay_window_draw_status_field(&status, 1, "File", "SONG.MOD", 0x1e, 0x2f);
        iplay_window_draw_status_u32(&status, 2, "Size", 1234u, 0x1e, 0x2f);
        iplay_window_draw_status_hex32(&status, 3, "Tag", 0x89abcdefUL, 0x1e, 0x2f);
        printf("line=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 5, 3), 12);
        printf(" field=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 6, 3), 28);
        printf(" size=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 7, 3), 20);
        printf(" tag=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 8, 3), 24);
        printf(" outside=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 5, 2), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "windowdraw")) {
        IplayNcPlane root_plane;
        IplayWindow root;
        IplayWindow child;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root_plane, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root_plane, 0x07);
        iplay_window_init_root(&root, &root_plane);
        iplay_window_init_subwindow(&child, &root, 2, 5, 4, 8);
        iplay_window_erase(&child, 0x18);
        iplay_window_box_yx(&child, 0, 0, 4, 8, 0x1e, 0x2f);
        iplay_window_fill_yx(&child, 1, 1, 1, 6, '.', 0x3a);
        iplay_window_putnstr_fill_yx(&child, 2, 1, "ABC", 0x4b, 6);
        printf("top=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 5), 16);
        printf(" fill=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 6), 12);
        printf(" text=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 6), 12);
        printf(" outside=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 4), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "windowcursor")) {
        IplayNcPlane root_plane;
        IplayWindow root;
        IplayWindow child;
        dw y = 0;
        dw x = 0;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root_plane, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root_plane, 0x07);
        iplay_window_init_root(&root, &root_plane);
        iplay_window_init_subwindow(&child, &root, 3, 4, 2, 8);
        iplay_window_cursor_move_yx(&child, 0, 1);
        iplay_window_putc(&child, 'A', 0x1e);
        iplay_window_putstr(&child, "BC", 0x2f);
        iplay_window_cursor_move_yx(&child, 1, 0);
        iplay_window_putnstr(&child, "WXYZ", 0x3a, 3);
        iplay_window_cursor_move_yx(&child, 99, 99);
        iplay_window_putc(&child, 'Z', 0x4c);
        iplay_window_cursor_yx(&child, &y, &x);
        printf("row0=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 4), 16);
        printf(" row1=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 4), 16);
        printf(" cursor=%u,%u outside=", (unsigned)y, (unsigned)x);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 3), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "windowscroll")) {
        IplayNcPlane root_plane;
        IplayWindow root;
        IplayWindow child;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root_plane, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root_plane, 0x07);
        iplay_window_init_root(&root, &root_plane);
        iplay_window_init_subwindow(&child, &root, 3, 4, 4, 6);
        iplay_window_putnstr_fill_yx(&child, 0, 0, "AAAAAA", 0x1e, 6);
        iplay_window_putnstr_fill_yx(&child, 1, 0, "BBBBBB", 0x2f, 6);
        iplay_window_putnstr_fill_yx(&child, 2, 0, "CCCCCC", 0x3a, 6);
        iplay_window_putnstr_fill_yx(&child, 3, 0, "DDDDDD", 0x4b, 6);
        iplay_window_scroll_up(&child, 0, 0, 4, 6, 1, 0x5c);
        printf("up0=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 4), 12);
        printf(" up2=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 5, 4), 12);
        printf(" up3=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 6, 4), 12);
        iplay_window_scroll_down(&child, 0, 0, 4, 6, 1, 0x6d);
        printf(" down0=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 4), 12);
        printf(" down1=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 4), 12);
        printf(" outside=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 3), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "windowaudiolevels")) {
        IplayNcPlane root_plane;
        IplayWindow root;
        IplayWindow child;
        IplayAudioLevels levels;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root_plane, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root_plane, 0x07);
        iplay_window_init_root(&root, &root_plane);
        iplay_window_init_subwindow(&child, &root, 6, 5, 2, 8);
        levels.left_peak = 0;
        levels.right_peak = 0;
        levels.left_16 = 8;
        levels.right_16 = 4;
        iplay_window_draw_audio_levels(&child, 0, 0, &levels, 8, '#', '.', 0x1e, 0x2f, 0x08);
        printf("left=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 6, 5), 16);
        printf(" right=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 7, 5), 16);
        printf(" outside=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 6, 4), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "ncplanebox")) {
        IplayNcPlane root;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_ncplane_box_yx(&root, 1, 2, 3, 4, 0x1e, 0x2f);
        iplay_ncplane_vline_yx(&root, 3, 39, '!', 0x4c, 4);
        printf("top=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 2), 8);
        printf(" mid=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 2), 8);
        printf(" bot=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 2), 8);
        printf(" vline=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 39), 2);
        printf(" vclip=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 39), 2);
        printf(" vend=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "drawframeplane")) {
        IplayNcPlane root;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_draw_frame_plane(&root, 3, 0x1e, 0x2f, 2, 1, 5, 3);
        iplay_draw_frame_plane(&root, 1, 0x7c, 0x7c, 8, 1, 11, 3);
        printf("top=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 2), 8);
        printf(" mid=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 2), 8);
        printf(" bot=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 2), 8);
        printf(" ignored=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 8), 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "ncplaneputnstr")) {
        IplayNcPlane root;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_ncplane_putnstr_yx(&root, 0, 0, "abcdef", 0x1e, 3);
        iplay_ncplane_putnstr_fill_yx(&root, 1, 0, "xy", 0x2f, 4);
        iplay_ncplane_putnstr_fill_yx(&root, 2, 38, "wxyz", 0x3a, 4);
        printf("data=");
        print_bytes(mem, 12);
        printf(" fill=");
        print_bytes(mem + IPLAY_TEXT_ROW_BYTES(IPLAY_TEXT_COLS_40), 8);
        printf(" clipped=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 38), 4);
        printf(" after=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 0), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "ncplanecursor")) {
        IplayNcPlane root;
        dw y;
        dw x;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_ncplane_cursor_move_yx(&root, 1, 2);
        iplay_ncplane_putstr(&root, "AB", 0x1e);
        iplay_ncplane_putnstr_fill(&root, "C", 0x2f, 3);
        iplay_ncplane_cursor_move_yx(&root, 99, 99);
        iplay_ncplane_putnstr(&root, "XY", 0x3a, 2);
        iplay_ncplane_cursor_yx(&root, &y, &x);
        printf("cursor=%u,%u data=",
               (unsigned)y,
               (unsigned)x);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 2), 10);
        printf(" edge=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" after=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 38), 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "ncplanescroll")) {
        IplayNcPlane root;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_ncplane_putnstr_fill_yx(&root, 0, 0, "AAAA", 0x1e, 4);
        iplay_ncplane_putnstr_fill_yx(&root, 1, 0, "BBBB", 0x2f, 4);
        iplay_ncplane_putnstr_fill_yx(&root, 2, 0, "CCCC", 0x3a, 4);
        iplay_ncplane_scroll_up(&root, 0, 0, 3, 4, 1, 0x4c);
        printf("row0=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 0, 0), 8);
        printf(" row1=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 0), 8);
        printf(" row2=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 0), 8);
        printf(" outside=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 0), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "ncplanescrolldown")) {
        IplayNcPlane root;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_ncplane_putnstr_fill_yx(&root, 0, 0, "AAAA", 0x1e, 4);
        iplay_ncplane_putnstr_fill_yx(&root, 1, 0, "BBBB", 0x2f, 4);
        iplay_ncplane_putnstr_fill_yx(&root, 2, 0, "CCCC", 0x3a, 4);
        iplay_ncplane_scroll_down(&root, 0, 0, 3, 4, 1, 0x4c);
        printf("row0=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 0, 0), 8);
        printf(" row1=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 0), 8);
        printf(" row2=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 0), 8);
        printf(" outside=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 0), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "textattr16")) {
        db attr;
        if (argc != 5) return 2;
        attr = iplay_text_attr(
            (IplayTextColor)parse_u32(argv[2]),
            (IplayTextColor)parse_u32(argv[3]),
            (int)parse_u32(argv[4]));
        printf("attr=%02x fg=%u bg=%u blink=%u\n",
               (unsigned)attr,
               (unsigned)iplay_text_attr_fg(attr),
               (unsigned)iplay_text_attr_bg(attr),
               (unsigned)iplay_text_attr_blink(attr));
        return 0;
    }

    if (streq(op, "textsetup")) {
        if (argc != 3) return 2;
        mem[0x164c] = 0xaa; mem[0x164d] = 0xaa;
        mem[0x164e] = 0xbb; mem[0x164f] = 0xbb;
        mem[0x1650] = 0xcc; mem[0x1651] = 0xcc;
        mem[0x1652] = 0xdd; mem[0x1653] = 0xdd;
        mem[0x1654] = 3; mem[0x1655] = 0;
        mem[0x167e] = 0xee; mem[0x167f] = 0xee;
        mem[0x1680] = 0;
        mem[0x1696] = 1;
        mem[0x1502] = 1;
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        if (!iplay_text_setup_small(&r, mem, argv[2])) return 2;
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + 0x164c, 8);
        print_bytes(mem + 0x167e, 2);
        print_bytes(mem + 0x1680, 1);
        print_bytes(mem + 0x1696, 1);
        print_bytes(mem + 0x162c, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "graphsetup")) {
        if (argc != 3) return 2;
        mem[0x164c] = 0xaa; mem[0x164d] = 0xaa;
        mem[0x164e] = 0xbb; mem[0x164f] = 0xbb;
        mem[0x1650] = 0xcc; mem[0x1651] = 0xcc;
        mem[0x1652] = 0xdd; mem[0x1653] = 0xdd;
        mem[0x1680] = streq(argv[2], "f2_waves") ? 0xee : 4;
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        if (!iplay_graph_setup_bounded(&r, mem, argv[2])) return 2;
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + 0x164c, 8);
        print_bytes(mem + 0x1680, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub1ab8c")) {
        db *channel = mem + DSEG_SCRATCH;
        if (argc != 4) return 2;
        channel[0x35] = (db)parse_u32(argv[2]);
        r.ecx = parse_u32(argv[3]);
        r.ebx = DSEG_SCRATCH;
        r.esi = 0x2222;
        iplay_sub_1ab8c(&r, channel);
        printf("ax=%04x si=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.esi & 0xffffu));
        return 0;
    }

    if (streq(op, "spectr1bce9equal")) {
        if (argc != 3) return 2;
        mem[DSEG_SCRATCH] = (db)parse_u32(argv[2]);
        mem[DSEG_SCRATCH + 0x64] = (db)parse_u32(argv[2]);
        r.ebx = DSEG_SCRATCH;
        r.ebp = DSEG_SCRATCH + 0x1000;
        iplay_spectr_1bce9_equal(&r, mem);
        printf("bx=%04x bp=%04x data=",
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ebp & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH + 0x1000, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "spectr1bc2dequal")) {
        r.ebx = DSEG_SCRATCH;
        r.ebp = DSEG_SCRATCH + 0x1000;
        iplay_spectr_1bc2d_equal(&r, mem);
        printf("bx=%04x bp=%04x data=",
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ebp & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH + 0x1000, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "spectr1bbc1zero")) {
        r.ecx = 1;
        r.esi = DSEG_SCRATCH;
        r.edi = DSEG_SCRATCH + 0x100;
        iplay_spectr_1bbc1_zero(&r, mem);
        printf("cx=%04x si=%04x di=%04x data=",
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        printf("%02x%02x%02x\n",
               (unsigned)mem[DSEG_SCRATCH + 0x100],
               (unsigned)mem[DSEG_SCRATCH + 0x100 + 0xc8],
               (unsigned)mem[DSEG_SCRATCH + 0x100 + 0x12c]);
        return 0;
    }

    if (streq(op, "videoprp")) {
        db *channels = mem + DSEG_SCRATCH;
        dw count;
        dw i;
        if (argc < 3) return 2;
        count = (dw)(argc - 2);
        mem[0x1654] = (db)count;
        mem[0x1655] = (db)(count >> 8);
        for (i = 0; i < count; ++i) {
            channels[(dw)(i * 0x50u + 0x3au)] = (db)strtoul(argv[2 + i], 0, 16);
        }
        iplay_video_prp_mtr_positn(mem, channels, count);
        printf("data=");
        print_bytes(mem + 0x1689, 2);
        print_bytes(mem + 0x1691, 1);
        print_bytes(mem + 0x16ac, (size_t)count * 2u);
        printf("\n");
        return 0;
    }

    if (streq(op, "videoprpmode")) {
        db *channels = mem + DSEG_SCRATCH;
        IplayRegs r;
        dw count;
        dw i;
        db video_mode;
        if (argc < 4) return 2;
        memset(&r, 0, sizeof(r));
        video_mode = (db)parse_u32(argv[2]);
        mem[0x1680] = video_mode;
        iplay_set_current_text_video_mode(video_mode);
        count = (dw)(argc - 3);
        mem[0x1654] = (db)count;
        mem[0x1655] = (db)(count >> 8);
        for (i = 0; i < count; ++i) {
            channels[(dw)(i * 0x50u + 0x3au)] = (db)strtoul(argv[3 + i], 0, 16);
        }
        iplay_video_prp_mtr_positn(mem, channels, count);
        printf("mode=%u cols=%u rows=%u data=",
               (unsigned)mem[0x1680],
               (unsigned)iplay_text_current_mode()->cols,
               (unsigned)iplay_text_current_mode()->rows);
        print_bytes(mem + 0x1689, 2);
        print_bytes(mem + 0x1691, 1);
        print_bytes(mem + 0x16ac, (size_t)count * 2u);
        printf("\n");
        return 0;
    }

    if (streq(op, "hex4") || streq(op, "hex8") || streq(op, "hex16") || streq(op, "hex32")) {
        size_t count = streq(op, "hex4") ? 1 : streq(op, "hex8") ? 2 : streq(op, "hex16") ? 4 : 8;
        dw out_off = ORIG_DST_OFF;
        dw off = out_off;
        unsigned long value;
        unsigned ax;
        db al;
        if (argc != 3) return 2;
        value = parse_u32(argv[2]);
        if (count == 1) al = iplay_hex4_to_buffer(mem, &off, (db)value);
        else if (count == 2) al = iplay_hex8_to_buffer(mem, &off, (db)value);
        else if (count == 4) al = iplay_hex16_to_buffer(mem, &off, (dw)value);
        else al = iplay_hex32_to_buffer(mem, &off, value);
        ax = (count < 4) ? (unsigned)((value & 0xff00u) | al) : (unsigned)(((value & 0xffu) << 8) | al);
        printf("ax=%04x si=%04x data=", ax, (unsigned)off);
        print_bytes(mem + out_off, count);
        printf("\n");
        return 0;
    }

    if (streq(op, "seg1hex")) {
        const char *sym;
        size_t count;
        dw out_off = DSEG_SCRATCH;
        dw off = out_off;
        unsigned long value;
        unsigned ax;
        db al;
        if (argc != 4) return 2;
        sym = argv[2];
        count = streq(sym, "my_u32tox") ? 8 : streq(sym, "my_u16tox") ? 4 : streq(sym, "my_u8tox") ? 2 : 1;
        value = parse_u32(argv[3]);
        if (count == 1) al = iplay_hex4_to_buffer(mem, &off, (db)value);
        else if (count == 2) al = iplay_hex8_to_buffer(mem, &off, (db)value);
        else if (count == 4) al = iplay_hex16_to_buffer(mem, &off, (dw)value);
        else al = iplay_hex32_to_buffer(mem, &off, value);
        ax = (count < 4) ? (unsigned)((value & 0xff00u) | al) : (unsigned)(((value & 0xffu) << 8) | al);
        printf("ax=%04x si=%04x data=", ax, (unsigned)off);
        print_bytes(mem + out_off, count);
        printf("\n");
        return 0;
    }

    if (streq(op, "abimyhex")) {
        const char *sym;
        size_t count;
        dw out_off = DSEG_SCRATCH;
        dw off = out_off;
        unsigned long value;
        unsigned ax;
        db al;
        if (argc != 4) return 2;
        sym = argv[2];
        count = streq(sym, "my_u32tox") ? 8 : streq(sym, "my_u16tox") ? 4 : streq(sym, "my_u8tox") ? 2 : 1;
        value = parse_u32(argv[3]);
        if (count == 1) al = iplay_hex4_to_buffer(mem, &off, (db)value);
        else if (count == 2) al = iplay_hex8_to_buffer(mem, &off, (db)value);
        else if (count == 4) al = iplay_hex16_to_buffer(mem, &off, (dw)value);
        else al = iplay_hex32_to_buffer(mem, &off, value);
        ax = (count < 4) ? (unsigned)((value & 0xff00u) | al) : (unsigned)(((value & 0xffu) << 8) | al);
        printf("ax=%04x si=%04x data=", ax, (unsigned)off);
        print_bytes(mem + out_off, count);
        printf("\n");
        return 0;
    }

    if (streq(op, "hex1be39")) {
        db value;
        db attr;
        if (argc != 4) return 2;
        value = (db)parse_u32(argv[2]);
        attr = (db)parse_u32(argv[3]);
        mem[DSEG_SCRATCH] = (db)((value & 0x0f) + '0');
        if (mem[DSEG_SCRATCH] > '9') mem[DSEG_SCRATCH] = (db)(mem[DSEG_SCRATCH] + 7);
        mem[DSEG_SCRATCH + 1] = attr;
        r.eax = (dw)mem[DSEG_SCRATCH] | ((dw)attr << 8);
        r.edi = DSEG_SCRATCH + 2;
        printf("ax=%04x di=%04x data=", (unsigned)(r.eax & 0xffffu), (unsigned)(r.edi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "putdigit")) {
        dw off = BUF_OFF;
        dw count;
        if (argc != 4) return 2;
        count = iplay_put_counted_char_to_buffer(mem, &off, (dw)parse_u32(argv[3]), (db)parse_u32(argv[2]));
        printf("cx=%04x si=%04x data=", (unsigned)count, (unsigned)off);
        print_bytes(mem + BUF_OFF, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "seg1putdigit")) {
        dw off = DSEG_SCRATCH;
        dw count;
        if (argc != 4) return 2;
        count = iplay_put_counted_char_to_buffer(mem, &off, (dw)parse_u32(argv[3]), (db)parse_u32(argv[2]));
        printf("cx=%04x si=%04x data=", (unsigned)count, (unsigned)off);
        print_bytes(mem + DSEG_SCRATCH, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "decimal")) {
        const char *sym;
        unsigned long value;
        dw off = BUF_OFF;
        IplayDecimalResult result;
        if (argc != 4) return 2;
        sym = argv[2];
        value = parse_u32(argv[3]);
        if (streq(sym, "my_u8toa_10")) result = iplay_u8_decimal_to_buffer(mem, &off, (db)value);
        else if (streq(sym, "my_u16toa_10")) result = iplay_u16_decimal_to_buffer(mem, &off, (dw)value);
        else if (streq(sym, "my_u32toa10_0")) result = iplay_u32_decimal_to_buffer(mem, &off, value);
        else if (streq(sym, "my_i8toa10_0")) result = iplay_i8_decimal_to_buffer(mem, &off, (db)value);
        else if (streq(sym, "my_i16toa10_0")) result = iplay_i16_decimal_to_buffer(mem, &off, (dw)value);
        else if (streq(sym, "my_i32toa10_0")) result = iplay_i32_decimal_to_buffer(mem, &off, value);
        else return 2;
        printf("cx=%04x si=%04x data=", (unsigned)result.count, (unsigned)result.offset);
        print_bytes(mem + BUF_OFF, result.count);
        printf("\n");
        return 0;
    }

    if (streq(op, "seg1decimal")) {
        const char *sym;
        unsigned long value;
        dw off = DSEG_SCRATCH;
        IplayDecimalResult result;
        if (argc != 4) return 2;
        sym = argv[2];
        value = parse_u32(argv[3]);
        if (streq(sym, "my_u8toa10")) result = iplay_u8_decimal_to_buffer(mem, &off, (db)value);
        else if (streq(sym, "my_u16toa10")) result = iplay_u16_decimal_to_buffer(mem, &off, (dw)value);
        else if (streq(sym, "my_u32toa10")) result = iplay_u32_decimal_to_buffer(mem, &off, value);
        else if (streq(sym, "my_i8toa10")) result = iplay_i8_decimal_to_buffer(mem, &off, (db)value);
        else return 2;
        printf("cx=%04x si=%04x data=", (unsigned)result.count, (unsigned)result.offset);
        print_bytes(mem + DSEG_SCRATCH, result.count);
        printf("\n");
        return 0;
    }

    if (streq(op, "seg1u32toa") || streq(op, "u32toa0direct")) {
        unsigned base;
        dw off;
        dw out_off;
        IplayDecimalResult result;
        if (argc != 4) return 2;
        base = (unsigned)parse_u32(argv[3]);
        off = streq(op, "seg1u32toa") ? DSEG_SCRATCH : ORIG_DST_OFF;
        out_off = off;
        result = iplay_u32_base_to_buffer(mem, &off, parse_u32(argv[2]), base, 0);
        printf("cx=%04x si=%04x data=", (unsigned)result.count, (unsigned)result.offset);
        print_bytes(mem + out_off, result.count);
        printf("\n");
        return 0;
    }

    if (streq(op, "seg1fill")) {
        const char *sym;
        unsigned count;
        unsigned prefix;
        dw off = DSEG_SCRATCH;
        if (argc != 5) return 2;
        sym = argv[2];
        count = (unsigned)parse_u32(argv[4]);
        prefix = streq(sym, "my_pnt_u32toa_fill");
        if (!prefix && !streq(sym, "my_u32toa_fill")) return 2;
        iplay_u32_decimal_fill_to_buffer(mem, &off, parse_u32(argv[3]), (dw)count, prefix);
        printf("di=%04x data=", (unsigned)off);
        print_bytes(mem + DSEG_SCRATCH, count + (prefix ? 2u : 0u));
        printf("\n");
        return 0;
    }

    if (streq(op, "myasmsprintf")) {
        static const db fmt[] = {
            'U', '=', 4, 'u', 0x50, 0x28,
            ' ', 'I', '=', 8, 'i', 0x52, 0x28,
            ' ', 'X', '=', 11, 'x', 0x54, 0x28,
            0, 0
        };
        memcpy(mem + DSEG_SCRATCH, fmt, sizeof(fmt));
        mem[DSEG_SCRATCH + 0x50] = 200;
        mem[DSEG_SCRATCH + 0x52] = 0x2e;
        mem[DSEG_SCRATCH + 0x53] = 0xfb;
        mem[DSEG_SCRATCH + 0x54] = 0xcd;
        mem[DSEG_SCRATCH + 0x55] = 0xab;
        {
            IplayAsmSprintfResult result = iplay_myasmsprintf_to_buffer(mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40, 0, 0, 0);
        printf("si=%04x di=%04x data=",
               (unsigned)result.src_offset,
               (unsigned)result.dst_offset);
        }
        print_bytes(mem + DSEG_SCRATCH + 0x40, strlen("U=200 I=-1234 X=ABCD"));
        printf("\n");
        return 0;
    }

    if (streq(op, "parsecmdline")) {
        char command[128];
        size_t n;
        if (argc < 3) command[0] = 0;
        else join_args(command, sizeof(command), argc, argv, 2);
        if (command[0] == '/' && strlen(command) + 1 < sizeof(command)) {
            memmove(command + 1, command, strlen(command) + 1);
            command[0] = ' ';
        }
        n = strlen(command);
        mem[0x0080] = (db)n;
        memcpy(mem + 0x0081, command, n);
        mem[0x0081 + n] = 0x0d;
        memset(mem + 0x137c, 0x2e, 32);
        iplay_parse_cmdline(&r, mem);
        printf("bp=%04x si=%04x di=%04x data=",
               (unsigned)(r.ebp & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        print_bytes(mem + 0x137c, n ? strlen((const char *)(mem + 0x137c)) + 1u : 1u);
        printf("\n");
        return 0;
    }

    if (streq(op, "getcomspec")) {
        static const db env[] = "COMSPEC=X\0";
        memcpy(mem, env, sizeof(env));
        iplay_get_comspec(&r, mem);
        printf("di=%04x\n", (unsigned)(r.edi & 0xffffu));
        return 0;
    }

    if (streq(op, "getexename")) {
        size_t path_len;
        if (argc != 3) return 2;
        mem[0] = 'A'; mem[1] = '='; mem[2] = 'B'; mem[3] = 0; mem[4] = 0;
        mem[5] = 1; mem[6] = 0;
        path_len = strlen(argv[2]);
        memcpy(mem + 7, argv[2], path_len + 1);
        r.esi = DSEG_SCRATCH;
        iplay_getexename(&r, mem, mem);
        printf("si=%04x data=",
               (unsigned)(r.esi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, path_len + 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "int2fcheck")) {
        if (argc != 3) return 2;
        r.eax = 0x60ff;
        r.ebx = 0x5344;
        r.ecx = 0x4d50;
        r.edx = parse_u32(argv[2]);
        mem[0x168c] = 0;
        iplay_int2f_checkmyself(&r, mem);
        printf("ax=%04x data=%02x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)mem[0x168c]);
        return 0;
    }

    if (streq(op, "spectr1b406small")) {
        size_t n;
        if (argc != 3) return 2;
        n = parse_hex_bytes(argv[2], mem + DSEG_SCRATCH, 8);
        while (n < 8) mem[DSEG_SCRATCH + n++] = 0;
        mem[0x7d30] = 1;
        mem[0x7d31] = 0;
        iplay_spectr_1b406_small(mem, DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 8);
        print_bytes(mem + 0x7d1c, 0x18);
        printf("\n");
        return 0;
    }

    if (streq(op, "spectrsqrt")) {
        if (argc != 3) return 2;
        r.ebx = parse_u32(argv[2]);
        iplay_spectr_1c4f8(&r);
        printf("ax=%04x bx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu));
        return 0;
    }

    if (streq(op, "interppatch")) {
        static const dw sampled[] = {
            0x58b4, 0x58e3, 0x5912, 0x5941, 0x5970, 0x599f, 0x59ce, 0x59fd,
            0x5a2c, 0x5a5b, 0x5a8a, 0x5ab9, 0x5ae8, 0x5b17, 0x5b46, 0x5b81,
            0x5bad, 0x5bda, 0x5c07, 0x5c34, 0x5c61, 0x5c8e, 0x5cbb, 0x5ce8,
            0x5d15, 0x5d42, 0x5d6f, 0x5d9c, 0x5dc9, 0x5df6, 0x5e23
        };
        unsigned i;
        if (argc != 3) return 2;
        iplay_interpolation_patch(mem, (db)parse_u32(argv[2]));
        printf("data=");
        for (i = 0; i < sizeof(sampled) / sizeof(sampled[0]); ++i) {
            print_bytes(mem + sampled[i], 1);
        }
        printf("\n");
        return 0;
    }

    if (streq(op, "strlen")) {
        char text[80];
        dw len;
        if (argc < 2) return 2;
        join_args(text, sizeof(text), argc, argv, 2);
        strncpy((char *)(mem + BUF_OFF), text, 64);
        len = iplay_string_length_at(mem, BUF_OFF);
        printf("ax=%04x si=%04x\n", (unsigned)len, (unsigned)BUF_OFF);
        return 0;
    }

    if (streq(op, "seg1strlen")) {
        char text[80];
        dw len;
        if (argc < 2) return 2;
        join_args(text, sizeof(text), argc, argv, 2);
        strncpy((char *)(mem + DSEG_SCRATCH), text, 64);
        len = iplay_string_length_at(mem, DSEG_SCRATCH);
        printf("ax=%04x si=%04x\n", (unsigned)len, (unsigned)DSEG_SCRATCH);
        return 0;
    }

    if (streq(op, "strcpy")) {
        size_t n;
        char text[80];
        IplayStringCopyResult result;
        if (argc < 3) return 2;
        join_args(text, sizeof(text), argc, argv, 2);
        strncpy((char *)(mem + BUF_OFF), text, 64);
        memset(mem + DST_OFF, '.', 64);
        result = iplay_strcpy_count_to_buffer(mem, mem, BUF_OFF, DST_OFF);
        n = strlen(text) + 1;
        printf("cx=%04x si=%04x di=%04x data=", (unsigned)result.count, (unsigned)result.src_offset, (unsigned)result.dst_offset);
        print_bytes(mem + DST_OFF, n);
        printf("\n");
        return 0;
    }

    if (streq(op, "copyprint")) {
        unsigned count;
        if (argc != 4) return 2;
        count = (unsigned)parse_u32(argv[3]);
        memcpy(mem + BUF_OFF, argv[2], strlen(argv[2]));
        memset(mem + DST_OFF, '.', 64);
        iplay_copy_printable_to_buffer(mem, mem, BUF_OFF, DST_OFF, (dw)count);
        printf("cx=%04x si=%04x di=%04x data=", (unsigned)count, (unsigned)BUF_OFF, (unsigned)DST_OFF);
        print_bytes(mem + DST_OFF, count);
        printf("\n");
        return 0;
    }

    if (streq(op, "seg1copyprint")) {
        unsigned count;
        if (argc != 4) return 2;
        count = (unsigned)parse_u32(argv[3]);
        memcpy(mem + DSEG_SCRATCH, argv[2], strlen(argv[2]));
        memset(mem + DSEG_SCRATCH + 0x40, 0, 64);
        iplay_copy_printable_padded_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40, (dw)count);
        printf("si=%04x di=%04x data=",
               (unsigned)DSEG_SCRATCH,
               (unsigned)(DSEG_SCRATCH + 0x40));
        print_bytes(mem + DSEG_SCRATCH + 0x40, count);
        printf("\n");
        return 0;
    }

    if (streq(op, "seg1strcpycount")) {
        size_t n;
        char text[80];
        IplayStringCopyResult result;
        if (argc < 3) text[0] = 0;
        else join_args(text, sizeof(text), argc, argv, 2);
        memcpy(mem + DSEG_SCRATCH, text, strlen(text) + 1);
        memset(mem + DSEG_SCRATCH + 0x40, 0, 64);
        result = iplay_strcpy_count_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40);
        n = strlen(text);
        printf("cx=%04x si=%04x di=%04x data=",
               (unsigned)result.count,
               (unsigned)result.src_offset,
               (unsigned)result.dst_offset);
        print_bytes(mem + DSEG_SCRATCH + 0x40, n);
        printf("\n");
        return 0;
    }

    if (streq(op, "txt1abae")) {
        char text[80];
        IplayAttributedTextResult result;
        if (argc < 3) return 2;
        join_args(text, sizeof(text), argc, argv, 2);
        memcpy(mem + DSEG_SCRATCH, text, strlen(text));
        memset(mem + DSEG_SCRATCH + 0x40, 0, 0x40);
        result = iplay_copy_attributed_fixed_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40, 0x16u, 0x7b);
        printf("si=%04x di=%04x data=",
               (unsigned)result.src_offset,
               (unsigned)result.dst_offset);
        print_bytes(mem + DSEG_SCRATCH + 0x40, 0x16u * 2u);
        printf("\n");
        return 0;
    }

    if (streq(op, "putmessage") || streq(op, "text1bf69") || streq(op, "putmessage2")) {
        char text[80];
        db attr;
        size_t len;
        IplayAttributedTextResult result;
        if (argc < 4) return 2;
        strncpy(text, argv[2], sizeof(text) - 1);
        text[sizeof(text) - 1] = 0;
        attr = (db)parse_u32(argv[argc - 1]);
        len = strlen(text);
        memset(mem + DSEG_SCRATCH + 0x40, 0, 0x80);
        if (streq(op, "putmessage2")) {
            if (len == 0) return 2;
            memcpy(mem + DSEG_SCRATCH, text + 1, len);
            result = iplay_put_attributed_message_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40, attr, 1, (db)text[0]);
        } else if (streq(op, "text1bf69")) {
            memcpy(mem + DSEG_SCRATCH, text, len + 1);
            result = iplay_put_controlled_attributed_text_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40, attr);
        } else {
            memcpy(mem + DSEG_SCRATCH, text, len + 1);
            result = iplay_put_attributed_message_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40, attr, 0, 0);
        }
        printf("si=%04x di=%04x data=",
               (unsigned)result.src_offset,
               (unsigned)result.dst_offset);
        print_bytes(mem + DSEG_SCRATCH + 0x40, len * 2u);
        printf("\n");
        return 0;
    }

    if (streq(op, "message1be77")) {
        char text[80];
        db y;
        db attr;
        size_t len;
        if (argc < 5) return 2;
        strncpy(text, argv[2], sizeof(text) - 1);
        text[sizeof(text) - 1] = 0;
        y = (db)parse_u32(argv[3]);
        attr = (db)parse_u32(argv[4]);
        len = strlen(text);
        memset(mem + DSEG_SCRATCH, 0, 1000);
        memcpy(mem + DSEG_SCRATCH + 0x500, text, len + 1);
        {
            IplayAttributedTextResult result = iplay_message_1be77_to_buffer(mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x500, y, attr);
        printf("si=%04x di=%04x data=",
               (unsigned)result.src_offset,
               (unsigned)result.dst_offset);
        }
        print_bytes(mem + DSEG_SCRATCH, 1000);
        printf("\n");
        return 0;
    }

    if (streq(op, "drawframe")) {
        db style;
        db attr;
        db fill_attr;
        db x;
        db y;
        db right;
        db bottom;
        if (argc != 9) return 2;
        style = (db)parse_u32(argv[2]);
        attr = (db)parse_u32(argv[3]);
        fill_attr = (db)parse_u32(argv[4]);
        x = (db)parse_u32(argv[5]);
        y = (db)parse_u32(argv[6]);
        right = (db)parse_u32(argv[7]);
        bottom = (db)parse_u32(argv[8]);
        memset(mem + DSEG_SCRATCH, 0, 0x500);
        iplay_draw_frame(mem + DSEG_SCRATCH, style, attr, fill_attr, x, y, right, bottom);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 400);
        printf("\n");
        return 0;
    }

    if (streq(op, "txtdrawtoptitle")) {
        memset(mem + DSEG_SCRATCH, 0, 0x500);
        iplay_txt_draw_top_title(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 0x500);
        printf("\n");
        return 0;
    }

    if (streq(op, "writescr")) {
        char text[128];
        db attr;
        dw delta;
        size_t len;
        if (argc < 5) return 2;
        strncpy(text, argv[2], sizeof(text) - 1);
        text[sizeof(text) - 1] = 0;
        attr = (db)parse_u32(argv[3]);
        delta = (dw)parse_u32(argv[4]);
        len = strlen(text);
        mem[DSEG_SCRATCH] = (db)delta;
        mem[DSEG_SCRATCH + 1] = (db)(delta >> 8);
        mem[DSEG_SCRATCH + 2] = attr;
        memcpy(mem + DSEG_SCRATCH + 3, text, len + 1);
        {
            IplayScreenStreamResult result = iplay_write_screen_stream_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40);
        printf("si=%04x di=%04x data=",
               (unsigned)result.src_offset,
               (unsigned)result.dst_offset);
        }
        print_bytes(mem + DSEG_SCRATCH + 0x40 + delta, len * 2u);
        printf("\n");
        return 0;
    }

    if (streq(op, "txtdrawbottom")) {
        db byte_1de72;
        db byte_1de73;
        db byte_1de74;
        db byte_1de75;
        db byte_1de76;
        db flags;
        dw volume;
        dw amplif;
        if (argc != 10) return 2;
        byte_1de72 = (db)parse_u32(argv[2]);
        byte_1de73 = (db)parse_u32(argv[3]);
        byte_1de74 = (db)parse_u32(argv[4]);
        byte_1de75 = (db)parse_u32(argv[5]);
        byte_1de76 = (db)parse_u32(argv[6]);
        flags = (db)parse_u32(argv[7]);
        volume = (dw)parse_u32(argv[8]);
        amplif = (dw)parse_u32(argv[9]);
        memset(mem + DSEG_SCRATCH, 0xcc, 0x600);
        iplay_txt_draw_bottom(mem + DSEG_SCRATCH, byte_1de72, byte_1de73, byte_1de74, byte_1de75, byte_1de76, flags, volume, amplif);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 0x600);
        printf("\n");
        return 0;
    }

    if (streq(op, "filelist")) {
        db entry_type;
        db flags;
        dw time_word;
        dw date_word;
        dd size;
        if (argc < 8) return 2;
        entry_type = (db)parse_u32(argv[2]);
        flags = (db)parse_u32(argv[3]);
        time_word = (dw)parse_u32(argv[4]);
        date_word = (dw)parse_u32(argv[5]);
        size = (dd)parse_u32(argv[6]);
        memset(mem + DSEG_SCRATCH + 0x1000 + 0x654, 0xcc, 160);
        iplay_filelist_row(mem + DSEG_SCRATCH + 0x1000 + 0x654, entry_type, flags, time_word, date_word, size, argv[7]);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH + 0x1000 + 0x654, 160);
        printf("\n");
        return 0;
    }

    if (streq(op, "findmodsguard")) {
        unsigned i;
        for (i = 0; i < 120; ++i) mem[0x137c + i] = 'X';
        mem[0x168e] = 0xaa;
        mem[0x1640] = 0xbb;
        mem[0x1641] = 0xbb;
        mem[0x1642] = 0xcc;
        mem[0x1643] = 0xcc;
        iplay_find_mods_no_nul_guard(&r, mem, 0x0d8f);
        printf("ax=0d00 di=2b05 data=");
        print_bytes(mem + 0x168e, 1);
        print_bytes(mem + 0x1640, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "recolortxt")) {
        unsigned i;
        dw base;
        dw row;
        db color;
        IplayRecolorResult result;
        if (argc != 4) return 2;
        row = (dw)parse_u32(argv[2]);
        color = (db)parse_u32(argv[3]);
        base = (dw)((row * iplay_text_mode_row_bytes(&IPLAY_TEXT_MODE_80X25)) + IPLAY_TEXT_OFFSET(IPLAY_TEXT_MODE_80X25.cols, 10u, 8u) + 1u);
        for (i = 0; i < 64; ++i) mem[base + i * 2u] = (db)(0xa0u | (i & 0x0fu));
        result = iplay_recolor_text_row(mem, &IPLAY_TEXT_MODE_80X25, row, color);
        printf("ax=%04x bx=%04x data=",
               (unsigned)result.ax,
               (unsigned)color);
        for (i = 0; i < 64; ++i) printf("%02x", mem[base + i * 2u]);
        printf("\n");
        return 0;
    }

    if (streq(op, "mouse_init") || streq(op, "mouse_deinit") || streq(op, "mouse_show") || streq(op, "mouse_hide") ||
        streq(op, "mouse_getpos") || streq(op, "mouse_showcur") || streq(op, "mouse_hide2")) {
        db visible;
        if (argc != 4) return 2;
        visible = (db)parse_u32(argv[3]);
        mem[DSEG_SCRATCH + 0] = 0xaa;
        mem[DSEG_SCRATCH + 1] = 0xaa;
        mem[DSEG_SCRATCH + 2] = 0xbb;
        mem[DSEG_SCRATCH + 3] = 0xbb;
        mem[DSEG_SCRATCH + 4] = 0xcc;
        mem[DSEG_SCRATCH + 5] = 0x00;
        if (streq(op, "mouse_init") || streq(op, "mouse_show") || streq(op, "mouse_showcur")) visible = 0;
        mem[DSEG_SCRATCH + 6] = visible;
        r.ebx = streq(op, "mouse_getpos") ? 0 : 0x1111;
        r.ecx = streq(op, "mouse_getpos") ? 0 : 0x2222;
        r.edx = streq(op, "mouse_getpos") ? 0 : 0x3333;
        printf("bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 7);
        printf("\n");
        return 0;
    }

    if (streq(op, "mouse_1c7a9")) {
        if (argc != 8) return 2;
        r.eax = parse_u32(argv[2]);
        r.ebp = parse_u32(argv[3]);
        r.ecx = parse_u32(argv[4]);
        r.edx = parse_u32(argv[5]);
        r.esi = parse_u32(argv[6]);
        r.edi = parse_u32(argv[7]);
        iplay_mouse_1c7a9(&r);
        printf("ax=%04x bp=%04x cx=%04x dx=%04x si=%04x di=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebp & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        return 0;
    }

    if (streq(op, "mouse_1c7cf")) {
        if (argc != 5) return 2;
        r.eax = parse_u32(argv[2]);
        r.ebp = parse_u32(argv[3]);
        r.ebx = DSEG_SCRATCH;
        parse_hex_bytes(argv[4], mem + DSEG_SCRATCH, 0x400);
        iplay_mouse_1c7cf(&r, mem);
        printf("ax=%04x bx=%04x bp=%04x cx=%04x dx=%04x si=%04x di=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ebp & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        return 0;
    }

    if (streq(op, "int24")) {
        if (argc != 3) return 2;
        r.eax = (parse_u32(argv[2]) & 0xffu) << 8;
        iplay_int24(&r);
        printf("ax=%04x\n", (unsigned)(r.eax & 0xffffu));
        return 0;
    }

    if (streq(op, "emsrestore")) {
        if (argc != 4) return 2;
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        iplay_ems_restore_mapctx_guard(&r, (db)parse_u32(argv[2]), (db)parse_u32(argv[3]));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "emsinit")) {
        if (argc != 3) return 2;
        mem[0x0104] = 0xff;
        iplay_ems_init_config(&r, mem, (dw)parse_u32(argv[2]));
        printf("ax=%04x ems=%02x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)mem[0x0104]);
        return 0;
    }

    if (streq(op, "emsguard")) {
        if (argc != 3) return 2;
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        iplay_ems_disabled_guard(&r, 0);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "emsmapcopy")) {
        db payload[16];
        unsigned i;
        dw dest;
        if (argc != 3) return 2;
        mem[DSEG_SCRATCH + 0x20] = (db)DSEG_SCRATCH;
        mem[DSEG_SCRATCH + 0x21] = (db)(DSEG_SCRATCH >> 8);
        mem[DSEG_SCRATCH + 0x2c] = (db)DSEG_SCRATCH;
        mem[DSEG_SCRATCH + 0x2d] = (db)(DSEG_SCRATCH >> 8);
        mem[DSEG_SCRATCH + 0x32] = 0xff;
        mem[DSEG_SCRATCH + 0x33] = 0xff;
        mem[DSEG_SCRATCH + 0x3c] = 0;
        for (i = 0; i < sizeof(payload); ++i) payload[i] = (db)(0x31u + i);
        if (streq(argv[2], "ems_mapmemx")) {
            memcpy(mem + DSEG_SCRATCH + 1, payload, sizeof(payload));
            dest = DSEG_SCRATCH + 0x800;
        } else {
            memcpy(mem + DSEG_SCRATCH + 0x800, payload, sizeof(payload));
            dest = DSEG_SCRATCH + 1;
        }
        r.eax = 0x0d8f;
        r.edi = DSEG_SCRATCH;
        iplay_ems_local_mapcopy(mem, argv[2], DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + dest, sizeof(payload));
        printf("\n");
        return 0;
    }

    if (streq(op, "emsrealloc2limit")) {
        dd requested;
        if (argc != 4) return 2;
        mem[0x0077] = (db)parse_u32(argv[2]);
        requested = (dd)parse_u32(argv[3]);
        mem[DSEG_SCRATCH + 0x20] = (db)requested;
        mem[DSEG_SCRATCH + 0x21] = (db)(requested >> 8);
        mem[DSEG_SCRATCH + 0x22] = (db)(requested >> 16);
        mem[DSEG_SCRATCH + 0x23] = (db)(requested >> 24);
        r.edi = DSEG_SCRATCH;
        iplay_ems_realloc2_fallback(&r, mem, DSEG_SCRATCH);
        printf("ax=%04x cx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu));
        print_bytes(mem + 0x0077, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "clean11c43")) {
        db flag;
        db byte_2461e;
        db byte_2461f;
        if (argc != 5) return 2;
        flag = (db)parse_u32(argv[2]);
        byte_2461e = (db)parse_u32(argv[3]);
        byte_2461f = (db)parse_u32(argv[4]);
        memset(mem, 0xff, RUNNER_MEM_SIZE);
        iplay_clean_11c43(mem, flag, byte_2461e, byte_2461f);
        printf("data=");
        print_bytes(mem + 0x0032, 10);
        print_bytes(mem + 0x003e, 2);
        print_bytes(mem + 0x0050, 10);
        print_bytes(mem + 0x005e, 2);
        print_bytes(mem + 0x007a, 1);
        print_bytes(mem + 0x0085, 1);
        print_bytes(mem + 0x00d3, 1);
        print_bytes(mem + 0x0090, 2);
        print_bytes(mem + 0x00d9, 1);
        print_bytes(mem + 0x00da, 1);
        print_bytes(mem + 0x00de, 1);
        print_bytes(mem + 0x0130, 2);
        print_bytes(mem + 0x3648, 4);
        print_bytes(mem + 0x3a48, 4);
        print_bytes(mem + 0x3b48, 4);
        print_bytes(mem + 0x3c48, 4);
        print_bytes(mem + 0x3d48, 4);
        print_bytes(mem + 0x3628, 3);
        printf("\n");
        return 0;
    }

    if (streq(op, "modsubdelta")) {
        db flag;
        db reset;
        db previous;
        size_t len;
        if (argc < 6) return 2;
        flag = (db)parse_u32(argv[2]);
        reset = (db)parse_u32(argv[3]);
        previous = (db)parse_u32(argv[4]);
        len = strlen(argv[5]);
        memcpy(mem + DSEG_SCRATCH, argv[5], len);
        r.esi = DSEG_SCRATCH;
        r.ecx = (dw)len;
        iplay_mod_sub_delta(&r, mem, flag, reset, &previous);
        printf("si=%04x cx=%04x data=",
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.ecx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, len);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub11ba6")) {
        db current_max;
        if (argc != 7) return 2;
        current_max = (db)parse_u32(argv[6]);
        r.ecx = ((dw)((db)parse_u32(argv[2])) << 8) | (db)parse_u32(argv[3]);
        r.ebx = (dw)parse_u32(argv[4]);
        r.edx = (dw)parse_u32(argv[5]);
        r.edi = DSEG_SCRATCH;
        memset(mem + DSEG_SCRATCH, 0x2e, 8);
        memset(mem + ORIG_DST_OFF, 0x2e, 8);
        iplay_sub_11ba6(&r, mem, &current_max);
        printf("di=%04x data=", (unsigned)(r.edi & 0xffffu));
        print_bytes(mem + ORIG_DST_OFF, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "mod102f5")) {
        char hexbuf[300];
        const char *hex;
        dw value;
        if (argc != 3) return 2;
        hex = argv[2];
        if (argv[2][0] == '@') {
            if (read_text_file_arg(argv[2], hexbuf, sizeof(hexbuf)) <= 0) return 2;
            hex = hexbuf;
        }
        memset(mem + DSEG_SCRATCH, 0, 128);
        parse_hex_bytes(hex, mem + DSEG_SCRATCH, 128);
        value = iplay_mod_102f5(mem + DSEG_SCRATCH);
        printf("data=%02x%02x\n", (unsigned)(value & 0xffu), (unsigned)(value >> 8));
        return 0;
    }

    if (streq(op, "sub126a9")) {
        if (argc != 7) return 2;
        iplay_sub_126a9(&r,
                        (dw)parse_u32(argv[2]),
                        (dw)parse_u32(argv[3]),
                        (dw)parse_u32(argv[4]),
                        (db)parse_u32(argv[5]),
                        (dd)parse_u32(argv[6]));
        printf("ax=%04x bx=%04x cx=%04x si=%04x di=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        return 0;
    }

    if (streq(op, "ultreadfast")) {
        dw word_value;
        if (argc != 3) return 2;
        word_value = (dw)parse_u32(argv[2]);
        mem[0xc09b] = (db)word_value;
        mem[0xc09c] = (db)(word_value >> 8);
        mem[0xc09d] = 0xa5;
        mem[0xc09e] = 0xa5;
        mem[0xc09f] = 0xa5;
        mem[0xc0a0] = 0xa5;
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        iplay_ult_read_fast(&r, mem);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + 0xc09b, 6);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub1265d")) {
        if (argc != 12) return 2;
        iplay_sub_1265d(&r,
                        (dw)parse_u32(argv[2]),
                        (db)parse_u32(argv[3]),
                        (db)parse_u32(argv[4]),
                        (db)parse_u32(argv[5]),
                        (db)parse_u32(argv[6]),
                        (db)parse_u32(argv[7]),
                        (db)parse_u32(argv[8]),
                        (db)parse_u32(argv[9]),
                        (dw)parse_u32(argv[10]),
                        (dw)parse_u32(argv[11]));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x bp=%04x si=%04x di=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.ebp & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        return 0;
    }

    if (streq(op, "memfree125da")) {
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        iplay_memfree_125da_guard(&r);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "mod1021e")) {
        char pattern_hex[300];
        char title_hex[128];
        const char *pattern_arg;
        const char *title_arg;
        if (argc != 6) return 2;
        pattern_arg = argv[4];
        title_arg = argv[5];
        if (pattern_arg[0] == '@') {
            if (read_text_file_arg(pattern_arg, pattern_hex, sizeof(pattern_hex)) <= 0) return 2;
            pattern_arg = pattern_hex;
        }
        if (title_arg[0] == '@') {
            if (read_text_file_arg(title_arg, title_hex, sizeof(title_hex)) <= 0) return 2;
            title_arg = title_hex;
        }
        memset(mem + DSEG_SCRATCH, 0, 152);
        memset(mem + DSEG_SCRATCH + 0x200, 0, 128);
        memset(mem + DSEG_SCRATCH + 0x300, 0, 20);
        parse_hex_bytes(pattern_arg, mem + DSEG_SCRATCH + 0x200, 128);
        parse_hex_bytes(title_arg, mem + DSEG_SCRATCH + 0x300, 20);
        iplay_mod_1021e(mem + DSEG_SCRATCH,
                        (db)parse_u32(argv[2]),
                        (db)parse_u32(argv[3]),
                        mem + DSEG_SCRATCH + 0x200,
                        mem + DSEG_SCRATCH + 0x300);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 152);
        printf("\n");
        return 0;
    }

    if (streq(op, "mod1024a")) {
        char sample_hex[256];
        const char *sample_arg;
        dw count;
        if (argc != 4) return 2;
        count = (dw)parse_u32(argv[2]);
        sample_arg = argv[3];
        if (sample_arg[0] == '@') {
            if (read_text_file_arg(sample_arg, sample_hex, sizeof(sample_hex)) <= 0) return 2;
            sample_arg = sample_hex;
        }
        memset(mem + DSEG_SCRATCH, 0, 6u + (dw)(count * 0x40u));
        memset(mem + DSEG_SCRATCH + 0x400, 0, (dw)(count * 30u));
        parse_hex_bytes(sample_arg, mem + DSEG_SCRATCH + 0x400, (dw)(count * 30u));
        iplay_mod_1024a(mem + DSEG_SCRATCH, count, mem + DSEG_SCRATCH + 0x400, 8363);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 6u + (dw)(count * 0x40u));
        printf("\n");
        return 0;
    }

    if (streq(op, "memfree18a28")) {
        if (argc != 3) return 2;
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        iplay_memfree_18a28_guard(&r, (db)parse_u32(argv[2]));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "sub11c0c")) {
        if (argc != 4) return 2;
        memset(mem, 0, 0x100);
        parse_hex_bytes(argv[3], mem, 0x100);
        r.eax = (db)parse_u32(argv[2]);
        iplay_sub_11c0c(&r, mem);
        printf("ax=%04x si=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.esi & 0xffffu));
        return 0;
    }

    if (streq(op, "sub1415e")) {
        dw index;
        if (argc != 6) return 2;
        index = (dw)parse_u32(argv[2]);
        memset(mem, 0, RUNNER_MEM_SIZE);
        iplay_sub_1415e(&r,
                        mem,
                        index,
                        (dw)parse_u32(argv[3]),
                        (db)parse_u32(argv[4]),
                        (db)parse_u32(argv[5]));
        r.esi = 0x3d48u + (index >> 3) + 1u;
        printf("si=%04x data=", (unsigned)(r.esi & 0xffffu));
        print_bytes(mem + 0x0014, 2);
        print_bytes(mem + 0x0050, 12);
        print_bytes(mem + 0x00c9, 5);
        print_bytes(mem + 0x3d48 + (index >> 3), 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub12f56")) {
        dw index;
        if (argc != 7) return 2;
        index = (dw)parse_u32(argv[2]);
        memset(mem, 0, RUNNER_MEM_SIZE);
        iplay_sub_12f56(&r,
                        mem,
                        index,
                        (dw)parse_u32(argv[3]),
                        (db)parse_u32(argv[4]),
                        (db)parse_u32(argv[5]),
                        (db)parse_u32(argv[6]));
        r.esi = 0x3d48u + (index >> 3) + 1u;
        printf("si=%04x data=", (unsigned)(r.esi & 0xffffu));
        print_bytes(mem + 0x0014, 2);
        print_bytes(mem + 0x0050, 12);
        print_bytes(mem + 0x00c9, 5);
        print_bytes(mem + 0x3d48 + (index >> 3), 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub135ca")) {
        memset(mem, 0, RUNNER_MEM_SIZE);
        mem[0x0014] = (db)DSEG_SCRATCH;
        mem[0x0015] = (db)(DSEG_SCRATCH >> 8);
        mem[DSEG_SCRATCH] = 0;
        mem[0x1368 + 0x0a] = 0xef;
        mem[0x1368 + 0x0b] = 0xbe;
        mem[0x1368 + 0x17] = 0;
        mem[0x1368 + 0x3d] = 0xaa;
        iplay_sub_135ca_zero_event(&r, mem);
        printf("si=%04x data=", (unsigned)(r.esi & 0xffffu));
        print_bytes(mem + 0x0014, 2);
        print_bytes(mem + 0x1368 + 0x0a, 2);
        print_bytes(mem + 0x1368 + 0x17, 1);
        print_bytes(mem + 0x1368 + 0x3d, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "spectr1b084len2")) {
        dw buf = DSEG_SCRATCH + 0x300;
        mem[0x7d24] = 2;
        mem[0x7d25] = 0;
        mem[0x7d30] = 1;
        mem[0x7d31] = 0;
        store_dword(mem + buf + 0, 0x00010000u);
        store_dword(mem + buf + 4, 0x00020000u);
        store_dword(mem + buf + 8, 0x00030000u);
        store_dword(mem + buf + 12, 0x00040000u);
        iplay_spectr_1b084_len2(&r, mem, buf);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        print_bytes(mem + buf, 16);
        print_bytes(mem + 0x7d1e, 2);
        print_bytes(mem + 0x7cd8, 12);
        printf("\n");
        return 0;
    }

    if (streq(op, "f5drawspectrinactive")) {
        memset(mem, 0, RUNNER_MEM_SIZE);
        iplay_f5_draw_spectr_inactive(&r, mem);
        printf("ax=%04x cx=%04x si=%04x di=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        print_bytes(mem + 0x7a14, 16);
        print_bytes(mem + 0x7758, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "fillbuf")) {
        dw count;
        unsigned i;
        IplayRegs6Result result;
        if (argc != 4) return 2;
        count = (dw)parse_u32(argv[3]);
        for (i = 0; i < 64; ++i) mem[DSEG_SCRATCH + i] = (db)(0x10u + i);
        for (i = 0; i < 8; ++i) mem[DSEG_SCRATCH + 0x100u + i] = 0xa5;
        result = iplay_fill_dma_small_result(mem, argv[2], DSEG_SCRATCH, DSEG_SCRATCH + 0x100u, count, 0, 0, count, 0, DSEG_SCRATCH, DSEG_SCRATCH + 0x100u);
        printf("si=%04x di=%04x data=",
               (unsigned)(result.esi & 0xffffu),
               (unsigned)(result.edi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH + 0x100u, 8);
        printf("\n");
        return 0;
    }

    if (effop(op, "filldmainactivemono")) {
        IplayRegs6Result result = iplay_fill_dma_inactive_mono_result(mem, DSEG_SCRATCH);
        printf("di=%04x data=", (unsigned)(result.edi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "keybsw")) {
        if (argc != 4) return 2;
        if (streq(argv[2], "get")) iplay_get_keybsw(&r, mem + DSEG_SCRATCH, (dw)parse_u32(argv[3]));
        else if (streq(argv[2], "set")) iplay_set_keybsw(&r, mem + DSEG_SCRATCH, (dw)parse_u32(argv[3]));
        else return 2;
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "int9keyb")) {
        iplay_int9_keyb_no_scancode(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 5);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub197f2")) {
        if (argc != 3) return 2;
        iplay_sub_197f2_labels(&r, mem + DSEG_SCRATCH, (dw)parse_u32(argv[2]));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 6);
        printf("\n");
        return 0;
    }

    if (streq(op, "useless11787zero")) {
        iplay_useless_11787_zero(&r, mem + DSEG_SCRATCH, 0x1368);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "timerport")) {
        if (argc != 4) return 2;
        iplay_timer_port_no_device(&r, mem + DSEG_SCRATCH, argv[2], (dw)parse_u32(argv[3]));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "settimerint")) {
        iplay_set_timer_int_alloc_fail(&r, mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "uselessdoswrite2")) {
        iplay_useless_doswrite2_header(&r, mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "uselessdoswrite")) {
        iplay_useless_doswrite_header(&r, mem + DSEG_SCRATCH);
        printf("dx=%04x data=", (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "ult1150b")) {
        if (argc != 3) return 2;
        iplay_ult_1150b(&r, (dw)parse_u32(argv[2]));
        printf("ax=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "egaseq")) {
        if (argc != 3) return 2;
        iplay_ega_seq_no_device(&r, streq(argv[2], "set_egasequencer"));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "uselessunsetegaseq")) {
        if (argc != 3) return 2;
        iplay_useless_unset_egaseq(&r, (db)parse_u32(argv[2]));
        printf("ax=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "uselessstrange")) {
        iplay_useless_strange_short(&r, mem + DSEG_SCRATCH);
        printf("si=%04x di=%04x data=",
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "uselesswriteinr118")) {
        iplay_useless_writeinr_118_header(&r, mem + DSEG_SCRATCH);
        printf("dx=%04x data=", (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 96);
        printf("\n");
        return 0;
    }

    if (streq(op, "uselesswriteinrfail")) {
        iplay_useless_writeinr_fail(&r);
        printf("ax=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "useless12d61")) {
        iplay_useless_12d61_no_device(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "txtblink") || streq(op, "abitxtblink")) {
        if (argc != 3) return 2;
        iplay_txt_blink_no_device(&r, streq(argv[2], "txt_enableblink"));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "mysprintfchunk")) {
        if (argc != 4) return 2;
        iplay_useless_sprintf_chunk(&r, mem + DSEG_SCRATCH, argv[2], (dd)parse_u32(argv[3]));
        printf("si=%04x di=%04x es=%04x data=",
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu),
               0x0d8f);
        print_bytes(mem + DSEG_SCRATCH, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "sndonparntbounded")) {
        memset(mem, 0, RUNNER_MEM_SIZE);
        mem[0x00c9] = 0x91;
        mem[0x00ca] = 0x92;
        mem[0x00cb] = 0x93;
        mem[0x00cc] = 0x94;
        mem[0x00cd] = 0x95;
        mem[0x00d1] = 0x96;
        mem[0x00df] = 0x97;
        mem[0x0060] = 0x11;
        mem[0x0061] = 0x11;
        mem[0x0062] = 0x22;
        mem[0x0063] = 0x22;
        mem[0x0080] = 0x33;
        mem[0x0081] = 0x44;
        mem[0x00d9] = 6;
        mem[0x00da] = 125;
        memset(mem + 0x1368, 0xa5, 0x20);
        iplay_snd_on_parnt_bounded(mem);
        mem[DSEG_SCRATCH + 0] = mem[0x00c9];
        mem[DSEG_SCRATCH + 1] = mem[0x00ca];
        mem[DSEG_SCRATCH + 2] = mem[0x00cb];
        mem[DSEG_SCRATCH + 3] = mem[0x00cc];
        mem[DSEG_SCRATCH + 4] = mem[0x00cd];
        mem[DSEG_SCRATCH + 5] = mem[0x00d1];
        mem[DSEG_SCRATCH + 6] = mem[0x00df];
        memcpy(mem + DSEG_SCRATCH + 7, mem + 0x0060, 4);
        memcpy(mem + DSEG_SCRATCH + 11, mem + 0x0080, 2);
        mem[DSEG_SCRATCH + 13] = mem[0x00c8];
        memcpy(mem + DSEG_SCRATCH + 14, mem + 0x00db, 2);
        mem[DSEG_SCRATCH + 16] = mem[0x1368];
        mem[DSEG_SCRATCH + 17] = mem[0x1387];
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 18);
        printf("\n");
        return 0;
    }

    if (streq(op, "memfree")) {
        if (argc != 3) return 2;
        (void)argv;
        iplay_memfree_invalid(&r);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "midiport")) {
        if (argc != 5) return 2;
        iplay_midi_port_no_device(&r, mem + DSEG_SCRATCH, argv[2]);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "midiset")) {
        iplay_midi_set_no_device(&r);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "midichannelport")) {
        db *channel = mem + DSEG_SCRATCH;
        if (argc != 3) return 2;
        memset(mem, 0, RUNNER_MEM_SIZE);
        mem[0x0000] = 0x30;
        channel[0x02] = 0x05;
        channel[0x03] = 0x02;
        channel[0x08] = 0x20;
        channel[0x17] = streq(argv[2], "midi_1544D") ? 0x83 : 0x00;
        channel[0x18] = 0x04;
        channel[0x1b] = 0x20;
        channel[0x35] = 0x31;
        mem[0x00bc] = 0x30;
        mem[0x00bd] = 0x03;
        mem[0x00d7] = 0x55;
        mem[0x00d8] = 0xa0;
        r.eax = 0x1234;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        iplay_midi_channel_event_no_device(&r, mem, channel, streq(argv[2], "midi_1544D"));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(channel, 0x40);
        print_bytes(mem + 0x00d7, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "intvect")) {
        if (argc != 5) return 2;
        iplay_int_vector_roundtrip(&r, (db)parse_u32(argv[2]), (dw)parse_u32(argv[3]), (dw)parse_u32(argv[4]));
        printf("ax=%04x bx=%04x dx=%04x ds=%04x es=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               0x156a,
               0x156a);
        return 0;
    }

    if (streq(op, "sndvector")) {
        if (argc != 5) return 2;
        iplay_snd_vector_roundtrip(&r, mem + DSEG_SCRATCH, (db)parse_u32(argv[2]), (dw)parse_u32(argv[3]), (dw)parse_u32(argv[4]));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.esi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 8);
        printf("\n");
        return 0;
    }

    if (effop(op, "sb16probe")) {
        IplaySb16ProbeResult result;
        if (argc != 3) return 2;
        result = iplay_sb16_probe_no_device_to_buffer(mem + DSEG_SCRATCH, argv[2]);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 10);
        printf("\n");
        return 0;
    }

    if (effop(op, "sb16initfail")) {
        IplaySb16RegsResult result = iplay_sb16_init_fail_to_buffer(mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 15);
        printf("\n");
        return 0;
    }

    if (effop(op, "sb16int")) {
        IplaySb16RegsResult result = iplay_sb16_int_ack_to_buffer(mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               0x156a);
        print_bytes(mem + DSEG_SCRATCH, 1);
        printf("\n");
        return 0;
    }

    if (effop(op, "sb16dmafail")) {
        IplaySb16RegsResult result = iplay_sb16_dma_fail_to_buffer(mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 10);
        printf("\n");
        return 0;
    }

    if (streq(op, "int1apass")) {
        iplay_int1a_passthrough(&r);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x es=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               0x0a15,
               0x0d8f);
        return 0;
    }

    if (streq(op, "inrread119b7")) {
        iplay_inr_read_119b7_eof(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "modread11f4eguard")) {
        iplay_mod_readfile_11f4e_guard(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "modread12247eof")) {
        iplay_mod_readfile_12247_eof(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "stereoint")) {
        iplay_stereo_timer_int_snapshot(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               0x156a);
        print_bytes(mem + DSEG_SCRATCH, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "timerend")) {
        iplay_timer_int_end_disabled(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               0x156a);
        print_bytes(mem + DSEG_SCRATCH, 3);
        printf("\n");
        return 0;
    }

    if (effop(op, "sb16off")) {
        IplaySb16RegsResult result;
        if (argc != 3) return 2;
        result = iplay_sb16_off_no_device_to_buffer(mem + DSEG_SCRATCH, argv[2]);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "cleandeinit")) {
        if (argc != 3) return 2;
        iplay_clean_deinit_no_device(&r, mem + DSEG_SCRATCH, argv[2]);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "dosdir")) {
        if (argc != 3) return 2;
        iplay_dos_dir_stub(&r, mem + DSEG_SCRATCH, streq(argv[2], "doschdir"));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.esi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 70);
        printf("\n");
        return 0;
    }

    if (streq(op, "dosfindnext")) {
        iplay_dos_findnext_fail(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "dosfread")) {
        iplay_dos_fread_eof(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "dosseeksuccess")) {
        iplay_dos_seek_success(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "inrread118b0fail")) {
        iplay_inr_read_118b0_fail(&r);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               0x156a);
        return 0;
    }

    if (streq(op, "read2buffer")) {
        iplay_read2buffer_empty(&r, mem + DSEG_SCRATCH);
        printf("si=%04x data=", (unsigned)(r.esi & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "memlimit")) {
        if (argc != 4) return 2;
        iplay_mem_limit(&r, (dd)parse_u32(argv[3]));
        printf("ax=%04x bx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu));
        return 0;
    }

    if (streq(op, "allocdmafail")) {
        if (argc != 4) return 2;
        iplay_alloc_dma_fail(&r, mem + DSEG_SCRATCH, (dd)parse_u32(argv[2]), (dw)parse_u32(argv[3]));
        printf("ax=%04x bx=%04x cx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 0x19);
        printf("\n");
        return 0;
    }

    if (streq(op, "gravisdma")) {
        if (argc != 3) return 2;
        iplay_gravis_dma_control(&r, mem + DSEG_SCRATCH, argv[2]);
        printf("ax=%04x cx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 11);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub1279dma")) {
        iplay_sub_1279a_dma(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x cx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 9);
        printf("\n");
        return 0;
    }

    if (streq(op, "programdma")) {
        iplay_program_dma_channel1(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x cx=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(mem + DSEG_SCRATCH, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "memstrat")) {
        if (argc != 4) return 2;
        iplay_mem_strategy(&r, argv[2], (dw)parse_u32(argv[3]));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "memreallocx")) {
        dw size;
        if (argc != 3) return 2;
        size = (dw)parse_u32(argv[2]);
        iplay_mem_reallocx_bookkeeping(mem + DSEG_SCRATCH, size);
        printf("di=%04x data=", (unsigned)size);
        print_bytes(mem + DSEG_SCRATCH, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "deinit125b9idle")) {
        iplay_deinit_125b9_idle(&r, mem + DSEG_SCRATCH);
        printf("ds=%04x data=", 0x156a);
        print_bytes(mem + DSEG_SCRATCH, 11);
        printf("\n");
        return 0;
    }

    if (streq(op, "rtcclock")) {
        if (argc != 3) return 2;
        iplay_rtc_clock_bcd_123456(&r, mem + DSEG_SCRATCH);
        printf("ax=%04x dx=%04x es=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               0);
        print_bytes(mem + DSEG_SCRATCH, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "loadcfgsuccess")) {
        iplay_loadcfg_success(&r, mem + DSEG_SCRATCH);
        printf("ds=%04x data=", 0x0d8f);
        print_bytes(mem + DSEG_SCRATCH, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "dosexecnocomspec")) {
        iplay_dosexec_no_comspec(&r, mem + DSEG_SCRATCH);
        printf("ds=%04x data=", 0x0a15);
        print_bytes(mem + DSEG_SCRATCH, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "callsubxfail")) {
        iplay_callsubx_fail(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 17);
        printf("\n");
        return 0;
    }

    if (streq(op, "memalloc12kbounded")) {
        iplay_memalloc12k_bounded(&r);
        printf("ax=%04x bx=%04x di=%04x es=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.edi & 0xffffu),
               0x2345);
        return 0;
    }

    if (streq(op, "initvgabounded")) {
        iplay_init_vga_bounded(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 5);
        printf("\n");
        return 0;
    }

    if (streq(op, "f2drawbounded")) {
        if (argc != 3) return 2;
        iplay_f2_draw_bounded(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "readallmoulesbounded")) {
        iplay_readallmoules_bounded(&r, mem + DSEG_SCRATCH);
        printf("flags=%04x data=", 0x7246);
        print_bytes(mem + DSEG_SCRATCH, 3);
        printf("\n");
        return 0;
    }

    if (streq(op, "readmodulefail")) {
        iplay_readmodule_fail(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 19);
        printf("\n");
        return 0;
    }

    if (streq(op, "modulereadfail")) {
        iplay_moduleread_fail(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 7);
        printf("\n");
        return 0;
    }

    if (streq(op, "modread10311bounded")) {
        iplay_modread_10311_bounded(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 64);
        printf("\n");
        return 0;
    }

    if (streq(op, "modntbounded")) {
        iplay_modnt_bounded(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 10);
        printf("\n");
        return 0;
    }

    if (streq(op, "formatloaderheader")) {
        if (argc != 3) return 2;
        iplay_format_loader_header(mem + DSEG_SCRATCH, argv[2]);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 20);
        printf("\n");
        return 0;
    }

    if (streq(op, "modulessearchbounded")) {
        iplay_modules_search_bounded(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 6);
        printf("\n");
        return 0;
    }

    if (streq(op, "startbounded")) {
        iplay_start_bounded(&r, mem + DSEG_SCRATCH);
        printf("ds=%04x data=", 0x0d8f);
        print_bytes(mem + DSEG_SCRATCH, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "keybbounded")) {
        iplay_keyb_bounded(mem + DSEG_SCRATCH);
        printf("data=");
        print_bytes(mem + DSEG_SCRATCH, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "memclean")) {
        unsigned fill_count;
        IplayRegs6Result result;
        if (argc != 4) return 2;
        fill_count = (unsigned)parse_u32(argv[3]);
        memset(mem + BUF_OFF, 0xa5, fill_count);
        result = iplay_memclean_result(mem, BUF_OFF, (dw)parse_u32(argv[2]), 0, 0, 0, 0, 0, BUF_OFF);
        printf("di=%04x data=", (unsigned)(result.edi & 0xffffu));
        print_bytes(mem + BUF_OFF, fill_count);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub131da")) {
        db *channel = mem + BUF_OFF;
        if (argc != 5) return 2;
        memset(channel, 0, 0x40);
        channel[0x1d] = (db)parse_u32(argv[2]);
        channel[0x17] = (db)parse_u32(argv[3]);
        channel[0x35] = (db)parse_u32(argv[4]);
        iplay_sub_131da(channel);
        printf("data=%02x%02x\n", (unsigned)channel[0x17], (unsigned)channel[0x35]);
        return 0;
    }

    if (streq(op, "sub131ef")) {
        db *channel = mem + BUF_OFF;
        if (argc != 7) return 2;
        memset(channel, 0, 0x42);
        channel[0x23] = (db)parse_u32(argv[5]);
        channel[0x3d] = (db)parse_u32(argv[6]);
        iplay_sub_131ef(channel, (db)parse_u32(argv[2]), (dw)parse_u32(argv[3]), (db)parse_u32(argv[4]));
        printf("data=%02x%02x%02x%02x%02x\n",
               (unsigned)channel[0x22],
               (unsigned)channel[0x23],
               (unsigned)channel[0x36],
               (unsigned)channel[0x37],
               (unsigned)channel[0x3d]);
        return 0;
    }

    if (streq(op, "sub13177")) {
        db *channel = mem + BUF_OFF;
        if (argc != 7) return 2;
        memset(channel, 0, 0x42);
        channel[0x3d] = (db)parse_u32(argv[6]);
        iplay_sub_13177(
            channel,
            (dw)parse_u32(argv[2]),
            (dd)parse_u32(argv[3]),
            (dd)parse_u32(argv[4]),
            (db)parse_u32(argv[5]));
        printf("data=%02x%02x%02x%02x%02x%02x%02x\n",
               (unsigned)channel[0x1e],
               (unsigned)channel[0x1f],
               (unsigned)channel[0x20],
               (unsigned)channel[0x21],
               (unsigned)channel[0x3d],
               (unsigned)channel[0x3e],
               (unsigned)channel[0x3f]);
        return 0;
    }

    if (streq(op, "sub12afd")) {
        db *channels = mem + BUF_OFF;
        dw channel_count;
        db channel_index;
        if (argc != 6) return 2;
        memset(channels, 0, 0x50 * 4);
        r.eax = parse_u32(argv[2]);
        channel_count = (dw)parse_u32(argv[3]);
        channel_index = (db)parse_u32(argv[4]);
        channels[(dw)channel_index * 0x50u + 0x17] = (db)parse_u32(argv[5]);
        iplay_sub_12afd(&r, channels, channel_count, channel_index, 0);
        printf("data=%02x\n", (unsigned)channels[(dw)channel_index * 0x50u + 0x17]);
        return 0;
    }

    if (streq(op, "sub12b18")) {
        db *globals = mem;
        db *channels_buf = mem + BUF_OFF;
        db src[32];
        dw channel_count;
        dw i;
        if (argc != 4) return 2;
        memset(globals, 0, 0x100);
        memset(channels_buf, 0, 0x50 * 32);
        memset(src, 0, sizeof(src));
        channel_count = (dw)parse_u32(argv[2]);
        parse_hex_bytes(argv[3], src, sizeof(src));
        iplay_sub_12b18(globals, channels_buf, src, channel_count, 0);
        printf("data=");
        print_bytes(globals + 0x007c, 2);
        for (i = 0; i < channel_count; ++i) {
            db *channel = channels_buf + i * 0x50u;
            print_bytes(channel + 0x18, 1);
            print_bytes(channel + 0x3a, 1);
        }
        printf("\n");
        return 0;
    }

    if (streq(op, "sub12b83")) {
        db *globals = mem;
        db *channels_buf = mem + BUF_OFF;
        db types[32];
        dw channel_count;
        dw i;
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        memset(channels_buf, 0, 0x50 * 32);
        memset(types, 0, sizeof(types));
        parse_hex_bytes(argv[3], types, sizeof(types));
        globals[0x00de] = (db)parse_u32(argv[4]);
        globals[0x00be] = 0x22;
        globals[0x00bf] = 0x56;
        globals[0x005e] = 100;
        globals[0x005f] = 0;
        globals[0x0089] = 0x20;
        channel_count = (db)parse_u32(argv[2]);
        if (channel_count >= 0x20) channel_count = 0x20;
        if (channel_count <= 2) channel_count = 2;
        for (i = 0; i < channel_count; ++i) {
            channels_buf[i * 0x50u + 0x3e] = 0xaa;
            channels_buf[i * 0x50u + 0x3f] = 0xaa;
        }
        iplay_sub_12b83_state(globals, channels_buf, 0x50, types, (db)parse_u32(argv[2]));
        printf("data=");
        print_bytes(globals + 0x0034, 8);
        print_bytes(globals + 0x007c, 2);
        print_bytes(globals + 0x001c, 8);
        print_bytes(globals + 0x00dd, 2);
        for (i = 0; i < channel_count; ++i) {
            db *channel = channels_buf + i * 0x50u;
            print_bytes(channel + 0x18, 1);
            print_bytes(channel + 0x3e, 2);
        }
        printf("\n");
        return 0;
    }

    if (streq(op, "someplaymode")) {
        db *globals = mem;
        db *channels_buf = mem + BUF_OFF;
        dw channel_count;
        if (argc != 7) return 2;
        memset(globals, 0, 0x200);
        memset(channels_buf, 0, 0x50 * 32);
        globals[0x00d2] = (db)parse_u32(argv[2]);
        globals[0x00be] = (db)parse_u32(argv[3]);
        globals[0x00bf] = (db)(parse_u32(argv[3]) >> 8);
        channel_count = (dw)parse_u32(argv[4]);
        globals[0x007a] = (db)parse_u32(argv[5]);
        globals[0x0082] = (db)parse_u32(argv[6]);
        globals[0x0089] = 0x20;
        channels_buf[0x3e] = 0xaa;
        channels_buf[0x3f] = 0xaa;
        iplay_someplaymode(globals, channels_buf, channel_count, 0x50);
        printf("data=");
        print_bytes(globals + 0x001c, 8);
        print_bytes(globals + 0x009c, 4);
        print_bytes(channels_buf + 0x3e, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub12d05")) {
        db *dst = mem + DSEG_SCRATCH;
        IplayRegs6Result result;
        if (argc != 4) return 2;
        memset(dst, 0, 64);
        result = iplay_sub_12d05_to_buffer(dst, (db)parse_u32(argv[2]), (db)parse_u32(argv[3]), 0, 0, 0, 0, 0, DSEG_SCRATCH);
        printf("cx=%04x data=", (unsigned)(result.ecx & 0xffffu));
        print_bytes(dst, strlen("Device not initialised!"));
        printf("\n");
        return 0;
    }

    if (streq(op, "sub12d35disable")) {
        db code_byte = 0xff;
        if (argc != 3) return 2;
        (void)parse_u32(argv[2]);
        code_byte = iplay_sub_12d35_disable_code(0);
        printf("ax=%04x bx=%04x data=%02x\n",
               0,
               0,
               (unsigned)code_byte);
        return 0;
    }

    if (streq(op, "sub12da8guard")) {
        db *globals = mem;
        memset(globals, 0, 0x200);
        globals[0x00e0] = 1;
        iplay_sub_12da8_guard_state(globals, 0x1603, 0x7856, 0x0907, 0x0220, 0x0084);
        printf("data=");
        print_bytes(globals + 0x0132, 11);
        print_bytes(globals + 0x00be, 2);
        print_bytes(globals + 0x00e0, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub1281asmallmix")) {
        db *dst = mem + DSEG_SCRATCH + 0x2a0;
        db *samples = mem;
        db *vlm_table = mem + 0x3d68;
        db *channel = mem + BUF_OFF;
        db sample = (argc >= 3) ? (db)parse_u32(argv[2]) : 0;
        IplayRegs6Result result;
        memset(dst, 0xa5, 4);
        memset(samples, 0, 8);
        memset(vlm_table, 0, 0x20);
        memset(channel, 0, 0x50);
        samples[0] = sample;
        samples[1] = 1;
        samples[2] = 2;
        samples[3] = 3;
        vlm_table[1] = 0x11;
        vlm_table[3] = 0x22;
        vlm_table[5] = 0x33;
        vlm_table[7] = 0x44;
        channel[0x20] = 0;
        channel[0x21] = 1;
        channel[0x23] = 0;
        result = iplay_sub_1281a_small_result(dst, samples, vlm_table, channel, 1, 4, 0, 0, 0, 0, 0, DSEG_SCRATCH + 0x2a0);
        result.esi = DSEG_SCRATCH + 0x2a0 + 4;
        result.edi = DSEG_SCRATCH + 0x2c0 + 4;
        printf("ax=%04x cx=%04x dx=%04x si=%04x di=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               (unsigned)(result.esi & 0xffffu),
               (unsigned)(result.edi & 0xffffu));
        print_bytes(dst, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub12cadguard")) {
        db *event_store = mem + BUF_OFF;
        IplayRegs6Result result;
        dd ecx;
        if (argc != 7) return 2;
        memset(event_store, 0, 5);
        ecx = ((dw)parse_u32(argv[2]) << 8) | ((dw)parse_u32(argv[3]) & 0xffu);
        result = iplay_sub_12cad_guard_result(event_store, (dw)parse_u32(argv[6]), 0, parse_u32(argv[4]), ecx, parse_u32(argv[5]), 0, 0);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               (unsigned)(result.esi & 0xffffu));
        print_bytes(event_store, 5);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub13623guard")) {
        IplayRegs6Result result;
        if (argc != 5) return 2;
        result = iplay_sub_13623_guard_result((dw)parse_u32(argv[4]), parse_u32(argv[2]), 0, 0, parse_u32(argv[3]), DSEG_SCRATCH, 0);
        printf("ax=%04x dx=%04x si=%04x\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               (unsigned)(result.esi & 0xffffu));
        return 0;
    }

    if (streq(op, "setplaysettings")) {
        db *globals = mem;
        db *channels_buf = mem + BUF_OFF;
        dw channel_count;
        IplaySb16RegsResult result;
        if (argc != 7) return 2;
        memset(globals, 0, 0x200);
        memset(channels_buf, 0, 0x50 * 32);
        globals[0x00d3] = (db)parse_u32(argv[3]);
        globals[0x00be] = (db)parse_u32(argv[4]);
        globals[0x00bf] = (db)(parse_u32(argv[4]) >> 8);
        channel_count = (dw)parse_u32(argv[5]);
        globals[0x007a] = (db)parse_u32(argv[6]);
        globals[0x0089] = 0x20;
        channels_buf[0x3e] = 0xaa;
        channels_buf[0x3f] = 0xaa;
        result = iplay_set_playsettings_result(globals, channels_buf, channel_count, 0x50, parse_u32(argv[2]), 0, 0, 0);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(globals + 0x00d2, 2);
        print_bytes(globals + 0x001c, 8);
        print_bytes(channels_buf + 0x3e, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "volume12a66")) {
        IplaySb16RegsResult result;
        if (argc != 3) return 2;
        result = iplay_volume_12a66_result((dw)parse_u32(argv[2]), 0x1234, 0x5678, 0x9abc, 0);
        printf("ax=%04x bx=%04x cx=%04x\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu));
        return 0;
    }

    if (streq(op, "vlm141df")) {
        db *globals = mem;
        IplaySb16RegsResult result;
        memset(globals, 0, 0x200);
        result = iplay_vlm_141df_result(globals, 1, 0x1234, 0x5678, 0x9abc, 0xdef0);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               (unsigned)globals[0x00d1]);
        return 0;
    }

    if (streq(op, "changevolume")) {
        db *globals = mem;
        db *channels_buf = mem + BUF_OFF;
        dw channel_count;
        IplaySb16RegsResult result;
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        memset(channels_buf, 0, 0x50 * 32);
        channel_count = (dw)parse_u32(argv[3]);
        globals[0x005c] = 0;
        globals[0x005d] = 1;
        channels_buf[0x08] = (db)parse_u32(argv[4]);
        result = iplay_change_volume_result(globals, channels_buf, channel_count, parse_u32(argv[2]), 0, 0, 0);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(globals + 0x005c, 2);
        print_bytes(channels_buf + 0x08, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub13429guard")) {
        db *channel = mem + BUF_OFF;
        memset(channel, 0, 0x40);
        channel[0x03] = 0x55;
        channel[0x17] = 0;
        r.eax = 0x1234;
        r.ebx = 0x9000;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        iplay_sub_13429_guard(&r, channel);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x data=%02x%02x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)channel[0x03],
               (unsigned)channel[0x17]);
        return 0;
    }

    if (streq(op, "sub137d5guard")) {
        db *channel = mem + BUF_OFF;
        if (argc != 3) return 2;
        memset(channel, 0, 0x40);
        channel[0x0a] = 33;
        channel[0x0b] = 0x77;
        channel[0x3d] = (db)parse_u32(argv[2]);
        r.eax = 0x1234;
        r.ebx = 0x9000;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        iplay_sub_137d5_guard(&r, channel);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=%02x%02x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.edi & 0xffffu),
               (unsigned)channel[0x0a],
               (unsigned)channel[0x3d]);
        return 0;
    }

    if (streq(op, "sub13813")) {
        db *channel = mem + BUF_OFF;
        memset(channel, 0, 0x40);
        channel[0x0a] = 33;
        channel[0x0b] = 0x7c;
        r.eax = 0x1234;
        r.ecx = 0x5678;
        r.edx = 0x9abc;
        r.edi = 0xdef0;
        iplay_sub_13813_guard(&r, channel);
        printf("ax=%04x cx=%04x dx=%04x di=%04x data=%02x%02x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.edi & 0xffffu),
               (unsigned)channel[0x0a],
               (unsigned)channel[0x0b]);
        return 0;
    }

    if (streq(op, "sub140b6guard")) {
        db *globals = mem;
        memset(globals, 0, 0x200);
        globals[0x00d1] = 1;
        globals[0x00c8] = 0;
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        iplay_sub_140b6_guard(&r, globals);
        r.ecx = 0;
        printf("ax=%04x bx=%04x cx=%04x data=%02x%02x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)globals[0x00d1],
               (unsigned)globals[0x00c8]);
        return 0;
    }

    if (streq(op, "eff13bc0") || streq(op, "eff13c34")) {
        db *channel = mem + BUF_OFF;
        if (argc != 4) return 2;
        memset(channel, 0, 0x10);
        channel[0x09] = (db)parse_u32(argv[2]);
        if (streq(op, "eff13bc0")) {
            iplay_eff_13bc0(channel, (db)parse_u32(argv[3]));
        } else {
            iplay_eff_13c34(channel, (db)parse_u32(argv[3]));
        }
        printf("data=%02x\n", (unsigned)channel[0x09]);
        return 0;
    }

    if (streq(op, "eff13a43")) {
        db *channel = mem + BUF_OFF;
        if (argc != 5) return 2;
        memset(channel, 0, 0x40);
        channel[0x17] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[4]);
        iplay_eff_13a43_state(channel, (db)r.eax, (db)parse_u32(argv[3]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13a43")) {
        db *channel = mem + BUF_OFF;
        if (argc != 5) return 2;
        memset(channel, 0, 0x40);
        channel[0x17] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[4]);
        iplay_eff_13a43(&r, channel, (db)parse_u32(argv[3]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13bb2") || streq(op, "eff13ba3")) {
        db *channel = mem + BUF_OFF;
        IplayRegs6Result result;
        if (argc != 4) return 2;
        memset(channel, 0, 0x40);
        channel[0x17] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[3]);
        if (streq(op, "eff13bb2")) iplay_eff_13bb2_state(channel, (db)r.eax);
        else {
            result = iplay_eff_13ba3_result(channel, (db)r.eax, r.eax, r.ebx, r.ecx, r.edx, r.esi, r.edi);
            r.eax = result.eax;
            r.edi = result.edi;
        }
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13bb2") || streq(op, "abieff13ba3")) {
        db *channel = mem + BUF_OFF;
        if (argc != 4) return 2;
        memset(channel, 0, 0x40);
        channel[0x17] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[3]);
        if (streq(op, "abieff13bb2")) iplay_eff_13bb2(&r, channel);
        else iplay_eff_13ba3(&r, channel);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13a94")) {
        db *channel = mem + BUF_OFF;
        dd sample_end;
        if (argc != 7) return 2;
        memset(channel, 0, 0x50);
        channel[0x16] = (db)parse_u32(argv[2]);
        sample_end = (dd)parse_u32(argv[3]);
        channel[0x30] = (db)sample_end;
        channel[0x31] = (db)(sample_end >> 8);
        channel[0x32] = (db)(sample_end >> 16);
        channel[0x33] = (db)(sample_end >> 24);
        channel[0x17] = (db)parse_u32(argv[5]);
        channel[0x4c] = 0xaa;
        channel[0x4d] = 0xaa;
        r.eax = parse_u32(argv[6]);
        iplay_eff_13a94(&r, channel, (db)parse_u32(argv[4]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x16, 2);
        print_bytes(channel + 0x4c, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13ad7")) {
        db *channel = mem + BUF_OFF;
        IplayRegs3Result result;
        if (argc != 5) return 2;
        memset(channel, 0, 0x10);
        channel[0x08] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[4]);
        result = iplay_eff_13ad7_result(channel, (db)r.eax, (db)parse_u32(argv[3]), r.eax, r.ecx, r.edx);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13ad7")) {
        db *channel = mem + BUF_OFF;
        if (argc != 5) return 2;
        memset(channel, 0, 0x10);
        channel[0x08] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[4]);
        iplay_eff_13ad7(&r, channel, (db)parse_u32(argv[3]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13b06")) {
        db *globals = mem;
        dw ax;
        if (argc != 4) return 2;
        memset(globals, 0, 0x200);
        globals[0x0050] = 0xaa;
        globals[0x0051] = 0xaa;
        r.eax = parse_u32(argv[3]);
        ax = iplay_eff_13b06_ax(globals, (db)r.eax);
        printf("ax=%04x data=", (unsigned)ax);
        print_bytes(globals + 0x0050, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13b06")) {
        db *globals = mem;
        if (argc != 4) return 2;
        memset(globals, 0, 0x200);
        globals[0x0050] = 0xaa;
        globals[0x0051] = 0xaa;
        r.eax = parse_u32(argv[3]);
        iplay_eff_13b06(&r, globals, (db)parse_u32(argv[2]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x0050, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13b78")) {
        db *channel = mem + BUF_OFF;
        db al;
        if (argc != 4) return 2;
        memset(channel, 0, 0x10);
        r.eax = parse_u32(argv[2]);
        al = iplay_eff_13b78_al(channel, (db)r.eax, (db)parse_u32(argv[3]));
        r.eax = (r.eax & 0xffffff00UL) | al;
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13b78")) {
        db *channel = mem + BUF_OFF;
        if (argc != 4) return 2;
        memset(channel, 0, 0x10);
        r.eax = parse_u32(argv[2]);
        iplay_eff_13b78(&r, channel, (db)parse_u32(argv[3]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13b88")) {
        db *globals = mem;
        IplayRegs3Result result;
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        globals[0x00c9] = (db)parse_u32(argv[2]);
        globals[0x00ca] = (db)parse_u32(argv[3]);
        r.eax = parse_u32(argv[4]);
        result = iplay_eff_13b88_result(globals, (db)r.eax, r.eax, r.ecx, r.edx);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(globals + 0x00c9, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13b88")) {
        db *globals = mem;
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        globals[0x00c9] = (db)parse_u32(argv[2]);
        globals[0x00ca] = (db)parse_u32(argv[3]);
        r.eax = parse_u32(argv[4]);
        iplay_eff_13b88(&r, globals);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x00c9, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13bc8")) {
        db *channel = mem + BUF_OFF;
        IplayRegs6Result result;
        if (argc != 5) return 2;
        memset(channel, 0, 0x40);
        r.edx = parse_u32(argv[3]);
        r.eax = parse_u32(argv[4]);
        result = iplay_eff_13bc8_result(channel, (db)r.eax, (dw)r.edx, (db)parse_u32(argv[2]), r.eax, r.ebx, r.ecx, r.edx, r.esi, r.edi);
        printf("ax=%04x dx=%04x data=", (unsigned)(result.eax & 0xffffu), (unsigned)(result.edx & 0xffffu));
        print_bytes(channel + 0x14, 2);
        print_bytes(channel + 0x38, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13bc8")) {
        db *channel = mem + BUF_OFF;
        if (argc != 5) return 2;
        memset(channel, 0, 0x40);
        r.edx = parse_u32(argv[3]);
        r.eax = parse_u32(argv[4]);
        iplay_eff_13bc8(&r, channel, (db)parse_u32(argv[2]));
        printf("ax=%04x dx=%04x data=", (unsigned)(r.eax & 0xffffu), (unsigned)(r.edx & 0xffffu));
        print_bytes(channel + 0x14, 2);
        print_bytes(channel + 0x38, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13c02")) {
        db *globals = mem;
        db *channel = mem + BUF_OFF;
        if (argc != 7) return 2;
        memset(globals, 0, 0x200);
        memset(channel, 0, 0x40);
        globals[0x00c8] = (db)parse_u32(argv[2]);
        globals[0x00c9] = 0xaa;
        globals[0x00cb] = 0xbb;
        channel[0x3b] = (db)parse_u32(argv[4]);
        channel[0x3c] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[6]);
        r.eax = iplay_eff_13c02_eax(channel, globals, (db)r.eax, (dw)parse_u32(argv[3]), r.eax);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x3b, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13c02")) {
        db *globals = mem;
        db *channel = mem + BUF_OFF;
        if (argc != 7) return 2;
        memset(globals, 0, 0x200);
        memset(channel, 0, 0x40);
        globals[0x00c8] = (db)parse_u32(argv[2]);
        globals[0x00c9] = 0xaa;
        globals[0x00cb] = 0xbb;
        channel[0x3b] = (db)parse_u32(argv[4]);
        channel[0x3c] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[6]);
        iplay_eff_13c02(&r, channel, globals, (dw)parse_u32(argv[3]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x3b, 2);
        printf(" globals=");
        print_bytes(globals + 0x00c8, 4);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13c3f")) {
        db *channel = mem + BUF_OFF;
        IplayRegs6Result result;
        if (argc != 6) return 2;
        memset(channel, 0, 0x40);
        channel[0x17] = (db)parse_u32(argv[3]);
        r.eax = parse_u32(argv[5]);
        result = iplay_eff_13c3f_result(channel, (db)r.eax, (db)parse_u32(argv[2]), (db)parse_u32(argv[4]), r.eax, r.ebx, r.ecx, r.edx, r.esi, r.edi);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13c3f")) {
        db *channel = mem + BUF_OFF;
        if (argc != 6) return 2;
        memset(channel, 0, 0x40);
        channel[0x17] = (db)parse_u32(argv[3]);
        r.eax = parse_u32(argv[5]);
        iplay_eff_13c3f(&r, channel, (db)parse_u32(argv[2]), (db)parse_u32(argv[4]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13c64")) {
        db *channel = mem + BUF_OFF;
        IplayRegs3Result result;
        if (argc != 5) return 2;
        memset(channel, 0, 0x40);
        channel[0x3d] = (db)parse_u32(argv[3]);
        r.eax = parse_u32(argv[4]);
        result = iplay_eff_13c64_result(channel, (db)r.eax, (db)parse_u32(argv[2]), r.eax, r.ecx, r.edx);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(channel + 0x3d, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13c64")) {
        db *channel = mem + BUF_OFF;
        if (argc != 5) return 2;
        memset(channel, 0, 0x40);
        channel[0x3d] = (db)parse_u32(argv[3]);
        r.eax = parse_u32(argv[4]);
        iplay_eff_13c64(&r, channel, (db)parse_u32(argv[2]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x3d, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13c88")) {
        db *channel = mem + BUF_OFF;
        IplayRegs3Result result;
        if (argc != 6) return 2;
        memset(channel, 0, 0x10);
        channel[0x08] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[5]);
        result = iplay_eff_13c88_result(channel, (db)r.eax, (db)parse_u32(argv[3]), (db)parse_u32(argv[4]), r.eax, r.ecx, r.edx);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13c88")) {
        db *channel = mem + BUF_OFF;
        if (argc != 6) return 2;
        memset(channel, 0, 0x10);
        channel[0x08] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[5]);
        iplay_eff_13c88(&r, channel, (db)parse_u32(argv[3]), (db)parse_u32(argv[4]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13c95")) {
        db *channel = mem + BUF_OFF;
        IplayRegs3Result result;
        if (argc != 5) return 2;
        memset(channel, 0, 0x10);
        channel[0x08] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[4]);
        result = iplay_eff_13c95_result(channel, (db)r.eax, (db)parse_u32(argv[3]), r.eax, r.ecx, r.edx);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13c95")) {
        db *channel = mem + BUF_OFF;
        if (argc != 5) return 2;
        memset(channel, 0, 0x10);
        channel[0x08] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[4]);
        iplay_eff_13c95(&r, channel, (db)parse_u32(argv[3]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13ca2")) {
        db *globals = mem;
        if (argc != 4) return 2;
        memset(globals, 0, 0x200);
        globals[0x00c8] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[3]);
        r.eax = iplay_eff_13ca2_eax((db)r.eax, globals[0x00c8], r.eax);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x00c8, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13ca2")) {
        db *globals = mem;
        if (argc != 4) return 2;
        memset(globals, 0, 0x200);
        globals[0x00c8] = (db)parse_u32(argv[2]);
        r.eax = parse_u32(argv[3]);
        iplay_eff_13ca2(&r, globals, globals[0x00c8]);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x00c8, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13cb3")) {
        db *channel = mem + BUF_OFF;
        dw period;
        if (argc != 7) return 2;
        memset(channel, 0, 0x10);
        period = (dw)parse_u32(argv[2]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x0a] = (db)parse_u32(argv[3]);
        channel[0x0b] = (db)parse_u32(argv[4]);
        r.eax = parse_u32(argv[6]);
        iplay_eff_13cb3_state(channel, (db)r.eax, (db)parse_u32(argv[5]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x0a, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13cb3")) {
        db *channel = mem + BUF_OFF;
        dw period;
        if (argc != 7) return 2;
        memset(channel, 0, 0x10);
        period = (dw)parse_u32(argv[2]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x0a] = (db)parse_u32(argv[3]);
        channel[0x0b] = (db)parse_u32(argv[4]);
        r.eax = parse_u32(argv[6]);
        iplay_eff_13cb3(&r, channel, (db)parse_u32(argv[5]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x0a, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13cc9")) {
        db *globals = mem;
        if (argc != 6) return 2;
        memset(globals, 0, 0x200);
        globals[0x00cc] = (db)parse_u32(argv[4]);
        r.eax = parse_u32(argv[5]);
        r.eax = iplay_eff_13cc9_eax(globals, (db)r.eax, (db)parse_u32(argv[2]), (db)parse_u32(argv[3]), r.eax);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x00cc, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13cc9")) {
        db *globals = mem;
        if (argc != 6) return 2;
        memset(globals, 0, 0x200);
        globals[0x00cc] = (db)parse_u32(argv[4]);
        r.eax = parse_u32(argv[5]);
        iplay_eff_13cc9(&r, globals, (db)parse_u32(argv[2]), (db)parse_u32(argv[3]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x00cc, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13cdd")) {
        db *globals = mem;
        if (argc != 6) return 2;
        memset(globals, 0, 0x200);
        globals[0x00c7] = (db)parse_u32(argv[3]);
        globals[0x00c8] = (db)parse_u32(argv[4]);
        r.eax = parse_u32(argv[5]);
        iplay_eff_13cdd_state(globals, (db)r.eax, (db)parse_u32(argv[2]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x00c7, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13cdd")) {
        db *globals = mem;
        if (argc != 6) return 2;
        memset(globals, 0, 0x200);
        globals[0x00c7] = (db)parse_u32(argv[3]);
        globals[0x00c8] = (db)parse_u32(argv[4]);
        r.eax = parse_u32(argv[5]);
        iplay_eff_13cdd(&r, globals, (db)parse_u32(argv[2]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x00c7, 2);
        printf("\n");
        return 0;
    }

    if (effop(op, "sub13826")) {
        db *channel = mem + BUF_OFF;
        dw table_word;
        if (argc != 4) return 2;
        memset(channel, 0, 0x40);
        table_word = (dw)parse_u32(argv[3]);
        channel[0x14] = 0;
        channel[0x15] = 0;
        r.eax = parse_u32(argv[2]);
        iplay_sub_13826(&r, channel, 1);
        r.eax = (r.eax & 0xffff0000UL) | (dw)(table_word >> ((db)parse_u32(argv[2]) >> 4));
        printf("ax=%04x cx=%04x di=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        return 0;
    }

    if (streq(op, "eff13e1e")) {
        db *channel = mem + BUF_OFF;
        dw period, target, step;
        if (argc != 7) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        target = (dw)parse_u32(argv[3]);
        step = (dw)parse_u32(argv[4]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x10] = (db)target;
        channel[0x11] = (db)(target >> 8);
        channel[0x12] = (db)step;
        channel[0x13] = (db)(step >> 8);
        channel[0x17] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[6]);
        r.eax = iplay_eff_13e1e_eax(channel, (db)r.eax, r.eax);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x10, 4);
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13e1e")) {
        db *channel = mem + BUF_OFF;
        dw period, target, step;
        if (argc != 7) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        target = (dw)parse_u32(argv[3]);
        step = (dw)parse_u32(argv[4]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x10] = (db)target;
        channel[0x11] = (db)(target >> 8);
        channel[0x12] = (db)step;
        channel[0x13] = (db)(step >> 8);
        channel[0x17] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[6]);
        iplay_eff_13e1e(&r, channel);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x10, 4);
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff138d2")) {
        db *channel = mem + BUF_OFF;
        dw period, target, step;
        if (argc != 7) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        target = (dw)parse_u32(argv[3]);
        step = (dw)parse_u32(argv[4]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x10] = (db)target;
        channel[0x11] = (db)(target >> 8);
        channel[0x12] = (db)step;
        channel[0x13] = (db)(step >> 8);
        channel[0x17] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[6]);
        iplay_eff_138d2(&r, channel);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x10, 4);
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13e2d")) {
        db *channel = mem + BUF_OFF;
        dw period;
        if (argc != 8) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x09] = (db)parse_u32(argv[3]);
        channel[0x0c] = (db)parse_u32(argv[4]);
        channel[0x0d] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[7]);
        r.eax = iplay_eff_13e2d_eax(channel, (db)r.eax, (db)parse_u32(argv[6]), r.eax);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x0c, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13e2d")) {
        db *channel = mem + BUF_OFF;
        dw period;
        if (argc != 8) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x09] = (db)parse_u32(argv[3]);
        channel[0x0c] = (db)parse_u32(argv[4]);
        channel[0x0d] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[7]);
        iplay_eff_13e2d(&r, channel, (db)parse_u32(argv[6]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x0c, 2);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff1392f")) {
        db *channel = mem + BUF_OFF;
        dw period;
        if (argc != 8) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x09] = (db)parse_u32(argv[3]);
        channel[0x0c] = (db)parse_u32(argv[4]);
        channel[0x0d] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[7]);
        iplay_eff_1392f(&r, channel, 0);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x0c, 2);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff139ac")) {
        db *channel = mem + BUF_OFF;
        dw period, target, step;
        if (argc != 9) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        target = (dw)parse_u32(argv[3]);
        step = (dw)parse_u32(argv[4]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x10] = (db)target;
        channel[0x11] = (db)(target >> 8);
        channel[0x12] = (db)step;
        channel[0x13] = (db)(step >> 8);
        channel[0x17] = (db)parse_u32(argv[5]);
        channel[0x08] = (db)parse_u32(argv[6]);
        r.eax = parse_u32(argv[8]);
        iplay_eff_139ac(&r, channel, (db)parse_u32(argv[7]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x10, 4);
        print_bytes(channel + 0x17, 1);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff139b2")) {
        db *channel = mem + BUF_OFF;
        dw period;
        if (argc != 10) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x09] = (db)parse_u32(argv[3]);
        channel[0x0c] = (db)parse_u32(argv[4]);
        channel[0x0d] = (db)parse_u32(argv[5]);
        channel[0x08] = (db)parse_u32(argv[7]);
        r.eax = parse_u32(argv[9]);
        iplay_eff_139b2(&r, channel, (db)parse_u32(argv[8]), (db)parse_u32(argv[6]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x0c, 2);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff139b9")) {
        db *channel = mem + BUF_OFF;
        if (argc != 8) return 2;
        memset(channel, 0, 0x40);
        channel[0x08] = (db)parse_u32(argv[2]);
        channel[0x09] = (db)parse_u32(argv[3]);
        channel[0x0e] = (db)parse_u32(argv[4]);
        channel[0x0f] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[7]);
        iplay_eff_139b9(&r, channel, (db)parse_u32(argv[6]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x0e, 2);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff13e7f")) {
        db *channel = mem + BUF_OFF;
        dw period, target, step;
        if (argc != 11) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        target = (dw)parse_u32(argv[3]);
        step = (dw)parse_u32(argv[4]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x10] = (db)target;
        channel[0x11] = (db)(target >> 8);
        channel[0x12] = (db)step;
        channel[0x13] = (db)(step >> 8);
        channel[0x17] = (db)parse_u32(argv[5]);
        channel[0x08] = (db)parse_u32(argv[6]);
        channel[0x34] = (db)parse_u32(argv[9]);
        r.eax = parse_u32(argv[10]);
        iplay_eff_13e7f(&r, channel, (db)parse_u32(argv[7]), (db)parse_u32(argv[8]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x10, 4);
        print_bytes(channel + 0x17, 1);
        print_bytes(channel + 0x34, 1);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff13e84")) {
        db *channel = mem + BUF_OFF;
        dw period;
        if (argc != 12) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x09] = (db)parse_u32(argv[3]);
        channel[0x0c] = (db)parse_u32(argv[4]);
        channel[0x0d] = (db)parse_u32(argv[5]);
        channel[0x08] = (db)parse_u32(argv[7]);
        channel[0x34] = (db)parse_u32(argv[10]);
        r.eax = parse_u32(argv[11]);
        iplay_eff_13e84(&r, channel, (db)parse_u32(argv[8]), (db)parse_u32(argv[9]), (db)parse_u32(argv[6]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x0c, 2);
        print_bytes(channel + 0x34, 1);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff13fbe")) {
        db *channel = mem + BUF_OFF;
        dw period;
        if (argc != 8) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x0b] = (db)parse_u32(argv[3]);
        channel[0x34] = (db)parse_u32(argv[5]);
        channel[0x35] = (db)parse_u32(argv[6]);
        r.eax = parse_u32(argv[7]);
        iplay_eff_13fbe(&r, channel, (db)parse_u32(argv[4]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x0b, 1);
        print_bytes(channel + 0x34, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13de5") || streq(op, "eff13def")) {
        db *channel = mem + BUF_OFF;
        IplayRegs3Result result;
        dw period;
        if (argc != 6) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[2]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x34] = (db)parse_u32(argv[4]);
        r.eax = parse_u32(argv[5]);
        if (streq(op, "eff13de5")) result = iplay_eff_13de5_result(channel, (db)r.eax, (db)parse_u32(argv[3]), r.eax, r.ecx, r.edx);
        else result = iplay_eff_13def_result(channel, (db)r.eax, (db)parse_u32(argv[3]), r.eax, r.ecx, r.edx);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x34, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13de")) {
        db *channel = mem + BUF_OFF;
        dw period;
        if (argc != 7) return 2;
        memset(channel, 0, 0x40);
        period = (dw)parse_u32(argv[3]);
        channel[0] = (db)period;
        channel[1] = (db)(period >> 8);
        channel[0x34] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[6]);
        if (streq(argv[2], "eff_13DE5")) iplay_eff_13de5(&r, channel, (db)parse_u32(argv[4]));
        else iplay_eff_13def(&r, channel, (db)parse_u32(argv[4]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        print_bytes(channel + 0x34, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff13e32")) {
        db *channel = mem + BUF_OFF;
        IplayRegs3Result result;
        if (argc != 7) return 2;
        memset(channel, 0, 0x40);
        channel[0x08] = (db)parse_u32(argv[2]);
        channel[0x34] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[6]);
        result = iplay_eff_13e32_result(channel, (db)r.eax, (db)parse_u32(argv[3]), (db)parse_u32(argv[4]), r.eax, r.ecx, r.edx);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x34, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13e32")) {
        db *channel = mem + BUF_OFF;
        if (argc != 7) return 2;
        memset(channel, 0, 0x40);
        channel[0x08] = (db)parse_u32(argv[2]);
        channel[0x34] = (db)parse_u32(argv[5]);
        r.eax = parse_u32(argv[6]);
        iplay_eff_13e32(&r, channel, (db)parse_u32(argv[3]), (db)parse_u32(argv[4]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x34, 1);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff13e8c")) {
        db *globals = mem;
        db data[11];
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        globals[0x0089] = 0x20;
        r.eax = parse_u32(argv[2]);
        iplay_eff_13e8c(&r, globals, (dw)parse_u32(argv[3]), (dw)parse_u32(argv[4]));
        memcpy(data, globals + 0x004a, 6);
        memcpy(data + 6, globals + 0x0088, 2);
        memcpy(data + 8, globals + 0x00c6, 3);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(data, sizeof(data));
        printf("\n");
        return 0;
    }

    if (effop(op, "eff13f05")) {
        db *channel = mem + BUF_OFF;
        if (argc != 6) return 2;
        memset(channel, 0, 0x40);
        channel[0x08] = (db)parse_u32(argv[2]);
        channel[0x34] = (db)parse_u32(argv[4]);
        r.eax = parse_u32(argv[5]);
        iplay_eff_13f05(&r, channel, (db)parse_u32(argv[3]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x34, 1);
        printf("\n");
        return 0;
    }

    if (effop(op, "eff13f3b")) {
        db *channel = mem + BUF_OFF;
        if (argc != 8) return 2;
        memset(channel, 0, 0x40);
        channel[0x08] = (db)parse_u32(argv[2]);
        channel[0x3d] = (db)parse_u32(argv[5]);
        channel[0x34] = (db)parse_u32(argv[6]);
        r.eax = parse_u32(argv[7]);
        iplay_eff_13f3b(&r, channel, (db)parse_u32(argv[3]), (db)parse_u32(argv[4]));
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel + 0x08, 1);
        print_bytes(channel + 0x34, 1);
        print_bytes(channel + 0x3d, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff14020") || streq(op, "changeamplif") || streq(op, "abiamplif")) {
        db *globals = mem;
        int abi = streq(op, "abiamplif");
        int arg = abi ? 3 : 2;
        const char *symbol = abi ? argv[2] : op;
        if (argc != (abi ? 6 : 5)) return 2;
        memset(globals, 0, 0x200);
        globals[0x005e] = 100;
        globals[0x005f] = 0;
        globals[0x00de] = (db)parse_u32(argv[arg + 1]);
        globals[0x0036] = (db)parse_u32(argv[arg + 2]);
        globals[0x0037] = (db)(parse_u32(argv[arg + 2]) >> 8);
        r.eax = parse_u32(argv[arg]);
        if (streq(symbol, "eff_14020") || streq(symbol, "eff14020")) r.eax = iplay_eff_14020_eax(globals, globals[0x00de], r.eax);
        else r.eax = iplay_change_amplif_eax(globals, globals[0x00de], r.eax);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x005e, 2);
        print_bytes(globals + 0x0085, 1);
        print_bytes(globals + 0x00dd, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff1387f") || streq(op, "eff13886") || streq(op, "eff1389d") || streq(op, "eff138a4") || streq(op, "abieffslide")) {
        db *channel = mem + BUF_OFF;
        dw initial;
        db active;
        int abi = streq(op, "abieffslide");
        int arg = abi ? 3 : 2;
        const char *symbol = abi ? argv[2] : op;
        if (argc != (abi ? 5 : 4) && argc != (abi ? 6 : 5)) return 2;
        memset(channel, 0, 0x40);
        initial = (dw)parse_u32(argv[arg]);
        channel[0] = (db)initial;
        channel[1] = (db)(initial >> 8);
        r.eax = parse_u32(argv[arg + 1]);
        active = argc == arg + 3 ? (db)parse_u32(argv[arg + 2]) : 0;
        if (streq(symbol, "eff1387f") || streq(symbol, "eff_1387F")) r.eax = iplay_eff_1387f_eax(channel, (db)r.eax, active, r.eax);
        else if (streq(symbol, "eff13886") || streq(symbol, "eff_13886")) r.eax = iplay_eff_13886_eax(channel, (db)r.eax, r.eax);
        else if (streq(symbol, "eff1389d") || streq(symbol, "eff_1389D")) r.eax = iplay_eff_1389d_eax(channel, (db)r.eax, active, r.eax);
        else r.eax = iplay_eff_138a4_eax(channel, (db)r.eax, r.eax);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(channel, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub14087")) {
        db *channel = mem + BUF_OFF;
        if (argc != 5) return 2;
        memset(channel, 0, 0x40);
        r.eax = parse_u32(argv[2]);
        r.edx = 0x0100;
        channel[0x34] = (db)parse_u32(argv[3]);
        iplay_sub_14087(&r, channel, (db)parse_u32(argv[4]));
        printf("ax=%04x dx=%04x data=", (unsigned)(r.eax & 0xffffu), (unsigned)(r.edx & 0xffffu));
        print_bytes(channel + 0x34, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "calc14043")) {
        db data[2];
        dw ax;
        if (argc != 4) return 2;
        data[0] = (db)parse_u32(argv[2]);
        data[1] = (db)parse_u32(argv[3]);
        ax = iplay_calc_14043_ax(data[0], data[1]);
        printf("ax=%04x data=", (unsigned)ax);
        print_bytes(data, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abicalc14043")) {
        db data[2];
        if (argc != 4) return 2;
        data[0] = (db)parse_u32(argv[2]);
        data[1] = (db)parse_u32(argv[3]);
        iplay_calc_14043(&r, data[0], data[1]);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(data, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub13d95")) {
        db *globals = mem;
        IplayRegs3Result result;
        if (argc != 3) return 2;
        memset(globals, 0, 0x200);
        result = iplay_sub_13d95_result(globals, 0, parse_u32(argv[2]), 0);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(globals + 0x0078, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub13d95")) {
        db *globals = mem;
        if (argc != 3) return 2;
        memset(globals, 0, 0x200);
        r.ecx = parse_u32(argv[2]);
        iplay_sub_13d95(&r, globals);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x0078, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub13044")) {
        db *globals = mem;
        db *vlm_table = mem + 0x3d68;
        if (argc != 6) return 2;
        memset(globals, 0, RUNNER_MEM_SIZE);
        globals[0x00de] = (db)parse_u32(argv[2]);
        globals[0x0036] = (db)parse_u32(argv[3]);
        globals[0x0037] = (db)(parse_u32(argv[3]) >> 8);
        globals[0x005e] = (db)parse_u32(argv[4]);
        globals[0x005f] = (db)(parse_u32(argv[4]) >> 8);
        globals[0x0085] = (db)parse_u32(argv[5]);
        iplay_sub_13044(globals, vlm_table);
        printf("data=");
        print_bytes(globals + 0x008e, 2);
        print_bytes(globals + 0x00b6, 2);
        print_bytes(globals + 0x00dd, 2);
        print_bytes(vlm_table, 32);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub13017bounded") || streq(op, "configuretimerbounded")) {
        db *globals = mem;
        db *samples = mem + 0x1d68;
        memset(globals, 0, RUNNER_MEM_SIZE);
        globals[0x0032] = 2;
        samples[0x24] = 0xaa;
        samples[0x25] = 0xaa;
        samples[0x26] = 0xaa;
        samples[0x27] = 0xaa;
        samples[0x2c] = 0x11;
        samples[0x2d] = 0x11;
        samples[0x2e] = 0x11;
        samples[0x2f] = 0x11;
        samples[0x3c] = 0;
        samples[0x40 + 0x24] = 0x22;
        samples[0x40 + 0x25] = 0x22;
        samples[0x40 + 0x26] = 0x22;
        samples[0x40 + 0x27] = 0x22;
        samples[0x40 + 0x2c] = 0x33;
        samples[0x40 + 0x2d] = 0x33;
        samples[0x40 + 0x2e] = 0x33;
        samples[0x40 + 0x2f] = 0x33;
        samples[0x40 + 0x3c] = 8;
        if (streq(op, "configuretimerbounded")) {
            (void)iplay_configure_timer_bounded_result(globals, samples, 2, 0, 0, 0, 0, 0, 0);
        } else {
            iplay_sub_13017_bounded(globals, samples, 2);
        }
        printf("data=");
        print_bytes(samples + 0x24, 4);
        print_bytes(samples + 0x40 + 0x24, 4);
        print_bytes(globals + 0x0060, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub13cf6")) {
        db *globals = mem;
        db data[9];
        IplayRegs3Result result;
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        result = iplay_sub_13cf6_result(globals, (db)parse_u32(argv[2]), (dw)parse_u32(argv[3]), (dw)parse_u32(argv[4]), parse_u32(argv[2]), 0, 0);
        data[0] = globals[0x00c6];
        memcpy(data + 1, globals + 0x004a, 6);
        memcpy(data + 7, globals + 0x0044, 2);
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(data, sizeof(data));
        printf("\n");
        return 0;
    }

    if (streq(op, "abisub13cf6")) {
        db *globals = mem;
        db data[9];
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        r.eax = parse_u32(argv[2]);
        iplay_sub_13cf6(&r, globals, (dw)parse_u32(argv[3]), (dw)parse_u32(argv[4]));
        data[0] = globals[0x00c6];
        memcpy(data + 1, globals + 0x004a, 6);
        memcpy(data + 7, globals + 0x0044, 2);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(data, sizeof(data));
        printf("\n");
        return 0;
    }

    if (streq(op, "sub13e9b")) {
        if (argc != 3) return 2;
        r.eax = parse_u32(argv[2]);
        iplay_sub_13e9b_public(&r);
        printf("ax=%04x dx=%04x di=%04x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.edi & 0xffffu));
        return 0;
    }

    if (streq(op, "eff13ce8")) {
        db *globals = mem;
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        globals[0x00c7] = (db)parse_u32(argv[2]);
        globals[0x00c8] = (db)parse_u32(argv[3]);
        r.eax = parse_u32(argv[4]);
        iplay_eff_13ce8_state(globals, (db)r.eax);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x00c7, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff13ce8")) {
        db *globals = mem;
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        globals[0x00c7] = (db)parse_u32(argv[2]);
        globals[0x00c8] = (db)parse_u32(argv[3]);
        r.eax = parse_u32(argv[4]);
        iplay_eff_13ce8(&r, globals);
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(globals + 0x00c7, 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "eff14030")) {
        db *globals = mem;
        db data[11];
        IplayRegs3Result result;
        if (argc != 6) return 2;
        memset(globals, 0, 0x200);
        globals[0x0089] = 0x20;
        result = iplay_eff_14030_result(globals, (db)parse_u32(argv[2]), (db)parse_u32(argv[3]), (dw)parse_u32(argv[4]), (dw)parse_u32(argv[5]), parse_u32(argv[2]), 0, 0);
        data[0] = globals[0x00db];
        data[1] = globals[0x00dc];
        memcpy(data + 2, globals + 0x004a, 6);
        memcpy(data + 8, globals + 0x0088, 2);
        data[10] = globals[0x00c6];
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(data, sizeof(data));
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff14030")) {
        db *globals = mem;
        db data[11];
        if (argc != 6) return 2;
        memset(globals, 0, 0x200);
        globals[0x0089] = 0x20;
        r.eax = parse_u32(argv[2]);
        iplay_eff_14030(&r, globals, (db)parse_u32(argv[3]), (dw)parse_u32(argv[4]), (dw)parse_u32(argv[5]));
        data[0] = globals[0x00db];
        data[1] = globals[0x00dc];
        memcpy(data + 2, globals + 0x004a, 6);
        memcpy(data + 8, globals + 0x0088, 2);
        data[10] = globals[0x00c6];
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(data, sizeof(data));
        printf("\n");
        return 0;
    }

    if (streq(op, "eff14067")) {
        db *globals = mem;
        db data[11];
        IplayRegs3Result result;
        if (argc != 7) return 2;
        memset(globals, 0, 0x200);
        globals[0x0089] = 0x20;
        result = iplay_eff_14067_result(globals, (db)parse_u32(argv[2]), (db)parse_u32(argv[3]), (db)parse_u32(argv[4]), (dw)parse_u32(argv[5]), (dw)parse_u32(argv[6]), parse_u32(argv[2]), 0, 0);
        data[0] = globals[0x00db];
        data[1] = globals[0x00dc];
        memcpy(data + 2, globals + 0x004a, 6);
        memcpy(data + 8, globals + 0x0088, 2);
        data[10] = globals[0x00c6];
        printf("ax=%04x data=", (unsigned)(result.eax & 0xffffu));
        print_bytes(data, sizeof(data));
        printf("\n");
        return 0;
    }

    if (streq(op, "abieff14067")) {
        db *globals = mem;
        db data[11];
        if (argc != 7) return 2;
        memset(globals, 0, 0x200);
        globals[0x0089] = 0x20;
        r.eax = parse_u32(argv[2]);
        iplay_eff_14067(&r, globals, (db)parse_u32(argv[3]), (db)parse_u32(argv[4]), (dw)parse_u32(argv[5]), (dw)parse_u32(argv[6]));
        data[0] = globals[0x00db];
        data[1] = globals[0x00dc];
        memcpy(data + 2, globals + 0x004a, 6);
        memcpy(data + 8, globals + 0x0088, 2);
        data[10] = globals[0x00c6];
        printf("ax=%04x data=", (unsigned)(r.eax & 0xffffu));
        print_bytes(data, sizeof(data));
        printf("\n");
        return 0;
    }

    if (streq(op, "midi154da")) {
        db *channel = mem + BUF_OFF;
        if (argc != 3) return 2;
        memset(channel, 0, 0x40);
        channel[0x18] = (db)parse_u32(argv[2]);
        iplay_midi_154da(&r, channel);
        printf("ax=%04x\n", (unsigned)(r.eax & 0xffffu));
        return 0;
    }

    if (streq(op, "midi154de")) {
        db *channel = mem + BUF_OFF;
        if (argc != 3) return 2;
        memset(channel, 0, 0x40);
        channel[0x35] = (db)parse_u32(argv[2]);
        iplay_midi_154de(&r, channel);
        printf("ax=%04x dx=%04x\n", (unsigned)(r.eax & 0xffffu), (unsigned)(r.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "midi154ac")) {
        db *channel = mem + BUF_OFF;
        if (argc != 5) return 2;
        memset(channel, 0, 0x40);
        channel[0x1b] = (db)parse_u32(argv[4]);
        r.eax = parse_u32(argv[2]);
        r.edi = 0x1000;
        iplay_midi_154ac(&r, channel, (db)parse_u32(argv[3]));
        printf("ax=%04x di=%04x data=", (unsigned)(r.eax & 0xffffu), (unsigned)(r.edi & 0xffffu));
        mem[DST_OFF] = '.';
        print_bytes(mem + DST_OFF, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "midi15413guard")) {
        db last_status;
        if (argc != 3) return 2;
        last_status = (db)parse_u32(argv[2]);
        r.eax = (((dw)last_status) << 8) | 0x34u;
        r.edx = 0x5678;
        iplay_midi_15413_guard(&r, &last_status);
        printf("ax=%04x dx=%04x data=%02x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)last_status);
        return 0;
    }

    if (streq(op, "sub15577guard")) {
        db *channel = mem + BUF_OFF;
        memset(channel, 0, 0x50);
        channel[0x17] = 0;
        r.eax = 0x1234;
        r.ebx = 0x5678;
        r.ecx = 0x9abc;
        r.edx = 0xdef0;
        r.esi = 0x9000;
        r.edi = 0x2468;
        iplay_sub_15577_disabled(&r, channel);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x data=%02x\n",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.edx & 0xffffu),
               (unsigned)(r.esi & 0xffffu),
               (unsigned)(r.edi & 0xffffu),
               (unsigned)channel[0x17]);
        return 0;
    }

#ifndef __WATCOMC__
    if (streq(op, "sub15577mix")) {
        db *channel = mem + BUF_OFF;
        db *samples = mem + 0x400u;
        db *mix = mem + DSEG_SCRATCH;
        int16_t volume_table[256];
        dw frames;
        dw period;
        dd position;
        dd sample_end;
        dd loop_start;
        dd loop_length;
        unsigned i;
        if (argc != 11) return 2;
        frames = (dw)parse_u32(argv[4]);
        period = (dw)parse_u32(argv[5]);
        position = (dd)parse_u32(argv[6]);
        sample_end = (dd)parse_u32(argv[7]);
        loop_start = (dd)parse_u32(argv[8]);
        loop_length = (dd)parse_u32(argv[9]);
        if (frames > 32u) return 2;
        memset(channel, 0, 0x50);
        memset(mix, 0, frames * 8u);
        for (i = 0; i < 256u; ++i) volume_table[i] = (int16_t)((int)i * 257 - 1000);
        for (i = 0; i < 80u; ++i) samples[i] = (db)(i * 2u + 1u);
        channel[0x17] = 1;
        channel[0x19] = parse_u32(argv[10]) != 0 ? 8 : 0;
        channel[0x04] = (db)position;
        channel[0x05] = (db)(position >> 8);
        channel[0x06] = (db)(position >> 16);
        channel[0x07] = (db)(position >> 24);
        channel[0x20] = (db)period;
        channel[0x21] = (db)(period >> 8);
        channel[0x35] = 0x77;
        channel[0x40] = (db)loop_start;
        channel[0x41] = (db)(loop_start >> 8);
        channel[0x42] = (db)(loop_start >> 16);
        channel[0x43] = (db)(loop_start >> 24);
        channel[0x44] = (db)loop_length;
        channel[0x45] = (db)(loop_length >> 8);
        channel[0x46] = (db)(loop_length >> 16);
        channel[0x47] = (db)(loop_length >> 24);
        channel[0x48] = (db)sample_end;
        channel[0x49] = (db)(sample_end >> 8);
        channel[0x4a] = (db)(sample_end >> 16);
        channel[0x4b] = (db)(sample_end >> 24);
        iplay_mix_channel_8bit(
            channel,
            samples,
            volume_table,
            mix,
            frames,
            parse_u32(argv[2]) != 0,
            parse_u32(argv[3]) != 0);
        printf("mix=");
        print_bytes(mix, frames * 8u);
        printf(" state=");
        print_bytes(channel + 0x04, 4);
        print_bytes(channel + 0x17, 3);
        print_bytes(channel + 0x20, 4);
        print_bytes(channel + 0x35, 3);
        printf("\n");
        return 0;
    }
#endif

    if (streq(op, "sub154f4")) {
        db *globals = mem;
        db *channel = mem + DSEG_SCRATCH;
        dd sample_ptr;
        dw period;
        dw seg_base;
        dw interp_word;
        if (argc != 9) return 2;
        memset(globals, 0, 0x200);
        memset(channel, 0, 0x40);
        globals[0x0044] = (db)parse_u32(argv[2]);
        globals[0x0045] = (db)(parse_u32(argv[2]) >> 8);
        globals[0x00d2] = (db)parse_u32(argv[3]);
        sample_ptr = (dd)parse_u32(argv[4]);
        channel[0x04] = (db)sample_ptr;
        channel[0x05] = (db)(sample_ptr >> 8);
        channel[0x06] = (db)(sample_ptr >> 16);
        channel[0x07] = (db)(sample_ptr >> 24);
        period = (dw)parse_u32(argv[5]);
        channel[0x20] = (db)period;
        channel[0x21] = (db)(period >> 8);
        channel[0x23] = (db)parse_u32(argv[6]);
        seg_base = (dw)parse_u32(argv[7]);
        channel[0x24] = (db)seg_base;
        channel[0x25] = (db)(seg_base >> 8);
        channel[0x26] = 0xff;
        channel[0x27] = 0xff;
        interp_word = (dw)parse_u32(argv[8]);
        channel[0x36] = (db)interp_word;
        channel[0x37] = (db)(interp_word >> 8);
        r.esi = DSEG_SCRATCH;
        iplay_sub_154f4(&r, globals, channel);
        printf("bx=%04x cx=%04x si=%04x data=",
               (unsigned)(r.ebx & 0xffffu),
               (unsigned)(r.ecx & 0xffffu),
               (unsigned)(r.esi & 0xffffu));
        print_bytes(globals + 0x0074, 3);
        print_bytes(globals + 0x00e3, 1);
        printf("\n");
        return 0;
    }

    if (streq(op, "sub1609fdisabled")) {
        db *dst = mem + DSEG_SCRATCH;
        dw buffer_size;
        IplayRegs6Result result;
        if (argc != 3) return 2;
        buffer_size = (dw)parse_u32(argv[2]);
        memset(dst, 0xa5, buffer_size * 8u);
        result = iplay_sub_1609f_disabled_result(dst, buffer_size, 0, 0, 0, 0, 0x9000, DSEG_SCRATCH);
        printf("ax=%04x bx=%04x cx=%04x si=%04x di=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.esi & 0xffffu),
               (unsigned)(result.edi & 0xffffu));
        print_bytes(dst, buffer_size * 8u);
        printf("\n");
        return 0;
    }

    if (streq(op, "volumeprepinactive")) {
        db *globals = mem;
        db *dst = mem + DSEG_SCRATCH + 0x100;
        dw word_24610;
        dw size;
        IplayRegs6Result result;
        if (argc != 4) return 2;
        memset(globals, 0, 0x200);
        word_24610 = (dw)parse_u32(argv[2]);
        size = (dw)parse_u32(argv[3]);
        memset(dst, 0xa5, size);
        result = iplay_volume_prep_inactive_result(globals, dst, word_24610, size, word_24610, 0, size, 0, 0, DSEG_SCRATCH + 0x100);
        result.edi = DSEG_SCRATCH + 0x200 + 12;
        printf("cx=%04x di=%04x data=",
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edi & 0xffffu));
        print_bytes(globals + 0x0070, 4);
        print_bytes(dst, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "sbhelper") || streq(op, "abireadsb") || streq(op, "abireadmixersb") || streq(op, "abiwritesb") || streq(op, "abiwritemixersb") || streq(op, "abichecksb")) {
        const char *symbol;
        dw base_port;
        dd ax;
        IplaySb16RegsResult result;
        if (streq(op, "sbhelper")) {
            if (argc != 5) return 2;
            symbol = argv[2];
            base_port = (dw)parse_u32(argv[3]);
            ax = parse_u32(argv[4]);
        } else {
            if (argc != 2) return 2;
            if (streq(op, "abireadsb")) {
                symbol = "ReadSB";
                base_port = 0x240;
                ax = 0xbeef;
            } else if (streq(op, "abireadmixersb")) {
                symbol = "ReadMixerSB";
                base_port = 0x220;
                ax = 0x5634;
            } else if (streq(op, "abiwritesb")) {
                symbol = "WriteSB";
                base_port = 0x240;
                ax = 0x00d1;
            } else if (streq(op, "abiwritemixersb")) {
                symbol = "WriteMixerSB";
                base_port = 0x220;
                ax = 0x1234;
            } else {
                symbol = "CheckSB";
                base_port = 0x220;
                ax = 0x7777;
            }
        }
        result = iplay_sb_helper_no_device_result(symbol, base_port, ax, 0x1357, 0x2468, 0x369a);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu));
        return 0;
    }

    if (effop(op, "setdmamask")) {
        dw channel;
        IplaySb16RegsResult result;
        if (argc != 3) return 2;
        channel = (dw)parse_u32(argv[2]);
        result = iplay_set_dmachn_mask_no_device_result(channel, 0x1234, 0x5678, channel, 0x9abc);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "adlibdelay")) {
        const char *symbol;
        IplaySb16RegsResult result;
        if (argc != 4) return 2;
        symbol = argv[2];
        result = iplay_adlib_delay_no_device_result(symbol, parse_u32(argv[3]), 0x5678, 0x9abc, 0xdef0);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu));
        return 0;
    }

    if (effop(op, "sbinitnodevice")) {
        db *globals = mem;
        const char *symbol;
        IplayRegs6Result result;
        if (argc != 3) return 2;
        symbol = argv[2];
        memset(globals, 0, 0x200);
        result = iplay_sb_legacy_init_no_device_result(globals, streq(symbol, "sbpro_init"), 0, 0, 0, 0, 0, 0);
        printf("ax=%04x dx=%04x flags=7217 data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.edx & 0xffffu));
        print_bytes(globals + 0x0082, 3);
        printf("\n");
        return 0;
    }

    if (streq(op, "sbdetectirqnodevice") || streq(op, "abisbdetectirq")) {
        IplaySb16RegsResult result = iplay_sb_detect_irq_no_device_result(0x1234, 0x5678, 0x9abc, 0xdef0);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x flags=7217\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu));
        return 0;
    }

    if (streq(op, "sbtestinterruptnodevice") || streq(op, "abisbtestinterruptnodevice")) {
        db counter = 0xaa;
        IplayRegs6Result result = iplay_sb_test_interrupt_no_device_result(&counter, 0x1234, 0x5678, 0x9abc, 0xdef0, 0, 0);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x si=%04x flags=7247 data=%02x\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               (unsigned)(result.esi & 0xffffu),
               (unsigned)counter);
        return 0;
    }

    if (effop(op, "sbonbounded")) {
        db *globals = mem;
        const char *symbol;
        if (argc != 3) return 2;
        symbol = argv[2];
        memset(globals, 0, 0x200);
        globals[0x00be] = 0x22;
        globals[0x00bf] = 0x56;
        globals[0x00b2] = 0x20;
        globals[0x00b3] = 0x02;
        globals[0x00b9] = 7;
        globals[0x00ba] = 0x55;
        globals[0x00b8] = 1;
        globals[0x0083] = streq(symbol, "sb16_on") ? 1 : 0;
        globals[0x0084] = 8;
        iplay_sb_on_bounded(globals, symbol);
        printf("data=");
        print_bytes(globals + 0x006e, 2);
        print_bytes(globals + 0x00ce, 2);
        printf("\n");
        return 0;
    }

    if (effop(op, "sbhandlerintbounded")) {
        db *globals = mem;
        memset(globals, 0, 0x200);
        globals[0x006e] = 0x00;
        globals[0x006f] = 0x10;
        r.eax = 0x1234;
        r.edx = 0x022e;
        iplay_sb_handler_int_bounded_state(globals);
        printf("ax=%04x dx=%04x data=",
               (unsigned)(r.eax & 0xffffu),
               (unsigned)(r.edx & 0xffffu));
        print_bytes(globals + 0x006e, 2);
        printf("\n");
        return 0;
    }

    if (effop(op, "sub19050bounded")) {
        db *globals = mem;
        IplaySb16RegsResult result;
        memset(globals, 0, 0x2000);
        globals[0x167e] = 7;
        result = iplay_sub_19050_bounded_result(0, 0, 0, 0);
        printf("ax=%04x dx=%04x data=%02x\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               (unsigned)globals[0x167e]);
        return 0;
    }

    if (effop(op, "memfill8080")) {
        db *dma = mem + 0x3000;
        IplayRegs6Result result;
        memset(dma, 0xa5, 16);
        result = iplay_memfill8080_result(dma, 0x12345678UL, 0x9abcdef0UL, 0x1357, 0x2468, 0, 0x1000);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               (unsigned)(result.edi & 0xffffu));
        print_bytes(dma, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "sndofffill")) {
        db *dma = mem + 0x3000;
        const char *symbol;
        IplayRegs6Result result;
        if (argc != 3) return 2;
        symbol = argv[2];
        memset(dma, 0xa5, 16);
        result = iplay_sndoff_fill_result(dma, symbol, 0x12345678UL, 0x9abcdef0UL, 0x1357, 0x2468, 0, 0x1000);
        printf("ax=%04x bx=%04x cx=%04x dx=%04x di=%04x data=",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               (unsigned)(result.edi & 0xffffu));
        print_bytes(dma, 16);
        printf("\n");
        return 0;
    }

    if (streq(op, "audioinitfail")) {
        db *globals = mem;
        db text[16];
        db ok;
        if (argc != 3) return 2;
        memset(globals, 0, 0x200);
        memset(text, 0, sizeof(text));
        ok = iplay_audio_init_failure(globals, text, argv[2]);
        printf("data=%02x\n", (unsigned)ok);
        return 0;
    }

    if (streq(op, "getplaysettings")) {
        dd eax;
        if (argc != 3) return 2;
        eax = iplay_get_playsettings_eax(0, (db)parse_u32(argv[2]));
        printf("ax=%04x\n", (unsigned)(eax & 0xffffu));
        return 0;
    }

    if (streq(op, "getsetplaystate")) {
        dd eax;
        if (argc != 4) return 2;
        eax = iplay_getset_playstate_eax(parse_u32(argv[3]), (db)parse_u32(argv[2]));
        printf("ax=%04x\n", (unsigned)(eax & 0xffffu));
        return 0;
    }

    if (streq(op, "get12f7c")) {
        IplaySb16RegsResult result;
        if (argc != 4) return 2;
        result = iplay_get_12f7c_result((dw)parse_u32(argv[2]), (dw)parse_u32(argv[3]), 0, 0, 0, 0);
        printf("ax=%04x bx=%04x\n", (unsigned)(result.eax & 0xffffu), (unsigned)(result.ebx & 0xffffu));
        return 0;
    }

    if (streq(op, "readsndsettings")) {
        IplaySndSettingsResult result;
        if (argc != 13) return 2;
        result = iplay_read_sndsettings_result(
            0, 0, 0, 0, 0, 0,
            (db)parse_u32(argv[2]),
            (dw)parse_u32(argv[3]),
            (db)parse_u32(argv[4]),
            (db)parse_u32(argv[5]),
            (db)parse_u32(argv[6]),
            (db)parse_u32(argv[7]),
            (db)parse_u32(argv[8]),
            (dw)parse_u32(argv[9]),
            (dw)parse_u32(argv[10]),
            (dw)parse_u32(argv[11]),
            (db)parse_u32(argv[12]));
        printf("ax=%04x bx=%04x cx=%04x dx=%04x bp=%04x si=%04x\n",
               (unsigned)(result.eax & 0xffffu),
               (unsigned)(result.ebx & 0xffffu),
               (unsigned)(result.ecx & 0xffffu),
               (unsigned)(result.edx & 0xffffu),
               (unsigned)(result.ebp & 0xffffu),
               (unsigned)(result.esi & 0xffffu));
        return 0;
    }

    if (streq(op, "sndinit") || streq(op, "sndon") || streq(op, "sndoff") || streq(op, "snddeinit") || streq(op, "sndoffx")) {
        db *globals = mem;
        unsigned snd_op = streq(op, "sndinit") ? 0 : streq(op, "sndon") ? 1 : streq(op, "sndoff") ? 2 : streq(op, "snddeinit") ? 3 : 4;
        if (argc != 5) return 2;
        memset(globals, 0, 0x200);
        globals[0x00e0] = (db)parse_u32(argv[2]);
        globals[0x00e1] = (db)parse_u32(argv[3]);
        globals[0x010c] = (db)parse_u32(argv[4]);
        iplay_snd_guard_state(globals, snd_op);
        printf("ax=%04x data=", 0x156a);
        print_bytes(globals + 0x00e0, 3);
        printf("\n");
        return 0;
    }

    if (streq(op, "audiosb16sink")) {
        IplayAudioSink sink;
        AudioCapture capture;
        static const db pcm[10] = {1,2,3,4,5,6,7,8,9,10};
        memset(&capture, 0, sizeof(capture));
        iplay_audio_sink_init(&sink, &IPLAY_AUDIO_SB16_STEREO_16, capture_audio_write, &capture);
        iplay_audio_sink_start(&sink);
        iplay_audio_sink_write(&sink, pcm, sizeof(pcm));
        printf("ok=%u framebytes=%u frames=%lu bytes=%u data=",
               (unsigned)iplay_audio_format_is_sb16_stereo_16(&sink.format),
               (unsigned)iplay_audio_bytes_per_frame(&sink.format),
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "audiosb16silence")) {
        IplayAudioSink sink;
        AudioCapture capture;
        memset(&capture, 0xa5, sizeof(capture));
        iplay_audio_sink_init(&sink, &IPLAY_AUDIO_SB16_STEREO_16, capture_audio_write, &capture);
        iplay_audio_sink_start(&sink);
        iplay_audio_sink_write_silence(&sink, 3);
        printf("frames=%lu underrun=%lu bytes=%u data=",
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned long)iplay_audio_sink_underrun_frames(&sink),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "audiou8tos16stereo")) {
        db dst[16];
        static const db mono[3] = {0x00, 0x80, 0xff};
        static const db stereo[4] = {0x00, 0xff, 0x80, 0x40};
        dw mono_bytes;
        dw stereo_bytes;
        if (argc != 2) return 2;
        memset(dst, 0xa5, sizeof(dst));
        mono_bytes = iplay_audio_u8_to_s16_stereo(mono, 3, 1, dst, 12);
        printf("mono_bytes=%u mono=", (unsigned)mono_bytes);
        print_bytes(dst, 12);
        memset(dst, 0xa5, sizeof(dst));
        stereo_bytes = iplay_audio_u8_to_s16_stereo(stereo, 2, 2, dst, 8);
        printf(" stereo_bytes=%u stereo=", (unsigned)stereo_bytes);
        print_bytes(dst, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "audios16tos16stereo")) {
        db dst[16];
        static const db mono[4] = {0x34,0x12,0x00,0x80};
        static const db stereo[8] = {0x34,0x12,0x78,0x56,0x00,0x80,0xff,0x7f};
        dw mono_bytes;
        dw stereo_bytes;
        if (argc != 2) return 2;
        memset(dst, 0xa5, sizeof(dst));
        mono_bytes = iplay_audio_s16_to_s16_stereo(mono, 2, 1, dst, 8);
        printf("mono_bytes=%u mono=", (unsigned)mono_bytes);
        print_bytes(dst, 8);
        memset(dst, 0xa5, sizeof(dst));
        stereo_bytes = iplay_audio_s16_to_s16_stereo(stereo, 2, 2, dst, 8);
        printf(" stereo_bytes=%u stereo=", (unsigned)stereo_bytes);
        print_bytes(dst, 8);
        printf("\n");
        return 0;
    }

    if (streq(op, "audioconvert")) {
        db dst[16];
        IplayAudioFormat u8mono = { 44100u, 8u, 1u, 0u };
        IplayAudioFormat s16stereo = { 44100u, 16u, 2u, 1u };
        IplayAudioFormat bad = { 44100u, 8u, 2u, 1u };
        IplayAudioFormat wrong_rate = { 22050u, 8u, 1u, 0u };
        static const db u8[2] = {0x00, 0xff};
        static const db s16[4] = {0x34,0x12,0x00,0x80};
        dw u8_bytes;
        dw s16_bytes;
        dw bad_bytes;
        dw rate_bytes;
        if (argc != 2) return 2;
        memset(dst, 0xa5, sizeof(dst));
        u8_bytes = iplay_audio_convert_to_sink_format(&u8mono, u8, 2, &IPLAY_AUDIO_SB16_STEREO_16, dst, 8);
        printf("u8_bytes=%u u8=", (unsigned)u8_bytes);
        print_bytes(dst, 8);
        memset(dst, 0xa5, sizeof(dst));
        s16_bytes = iplay_audio_convert_to_sink_format(&s16stereo, s16, 1, &IPLAY_AUDIO_SB16_STEREO_16, dst, 4);
        printf(" s16_bytes=%u s16=", (unsigned)s16_bytes);
        print_bytes(dst, 4);
        bad_bytes = iplay_audio_convert_to_sink_format(&bad, u8, 1, &IPLAY_AUDIO_SB16_STEREO_16, dst, 4);
        rate_bytes = iplay_audio_convert_to_sink_format(&wrong_rate, u8, 1, &IPLAY_AUDIO_SB16_STEREO_16, dst, 4);
        printf(" bad_bytes=%u rate_bytes=%u rates_match=%u sink_equals=%u\n",
               (unsigned)bad_bytes,
               (unsigned)rate_bytes,
               (unsigned)iplay_audio_rates_match(&u8mono, &IPLAY_AUDIO_SB16_STEREO_16),
               (unsigned)iplay_audio_format_equals(&s16stereo, &IPLAY_AUDIO_SB16_STEREO_16));
        return 0;
    }

    if (streq(op, "audiosourcefmt")) {
        IplayAudioFormat fmt;
        if (argc != 6) return 2;
        if (!iplay_audio_make_source_format(&fmt, (dw)parse_u32(argv[2]), (db)parse_u32(argv[3]), (db)parse_u32(argv[4]), (db)parse_u32(argv[5]))) {
            printf("ok=0\n");
        } else {
            printf("ok=1 rate=%u bits=%u channels=%u signed=%u framebytes=%u frames10=%u\n",
                   (unsigned)fmt.sample_rate,
                   (unsigned)fmt.bits_per_sample,
                   (unsigned)fmt.channels,
                   (unsigned)fmt.signed_samples,
                   (unsigned)iplay_audio_bytes_per_frame(&fmt),
                   (unsigned)iplay_audio_frames_for_bytes(&fmt, 10));
        }
        return 0;
    }

    if (streq(op, "audiofmtname")) {
        IplayAudioFormat fmt;
        if (argc != 6) return 2;
        if (!iplay_audio_make_source_format(&fmt, (dw)parse_u32(argv[2]), (db)parse_u32(argv[3]), (db)parse_u32(argv[4]), (db)parse_u32(argv[5]))) {
            fmt.sample_rate = (dw)parse_u32(argv[2]);
            fmt.bits_per_sample = (db)parse_u32(argv[3]);
            fmt.channels = (db)parse_u32(argv[4]);
            fmt.signed_samples = (db)parse_u32(argv[5]);
        }
        printf("name=%s rate=%u framebytes=%u\n",
               iplay_audio_format_name(&fmt),
                   (unsigned)fmt.sample_rate,
                   (unsigned)iplay_audio_bytes_per_frame(&fmt));
        return 0;
    }

    if (streq(op, "audiowriteconverted")) {
        IplayAudioSink sink;
        AudioCapture capture;
        IplayAudioFormat u8stereo = { 44100u, 8u, 2u, 0u };
        static const db src[4] = {0x00,0xff,0x80,0x40};
        db scratch[16];
        dw bytes;
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        memset(scratch, 0xa5, sizeof(scratch));
        iplay_audio_sink_init(&sink, &IPLAY_AUDIO_SB16_STEREO_16, capture_audio_write, &capture);
        iplay_audio_sink_start(&sink);
        bytes = iplay_audio_sink_write_converted(&sink, &u8stereo, src, 2, scratch, sizeof(scratch));
        printf("bytes=%u frames=%lu captured=%u data=",
               (unsigned)bytes,
               (unsigned long)sink.frames_written,
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "audiooutputmixer")) {
        IplayAudioOutput output;
        AudioCapture capture;
        IplayAudioFormat u8stereo = { 44100u, 8u, 2u, 0u };
        static const db src[6] = {0x00,0xff,0x80,0x40,0xff,0x00};
        db scratch[16];
        dw bytes;
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        memset(scratch, 0xa5, sizeof(scratch));
        iplay_audio_output_init(&output, &u8stereo, capture_audio_write, &capture, scratch, sizeof(scratch));
        iplay_audio_output_start(&output);
        iplay_audio_output_set_capacity(&output, 2);
        bytes = iplay_audio_output_write_mixer_frames(&output, src, 3);
        iplay_audio_output_write_silence(&output, 1);
        printf("bytes=%u frames=%lu dropped=%lu captured=%u data=",
               (unsigned)bytes,
               (unsigned long)iplay_audio_output_frames_written(&output),
               (unsigned long)iplay_audio_output_dropped_frames(&output),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf(" scratch=");
        print_bytes(scratch, sizeof(scratch));
        printf("\n");
        return 0;
    }

    if (streq(op, "audiosb16output")) {
        IplayAudioOutput output;
        AudioCapture capture;
        static const db pcm[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
        dw bytes;
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_audio_output_init_sb16_stereo(&output, capture_audio_write, &capture);
        iplay_audio_output_start(&output);
        iplay_audio_output_set_capacity(&output, 2);
        bytes = iplay_audio_output_write_sb16_frames(&output, pcm, 3);
        printf("ok=%u bytes=%u frames=%lu dropped=%lu captured=%u data=",
               (unsigned)iplay_audio_output_is_sb16_stereo(&output),
               (unsigned)bytes,
               (unsigned long)iplay_audio_output_frames_written(&output),
               (unsigned long)iplay_audio_output_dropped_frames(&output),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "audiooutputrefill")) {
        IplayAudioOutput output;
        AudioCapture capture;
        static const db pcm[8] = {1,2,3,4,5,6,7,8};
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_audio_output_init_sb16_stereo(&output, capture_audio_write, &capture);
        iplay_audio_output_start(&output);
        iplay_audio_output_set_capacity(&output, 1);
        iplay_audio_output_write_sb16_frames(&output, pcm, 2);
        printf("first_frames=%lu first_dropped=%lu first_capacity=%lu first_bytes=%u first_data=",
               (unsigned long)iplay_audio_output_frames_written(&output),
               (unsigned long)iplay_audio_output_dropped_frames(&output),
               (unsigned long)iplay_audio_output_capacity(&output),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        iplay_audio_output_add_capacity(&output, 2);
        iplay_audio_output_write_silence(&output, 1);
        printf(" refill_frames=%lu refill_underrun=%lu refill_dropped=%lu refill_capacity=%lu refill_bytes=%u refill_data=",
               (unsigned long)iplay_audio_output_frames_written(&output),
               (unsigned long)iplay_audio_output_underrun_frames(&output),
               (unsigned long)iplay_audio_output_dropped_frames(&output),
               (unsigned long)iplay_audio_output_capacity(&output),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "audiolevels")) {
        IplayAudioLevels levels;
        static const db pcm[12] = {
            0x00,0x80, 0x00,0x00,
            0x34,0x12, 0xff,0x7f,
            0xff,0xff, 0x00,0xe0
        };
        if (argc != 2) return 2;
        iplay_audio_sb16_stereo_levels(&levels, pcm, 3);
        printf("left_peak=%u right_peak=%u left16=%u right16=%u scale0=%u scale1=%u scale2048=%u scale16384=%u scale32768=%u\n",
               (unsigned)levels.left_peak,
               (unsigned)levels.right_peak,
               (unsigned)levels.left_16,
               (unsigned)levels.right_16,
               (unsigned)iplay_audio_level_to_16(0),
               (unsigned)iplay_audio_level_to_16(1),
               (unsigned)iplay_audio_level_to_16(2048),
               (unsigned)iplay_audio_level_to_16(16384),
               (unsigned)iplay_audio_level_to_16(32768u));
        return 0;
    }

    if (streq(op, "audiooutputlevels")) {
        IplayAudioOutput direct;
        IplayAudioOutput converted;
        AudioCapture capture;
        IplayAudioFormat u8stereo = { 44100u, 8u, 2u, 0u };
        static const db sb16_pcm[12] = {
            0x00,0x80, 0x00,0x00,
            0x34,0x12, 0xff,0x7f,
            0xff,0xff, 0x00,0xe0
        };
        static const db u8_pcm[6] = {0x00,0x80,0xff,0x40,0x80,0xff};
        db scratch[16];
        const IplayAudioLevels *levels;
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        memset(scratch, 0xa5, sizeof(scratch));
        iplay_audio_output_init_sb16_stereo(&direct, capture_audio_write, &capture);
        iplay_audio_output_start(&direct);
        iplay_audio_output_set_capacity(&direct, 2);
        iplay_audio_output_write_sb16_frames(&direct, sb16_pcm, 3);
        levels = iplay_audio_output_levels(&direct);
        printf("direct_left=%u direct_right=%u direct_l16=%u direct_r16=%u ",
               (unsigned)levels->left_peak,
               (unsigned)levels->right_peak,
               (unsigned)levels->left_16,
               (unsigned)levels->right_16);
        iplay_audio_output_init(&converted, &u8stereo, capture_audio_write, &capture, scratch, sizeof(scratch));
        iplay_audio_output_start(&converted);
        iplay_audio_output_set_capacity(&converted, 3);
        iplay_audio_output_write_mixer_frames(&converted, u8_pcm, 3);
        levels = iplay_audio_output_levels(&converted);
        printf("conv_left=%u conv_right=%u conv_l16=%u conv_r16=%u scratch=",
               (unsigned)levels->left_peak,
               (unsigned)levels->right_peak,
               (unsigned)levels->left_16,
               (unsigned)levels->right_16);
        print_bytes(scratch, 12);
        iplay_audio_output_reset_levels(&converted);
        levels = iplay_audio_output_levels(&converted);
        printf(" reset=%u,%u\n",
               (unsigned)levels->left_16,
               (unsigned)levels->right_16);
        return 0;
    }

    if (streq(op, "audioleveldraw")) {
        IplayNcPlane root;
        IplayAudioLevels levels;
        if (argc != 2) return 2;
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        levels.left_peak = 0;
        levels.right_peak = 0;
        levels.left_16 = 8;
        levels.right_16 = 15;
        iplay_audio_levels_draw_yx(&root, 1, 2, &levels, 16, 0xdb, 0xb0, 0x2a, 0x4c, 0x08);
        iplay_ncplane_meter16_yx(&root, 3, 36, 15, 8, '#', '.', 0x1e, 0x08);
        printf("left=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 2), 32);
        printf(" right=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 2), 32);
        printf(" clip=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 36), 8);
        printf(" after=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 0), 2);
        printf("\n");
        return 0;
    }

    if (streq(op, "audiooutputdraw")) {
        IplayNcPlane root;
        IplayAudioOutput output;
        AudioCapture capture;
        static const db pcm[8] = {
            0x00,0xc0, 0x00,0x20,
            0xff,0x7f, 0x00,0x80
        };
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_erase(&root, 0x07);
        iplay_audio_output_init_sb16_stereo(&output, capture_audio_write, &capture);
        iplay_audio_output_start(&output);
        iplay_audio_output_write_sb16_frames(&output, pcm, 2);
        iplay_audio_output_draw_levels_yx(&root, 0, 0, &output, 16, 0xdb, 0xb0, 0x2a, 0x4c, 0x08);
        printf("left=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 0, 0), 32);
        printf(" right=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 1, 0), 32);
        printf(" captured=%u data=",
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    if (streq(op, "audiolifecycle")) {
        IplayAudioSink sink;
        AudioCapture capture;
        static const db pcm[4] = {1,2,3,4};
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_audio_sink_init(&sink, &IPLAY_AUDIO_SB16_STEREO_16, capture_audio_write, &capture);
        iplay_audio_sink_write(&sink, pcm, sizeof(pcm));
        printf("inactive_frames=%lu inactive_underrun=%lu inactive_bytes=%u ",
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned long)iplay_audio_sink_underrun_frames(&sink),
               (unsigned)capture.bytes);
        iplay_audio_sink_start(&sink);
        iplay_audio_sink_write(&sink, pcm, sizeof(pcm));
        iplay_audio_sink_write_silence(&sink, 1);
        printf("active_frames=%lu active_underrun=%lu active_bytes=%u ",
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned long)iplay_audio_sink_underrun_frames(&sink),
               (unsigned)capture.bytes);
        iplay_audio_sink_stop(&sink);
        iplay_audio_sink_write(&sink, pcm, sizeof(pcm));
        iplay_audio_sink_write_silence(&sink, 1);
        printf("stopped_frames=%lu stopped_underrun=%lu stopped_bytes=%u active=%u\n",
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned long)iplay_audio_sink_underrun_frames(&sink),
               (unsigned)capture.bytes,
               (unsigned)iplay_audio_sink_is_active(&sink));
        return 0;
    }

    if (streq(op, "audioreset")) {
        IplayAudioSink sink;
        AudioCapture capture;
        static const db pcm[4] = {1,2,3,4};
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_audio_sink_init(&sink, &IPLAY_AUDIO_SB16_STEREO_16, capture_audio_write, &capture);
        iplay_audio_sink_start(&sink);
        iplay_audio_sink_write(&sink, pcm, sizeof(pcm));
        iplay_audio_sink_write_silence(&sink, 2);
        printf("before_frames=%lu before_underrun=%lu before_dropped=%lu ",
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned long)iplay_audio_sink_underrun_frames(&sink),
               (unsigned long)iplay_audio_sink_dropped_frames(&sink));
        iplay_audio_sink_reset_counters(&sink);
        printf("after_frames=%lu after_underrun=%lu after_dropped=%lu active=%u\n",
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned long)iplay_audio_sink_underrun_frames(&sink),
               (unsigned long)iplay_audio_sink_dropped_frames(&sink),
               (unsigned)iplay_audio_sink_is_active(&sink));
        return 0;
    }

    if (streq(op, "audiocapacity")) {
        IplayAudioSink sink;
        AudioCapture capture;
        static const db pcm[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
        if (argc != 2) return 2;
        memset(&capture, 0, sizeof(capture));
        iplay_audio_sink_init(&sink, &IPLAY_AUDIO_SB16_STEREO_16, capture_audio_write, &capture);
        iplay_audio_sink_start(&sink);
        iplay_audio_sink_set_capacity(&sink, 2);
        iplay_audio_sink_write(&sink, pcm, sizeof(pcm));
        printf("frames=%lu dropped=%lu capacity=%lu bytes=%u data=",
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned long)iplay_audio_sink_dropped_frames(&sink),
               (unsigned long)iplay_audio_sink_capacity(&sink),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        iplay_audio_sink_write(&sink, pcm + 8, 4);
        printf(" after_frames=%lu after_dropped=%lu after_capacity=%lu",
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned long)iplay_audio_sink_dropped_frames(&sink),
               (unsigned long)iplay_audio_sink_capacity(&sink));
        iplay_audio_sink_add_capacity(&sink, 1);
        iplay_audio_sink_write(&sink, pcm + 8, 4);
        printf(" refill_frames=%lu refill_dropped=%lu refill_capacity=%lu refill_bytes=%u refill_data=",
               (unsigned long)iplay_audio_sink_frames_written(&sink),
               (unsigned long)iplay_audio_sink_dropped_frames(&sink),
               (unsigned long)iplay_audio_sink_capacity(&sink),
               (unsigned)capture.bytes);
        print_bytes(capture.data, capture.bytes);
        printf("\n");
        return 0;
    }

    fprintf(stderr, "rewrite runner does not implement case: %s\n", op);
    return 2;
}
