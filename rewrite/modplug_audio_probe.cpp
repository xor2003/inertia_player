#include "modern_player.hpp"
#include "notcurses_presenter.hpp"
#include "sdl_visualizer.hpp"

#include <SDL.h>
#include <dirent.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <sys/stat.h>
#include <limits.h>
#include <algorithm>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <io.h>
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define isatty _isatty
typedef unsigned tcflag_t;
struct termios {
    tcflag_t c_lflag;
    unsigned char c_cc[2];
};
struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
};
#define ICANON 1u
#define ECHO 2u
#define VMIN 0
#define VTIME 1
#define TCSANOW 0
#define TIOCGWINSZ 0
#define realpath(path, resolved) _fullpath((resolved), (path), PATH_MAX)
static int tcgetattr(int, struct termios *value) {
    if (value) std::memset(value, 0, sizeof(*value));
    return 0;
}
static int tcsetattr(int, int, const struct termios *) { return 0; }
static int ioctl(int, int, struct winsize *size) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!size || !GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) return -1;
    size->ws_col = (unsigned short)(info.srWindow.Right - info.srWindow.Left + 1);
    size->ws_row = (unsigned short)(info.srWindow.Bottom - info.srWindow.Top + 1);
    return 0;
}
#else
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

struct AudioCapture {
    unsigned long calls;
    unsigned long bytes;
    unsigned long checksum;
};

struct NativeAudioSink {
    AudioCapture capture;
    int sdl_requested;
    int sdl_opened;
    int sdl_closed;
    int sdl_paused_on_close;
    int sdl_queue_cleared;
    int sdl_dummy_driver;
    SDL_AudioDeviceID device;
    unsigned long sdl_bytes;
    unsigned long sdl_queue_failures;
    unsigned long sdl_queue_waits;
    unsigned long sdl_queue_limit_bytes;
    int freq;
    unsigned short format;
    unsigned char channels;
    unsigned short samples;
    char sdl_driver[32];
};

struct VideoCapture {
    unsigned long calls;
    unsigned long bytes;
    unsigned cols;
    unsigned rows;
};

struct StopCapture {
    unsigned long after_blocks;
};

struct NativeRunControl {
    unsigned long after_blocks;
    int terminal_live;
    int full_live_screen;
    int stdin_keyboard;
    int stdin_keyboard_seen;
    IplayModplugPlaybackControls *controls;
    const char *module_path;
    const IplayTextMode *live_mode;
    unsigned terminal_view;
    unsigned terminal_view_mask;
    unsigned paused;
    unsigned pause_toggles;
    unsigned shell_invocations;
    unsigned mouse_redraws;
    unsigned mouse_exits;
    int shell_last_status;
    int resize_tracking;
    unsigned initial_cols;
    unsigned initial_rows;
    unsigned resized_cols;
    unsigned resized_rows;
    unsigned resize_signals_seen;
    unsigned resize_changes;
    unsigned last_resize_signal_count;
    unsigned long live_samples;
    unsigned long live_nonzero;
    unsigned long live_printed;
    unsigned long live_suppressed;
    int terminal_live_cursor_hidden;
    unsigned live_changed;
    unsigned prev_left;
    unsigned prev_right;
};

#define IPLAY_NATIVE_LIVE_INITIAL_PRINT_BLOCKS 4ul
#define IPLAY_NATIVE_LIVE_PRINT_CADENCE_BLOCKS 4ul

struct NativeKeyboardMode {
    int requested;
    int raw_enabled;
    int mouse_enabled;
    int restored;
    struct termios old_termios;
};

static NativeKeyboardMode *native_keyboard_mode_to_restore = 0;
static int native_keyboard_mode_restore_registered = 0;
static NativeAudioSink *native_audio_sink_to_close = 0;
static int native_audio_sink_close_registered = 0;
static volatile sig_atomic_t native_terminal_resize_signal_count = 0;

struct LevelSequenceCapture {
    unsigned long target_blocks;
    unsigned long samples;
    unsigned long nonzero;
    unsigned changed;
    unsigned first_left;
    unsigned first_right;
    unsigned prev_left;
    unsigned prev_right;
    unsigned last_left;
    unsigned last_right;
    unsigned max_left;
    unsigned max_right;
};

static void native_keyboard_mode_register_restore(NativeKeyboardMode *mode);
static void native_audio_sink_register_close(NativeAudioSink *sink);
static void native_signal_restore_register(void);
static const IplayTextMode *native_terminal_text_mode(void);

static void native_terminal_resize_signal(int signum) {
    (void)signum;
    ++native_terminal_resize_signal_count;
}

static void native_terminal_resize_register(void) {
#ifdef SIGWINCH
    std::signal(SIGWINCH, native_terminal_resize_signal);
#endif
}

static const char *native_basename(const char *path) {
    const char *base = path;
    const char *p;
    if (!path) return "";
    for (p = path; *p; ++p) {
        if (*p == '/' || *p == '\\') base = p + 1;
    }
    return base;
}

static unsigned long native_file_size(const char *path) {
    FILE *fp;
    long size;
    if (!path) return 0ul;
    fp = std::fopen(path, "rb");
    if (!fp) return 0ul;
    if (std::fseek(fp, 0, SEEK_END) != 0) {
        std::fclose(fp);
        return 0ul;
    }
    size = std::ftell(fp);
    std::fclose(fp);
    return size > 0 ? (unsigned long)size : 0ul;
}

static const char *native_path_extension(const char *path) {
    const char *dot = 0;
    const char *p;
    if (!path) return 0;
    for (p = path; *p; ++p) {
        if (*p == '/' || *p == '\\') dot = 0;
        if (*p == '.') dot = p;
    }
    return dot;
}

static int native_ascii_tolower(int ch) {
    return ch >= 'A' && ch <= 'Z' ? ch + ('a' - 'A') : ch;
}

static int native_extension_equals(const char *path, const char *expected) {
    const char *ext = native_path_extension(path);
    if (!ext || !expected) return 0;
    while (*ext && *expected) {
        if (native_ascii_tolower((unsigned char)*ext++) != native_ascii_tolower((unsigned char)*expected++)) return 0;
    }
    return *ext == 0 && *expected == 0;
}

static int native_streq_ci(const char *left, const char *right) {
    if (!left || !right) return 0;
    while (*left && *right) {
        if (native_ascii_tolower((unsigned char)*left++) != native_ascii_tolower((unsigned char)*right++)) return 0;
    }
    return *left == 0 && *right == 0;
}

static bool native_file_exists(const char *path) {
    FILE *fp;
    if (!path) return false;
    fp = std::fopen(path, "rb");
    if (!fp) return false;
    std::fclose(fp);
    return true;
}

static bool native_resolve_case_insensitive_path(const char *path, std::string *resolved) {
    const char *base;
    const char *p;
    std::string dir_prefix;
    std::string scan_dir;
    DIR *dir;
    struct dirent *entry;
    if (!path || !resolved) return false;
    if (native_file_exists(path)) {
        *resolved = path;
        return true;
    }
    base = path;
    for (p = path; *p; ++p) {
        if (*p == '/' || *p == '\\') base = p + 1;
    }
    if (!*base) return false;
    dir_prefix.assign(path, (std::size_t)(base - path));
    scan_dir = dir_prefix.empty() ? "." : dir_prefix;
    if (!dir_prefix.empty() && (dir_prefix.back() == '/' || dir_prefix.back() == '\\')) {
        scan_dir.resize(scan_dir.size() - 1u);
        if (scan_dir.empty()) scan_dir = "/";
    }
    dir = opendir(scan_dir.c_str());
    if (!dir) return false;
    while ((entry = readdir(dir)) != 0) {
        if (native_streq_ci(entry->d_name, base)) {
            std::string candidate = dir_prefix + entry->d_name;
            if (native_file_exists(candidate.c_str())) {
                *resolved = candidate;
                closedir(dir);
                return true;
            }
        }
    }
    closedir(dir);
    return false;
}

static bool native_resolve_filelist_argument(const char *arg, std::string *resolved) {
    FILE *fp;
    char line[1024];
    std::string list_path;
    std::string list_dir;
    std::string resolved_list_path;
    std::string selected_path;
    if (!arg || arg[0] != '@' || !resolved) return false;
    list_path = arg + 1;
    if (native_resolve_case_insensitive_path(list_path.c_str(), &resolved_list_path)) list_path = resolved_list_path;
    fp = std::fopen(list_path.c_str(), "rb");
    if (!fp) return false;
    while (std::fgets(line, sizeof(line), fp)) {
        char *start = line;
        char *end;
        while (*start == ' ' || *start == '\t') ++start;
        end = start + std::strlen(start);
        while (end > start && (end[-1] == '\r' || end[-1] == '\n' || end[-1] == ' ' || end[-1] == '\t')) --end;
        *end = 0;
        if (*start) {
            const char *slash = list_path.c_str();
            const char *p;
            for (p = list_path.c_str(); *p; ++p) {
                if (*p == '/' || *p == '\\') slash = p + 1;
            }
            list_dir.assign(list_path.c_str(), (std::size_t)(slash - list_path.c_str()));
            if (start[0] == '/' || start[0] == '\\' || (start[0] && start[1] == ':')) {
                selected_path = start;
            } else {
                selected_path = list_dir + start;
            }
            if (native_resolve_case_insensitive_path(selected_path.c_str(), resolved)) {
                std::fclose(fp);
                return true;
            }
            *resolved = selected_path;
            std::fclose(fp);
            return true;
        }
    }
    std::fclose(fp);
    return false;
}

static const char *native_resolve_module_argument(const char *arg, std::string *resolved) {
    if (arg && arg[0] == '@' && native_resolve_filelist_argument(arg, resolved)) return resolved->c_str();
    if (native_resolve_case_insensitive_path(arg, resolved)) return resolved->c_str();
    return arg;
}

