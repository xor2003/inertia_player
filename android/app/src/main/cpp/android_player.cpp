#include <SDL.h>
#include <SDL_system.h>
#include <xmp.h>
#include <jni.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace {

constexpr int kAudioRate = 44100;
constexpr int kAudioChannels = 2;
constexpr int kAudioFrames = 1024;
constexpr int kButtonCount = 8;

enum Button {
    kOpen,
    kSeekBack,
    kPause,
    kSeekForward,
    kView,
    kVolumeDown,
    kVolumeUp,
    kQuit
};

std::mutex selected_file_mutex;
std::string selected_file;

extern "C" JNIEXPORT void JNICALL
Java_com_xor2003_inertiaplayer_MainActivity_nativeSetSelectedFile(
    JNIEnv *env, jclass, jstring path) {
    const char *utf8 = env->GetStringUTFChars(path, nullptr);
    if (!utf8) return;
    {
        std::lock_guard<std::mutex> lock(selected_file_mutex);
        selected_file = utf8;
    }
    env->ReleaseStringUTFChars(path, utf8);
}

struct Player {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_AudioDeviceID audio = 0;
    xmp_context module = nullptr;
    bool running = true;
    bool paused = false;
    int view = 0;
    int volume = 80;
    int width = 1280;
    int height = 720;
    std::array<int16_t, kAudioFrames * kAudioChannels> pcm{};
    std::array<float, 64> spectrum{};
};

SDL_Rect button_rect(const Player &p, int index) {
    const int margin = std::max(8, p.width / 100);
    const int gap = std::max(5, p.width / 180);
    const int panel_height = std::max(86, p.height / 7);
    const int button_width = (p.width - margin * 2 - gap * (kButtonCount - 1)) / kButtonCount;
    return {margin + index * (button_width + gap), p.height - panel_height - margin,
            button_width, panel_height};
}

