#!/bin/sh
set -eu
cd "$(dirname "$0")/.."

print_usage() {
  echo "usage: ./rewrite/try_player.sh [--rebuild] [--modern|--native|--native-interactive|--native-source-end|--native-keyboard-after-one|--native-stdin-keyboard|--native-audio|--native-terminal|--native-live|--quiet|--diagnostics|--continuous-diagnostics|--hardware-diagnostics|--production] [--blocks=N] [--video-mode=MODE] <module-file|@file-list>" >&2
  echo "runs bounded IPLAYDIAG.EXE diagnostics under kvikdos by default so module/UI/SB16-wrapper playback returns quickly" >&2
  echo "default mode is the safe kvikdos proof path; use --production only on real SB16-capable DOS or when checking the SB16-unavailable exit" >&2
  echo "--rebuild forces a player-only rebuild before launching kvikdos" >&2
  echo "--native runs rewrite/.build/iplay_native directly on the host through libmikmod plus the SDL-compatible SB16/notcurses-style runtime path" >&2
  echo "--modern runs rewrite/.build/iplay, the preferred SDL/notcurses host player; it enables source-end playback, SDL2 audio, terminal render, live meters, and raw stdin keyboard stop" >&2
  echo "direct SDL/notcurses player example: ./rewrite/iplay.sh <module-file>; use ./rewrite/iplay.sh --diagnostics --video-mode=80x50 <module-file> for raw evidence" >&2
  echo "--native-interactive enables native source-end playback, SDL2 audio, terminal render, live meters, and raw stdin keyboard stop" >&2
  echo "--native-source-end runs the native host path until libmikmod reports natural source end" >&2
  echo "--native-keyboard-after-one runs the native host path until the keyboard/interactive stop seam fires after one block" >&2
  echo "--native-stdin-keyboard stops native playback when q, Q, or Escape is read from stdin" >&2
  echo "--native-audio opens a real SDL2 queued-audio device in native mode; set SDL_AUDIODRIVER=dummy for headless tests" >&2
  echo "--native-terminal renders the final notcurses-style text cells to the host terminal with ANSI 16-color output" >&2
  echo "--native-live updates ANSI audio level meters from the native playback callback while blocks are submitted" >&2
  echo "--quiet runs IPLAYTRY.EXE continuous quiet playback; in headless kvikdos this can end by timeout" >&2
  echo "--diagnostics runs IPLAYDIAG.EXE for visible bounded diagnostic stdout" >&2
  echo "--continuous-diagnostics runs IPLAYCONT.EXE for visible continuous-loop diagnostic stdout" >&2
  echo "--hardware-diagnostics runs IPLAYHW.EXE for real-SB16 probe/unavailable diagnostics" >&2
  echo "--production runs IPLAYC.EXE, the quiet production real-SB16 DOS player" >&2
  echo "--blocks=N is consumed by IPLAYDIAG.EXE when bounded diagnostics are enabled" >&2
  echo "--video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50|terminal|auto selects the text mode for the trial; aliases 40x25mono, 40x25, 80x25mono, 80x25, and 80x50project are accepted case-insensitively; terminal/auto selects the nearest supported size from COLUMNS/LINES or stty size" >&2
}