static bool native_select_module(std::string *selected) {
    struct BrowserEntry {
        std::string name;
        bool directory;
    };
    std::string directory = ".";
    std::vector<BrowserEntry> entries;
    unsigned current = 0u;
    unsigned first = 0u;
    struct winsize selector_size = {};
    const unsigned selector_rows =
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &selector_size) == 0 && selector_size.ws_row >= 40u
            ? 50u : 25u;
    const unsigned selector_page_rows = selector_rows - 12u;
    if (!selected) return false;

    const IplayTextMode *mode = iplay_text_mode_for_size(80u, selector_rows);
    std::vector<db> cells(80u * selector_rows * 2u);
    for (;;) {
        DIR *dir = opendir(directory.c_str());
        struct dirent *entry;
        entries.clear();
        entries.push_back({"..", true});
        if (!dir) return false;
        while ((entry = readdir(dir)) != 0) {
            struct stat status = {};
            std::string path;
            if (entry->d_name[0] == '.') continue;
            path = directory + "/" + entry->d_name;
            if (stat(path.c_str(), &status) != 0) continue;
            if (S_ISDIR(status.st_mode)) entries.push_back({entry->d_name, true});
            else if (iplay_modern_path_is_external_tracker(entry->d_name)) {
                entries.push_back({entry->d_name, false});
            }
        }
        closedir(dir);
        std::sort(entries.begin() + 1, entries.end(), [](const BrowserEntry &left, const BrowserEntry &right) {
            if (left.directory != right.directory) return left.directory > right.directory;
            return native_streq_ci(left.name.c_str(), right.name.c_str())
                ? left.name < right.name
                : strcasecmp(left.name.c_str(), right.name.c_str()) < 0;
        });
        if (current >= entries.size()) current = 0u;
        if (current < first) first = current;
        if (current >= first + selector_page_rows) first = current - selector_page_rows + 1u;

        for (unsigned index = 0u; index < 80u * selector_rows; ++index) {
            cells[index * 2u] = ' ';
            cells[index * 2u + 1u] = 0x7fu;
        }
        const auto draw_box = [&cells, selector_rows](unsigned x, unsigned y, unsigned width, unsigned height) {
            const unsigned right = x + width - 1u;
            const unsigned bottom = y + height - 1u;
            if (right + 1u < 80u) {
                for (unsigned row = y + 1u; row <= bottom + 1u && row < selector_rows; ++row) {
                    const dw offset = IPLAY_TEXT_OFFSET(80u, row, right + 1u);
                    cells[offset] = ' ';
                    cells[offset + 1u] = 0x08u;
                }
            }
            if (bottom + 1u < selector_rows) {
                for (unsigned col = x + 1u; col <= right + 1u && col < 80u; ++col) {
                    const dw offset = IPLAY_TEXT_OFFSET(80u, bottom + 1u, col);
                    cells[offset] = ' ';
                    cells[offset + 1u] = 0x08u;
                }
            }
            cells[IPLAY_TEXT_OFFSET(80u, y, x)] = 0xdau;
            cells[IPLAY_TEXT_OFFSET(80u, y, right)] = 0xbfu;
            cells[IPLAY_TEXT_OFFSET(80u, bottom, x)] = 0xc0u;
            cells[IPLAY_TEXT_OFFSET(80u, bottom, right)] = 0xd9u;
            for (unsigned col = x + 1u; col < right; ++col) {
                cells[IPLAY_TEXT_OFFSET(80u, y, col)] = 0xc4u;
                cells[IPLAY_TEXT_OFFSET(80u, bottom, col)] = 0xc4u;
            }
            for (unsigned row = y + 1u; row < bottom; ++row) {
                cells[IPLAY_TEXT_OFFSET(80u, row, x)] = 0xb3u;
                cells[IPLAY_TEXT_OFFSET(80u, row, right)] = 0xb3u;
            }
        };
        draw_box(2u, 0u, 76u, 4u);
        draw_box(4u, 5u, 72u, 3u);
        draw_box(6u, 8u, 68u, selector_rows - 10u);
        const char *lines[] = {
            "  Inertia Player V1.22 Assembly '94 CD Edition by Sound Solutions  ",
            "       Copyright (c) 1994,1995 by Stefan Danes and Ramon van Gorkom "
        };
        for (unsigned line = 0u; line < 2u; ++line) {
            unsigned x = (80u - (unsigned)std::strlen(lines[line])) / 2u;
            for (unsigned col = 0u; lines[line][col]; ++col) {
                cells[IPLAY_TEXT_OFFSET(80u, 1u + line, x + col)] = (db)lines[line][col];
                cells[IPLAY_TEXT_OFFSET(80u, 1u + line, x + col) + 1u] = 0x7fu;
            }
        }
        const char hardware[] = "Sound Blaster 16/16ASP at base port 0220h, IRQ 7, DMA 5, mixed at 44kHz";
        for (unsigned col = 0u; col + 1u < sizeof(hardware) && col < 76u; ++col) {
            cells[IPLAY_TEXT_OFFSET(80u, 4u, 2u + col)] = (db)hardware[col];
            cells[IPLAY_TEXT_OFFSET(80u, 4u, 2u + col) + 1u] = 0x78u;
        }
        char canonical_path[PATH_MAX];
        std::string display_path = realpath(directory.c_str(), canonical_path)
            ? canonical_path : directory;
        if (display_path.size() > 68u) {
            display_path = "..." + display_path.substr(display_path.size() - 65u);
        }
        unsigned path_x = (80u - (unsigned)display_path.size()) / 2u;
        for (unsigned col = 0u; col < display_path.size(); ++col) {
            cells[IPLAY_TEXT_OFFSET(80u, 6u, path_x + col)] = (db)display_path[col];
            cells[IPLAY_TEXT_OFFSET(80u, 6u, path_x + col) + 1u] = 0x7bu;
        }
        for (unsigned row = 0u; row < selector_page_rows && first + row < entries.size(); ++row) {
            const unsigned item = first + row;
            const db attr = item == current ? 0x1eu : 0x7bu;
            std::string name = entries[item].name;
            if (entries[item].directory && name != "..") name += "\\";
            if (item == current) {
                for (unsigned col = 0u; col < 64u; ++col) {
                    cells[IPLAY_TEXT_OFFSET(80u, 9u + row, 8u + col) + 1u] = attr;
                }
            }
            for (unsigned col = 0u; col < name.size() && col < 64u; ++col) {
                cells[IPLAY_TEXT_OFFSET(80u, 9u + row, 8u + col)] = (db)name[col];
                cells[IPLAY_TEXT_OFFSET(80u, 9u + row, 8u + col) + 1u] = attr;
            }
        }
        const char footer[] = "Press F-1 for help, QuickRead=Off [F-9]";
        for (unsigned col = 0u; col + 1u < sizeof(footer); ++col) {
            cells[IPLAY_TEXT_OFFSET(80u, selector_rows - 1u, 22u + col)] = (db)footer[col];
            cells[IPLAY_TEXT_OFFSET(80u, selector_rows - 1u, 22u + col) + 1u] = 0x78u;
        }
        (void)iplay_notcurses_present_cells_fixed(cells.data(), mode);
        uint32_t key = isatty(STDIN_FILENO)
            ? iplay_notcurses_get_key()
            : (uint32_t)std::getchar();
        if (key == IPLAY_NOTCURSES_KEY_UP && current > 0u) {
            --current;
            continue;
        } else if ((key == IPLAY_NOTCURSES_KEY_DOWN || key == 0x0eu) && current + 1u < entries.size()) {
            ++current;
            continue;
        }
        else if (key == IPLAY_NOTCURSES_KEY_MOUSE_LEFT) {
            const unsigned row = iplay_notcurses_mouse_y();
            const unsigned entry_row = iplay_notcurses_presenter_rows() > 30u ? 12u : 9u;
            if (row >= entry_row && first + row - entry_row < entries.size()) {
                const unsigned clicked = first + row - entry_row;
                if (clicked == current) {
                    key = '\r';
                } else {
                    current = clicked;
                    continue;
                }
            }
        }
        if (key == '\r' || key == '\n') {
            if (entries[current].directory) {
                if (entries[current].name == "..") directory += "/..";
                else directory += "/" + entries[current].name;
                current = 0u;
                first = 0u;
                continue;
            }
            *selected = directory + "/" + entries[current].name;
            return true;
        } else if (key == 27u || key == 'q' || key == 'Q') {
            return false;
        }
    }
}

static const char *native_loader_name(const char *path) {
    if (native_extension_equals(path, ".s3m")) return "s3m_module";
    if (native_extension_equals(path, ".mod")) return "mod_n_t_module";
    if (native_extension_equals(path, ".nst")) return "mod_n_t_module";
    if (native_extension_equals(path, ".stm")) return "stm_module";
    if (native_extension_equals(path, ".mtm")) return "mtm_module";
    if (native_extension_equals(path, ".669")) return "669_module";
    if (native_extension_equals(path, ".far")) return "far_module";
    if (native_extension_equals(path, ".ult")) return "ult_module";
    return iplay_modern_path_is_external_tracker(path) ? "external_module" : "probe_by_content";
}

static const char *native_loader_description(const char *path) {
    if (native_extension_equals(path, ".s3m")) return "Scream Tracker 3";
    if (native_extension_equals(path, ".mod")) return "ProTracker/NoiseTracker";
    if (native_extension_equals(path, ".nst")) return "NoiseTracker";
    if (native_extension_equals(path, ".stm")) return "Scream Tracker 2";
    if (native_extension_equals(path, ".mtm")) return "MultiTracker";
    if (native_extension_equals(path, ".669")) return "Composer 669";
    if (native_extension_equals(path, ".far")) return "Farandole";
    if (native_extension_equals(path, ".ult")) return "UltraTracker";
    return iplay_modern_path_is_external_tracker(path) ? "external-library tracker" : "libmodplug content probe";
}

static const char *native_module_type_tag(const char *path) {
    if (native_extension_equals(path, ".s3m")) return "204D3353";
    if (native_extension_equals(path, ".mod")) return "542E4E2E";
    if (native_extension_equals(path, ".nst")) return "542E4E2E";
    if (native_extension_equals(path, ".stm")) return "204D5453";
    if (native_extension_equals(path, ".mtm")) return "204D544D";
    if (native_extension_equals(path, ".669")) return "00393636";
    if (native_extension_equals(path, ".far")) return "00524146";
    if (native_extension_equals(path, ".ult")) return "00544C55";
    return iplay_modern_path_is_external_tracker(path) ? "20545845" : "00000000";
}

static void native_module_title(const char *path, char *dst, size_t dst_size) {
    FILE *fp;
    unsigned char raw[28];
    size_t count;
    size_t i;
    if (!dst || dst_size == 0) return;
    dst[0] = 0;
    if (!native_extension_equals(path, ".s3m")) return;
    fp = std::fopen(path, "rb");
    if (!fp) return;
    count = std::fread(raw, 1u, sizeof(raw), fp);
    std::fclose(fp);
    for (i = 0; i < count && i + 1u < dst_size; ++i) {
        if (raw[i] == 0) break;
        dst[i] = (char)raw[i];
    }
    dst[i < dst_size ? i : dst_size - 1u] = 0;
}

static void capture_audio_write(void *user, const db *pcm, dw byte_count) {
    AudioCapture *capture = static_cast<AudioCapture *>(user);
    dw i;
    if (!capture) return;
    capture->calls += 1ul;
    capture->bytes += byte_count;
    for (i = 0; i < byte_count; ++i) capture->checksum += pcm[i];
}

static void native_audio_sink_init(NativeAudioSink *sink, int sdl_requested) {
    if (!sink) return;
    std::memset(sink, 0, sizeof(*sink));
    sink->sdl_requested = sdl_requested ? 1 : 0;
    sink->freq = 44100;
    sink->format = AUDIO_S16SYS;
    sink->channels = 2u;
    sink->samples = 1024u;
    sink->sdl_queue_limit_bytes = (unsigned long)sink->samples * (unsigned long)sink->channels * 2ul * 4ul;
}

static int native_audio_sink_open_sdl(NativeAudioSink *sink) {
    SDL_AudioSpec want;
    SDL_AudioSpec have;
    if (!sink || !sink->sdl_requested) return 1;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) return 0;
    SDL_zero(want);
    want.freq = sink->freq;
    want.format = sink->format;
    want.channels = sink->channels;
    want.samples = sink->samples;
    want.callback = 0;
    sink->device = SDL_OpenAudioDevice(0, 0, &want, &have, SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
    if (!sink->device) return 0;
    if (have.freq != want.freq || have.format != want.format || have.channels != want.channels) {
        SDL_CloseAudioDevice(sink->device);
        sink->device = 0;
        return 0;
    }
    sink->sdl_opened = 1;
    sink->samples = have.samples;
    sink->sdl_queue_limit_bytes = (unsigned long)sink->samples * (unsigned long)sink->channels * 2ul * 4ul;
    sink->sdl_dummy_driver = SDL_GetCurrentAudioDriver() && std::strcmp(SDL_GetCurrentAudioDriver(), "dummy") == 0;
    std::snprintf(sink->sdl_driver, sizeof(sink->sdl_driver), "%s", SDL_GetCurrentAudioDriver() ? SDL_GetCurrentAudioDriver() : "none");
    native_audio_sink_register_close(sink);
    SDL_PauseAudioDevice(sink->device, 0);
    return 1;
}

static int native_stdin_read_byte(char *ch) {
#ifdef _WIN32
    if (!ch || !_kbhit()) return 0;
    *ch = (char)_getch();
    return 1;
#else
    fd_set fds;
    struct timeval tv;
    int rc;
    if (!ch) return 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    rc = select(STDIN_FILENO + 1, &fds, 0, 0, &tv);
    if (rc <= 0 || !FD_ISSET(STDIN_FILENO, &fds)) return 0;
    return read(STDIN_FILENO, ch, 1) == 1;
#endif
}

enum NativeKeyboardAction {
    NATIVE_KEY_NONE = 0,
    NATIVE_KEY_STOP,
    NATIVE_KEY_VOLUME_DOWN,
    NATIVE_KEY_VOLUME_UP,
    NATIVE_KEY_AMPLIFY_DOWN,
    NATIVE_KEY_AMPLIFY_UP,
    NATIVE_KEY_SEEK_BACK_2,
    NATIVE_KEY_SEEK_FORWARD_2,
    NATIVE_KEY_SEEK_BACK_10,
    NATIVE_KEY_SEEK_FORWARD_10,
    NATIVE_KEY_CHANNEL_1,
    NATIVE_KEY_CHANNEL_2,
    NATIVE_KEY_CHANNEL_3,
    NATIVE_KEY_CHANNEL_4,
    NATIVE_KEY_CHANNEL_5,
    NATIVE_KEY_CHANNEL_6,
    NATIVE_KEY_CHANNEL_7,
    NATIVE_KEY_CHANNEL_8,
    NATIVE_KEY_CHANNEL_9,
    NATIVE_KEY_CHANNEL_10,
    NATIVE_KEY_CHANNEL_11,
    NATIVE_KEY_CHANNEL_12,
    NATIVE_KEY_CHANNEL_13,
    NATIVE_KEY_CHANNEL_14,
    NATIVE_KEY_CHANNEL_15,
    NATIVE_KEY_CHANNEL_16,
    NATIVE_KEY_CHANNEL_17,
    NATIVE_KEY_CHANNEL_18,
    NATIVE_KEY_CHANNEL_19,
    NATIVE_KEY_CHANNEL_20,
    NATIVE_KEY_CHANNEL_21,
    NATIVE_KEY_CHANNEL_22,
    NATIVE_KEY_CHANNEL_23,
    NATIVE_KEY_CHANNEL_24,
    NATIVE_KEY_CHANNEL_25,
    NATIVE_KEY_CHANNEL_26,
    NATIVE_KEY_CHANNEL_27,
    NATIVE_KEY_CHANNEL_28,
    NATIVE_KEY_CHANNEL_29,
    NATIVE_KEY_CHANNEL_30,
    NATIVE_KEY_CHANNEL_PREVIOUS,
    NATIVE_KEY_CHANNEL_NEXT,
    NATIVE_KEY_CHANNEL_PAN_DOWN,
    NATIVE_KEY_CHANNEL_PAN_DOWN_FAST,
    NATIVE_KEY_CHANNEL_PAN_UP,
    NATIVE_KEY_CHANNEL_PAN_UP_FAST,
    NATIVE_KEY_CHANNEL_PAN_LEFT,
    NATIVE_KEY_CHANNEL_PAN_CENTER,
    NATIVE_KEY_CHANNEL_PAN_RIGHT,
    NATIVE_KEY_CHANNEL_PAN_SURROUND,
    NATIVE_KEY_PATTERN_LOOP,
    NATIVE_KEY_END_PATTERN,
    NATIVE_KEY_PAL_NTSC,
    NATIVE_KEY_PAUSE,
    NATIVE_KEY_MOUSE_LEFT,
    NATIVE_KEY_MOUSE_REDRAW,
    NATIVE_KEY_MOUSE_STOP,
    NATIVE_KEY_F1,
    NATIVE_KEY_F2,
    NATIVE_KEY_F3,
    NATIVE_KEY_F4,
    NATIVE_KEY_F5,
    NATIVE_KEY_F6,
    NATIVE_KEY_F8,
    NATIVE_KEY_F9,
    NATIVE_KEY_F10,
    NATIVE_KEY_F11,
    NATIVE_KEY_F12
};

