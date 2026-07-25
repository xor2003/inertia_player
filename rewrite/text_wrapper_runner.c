#include <stdio.h>
#include <string.h>

#include "iplay_rewrite.h"

#define TEXT_RUNNER_MEM_SIZE IPLAY_TEXT_MAX_SCREEN_BYTES

static db mem[TEXT_RUNNER_MEM_SIZE];

typedef struct TextPresentCapture {
    dw cols;
    dw rows;
    dw bytes;
    dd checksum;
    dw nonblank;
    db first[2];
    db tail[2];
} TextPresentCapture;

static void capture_text_present(void *user, const db *cells, const IplayTextMode *mode, dw byte_count) {
    TextPresentCapture *capture = (TextPresentCapture *)user;
    dw tail = (dw)(byte_count - 2u);
    capture->cols = iplay_text_mode_cols(mode);
    capture->rows = iplay_text_mode_rows(mode);
    capture->bytes = byte_count;
    capture->checksum = iplay_text_cells_checksum(cells, byte_count);
    capture->nonblank = iplay_text_cells_nonblank_count(cells, byte_count);
    capture->first[0] = cells[0];
    capture->first[1] = cells[1];
    capture->tail[0] = cells[tail];
    capture->tail[1] = cells[(dw)(tail + 1u)];
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

    if (streq(argv[1], "textcelldigest")) {
        IplayTextScreen screen;
        IplayNcPlane *root;
        dw bytes;
        memset(mem, 0, sizeof(mem));
        iplay_text_screen_init_capacity(&screen, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 0, 0, 'A', 0x1e);
        iplay_ncplane_putc_yx(root, 0, 1, 'B', 0x2f);
        iplay_ncplane_putc_yx(root, 24, 39, 'C', 0x3a);
        bytes = iplay_text_screen_bytes(&screen);
        printf("bytes=%u raw_checksum=%lu screen_checksum=%lu raw_nonblank=%u screen_nonblank=%u first=",
               (unsigned)bytes,
               (unsigned long)iplay_text_cells_checksum(mem, bytes),
               (unsigned long)iplay_text_screen_checksum(&screen),
               (unsigned)iplay_text_cells_nonblank_count(mem, bytes),
               (unsigned)iplay_text_screen_nonblank_count(&screen));
        print_bytes(mem, 4);
        printf(" tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimetextdigest")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        dw bytes;
        memset(mem, 0, sizeof(mem));
        iplay_runtime_config_sdl_capacity(&config, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, 0, 0, 0, 0);
        iplay_runtime_start_config_checked(&runtime, &config, IPLAY_VIDEO_MODE_80X25_COLOR);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'R', 0x1a);
        iplay_ncplane_putc_yx(root, 12, 34, 'U', 0x2b);
        iplay_ncplane_putc_yx(root, 24, 79, 'N', 0x3c);
        bytes = iplay_runtime_video_screen_bytes(&runtime);
        printf("mode=%u,%u bytes=%u raw_checksum=%lu runtime_checksum=%lu raw_nonblank=%u runtime_nonblank=%u first=",
               (unsigned)iplay_runtime_video_cols(&runtime),
               (unsigned)iplay_runtime_video_rows(&runtime),
               (unsigned)bytes,
               (unsigned long)iplay_text_cells_checksum(mem, bytes),
               (unsigned long)iplay_runtime_video_checksum(&runtime),
               (unsigned)iplay_text_cells_nonblank_count(mem, bytes),
               (unsigned)iplay_runtime_video_nonblank_cells(&runtime));
        print_bytes(mem, 2);
        printf(" mid=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 12, 34), 2);
        printf(" tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 24, 79), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimepresentdigest")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture capture;
        dw presented;
        dw bytes;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_start_config_checked(&runtime, &config, IPLAY_VIDEO_MODE_80X25_COLOR);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'P', 0x1d);
        iplay_ncplane_putc_yx(root, 7, 11, 'C', 0x2e);
        iplay_ncplane_putc_yx(root, 24, 79, 'B', 0x3f);
        bytes = iplay_runtime_video_screen_bytes(&runtime);
        presented = iplay_runtime_present(&runtime);
        printf("presented=%u cb=%u,%u,%u raw_checksum=%lu runtime_checksum=%lu cb_checksum=%lu raw_nonblank=%u runtime_nonblank=%u cb_nonblank=%u first=",
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned long)iplay_text_cells_checksum(mem, bytes),
               (unsigned long)iplay_runtime_video_checksum(&runtime),
               (unsigned long)capture.checksum,
               (unsigned)iplay_text_cells_nonblank_count(mem, bytes),
               (unsigned)iplay_runtime_video_nonblank_cells(&runtime),
               (unsigned)capture.nonblank);
        print_bytes(capture.first, 2);
        printf(" mid=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 7, 11), 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textscreenresizebad")) {
        IplayTextScreen screen;
        IplayNcPlane *root;
        const IplayTextMode *mode;
        int ok;
        memset(mem, 0, sizeof(mem));
        iplay_text_screen_init_capacity(&screen, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_80X25);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 24, 79, 'R', 0x2a);
        ok = iplay_text_screen_resize_to_size_checked(&screen, 132, 43);
        mode = iplay_text_screen_mode(&screen);
        root = iplay_text_screen_root(&screen);
        printf("ok=%u cols=%u rows=%u stride=%u tail=",
               (unsigned)ok,
               (unsigned)iplay_text_mode_cols(mode),
               (unsigned)iplay_text_mode_rows(mode),
               (unsigned)iplay_ncplane_stride_cols(root));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 24, 79), 2);
        printf(" screenbytes=%u supported=%u\n",
               (unsigned)iplay_text_mode_screen_bytes(mode),
               (unsigned)iplay_text_size_is_supported(132, 43));
        return 0;
    }

    if (streq(argv[1], "textscreenresizecapacity")) {
        IplayTextScreen screen;
        IplayNcPlane *root;
        const IplayTextMode *mode;
        int ok;
        memset(mem, 0, sizeof(mem));
        iplay_text_screen_init_capacity(&screen, mem, iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25), &IPLAY_TEXT_MODE_40X25);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 24, 39, 'C', 0x1d);
        ok = iplay_text_screen_resize_to_size_checked(&screen, 80, 50);
        mode = iplay_text_screen_mode(&screen);
        root = iplay_text_screen_root(&screen);
        printf("ok=%u cols=%u rows=%u stride=%u tail=",
               (unsigned)ok,
               (unsigned)iplay_text_mode_cols(mode),
               (unsigned)iplay_text_mode_rows(mode),
               (unsigned)iplay_ncplane_stride_cols(root));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" screenbytes=%u capacity=%u can80x25=%u can80x50=%u fits80x25=%u fits80x50=%u\n",
               (unsigned)iplay_text_mode_screen_bytes(mode),
               (unsigned)iplay_text_screen_capacity(&screen),
               (unsigned)iplay_text_screen_can_resize(&screen, &IPLAY_TEXT_MODE_80X25),
               (unsigned)iplay_text_screen_can_resize(&screen, &IPLAY_TEXT_MODE_80X50),
               (unsigned)iplay_text_mode_fits_capacity(&IPLAY_TEXT_MODE_80X25, iplay_text_screen_capacity(&screen)),
               (unsigned)iplay_text_mode_fits_capacity(&IPLAY_TEXT_MODE_80X50, iplay_text_screen_capacity(&screen)));
        return 0;
    }

    if (streq(argv[1], "textscreenresize80x25")) {
        IplayTextScreen screen;
        IplayNcPlane *root;
        const IplayTextMode *mode;
        int ok;
        memset(mem, 0, sizeof(mem));
        iplay_text_screen_init_capacity(&screen, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 24, 39, 'O', 0x1a);
        ok = iplay_text_screen_resize_to_size_checked(&screen, 80, 25);
        mode = iplay_text_screen_mode(&screen);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 0, 0, 'S', 0x1b);
        iplay_ncplane_putc_yx(root, 24, 79, 'Z', 0x2c);
        printf("ok=%u cols=%u rows=%u stride=%u first=",
               (unsigned)ok,
               (unsigned)iplay_text_mode_cols(mode),
               (unsigned)iplay_text_mode_rows(mode),
               (unsigned)iplay_ncplane_stride_cols(root));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 0, 0), 2);
        printf(" tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 24, 79), 2);
        printf(" old40=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" screenbytes=%u supported=%u\n",
               (unsigned)iplay_text_mode_screen_bytes(mode),
               (unsigned)iplay_text_size_is_supported(80, 25));
        return 0;
    }

    if (streq(argv[1], "textscreenresize80x50")) {
        IplayTextScreen screen;
        IplayNcPlane *root;
        const IplayTextMode *mode;
        int ok;
        memset(mem, 0, sizeof(mem));
        iplay_text_screen_init_capacity(&screen, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 24, 39, 'O', 0x1c);
        ok = iplay_text_screen_resize_to_size_checked(&screen, 80, 50);
        mode = iplay_text_screen_mode(&screen);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 0, 0, 'T', 0x1d);
        iplay_ncplane_putc_yx(root, 49, 79, 'F', 0x2e);
        printf("ok=%u cols=%u rows=%u stride=%u first=",
               (unsigned)ok,
               (unsigned)iplay_text_mode_cols(mode),
               (unsigned)iplay_text_mode_rows(mode),
               (unsigned)iplay_ncplane_stride_cols(root));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 0, 0), 2);
        printf(" tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" old40=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" screenbytes=%u supported=%u\n",
               (unsigned)iplay_text_mode_screen_bytes(mode),
               (unsigned)iplay_text_size_is_supported(80, 50));
        return 0;
    }

    if (streq(argv[1], "textscreenresizecycle")) {
        IplayTextScreen screen;
        IplayNcPlane *root;
        const IplayTextMode *mode;
        int wide_ok;
        int narrow_ok;
        memset(mem, 0, sizeof(mem));
        iplay_text_screen_init_capacity(&screen, mem, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_40X25);
        wide_ok = iplay_text_screen_resize_to_size_checked(&screen, 80, 50);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 49, 79, 'W', 0x1e);
        narrow_ok = iplay_text_screen_resize_to_size_checked(&screen, 40, 25);
        mode = iplay_text_screen_mode(&screen);
        root = iplay_text_screen_root(&screen);
        iplay_ncplane_putc_yx(root, 0, 0, 'N', 0x2f);
        iplay_ncplane_putc_yx(root, 24, 39, 'R', 0x3a);
        printf("wide_ok=%u narrow_ok=%u cols=%u rows=%u stride=%u first=",
               (unsigned)wide_ok,
               (unsigned)narrow_ok,
               (unsigned)iplay_text_mode_cols(mode),
               (unsigned)iplay_text_mode_rows(mode),
               (unsigned)iplay_ncplane_stride_cols(root));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 0, 0), 2);
        printf(" tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" oldwide=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" screenbytes=%u supported=%u\n",
               (unsigned)iplay_text_mode_screen_bytes(mode),
               (unsigned)iplay_text_size_is_supported(40, 25));
        return 0;
    }

    if (streq(argv[1], "textsubplaneclip")) {
        IplayNcPlane root;
        IplayNcPlane child;
        dw origin_y = 0;
        dw origin_x = 0;
        memset(mem, 0, sizeof(mem));
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_subplane(&child, &root, 23, 38, 4, 5);
        iplay_ncplane_origin_yx(&child, &origin_y, &origin_x);
        iplay_ncplane_fill_yx(&child, 0, 0, 4, 5, 'Z', 0x6c);
        printf("rows=%u cols=%u origin=%u,%u stride=%u inside=",
               (unsigned)iplay_ncplane_rows(&child),
               (unsigned)iplay_ncplane_cols(&child),
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_ncplane_stride_cols(&child));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 23, 38), 4);
        printf(" edge=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" clipped=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 22, 38), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textsubplanezeroedge")) {
        IplayNcPlane root;
        IplayNcPlane child;
        dw origin_y = 0;
        dw origin_x = 0;
        memset(mem, 0, sizeof(mem));
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_subplane(&child, &root, 25, 40, 3, 4);
        iplay_ncplane_origin_yx(&child, &origin_y, &origin_x);
        iplay_ncplane_fill_yx(&child, 0, 0, 3, 4, 'E', 0x4e);
        printf("rows=%u cols=%u origin=%u,%u stride=%u tail=",
               (unsigned)iplay_ncplane_rows(&child),
               (unsigned)iplay_ncplane_cols(&child),
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_ncplane_stride_cols(&child));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" before=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 38), 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textsubplane80x50zeroedge")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        IplayNcPlane child;
        dw origin_y = 0;
        dw origin_x = 0;
        int ok;
        memset(mem, 0, sizeof(mem));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X50_PROJECT);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 49, 79, 'P', 0x6a);
        iplay_ncplane_subplane(&child, root, 50, 80, 3, 4);
        iplay_ncplane_origin_yx(&child, &origin_y, &origin_x);
        iplay_ncplane_fill_yx(&child, 0, 0, 3, 4, 'F', 0x5f);
        printf("ok=%u rows=%u cols=%u origin=%u,%u stride=%u tail=",
               (unsigned)ok,
               (unsigned)iplay_ncplane_rows(&child),
               (unsigned)iplay_ncplane_cols(&child),
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_ncplane_stride_cols(&child));
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" before=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 78), 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textpresent80x50")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X50_PROJECT);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 0, 0, 'H', 0x1e);
        iplay_ncplane_putc_yx(root, 49, 79, 'T', 0x2f);
        presented = iplay_notcurses_present(&nc);
        printf("ok=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u first=",
               (unsigned)ok,
               (unsigned)iplay_notcurses_cols(&nc),
               (unsigned)iplay_notcurses_rows(&nc),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textpresent80x25bw")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X25_BW);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 0, 0, 'B', 0x17);
        iplay_ncplane_putc_yx(root, 24, 79, 'W', 0x70);
        presented = iplay_notcurses_present(&nc);
        printf("ok=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u first=",
               (unsigned)ok,
               (unsigned)iplay_notcurses_cols(&nc),
               (unsigned)iplay_notcurses_rows(&nc),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textresize80x25present")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        ok = iplay_notcurses_resize_to_size_checked(&nc, 80, 25);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 0, 0, 'R', 0x1b);
        iplay_ncplane_putc_yx(root, 24, 79, 'S', 0x2c);
        presented = iplay_notcurses_present(&nc);
        printf("ok=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u first=",
               (unsigned)ok,
               (unsigned)iplay_notcurses_cols(&nc),
               (unsigned)iplay_notcurses_rows(&nc),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textmodecyclepresent")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        TextPresentCapture wide_capture;
        TextPresentCapture narrow_capture;
        dw wide_root_cols;
        dw wide_root_rows;
        int wide_ok;
        int narrow_ok;
        dw wide_presented;
        dw narrow_presented;
        memset(mem, 0, sizeof(mem));
        memset(&wide_capture, 0, sizeof(wide_capture));
        memset(&narrow_capture, 0, sizeof(narrow_capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        wide_ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X25_COLOR);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 0, 0, 'W', 0x1e);
        iplay_ncplane_putc_yx(root, 24, 79, 'E', 0x2f);
        wide_root_cols = iplay_ncplane_cols(root);
        wide_root_rows = iplay_ncplane_rows(root);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &wide_capture);
        wide_presented = iplay_notcurses_present(&nc);
        narrow_ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_40X25_COLOR);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 0, 0, 'N', 0x3a);
        iplay_ncplane_putc_yx(root, 24, 39, 'R', 0x4b);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &narrow_capture);
        narrow_presented = iplay_notcurses_present(&nc);
        printf("wide_ok=%u wide_mode=%u,%u wide_root=%u,%u wide_presented=%u wide_cb=%u,%u,%u wide_first=",
               (unsigned)wide_ok,
               (unsigned)wide_capture.cols,
               (unsigned)wide_capture.rows,
               (unsigned)wide_root_cols,
               (unsigned)wide_root_rows,
               (unsigned)wide_presented,
               (unsigned)wide_capture.cols,
               (unsigned)wide_capture.rows,
               (unsigned)wide_capture.bytes);
        print_bytes(wide_capture.first, 2);
        printf(" wide_tail=");
        print_bytes(wide_capture.tail, 2);
        printf(" narrow_ok=%u narrow_mode=%u,%u narrow_root=%u,%u narrow_presented=%u narrow_cb=%u,%u,%u narrow_first=",
               (unsigned)narrow_ok,
               (unsigned)narrow_capture.cols,
               (unsigned)narrow_capture.rows,
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)narrow_presented,
               (unsigned)narrow_capture.cols,
               (unsigned)narrow_capture.rows,
               (unsigned)narrow_capture.bytes);
        print_bytes(narrow_capture.first, 2);
        printf(" narrow_tail=");
        print_bytes(narrow_capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textpresentclear")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        TextPresentCapture capture;
        dw no_cb_presented;
        dw cb_presented;
        dw clear_presented;
        int no_cb_has;
        int cb_has;
        int clear_has;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 0, 0, 'A', 0x1a);
        no_cb_has = iplay_notcurses_has_present(&nc);
        no_cb_presented = iplay_notcurses_present(&nc);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        iplay_ncplane_putc_yx(root, 0, 0, 'B', 0x2b);
        cb_has = iplay_notcurses_has_present(&nc);
        cb_presented = iplay_notcurses_present(&nc);
        iplay_notcurses_clear_present_callback(&nc);
        iplay_ncplane_putc_yx(root, 0, 0, 'C', 0x3c);
        clear_has = iplay_notcurses_has_present(&nc);
        clear_presented = iplay_notcurses_present(&nc);
        printf("no_cb_has=%u no_cb_presented=%u cb_has=%u cb_presented=%u clear_has=%u clear_presented=%u cb=%u,%u,%u first=",
               (unsigned)no_cb_has,
               (unsigned)no_cb_presented,
               (unsigned)cb_has,
               (unsigned)cb_presented,
               (unsigned)clear_has,
               (unsigned)clear_presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" current=");
        print_bytes(mem, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textpresentreplace")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        TextPresentCapture first;
        TextPresentCapture second;
        dw first_presented;
        dw second_presented;
        int first_has;
        int second_has;
        memset(mem, 0, sizeof(mem));
        memset(&first, 0, sizeof(first));
        memset(&second, 0, sizeof(second));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        root = iplay_notcurses_stdplane(&nc);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &first);
        iplay_ncplane_putc_yx(root, 0, 0, 'I', 0x2c);
        first_has = iplay_notcurses_has_present(&nc);
        first_presented = iplay_notcurses_present(&nc);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &second);
        iplay_ncplane_putc_yx(root, 0, 0, 'J', 0x2d);
        second_has = iplay_notcurses_has_present(&nc);
        second_presented = iplay_notcurses_present(&nc);
        printf("first_has=%u first_presented=%u second_has=%u second_presented=%u first_cb=%u,%u,%u first=",
               (unsigned)first_has,
               (unsigned)first_presented,
               (unsigned)second_has,
               (unsigned)second_presented,
               (unsigned)first.cols,
               (unsigned)first.rows,
               (unsigned)first.bytes);
        print_bytes(first.first, 2);
        printf(" second_cb=%u,%u,%u second=",
               (unsigned)second.cols,
               (unsigned)second.rows,
               (unsigned)second.bytes);
        print_bytes(second.first, 2);
        printf(" current=");
        print_bytes(mem, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textbadmodepresent")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 0, 0, 'K', 0x1b);
        iplay_ncplane_putc_yx(root, 24, 39, 'Z', 0x2c);
        ok = iplay_notcurses_set_video_mode_checked(&nc, 0x99);
        root = iplay_notcurses_stdplane(&nc);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        presented = iplay_notcurses_present(&nc);
        printf("ok=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u first=",
               (unsigned)ok,
               (unsigned)iplay_notcurses_cols(&nc),
               (unsigned)iplay_notcurses_rows(&nc),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textresizecapacitypresent")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25), &IPLAY_TEXT_MODE_40X25);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 0, 0, 'C', 0x1d);
        iplay_ncplane_putc_yx(root, 24, 39, 'P', 0x2e);
        ok = iplay_notcurses_resize_to_size_checked(&nc, 80, 50);
        root = iplay_notcurses_stdplane(&nc);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        presented = iplay_notcurses_present(&nc);
        printf("ok=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u capacity=%u first=",
               (unsigned)ok,
               (unsigned)iplay_notcurses_cols(&nc),
               (unsigned)iplay_notcurses_rows(&nc),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_notcurses_capacity(&nc));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf(" fits80x25=%u fits80x50=%u\n",
               (unsigned)iplay_text_mode_fits_capacity(&IPLAY_TEXT_MODE_80X25, iplay_notcurses_capacity(&nc)),
               (unsigned)iplay_text_mode_fits_capacity(&IPLAY_TEXT_MODE_80X50, iplay_notcurses_capacity(&nc)));
        return 0;
    }

    if (streq(argv[1], "terminalresizecapacitypresent")) {
        IplayTerminal terminal;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_terminal_init_vga_memory_capacity(&terminal, mem, iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25), &IPLAY_TEXT_MODE_40X25);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &capture);
        root = iplay_terminal_root(&terminal);
        iplay_ncplane_putc_yx(root, 0, 0, 'T', 0x1f);
        iplay_ncplane_putc_yx(root, 24, 39, 'C', 0x30);
        ok = iplay_terminal_resize_to_size_checked(&terminal, 80, 50);
        root = iplay_terminal_root(&terminal);
        presented = iplay_terminal_present(&terminal);
        printf("ok=%u backend=%u has=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u capacity=%u first=",
               (unsigned)ok,
               (unsigned)iplay_terminal_backend(&terminal),
               (unsigned)iplay_terminal_has_present(&terminal),
               (unsigned)iplay_text_mode_cols(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_text_mode_rows(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_terminal_capacity(&terminal));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf(" fits80x25=%u fits80x50=%u\n",
               (unsigned)iplay_text_mode_fits_capacity(&IPLAY_TEXT_MODE_80X25, iplay_terminal_capacity(&terminal)),
               (unsigned)iplay_text_mode_fits_capacity(&IPLAY_TEXT_MODE_80X50, iplay_terminal_capacity(&terminal)));
        return 0;
    }

    if (streq(argv[1], "terminalresize80x25present")) {
        IplayTerminal terminal;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_terminal_init_vga_memory_capacity(&terminal, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &capture);
        ok = iplay_terminal_resize_to_size_checked(&terminal, 80, 25);
        root = iplay_terminal_root(&terminal);
        iplay_ncplane_putc_yx(root, 0, 0, 'A', 0x21);
        iplay_ncplane_putc_yx(root, 24, 79, 'B', 0x32);
        presented = iplay_terminal_present(&terminal);
        printf("ok=%u backend=%u has=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u capacity=%u first=",
               (unsigned)ok,
               (unsigned)iplay_terminal_backend(&terminal),
               (unsigned)iplay_terminal_has_present(&terminal),
               (unsigned)iplay_text_mode_cols(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_text_mode_rows(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_terminal_capacity(&terminal));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "terminalpresent80x25bw")) {
        IplayTerminal terminal;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_terminal_init_vga_memory_capacity(&terminal, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &capture);
        ok = iplay_terminal_set_video_mode_checked(&terminal, IPLAY_VIDEO_MODE_80X25_BW);
        root = iplay_terminal_root(&terminal);
        iplay_ncplane_putc_yx(root, 0, 0, 'B', 0x25);
        iplay_ncplane_putc_yx(root, 24, 79, 'W', 0x37);
        presented = iplay_terminal_present(&terminal);
        printf("ok=%u backend=%u has=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u capacity=%u first=",
               (unsigned)ok,
               (unsigned)iplay_terminal_backend(&terminal),
               (unsigned)iplay_terminal_has_present(&terminal),
               (unsigned)iplay_text_mode_cols(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_text_mode_rows(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_terminal_capacity(&terminal));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "terminalpresent80x25color")) {
        IplayTerminal terminal;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_terminal_init_vga_memory_capacity(&terminal, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &capture);
        ok = iplay_terminal_set_video_mode_checked(&terminal, IPLAY_VIDEO_MODE_80X25_COLOR);
        root = iplay_terminal_root(&terminal);
        iplay_ncplane_putc_yx(root, 0, 0, 'C', 0x26);
        iplay_ncplane_putc_yx(root, 24, 79, 'O', 0x38);
        presented = iplay_terminal_present(&terminal);
        printf("ok=%u backend=%u has=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u capacity=%u first=",
               (unsigned)ok,
               (unsigned)iplay_terminal_backend(&terminal),
               (unsigned)iplay_terminal_has_present(&terminal),
               (unsigned)iplay_text_mode_cols(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_text_mode_rows(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_terminal_capacity(&terminal));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "terminalbadmodepresent")) {
        IplayTerminal terminal;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_terminal_init_vga_memory_capacity(&terminal, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &capture);
        root = iplay_terminal_root(&terminal);
        iplay_ncplane_putc_yx(root, 0, 0, 'X', 0x27);
        iplay_ncplane_putc_yx(root, 24, 39, 'Y', 0x39);
        ok = iplay_terminal_set_video_mode_checked(&terminal, 0x99);
        root = iplay_terminal_root(&terminal);
        presented = iplay_terminal_present(&terminal);
        printf("ok=%u backend=%u has=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u capacity=%u first=",
               (unsigned)ok,
               (unsigned)iplay_terminal_backend(&terminal),
               (unsigned)iplay_terminal_has_present(&terminal),
               (unsigned)iplay_text_mode_cols(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_text_mode_rows(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_terminal_capacity(&terminal));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "terminalpresentclear")) {
        IplayTerminal terminal;
        IplayNcPlane *root;
        TextPresentCapture capture;
        dw no_cb_presented;
        dw cb_presented;
        dw clear_presented;
        int no_cb_has;
        int cb_has;
        int clear_has;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_terminal_init_vga_memory_capacity(&terminal, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        root = iplay_terminal_root(&terminal);
        iplay_ncplane_putc_yx(root, 0, 0, 'D', 0x28);
        no_cb_has = iplay_terminal_has_present(&terminal);
        no_cb_presented = iplay_terminal_present(&terminal);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &capture);
        iplay_ncplane_putc_yx(root, 0, 0, 'E', 0x29);
        cb_has = iplay_terminal_has_present(&terminal);
        cb_presented = iplay_terminal_present(&terminal);
        iplay_terminal_clear_present_callback(&terminal);
        iplay_ncplane_putc_yx(root, 0, 0, 'F', 0x3b);
        clear_has = iplay_terminal_has_present(&terminal);
        clear_presented = iplay_terminal_present(&terminal);
        printf("no_cb_has=%u no_cb_presented=%u cb_has=%u cb_presented=%u clear_has=%u clear_presented=%u cb=%u,%u,%u first=",
               (unsigned)no_cb_has,
               (unsigned)no_cb_presented,
               (unsigned)cb_has,
               (unsigned)cb_presented,
               (unsigned)clear_has,
               (unsigned)clear_presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" current=");
        print_bytes(mem, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "terminalpresentreplace")) {
        IplayTerminal terminal;
        IplayNcPlane *root;
        TextPresentCapture first;
        TextPresentCapture second;
        dw first_presented;
        dw second_presented;
        int first_has;
        int second_has;
        memset(mem, 0, sizeof(mem));
        memset(&first, 0, sizeof(first));
        memset(&second, 0, sizeof(second));
        iplay_terminal_init_vga_memory_capacity(&terminal, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        root = iplay_terminal_root(&terminal);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &first);
        iplay_ncplane_putc_yx(root, 0, 0, 'G', 0x2a);
        first_has = iplay_terminal_has_present(&terminal);
        first_presented = iplay_terminal_present(&terminal);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &second);
        iplay_ncplane_putc_yx(root, 0, 0, 'H', 0x2b);
        second_has = iplay_terminal_has_present(&terminal);
        second_presented = iplay_terminal_present(&terminal);
        printf("first_has=%u first_presented=%u second_has=%u second_presented=%u first_cb=%u,%u,%u first=",
               (unsigned)first_has,
               (unsigned)first_presented,
               (unsigned)second_has,
               (unsigned)second_presented,
               (unsigned)first.cols,
               (unsigned)first.rows,
               (unsigned)first.bytes);
        print_bytes(first.first, 2);
        printf(" second_cb=%u,%u,%u second=",
               (unsigned)second.cols,
               (unsigned)second.rows,
               (unsigned)second.bytes);
        print_bytes(second.first, 2);
        printf(" current=");
        print_bytes(mem, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "terminalresize80x50present")) {
        IplayTerminal terminal;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_terminal_init_vga_memory_capacity(&terminal, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &capture);
        ok = iplay_terminal_resize_to_size_checked(&terminal, 80, 50);
        root = iplay_terminal_root(&terminal);
        iplay_ncplane_putc_yx(root, 0, 0, 'W', 0x23);
        iplay_ncplane_putc_yx(root, 49, 79, 'H', 0x34);
        presented = iplay_terminal_present(&terminal);
        printf("ok=%u backend=%u has=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u capacity=%u first=",
               (unsigned)ok,
               (unsigned)iplay_terminal_backend(&terminal),
               (unsigned)iplay_terminal_has_present(&terminal),
               (unsigned)iplay_text_mode_cols(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_text_mode_rows(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_terminal_capacity(&terminal));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "terminalresizecyclepresent")) {
        IplayTerminal terminal;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int wide_ok;
        int narrow_ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_terminal_init_vga_memory_capacity(&terminal, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_terminal_set_present_callback(&terminal, capture_text_present, &capture);
        wide_ok = iplay_terminal_resize_to_size_checked(&terminal, 80, 50);
        root = iplay_terminal_root(&terminal);
        iplay_ncplane_putc_yx(root, 49, 79, 'W', 0x35);
        narrow_ok = iplay_terminal_resize_to_size_checked(&terminal, 40, 25);
        root = iplay_terminal_root(&terminal);
        iplay_ncplane_putc_yx(root, 0, 0, 'N', 0x24);
        iplay_ncplane_putc_yx(root, 24, 39, 'R', 0x36);
        presented = iplay_terminal_present(&terminal);
        printf("wide_ok=%u narrow_ok=%u backend=%u has=%u mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u capacity=%u first=",
               (unsigned)wide_ok,
               (unsigned)narrow_ok,
               (unsigned)iplay_terminal_backend(&terminal),
               (unsigned)iplay_terminal_has_present(&terminal),
               (unsigned)iplay_text_mode_cols(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_text_mode_rows(iplay_terminal_mode(&terminal)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_terminal_capacity(&terminal));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf(" oldwide=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textsubwindowpresent")) {
        IplayNotcurses nc;
        IplayWindow root;
        IplayWindow child;
        TextPresentCapture capture;
        dw origin_y = 0;
        dw origin_x = 0;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        iplay_ncplane_putc_yx(iplay_notcurses_stdplane(&nc), 3, 4, '!', 0x6d);
        iplay_window_init_root(&root, iplay_notcurses_stdplane(&nc));
        iplay_window_init_subwindow(&child, &root, 3, 5, 4, 16);
        iplay_window_origin_yx(&child, &origin_y, &origin_x);
        iplay_window_erase(&child, 0x07);
        iplay_window_draw_status_line(&child, 0, "SUB", 0x1e);
        iplay_window_draw_status_field(&child, 1, "Song", "DEMO", 0x2a, 0x4c);
        presented = iplay_notcurses_present(&nc);
        printf("origin=%u,%u rows=%u cols=%u presented=%u cb=%u,%u,%u outside=",
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_window_rows(&child),
               (unsigned)iplay_window_cols(&child),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 4), 2);
        printf(" title=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 3, 5), 32);
        printf(" field=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 4, 5), 32);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textsubwindowredraw")) {
        IplayNotcurses nc;
        IplayWindow root;
        IplayWindow child;
        TextPresentCapture capture;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        iplay_ncplane_putc_yx(iplay_notcurses_stdplane(&nc), 6, 6, '?', 0x6e);
        iplay_window_init_root(&root, iplay_notcurses_stdplane(&nc));
        iplay_window_init_subwindow(&child, &root, 6, 7, 3, 16);
        iplay_window_erase(&child, 0x07);
        iplay_window_draw_status_line(&child, 0, "OLD", 0x1e);
        printf("before=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 6, 7), 32);
        iplay_window_erase(&child, 0x03);
        iplay_window_draw_status_line(&child, 0, "NEW", 0x5a);
        presented = iplay_notcurses_present(&nc);
        printf(" after=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 6, 7), 32);
        printf(" cleared=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 7, 7), 32);
        printf(" outside=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 6, 6), 2);
        printf(" presented=%u cb=%u,%u,%u\n",
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        return 0;
    }

    if (streq(argv[1], "textsubwindowzeroedge")) {
        IplayNotcurses nc;
        IplayWindow root;
        IplayWindow child;
        TextPresentCapture capture;
        dw origin_y = 0;
        dw origin_x = 0;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        iplay_ncplane_putc_yx(iplay_notcurses_stdplane(&nc), 24, 39, 'T', 0x2d);
        iplay_window_init_root(&root, iplay_notcurses_stdplane(&nc));
        iplay_window_init_subwindow(&child, &root, 25, 40, 3, 4);
        iplay_window_origin_yx(&child, &origin_y, &origin_x);
        iplay_window_erase(&child, 0x07);
        iplay_window_draw_status_line(&child, 0, "EDGE", 0x6a);
        presented = iplay_notcurses_present(&nc);
        printf("origin=%u,%u rows=%u cols=%u presented=%u cb=%u,%u,%u tail=",
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_window_rows(&child),
               (unsigned)iplay_window_cols(&child),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" before=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 38), 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textsubwindow80x50clip")) {
        IplayNotcurses nc;
        IplayWindow root;
        IplayWindow child;
        TextPresentCapture capture;
        dw origin_y = 0;
        dw origin_x = 0;
        dw presented;
        int ok;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X50_PROJECT);
        iplay_window_init_root(&root, iplay_notcurses_stdplane(&nc));
        iplay_window_init_subwindow(&child, &root, 48, 78, 4, 5);
        iplay_window_origin_yx(&child, &origin_y, &origin_x);
        iplay_window_fill_yx(&child, 0, 0, 4, 5, 'W', 0x3c);
        presented = iplay_notcurses_present(&nc);
        printf("ok=%u origin=%u,%u rows=%u cols=%u presented=%u cb=%u,%u,%u inside=",
               (unsigned)ok,
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_window_rows(&child),
               (unsigned)iplay_window_cols(&child),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 48, 78), 4);
        printf(" tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" clipped=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 47, 78), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textsubwindow80x50zeroedge")) {
        IplayNotcurses nc;
        IplayWindow root;
        IplayWindow child;
        TextPresentCapture capture;
        dw origin_y = 0;
        dw origin_x = 0;
        dw presented;
        int ok;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X50_PROJECT);
        iplay_ncplane_putc_yx(iplay_notcurses_stdplane(&nc), 49, 79, 'Y', 0x5f);
        iplay_window_init_root(&root, iplay_notcurses_stdplane(&nc));
        iplay_window_init_subwindow(&child, &root, 50, 80, 3, 4);
        iplay_window_origin_yx(&child, &origin_y, &origin_x);
        iplay_window_erase(&child, 0x07);
        iplay_window_draw_status_line(&child, 0, "EDGE80", 0x6d);
        presented = iplay_notcurses_present(&nc);
        printf("ok=%u origin=%u,%u rows=%u cols=%u presented=%u cb=%u,%u,%u tail=",
               (unsigned)ok,
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_window_rows(&child),
               (unsigned)iplay_window_cols(&child),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" before=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 78), 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textcursorresize")) {
        IplayNcPlane root;
        IplayNcPlane child;
        dw before_y = 0;
        dw before_x = 0;
        dw after_y = 0;
        dw after_x = 0;
        memset(mem, 0, sizeof(mem));
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_subplane(&child, &root, 1, 1, 4, 5);
        iplay_ncplane_cursor_move_yx(&child, 3, 4);
        iplay_ncplane_resize(&child, 2, 3);
        iplay_ncplane_cursor_yx(&child, &before_y, &before_x);
        iplay_ncplane_putc(&child, 'Q', 0x5d);
        iplay_ncplane_cursor_yx(&child, &after_y, &after_x);
        printf("rows=%u cols=%u before=%u,%u after=%u,%u cell=",
               (unsigned)iplay_ncplane_rows(&child),
               (unsigned)iplay_ncplane_cols(&child),
               (unsigned)before_y,
               (unsigned)before_x,
               (unsigned)after_y,
               (unsigned)after_x);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 3), 2);
        printf(" outside=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 4), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textcursorresizezero")) {
        IplayNcPlane root;
        IplayNcPlane child;
        dw before_y = 0;
        dw before_x = 0;
        dw after_y = 0;
        dw after_x = 0;
        memset(mem, 0, sizeof(mem));
        iplay_ncplane_init_mode(&root, mem, &IPLAY_TEXT_MODE_40X25);
        iplay_ncplane_subplane(&child, &root, 2, 3, 4, 5);
        iplay_ncplane_cursor_move_yx(&child, 3, 4);
        iplay_ncplane_resize(&child, 0, 0);
        iplay_ncplane_cursor_yx(&child, &before_y, &before_x);
        iplay_ncplane_putc(&child, 'Z', 0x6e);
        iplay_ncplane_cursor_yx(&child, &after_y, &after_x);
        printf("rows=%u cols=%u before=%u,%u after=%u,%u origin=",
               (unsigned)iplay_ncplane_rows(&child),
               (unsigned)iplay_ncplane_cols(&child),
               (unsigned)before_y,
               (unsigned)before_x,
               (unsigned)after_y,
               (unsigned)after_x);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 3), 2);
        printf(" neighbor=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 4), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textcursor80x50resizezero")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        IplayNcPlane child;
        dw before_y = 0;
        dw before_x = 0;
        dw after_y = 0;
        dw after_x = 0;
        int ok;
        memset(mem, 0, sizeof(mem));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X50_PROJECT);
        root = iplay_notcurses_stdplane(&nc);
        iplay_ncplane_putc_yx(root, 49, 79, 'C', 0x7a);
        iplay_ncplane_subplane(&child, root, 48, 78, 2, 2);
        iplay_ncplane_cursor_move_yx(&child, 1, 1);
        iplay_ncplane_resize(&child, 0, 0);
        iplay_ncplane_cursor_yx(&child, &before_y, &before_x);
        iplay_ncplane_putc(&child, 'X', 0x4b);
        iplay_ncplane_cursor_yx(&child, &after_y, &after_x);
        printf("ok=%u rows=%u cols=%u before=%u,%u after=%u,%u tail=",
               (unsigned)ok,
               (unsigned)iplay_ncplane_rows(&child),
               (unsigned)iplay_ncplane_cols(&child),
               (unsigned)before_y,
               (unsigned)before_x,
               (unsigned)after_y,
               (unsigned)after_x);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" neighbor=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 78), 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "textcolorattrs16")) {
        IplayNotcurses nc;
        IplayNcPlane *root;
        TextPresentCapture capture;
        dw i;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_notcurses_init_vga_memory_capacity(&nc, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);
        iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);
        root = iplay_notcurses_stdplane(&nc);
        for (i = 0; i < 16; ++i) {
            iplay_ncplane_putc_yx(root, 2, i, (db)('A' + i), (db)((i << 4) | i));
        }
        presented = iplay_notcurses_present(&nc);
        printf("mode=%u,%u presented=%u cb=%u,%u,%u attrs=",
               (unsigned)iplay_notcurses_cols(&nc),
               (unsigned)iplay_notcurses_rows(&nc),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 0), 32);
        printf(" empty=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 16), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimepresentclear")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture first;
        TextPresentCapture second;
        dw first_presented;
        dw second_presented;
        dw clear_presented;
        int first_has;
        int second_has;
        int clear_has;
        memset(mem, 0, sizeof(mem));
        memset(&first, 0, sizeof(first));
        memset(&second, 0, sizeof(second));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &first, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'R', 0x1d);
        first_has = iplay_runtime_video_has_present(&runtime);
        first_presented = iplay_runtime_present(&runtime);
        iplay_runtime_video_set_present_callback(&runtime, capture_text_present, &second);
        iplay_ncplane_putc_yx(root, 0, 0, 'S', 0x2e);
        second_has = iplay_runtime_video_has_present(&runtime);
        second_presented = iplay_runtime_present(&runtime);
        iplay_runtime_video_clear_present_callback(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'T', 0x3f);
        clear_has = iplay_runtime_video_has_present(&runtime);
        clear_presented = iplay_runtime_present(&runtime);
        printf("first_has=%u first_presented=%u second_has=%u second_presented=%u clear_has=%u clear_presented=%u first_cb=%u,%u,%u first=",
               (unsigned)first_has,
               (unsigned)first_presented,
               (unsigned)second_has,
               (unsigned)second_presented,
               (unsigned)clear_has,
               (unsigned)clear_presented,
               (unsigned)first.cols,
               (unsigned)first.rows,
               (unsigned)first.bytes);
        print_bytes(first.first, 2);
        printf(" second_cb=%u,%u,%u second=",
               (unsigned)second.cols,
               (unsigned)second.rows,
               (unsigned)second.bytes);
        print_bytes(second.first, 2);
        printf(" current=");
        print_bytes(mem, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimebadmodepresent")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'V', 0x1c);
        iplay_ncplane_putc_yx(root, 24, 39, 'X', 0x2d);
        ok = iplay_runtime_set_video_mode_checked(&runtime, 0x99);
        root = iplay_runtime_stdplane(&runtime);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u status=%s token=%s mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u first=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               iplay_runtime_video_status_text(&runtime),
               iplay_runtime_video_status_token(&runtime),
               (unsigned)iplay_text_mode_cols(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_text_mode_rows(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimeresizebadpresent")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'Y', 0x1f);
        iplay_ncplane_putc_yx(root, 24, 39, 'Q', 0x2e);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 132, 43);
        root = iplay_runtime_stdplane(&runtime);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u status=%s token=%s mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u supported=%u first=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               iplay_runtime_video_status_text(&runtime),
               iplay_runtime_video_status_token(&runtime),
               (unsigned)iplay_text_mode_cols(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_text_mode_rows(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_text_size_is_supported(132, 43));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimeresizecapacitypresent")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'C', 0x1e);
        iplay_ncplane_putc_yx(root, 24, 39, 'R', 0x2f);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        root = iplay_runtime_stdplane(&runtime);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u status=%s token=%s mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u capacity=%u first=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               iplay_runtime_video_status_text(&runtime),
               iplay_runtime_video_status_token(&runtime),
               (unsigned)iplay_text_mode_cols(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_text_mode_rows(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_runtime_video_capacity(&runtime));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf(" fits80x25=%u fits80x50=%u\n",
               (unsigned)iplay_text_mode_fits_capacity(&IPLAY_TEXT_MODE_80X25, iplay_runtime_video_capacity(&runtime)),
               (unsigned)iplay_text_mode_fits_capacity(&IPLAY_TEXT_MODE_80X50, iplay_runtime_video_capacity(&runtime)));
        return 0;
    }

    if (streq(argv[1], "runtimepresent80x25bw")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_set_video_mode_checked(&runtime, IPLAY_VIDEO_MODE_80X25_BW);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'B', 0x18);
        iplay_ncplane_putc_yx(root, 24, 79, 'W', 0x71);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u status=%s token=%s mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u first=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               iplay_runtime_video_status_text(&runtime),
               iplay_runtime_video_status_token(&runtime),
               (unsigned)iplay_text_mode_cols(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_text_mode_rows(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimepresent80x25color")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_set_video_mode_checked(&runtime, IPLAY_VIDEO_MODE_80X25_COLOR);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'C', 0x19);
        iplay_ncplane_putc_yx(root, 24, 79, 'O', 0x72);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u status=%s token=%s mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u first=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               iplay_runtime_video_status_text(&runtime),
               iplay_runtime_video_status_token(&runtime),
               (unsigned)iplay_text_mode_cols(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_text_mode_rows(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimeresize80x50present")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'A', 0x1a);
        iplay_ncplane_putc_yx(root, 49, 79, 'B', 0x2b);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u status=%s token=%s mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u supported=%u first=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               iplay_runtime_video_status_text(&runtime),
               iplay_runtime_video_status_token(&runtime),
               (unsigned)iplay_text_mode_cols(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_text_mode_rows(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_text_size_is_supported(80, 50));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimeresize80x25present")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 25);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'S', 0x1b);
        iplay_ncplane_putc_yx(root, 24, 79, 'Z', 0x2c);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u status=%s token=%s mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u supported=%u first=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               iplay_runtime_video_status_text(&runtime),
               iplay_runtime_video_status_token(&runtime),
               (unsigned)iplay_text_mode_cols(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_text_mode_rows(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes,
               (unsigned)iplay_text_size_is_supported(80, 25));
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimeresizecyclepresent")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        TextPresentCapture capture;
        int wide_ok;
        int narrow_ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        wide_ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 49, 79, 'W', 0x1a);
        narrow_ok = iplay_runtime_resize_to_size_checked(&runtime, 40, 25);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 0, 0, 'C', 0x3c);
        iplay_ncplane_putc_yx(root, 24, 39, 'D', 0x4d);
        presented = iplay_runtime_present(&runtime);
        printf("wide_ok=%u narrow_ok=%u flag=%u status=%s token=%s mode=%u,%u root=%u,%u stride=%u presented=%u cb=%u,%u,%u first=",
               (unsigned)wide_ok,
               (unsigned)narrow_ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               iplay_runtime_video_status_text(&runtime),
               iplay_runtime_video_status_token(&runtime),
               (unsigned)iplay_text_mode_cols(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_text_mode_rows(iplay_runtime_video_mode(&runtime)),
               (unsigned)iplay_ncplane_cols(root),
               (unsigned)iplay_ncplane_rows(root),
               (unsigned)iplay_ncplane_stride_cols(root),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(capture.first, 2);
        printf(" tail=");
        print_bytes(capture.tail, 2);
        printf(" old_wide_tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimecursor80x50resizezero")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayNcPlane *root;
        IplayNcPlane child;
        TextPresentCapture capture;
        dw before_y = 0;
        dw before_x = 0;
        dw after_y = 0;
        dw after_x = 0;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 49, 79, 'M', 0x6d);
        iplay_ncplane_subplane(&child, root, 48, 78, 2, 2);
        iplay_ncplane_cursor_move_yx(&child, 1, 1);
        iplay_ncplane_resize(&child, 0, 0);
        iplay_ncplane_cursor_yx(&child, &before_y, &before_x);
        iplay_ncplane_putc(&child, 'N', 0x4e);
        iplay_ncplane_cursor_yx(&child, &after_y, &after_x);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u rows=%u cols=%u before=%u,%u after=%u,%u presented=%u cb=%u,%u,%u tail=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               (unsigned)iplay_ncplane_rows(&child),
               (unsigned)iplay_ncplane_cols(&child),
               (unsigned)before_y,
               (unsigned)before_x,
               (unsigned)after_y,
               (unsigned)after_x,
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" neighbor=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 78), 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimesubwindow80x50clip")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayWindow root_window;
        IplayWindow child;
        TextPresentCapture capture;
        dw origin_y = 0;
        dw origin_x = 0;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        iplay_window_init_root(&root_window, iplay_runtime_stdplane(&runtime));
        iplay_window_init_subwindow(&child, &root_window, 48, 78, 4, 5);
        iplay_window_origin_yx(&child, &origin_y, &origin_x);
        iplay_window_fill_yx(&child, 0, 0, 4, 5, 'R', 0x5e);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u origin=%u,%u rows=%u cols=%u presented=%u cb=%u,%u,%u inside=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_window_rows(&child),
               (unsigned)iplay_window_cols(&child),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 48, 78), 4);
        printf(" tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" clipped=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 47, 78), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimesubwindowresizecycleclip")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayWindow root_window;
        IplayWindow child;
        TextPresentCapture capture;
        IplayNcPlane *root;
        dw origin_y = 0;
        dw origin_x = 0;
        int wide_ok;
        int narrow_ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        wide_ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        root = iplay_runtime_stdplane(&runtime);
        iplay_ncplane_putc_yx(root, 49, 79, 'W', 0x1a);
        narrow_ok = iplay_runtime_resize_to_size_checked(&runtime, 40, 25);
        iplay_window_init_root(&root_window, iplay_runtime_stdplane(&runtime));
        iplay_window_init_subwindow(&child, &root_window, 23, 38, 4, 5);
        iplay_window_origin_yx(&child, &origin_y, &origin_x);
        iplay_window_fill_yx(&child, 0, 0, 4, 5, 'S', 0x6f);
        presented = iplay_runtime_present(&runtime);
        printf("wide_ok=%u narrow_ok=%u flag=%u origin=%u,%u rows=%u cols=%u presented=%u cb=%u,%u,%u inside=",
               (unsigned)wide_ok,
               (unsigned)narrow_ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_window_rows(&child),
               (unsigned)iplay_window_cols(&child),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 23, 38), 4);
        printf(" tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" clipped=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 22, 38), 2);
        printf(" old_wide_tail=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimesubwindowzeroedge")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayWindow root_window;
        IplayWindow child;
        TextPresentCapture capture;
        dw origin_y = 0;
        dw origin_x = 0;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        iplay_ncplane_putc_yx(iplay_runtime_stdplane(&runtime), 24, 39, 'U', 0x3d);
        iplay_window_init_root(&root_window, iplay_runtime_stdplane(&runtime));
        iplay_window_init_subwindow(&child, &root_window, 25, 40, 3, 4);
        iplay_window_origin_yx(&child, &origin_y, &origin_x);
        iplay_window_erase(&child, 0x07);
        iplay_window_draw_status_line(&child, 0, "RT", 0x6b);
        presented = iplay_runtime_present(&runtime);
        printf("flag=%u origin=%u,%u rows=%u cols=%u presented=%u cb=%u,%u,%u tail=",
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_window_rows(&child),
               (unsigned)iplay_window_cols(&child),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 39), 2);
        printf(" before=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 24, 38), 4);
        printf("\n");
        return 0;
    }

    if (streq(argv[1], "runtimesubwindow80x50zeroedge")) {
        IplayRuntime runtime;
        IplayRuntimeConfig config;
        IplayWindow root_window;
        IplayWindow child;
        TextPresentCapture capture;
        dw origin_y = 0;
        dw origin_x = 0;
        int ok;
        dw presented;
        memset(mem, 0, sizeof(mem));
        memset(&capture, 0, sizeof(capture));
        iplay_runtime_config_sdl_capacity(&config, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);
        iplay_runtime_init_config(&runtime, &config);
        ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
        iplay_ncplane_putc_yx(iplay_runtime_stdplane(&runtime), 49, 79, 'V', 0x4e);
        iplay_window_init_root(&root_window, iplay_runtime_stdplane(&runtime));
        iplay_window_init_subwindow(&child, &root_window, 50, 80, 3, 4);
        iplay_window_origin_yx(&child, &origin_y, &origin_x);
        iplay_window_erase(&child, 0x07);
        iplay_window_draw_status_line(&child, 0, "RT80", 0x6c);
        presented = iplay_runtime_present(&runtime);
        printf("ok=%u flag=%u origin=%u,%u rows=%u cols=%u presented=%u cb=%u,%u,%u tail=",
               (unsigned)ok,
               (unsigned)iplay_runtime_video_mode_ok_flag(&runtime),
               (unsigned)origin_y,
               (unsigned)origin_x,
               (unsigned)iplay_window_rows(&child),
               (unsigned)iplay_window_cols(&child),
               (unsigned)presented,
               (unsigned)capture.cols,
               (unsigned)capture.rows,
               (unsigned)capture.bytes);
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 79), 2);
        printf(" before=");
        print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_80, 49, 78), 4);
        printf("\n");
        return 0;
    }

    return 2;
}
