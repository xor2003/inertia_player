#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."

run_log=
launcher_display=${IPLAY_LAUNCHER_DISPLAY:-./rewrite/iplay.sh}
cleanup_run_log() {
  if [ -n "$run_log" ]; then
    rm -f "$run_log"
  fi
}
restore_terminal() {
  if [ -t 0 ] && [ -e /dev/tty ]; then
    stty sane </dev/tty 2>/dev/null || true
    tput sgr0 >/dev/tty 2>/dev/null || true
    tput cnorm >/dev/tty 2>/dev/null || true
  fi
}
cleanup_launcher() {
  cleanup_run_log
  restore_terminal
}
exit_after_signal() {
  exit "$1"
}
trap cleanup_launcher EXIT
trap 'exit_after_signal 129' HUP
trap 'exit_after_signal 130' INT
trap 'exit_after_signal 143' TERM

print_usage() {
  echo "usage: $launcher_display [--check] [--rebuild] [--diagnostics] [iplay-options] [module-file|@file-list]" >&2
  echo "builds rewrite/.build/iplay when missing or stale, then runs the SDL/notcurses host player" >&2
  echo "--check verifies the native SDL/notcurses host player is built and executable without starting playback" >&2
  echo "--check-playback <module-file|@file-list> runs a short SDL/notcurses playback readiness check with dummy SDL audio by default" >&2
  echo "--rebuild forces the native SDL/notcurses host player rebuild before launch" >&2
  echo "streams player output live while keeping status evidence for the wrapper exit decision" >&2
  echo "--diagnostics or IPLAY_LAUNCHER_DIAGNOSTICS=1 shows raw player evidence" >&2
  echo "--list-extensions is passed through to list external tracker formats handled by the library-backed path" >&2
  echo "--classify <path> is passed through to show decoder route selection without playback" >&2
  echo "when --video-mode is omitted, iplay selects the nearest supported text mode from the terminal size" >&2
  echo "supported --video-mode values: 40x25bw, 40x25color, 40x25mono, 80x25bw, 80x25color, 80x25mono, 80x28, original, 80x50, 80x50project, auto, terminal" >&2
  echo "a bare text-mode token may be supplied before or after <module-file|@file-list>" >&2
  echo "press q, Q, or Escape to stop playback" >&2
  echo "keyboard and block-limit player stops from the diagnostic host status code are returned as success" >&2
  echo "example: $launcher_display --video-mode=80x50 samples/aryx.s3m" >&2
}

check_playback_video_mode_supported() {
  for player_arg in "${player_args[@]}"; do
    case "$player_arg" in
      --video-mode=*)
        video_mode=${player_arg#--video-mode=}
        video_mode_normalized=${video_mode,,}
        case "$video_mode_normalized" in
          40x25|40x25bw|40x25color|40x25mono|80x25|80x25bw|80x25color|80x25mono|80x28|original|80x50|80x50project|auto|terminal)
            ;;
          *)
            echo "iplay: unsupported text mode: $video_mode" >&2
            return 2
            ;;
        esac
        ;;
    esac
  done
  return 0
}

is_bare_video_mode_token() {
  case "${1,,}" in
    40x25|40x25bw|40x25color|40x25mono|80x25|80x25bw|80x25color|80x25mono|80x28|original|80x50|80x50project|auto|terminal)
      return 0
      ;;
  esac
  return 1
}

looks_like_bare_video_mode_token() {
  case "${1,,}" in
    auto|terminal|[0-9]*x[0-9]*|[0-9]*x[0-9]*bw|[0-9]*x[0-9]*color|[0-9]*x[0-9]*mono|[0-9]*x[0-9]*project)
      return 0
      ;;
  esac
  return 1
}