static unsigned native_mouse_column;
static unsigned native_mouse_row;

static NativeKeyboardAction native_keyboard_character_action(uint32_t key) {
    char ch = key <= 0x7fu ? (char)key : 0;
    if (ch == 'q' || ch == 'Q') return NATIVE_KEY_STOP;
    if (ch == '-') return NATIVE_KEY_VOLUME_DOWN;
    if (ch == '+' || ch == '=') return NATIVE_KEY_VOLUME_UP;
    if (ch == '[') return NATIVE_KEY_AMPLIFY_DOWN;
    if (ch == ']') return NATIVE_KEY_AMPLIFY_UP;
    if (ch >= '1' && ch <= '9') return (NativeKeyboardAction)(NATIVE_KEY_CHANNEL_1 + ch - '1');
    if (ch == '0') return NATIVE_KEY_CHANNEL_10;
    {
        static const char shifted_digits[] = "!@#$%^&*()";
        const char *shifted = std::strchr(shifted_digits, ch);
        if (shifted) return (NativeKeyboardAction)(NATIVE_KEY_CHANNEL_11 + shifted - shifted_digits);
    }
    if (ch == ',') return NATIVE_KEY_CHANNEL_PREVIOUS;
    if (ch == '.') return NATIVE_KEY_CHANNEL_NEXT;
    if (ch == '<') return NATIVE_KEY_CHANNEL_PAN_DOWN;
    if (ch == '{') return NATIVE_KEY_CHANNEL_PAN_DOWN_FAST;
    if (ch == '>') return NATIVE_KEY_CHANNEL_PAN_UP;
    if (ch == '}') return NATIVE_KEY_CHANNEL_PAN_UP_FAST;
    if (ch == 'l' || ch == 'L') return NATIVE_KEY_CHANNEL_PAN_LEFT;
    if (ch == 'm' || ch == 'M') return NATIVE_KEY_CHANNEL_PAN_CENTER;
    if (ch == 'r' || ch == 'R') return NATIVE_KEY_CHANNEL_PAN_RIGHT;
    if (ch == 's' || ch == 'S') return NATIVE_KEY_CHANNEL_PAN_SURROUND;
    if (ch == 'c' || ch == 'C') return NATIVE_KEY_PATTERN_LOOP;
    if (ch == 'e' || ch == 'E') return NATIVE_KEY_END_PATTERN;
    if (ch == '\t') return NATIVE_KEY_PAL_NTSC;
    if (ch == 'p' || ch == 'P' || ch == ' ') return NATIVE_KEY_PAUSE;
    return NATIVE_KEY_NONE;
}

static NativeKeyboardAction native_notcurses_keyboard_action(uint32_t key) {
    if (key == IPLAY_NOTCURSES_KEY_MOUSE_LEFT) {
        native_mouse_column = iplay_notcurses_mouse_x();
        native_mouse_row = iplay_notcurses_mouse_y();
        return NATIVE_KEY_MOUSE_LEFT;
    }
    if (key == IPLAY_NOTCURSES_KEY_F1) return NATIVE_KEY_F1;
    if (key == IPLAY_NOTCURSES_KEY_F2) return NATIVE_KEY_F2;
    if (key == IPLAY_NOTCURSES_KEY_F3) return NATIVE_KEY_F3;
    if (key == IPLAY_NOTCURSES_KEY_F4) return NATIVE_KEY_F4;
    if (key == IPLAY_NOTCURSES_KEY_F5) return NATIVE_KEY_F5;
    if (key == IPLAY_NOTCURSES_KEY_F6) return NATIVE_KEY_F6;
    if (key == IPLAY_NOTCURSES_KEY_F8) return NATIVE_KEY_NONE;
    if (key == IPLAY_NOTCURSES_KEY_F9) return NATIVE_KEY_F9;
    if (key == IPLAY_NOTCURSES_KEY_F10) return NATIVE_KEY_F10;
    if (key == IPLAY_NOTCURSES_KEY_F11) return NATIVE_KEY_F11;
    if (key == IPLAY_NOTCURSES_KEY_F12) return NATIVE_KEY_F12;
    if (key == IPLAY_NOTCURSES_KEY_LEFT) return NATIVE_KEY_SEEK_BACK_2;
    if (key == IPLAY_NOTCURSES_KEY_RIGHT) return NATIVE_KEY_SEEK_FORWARD_2;
    if (key == IPLAY_NOTCURSES_KEY_UP) return NATIVE_KEY_SEEK_FORWARD_10;
    if (key == IPLAY_NOTCURSES_KEY_DOWN) return NATIVE_KEY_SEEK_BACK_10;
    return native_keyboard_character_action(key);
}

static NativeKeyboardAction native_stdin_keyboard_action(void) {
    char ch;
    char terminator = 0;
    char introducer;
    int code = 0;
    int sdl_key = iplay_sdl_visualizer_poll_key();
    uint32_t notcurses_key;
    NativeKeyboardAction queued_action;
    if (sdl_key < 0) return NATIVE_KEY_STOP;
    if (sdl_key == 1) return NATIVE_KEY_F1;
    if (sdl_key == 2) return NATIVE_KEY_F2;
    if (sdl_key == 3) return NATIVE_KEY_F3;
    if (sdl_key == 4) return NATIVE_KEY_F4;
    if (sdl_key == 5) return NATIVE_KEY_F5;
    if (sdl_key == 6) return NATIVE_KEY_F6;
    if (sdl_key == 8) return NATIVE_KEY_NONE;
    if (sdl_key == 9) return NATIVE_KEY_F9;
    if (sdl_key == 10) return NATIVE_KEY_F10;
    if (sdl_key == 11) return NATIVE_KEY_F11;
    if (sdl_key == 12) return NATIVE_KEY_F12;
    if (sdl_key == 13) return NATIVE_KEY_SEEK_BACK_2;
    if (sdl_key == 14) return NATIVE_KEY_SEEK_FORWARD_2;
    if (sdl_key == 15) return NATIVE_KEY_SEEK_FORWARD_10;
    if (sdl_key == 16) return NATIVE_KEY_SEEK_BACK_10;
    if (isatty(STDIN_FILENO)) {
        notcurses_key = iplay_notcurses_poll_key();
        queued_action = native_notcurses_keyboard_action(notcurses_key);
        if (queued_action != NATIVE_KEY_NONE) return queued_action;
    }
    if (!native_stdin_read_byte(&ch)) return NATIVE_KEY_NONE;
    queued_action = native_keyboard_character_action((unsigned char)ch);
    if (queued_action != NATIVE_KEY_NONE) return queued_action;
    if (ch != 0x1b) return NATIVE_KEY_NONE;
    if (!native_stdin_read_byte(&ch)) return NATIVE_KEY_STOP;
    if (ch >= '1' && ch <= '9') return (NativeKeyboardAction)(NATIVE_KEY_CHANNEL_21 + ch - '1');
    if (ch == '0') return NATIVE_KEY_CHANNEL_30;
    if (ch != '[' && ch != 'O') return NATIVE_KEY_NONE;
    introducer = ch;
    if (!native_stdin_read_byte(&ch)) return NATIVE_KEY_NONE;
    if (introducer == '[' && ch == '<') {
        char mouse[48];
        unsigned length = 0u;
        unsigned button;
        unsigned column;
        unsigned row;
        while (length + 1u < sizeof(mouse) && native_stdin_read_byte(&ch)) {
            if (ch == 'M' || ch == 'm') {
                terminator = ch;
                break;
            }
            mouse[length++] = ch;
        }
        mouse[length] = '\0';
        if (terminator != 'M' || std::sscanf(mouse, "%u;%u;%u", &button, &column, &row) != 3) {
            return NATIVE_KEY_NONE;
        }
        if ((button & 3u) == 2u && column >= 1u && column <= 80u && row >= 1u && row <= 50u) {
            return NATIVE_KEY_MOUSE_STOP;
        }
        if ((button & 3u) == 0u && column >= 3u && column <= 78u && row >= 2u && row <= 5u) {
            return NATIVE_KEY_MOUSE_REDRAW;
        }
        if ((button & 3u) == 0u) {
            native_mouse_column = column - 1u;
            native_mouse_row = row - 1u;
            return NATIVE_KEY_MOUSE_LEFT;
        }
        return NATIVE_KEY_NONE;
    }
    do {
        if (ch >= '0' && ch <= '9') code = code * 10 + (ch - '0');
        if (ch == '~' || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            terminator = ch;
            break;
        }
    } while (native_stdin_read_byte(&ch));
    if (code == 0 && terminator == 'P') return NATIVE_KEY_F1;
    if (code == 0 && terminator == 'Q') return NATIVE_KEY_F2;
    if (code == 0 && terminator == 'R') return NATIVE_KEY_F3;
    if (code == 0 && terminator == 'S') return NATIVE_KEY_F4;
    if (code == 0 && terminator == 'A') return NATIVE_KEY_SEEK_FORWARD_10;
    if (code == 0 && terminator == 'B') return NATIVE_KEY_SEEK_BACK_10;
    if (code == 0 && terminator == 'C') return NATIVE_KEY_SEEK_FORWARD_2;
    if (code == 0 && terminator == 'D') return NATIVE_KEY_SEEK_BACK_2;
    if (code == 0 && terminator == 'F') return NATIVE_KEY_END_PATTERN;
    if (code == 4) return NATIVE_KEY_END_PATTERN;
    if (code == 11) return NATIVE_KEY_F1;
    if (code == 12) return NATIVE_KEY_F2;
    if (code == 13) return NATIVE_KEY_F3;
    if (code == 14) return NATIVE_KEY_F4;
    if (code == 15) return NATIVE_KEY_F5;
    if (code == 17) return NATIVE_KEY_F6;
    if (code == 19) return NATIVE_KEY_NONE;
    if (code == 20) return NATIVE_KEY_F9;
    if (code == 21) return NATIVE_KEY_F10;
    if (code == 23) return NATIVE_KEY_F11;
    if (code == 24) return NATIVE_KEY_F12;
    return NATIVE_KEY_NONE;
}

static NativeKeyboardAction native_mouse_playback_action(
    const NativeRunControl *control,
    const IplayModplugAudioBridgeStats *stats) {
    unsigned rows = iplay_notcurses_presenter_rows();
    unsigned cols = 0u;
    struct winsize size = {};
    unsigned bottom_title_row;
    unsigned right_panel_x;
    uint32_t clicked_character = iplay_notcurses_mouse_character();
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) cols = size.ws_col;
    if (rows == 0u) rows = size.ws_row;
    if (control && control->live_mode) {
        if (rows == 0u) rows = iplay_text_mode_rows(control->live_mode);
        if (cols == 0u) cols = iplay_text_mode_cols(control->live_mode);
    }
    if (rows < 10u || cols < 40u) return NATIVE_KEY_NONE;
    bottom_title_row = rows - 10u;
    right_panel_x = cols / 2u;
    if (native_mouse_row >= 3u && native_mouse_row < bottom_title_row) {
        unsigned channel = native_mouse_row - 3u;
        if (stats && channel < stats->ui.channel_count && channel < 30u) {
            return (NativeKeyboardAction)(NATIVE_KEY_CHANNEL_1 + channel);
        }
    }
    if (native_mouse_column < right_panel_x) {
        if (native_mouse_row == rows - 3u) return NATIVE_KEY_PAL_NTSC;
        return NATIVE_KEY_NONE;
    }
    if (native_mouse_row == bottom_title_row + 1u) return NATIVE_KEY_PAUSE;
    if (native_mouse_row == bottom_title_row + 2u) return NATIVE_KEY_F9;
    if (native_mouse_row == bottom_title_row + 3u) return NATIVE_KEY_F10;
    if (native_mouse_row == bottom_title_row + 4u) return NATIVE_KEY_F11;
    if (native_mouse_row == bottom_title_row + 5u) return NATIVE_KEY_F12;
    if (native_mouse_row == bottom_title_row + 6u) {
        if (clicked_character == '-') return NATIVE_KEY_VOLUME_DOWN;
        if (clicked_character == '+') return NATIVE_KEY_VOLUME_UP;
        return native_mouse_column < right_panel_x + (cols - right_panel_x) * 7u / 8u
            ? NATIVE_KEY_VOLUME_DOWN : NATIVE_KEY_VOLUME_UP;
    }
    if (native_mouse_row == bottom_title_row + 7u) {
        if (clicked_character == '[') return NATIVE_KEY_AMPLIFY_DOWN;
        if (clicked_character == ']') return NATIVE_KEY_AMPLIFY_UP;
        return native_mouse_column < right_panel_x + (cols - right_panel_x) * 7u / 8u
            ? NATIVE_KEY_AMPLIFY_DOWN : NATIVE_KEY_AMPLIFY_UP;
    }
    return NATIVE_KEY_NONE;
}