resolve_case_insensitive_file() {
  requested_path=$1
  requested_dir=$(dirname -- "$requested_path")
  requested_base=$(basename -- "$requested_path")
  requested_key=$(printf '%s' "$requested_base" | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz')
  if [ -d "$requested_dir" ]; then
    for candidate in "$requested_dir"/*; do
      if [ ! -f "$candidate" ]; then
        continue
      fi
      candidate_base=$(basename -- "$candidate")
      candidate_key=$(printf '%s' "$candidate_base" | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz')
      if [ "$candidate_key" = "$requested_key" ]; then
        printf '%s\n' "$candidate"
        return 0
      fi
    done
  fi
  return 1
}

if [ "$#" -lt 1 ]; then
  print_usage
  exit 2
fi

case "$1" in
  -h|--help)
    print_usage
    exit 0
    ;;
esac

player_args=
trial_blocks_set=0
trial_video_mode_arg=
trial_diagnostics=${IPLAY_TRIAL_DIAGNOSTICS:-1}
trial_hardware_diagnostics=${IPLAY_TRIAL_HARDWARE_DIAGNOSTICS:-0}
trial_production=0
trial_native=0
trial_native_source_end=0
trial_native_keyboard_after_one=0
trial_native_stdin_keyboard=0
trial_native_audio=0
trial_native_terminal=0
trial_native_live=0
trial_modern=0
trial_rebuild=${IPLAY_TRIAL_REBUILD:-auto}
trial_blocks_value=${IPLAY_TRIAL_DIAGNOSTIC_BLOCKS:-32}
while [ "$#" -gt 0 ]; do
  case "$1" in
    -h|--help)
      print_usage
      exit 0
      ;;
    --diagnostics)
      trial_diagnostics=1
      trial_hardware_diagnostics=0
      shift
      ;;
    --continuous-diagnostics)
      trial_diagnostics=1
      trial_hardware_diagnostics=0
      IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYCONT.EXE}
      shift
      ;;
    --quiet)
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      shift
      ;;
    --native)
      trial_native=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      shift
      ;;
    --modern)
      trial_modern=1
      trial_native=1
      trial_native_source_end=1
      trial_native_stdin_keyboard=1
      trial_native_audio=1
      trial_native_terminal=1
      trial_native_live=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      shift
      ;;
    --native-interactive)
      trial_native=1
      trial_native_source_end=1
      trial_native_stdin_keyboard=1
      trial_native_audio=1
      trial_native_terminal=1
      trial_native_live=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      shift
      ;;
    --native-source-end)
      trial_native=1
      trial_native_source_end=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      shift
      ;;
    --native-keyboard-after-one)
      trial_native=1
      trial_native_keyboard_after_one=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      shift
      ;;
    --native-stdin-keyboard)
      trial_native=1
      trial_native_stdin_keyboard=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      shift
      ;;
    --native-audio)
      trial_native=1
      trial_native_audio=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      shift
      ;;
    --native-terminal)
      trial_native=1
      trial_native_terminal=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      shift
      ;;
    --native-live)
      trial_native=1
      trial_native_live=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      shift
      ;;
    --hardware-diagnostics)
      trial_diagnostics=1
      trial_hardware_diagnostics=1
      trial_production=0
      shift
      ;;
    --production)
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=1
      IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYC.EXE}
      shift
      ;;
    --rebuild)
      trial_rebuild=1
      shift
      ;;
    --blocks=*)
      player_args="${player_args}${player_args:+ }$1"
      trial_blocks_set=1
      trial_blocks_value=${1#--blocks=}
      shift
      ;;
    --video-mode=*)
      player_args="${player_args}${player_args:+ }$1"
      trial_video_mode_arg=${1#--video-mode=}
      shift
      ;;
    *)
      break
      ;;
  esac
done

if [ "$#" -lt 1 ]; then
  echo "try_player: missing module file after trial options" >&2
  exit 2
fi

host_module=$1
trial_filelist_arg=
trial_filelist_path=
trial_filelist_selected=
trial_filelist_selected_host=
native_module_arg=
case "$host_module" in
  @*)
    trial_filelist_arg=$host_module
    trial_filelist_path=${host_module#@}
    if [ -z "$trial_filelist_path" ] || [ ! -f "$trial_filelist_path" ]; then
      trial_filelist_case_match=
      if [ -n "$trial_filelist_path" ]; then
        trial_filelist_case_match=$(resolve_case_insensitive_file "$trial_filelist_path" || true)
      fi
      if [ -n "$trial_filelist_case_match" ]; then
        trial_filelist_path=$trial_filelist_case_match
        printf 'try_player: resolved DOS-style case-insensitive file-list path: %s -> %s\n' "${host_module#@}" "$trial_filelist_path" >&2
      else
        echo "try_player: file list not found: $trial_filelist_path" >&2
        exit 2
      fi
    fi
    trial_filelist_dir=$(CDPATH= cd -- "$(dirname -- "$trial_filelist_path")" && pwd)
    trial_filelist_abs=$trial_filelist_dir/$(basename -- "$trial_filelist_path")
    while IFS= read -r trial_filelist_line || [ -n "$trial_filelist_line" ]; do
      trial_filelist_line=$(printf '%s' "$trial_filelist_line" | sed 's/\r$//; s/^[[:space:]]*//; s/[[:space:]]*$//')
      if [ -n "$trial_filelist_line" ]; then
        trial_filelist_selected=$trial_filelist_line
        break
      fi
    done < "$trial_filelist_path"
    if [ -z "$trial_filelist_selected" ]; then
      echo "try_player: file list has no module entries: $trial_filelist_path" >&2
      exit 2
    fi
    case "$trial_filelist_selected" in
      /*) trial_filelist_selected_host=$trial_filelist_selected ;;
      *) trial_filelist_selected_host=$trial_filelist_dir/$trial_filelist_selected ;;
    esac
    host_module=$trial_filelist_selected_host
    native_module_arg=@$trial_filelist_abs
    ;;
esac
if [ ! -f "$host_module" ]; then
  requested_module=$host_module
  requested_dir=$(dirname -- "$requested_module")
  requested_base=$(basename -- "$requested_module")
  requested_key=$(printf '%s' "$requested_base" | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz')
  case_insensitive_match=
  if [ -d "$requested_dir" ]; then
    for candidate in "$requested_dir"/*; do
      if [ ! -f "$candidate" ]; then
        continue
      fi
      candidate_base=$(basename -- "$candidate")
      candidate_key=$(printf '%s' "$candidate_base" | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz')
      if [ "$candidate_key" = "$requested_key" ]; then
        case_insensitive_match=$candidate
        break
      fi
    done
  fi
  if [ -n "$case_insensitive_match" ]; then
    host_module=$case_insensitive_match
    printf 'try_player: resolved DOS-style case-insensitive module path: %s -> %s\n' "$requested_module" "$host_module" >&2
  else
    echo "try_player: module file not found: $host_module" >&2
    exit 2
  fi
fi

name=$(basename "$host_module")
src_dir=$(CDPATH= cd -- "$(dirname -- "$host_module")" && pwd)
host_module_size=$(wc -c < "$host_module" | tr -d ' ')
if [ -z "$native_module_arg" ]; then
  native_module_arg=$src_dir/$name
fi
shift
while [ "$#" -gt 0 ]; do
  case "$1" in
    -h|--help)
      print_usage
      exit 0
      ;;
    --diagnostics)
      trial_diagnostics=1
      trial_hardware_diagnostics=0
      ;;
    --continuous-diagnostics)
      trial_diagnostics=1
      trial_hardware_diagnostics=0
      IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYCONT.EXE}
      ;;
    --quiet)
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      ;;
    --native)
      trial_native=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      ;;
    --modern)
      trial_modern=1
      trial_native=1
      trial_native_source_end=1
      trial_native_stdin_keyboard=1
      trial_native_audio=1
      trial_native_terminal=1
      trial_native_live=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      ;;
    --native-interactive)
      trial_native=1
      trial_native_source_end=1
      trial_native_stdin_keyboard=1
      trial_native_audio=1
      trial_native_terminal=1
      trial_native_live=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      ;;
    --native-source-end)
      trial_native=1
      trial_native_source_end=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      ;;
    --native-keyboard-after-one)
      trial_native=1
      trial_native_keyboard_after_one=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      ;;
    --native-stdin-keyboard)
      trial_native=1
      trial_native_stdin_keyboard=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      ;;
    --native-audio)
      trial_native=1
      trial_native_audio=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      ;;
    --native-terminal)
      trial_native=1
      trial_native_terminal=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      ;;
    --native-live)
      trial_native=1
      trial_native_live=1
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=0
      ;;
    --hardware-diagnostics)
      trial_diagnostics=1
      trial_hardware_diagnostics=1
      trial_production=0
      ;;
    --production)
      trial_diagnostics=0
      trial_hardware_diagnostics=0
      trial_production=1
      IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYC.EXE}
      ;;
    --rebuild)
      trial_rebuild=1
      ;;
    --blocks=*)
      player_args="${player_args}${player_args:+ }$1"
      trial_blocks_set=1
      trial_blocks_value=${1#--blocks=}
      ;;
    --video-mode=*)
      player_args="${player_args}${player_args:+ }$1"
      trial_video_mode_arg=${1#--video-mode=}
      ;;
    *)
      break
      ;;
  esac
  shift
done
if [ "$trial_diagnostics" = "1" ] && [ "$trial_blocks_set" = "0" ]; then
  player_args="${player_args}${player_args:+ }--blocks=${IPLAY_TRIAL_DIAGNOSTIC_BLOCKS:-32}"
fi
if [ "$trial_diagnostics" != "1" ] && [ "$trial_native" != "1" ] && [ "$trial_blocks_set" = "1" ]; then
  echo "try_player: --blocks=N is only supported with diagnostic trial modes" >&2
  exit 2
fi
set -- $player_args "$name" "$@"

trial_video_mode_default=80x25color
if [ -z "${trial_video_mode_arg:-}" ] && [ "$trial_native" = "1" ] && [ "$trial_native_source_end" = "1" ] && [ "$trial_native_stdin_keyboard" = "1" ] && [ "$trial_native_audio" = "1" ] && [ "$trial_native_terminal" = "1" ] && [ "$trial_native_live" = "1" ]; then
  trial_video_mode_default=auto
fi
trial_video_mode_key=$(printf '%s' "${trial_video_mode_arg:-$trial_video_mode_default}" | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz')
case "$trial_video_mode_key" in
  40x25bw|40x25mono)
    trial_video_mode=40x25bw
    trial_video_cols=40
    trial_video_rows=25
    ;;
  40x25color|40x25)
    trial_video_mode=40x25color
    trial_video_cols=40
    trial_video_rows=25
    ;;
  80x25bw|80x25mono)
    trial_video_mode=80x25bw
    trial_video_cols=80
    trial_video_rows=25
    ;;
  80x25color|80x25)
    trial_video_mode=80x25color
    trial_video_cols=80
    trial_video_rows=25
    ;;
  80x50|80x50project)
    trial_video_mode=80x50
    trial_video_cols=80
    trial_video_rows=50
    ;;
  terminal|auto)
    trial_terminal_cols=${COLUMNS:-0}
    trial_terminal_rows=${LINES:-0}
    case "$trial_terminal_cols" in ''|*[!0-9]*) trial_terminal_cols=0 ;; esac
    case "$trial_terminal_rows" in ''|*[!0-9]*) trial_terminal_rows=0 ;; esac
    if { [ "$trial_terminal_cols" -eq 0 ] || [ "$trial_terminal_rows" -eq 0 ]; } && command -v stty >/dev/null 2>&1; then
      trial_terminal_size=$(stty size 2>/dev/null || true)
      case "$trial_terminal_size" in
        *" "*)
          trial_terminal_rows_from_stty=${trial_terminal_size%% *}
          trial_terminal_cols_from_stty=${trial_terminal_size#* }
          case "$trial_terminal_rows_from_stty" in ''|*[!0-9]*) trial_terminal_rows_from_stty=0 ;; esac
          case "$trial_terminal_cols_from_stty" in ''|*[!0-9]*) trial_terminal_cols_from_stty=0 ;; esac
          if [ "$trial_terminal_rows" -eq 0 ]; then trial_terminal_rows=$trial_terminal_rows_from_stty; fi
          if [ "$trial_terminal_cols" -eq 0 ]; then trial_terminal_cols=$trial_terminal_cols_from_stty; fi
          ;;
      esac
    fi
    if [ "$trial_terminal_cols" -ge 80 ] && [ "$trial_terminal_rows" -ge 50 ]; then
      trial_video_mode=80x50
      trial_video_cols=80
      trial_video_rows=50
    elif [ "$trial_terminal_cols" -ge 80 ]; then
      trial_video_mode=80x25color
      trial_video_cols=80
      trial_video_rows=25
    else
      trial_video_mode=40x25color
      trial_video_cols=40
      trial_video_rows=25
    fi
    ;;
  *)
    if [ -n "${trial_video_mode_arg:-}" ]; then
      echo "try_player: unsupported video mode: $trial_video_mode_arg" >&2
      exit 2
    fi
    trial_video_mode=80x25color
    trial_video_cols=80
    trial_video_rows=25
    ;;
esac

if [ "${IPLAY_TRIAL_VALIDATE_ONLY:-0}" = "1" ]; then
  if [ "$trial_native" = "1" ]; then
    if [ "$trial_modern" = "1" ]; then
      validate_trial_exe=${IPLAY_TRIAL_EXE:-iplay}
    else
      validate_trial_exe=${IPLAY_TRIAL_EXE:-iplay_native}
    fi
  elif [ "$trial_production" = "1" ]; then
    validate_trial_exe=${IPLAY_TRIAL_EXE:-IPLAYC.EXE}
  elif [ "$trial_diagnostics" = "1" ]; then
    if [ "$trial_hardware_diagnostics" = "1" ]; then
      validate_trial_exe=${IPLAY_TRIAL_EXE:-IPLAYHW.EXE}
    else
      validate_trial_exe=${IPLAY_TRIAL_EXE:-IPLAYDIAG.EXE}
    fi
  else
    validate_trial_exe=${IPLAY_TRIAL_EXE:-IPLAYTRY.EXE}
  fi
  printf 'trial_exe=%s diagnostics=%s hardware_diagnostics=%s production=%s native=%s native_source_end=%s native_keyboard_after_one=%s native_stdin_keyboard=%s native_audio=%s native_terminal=%s native_live=%s native_modern=%s\n' "$validate_trial_exe" "$trial_diagnostics" "$trial_hardware_diagnostics" "$trial_production" "$trial_native" "$trial_native_source_end" "$trial_native_keyboard_after_one" "$trial_native_stdin_keyboard" "$trial_native_audio" "$trial_native_terminal" "$trial_native_live" "$trial_modern"
  printf 'trial_video_mode=%s cols=%s rows=%s\n' "$trial_video_mode" "$trial_video_cols" "$trial_video_rows"
  if [ -n "$trial_filelist_arg" ]; then
    printf 'trial_filelist_arg=%s\n' "$trial_filelist_arg"
    printf 'trial_filelist_path=%s\n' "$trial_filelist_abs"
    printf 'trial_filelist_selected=%s\n' "$trial_filelist_selected"
    printf 'trial_filelist_selected_host=%s\n' "$src_dir/$name"
  fi
  printf 'dos_args='
  sep=
  for arg in "$@"; do
    printf '%s%s' "$sep" "$arg"
    sep=' '
  done
  printf '\n'
  exit 0
fi

if [ "$trial_native" = "1" ]; then
  IPLAY_TRIAL_LOG=${IPLAY_TRIAL_LOG:-RES.TXT}
  if [ "$trial_modern" = "1" ]; then
    IPLAY_NATIVE_EXE=${IPLAY_TRIAL_EXE:-iplay}
    native_use_default_player_args=1
  else
    IPLAY_NATIVE_EXE=${IPLAY_TRIAL_EXE:-iplay_native}
    native_use_default_player_args=0
  fi
  if [ "$trial_native_keyboard_after_one" = "1" ]; then
    native_play_arg=--keyboard-after-one
    native_expected_status=keyboard
    native_loop_policy=native-keyboard-stop
  elif [ "$trial_native_stdin_keyboard" = "1" ] && [ "$trial_native_source_end" = "1" ]; then
    native_play_arg=--source-end
    native_expected_status=keyboard
    native_loop_policy=native-interactive-source-end-keyboard-stop
  elif [ "$trial_native_stdin_keyboard" = "1" ]; then
    native_play_arg=$trial_blocks_value
    native_expected_status=keyboard
    native_loop_policy=native-stdin-keyboard-stop
  elif [ "$trial_native_source_end" = "1" ]; then
    native_play_arg=--source-end
    native_expected_status=ok
    native_loop_policy=native-source-end
  else
    native_play_arg=$trial_blocks_value
    native_expected_status=block-limit
    native_loop_policy=native-libmikmod
  fi
  native_rebuild_deps="rewrite/iplay_rewrite.c rewrite/iplay_rewrite.h rewrite/modplug_renderer.cpp rewrite/modplug_audio_bridge.cpp rewrite/modern_player.cpp rewrite/modplug_audio_probe.cpp rewrite/build_native_player.sh"
  native_needs_rebuild=0
  if [ "$trial_rebuild" = "1" ] || [ ! -x "rewrite/.build/$IPLAY_NATIVE_EXE" ]; then
    native_needs_rebuild=1
  else
    for src in $native_rebuild_deps; do
      if [ "$src" -nt "rewrite/.build/$IPLAY_NATIVE_EXE" ]; then
        native_needs_rebuild=1
        break
      fi
    done
  fi
  if [ "$native_needs_rebuild" = "1" ]; then
    ./rewrite/build_native_player.sh
  fi
  if [ ! -x "rewrite/.build/$IPLAY_NATIVE_EXE" ]; then
    echo "try_player: native executable not found after build: $IPLAY_NATIVE_EXE" >&2
    exit 2
  fi
  {
    printf 'host_module=%s dos_module=%s\n' "$src_dir/$name" "$name"
    printf 'host_module_size=%s\n' "$host_module_size"
    printf 'trial_exe=%s diagnostics=0 hardware_diagnostics=0 production=0 native=1 native_stdin_keyboard=%s native_audio=%s native_terminal=%s native_live=%s native_modern=%s rebuild=%s needs_rebuild=%s\n' "$IPLAY_NATIVE_EXE" "$trial_native_stdin_keyboard" "$trial_native_audio" "$trial_native_terminal" "$trial_native_live" "$trial_modern" "$trial_rebuild" "$native_needs_rebuild"
    printf 'trial_exe_path=%s\n' "$(CDPATH= cd -- rewrite/.build && pwd)/$IPLAY_NATIVE_EXE"
    printf 'audio_mode=sdl-compatible-sb16-native\n'
    printf 'trial_loop_policy=%s\n' "$native_loop_policy"
    printf 'trial_proof_scope=native-sdl-notcurses\n'
    printf 'trial_video_mode=%s cols=%s rows=%s\n' "$trial_video_mode" "$trial_video_cols" "$trial_video_rows"
    if [ -n "$trial_filelist_arg" ]; then
      printf 'trial_filelist_arg=%s\n' "$trial_filelist_arg"
      printf 'trial_filelist_path=%s\n' "$trial_filelist_abs"
      printf 'trial_filelist_selected=%s\n' "$trial_filelist_selected"
      printf 'trial_filelist_selected_host=%s\n' "$src_dir/$name"
    fi
    if [ "$native_use_default_player_args" = "1" ]; then
      printf 'native_args=%s %s' "$native_module_arg" "$trial_video_mode"
    else
      printf 'native_args=%s %s %s' "$native_module_arg" "$native_play_arg" "$trial_video_mode"
      if [ "$trial_native_audio" = "1" ]; then
        printf ' --sdl-audio'
      fi
      if [ "$trial_native_terminal" = "1" ]; then
        printf ' --terminal-render'
      fi
      if [ "$trial_native_live" = "1" ]; then
        printf ' --terminal-live'
      fi
      if [ "$trial_native_stdin_keyboard" = "1" ]; then
        printf ' --stdin-keyboard'
      fi
    fi
    printf '\n'
  } > "$IPLAY_TRIAL_LOG"
  set +e
  if [ "$native_use_default_player_args" = "1" ]; then
    set -- "$native_module_arg" "$trial_video_mode"
  else
    set -- "$native_module_arg" "$native_play_arg" "$trial_video_mode"
    if [ "$trial_native_audio" = "1" ]; then
      set -- "$@" --sdl-audio
    fi
    if [ "$trial_native_terminal" = "1" ]; then
      set -- "$@" --terminal-render
    fi
    if [ "$trial_native_live" = "1" ]; then
      set -- "$@" --terminal-live
    fi
    if [ "$trial_native_stdin_keyboard" = "1" ]; then
      set -- "$@" --stdin-keyboard
    fi
  fi
  native_passthrough=0
  if [ "$trial_native_source_end" = "1" ] && [ "$trial_native_stdin_keyboard" = "1" ] && [ "$trial_native_audio" = "1" ] && [ "$trial_native_terminal" = "1" ] && [ "$trial_native_live" = "1" ]; then
    native_passthrough=1
  fi
  if [ "$native_passthrough" = "1" ]; then
    if [ "$trial_modern" = "1" ]; then
      native_passthrough_label=modern
    else
      native_passthrough_label="native interactive"
    fi
    printf 'try_player %s: module=%s mode=%s cols=%s rows=%s stop_keys=q,Q,Esc log=%s\n' "$native_passthrough_label" "$src_dir/$name" "$trial_video_mode" "$trial_video_cols" "$trial_video_rows" "$IPLAY_TRIAL_LOG" | tee -a "$IPLAY_TRIAL_LOG"
    native_rc_file=$(mktemp "${TMPDIR:-/tmp}/iplay-native-rc.XXXXXX") || exit 2
    trap 'rm -f "$native_rc_file"' EXIT HUP INT TERM
    ( "rewrite/.build/$IPLAY_NATIVE_EXE" "$@"; printf '%s\n' "$?" > "$native_rc_file" ) 2>&1 | tee -a "$IPLAY_TRIAL_LOG"
    rc=$(cat "$native_rc_file" 2>/dev/null || printf '4')
    rm -f "$native_rc_file"
    trap - EXIT HUP INT TERM
  else
    "rewrite/.build/$IPLAY_NATIVE_EXE" "$@" >> "$IPLAY_TRIAL_LOG" 2>&1
    rc=$?
  fi
  set -e
  if grep '^Module: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_module_loaded=yes\n' >> "$IPLAY_TRIAL_LOG"
    native_trial_loaded_module_name=$(grep '^Module: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r' | sed 's/^Module: //')
    printf 'trial_loaded_module_name=%s\n' "$native_trial_loaded_module_name" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_module_loaded=no\n' >> "$IPLAY_TRIAL_LOG"
    native_trial_loaded_module_name=
    printf 'trial_loaded_module_name=none\n' >> "$IPLAY_TRIAL_LOG"
  fi
  native_trial_loaded_module_key=$(printf '%s' "$native_trial_loaded_module_name" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')
  native_trial_requested_module_key=$(printf '%s' "$name" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')
  printf 'trial_loaded_module_key=%s\n' "$native_trial_loaded_module_key" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_requested_module_key=%s\n' "$native_trial_requested_module_key" >> "$IPLAY_TRIAL_LOG"
  if [ "$native_trial_loaded_module_key" = "$native_trial_requested_module_key" ]; then
    printf 'trial_requested_module_loaded=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_requested_module_loaded=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Size: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    native_trial_module_size=$(grep '^Size: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r' | sed 's/^Size: //; s/ bytes$//')
    printf 'trial_module_size=%s\n' "$native_trial_module_size" >> "$IPLAY_TRIAL_LOG"
  else
    native_trial_module_size=none
    printf 'trial_module_size=none\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if [ "$native_trial_module_size" = "$host_module_size" ]; then
    printf 'trial_module_size_matches_host=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_module_size_matches_host=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Loader: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    native_trial_module_loader_line=$(grep '^Loader: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r')
    native_trial_module_loader=$(printf '%s\n' "$native_trial_module_loader_line" | sed 's/^Loader: //')
    printf 'trial_module_loader_line=%s\n' "$native_trial_module_loader_line" >> "$IPLAY_TRIAL_LOG"
    printf 'trial_module_loader=%s\n' "$native_trial_module_loader" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_module_loader_line=none\n' >> "$IPLAY_TRIAL_LOG"
    printf 'trial_module_loader=none\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Module type tag: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    native_trial_module_type_tag=$(grep '^Module type tag: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r' | sed 's/^Module type tag: //')
    printf 'trial_module_type_tag=%s\n' "$native_trial_module_type_tag" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_module_type_tag=none\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Title: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    native_trial_module_title=$(grep '^Title: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r' | sed 's/^Title: //')
    printf 'trial_module_title=%s\n' "$native_trial_module_title" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_module_title=none\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^trial_module_loader=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_module_type_tag=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_module_type_tag=00000000$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_ok_loader_metadata=no\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_ok_loader_metadata=yes\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Playback pump: .* stop=' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_playback_pump=yes\n' >> "$IPLAY_TRIAL_LOG"
    native_trial_playback_line=$(grep '^Playback pump: .* stop=' "$IPLAY_TRIAL_LOG" | tail -n 1)
    printf 'trial_playback_line=%s\n' "$native_trial_playback_line" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_playback_pump=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Playback pump: blocks=[1-9][0-9]* frames=[1-9][0-9]* accepted=[1-9][0-9]* checksum=[1-9][0-9]* .* stop=' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_playback_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_playback_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep 'Audio backend: SDL-compatible SB16 16-bit stereo' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_audio_backend=SDL-compatible SB16 16-bit stereo\n' >> "$IPLAY_TRIAL_LOG"
    printf 'trial_audio_backend_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_audio_backend=none\n' >> "$IPLAY_TRIAL_LOG"
    printf 'trial_audio_backend_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  native_trial_audio_status_line=$(grep '^status=' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r')
  if [ -n "$native_trial_audio_status_line" ]; then
    printf 'trial_audio_status_line=%s\n' "$native_trial_audio_status_line" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_audio_status_line=none\n' >> "$IPLAY_TRIAL_LOG"
  fi
  case "$native_trial_audio_status_line" in *" levels="*) native_trial_audio_levels=$(printf '%s\n' "$native_trial_audio_status_line" | sed 's/^.* levels=//; s/[ "].*$//') ;; *) native_trial_audio_levels=none ;; esac
  case "$native_trial_audio_status_line" in *" maxlevels="*) native_trial_audio_maxlevels=$(printf '%s\n' "$native_trial_audio_status_line" | sed 's/^.* maxlevels=//; s/[ "].*$//') ;; *) native_trial_audio_maxlevels=none ;; esac
  case "$native_trial_audio_status_line" in
    *" active="*) native_trial_audio_active=$(printf '%s\n' "$native_trial_audio_status_line" | sed 's/^.* active=//; s/[ "].*$//') ;;
    *) if printf '%s\n' "$native_trial_audio_status_line" | grep 'summary="Audio backend: .*; Playback enabled;' >/dev/null 2>&1 || grep 'Playback Playback enabled active=1' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '24bit Interpolation Playback enabled active=1' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then native_trial_audio_active=1; else native_trial_audio_active=none; fi ;;
  esac
  printf 'trial_audio_levels=%s\n' "$native_trial_audio_levels" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_audio_maxlevels=%s\n' "$native_trial_audio_maxlevels" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_audio_active=%s\n' "$native_trial_audio_active" >> "$IPLAY_TRIAL_LOG"
  if [ "$native_trial_audio_active" = "1" ] && { printf '%s\n' "$native_trial_audio_maxlevels" | grep '^[1-9][0-9]*,[0-9][0-9]*$' >/dev/null 2>&1 || printf '%s\n' "$native_trial_audio_maxlevels" | grep '^[0-9][0-9]*,[1-9][0-9]*$' >/dev/null 2>&1; }; then
    printf 'trial_audio_levels_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_audio_levels_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^PCM source: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    native_trial_pcm_source_line=$(grep '^PCM source: ' "$IPLAY_TRIAL_LOG" | tail -n 1)
    printf 'trial_pcm_source_line=%s\n' "$native_trial_pcm_source_line" >> "$IPLAY_TRIAL_LOG"
    case "$native_trial_pcm_source_line" in *" provider="*) native_trial_pcm_provider=$(printf '%s\n' "$native_trial_pcm_source_line" | sed 's/^.* provider=//; s/ .*$//') ;; *) native_trial_pcm_provider=none ;; esac
    case "$native_trial_pcm_source_line" in *" renderer="*) native_trial_pcm_renderer=$(printf '%s\n' "$native_trial_pcm_source_line" | sed 's/^.* renderer=//; s/ .*$//') ;; *) native_trial_pcm_renderer=none ;; esac
    case "$native_trial_pcm_source_line" in *" route="*) native_trial_pcm_route=$(printf '%s\n' "$native_trial_pcm_source_line" | sed 's/^.* route=//; s/ .*$//') ;; *) native_trial_pcm_route=none ;; esac
    case "$native_trial_pcm_source_line" in *" input="*) native_trial_pcm_input=$(printf '%s\n' "$native_trial_pcm_source_line" | sed 's/^.* input=//; s/ .*$//') ;; *) native_trial_pcm_input=none ;; esac
    case "$native_trial_pcm_source_line" in *" truncated="*) native_trial_pcm_truncated=$(printf '%s\n' "$native_trial_pcm_source_line" | sed 's/^.* truncated=//; s/ .*$//') ;; *) native_trial_pcm_truncated=none ;; esac
    case "$native_trial_pcm_source_line" in *" hook_provider="*) native_trial_pcm_hook_provider=$(printf '%s\n' "$native_trial_pcm_source_line" | sed 's/^.* hook_provider=//; s/ .*$//') ;; *) native_trial_pcm_hook_provider=missing ;; esac
    case "$native_trial_pcm_source_line" in *" stream_start="*) native_trial_pcm_stream_start=$(printf '%s\n' "$native_trial_pcm_source_line" | sed 's/^.* stream_start=//; s/ .*$//') ;; *) native_trial_pcm_stream_start=none ;; esac
    printf 'trial_pcm_provider=%s\n' "$native_trial_pcm_provider" >> "$IPLAY_TRIAL_LOG"
    printf 'trial_pcm_renderer=%s\n' "$native_trial_pcm_renderer" >> "$IPLAY_TRIAL_LOG"
    printf 'trial_pcm_route=%s\n' "$native_trial_pcm_route" >> "$IPLAY_TRIAL_LOG"
    printf 'trial_pcm_input=%s\n' "$native_trial_pcm_input" >> "$IPLAY_TRIAL_LOG"
    printf 'trial_pcm_truncated=%s\n' "$native_trial_pcm_truncated" >> "$IPLAY_TRIAL_LOG"
    printf 'trial_pcm_hook_provider=%s\n' "$native_trial_pcm_hook_provider" >> "$IPLAY_TRIAL_LOG"
    printf 'trial_pcm_stream_start=%s\n' "$native_trial_pcm_stream_start" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_pcm_source_line=none\ntrial_pcm_provider=none\ntrial_pcm_renderer=none\ntrial_pcm_route=none\ntrial_pcm_input=none\ntrial_pcm_truncated=none\ntrial_pcm_hook_provider=missing\ntrial_pcm_stream_start=none\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Decoder route: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    native_trial_decoder_route_line=$(grep '^Decoder route: ' "$IPLAY_TRIAL_LOG" | tail -n 1)
    printf 'trial_decoder_route_line=%s\n' "$native_trial_decoder_route_line" >> "$IPLAY_TRIAL_LOG"
    case "$native_trial_decoder_route_line" in *" id="*) native_trial_decoder_route_id=$(printf '%s\n' "$native_trial_decoder_route_line" | sed 's/^.* id=//; s/ .*$//') ;; *) native_trial_decoder_route_id=none ;; esac
    case "$native_trial_decoder_route_line" in *" name="*) native_trial_decoder_route_name=$(printf '%s\n' "$native_trial_decoder_route_line" | sed 's/^.* name=//; s/ .*$//') ;; *) native_trial_decoder_route_name=none ;; esac
    printf 'trial_decoder_route_id=%s\n' "$native_trial_decoder_route_id" >> "$IPLAY_TRIAL_LOG"
    printf 'trial_decoder_route_name=%s\n' "$native_trial_decoder_route_name" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_decoder_route_line=none\ntrial_decoder_route_id=none\ntrial_decoder_route_name=none\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Decoder handoff: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    native_trial_decoder_handoff_line=$(grep '^Decoder handoff: ' "$IPLAY_TRIAL_LOG" | tail -n 1)
    native_trial_decoder_handoff=$(printf '%s\n' "$native_trial_decoder_handoff_line" | sed 's/^Decoder handoff: //')
    printf 'trial_decoder_handoff_line=%s\n' "$native_trial_decoder_handoff_line" >> "$IPLAY_TRIAL_LOG"
    printf 'trial_decoder_handoff=%s\n' "$native_trial_decoder_handoff" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_decoder_handoff_line=none\ntrial_decoder_handoff=none\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Screen present: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_screen_present=yes\n' >> "$IPLAY_TRIAL_LOG"
    native_trial_screen_reasons=$(grep '^Screen present: ' "$IPLAY_TRIAL_LOG" | sed 's/^Screen present: reason=//; s/ .*//' | tr '\n' ',' | sed 's/,$//')
    printf 'trial_screen_reasons=%s\n' "$native_trial_screen_reasons" >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_screen_present=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep "^selected_present=calls:1 bytes:$((trial_video_cols * trial_video_rows * 2)) cols:$trial_video_cols rows:$trial_video_rows" "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_selected_screen_geometry_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_selected_screen_geometry_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Screen present: reason=playback-position ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_playback_position_present=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_playback_position_present=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Screen present: reason=playback-position .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_playback_position_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_playback_position_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep "^Screen present: reason=playback-position .* cols=$trial_video_cols rows=$trial_video_rows " "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_playback_position_geometry_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_playback_position_geometry_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Screen present: reason=post-playback-status ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_post_playback_status_present=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_post_playback_status_present=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Screen present: reason=post-playback-status .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_post_playback_status_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_post_playback_status_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep "^Screen present: reason=post-playback-status .* cols=$trial_video_cols rows=$trial_video_rows " "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_selected_screen_geometry_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_post_playback_status_geometry_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_post_playback_status_geometry_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Resize present: phase=before .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* cols=80 rows=25 resize_ok=1 audio_frames=[1-9][0-9]* ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^Resize present: phase=after .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* cols=80 rows=50 resize_ok=1 audio_frames=[1-9][0-9]* ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^resize_after_present=calls:2 bytes:12000 cols:80 rows:50 resize_ok:1$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_resize_cycle_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_resize_cycle_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Subwindow present: origin=3,5 rows=5 cols=34 screen_bytes=4000 screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* calls=1 bytes=4000 present_cols=80 present_rows=25 audio_frames=[1-9][0-9]* ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^subwindow_title=.*SUBWINDOW' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^subwindow_audio=.*SDL-compatible SB16 16-bit' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_subwindow_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_subwindow_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Color probe: target_fg=16 target_bg=8 cells=24 fg_matches=16 bg_matches=8 blink_matches=8 fg_mask=ffff bg_mask=ff blink_mask=aa present_calls=1 bytes=4000 cols=80 rows=25$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_color_probe_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_color_probe_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if grep '^Level sequence: target=16 samples=16 nonzero=[1-9][0-9]* changed=1 .* max=[1-9][0-9]*,[0-9][0-9]* .* status=keyboard stop=keyboard$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^Level sequence: target=16 samples=16 nonzero=[1-9][0-9]* changed=1 .* max=[0-9][0-9]*,[1-9][0-9]* .* status=keyboard stop=keyboard$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    printf 'trial_audio_level_sequence_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
  else
    printf 'trial_audio_level_sequence_valid=no\n' >> "$IPLAY_TRIAL_LOG"
  fi
  if { [ "$rc" -eq 0 ] || [ "$rc" -eq 3 ]; } && { grep '^status=ok ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep "^status=$native_expected_status " "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; } && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_module_size_matches_host=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_ok_loader_metadata=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_audio_backend_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_audio_levels_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_audio_level_sequence_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_color_probe_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_pcm_provider=libmikmod$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_pcm_input=file-path$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_pcm_hook_provider=libmikmod$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_pcm_stream_start=0$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_decoder_route_id=0$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_decoder_route_name=external-library$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_decoder_handoff=external tracker -> SB16 PCM seam.$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_position_present=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_position_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_position_geometry_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_post_playback_status_present=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_post_playback_status_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_post_playback_status_geometry_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_resize_cycle_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_subwindow_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep "^Selected text mode: $trial_video_mode cols=$trial_video_cols rows=$trial_video_rows" "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_selected_screen_geometry_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^screen_present=calls:1 bytes:4000 cols:80 rows:25' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^screen40_present=calls:1 bytes:2000 cols:40 rows:25' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^screen80x50_present=calls:1 bytes:8000 cols:80 rows:50' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    trial_result=native-sdl-notcurses-ok
    trial_failure_reason=none
    trial_script_exit_status=0
  elif grep '^status=project-decoder-unavailable ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    trial_result=project-decoder-unavailable
    trial_failure_reason=project-decoder-unavailable
    trial_script_exit_status=4
  elif grep '^status=unsupported-format ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    trial_result=unsupported-format
    trial_failure_reason=unsupported-format
    trial_script_exit_status=4
  elif grep '^status=external-decoder-failed ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
    trial_result=external-decoder-failed
    trial_failure_reason=external-decoder-failed
    trial_script_exit_status=4
  else
    trial_result=native-sdl-notcurses-failed
    trial_failure_reason=native-player-process-or-evidence-failed
    trial_script_exit_status=${rc:-4}
    if [ "$trial_script_exit_status" -eq 0 ]; then
      trial_script_exit_status=4
    fi
  fi
  printf 'trial_result=%s\n' "$trial_result" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_failure_reason=%s\n' "$trial_failure_reason" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_exe=%s exit_status=%s\n' "$IPLAY_NATIVE_EXE" "$rc" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_script_exit_status=%s\n' "$trial_script_exit_status" >> "$IPLAY_TRIAL_LOG"
  if [ "$native_passthrough" = "1" ]; then
    printf 'trial_result=%s\n' "$trial_result"
    printf 'trial_failure_reason=%s\n' "$trial_failure_reason"
    printf 'trial_exe=%s exit_status=%s\n' "$IPLAY_NATIVE_EXE" "$rc"
    printf 'trial_script_exit_status=%s\n' "$trial_script_exit_status"
  else
    cat "$IPLAY_TRIAL_LOG"
  fi
  exit "$trial_script_exit_status"
fi

KVIKDOS_TIMEOUT=${KVIKDOS_TIMEOUT:-timeout}
KVIKDOS_SECONDS=${KVIKDOS_SECONDS:-3}
KVIKDOS=${KVIKDOS:-/home/xor/kvikdos/kvikdos}
IPLAY_TRIAL_LOG=${IPLAY_TRIAL_LOG:-RES.TXT}
if [ "$trial_production" = "1" ]; then
  IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYC.EXE}
elif [ "$trial_diagnostics" = "1" ]; then
  if [ "$trial_hardware_diagnostics" = "1" ]; then
    IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYHW.EXE}
  else
    IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYDIAG.EXE}
  fi
