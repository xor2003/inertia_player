#ifndef IPLAY_TEST_STUB_SDL_H
#define IPLAY_TEST_STUB_SDL_H

#include <stdint.h>
#include <string.h>

#define SDL_MAJOR_VERSION 1
#define SDL_INIT_AUDIO 0x00000010u
#define SDL_INIT_TIMER 0x00000001u
#define AUDIO_U8 0x0008
#define SDL_MIX_MAXVOLUME 128

typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;

typedef struct SDL_AudioSpec {
    int freq;
    Uint16 format;
    Uint8 channels;
    Uint8 silence;
    Uint16 samples;
    void (*callback)(void *userdata, Uint8 *stream, int len);
    void *userdata;
} SDL_AudioSpec;

static inline int SDL_Init(Uint32 flags) { (void)flags; return 0; }
static inline const char *SDL_GetError(void) { return "SDL test stub"; }
static inline int SDL_OpenAudio(SDL_AudioSpec *wanted, SDL_AudioSpec *obtained) {
    if (obtained && wanted) {
        *obtained = *wanted;
    }
    return 0;
}
static inline void SDL_PauseAudio(int pause_on) { (void)pause_on; }
static inline void SDL_Delay(Uint32 ms) { (void)ms; }
static inline void SDL_memset(void *dst, int c, size_t n) { memset(dst, c, n); }
static inline void SDL_MixAudio(Uint8 *dst, const Uint8 *src, Uint32 len, int volume) {
    (void)volume;
    if (dst && src) {
        memcpy(dst, src, len);
    }
}

#endif