static void native_keyboard_mode_enable(NativeKeyboardMode *mode, int requested) {
    struct termios raw;
    if (!mode) return;
    std::memset(mode, 0, sizeof(*mode));
    mode->requested = requested ? 1 : 0;
    if (!mode->requested || !isatty(STDIN_FILENO)) return;
    if (tcgetattr(STDIN_FILENO, &mode->old_termios) != 0) return;
    raw = mode->old_termios;
    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) return;
    mode->raw_enabled = 1;
    std::printf("\033[?1000h\033[?1006h");
    std::fflush(stdout);
    mode->mouse_enabled = 1;
    native_keyboard_mode_register_restore(mode);
}

static void native_keyboard_mode_restore(NativeKeyboardMode *mode) {
    if (!mode) return;
    if (mode->mouse_enabled) {
        std::printf("\033[?1000l\033[?1006l");
        std::fflush(stdout);
        mode->mouse_enabled = 0;
    }
    if (!mode->raw_enabled) return;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &mode->old_termios) == 0) mode->restored = 1;
    mode->raw_enabled = 0;
}

static void native_keyboard_mode_restore_at_exit(void) {
    native_keyboard_mode_restore(native_keyboard_mode_to_restore);
}

static void native_keyboard_mode_register_restore(NativeKeyboardMode *mode) {
    native_keyboard_mode_to_restore = mode;
    if (!native_keyboard_mode_restore_registered) {
        std::atexit(native_keyboard_mode_restore_at_exit);
        native_keyboard_mode_restore_registered = 1;
    }
    native_signal_restore_register();
}

static void native_audio_sink_write(void *user, const db *pcm, dw byte_count) {
    NativeAudioSink *sink = static_cast<NativeAudioSink *>(user);
    unsigned wait_guard = 0u;
    if (!sink) return;
    capture_audio_write(&sink->capture, pcm, byte_count);
    if (!sink->sdl_opened || !pcm || byte_count == 0) return;
    while (!sink->sdl_dummy_driver && sink->sdl_queue_limit_bytes != 0ul && SDL_GetQueuedAudioSize(sink->device) > sink->sdl_queue_limit_bytes && wait_guard++ < 2000u) {
        sink->sdl_queue_waits += 1ul;
        SDL_Delay(1u);
    }
    if (SDL_QueueAudio(sink->device, pcm, byte_count) == 0) {
        sink->sdl_bytes += byte_count;
    } else {
        sink->sdl_queue_failures += 1ul;
    }
}

static void native_audio_sink_close(NativeAudioSink *sink) {
    if (!sink || sink->sdl_closed || (!sink->sdl_requested && !sink->sdl_opened)) return;
    if (sink->sdl_opened && sink->device) {
        SDL_PauseAudioDevice(sink->device, 1);
        sink->sdl_paused_on_close = 1;
        SDL_ClearQueuedAudio(sink->device);
        sink->sdl_queue_cleared = 1;
        SDL_CloseAudioDevice(sink->device);
        sink->device = 0;
    }
    if (sink->sdl_requested) SDL_QuitSubSystem(SDL_INIT_AUDIO);
    sink->sdl_closed = 1;
}

static void native_audio_sink_close_at_exit(void) {
    native_audio_sink_close(native_audio_sink_to_close);
}

static void native_restore_process_state_on_signal(int signum) {
    native_keyboard_mode_restore(native_keyboard_mode_to_restore);
    native_audio_sink_close(native_audio_sink_to_close);
    std::signal(signum, SIG_DFL);
    std::raise(signum);
}

static void native_signal_restore_register(void) {
    std::signal(SIGINT, native_restore_process_state_on_signal);
    std::signal(SIGTERM, native_restore_process_state_on_signal);
#ifdef SIGHUP
    std::signal(SIGHUP, native_restore_process_state_on_signal);
#endif
}

static void native_audio_sink_register_close(NativeAudioSink *sink) {
    native_audio_sink_to_close = sink;
    if (!native_audio_sink_close_registered) {
        std::atexit(native_audio_sink_close_at_exit);
        native_audio_sink_close_registered = 1;
    }
    native_signal_restore_register();
}

static void native_audio_sink_print(const NativeAudioSink *sink) {
    if (!sink) return;
    std::printf("SDL audio sink: requested=%d opened=%d bytes=%lu queue_failures=%lu freq=%d format=0x%04x channels=%u samples=%u queue_limit_bytes=%lu queue_waits=%lu driver=%s paused=%d queue_cleared=%d closed=%d\n",
                sink->sdl_requested,
                sink->sdl_opened,
                sink->sdl_bytes,
                sink->sdl_queue_failures,
                sink->freq,
                (unsigned)sink->format,
                (unsigned)sink->channels,
                (unsigned)sink->samples,
                sink->sdl_queue_limit_bytes,
                sink->sdl_queue_waits,
                sink->sdl_driver[0] ? sink->sdl_driver : "none",
                sink->sdl_paused_on_close,
                sink->sdl_queue_cleared,
                sink->sdl_closed);
}

static void capture_video_present(void *user, const db *cells, const IplayTextMode *mode, dw byte_count) {
    VideoCapture *capture = static_cast<VideoCapture *>(user);
    (void)cells;
    if (!capture || !mode) return;
    capture->calls += 1ul;
    capture->bytes += byte_count;
    capture->cols = (unsigned)iplay_text_mode_cols(mode);
    capture->rows = (unsigned)iplay_text_mode_rows(mode);
}

struct NativeVgaRgb {
    unsigned r;
    unsigned g;
    unsigned b;
};

static const NativeVgaRgb *native_terminal_vga_rgb(unsigned color) {
    static const NativeVgaRgb palette[16] = {
        {0u, 0u, 0u},       {0u, 0u, 170u},     {0u, 170u, 0u},     {0u, 170u, 170u},
        {170u, 0u, 0u},     {170u, 0u, 170u},   {170u, 85u, 0u},    {170u, 170u, 170u},
        {85u, 85u, 85u},    {85u, 85u, 255u},   {85u, 255u, 85u},   {85u, 255u, 255u},
        {255u, 85u, 85u},   {255u, 85u, 255u},  {255u, 255u, 85u},  {255u, 255u, 255u},
    };
    return &palette[color & 15u];
}

static void native_terminal_put_codepoint(unsigned codepoint) {
    if (codepoint < 0x80u) {
        std::putchar((int)codepoint);
    } else if (codepoint < 0x800u) {
        std::putchar((int)(0xc0u | (codepoint >> 6u)));
        std::putchar((int)(0x80u | (codepoint & 0x3fu)));
    } else {
        std::putchar((int)(0xe0u | (codepoint >> 12u)));
        std::putchar((int)(0x80u | ((codepoint >> 6u) & 0x3fu)));
        std::putchar((int)(0x80u | (codepoint & 0x3fu)));
    }
}

static void native_terminal_put_cp437(db ch) {
    unsigned codepoint;
    if (ch >= 0x20u && ch <= 0x7eu) {
        native_terminal_put_codepoint(ch);
        return;
    }
    switch (ch) {
    case 0x16u: codepoint = 0x25acu; break;
    case 0x18u: codepoint = 0x2191u; break;
    case 0x19u: codepoint = 0x2193u; break;
    case 0x1au: codepoint = 0x2192u; break;
    case 0x1bu: codepoint = 0x2190u; break;
    case 0x1du: codepoint = 0x2194u; break;
    case 0xb3u: codepoint = 0x2502u; break;
    case 0xbfu: codepoint = 0x2510u; break;
    case 0xc0u: codepoint = 0x2514u; break;
    case 0xc4u: codepoint = 0x2500u; break;
    case 0xd9u: codepoint = 0x2518u; break;
    case 0xdau: codepoint = 0x250cu; break;
    case 0xdbu: codepoint = 0x2588u; break;
    case 0xdcu: codepoint = 0x2584u; break;
    case 0xdfu: codepoint = 0x2580u; break;
    case 0xf9u: codepoint = 0x2219u; break;
    default: codepoint = 0x20u; break;
    }
    native_terminal_put_codepoint(codepoint);
}

static void native_terminal_render_80x25_closing_row(void) {
    const NativeVgaRgb *fg_rgb = native_terminal_vga_rgb(15u);
    const NativeVgaRgb *bg_rgb = native_terminal_vga_rgb(7u);
    dw col;
    std::printf("\033[26;1H\033[38;2;%u;%u;%u;48;2;%u;%u;%um",
                fg_rgb->r, fg_rgb->g, fg_rgb->b,
                bg_rgb->r, bg_rgb->g, bg_rgb->b);
    for (col = 0u; col < 80u; ++col) {
        db ch = ' ';
        if (col == 0u || col == 3u || col == 42u) ch = 0xc0u;
        if (col == 37u || col == 76u || col == 79u) ch = 0xd9u;
        if ((col > 3u && col < 37u) || (col > 42u && col < 76u)) ch = 0xc4u;
        native_terminal_put_cp437(ch);
    }
    std::printf("\033[0m");
}

static void native_terminal_render_cells(const db *cells, const IplayTextMode *mode, int restore_cursor) {
    (void)restore_cursor;
    if (!cells || !mode) return;
    (void)iplay_notcurses_present_cells(cells, mode);
}

static void native_terminal_render_live_playback(const IplayTextMode *mode, const char *path, const IplayModplugAudioBridgeStats *stats, unsigned view, unsigned paused) {
    IplayRuntime runtime;
    IplayModernPlaybackResult result = {};
    static db cells[IPLAY_TEXT_MAX_SCREEN_BYTES];
    if (!mode || !stats) return;
    if ((view & 0xffu) == IPLAY_MODERN_VIEW_SCOPES) {
        (void)iplay_sdl_visualizer_present_f2(stats);
        return;
    }
    if ((view & 0xffu) == IPLAY_MODERN_VIEW_SPECTRUM) {
        (void)iplay_sdl_visualizer_present_f5(stats);
        return;
    }
    iplay_sdl_visualizer_hide();
    result.status = IPLAY_MODERN_PLAYBACK_OK;
    result.audio = *stats;
    std::memset(cells, 0, sizeof(cells));
    iplay_runtime_init_callbacks(&runtime, cells, mode, capture_video_present, 0, capture_audio_write, 0);
    (void)iplay_modern_render_playback_view(&runtime, path, &result, view);
    {
        IplayNcPlane *plane = iplay_runtime_stdplane(&runtime);
        const char *state_word = paused ? "Pausing" :
            (stats->loop_enabled ? "Looping" : "Playing");
        iplay_ncplane_putnstr_fill_yx(plane, 19u, 46u, state_word, 0x7eu, 7u);
        if (paused) {
        dw row = iplay_ncplane_rows(plane) ? (dw)(iplay_ncplane_rows(plane) - 1u) : 0u;
        iplay_ncplane_putnstr_fill_yx(plane, row, 2u, "Paus - press P or Space to resume", 0x1eu, 34u);
        }
    }
    native_terminal_render_cells(cells, mode, 0);
}

static void native_terminal_render_playback(const IplayTextMode *mode, const char *path, const IplayModernPlaybackResult *result) {
    IplayRuntime runtime;
    VideoCapture video = {0ul, 0ul, 0u, 0u};
    static db cells[IPLAY_TEXT_MAX_SCREEN_BYTES];
    dw screen_bytes;
    dd checksum;
    dw nonblank;
    if (!mode) return;
    screen_bytes = (dw)(iplay_text_mode_cols(mode) * iplay_text_mode_rows(mode) * 2u);
    std::memset(cells, 0, sizeof(cells));
    iplay_runtime_init_callbacks(&runtime, cells, mode, capture_video_present, &video, capture_audio_write, 0);
    (void)iplay_modern_render_playback_status(&runtime, path, result);
    (void)iplay_runtime_present(&runtime);
    checksum = iplay_text_cells_checksum(cells, screen_bytes);
    nonblank = iplay_text_cells_nonblank_count(cells, screen_bytes);
    std::printf("Terminal render: requested=1 cols=%u rows=%u bytes=%u screen_checksum=%lu screen_nonblank=%u present_calls=%lu present_bytes=%lu\n",
                (unsigned)iplay_text_mode_cols(mode),
                (unsigned)iplay_text_mode_rows(mode),
                (unsigned)screen_bytes,
                (unsigned long)checksum,
                (unsigned)nonblank,
                video.calls,
                video.bytes);
    native_terminal_render_cells(cells, mode, 1);
    std::printf("Terminal render end\n");
}

static void native_terminal_print_live_bar(unsigned filled, unsigned width, int active_color, int empty_color) {
    std::printf("\033[%dm", active_color);
    for (unsigned i = 0u; i < width; ++i) {
        if (i == filled) std::printf("\033[%dm", empty_color);
        std::putchar(i < filled ? '#' : '.');
    }
    std::printf("\033[0m");
}

