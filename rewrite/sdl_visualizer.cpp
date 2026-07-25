#include "sdl_visualizer.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>

struct IplaySdlVisualizer {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int width;
    int height;
    std::vector<Uint32> pixels;
    std::vector<Uint32> f2_background;
    int f2_view_x;
    int f2_view_y;
    int f2_view_width;
    int f2_view_height;
    unsigned f2_lane_count;
    int f2_lane_x[IPLAY_MODPLUG_UI_MAX_CHANNELS];
    int f2_lane_center[IPLAY_MODPLUG_UI_MAX_CHANNELS];
    unsigned f5_peaks[2][99];
    unsigned f5_peak_ticks[2][99];
};

static IplaySdlVisualizer visualizer = {};

static Uint32 rgb(unsigned r, unsigned g, unsigned b) {
    return 0xff000000u | (r << 16u) | (g << 8u) | b;
}

static void fit_aspect(int source_width,
                       int source_height,
                       int target_width,
                       int target_height,
                       int &view_x,
                       int &view_y,
                       int &view_width,
                       int &view_height) {
    view_width = target_width;
    view_height = target_width * source_height / source_width;
    if (view_height > target_height) {
        view_height = target_height;
        view_width = target_height * source_width / source_height;
    }
    view_x = (target_width - view_width) / 2;
    view_y = (target_height - view_height) / 2;
}

static std::vector<Uint32> scale_pixels(const std::vector<Uint32> &source,
                                        int source_width,
                                        int source_height,
                                        int target_width,
                                        int target_height,
                                        int view_x,
                                        int view_y,
                                        int view_width,
                                        int view_height) {
    std::vector<Uint32> target(
        (std::size_t)target_width * (std::size_t)target_height,
        rgb(0u, 0u, 0u));
    for (int y = 0; y < view_height; ++y) {
        int source_y = y * source_height / view_height;
        for (int x = 0; x < view_width; ++x) {
            int source_x = x * source_width / view_width;
            target[(std::size_t)(view_y + y) * (std::size_t)target_width
                + (std::size_t)(view_x + x)] =
                source[(std::size_t)source_y * (std::size_t)source_width + (std::size_t)source_x];
        }
    }
    return target;
}

static void put_pixel(int x, int y, Uint32 color) {
    if (x < 0 || y < 0 || x >= visualizer.width || y >= visualizer.height) return;
    visualizer.pixels[(std::size_t)y * (std::size_t)visualizer.width + (std::size_t)x] = color;
}

static void line(int x0, int y0, int x1, int y1, Uint32 color) {
    int dx = std::abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;
    for (;;) {
        put_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int doubled = error * 2;
        if (doubled >= dy) {
            error += dy;
            x0 += sx;
        }
        if (doubled <= dx) {
            error += dx;
            y0 += sy;
        }
    }
}

static bool load_f2_background(std::vector<Uint32> &pixels, int &width, int &height) {
    const char *override_path = std::getenv("IPLAY_F2_BACKGROUND");
    static const char *paths[] = {
        "rewrite/assets/iplay.png",
        "assets/iplay.png"
    };
    SDL_Surface *loaded = 0;
    SDL_Surface *converted;
    if (override_path && override_path[0]) loaded = IMG_Load(override_path);
    for (const char *path : paths) {
        if (loaded) break;
        loaded = IMG_Load(path);
    }
    if (!loaded) return false;
    converted = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(loaded);
    if (!converted || converted->w <= 0 || converted->h <= 0) {
        if (converted) SDL_FreeSurface(converted);
        return false;
    }
    width = converted->w;
    height = converted->h;
    pixels.resize((std::size_t)width * (std::size_t)height);
    for (int y = 0; y < height; ++y) {
        std::memcpy(
            pixels.data() + (std::size_t)y * (std::size_t)width,
            static_cast<const Uint8 *>(converted->pixels) + (std::size_t)y * (std::size_t)converted->pitch,
            (std::size_t)width * sizeof(Uint32));
    }
    SDL_FreeSurface(converted);
    return true;
}

static bool ensure_visualizer(int width, int height, const char *title) {
    if ((SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO) == 0u && SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) return false;
    if (visualizer.window && visualizer.width == width && visualizer.height == height) {
        SDL_SetWindowTitle(visualizer.window, title);
        SDL_ShowWindow(visualizer.window);
        return true;
    }
    iplay_sdl_visualizer_stop();
    visualizer.window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width * (width <= 640 ? 2 : 1),
        height * (height <= 480 ? 2 : 1),
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!visualizer.window) return false;
    visualizer.renderer = SDL_CreateRenderer(visualizer.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!visualizer.renderer) visualizer.renderer = SDL_CreateRenderer(visualizer.window, -1, SDL_RENDERER_SOFTWARE);
    if (!visualizer.renderer) {
        iplay_sdl_visualizer_stop();
        return false;
    }
    visualizer.texture = SDL_CreateTexture(
        visualizer.renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height);
    if (!visualizer.texture) {
        iplay_sdl_visualizer_stop();
        return false;
    }
    visualizer.width = width;
    visualizer.height = height;
    visualizer.pixels.resize((std::size_t)width * (std::size_t)height);
    SDL_RenderSetLogicalSize(visualizer.renderer, width, height);
    return true;
}