else
  IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYTRY.EXE}
fi
case "$IPLAY_TRIAL_EXE" in
  IPLAYC.EXE|IPLAYHW.EXE)
    trial_audio_mode=real-sb16-hardware
    ;;
  *)
    trial_audio_mode=wrapper-sb16-kvikdos-not-audible
    ;;
esac
case "$IPLAY_TRIAL_EXE" in
  IPLAYDIAG.EXE)
    trial_loop_policy=bounded-diagnostics
    trial_proof_scope=playable-wrapper-diagnostic
    ;;
  IPLAYCONT.EXE)
    trial_loop_policy=continuous-diagnostics
    trial_proof_scope=playable-wrapper-continuous
    ;;
  IPLAYTRY.EXE)
    trial_loop_policy=continuous-quiet-wrapper
    trial_proof_scope=playable-wrapper-continuous
    ;;
  IPLAYC.EXE)
    trial_loop_policy=continuous-real-sb16
    trial_proof_scope=production-real-sb16
    ;;
  IPLAYHW.EXE)
    trial_loop_policy=continuous-real-sb16-diagnostics
    trial_proof_scope=hardware-unavailable-probe
    ;;
  *)
    trial_loop_policy=custom
    trial_proof_scope=custom
    ;;
esac

player_rebuild_deps="rewrite/iplay_player.c rewrite/iplay_rewrite.c rewrite/iplay_rewrite.h rewrite/iplay_abi_watcom.c rewrite/build_player.sh"
needs_rebuild=0
if [ "$trial_rebuild" = "1" ] || [ ! -f "rewrite/.build/$IPLAY_TRIAL_EXE" ]; then
  needs_rebuild=1