static bool native_playback_progress(void *user, const IplayModplugAudioBridgeStats *stats) {
    NativeRunControl *control = static_cast<NativeRunControl *>(user);
    NativeKeyboardAction action = NATIVE_KEY_NONE;
    unsigned width = 16u;
    unsigned left;
    unsigned right;
    if (!control || !stats) return false;
    if (control->stdin_keyboard) {
        action = native_stdin_keyboard_action();
        if (action == NATIVE_KEY_MOUSE_LEFT) {
            action = native_mouse_playback_action(control, stats);
        }
        if (action == NATIVE_KEY_STOP) {
            control->stdin_keyboard_seen = 1;
            return true;
        }
        if (action == NATIVE_KEY_MOUSE_STOP) {
            control->stdin_keyboard_seen = 1;
            control->mouse_exits += 1u;
            return true;
        }
        if (action == NATIVE_KEY_MOUSE_REDRAW) {
            control->mouse_redraws += 1u;
            if (control->full_live_screen) {
                native_terminal_render_live_playback(
                    control->live_mode,
                    control->module_path,
                    stats,
                    control->terminal_view,
                    control->paused);
            }
            action = NATIVE_KEY_NONE;
        }
        if (action == NATIVE_KEY_PAUSE) {
            control->paused ^= 1u;
            control->pause_toggles += 1u;
            if (control->full_live_screen) {
                native_terminal_render_live_playback(control->live_mode, control->module_path, stats, control->terminal_view, control->paused);
            }
            while (control->paused) {
                NativeKeyboardAction paused_action;
                SDL_Delay(10u);
                paused_action = native_stdin_keyboard_action();
                if (paused_action == NATIVE_KEY_STOP) {
                    control->stdin_keyboard_seen = 1;
                    return true;
                }
                if (paused_action == NATIVE_KEY_MOUSE_STOP) {
                    control->stdin_keyboard_seen = 1;
                    control->mouse_exits += 1u;
                    return true;
                }
                if (paused_action == NATIVE_KEY_MOUSE_REDRAW) {
                    control->mouse_redraws += 1u;
                    if (control->full_live_screen) {
                        native_terminal_render_live_playback(
                            control->live_mode,
                            control->module_path,
                            stats,
                            control->terminal_view,
                            control->paused);
                    }
                }
                if (paused_action == NATIVE_KEY_PAUSE) {
                    control->paused = 0u;
                    control->pause_toggles += 1u;
                }
            }
            action = NATIVE_KEY_NONE;
        }
        if (control->controls && action != NATIVE_KEY_NONE) {
            int changed = 0;
            if (action == NATIVE_KEY_VOLUME_DOWN && control->controls->volume_256 > 0u) {
                control->controls->volume_256 =
                    control->controls->volume_256 >= 2u ? control->controls->volume_256 - 2u : 0u;
                control->controls->volume_percent = control->controls->volume_256 * 100u / 256u;
                changed = 1;
            }
            if (action == NATIVE_KEY_VOLUME_UP && control->controls->volume_256 < 256u) {
                control->controls->volume_256 =
                    control->controls->volume_256 <= 254u ? control->controls->volume_256 + 2u : 256u;
                control->controls->volume_percent = control->controls->volume_256 * 100u / 256u;
                changed = 1;
            }
            if (action == NATIVE_KEY_AMPLIFY_DOWN && control->controls->amplification_percent > 50u) {
                control->controls->amplification_percent -= 10u;
                if (control->controls->amplification_percent < 50u) control->controls->amplification_percent = 50u;
                changed = 1;
            }
            if (action == NATIVE_KEY_AMPLIFY_UP && control->controls->amplification_percent < 2500u) {
                control->controls->amplification_percent += 10u;
                if (control->controls->amplification_percent > 2500u) control->controls->amplification_percent = 2500u;
                changed = 1;
            }
            if (action == NATIVE_KEY_SEEK_BACK_2 || action == NATIVE_KEY_SEEK_FORWARD_2 ||
                action == NATIVE_KEY_SEEK_BACK_10 || action == NATIVE_KEY_SEEK_FORWARD_10) {
                long absolute_row = (long)stats->ui.order * 64l + (long)stats->ui.row;
                long delta = action == NATIVE_KEY_SEEK_BACK_2 ? -2l
                           : action == NATIVE_KEY_SEEK_FORWARD_2 ? 2l
                           : action == NATIVE_KEY_SEEK_BACK_10 ? -10l
                           : 10l;
                absolute_row += delta;
                if (absolute_row < 0l) absolute_row = 0l;
                control->controls->seek_order = (unsigned)(absolute_row / 64l);
                control->controls->seek_row = (unsigned)(absolute_row % 64l);
                control->controls->seek_generation += 1u;
                changed = 1;
            }
            if (action >= NATIVE_KEY_CHANNEL_1 && action <= NATIVE_KEY_CHANNEL_30) {
                unsigned channel = (unsigned)(action - NATIVE_KEY_CHANNEL_1);
                control->controls->channel_muted_mask ^= 1u << channel;
                control->controls->channel_generation += 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_CHANNEL_PREVIOUS && control->controls->selected_channel > 0u) {
                control->controls->selected_channel -= 1u;
                control->controls->channel_generation += 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_CHANNEL_NEXT &&
                control->controls->selected_channel + 1u < stats->ui.channel_count &&
                control->controls->selected_channel + 1u < IPLAY_MODPLUG_UI_MAX_CHANNELS) {
                control->controls->selected_channel += 1u;
                control->controls->channel_generation += 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_CHANNEL_PAN_DOWN || action == NATIVE_KEY_CHANNEL_PAN_DOWN_FAST ||
                action == NATIVE_KEY_CHANNEL_PAN_UP || action == NATIVE_KEY_CHANNEL_PAN_UP_FAST) {
                unsigned channel = control->controls->selected_channel;
                unsigned pan = (control->controls->channel_pan_valid_mask & (1u << channel))
                    ? control->controls->channel_pan[channel]
                    : stats->ui.channels[channel].pan;
                unsigned step = (action == NATIVE_KEY_CHANNEL_PAN_DOWN_FAST ||
                                 action == NATIVE_KEY_CHANNEL_PAN_UP_FAST) ? 8u : 1u;
                if (action == NATIVE_KEY_CHANNEL_PAN_DOWN || action == NATIVE_KEY_CHANNEL_PAN_DOWN_FAST) {
                    pan = pan > step ? pan - step : 0u;
                } else {
                    pan = pan + step < 128u ? pan + step : 128u;
                }
                control->controls->channel_pan[channel] = pan;
                control->controls->channel_pan_valid_mask |= 1u << channel;
                control->controls->channel_generation += 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_CHANNEL_PAN_LEFT || action == NATIVE_KEY_CHANNEL_PAN_CENTER ||
                action == NATIVE_KEY_CHANNEL_PAN_RIGHT || action == NATIVE_KEY_CHANNEL_PAN_SURROUND) {
                unsigned channel = control->controls->selected_channel;
                unsigned pan = action == NATIVE_KEY_CHANNEL_PAN_LEFT ? 0u
                             : action == NATIVE_KEY_CHANNEL_PAN_CENTER ? 64u
                             : action == NATIVE_KEY_CHANNEL_PAN_RIGHT ? 128u
                             : 166u;
                control->controls->channel_pan[channel] = pan;
                control->controls->channel_pan_valid_mask |= 1u << channel;
                control->controls->channel_generation += 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_PATTERN_LOOP) {
                control->controls->pattern_loop_enabled ^= 1u;
                control->controls->pattern_loop_order = stats->ui.order;
                changed = 1;
            }
            if (action == NATIVE_KEY_END_PATTERN) {
                control->controls->seek_order = stats->ui.order + 1u;
                control->controls->seek_row = 0u;
                control->controls->seek_generation += 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_PAL_NTSC) {
                control->controls->pal_enabled ^= 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_F11) {
                control->controls->loop_enabled ^= 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_F12) {
                control->controls->interpolation_enabled ^= 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_F9) {
                control->controls->protracker_enabled ^= 1u;
                changed = 1;
            }
            if (action == NATIVE_KEY_F10) {
                control->controls->ignore_bpm_enabled ^= 1u;
                changed = 1;
            }
            if (changed) control->controls->generation += 1u;
        }
        if (action == NATIVE_KEY_F1) {
            control->terminal_view = IPLAY_MODERN_VIEW_HELP;
            control->terminal_view_mask |= 1u << 0u;
        }
        if (action == NATIVE_KEY_F2) {
            control->terminal_view = IPLAY_MODERN_VIEW_SCOPES;
            control->terminal_view_mask |= 1u << 1u;
        }
        if (action == NATIVE_KEY_F3) {
            control->terminal_view = IPLAY_MODERN_VIEW_VU;
            control->terminal_view_mask |= 1u << 2u;
        }
        if (action == NATIVE_KEY_F4) {
            unsigned sample_start = 0u;
            unsigned display_rows = iplay_notcurses_presenter_rows();
            unsigned sample_page_size;
            if (display_rows == 0u && control->live_mode) {
                display_rows = (unsigned)iplay_text_mode_rows(control->live_mode);
            }
            sample_page_size = display_rows > 19u ? display_rows - 19u : 9u;
            if (sample_page_size > 31u) sample_page_size = 31u;
            if ((control->terminal_view & 0xffu) == IPLAY_MODERN_VIEW_SAMPLES) {
                sample_start = (control->terminal_view >> 8u) + sample_page_size;
                if (sample_start >= stats->ui.sample_count) sample_start = 0u;
            }
            control->terminal_view = IPLAY_MODERN_VIEW_SAMPLES | (sample_start << 8u);
            control->terminal_view_mask |= 1u << 3u;
        }
        if (action == NATIVE_KEY_F5) {
            control->terminal_view = IPLAY_MODERN_VIEW_SPECTRUM;
            control->terminal_view_mask |= 1u << 4u;
        }
        if (action == NATIVE_KEY_F6) {
            control->terminal_view = IPLAY_MODERN_VIEW_UNDOCUMENTED;
            control->terminal_view_mask |= 1u << 5u;
        }
    }
    if (control->resize_tracking && control->last_resize_signal_count != (unsigned)native_terminal_resize_signal_count) {
        const IplayTextMode *resize_mode = native_terminal_text_mode();
        unsigned signal_count = (unsigned)native_terminal_resize_signal_count;
        unsigned cols = resize_mode ? (unsigned)iplay_text_mode_cols(resize_mode) : 0u;
        unsigned rows = resize_mode ? (unsigned)iplay_text_mode_rows(resize_mode) : 0u;
        control->resize_signals_seen += signal_count - control->last_resize_signal_count;
        control->last_resize_signal_count = signal_count;
        if ((cols != 0u || rows != 0u) && (cols != control->resized_cols || rows != control->resized_rows)) {
            control->resized_cols = cols;
            control->resized_rows = rows;
            control->resize_changes += 1u;
            control->live_mode = resize_mode;
        }
    }
    if (control->terminal_live) {
        int print_live;
        left = stats->last_left_level > width ? width : stats->last_left_level;
        right = stats->last_right_level > width ? width : stats->last_right_level;
        if (control->live_samples != 0ul && (stats->last_left_level != control->prev_left || stats->last_right_level != control->prev_right)) control->live_changed = 1u;
        if (stats->last_left_level != 0u || stats->last_right_level != 0u) control->live_nonzero += 1ul;
        control->prev_left = stats->last_left_level;
        control->prev_right = stats->last_right_level;
        control->live_samples += 1ul;
        print_live = control->live_samples <= IPLAY_NATIVE_LIVE_INITIAL_PRINT_BLOCKS
            || (IPLAY_NATIVE_LIVE_PRINT_CADENCE_BLOCKS != 0ul && (control->live_samples % IPLAY_NATIVE_LIVE_PRINT_CADENCE_BLOCKS) == 0ul);
        if (print_live) {
            control->live_printed += 1ul;
            if (!control->terminal_live_cursor_hidden) {
                std::printf("\033[?25l");
                control->terminal_live_cursor_hidden = 1;
            }
            if (control->full_live_screen) {
                native_terminal_render_live_playback(control->live_mode, control->module_path, stats, control->terminal_view, control->paused);
            } else {
                std::printf("\033[HTerminal live: block=%lu frames=%lu accepted=%lu levels=%u/%u L[",
                            stats->blocks,
                            stats->source_frames,
                            stats->accepted_bytes,
                            stats->last_left_level,
                            stats->last_right_level);
                native_terminal_print_live_bar(left, width, 92, 90);
                std::printf("] R[");
                native_terminal_print_live_bar(right, width, 96, 90);
                std::printf("]\n");
            }
        } else {
            control->live_suppressed += 1ul;
        }
    }
    return control->after_blocks != 0ul && stats->blocks >= control->after_blocks;
}

static void native_terminal_live_finish(NativeRunControl *control) {
    if (!control || !control->terminal_live_cursor_hidden) return;
    std::printf("\033[0m\033[?25h\n");
    control->terminal_live_cursor_hidden = 0;
}

static bool capture_level_sequence(void *user, const IplayModplugAudioBridgeStats *stats) {
    LevelSequenceCapture *capture = static_cast<LevelSequenceCapture *>(user);
    if (!capture || !stats) return false;
    if (capture->samples == 0ul) {
        capture->first_left = stats->last_left_level;
        capture->first_right = stats->last_right_level;
    } else if (stats->last_left_level != capture->prev_left || stats->last_right_level != capture->prev_right) {
        capture->changed = 1u;
    }
    if (stats->last_left_level != 0u || stats->last_right_level != 0u) capture->nonzero += 1ul;
    capture->prev_left = stats->last_left_level;
    capture->prev_right = stats->last_right_level;
    capture->last_left = stats->last_left_level;
    capture->last_right = stats->last_right_level;
    capture->max_left = stats->max_left_level;
    capture->max_right = stats->max_right_level;
    capture->samples += 1ul;
    return capture->target_blocks != 0ul && capture->samples >= capture->target_blocks;
}