static bool present_pixels(void) {
    if (SDL_UpdateTexture(visualizer.texture, 0, visualizer.pixels.data(), visualizer.width * (int)sizeof(Uint32)) != 0) return false;
    if (SDL_RenderClear(visualizer.renderer) != 0) return false;
    if (SDL_RenderCopy(visualizer.renderer, visualizer.texture, 0, 0) != 0) return false;
    SDL_RenderPresent(visualizer.renderer);
    return true;
}

bool iplay_sdl_visualizer_present_f2(const IplayModplugAudioBridgeStats *stats) {
    static const Uint32 trace = 0xff00aaaau;
    if (!stats) return false;
    if (visualizer.f2_background.empty()) {
        std::vector<Uint32> background;
        int width = 0;
        int height = 0;
        SDL_DisplayMode desktop = {};
        int render_width;
        int render_height;
        if (!load_f2_background(background, width, height)) return false;
        render_width = width;
        render_height = height;
        if ((SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO) == 0u
            && SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
            return false;
        }
        if (SDL_GetDesktopDisplayMode(0, &desktop) == 0 && desktop.w > 0 && desktop.h > 0) {
            render_width = desktop.w;
            render_height = desktop.h;
        }
        if (!ensure_visualizer(render_width, render_height, "Inertia Player - F2 Graphical Scopes")) {
            return false;
        }
        fit_aspect(
            width,
            height,
            render_width,
            render_height,
            visualizer.f2_view_x,
            visualizer.f2_view_y,
            visualizer.f2_view_width,
            visualizer.f2_view_height);
        visualizer.f2_background = scale_pixels(
            background,
            width,
            height,
            render_width,
            render_height,
            visualizer.f2_view_x,
            visualizer.f2_view_y,
            visualizer.f2_view_width,
            visualizer.f2_view_height);
    } else if (!ensure_visualizer(visualizer.width, visualizer.height, "Inertia Player - F2 Graphical Scopes")) {
        return false;
    }
    visualizer.pixels = visualizer.f2_background;
    const unsigned requested_lane_count = std::min(
        stats->ui.channel_count,
        (unsigned)IPLAY_MODPLUG_UI_MAX_CHANNELS);
    if (visualizer.f2_lane_count != requested_lane_count) {
        unsigned left_count;
        unsigned right_count;
        unsigned left_index = 0u;
        unsigned right_index = 0u;
        unsigned spacing_count;
        visualizer.f2_lane_count = requested_lane_count;
        left_count = (visualizer.f2_lane_count + 1u) / 2u;
        right_count = visualizer.f2_lane_count / 2u;
        spacing_count = std::max(left_count, right_count);
        if (spacing_count == 0u) spacing_count = 1u;
        for (unsigned channel = 0u; channel < visualizer.f2_lane_count; ++channel) {
            const bool left = (channel & 1u) == 0u;
            unsigned slot = left ? left_index++ : right_index++;
            int original_x = left ? 8 : 336;
            int original_center = (int)(((slot * 2u + 1u) * 280u) / (spacing_count * 2u));
            visualizer.f2_lane_x[channel] =
                visualizer.f2_view_x + original_x * visualizer.f2_view_width / 640;
            visualizer.f2_lane_center[channel] =
                visualizer.f2_view_y + original_center * visualizer.f2_view_height / 480;
        }
    }
    for (unsigned channel = 0u; channel < visualizer.f2_lane_count; ++channel) {
        int start_x = visualizer.f2_lane_x[channel];
        int center = visualizer.f2_lane_center[channel];
        int trace_width = std::max(1, 296 * visualizer.f2_view_width / 640);
        int previous_x = start_x;
        unsigned level = stats->ui.channels[channel].level;
        unsigned phase = channel * 7u;
        int previous_sample = (int)stats->scope[phase % IPLAY_MODPLUG_SCOPE_SAMPLES];
        int previous_y = center - ((previous_sample * (int)(level + 4u)) / 42)
            * visualizer.f2_view_height / 480;
        for (int x = start_x + 1; x < start_x + trace_width; ++x) {
            unsigned position = (unsigned)((x - start_x) * (int)IPLAY_MODPLUG_SCOPE_SAMPLES);
            unsigned index = position / (unsigned)trace_width;
            unsigned fraction = position % (unsigned)trace_width;
            int sample0 = (int)stats->scope[(index + phase) % IPLAY_MODPLUG_SCOPE_SAMPLES];
            int sample1 = (int)stats->scope[(index + phase + 1u) % IPLAY_MODPLUG_SCOPE_SAMPLES];
            int sample = (sample0 * (int)((unsigned)trace_width - fraction)
                + sample1 * (int)fraction) / trace_width;
            int y = center - ((sample * (int)(level + 4u)) / 42)
                * visualizer.f2_view_height / 480;
            line(previous_x, previous_y, x, y, trace);
            previous_x = x;
            previous_y = y;
        }
    }
    return present_pixels();
}