else
  for src in $player_rebuild_deps; do
    if [ "$src" -nt "rewrite/.build/$IPLAY_TRIAL_EXE" ]; then
      needs_rebuild=1
      break
    fi
  done
fi
if [ "$needs_rebuild" = "1" ]; then
  ./rewrite/build_player.sh
fi
if [ ! -f "rewrite/.build/$IPLAY_TRIAL_EXE" ]; then
  echo "try_player: trial executable not found after player build: $IPLAY_TRIAL_EXE" >&2
  exit 2
fi
trial_binary_fresh=yes
for src in $player_rebuild_deps; do
  if [ "$src" -nt "rewrite/.build/$IPLAY_TRIAL_EXE" ]; then
    trial_binary_fresh=no
    break
  fi
done
if [ "$trial_binary_fresh" != "yes" ]; then
  echo "try_player: selected trial executable is stale after player build: $IPLAY_TRIAL_EXE" >&2
  exit 2
fi

mkdir -p rewrite/.build
dst_dir=$(CDPATH= cd -- "rewrite/.build" && pwd)
if [ "$src_dir/$name" != "$dst_dir/$name" ]; then
  cp "$src_dir/$name" "$dst_dir/$name"
fi

cd rewrite/.build
{
  printf 'host_module=%s dos_module=%s\n' "$src_dir/$name" "$name"
  printf 'host_module_size=%s\n' "$host_module_size"
  printf 'trial_exe=%s diagnostics=%s hardware_diagnostics=%s production=%s rebuild=%s needs_rebuild=%s\n' "$IPLAY_TRIAL_EXE" "$trial_diagnostics" "$trial_hardware_diagnostics" "$trial_production" "$trial_rebuild" "$needs_rebuild"
  printf 'trial_exe_path=%s\n' "$dst_dir/$IPLAY_TRIAL_EXE"
  printf 'trial_binary_fresh=%s\n' "$trial_binary_fresh"
  printf 'audio_mode=%s\n' "$trial_audio_mode"
  printf 'trial_loop_policy=%s\n' "$trial_loop_policy"
  printf 'trial_proof_scope=%s\n' "$trial_proof_scope"
  printf 'trial_video_mode=%s cols=%s rows=%s\n' "$trial_video_mode" "$trial_video_cols" "$trial_video_rows"
  if [ "$trial_diagnostics" = "0" ]; then
    printf 'trial_mode_note=quiet-player-no-diagnostic-stdout\n'
  fi
  printf 'dos_args='
  sep=
  for arg in "$@"; do
    printf '%s%s' "${sep:-}" "$arg"
    sep=' '
  done
  printf '\n'
  printf 'kvikdos_timeout_seconds=%s\n' "$KVIKDOS_SECONDS"
} > "$IPLAY_TRIAL_LOG"
set +e
"$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" "$IPLAY_TRIAL_EXE" "$@" >> "$IPLAY_TRIAL_LOG" 2>&1
rc=$?
set -e
case "$rc" in
  124|137)
    printf 'kvikdos_timeout=yes seconds=%s\n' "$KVIKDOS_SECONDS" >> "$IPLAY_TRIAL_LOG"
    if [ "$trial_diagnostics" = "0" ]; then
      printf 'quiet_trial_timeout=yes meaning=headless-run-ended-by-timeout-not-by-player-exit\n' >> "$IPLAY_TRIAL_LOG"
    fi
    ;;