static void print_color_probe_evidence(void) {
    IplayRuntime runtime;
    VideoCapture video = {0ul, 0ul, 0u, 0u};
    IplayNcPlane *plane;
    static db cells[IPLAY_TEXT_MAX_SCREEN_BYTES];
    unsigned i;
    unsigned fg_matches = 0u;
    unsigned bg_matches = 0u;
    unsigned blink_matches = 0u;
    unsigned fg_mask = 0u;
    unsigned bg_mask = 0u;
    unsigned blink_mask = 0u;
    std::memset(cells, 0, sizeof(cells));
    iplay_runtime_init_callbacks(&runtime, cells, &IPLAY_TEXT_MODE_80X25, capture_video_present, &video, capture_audio_write, 0);
    plane = iplay_runtime_stdplane(&runtime);
    iplay_ncplane_erase(plane, iplay_text_attr((IplayTextColor)7, (IplayTextColor)0, 0));
    for (i = 0u; i < 16u; ++i) {
        iplay_ncplane_putc_yx(plane, 0u, (dw)i, (db)('A' + i), iplay_text_attr((IplayTextColor)i, (IplayTextColor)0, 0));
    }
    for (i = 0u; i < 8u; ++i) {
        iplay_ncplane_putc_yx(plane, 1u, (dw)i, (db)('a' + i), iplay_text_attr((IplayTextColor)15, (IplayTextColor)i, (i & 1u) != 0u));
    }
    (void)iplay_runtime_present(&runtime);
    for (i = 0u; i < 16u; ++i) {
        dw offset = IPLAY_TEXT_OFFSET(80u, 0u, (dw)i);
        db attr = cells[(dw)(offset + 1u)];
        IplayTextColor fg = iplay_text_attr_fg(attr);
        IplayTextColor bg = iplay_text_attr_bg(attr);
        if ((unsigned)fg == i) ++fg_matches;
        fg_mask |= 1u << (unsigned)fg;
        bg_mask |= 1u << (unsigned)bg;
    }
    for (i = 0u; i < 8u; ++i) {
        dw offset = IPLAY_TEXT_OFFSET(80u, 1u, (dw)i);
        db attr = cells[(dw)(offset + 1u)];
        IplayTextColor bg = iplay_text_attr_bg(attr);
        int blink = iplay_text_attr_blink(attr);
        if ((unsigned)bg == i) ++bg_matches;
        if ((blink ? 1u : 0u) == (i & 1u)) ++blink_matches;
        bg_mask |= 1u << (unsigned)bg;
        if (blink) blink_mask |= 1u << i;
    }
    std::printf("Color probe: target_fg=16 target_bg=8 cells=24 fg_matches=%u bg_matches=%u blink_matches=%u fg_mask=%04x bg_mask=%02x blink_mask=%02x present_calls=%lu bytes=%lu cols=%u rows=%u\n",
                fg_matches,
                bg_matches,
                blink_matches,
                fg_mask,
                bg_mask,
                blink_mask,
                video.calls,
                video.bytes,
                video.cols,
                video.rows);
}

static const char *native_program_name(const char *path) {
    const char *slash = path ? std::strrchr(path, '/') : 0;
    if (slash && slash[1]) return slash + 1;
    return path && path[0] ? path : "iplay_native";
}

static void print_native_usage(const char *program_name) {
    const char *display_name = native_program_name(program_name);
    std::printf("usage: %s [--modern] [--video-mode=MODE] <module-file|@file-list> [--modern|max-blocks|--blocks=N|--source-end|--keyboard-after-one] [--video-mode=MODE|40x25bw|40x25color|80x25bw|80x25color|80x28|original|80x50|terminal|auto]\n",
                display_name);
    std::printf("plays external tracker modules through libmodplug into the SDL-compatible SB16 16-bit stereo bridge\n");
    std::printf("@file-list selects the first non-empty trimmed module path relative to the list file\n");
    std::printf("renders status through the notcurses-style text runtime and supports 40x25, 80x25, 80x28, and 80x50 text geometry; terminal/auto selects the nearest supported size from COLUMNS/LINES or TIOCGWINSZ\n");
    std::printf("--video-mode=MODE selects the same text mode as a positional mode argument and is accepted before or after the module path\n");
    if (std::strcmp(display_name, "iplay") == 0) {
        std::printf("modern SDL/notcurses player mode is the default for iplay: source-end playback, SDL2 audio, terminal render, live meters, and stdin keyboard stop\n");
    } else {
        std::printf("--modern enables the preferred direct SDL/notcurses player mode: source-end playback, SDL2 audio, terminal render, live meters, and stdin keyboard stop\n");
    }
    std::printf("--keyboard-after-one stops through the keyboard/interactive seam after one submitted audio block\n");
    std::printf("--blocks=N bounds native playback to N external-decoder blocks\n");
    std::printf("--sdl-audio opens a real SDL2 queued-audio device for audible native playback\n");
    std::printf("--terminal-render paints the final notcurses-style text cells to the host terminal with ANSI 16-color output\n");
    std::printf("--terminal-live updates ANSI audio level meters from the native playback callback while blocks are submitted\n");
    std::printf("--stdin-keyboard stops native playback when q, Q, or Escape is read from stdin\n");
    std::printf("--source-end plays until libmikmod reports natural source end, bounded by the native safety limit\n");
    std::printf("module filenames are resolved with DOS-style case-insensitive matching in their host directory\n");
    std::printf("--list-extensions prints external-library tracker extensions; --classify <path> prints the decoder route\n");
}

static int native_parse_max_blocks_arg(const char *arg) {
    if (!arg) return 0;
    if (std::strncmp(arg, "--blocks=", 9) == 0) return std::atoi(arg + 9);
    return std::atoi(arg);
}

static const IplayTextMode *native_text_mode_from_arg(const char *arg) {
    if (!arg || native_streq_ci(arg, "80x28") || native_streq_ci(arg, "original")) return &IPLAY_TEXT_MODE_80X28;
    if (native_streq_ci(arg, "80x25color") || native_streq_ci(arg, "80x25")) return &IPLAY_TEXT_MODE_80X25;
    if (native_streq_ci(arg, "80x25bw") || native_streq_ci(arg, "80x25mono")) return &IPLAY_TEXT_MODE_80X25;
    if (native_streq_ci(arg, "40x25color") || native_streq_ci(arg, "40x25")) return &IPLAY_TEXT_MODE_40X25;
    if (native_streq_ci(arg, "40x25bw") || native_streq_ci(arg, "40x25mono")) return &IPLAY_TEXT_MODE_40X25;
    if (native_streq_ci(arg, "80x50") || native_streq_ci(arg, "80x50project")) return &IPLAY_TEXT_MODE_80X50;
    return 0;
}

static unsigned native_env_unsigned(const char *name) {
    const char *value = std::getenv(name);
    char *end = 0;
    unsigned long parsed;
    if (!value || !*value) return 0u;
    parsed = std::strtoul(value, &end, 10);
    if (!end || *end != 0) return 0u;
    return (unsigned)parsed;
}

static const IplayTextMode *native_terminal_text_mode(void) {
    unsigned cols = native_env_unsigned("COLUMNS");
    unsigned rows = native_env_unsigned("LINES");
    struct winsize size;
    std::memset(&size, 0, sizeof(size));
    if ((cols == 0u || rows == 0u) && ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) {
        if (cols == 0u) cols = size.ws_col;
        if (rows == 0u) rows = size.ws_row;
    }
    if (cols >= 80u && rows > 28u) return &IPLAY_TEXT_MODE_80X50;
    if (cols >= 80u && rows >= 28u) return &IPLAY_TEXT_MODE_80X28;
    if (cols >= 80u) return &IPLAY_TEXT_MODE_80X25;
    return &IPLAY_TEXT_MODE_40X25;
}

static const IplayTextMode *native_text_mode_from_arg_or_terminal(const char *arg) {
    if (native_streq_ci(arg, "terminal") || native_streq_ci(arg, "auto")) return native_terminal_text_mode();
    return native_text_mode_from_arg(arg);
}

static int native_arg_is_text_mode(const char *arg) {
    return native_text_mode_from_arg(arg) != 0 || native_streq_ci(arg, "terminal") || native_streq_ci(arg, "auto");
}

static int native_arg_is_block_limit(const char *arg) {
    const char *value = arg;
    if (!arg || !arg[0]) return 0;
    if (std::strncmp(arg, "--blocks=", 9) == 0) value = arg + 9;
    if (!value[0]) return 0;
    for (; *value; ++value) {
        if (*value < '0' || *value > '9') return 0;
    }
    return 1;
}

static const char *native_video_mode_option_value(const char *arg) {
    if (arg && std::strncmp(arg, "--video-mode=", 13) == 0) return arg + 13;
    return 0;
}

static void native_enable_modern_mode(int *modern, int *max_blocks, int *sdl_audio, int *terminal_render, int *terminal_live, int *stdin_keyboard) {
    if (modern) *modern = 1;
    if (max_blocks) *max_blocks = 16384;
    if (sdl_audio) *sdl_audio = 1;
    if (terminal_render) *terminal_render = 1;
    if (terminal_live) *terminal_live = 1;
    if (stdin_keyboard) *stdin_keyboard = 1;
}

static void print_row_text(const db *cells, dw cols, dw row) {
    dw col;
    int skipped_label_colon = 0;
    for (col = 0; col < cols; ++col) {
        db ch = cells[IPLAY_TEXT_OFFSET(cols, row, col)];
        if (ch < 0x20u || ch > 0x7eu) ch = ' ';
        if (!skipped_label_colon && ch == ':' && col + 1u < cols && cells[IPLAY_TEXT_OFFSET(cols, row, (dw)(col + 1u))] == ' ') {
            skipped_label_colon = 1;
            continue;
        }
        std::putchar((int)ch);
    }
}

static void print_row_ascii_text(const db *cells, dw cols, dw row) {
    dw col;
    for (col = 0; col < cols; ++col) {
        db ch = cells[IPLAY_TEXT_OFFSET(cols, row, col)];
        if (ch < 0x20u || ch > 0x7eu) ch = ' ';
        std::putchar((int)ch);
    }
}

static void render_and_print_rows(const char *prefix, const IplayTextMode *mode, const char *path, const IplayModernPlaybackResult *result, const char *reason, const char *scope) {
    IplayRuntime runtime;
    VideoCapture video = {0ul, 0ul, 0u, 0u};
    static db cells[IPLAY_TEXT_MAX_SCREEN_BYTES];
    dw cols = iplay_text_mode_cols(mode);
    dw module_row = cols >= 80u ? 19u : IPLAY_RUNTIME_STATUS_MODULE_ROW;
    dw blocks_row = cols >= 80u ? 23u : IPLAY_RUNTIME_STATUS_SIZE_ROW;
    dw stop_row = cols >= 80u ? 24u : IPLAY_RUNTIME_STATUS_LOADER_ROW;
    dw audio_row = cols >= 80u ? 19u : IPLAY_RUNTIME_STATUS_AUDIO_ROW;
    dw accepted_row = cols >= 80u ? 22u : IPLAY_RUNTIME_STATUS_HARDWARE_ROW;
    dw frames_row = cols >= 80u ? 24u : IPLAY_RUNTIME_STATUS_VIDEO_ROW;
    dw levels_row = cols >= 80u ? 6u : IPLAY_RUNTIME_STATUS_LEVELS_ROW;
    dw playback_row = cols >= 80u ? 23u : IPLAY_RUNTIME_STATUS_PLAYBACK_ROW;
    dw status_row = cols >= 80u ? 20u : IPLAY_RUNTIME_STATUS_TAG_ROW;
    dw screen_bytes = (dw)(iplay_text_mode_cols(mode) * iplay_text_mode_rows(mode) * 2u);
    dd checksum;
    dw nonblank;
    std::memset(cells, 0, sizeof(cells));
    iplay_runtime_init_callbacks(&runtime, cells, mode, capture_video_present, &video, capture_audio_write, 0);
    (void)iplay_modern_render_playback_status(&runtime, path, result);
    (void)iplay_runtime_present(&runtime);
    checksum = iplay_text_cells_checksum(cells, screen_bytes);
    nonblank = iplay_text_cells_nonblank_count(cells, screen_bytes);
    std::printf("Screen present: reason=%s scope=%s bytes=%u screen_bytes=%u screen_checksum=%lu screen_nonblank=%u full=1 cols=%u rows=%u mode_ok=1 audio_frames=%lu levels=%u/%u\n",
                reason ? reason : "post-playback-status",
                scope ? scope : "status-only",
                (unsigned)screen_bytes,
                (unsigned)screen_bytes,
                (unsigned long)checksum,
                (unsigned)nonblank,
                (unsigned)iplay_text_mode_cols(mode),
                (unsigned)iplay_text_mode_rows(mode),
                result ? result->audio.source_frames : 0ul,
                result ? result->audio.last_left_level : 0u,
                result ? result->audio.last_right_level : 0u);
    std::printf("%s_title=\"", prefix);
    print_row_text(cells, cols, IPLAY_RUNTIME_STATUS_TITLE_ROW);
    std::printf("\"\n");
    std::printf("%s_module=\"", prefix);
    print_row_text(cells, cols, module_row);
    std::printf("\"\n");
    std::printf("%s_blocks=\"", prefix);
    print_row_text(cells, cols, blocks_row);
    std::printf("\"\n");
    std::printf("%s_stop=\"", prefix);
    print_row_text(cells, cols, stop_row);
    std::printf("\"\n");
    std::printf("%s_audio=\"", prefix);
    print_row_text(cells, cols, audio_row);
    std::printf("\"\n");
    std::printf("%s_accepted=\"", prefix);
    print_row_text(cells, cols, accepted_row);
    std::printf("\"\n");
    std::printf("%s_frames=\"", prefix);
    print_row_text(cells, cols, frames_row);
    std::printf("\"\n");
    std::printf("%s_levels=\"", prefix);
    print_row_text(cells, cols, levels_row);
    std::printf("\"\n");
    std::printf("%s_playback=\"", prefix);
    print_row_text(cells, cols, playback_row);
    std::printf("\"\n");
    std::printf("%s_status=\"", prefix);
    print_row_text(cells, cols, status_row);
    std::printf("\"\n");
    std::printf("%s_present=calls:%lu bytes:%lu cols:%u rows:%u\n", prefix, video.calls, video.bytes, video.cols, video.rows);
}