normalize_leading_bare_video_mode_arg() {
  if [ "${#player_args[@]}" -ge 2 ] && is_bare_video_mode_token "${player_args[0]}"; then
    leading_video_mode=${player_args[0]}
    leading_module_arg=${player_args[1]}
    normalized_player_args=("$leading_module_arg" "$leading_video_mode")
    if [ "${#player_args[@]}" -gt 2 ]; then
      normalized_player_args+=("${player_args[@]:2}")
    fi
    player_args=("${normalized_player_args[@]}")
  elif [ "${#player_args[@]}" -ge 2 ] && looks_like_bare_video_mode_token "${player_args[0]}"; then
    echo "iplay: unsupported text mode: ${player_args[0]}" >&2
    return 2
  fi
  return 0
}

case "${1:-}" in
  -h|--help)
    print_usage
    exit 0
    ;;
esac

launcher_diagnostics=${IPLAY_LAUNCHER_DIAGNOSTICS:-0}
launcher_rebuild=0
launcher_check=0
launcher_check_playback=0
player_args=()
for arg in "$@"; do
  case "$arg" in
    -h|--help)
      print_usage
      exit 0
      ;;
    --rebuild)
      launcher_rebuild=1
      ;;
    --check)
      launcher_check=1
      ;;
    --check-playback)
      launcher_check_playback=1
      ;;
    --diagnostics)
      launcher_diagnostics=1
      ;;
    *)
      player_args+=("$arg")
      ;;
  esac
done

normalize_leading_bare_video_mode_arg || {
  if [ "$launcher_check_playback" = "1" ]; then
    echo "iplay.sh: playback readiness check failed with status 2" >&2
  fi
  exit 2
}

check_playback_video_mode_supported || {
  if [ "$launcher_check_playback" = "1" ]; then
    echo "iplay.sh: playback readiness check failed with status 2" >&2
  else
    echo "iplay.sh: player exited with status 2" >&2
  fi
  exit 2
}

native_rebuild_deps="rewrite/iplay_rewrite.c rewrite/iplay_rewrite.h rewrite/modplug_renderer.cpp rewrite/modplug_renderer.hpp rewrite/modplug_audio_bridge.cpp rewrite/modplug_audio_bridge.hpp rewrite/modern_player.cpp rewrite/modern_player.hpp rewrite/modplug_audio_probe.cpp rewrite/notcurses_presenter.cpp rewrite/notcurses_presenter.hpp rewrite/sdl_visualizer.cpp rewrite/sdl_visualizer.hpp rewrite/build_native_player.sh"
native_needs_rebuild=$launcher_rebuild
native_rebuilt=0
if [ ! -x rewrite/.build/iplay ]; then
  native_needs_rebuild=1
else
  for src in $native_rebuild_deps; do
    if [ "$src" -nt rewrite/.build/iplay ]; then
      native_needs_rebuild=1
      break
    fi
  done
fi

if [ "$native_needs_rebuild" = "1" ]; then
  ./rewrite/build_native_player.sh
  native_rebuilt=1
fi

if [ ! -x rewrite/.build/iplay ]; then
  echo "iplay.sh: native executable not found after build: rewrite/.build/iplay" >&2
  exit 2
fi

if [ "$launcher_check" = "1" ]; then
  if ! ./rewrite/.build/iplay --list-extensions >/dev/null; then
    echo "iplay.sh: native executable failed no-playback startup check: rewrite/.build/iplay" >&2
    exit 2
  fi
  echo "iplay.sh: ready: exe=rewrite/.build/iplay rebuilt=$native_rebuilt"
  exit 0
fi

if [ "$launcher_check_playback" = "1" ]; then
  check_playback_video_mode_supported || {
    echo "iplay.sh: playback readiness check failed with status 2" >&2
    exit 2
  }
  playback_args=("${player_args[@]}")
  playback_has_video_mode=0
  for player_arg in "${player_args[@]}"; do
    if [[ "$player_arg" == --video-mode=* ]] || is_bare_video_mode_token "$player_arg"; then
      playback_has_video_mode=1
      break
    fi
  done
  if [ "$playback_has_video_mode" = "0" ]; then
    playback_args+=(--video-mode=40x25color)
  fi
  run_log=$(mktemp)
  set +e
  SDL_AUDIODRIVER=${SDL_AUDIODRIVER:-dummy} ./rewrite/.build/iplay "${playback_args[@]}" >"$run_log" <<'EOF'
