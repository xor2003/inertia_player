#ifndef IPLAY_SDL_VISUALIZER_HPP
#define IPLAY_SDL_VISUALIZER_HPP

#include "modplug_audio_bridge.hpp"

bool iplay_sdl_visualizer_present_f2(const IplayModplugAudioBridgeStats *stats);
bool iplay_sdl_visualizer_present_f5(const IplayModplugAudioBridgeStats *stats);
int iplay_sdl_visualizer_poll_key(void);
void iplay_sdl_visualizer_hide(void);
void iplay_sdl_visualizer_stop(void);

#endif