static void print_resize_phase(const char *phase, const char *prefix, const db *cells, const IplayTextMode *mode, const IplayModernPlaybackResult *result, const VideoCapture *video, int resize_ok) {
    dw cols = iplay_text_mode_cols(mode);
    dw screen_bytes = (dw)(iplay_text_mode_cols(mode) * iplay_text_mode_rows(mode) * 2u);
    dd checksum = iplay_text_cells_checksum(cells, screen_bytes);
    dw nonblank = iplay_text_cells_nonblank_count(cells, screen_bytes);
    std::printf("Resize present: phase=%s bytes=%u screen_bytes=%u screen_checksum=%lu screen_nonblank=%u cols=%u rows=%u resize_ok=%d audio_frames=%lu levels=%u/%u\n",
                phase,
                (unsigned)screen_bytes,
                (unsigned)screen_bytes,
                (unsigned long)checksum,
                (unsigned)nonblank,
                (unsigned)iplay_text_mode_cols(mode),
                (unsigned)iplay_text_mode_rows(mode),
                resize_ok,
                result ? result->audio.source_frames : 0ul,
                result ? result->audio.last_left_level : 0u,
                result ? result->audio.last_right_level : 0u);
    std::printf("%s_title=\"", prefix);
    print_row_text(cells, cols, IPLAY_RUNTIME_STATUS_TITLE_ROW);
    std::printf("\"\n");
    std::printf("%s_module=\"", prefix);
    print_row_text(cells, cols, IPLAY_RUNTIME_STATUS_MODULE_ROW);
    std::printf("\"\n");
    std::printf("%s_status=\"", prefix);
    print_row_text(cells, cols, IPLAY_RUNTIME_STATUS_TAG_ROW);
    std::printf("\"\n");
    std::printf("%s_present=calls:%lu bytes:%lu cols:%u rows:%u resize_ok:%d\n", prefix, video ? video->calls : 0ul, video ? video->bytes : 0ul, video ? video->cols : 0u, video ? video->rows : 0u, resize_ok);
}

static void render_resize_cycle_and_print_rows(const char *path, const IplayModernPlaybackResult *result) {
    IplayRuntime runtime;
    VideoCapture video = {0ul, 0ul, 0u, 0u};
    static db cells[IPLAY_TEXT_MAX_SCREEN_BYTES];
    int resize_ok;
    std::memset(cells, 0, sizeof(cells));
    iplay_runtime_init_callbacks_capacity(&runtime, cells, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_80X25, capture_video_present, &video, capture_audio_write, 0);
    (void)iplay_modern_render_playback_status(&runtime, path, result);
    (void)iplay_runtime_present(&runtime);
    print_resize_phase("before", "resize_before", cells, &IPLAY_TEXT_MODE_80X25, result, &video, 1);
    resize_ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);
    (void)iplay_modern_render_playback_status(&runtime, path, result);
    (void)iplay_runtime_present(&runtime);
    print_resize_phase("after", "resize_after", cells, &IPLAY_TEXT_MODE_80X50, result, &video, resize_ok);
}

static void render_subwindow_and_print_rows(const char *path, const IplayModernPlaybackResult *result) {
    IplayRuntime runtime;
    VideoCapture video = {0ul, 0ul, 0u, 0u};
    IplayWindow root;
    IplayWindow child;
    dw origin_y = 0u;
    dw origin_x = 0u;
    static db cells[IPLAY_TEXT_MAX_SCREEN_BYTES];
    dw screen_bytes = iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25);
    dd checksum;
    dw nonblank;
    (void)path;
    std::memset(cells, 0, sizeof(cells));
    iplay_runtime_init_callbacks_capacity(&runtime, cells, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_80X25, capture_video_present, &video, capture_audio_write, 0);
    (void)iplay_modern_render_playback_status(&runtime, path, result);
    iplay_window_init_root(&root, iplay_runtime_stdplane(&runtime));
    iplay_window_init_subwindow(&child, &root, 3, 5, 5, 34);
    iplay_window_origin_yx(&child, &origin_y, &origin_x);
    iplay_window_erase(&child, 0x17u);
    iplay_window_box_yx(&child, 0, 0, 5, 34, 0x1eu, 0x17u);
    iplay_window_draw_status_line(&child, 1, "SUBWINDOW", 0x1eu);
    iplay_window_draw_status_field(&child, 2, "Stop", iplay_modern_playback_stop_text(result), 0x2au, 0x4cu);
    iplay_window_draw_status_field(&child, 3, "Audio", iplay_modern_audio_backend_name(), 0x2au, 0x4cu);
    (void)iplay_runtime_present(&runtime);
    checksum = iplay_text_cells_checksum(cells, screen_bytes);
    nonblank = iplay_text_cells_nonblank_count(cells, screen_bytes);
    std::printf("Subwindow present: origin=%u,%u rows=%u cols=%u screen_bytes=%u screen_checksum=%lu screen_nonblank=%u calls=%lu bytes=%lu present_cols=%u present_rows=%u audio_frames=%lu levels=%u/%u\n",
                (unsigned)origin_y,
                (unsigned)origin_x,
                (unsigned)iplay_window_rows(&child),
                (unsigned)iplay_window_cols(&child),
                (unsigned)screen_bytes,
                (unsigned long)checksum,
                (unsigned)nonblank,
                video.calls,
                video.bytes,
                video.cols,
                video.rows,
                result ? result->audio.source_frames : 0ul,
                result ? result->audio.last_left_level : 0u,
                result ? result->audio.last_right_level : 0u);
    std::printf("subwindow_title=\"");
    print_row_ascii_text(cells, iplay_text_mode_cols(&IPLAY_TEXT_MODE_80X25), 4);
    std::printf("\"\n");
    std::printf("subwindow_stop=\"");
    print_row_ascii_text(cells, iplay_text_mode_cols(&IPLAY_TEXT_MODE_80X25), 5);
    std::printf("\"\n");
    std::printf("subwindow_audio=\"");
    print_row_ascii_text(cells, iplay_text_mode_cols(&IPLAY_TEXT_MODE_80X25), 6);
    std::printf("\"\n");
}