esac
if [ "$trial_diagnostics" = "0" ] && [ "$rc" -eq 0 ]; then
  printf 'quiet_trial_completed=yes meaning=player-exited-without-diagnostic-stdout\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Module: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  # CR-safe variant of: trial_loaded_module_name=$(grep '^Module: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | sed 's/^Module: //')
  printf 'trial_module_loaded=yes\n' >> "$IPLAY_TRIAL_LOG"
  trial_loaded_module_name=$(grep '^Module: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r' | sed 's/^Module: //')
  printf 'trial_loaded_module_name=%s\n' "$trial_loaded_module_name" >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_module_loaded=no\n' >> "$IPLAY_TRIAL_LOG"
  trial_loaded_module_name=
  printf 'trial_loaded_module_name=none\n' >> "$IPLAY_TRIAL_LOG"
fi
trial_loaded_module_key=$(printf '%s' "$trial_loaded_module_name" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')
trial_requested_module_key=$(printf '%s' "$name" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')
printf 'trial_loaded_module_key=%s\n' "$trial_loaded_module_key" >> "$IPLAY_TRIAL_LOG"
printf 'trial_requested_module_key=%s\n' "$trial_requested_module_key" >> "$IPLAY_TRIAL_LOG"
if [ "$trial_loaded_module_key" = "$trial_requested_module_key" ]; then
  printf 'trial_requested_module_loaded=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_requested_module_loaded=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Size: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  # CR-safe variant of: trial_module_size=$(grep '^Size: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | sed 's/^Size: //; s/ bytes$//')
  trial_module_size=$(grep '^Size: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r' | sed 's/^Size: //; s/ bytes$//')
  printf 'trial_module_size=%s\n' "$trial_module_size" >> "$IPLAY_TRIAL_LOG"
else
  trial_module_size=none
  printf 'trial_module_size=none\n' >> "$IPLAY_TRIAL_LOG"
fi
if [ "$trial_module_size" = "$host_module_size" ]; then
  printf 'trial_module_size_matches_host=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_module_size_matches_host=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Loader: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  # CR-safe variant of: trial_module_loader_line=$(grep '^Loader: ' "$IPLAY_TRIAL_LOG" | tail -n 1)
  # CR-safe variant of: trial_module_loader=$(printf '%s\n' "$trial_module_loader_line" | sed 's/^Loader: //')
  trial_module_loader_line=$(grep '^Loader: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r')
  trial_module_loader=$(printf '%s\n' "$trial_module_loader_line" | sed 's/^Loader: //')
  printf 'trial_module_loader_line=%s\n' "$trial_module_loader_line" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_module_loader=%s\n' "$trial_module_loader" >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_module_loader_line=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_module_loader=none\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Module type tag: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  # CR-safe variant of: trial_module_type_tag=$(grep '^Module type tag: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | sed 's/^Module type tag: //')
  trial_module_type_tag=$(grep '^Module type tag: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r' | sed 's/^Module type tag: //')
  printf 'trial_module_type_tag=%s\n' "$trial_module_type_tag" >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_module_type_tag=none\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Title: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  # CR-safe variant of: trial_module_title=$(grep '^Title: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | sed 's/^Title: //')
  trial_module_title=$(grep '^Title: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | tr -d '\r' | sed 's/^Title: //')
  printf 'trial_module_title=%s\n' "$trial_module_title" >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_module_title=none\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Playback pump: .* stop=' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_playback_pump=yes\n' >> "$IPLAY_TRIAL_LOG"
  trial_playback_line=$(grep '^Playback pump: .* stop=' "$IPLAY_TRIAL_LOG" | tail -n 1)
  printf 'trial_playback_line=%s\n' "$trial_playback_line" >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_playback_pump=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Playback pump: blocks=[1-9][0-9]* frames=[1-9][0-9]* accepted=[1-9][0-9]* checksum=[1-9][0-9]* .* stop=' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_playback_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_playback_valid=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^PCM source: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_pcm_source_line=$(grep '^PCM source: ' "$IPLAY_TRIAL_LOG" | tail -n 1)
  printf 'trial_pcm_source_line=%s\n' "$trial_pcm_source_line" >> "$IPLAY_TRIAL_LOG"
  case "$trial_pcm_source_line" in *" provider="*) trial_pcm_provider=$(printf '%s\n' "$trial_pcm_source_line" | sed 's/^.* provider=//; s/ .*$//') ;; *) trial_pcm_provider=none ;; esac
  case "$trial_pcm_source_line" in *" renderer="*) trial_pcm_renderer=$(printf '%s\n' "$trial_pcm_source_line" | sed 's/^.* renderer=//; s/ .*$//') ;; *) trial_pcm_renderer=none ;; esac
  case "$trial_pcm_source_line" in *" route="*) trial_pcm_route=$(printf '%s\n' "$trial_pcm_source_line" | sed 's/^.* route=//; s/ .*$//') ;; *) trial_pcm_route=none ;; esac
  case "$trial_pcm_source_line" in *" input="*) trial_pcm_input=$(printf '%s\n' "$trial_pcm_source_line" | sed 's/^.* input=//; s/ .*$//') ;; *) trial_pcm_input=none ;; esac
  case "$trial_pcm_source_line" in *" truncated="*) trial_pcm_truncated=$(printf '%s\n' "$trial_pcm_source_line" | sed 's/^.* truncated=//; s/ .*$//') ;; *) trial_pcm_truncated=none ;; esac
  case "$trial_pcm_source_line" in *" hook_provider="*) trial_pcm_hook_provider=$(printf '%s\n' "$trial_pcm_source_line" | sed 's/^.* hook_provider=//; s/ .*$//') ;; *) trial_pcm_hook_provider=missing ;; esac
  case "$trial_pcm_source_line" in *" stream_start="*) trial_pcm_stream_start=$(printf '%s\n' "$trial_pcm_source_line" | sed 's/^.* stream_start=//; s/ .*$//') ;; *) trial_pcm_stream_start=none ;; esac
  printf 'trial_pcm_provider=%s\n' "$trial_pcm_provider" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_renderer=%s\n' "$trial_pcm_renderer" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_route=%s\n' "$trial_pcm_route" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_input=%s\n' "$trial_pcm_input" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_truncated=%s\n' "$trial_pcm_truncated" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_hook_provider=%s\n' "$trial_pcm_hook_provider" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_stream_start=%s\n' "$trial_pcm_stream_start" >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_pcm_source_line=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_provider=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_renderer=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_route=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_input=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_truncated=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_hook_provider=missing\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_pcm_stream_start=none\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Decoder route: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_decoder_route_line=$(grep '^Decoder route: ' "$IPLAY_TRIAL_LOG" | tail -n 1)
  printf 'trial_decoder_route_line=%s\n' "$trial_decoder_route_line" >> "$IPLAY_TRIAL_LOG"
  case "$trial_decoder_route_line" in *" id="*) trial_decoder_route_id=$(printf '%s\n' "$trial_decoder_route_line" | sed 's/^.* id=//; s/ .*$//') ;; *) trial_decoder_route_id=none ;; esac
  case "$trial_decoder_route_line" in *" name="*) trial_decoder_route_name=$(printf '%s\n' "$trial_decoder_route_line" | sed 's/^.* name=//; s/ .*$//') ;; *) trial_decoder_route_name=none ;; esac
  printf 'trial_decoder_route_id=%s\n' "$trial_decoder_route_id" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_decoder_route_name=%s\n' "$trial_decoder_route_name" >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_decoder_route_line=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_decoder_route_id=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_decoder_route_name=none\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Decoder handoff: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_decoder_handoff_line=$(grep '^Decoder handoff: ' "$IPLAY_TRIAL_LOG" | tail -n 1)
  trial_decoder_handoff=$(printf '%s\n' "$trial_decoder_handoff_line" | sed 's/^Decoder handoff: //')
  printf 'trial_decoder_handoff_line=%s\n' "$trial_decoder_handoff_line" >> "$IPLAY_TRIAL_LOG"
  printf 'trial_decoder_handoff=%s\n' "$trial_decoder_handoff" >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_decoder_handoff_line=none\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_decoder_handoff=none\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Screen present: ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_screen_present=yes\n' >> "$IPLAY_TRIAL_LOG"
  trial_screen_reasons=$(grep '^Screen present: ' "$IPLAY_TRIAL_LOG" | sed 's/^Screen present: reason=//; s/ .*//' | tr '\n' ',' | sed 's/,$//')
  printf 'trial_screen_reasons=%s\n' "$trial_screen_reasons" >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_screen_present=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Screen present: reason=playback-position ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_playback_position_present=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_playback_position_present=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Screen present: reason=playback-position .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_playback_position_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_playback_position_valid=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep "^Screen present: reason=playback-position .* cols=$trial_video_cols rows=$trial_video_rows " "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_playback_position_geometry_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_playback_position_geometry_valid=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Screen present: reason=post-playback-status ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_post_playback_status_present=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_post_playback_status_present=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Screen present: reason=post-playback-status .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_post_playback_status_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_post_playback_status_valid=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep "^Screen present: reason=post-playback-status .* cols=$trial_video_cols rows=$trial_video_rows " "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_post_playback_status_geometry_valid=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_post_playback_status_geometry_valid=no\n' >> "$IPLAY_TRIAL_LOG"