bool iplay_sdl_visualizer_present_f5(const IplayModplugAudioBridgeStats *stats) {
    static const Uint32 background = rgb(0u, 0u, 0u);
    if (!stats || !ensure_visualizer(320, 200, "Inertia Player - F5 Frequency Analysis")) return false;
    std::fill(visualizer.pixels.begin(), visualizer.pixels.end(), background);
    for (unsigned half = 0u; half < 2u; ++half) {
        int baseline = half == 0u ? 99 : 199;
        unsigned level = half == 0u ? stats->last_left_level : stats->last_right_level;
        unsigned level_height = std::min(90u, (level * 90u) / 16u);

        /* spectr_1BCE9: separate 8-pixel blue stereo level at x=4. */
        for (unsigned y = 0u; y < level_height; ++y) {
            Uint32 color = (y & 1u) ? rgb(0u, 0u, 0u) : rgb(0u, 96u, 224u);
            for (int x = 4; x < 12; ++x) put_pixel(x, baseline - (int)y, color);
        }

        /* spectr_1BC2D: 99 two-pixel spectrum bins beginning at x=22. */
        for (unsigned bin = 0u; bin < 99u; ++bin) {
            unsigned first = ((bin + 1u) * IPLAY_MODPLUG_SPECTRUM_BANDS) / 100u;
            unsigned end = ((bin + 2u) * IPLAY_MODPLUG_SPECTRUM_BANDS) / 100u;
            unsigned magnitude = 0u;
            if (end <= first) end = first + 1u;
            for (unsigned band = first; band < end && band < IPLAY_MODPLUG_SPECTRUM_BANDS; ++band) {
                magnitude = std::max(magnitude, (unsigned)stats->spectrum[band]);
            }
            unsigned height = std::min(90u, (magnitude * 90u) / IPLAY_MODPLUG_SPECTRUM_MAX_LEVEL);
            if (height >= visualizer.f5_peaks[half][bin]) {
                visualizer.f5_peaks[half][bin] = height;
                visualizer.f5_peak_ticks[half][bin] = 20u;
            } else if (visualizer.f5_peak_ticks[half][bin] > 0u) {
                --visualizer.f5_peak_ticks[half][bin];
            } else if (visualizer.f5_peaks[half][bin] > 0u) {
                --visualizer.f5_peaks[half][bin];
            }
            int x0 = 22 + (int)bin * 3;
            for (unsigned y = 0u; y < height; ++y) {
                unsigned ramp = (y * 63u) / 90u;
                Uint32 color = ramp < 32u
                    ? rgb(ramp * 8u, 255u, 0u)
                    : rgb(255u, (63u - ramp) * 8u, 0u);
                put_pixel(x0, baseline - (int)y, color);
                put_pixel(x0 + 1, baseline - (int)y, color);
            }
            if (visualizer.f5_peaks[half][bin] > 0u) {
                int peak_y = baseline - (int)visualizer.f5_peaks[half][bin];
                put_pixel(x0, peak_y, rgb(254u, 254u, 254u));
                put_pixel(x0 + 1, peak_y, rgb(254u, 254u, 254u));
            }
        }
    }
    return present_pixels();
}

int iplay_sdl_visualizer_poll_key(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return -1;
        if (event.type != SDL_KEYDOWN) continue;
        if (event.key.keysym.sym == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT) != 0) {
            Uint32 flags = SDL_GetWindowFlags(visualizer.window);
            Uint32 mode = (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0u
                ? 0u
                : SDL_WINDOW_FULLSCREEN_DESKTOP;
            (void)SDL_SetWindowFullscreen(visualizer.window, mode);
            continue;
        }
        if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) return -1;
        if (event.key.keysym.sym >= SDLK_F1 && event.key.keysym.sym <= SDLK_F12) {
            return 1 + (int)(event.key.keysym.sym - SDLK_F1);
        }
        if (event.key.keysym.sym == SDLK_LEFT) return 13;
        if (event.key.keysym.sym == SDLK_RIGHT) return 14;
        if (event.key.keysym.sym == SDLK_UP) return 15;
        if (event.key.keysym.sym == SDLK_DOWN) return 16;
    }
    return 0;
}

void iplay_sdl_visualizer_hide(void) {
    if (visualizer.window) SDL_HideWindow(visualizer.window);
}

void iplay_sdl_visualizer_stop(void) {
    if (visualizer.texture) SDL_DestroyTexture(visualizer.texture);
    if (visualizer.renderer) SDL_DestroyRenderer(visualizer.renderer);
    if (visualizer.window) SDL_DestroyWindow(visualizer.window);
    visualizer = {};
}