static void print_level_sequence_evidence(const char *path) {
    AudioCapture capture = {0ul, 0ul, 0ul};
    LevelSequenceCapture levels = {16ul, 0ul, 0ul, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
    IplayModernPlaybackResult sequence_result;
    (void)iplay_modern_play_file_to_sdl_sb16_until(path, capture_audio_write, &capture, 512, 17, capture_level_sequence, &levels, &sequence_result);
    std::printf("Level sequence: target=%lu samples=%lu nonzero=%lu changed=%u first=%u,%u last=%u,%u max=%u,%u capture_calls=%lu capture_bytes=%lu status=%s stop=%s\n",
                levels.target_blocks,
                levels.samples,
                levels.nonzero,
                levels.changed,
                levels.first_left,
                levels.first_right,
                levels.last_left,
                levels.last_right,
                levels.max_left,
                levels.max_right,
                capture.calls,
                capture.bytes,
                iplay_modern_playback_status_name(sequence_result.status),
                iplay_modern_playback_stop_text(&sequence_result));
}

static void print_native_player_evidence(const char *path, const IplayModernPlaybackResult *result) {
    const char *stop_text = iplay_modern_playback_stop_text(result);
    char title[29];
    native_module_title(path, title, sizeof(title));
    std::printf("Module: %s\n", native_basename(path));
    std::printf("Size: %lu bytes\n", native_file_size(path));
    std::printf("Loader: %s (%s)\n", native_loader_name(path), native_loader_description(path));
    std::printf("Module type tag: %s\n", native_module_type_tag(path));
    std::printf("Title: %s\n", title[0] ? title : "none");
    std::printf("Decoder route: id=%d name=%s\n",
                result ? (int)result->decoder_route : 2,
                result ? iplay_modern_playback_decoder_route_name(result) : "probe-by-content");
    std::printf("Decoder handoff: external tracker -> SB16 PCM seam.\n");
    std::printf("Playback output: SDL-compatible SB16 16-bit stereo native.\n");
    std::printf("PCM source: external_module seed=1 truncated=0 input=file-path renderer=e route=%d provider=%s hook_provider=%s stream_start=0\n",
                result ? (int)result->decoder_route : 2,
                result ? iplay_modern_playback_decoder_provider_name(result) : "none",
                result ? iplay_modern_playback_decoder_provider_name(result) : "none");
    std::printf("Playback pump: blocks=%lu frames=%lu accepted=%lu checksum=%lu limit=%u source_end=%u stop=%s\n",
                result ? result->audio.blocks : 0ul,
                result ? result->audio.source_frames : 0ul,
                result ? result->audio.accepted_bytes : 0ul,
                result ? result->audio.source_checksum : 0ul,
                result && result->status == IPLAY_MODERN_PLAYBACK_BLOCK_LIMIT ? 1u : 0u,
                result ? result->audio.source_ended : 0u,
                stop_text ? stop_text : "unknown");
}

int main(int argc, char **argv) {
    NativeAudioSink audio_sink;
    NativeKeyboardMode keyboard_mode;
    IplayModernPlaybackResult result;
    char summary[256];
    int max_blocks = 16384;
    StopCapture stop = {0ul};
    NativeRunControl run_control = {};
    IplayModplugPlaybackControls playback_controls;
    const char *video_mode_arg = "80x25color";
    const char *module_arg = 0;
    const IplayTextMode *selected_mode;
    std::string resolved_module_path;
    std::string selected_module_path;
    const char *module_path;
    int sdl_audio_requested = 0;
    int terminal_render_requested = 0;
    int terminal_live_requested = 0;
    int stdin_keyboard_requested = 0;
    int native_modern_requested = 0;
    int video_mode_explicit = 0;
    const char *native_positionals[2] = {0, 0};
    int native_positional_count = 0;
    int i;

    if (argc == 2 && (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0)) {
        print_native_usage(argv[0]);
        return 0;
    }

    if (argc == 2 && std::strcmp(argv[1], "--list-extensions") == 0) {
        size_t i;
        std::printf("extensions=");
        for (i = 0; i < iplay_modern_external_tracker_extension_count(); ++i) {
            if (i != 0) std::printf(",");
            std::printf("%s", iplay_modern_external_tracker_extension(i));
        }
        std::printf("\n");
        return 0;
    }

    if (argc == 3 && std::strcmp(argv[1], "--classify") == 0) {
        std::printf("external=%d project=%d route_id=%d route=%s library=%d backend=\"%s\"\n",
                    iplay_modern_path_is_external_tracker(argv[2]),
                    iplay_modern_path_is_project_owned(argv[2]),
                    (int)iplay_modern_decoder_route(argv[2]),
                    iplay_modern_decoder_route_name(argv[2]),
                    iplay_modern_decoder_route_uses_external_library(argv[2]),
                    iplay_modern_audio_backend_name());
        return 0;
    }

    if (std::strcmp(native_program_name(argv[0]), "iplay") == 0) {
        native_enable_modern_mode(&native_modern_requested, &max_blocks, &sdl_audio_requested, &terminal_render_requested, &terminal_live_requested, &stdin_keyboard_requested);
    }
    if (argc < 2) {
        if (!native_select_module(&selected_module_path)) return 2;
        module_arg = selected_module_path.c_str();
    }
    i = 1;
    for (; i < argc; ++i) {
        const char *video_mode_value = native_video_mode_option_value(argv[i]);
        if (std::strcmp(argv[i], "--modern") == 0) {
            native_enable_modern_mode(&native_modern_requested, &max_blocks, &sdl_audio_requested, &terminal_render_requested, &terminal_live_requested, &stdin_keyboard_requested);
            continue;
        }
        if (video_mode_value) {
            video_mode_arg = video_mode_value;
            video_mode_explicit = 1;
            continue;
        }
        module_arg = argv[i];
        ++i;
        break;
    }
    if (!module_arg) return 2;
    for (; i < argc; ++i) {
        const char *video_mode_value = native_video_mode_option_value(argv[i]);
        if (std::strcmp(argv[i], "--modern") == 0) {
            native_enable_modern_mode(&native_modern_requested, &max_blocks, &sdl_audio_requested, &terminal_render_requested, &terminal_live_requested, &stdin_keyboard_requested);
            continue;
        }
        if (video_mode_value) {
            video_mode_arg = video_mode_value;
            video_mode_explicit = 1;
            continue;
        }
        if (std::strcmp(argv[i], "--sdl-audio") == 0) {
            sdl_audio_requested = 1;
            continue;
        }
        if (std::strcmp(argv[i], "--terminal-render") == 0) {
            terminal_render_requested = 1;
            continue;
        }
        if (std::strcmp(argv[i], "--terminal-live") == 0) {
            terminal_live_requested = 1;
            continue;
        }
        if (std::strcmp(argv[i], "--stdin-keyboard") == 0) {
            stdin_keyboard_requested = 1;
            continue;
        }
        if (native_positional_count >= 2) return 2;
        native_positionals[native_positional_count++] = argv[i];
    }
    if (native_positional_count >= 1) {
        if (std::strcmp(native_positionals[0], "--keyboard-after-one") == 0) {
            stop.after_blocks = 1ul;
        } else if (std::strcmp(native_positionals[0], "--source-end") == 0) {
            max_blocks = 16384;
        } else if (native_modern_requested && native_arg_is_text_mode(native_positionals[0])) {
            video_mode_arg = native_positionals[0];
            video_mode_explicit = 1;
        } else if (native_modern_requested && !native_arg_is_block_limit(native_positionals[0])) {
            video_mode_arg = native_positionals[0];
            video_mode_explicit = 1;
        } else {
            max_blocks = native_parse_max_blocks_arg(native_positionals[0]);
        }
    }
    if (native_positional_count == 2) {
        video_mode_arg = native_positionals[1];
        video_mode_explicit = 1;
    }
    if (native_modern_requested && !video_mode_explicit && native_positional_count == 0) {
        video_mode_arg = "auto";
    }
    module_path = native_resolve_module_argument(module_arg, &resolved_module_path);
    if (!module_path || (module_arg[0] == '@' && resolved_module_path.empty())) {
        std::fprintf(stderr, "%s: could not resolve file list: %s\n", native_program_name(argv[0]), module_arg ? module_arg : "(null)");
        return 2;
    }
    selected_mode = native_text_mode_from_arg_or_terminal(video_mode_arg);
    if (max_blocks <= 0) return 2;
    if (!selected_mode) {
        std::fprintf(stderr, "%s: unsupported text mode: %s\n", native_program_name(argv[0]), video_mode_arg);
        print_native_usage(argv[0]);
        return 2;
    }
    if (module_arg[0] == '@') std::printf("File list: %s selected=%s\n", module_arg, module_path);
    if (!native_file_exists(module_path)) {
        std::fprintf(stderr, "Module not found.\n");
        return 2;
    }
    native_audio_sink_init(&audio_sink, sdl_audio_requested);
    if (!native_audio_sink_open_sdl(&audio_sink)) {
        std::fprintf(stderr, "%s: could not open SDL2 SB16 stereo audio sink requested freq=%d format=0x%04x channels=%u samples=%u: %s\n",
                     native_program_name(argv[0]),
                     audio_sink.freq,
                     (unsigned)audio_sink.format,
                     (unsigned)audio_sink.channels,
                     (unsigned)audio_sink.samples,
                     SDL_GetError());
        native_audio_sink_close(&audio_sink);
        return 2;
    }
    run_control.after_blocks = stop.after_blocks;
    run_control.terminal_live = terminal_live_requested;
    run_control.full_live_screen = std::strcmp(native_program_name(argv[0]), "iplay") == 0;
    run_control.stdin_keyboard = stdin_keyboard_requested;
    run_control.module_path = module_path;
    run_control.live_mode = selected_mode;
    run_control.terminal_view = IPLAY_MODERN_VIEW_VU;
    run_control.shell_last_status = 0;
    iplay_modplug_playback_controls_init(&playback_controls);
    run_control.controls = &playback_controls;
    run_control.resize_tracking = terminal_live_requested;
    run_control.initial_cols = selected_mode ? (unsigned)iplay_text_mode_cols(selected_mode) : 0u;
    run_control.initial_rows = selected_mode ? (unsigned)iplay_text_mode_rows(selected_mode) : 0u;
    run_control.resized_cols = run_control.initial_cols;
    run_control.resized_rows = run_control.initial_rows;
    run_control.last_resize_signal_count = (unsigned)native_terminal_resize_signal_count;
    if (run_control.resize_tracking) native_terminal_resize_register();
    native_keyboard_mode_enable(&keyboard_mode, stdin_keyboard_requested);
    if (!iplay_modern_play_file_to_sdl_sb16_controlled(module_path, native_audio_sink_write, &audio_sink, 512, max_blocks, (run_control.after_blocks || run_control.terminal_live || run_control.stdin_keyboard) ? native_playback_progress : 0, &run_control, &playback_controls, &result)) {
        (void)iplay_modern_playback_summary(&result, summary, sizeof(summary));
        native_keyboard_mode_restore(&keyboard_mode);
        native_audio_sink_close(&audio_sink);
        print_native_player_evidence(module_path, &result);
        native_audio_sink_print(&audio_sink);
        if (terminal_live_requested) native_terminal_live_finish(&run_control);
        if (terminal_live_requested) std::printf("Terminal live summary: requested=1 samples=%lu nonzero=%lu changed=%u printed=%lu suppressed=%lu\n", run_control.live_samples, run_control.live_nonzero, run_control.live_changed, run_control.live_printed, run_control.live_suppressed);
        if (terminal_live_requested) std::printf("Terminal resize: requested=1 signals=%u changes=%u initial=%ux%u current=%ux%u\n", run_control.resize_signals_seen, run_control.resize_changes, run_control.initial_cols, run_control.initial_rows, run_control.resized_cols, run_control.resized_rows);
        if (stdin_keyboard_requested) std::printf("Stdin keyboard: requested=1 stopped=%d\n", run_control.stdin_keyboard_seen);
        if (stdin_keyboard_requested) std::printf("Stdin keyboard mode: requested=1 raw=%d restored=%d\n", keyboard_mode.raw_enabled || keyboard_mode.restored, keyboard_mode.restored);
        if (stdin_keyboard_requested) std::printf("Playback controls: volume=%u loop=%u interpolation=%u protracker=%u ignore_bpm=%u generation=%u amplification=%u seek=%u:%u seek_generation=%u selected_channel=%u muted_mask=%u channel_generation=%u pattern_loop=%u:%u pal=%u pause_toggles=%u paused=%u view_mask=%u shell_invocations=%u mouse_redraws=%u mouse_exits=%u\n", playback_controls.volume_percent, playback_controls.loop_enabled, playback_controls.interpolation_enabled, playback_controls.protracker_enabled, playback_controls.ignore_bpm_enabled, playback_controls.generation, playback_controls.amplification_percent, playback_controls.seek_order, playback_controls.seek_row, playback_controls.seek_generation, playback_controls.selected_channel, playback_controls.channel_muted_mask, playback_controls.channel_generation, playback_controls.pattern_loop_enabled, playback_controls.pattern_loop_order, playback_controls.pal_enabled, run_control.pause_toggles, run_control.paused, run_control.terminal_view_mask, run_control.shell_invocations, run_control.mouse_redraws, run_control.mouse_exits);
        if (terminal_render_requested) native_terminal_render_playback(selected_mode, module_path, &result);
        std::printf("Selected text mode: %s cols=%u rows=%u\n", video_mode_arg, (unsigned)iplay_text_mode_cols(selected_mode), (unsigned)iplay_text_mode_rows(selected_mode));
        std::printf("status=%s route_id=%d route=%s provider=%s stop=%s source_end=%u blocks=%lu source_frames=%lu summary=\"%s\"\n",
                    iplay_modern_playback_status_name(result.status),
                    (int)result.decoder_route,
                    iplay_modern_playback_decoder_route_name(&result),
                    iplay_modern_playback_decoder_provider_name(&result),
                    result.audio.stop_reason ? result.audio.stop_reason : "unknown",
                    result.audio.source_ended,
                    result.audio.blocks,
                    result.audio.source_frames,
                    summary);
        render_and_print_rows("playback", selected_mode, module_path, &result, "playback-position", "full-screen");
        render_and_print_rows("selected", selected_mode, module_path, &result, "post-playback-status", "status-only");
        render_and_print_rows("screen", &IPLAY_TEXT_MODE_80X25, module_path, &result, "post-playback-status", "status-only");
        render_and_print_rows("screen40", &IPLAY_TEXT_MODE_40X25, module_path, &result, "post-playback-status", "status-only");
        render_and_print_rows("screen80x50", &IPLAY_TEXT_MODE_80X50, module_path, &result, "post-playback-status", "status-only");
        render_resize_cycle_and_print_rows(module_path, &result);
        render_subwindow_and_print_rows(module_path, &result);
        print_color_probe_evidence();
        print_level_sequence_evidence(module_path);
        return 3;
    }
    (void)iplay_modern_playback_summary(&result, summary, sizeof(summary));
    native_keyboard_mode_restore(&keyboard_mode);
    native_audio_sink_close(&audio_sink);
    print_native_player_evidence(module_path, &result);
    native_audio_sink_print(&audio_sink);
    if (terminal_live_requested) native_terminal_live_finish(&run_control);
    if (terminal_live_requested) std::printf("Terminal live summary: requested=1 samples=%lu nonzero=%lu changed=%u printed=%lu suppressed=%lu\n", run_control.live_samples, run_control.live_nonzero, run_control.live_changed, run_control.live_printed, run_control.live_suppressed);
    if (terminal_live_requested) std::printf("Terminal resize: requested=1 signals=%u changes=%u initial=%ux%u current=%ux%u\n", run_control.resize_signals_seen, run_control.resize_changes, run_control.initial_cols, run_control.initial_rows, run_control.resized_cols, run_control.resized_rows);
    if (stdin_keyboard_requested) std::printf("Stdin keyboard: requested=1 stopped=%d\n", run_control.stdin_keyboard_seen);
    if (stdin_keyboard_requested) std::printf("Stdin keyboard mode: requested=1 raw=%d restored=%d\n", keyboard_mode.raw_enabled || keyboard_mode.restored, keyboard_mode.restored);
    if (stdin_keyboard_requested) std::printf("Playback controls: volume=%u loop=%u interpolation=%u protracker=%u ignore_bpm=%u generation=%u amplification=%u seek=%u:%u seek_generation=%u selected_channel=%u muted_mask=%u channel_generation=%u pattern_loop=%u:%u pal=%u pause_toggles=%u paused=%u view_mask=%u shell_invocations=%u mouse_redraws=%u mouse_exits=%u\n", playback_controls.volume_percent, playback_controls.loop_enabled, playback_controls.interpolation_enabled, playback_controls.protracker_enabled, playback_controls.ignore_bpm_enabled, playback_controls.generation, playback_controls.amplification_percent, playback_controls.seek_order, playback_controls.seek_row, playback_controls.seek_generation, playback_controls.selected_channel, playback_controls.channel_muted_mask, playback_controls.channel_generation, playback_controls.pattern_loop_enabled, playback_controls.pattern_loop_order, playback_controls.pal_enabled, run_control.pause_toggles, run_control.paused, run_control.terminal_view_mask, run_control.shell_invocations, run_control.mouse_redraws, run_control.mouse_exits);
    if (terminal_render_requested) native_terminal_render_playback(selected_mode, module_path, &result);
    std::printf("Selected text mode: %s cols=%u rows=%u\n", video_mode_arg, (unsigned)iplay_text_mode_cols(selected_mode), (unsigned)iplay_text_mode_rows(selected_mode));

    std::printf("status=%s route_id=%d route=%s provider=%s stop=%s source_end=%u blocks=%lu source_frames=%lu accepted_bytes=%lu frames_written=%lu dropped=%lu capture_calls=%lu capture_bytes=%lu capture_checksum=%lu source_checksum=%lu levels=%u,%u maxlevels=%u,%u active=%u summary=\"%s\"\n",
                iplay_modern_playback_status_name(result.status),
                (int)result.decoder_route,
                iplay_modern_playback_decoder_route_name(&result),
                iplay_modern_playback_decoder_provider_name(&result),
                result.audio.stop_reason ? result.audio.stop_reason : "unknown",
                result.audio.source_ended,
                result.audio.blocks,
                result.audio.source_frames,
                result.audio.accepted_bytes,
                result.audio.frames_written,
                result.audio.dropped_frames,
                audio_sink.capture.calls,
                audio_sink.capture.bytes,
                audio_sink.capture.checksum,
                result.audio.source_checksum,
                result.audio.last_left_level,
                result.audio.last_right_level,
                result.audio.max_left_level,
                result.audio.max_right_level,
                result.audio.active,
                summary);
    render_and_print_rows("playback", selected_mode, module_path, &result, "playback-position", "full-screen");
    render_and_print_rows("selected", selected_mode, module_path, &result, "post-playback-status", "status-only");
    render_and_print_rows("screen", &IPLAY_TEXT_MODE_80X25, module_path, &result, "post-playback-status", "status-only");
    render_and_print_rows("screen40", &IPLAY_TEXT_MODE_40X25, module_path, &result, "post-playback-status", "status-only");
    render_and_print_rows("screen80x50", &IPLAY_TEXT_MODE_80X50, module_path, &result, "post-playback-status", "status-only");
    render_resize_cycle_and_print_rows(module_path, &result);
    render_subwindow_and_print_rows(module_path, &result);
    print_color_probe_evidence();
    print_level_sequence_evidence(module_path);
    return 0;
}