void fill(SDL_Renderer *renderer, const SDL_Rect &rect, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2,
          Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void draw_icon(Player &p, int index, const SDL_Rect &r) {
    const int cx = r.x + r.w / 2;
    const int cy = r.y + r.h / 2;
    const int s = std::min(r.w, r.h) / 4;
    SDL_SetRenderDrawColor(p.renderer, 255, 245, 80, 255);
    if (index == kOpen) {
        SDL_Rect folder{cx - s, cy - s / 2, s * 2, s + s / 2};
        SDL_RenderDrawRect(p.renderer, &folder);
        line(p.renderer, cx - s, cy - s / 2, cx - s / 3, cy - s, 255, 245, 80);
        line(p.renderer, cx - s / 3, cy - s, cx + s / 3, cy - s, 255, 245, 80);
    } else if (index == kSeekBack || index == kSeekForward) {
        const int direction = index == kSeekBack ? -1 : 1;
        for (int offset : {-s / 2, s / 2}) {
            line(p.renderer, cx + direction * offset, cy,
                 cx - direction * (s + offset), cy - s, 255, 245, 80);
            line(p.renderer, cx + direction * offset, cy,
                 cx - direction * (s + offset), cy + s, 255, 245, 80);
        }
    } else if (index == kPause) {
        if (p.paused) {
            line(p.renderer, cx - s, cy - s, cx + s, cy, 255, 245, 80);
            line(p.renderer, cx + s, cy, cx - s, cy + s, 255, 245, 80);
            line(p.renderer, cx - s, cy + s, cx - s, cy - s, 255, 245, 80);
        } else {
            SDL_Rect left{cx - s, cy - s, s / 2, s * 2};
            SDL_Rect right{cx + s / 2, cy - s, s / 2, s * 2};
            SDL_RenderFillRect(p.renderer, &left);
            SDL_RenderFillRect(p.renderer, &right);
        }
    } else if (index == kView) {
        for (int i = -2; i <= 2; ++i) {
            const int h = (std::abs(i) + 1) * s / 2;
            line(p.renderer, cx + i * s / 2, cy - h, cx + i * s / 2, cy + h,
                 80, 255, 110);
        }
    } else if (index == kVolumeDown || index == kVolumeUp) {
        line(p.renderer, cx - s, cy, cx + s, cy, 255, 245, 80);
        if (index == kVolumeUp)
            line(p.renderer, cx, cy - s, cx, cy + s, 255, 245, 80);
    } else {
        line(p.renderer, cx - s, cy - s, cx + s, cy + s, 255, 90, 90);
        line(p.renderer, cx + s, cy - s, cx - s, cy + s, 255, 90, 90);
    }
}

void update_spectrum(Player &p) {
    for (std::size_t band = 0; band < p.spectrum.size(); ++band) {
        float energy = 0.0f;
        const std::size_t begin = band * kAudioFrames / p.spectrum.size();
        const std::size_t end = (band + 1) * kAudioFrames / p.spectrum.size();
        for (std::size_t i = begin; i < end; ++i)
            energy += std::abs((int)p.pcm[i * 2]);
        energy /= std::max<std::size_t>(1, end - begin);
        p.spectrum[band] = p.spectrum[band] * 0.72f + energy * 0.28f;
    }
}

void queue_audio(Player &p) {
    if (p.paused || SDL_GetQueuedAudioSize(p.audio) > p.pcm.size() * sizeof(int16_t) * 4)
        return;
    const int result = xmp_play_buffer(
        p.module, p.pcm.data(), (int)(p.pcm.size() * sizeof(int16_t)), 0);
    if (result == -XMP_END) {
        xmp_restart_module(p.module);
        return;
    }
    if (result == 0) {
        update_spectrum(p);
        SDL_QueueAudio(p.audio, p.pcm.data(), (Uint32)(p.pcm.size() * sizeof(int16_t)));
    }
}

void render(Player &p) {
    SDL_GetRendererOutputSize(p.renderer, &p.width, &p.height);
    SDL_SetRenderDrawColor(p.renderer, 176, 176, 176, 255);
    SDL_RenderClear(p.renderer);

    const int controls_top = button_rect(p, 0).y - 12;
    if (p.view == 0) {
        constexpr int scopes = 10;
        const int top = 18;
        const int lane_height = std::max(8, (controls_top - top) / scopes);
        for (int scope = 0; scope < scopes; ++scope) {
            const int center = top + scope * lane_height + lane_height / 2;
            line(p.renderer, 12, center, p.width - 12, center, 105, 105, 105);
            int previous_x = 12;
            int previous_y = center;
            const int channel = scope & 1;
            const int phase = scope * 37;
            for (int x = 13; x < p.width - 12; x += 2) {
                const int sample = ((x - 12) * kAudioFrames / std::max(1, p.width - 24) + phase)
                    % kAudioFrames;
                const int amplitude = p.pcm[(std::size_t)sample * 2u + channel];
                const int y = center - amplitude * (lane_height - 4) / 65536;
                line(p.renderer, previous_x, previous_y, x, y, 70, 255, 90);
                previous_x = x;
                previous_y = y;
            }
        }
    } else {
        const int bands = (int)p.spectrum.size();
        const int gap = std::max(2, p.width / 400);
        const int bar_width = std::max(2, (p.width - 32 - gap * bands) / bands);
        for (int i = 0; i < bands; ++i) {
            const float value = p.spectrum[(std::size_t)i];
            const int h = std::min(controls_top - 50,
                (int)(value * (controls_top - 50) / 26000.0f));
            SDL_Rect green{16 + i * (bar_width + gap), controls_top - h, bar_width, h};
            fill(p.renderer, green, 70, 255, 90);
            if (h > controls_top * 2 / 3) {
                SDL_Rect peak{green.x, green.y, green.w, std::max(3, p.height / 100)};
                fill(p.renderer, peak, 255, 80, 80);
            }
        }
    }

    const int volume_width = p.width * p.volume / 100;
    SDL_Rect volume{0, 0, volume_width, std::max(5, p.height / 100)};
    fill(p.renderer, volume, 255, 245, 80);

    for (int i = 0; i < kButtonCount; ++i) {
        SDL_Rect rect = button_rect(p, i);
        fill(p.renderer, rect, i == kQuit ? 110 : 35, 45, i == kQuit ? 45 : 70);
        SDL_SetRenderDrawColor(p.renderer, 235, 235, 235, 255);
        SDL_RenderDrawRect(p.renderer, &rect);
        draw_icon(p, i, rect);
    }
    SDL_RenderPresent(p.renderer);
}

void seek(Player &p, int delta) {
    xmp_frame_info info{};
    xmp_get_frame_info(p.module, &info);
    xmp_seek_time(p.module, std::max(0, info.time + delta));
    SDL_ClearQueuedAudio(p.audio);
}

void request_file_picker() {
    JNIEnv *env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
    jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
    if (!env || !activity) return;
    jclass type = env->GetObjectClass(activity);
    jmethodID method = env->GetStaticMethodID(type, "openModulePicker", "()V");
    if (method) env->CallStaticVoidMethod(type, method);
    env->DeleteLocalRef(type);
    env->DeleteLocalRef(activity);
}

void activate(Player &p, int button) {
    switch (button) {
    case kOpen: request_file_picker(); break;
    case kSeekBack: seek(p, -10000); break;
    case kPause:
        p.paused = !p.paused;
        SDL_PauseAudioDevice(p.audio, p.paused ? 1 : 0);
        break;
    case kSeekForward: seek(p, 10000); break;
    case kView: p.view = (p.view + 1) % 2; break;
    case kVolumeDown:
        p.volume = std::max(0, p.volume - 10);
        xmp_set_player(p.module, XMP_PLAYER_VOLUME, p.volume);
        break;
    case kVolumeUp:
        p.volume = std::min(100, p.volume + 10);
        xmp_set_player(p.module, XMP_PLAYER_VOLUME, p.volume);
        break;
    case kQuit: p.running = false; break;
    }
}

void touch(Player &p, float normalized_x, float normalized_y) {
    const int x = (int)(normalized_x * p.width);
    const int y = (int)(normalized_y * p.height);
    for (int i = 0; i < kButtonCount; ++i) {
        const SDL_Rect rect = button_rect(p, i);
        if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h) {
            activate(p, i);
            return;
        }
    }
}

