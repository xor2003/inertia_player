#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p rewrite/.build

native_link_flags=
case "$(uname -s)" in
  MINGW*|MSYS*) native_link_flags="-static-libgcc -static-libstdc++" ;;
esac

cc -std=c99 -O0 -Wall -Wextra -Irewrite -c rewrite/iplay_rewrite.c -o rewrite/.build/iplay_rewrite_for_modplug_audio.o
c++ -std=c++17 -O0 -Wall -Wextra $(pkg-config --cflags libmodplug sdl2 SDL2_image notcurses) -Irewrite \
  rewrite/modplug_renderer.cpp \
  rewrite/modplug_audio_bridge.cpp \
  rewrite/modern_player.cpp \
  rewrite/notcurses_presenter.cpp \
  rewrite/sdl_visualizer.cpp \
  rewrite/modplug_audio_probe.cpp \
  rewrite/.build/iplay_rewrite_for_modplug_audio.o \
  $(pkg-config --libs libmodplug sdl2 SDL2_image notcurses) \
  $native_link_flags \
  -o rewrite/.build/iplay_modern_host
install_player_atomically() {
  src=$1
  dst=$2
  tmp="${dst}.$$"
  trap 'rm -f "$tmp"' EXIT HUP INT TERM
  cp "$src" "$tmp"
  chmod +x "$tmp"
  mv -f "$tmp" "$dst"
  trap - EXIT HUP INT TERM
}
install_player_atomically rewrite/.build/iplay_modern_host rewrite/.build/iplay_native
install_player_atomically rewrite/.build/iplay_modern_host rewrite/.build/iplay