q
EOF
  status=$?
  set -e
  if [ "$status" != "0" ] && ! { [ "$status" = "3" ] && grep -q '^status=keyboard route_id=0 route=external-library provider=libmikmod' "$run_log"; }; then
    failure_status=$(grep -E '^status=' "$run_log" | tail -n 1 || true)
    if [ -n "$failure_status" ]; then
      echo "iplay.sh: playback readiness check failed: $failure_status" >&2
    elif [ "$status" != "2" ] && [ -s "$run_log" ]; then
      cat "$run_log" >&2
    fi
    echo "iplay.sh: playback readiness check failed with status $status" >&2
    cleanup_run_log
    exit 2
  fi
  if ! grep -q '^SDL audio sink: requested=1 opened=1' "$run_log"; then
    cat "$run_log" >&2
    echo "iplay.sh: playback readiness check did not open SDL SB16 stereo output" >&2
    cleanup_run_log
    exit 2
  fi
  terminal_render_line=$(grep -E '^Terminal render: requested=1 cols=[0-9]+ rows=[0-9]+ bytes=[0-9]+' "$run_log" | tail -n 1 || true)
  if [[ ! "$terminal_render_line" =~ cols=([0-9]+)\ rows=([0-9]+)\ bytes=([0-9]+) ]]; then
    cat "$run_log" >&2
    echo "iplay.sh: playback readiness check did not render a supported terminal screen" >&2
    cleanup_run_log
    exit 2
  fi
  playback_cols=${BASH_REMATCH[1]}
  playback_rows=${BASH_REMATCH[2]}
  playback_bytes=${BASH_REMATCH[3]}
  if [ "$playback_bytes" -ne $((playback_cols * playback_rows * 2)) ]; then
    cat "$run_log" >&2
    echo "iplay.sh: playback readiness check rendered inconsistent terminal geometry" >&2
    cleanup_run_log
    exit 2
  fi
  if ! grep -q '^status=keyboard route_id=0 route=external-library provider=libmikmod' "$run_log"; then
    cat "$run_log" >&2
    echo "iplay.sh: playback readiness check did not reach libmikmod keyboard-stop playback" >&2
    cleanup_run_log
    exit 2
  fi
  echo "iplay.sh: playback-ready: exe=rewrite/.build/iplay video=${playback_cols}x${playback_rows} audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod"
  cleanup_run_log
  exit 0
fi

run_log=$(mktemp)
set +e
if [ "$launcher_diagnostics" != "1" ] && [ -t 0 ] && [ -t 1 ]; then
  ./rewrite/.build/iplay "${player_args[@]}"
elif [ "$launcher_diagnostics" = "1" ]; then
  ./rewrite/.build/iplay "${player_args[@]}" | tee "$run_log"
else
  if command -v python3 >/dev/null 2>&1; then
    python3 ./rewrite/pty_run.py "$run_log" ./rewrite/.build/iplay "${player_args[@]}"
  else
    ./rewrite/.build/iplay "${player_args[@]}" | tee "$run_log"
  fi
fi
status=${PIPESTATUS[0]}
set -e

if [ ! -s "$run_log" ] && [ "$status" = "3" ]; then
  cleanup_run_log
  exit 0
fi
if [ "$status" = "3" ] && grep -Eq '^status=(keyboard|block-limit) ' "$run_log"; then
  cleanup_run_log
  exit 0
fi
if [ "$launcher_diagnostics" != "1" ] && [ "$status" -ne 0 ]; then
  failure_status=$(grep -E '^status=' "$run_log" | tail -n 1 || true)
  if [ -n "$failure_status" ]; then
    echo "iplay.sh: playback failed: $failure_status" >&2
  else
    echo "iplay.sh: player exited with status $status" >&2
  fi
fi
cleanup_run_log
exit "$status"