bool load_asset(xmp_context context, const char *name) {
    SDL_RWops *rw = SDL_RWFromFile(name, "rb");
    if (!rw) return false;
    const Sint64 size = SDL_RWsize(rw);
    if (size <= 0) {
        SDL_RWclose(rw);
        return false;
    }
    std::vector<uint8_t> bytes((std::size_t)size);
    const bool read = SDL_RWread(rw, bytes.data(), 1, bytes.size()) == bytes.size();
    SDL_RWclose(rw);
    return read && xmp_load_module_from_memory(context, bytes.data(), (long)bytes.size()) == 0;
}

bool load_selected_module(Player &p, const std::string &path) {
    xmp_context next = xmp_create_context();
    if (!next || xmp_load_module(next, path.c_str()) != 0 ||
            xmp_start_player(next, kAudioRate, 0) != 0) {
        if (next) xmp_free_context(next);
        return false;
    }
    xmp_set_player(next, XMP_PLAYER_VOLUME, p.volume);
    SDL_ClearQueuedAudio(p.audio);
    xmp_end_player(p.module);
    xmp_release_module(p.module);
    xmp_free_context(p.module);
    p.module = next;
    p.paused = false;
    SDL_PauseAudioDevice(p.audio, 0);
    return true;
}

void apply_pending_selection(Player &p) {
    std::string path;
    {
        std::lock_guard<std::mutex> lock(selected_file_mutex);
        path.swap(selected_file);
    }
    if (!path.empty()) load_selected_module(p, path);
}

} // namespace

int main(int, char **) {
    Player p;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) return 1;
    p.window = SDL_CreateWindow("Inertia Player", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, p.width, p.height,
        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE);
    p.renderer = SDL_CreateRenderer(p.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!p.renderer) p.renderer = SDL_CreateRenderer(p.window, -1, SDL_RENDERER_SOFTWARE);

    p.module = xmp_create_context();
    if (!p.window || !p.renderer || !p.module || !load_asset(p.module, "HACKER4.S3M"))
        return 2;
    if (xmp_start_player(p.module, kAudioRate, 0) != 0) return 3;
    xmp_set_player(p.module, XMP_PLAYER_VOLUME, p.volume);

    SDL_AudioSpec desired{};
    desired.freq = kAudioRate;
    desired.format = AUDIO_S16SYS;
    desired.channels = kAudioChannels;
    desired.samples = kAudioFrames;
    p.audio = SDL_OpenAudioDevice(nullptr, 0, &desired, nullptr, 0);
    if (!p.audio) return 4;
    SDL_PauseAudioDevice(p.audio, 0);

    while (p.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) p.running = false;
            else if (event.type == SDL_FINGERDOWN)
                touch(p, event.tfinger.x, event.tfinger.y);
            else if (event.type == SDL_MOUSEBUTTONDOWN)
                touch(p, (float)event.button.x / p.width, (float)event.button.y / p.height);
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                p.running = false;
        }
        apply_pending_selection(p);
        queue_audio(p);
        render(p);
        SDL_Delay(5);
    }

    SDL_CloseAudioDevice(p.audio);
    xmp_end_player(p.module);
    xmp_release_module(p.module);
    xmp_free_context(p.module);
    SDL_DestroyRenderer(p.renderer);
    SDL_DestroyWindow(p.window);
    SDL_Quit();
    return 0;
}