fi
if grep '^Screen present: reason=audio-unavailable ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  printf 'trial_audio_unavailable=yes\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_audio_unavailable_source=screen\n' >> "$IPLAY_TRIAL_LOG"
elif [ "$rc" -eq 3 ] && [ "$trial_audio_mode" = "real-sb16-hardware" ]; then
  printf 'trial_audio_unavailable=yes\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_audio_unavailable_source=exit-code\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_audio_unavailable=no\n' >> "$IPLAY_TRIAL_LOG"
  printf 'trial_audio_unavailable_source=none\n' >> "$IPLAY_TRIAL_LOG"
fi
trial_ok_loader_metadata=0
if grep '^trial_module_loader=[^ ]' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_module_loader=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_module_type_tag=[0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F]$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_module_type_tag=00000000$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_ok_loader_metadata=1
fi
if [ "$trial_ok_loader_metadata" = "1" ]; then
  printf 'trial_ok_loader_metadata=yes\n' >> "$IPLAY_TRIAL_LOG"
else
  printf 'trial_ok_loader_metadata=no\n' >> "$IPLAY_TRIAL_LOG"
fi
trial_result=
trial_script_exit_status=$rc
if [ "$rc" -eq 0 ] && [ "$trial_ok_loader_metadata" = "1" ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_module_size_matches_host=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_position_geometry_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_post_playback_status_geometry_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_decoder_route_id=[0-9][0-9]*$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_decoder_route_name=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_decoder_handoff=[^ ]' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_decoder_handoff=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_pcm_provider=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_pcm_input=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_pcm_hook_provider=missing$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_pcm_stream_start=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^Playback pump: .* stop=block-limit' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=bounded-ui-playback-ok
elif [ "$rc" -eq 0 ] && [ "$trial_ok_loader_metadata" = "1" ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_module_size_matches_host=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_position_geometry_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_post_playback_status_geometry_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_decoder_route_id=[0-9][0-9]*$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_decoder_route_name=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_decoder_handoff=[^ ]' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_decoder_handoff=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_pcm_provider=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_pcm_input=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_pcm_hook_provider=missing$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_pcm_stream_start=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^Playback pump: .* stop=source-end' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=source-ended-ui-ok
elif [ "$rc" -eq 0 ] && [ "$trial_loop_policy" = "continuous-quiet-wrapper" ] && grep '^quiet_trial_completed=yes ' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=quiet-completed-no-diagnostics
elif [ "$rc" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_module_size_matches_host=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=module-size-mismatch
  trial_script_exit_status=4
elif [ "$rc" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_ok_loader_metadata=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=loader-metadata-invalid
  trial_script_exit_status=4
elif [ "$rc" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && { grep '^trial_decoder_route_line=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_decoder_route_id=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_decoder_route_name=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; }; then
  trial_result=decoder-route-missing
  trial_script_exit_status=4
elif [ "$rc" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_decoder_handoff=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=decoder-handoff-missing
  trial_script_exit_status=4
elif [ "$rc" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && { grep '^trial_pcm_source_line=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_pcm_provider=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_pcm_input=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_pcm_hook_provider=missing$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_pcm_stream_start=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; }; then
  trial_result=pcm-source-missing
  trial_script_exit_status=4
elif [ "$rc" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_pump=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=playback-pump-invalid
  trial_script_exit_status=4
elif [ "$rc" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && { grep '^trial_playback_position_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_playback_position_geometry_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; }; then
  trial_result=screen-evidence-invalid
  trial_script_exit_status=4
elif [ "$rc" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^Playback pump: .* stop=block-limit' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && { grep '^trial_post_playback_status_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_post_playback_status_geometry_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; }; then
  trial_result=post-screen-evidence-invalid
  trial_script_exit_status=4
elif [ "$rc" -eq 0 ] && grep '^trial_requested_module_loaded=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=requested-module-not-loaded
  trial_script_exit_status=4
elif [ "$rc" -eq 0 ] && grep '^Playback pump: .* stop=' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=playback-without-screen
  trial_script_exit_status=4
elif grep '^trial_audio_unavailable=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then
  trial_result=audio-unavailable
  if [ "$rc" -eq 0 ]; then
    trial_script_exit_status=4
  fi
elif [ "$rc" -eq 124 ] || [ "$rc" -eq 137 ]; then
  trial_result=kvikdos-timeout
elif [ "$rc" -eq 0 ]; then
  trial_result=exited-without-playback-pump
  trial_script_exit_status=4
else
  trial_result=failed
fi
case "$trial_result" in
  bounded-ui-playback-ok|source-ended-ui-ok|quiet-completed-no-diagnostics)
    trial_failure_reason=none
    ;;
  playback-without-screen)
    trial_failure_reason=audio-pump-without-valid-screen-present
    ;;
  loader-metadata-invalid)
    trial_failure_reason=loader-metadata-invalid
    ;;
  decoder-route-missing)
    trial_failure_reason=decoder-route-missing
    ;;
  module-size-mismatch)
    trial_failure_reason=module-size-mismatch
    ;;
  decoder-handoff-missing)
    trial_failure_reason=decoder-handoff-missing
    ;;
  pcm-source-missing)
    trial_failure_reason=pcm-source-missing
    ;;
  playback-pump-invalid)
    trial_failure_reason=playback-pump-invalid
    ;;
  screen-evidence-invalid)
    trial_failure_reason=screen-evidence-invalid
    ;;
  post-screen-evidence-invalid)
    trial_failure_reason=post-screen-evidence-invalid
    ;;
  requested-module-not-loaded)
    trial_failure_reason=requested-module-not-loaded
    ;;
  audio-unavailable)
    trial_failure_reason=sb16-audio-unavailable
    ;;
  kvikdos-timeout)
    trial_failure_reason=emulator-timeout
    ;;
  exited-without-playback-pump)
    trial_failure_reason=no-playback-pump-evidence
    ;;
  failed)
    trial_failure_reason=player-process-failed
    ;;
  *)
    trial_failure_reason=unknown
    ;;
esac
printf 'trial_result=%s\n' "$trial_result" >> "$IPLAY_TRIAL_LOG"
printf 'trial_failure_reason=%s\n' "$trial_failure_reason" >> "$IPLAY_TRIAL_LOG"
printf 'trial_exe=%s exit_status=%s\n' "$IPLAY_TRIAL_EXE" "$rc" >> "$IPLAY_TRIAL_LOG"
printf 'trial_script_exit_status=%s\n' "$trial_script_exit_status" >> "$IPLAY_TRIAL_LOG"
cat "$IPLAY_TRIAL_LOG"
exit "$trial_script_exit_status"
