#!/bin/sh
set -eu
cd "$(dirname "$0")/.."

run_pytest_no_skip() {
  log=$(mktemp)
  set +e
  "$@" >"$log" 2>&1
  rc=$?
  set -e
  cat "$log"
  if [ "$rc" -ne 0 ]; then
    rm -f "$log"
    exit "$rc"
  fi
  if grep -Eiq '([0-9]+ skipped|xfailed|xpassed)' "$log"; then
    echo "pytest stage reported skipped/xfail/xpass tests: $*" >&2
    rm -f "$log"
    exit 1
  fi
  rm -f "$log"
}

run_clean_normal_playback() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  printf 'q\n' | SDL_AUDIODRIVER=dummy COLUMNS=80 LINES=50 "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit "$rc"
  fi
  if ! grep 'Inertia Player V1\.22' "$out" >/dev/null; then
    echo "normal playback did not render the Inertia title: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! python3 - "$out" <<'PY'
import pathlib
import sys

data = pathlib.Path(sys.argv[1]).read_bytes()
required = [
    b"\x1b[?25l",
    b"\x1b[?7l",
    b"\x1b[38;2;255;255;255;48;2;170;170;170m",
    b"\x1b[38;2;255;255;85;48;2;",
    b"\x1b[38;2;85;85;85;48;2;170;170;170m",
    b"Filename      : ",
    b"aryx.s3m",
    b"Playing in Stereo",
    b"Current Track",
    b"Track Position",
    b"Main Volume",
    b"Module Type   : ",
    b"S3M",
    b"24bit Interpolation",
    b"F-12",
]
missing = [escape for escape in required if escape not in data]
if b"Output Levels : " not in data and b"Sound Blaster 16" not in data:
    missing.append(b"Output Levels : | Sound Blaster 16")
if missing:
    print("missing required terminal UI bytes: " + ", ".join(repr(escape) for escape in missing), file=sys.stderr)
    sys.exit(1)
PY
  then
    echo "normal playback did not emit required notcurses-style status UI: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Terminal render end|^(File list|Module|Size|Loader|Module type tag|Title|Terminal render|SDL audio sink|Stdin keyboard|status=|Playback pump):' "$out"; then
    echo "normal playback leaked diagnostic output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "normal playback wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_missing_module_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=not-a-real-driver "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "missing module command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'Module not found.' "$err" >/dev/null; then
    echo "missing module command did not report original-style not found: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -F 'could not open SDL2 SB16 stereo audio sink' "$err" >/dev/null; then
    echo "missing module command tried to open SDL audio before failing: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Inertia Player|^(File list|Module|Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out"; then
    echo "missing module command leaked playback output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_invalid_video_mode_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "invalid video-mode command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay: unsupported text mode: bad' "$err" >/dev/null; then
    echo "invalid video-mode command did not report unsupported text mode: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: player exited with status 2' "$err" >/dev/null; then
    echo "invalid video-mode command did not report wrapper failure status: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -F 'could not open SDL2 SB16 stereo audio sink' "$err" >/dev/null; then
    echo "invalid video-mode command tried to open SDL audio before failing: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Inertia Player|^(File list|Module|Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out"; then
    echo "invalid video-mode command leaked playback output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_missing_filelist_failure() {
  out=$(mktemp)
  err=$(mktemp)
  filelist_arg=$2
  set +e
  SDL_AUDIODRIVER=not-a-real-driver "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "missing file-list command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F "iplay: could not resolve file list: $filelist_arg" "$err" >/dev/null; then
    echo "missing file-list command did not report clear file-list error: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: player exited with status 2' "$err" >/dev/null; then
    echo "missing file-list command did not report wrapper failure status: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -F 'could not open SDL2 SB16 stereo audio sink' "$err" >/dev/null; then
    echo "missing file-list command tried to open SDL audio before failing: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Inertia Player|^(File list|Module|Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out"; then
    echo "missing file-list command leaked playback output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_check_playback_invalid_video_mode_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "invalid video-mode readiness command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$out" ]; then
    echo "invalid video-mode readiness command wrote stdout: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay: unsupported text mode: bad' "$err" >/dev/null; then
    echo "invalid video-mode readiness command did not report unsupported text mode: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: playback readiness check failed with status 2' "$err" >/dev/null; then
    echo "invalid video-mode readiness command did not report readiness failure status: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'could not open SDL2 SB16 stereo audio sink|iplay\.sh: playback-ready:|Playback pump:|Terminal render:|status=' "$err"; then
    echo "invalid video-mode readiness command leaked wrong readiness state: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_check_playback_missing_filelist_failure() {
  out=$(mktemp)
  err=$(mktemp)
  filelist_arg=$3
  set +e
  SDL_AUDIODRIVER=not-a-real-driver "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "missing file-list readiness command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$out" ]; then
    echo "missing file-list readiness command wrote stdout: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F "iplay: could not resolve file list: $filelist_arg" "$err" >/dev/null; then
    echo "missing file-list readiness command did not report clear file-list error: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: playback readiness check failed with status 2' "$err" >/dev/null; then
    echo "missing file-list readiness command did not report readiness failure status: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'could not open SDL2 SB16 stereo audio sink|File list:|Playback pump:|Terminal render:|status=' "$err"; then
    echo "missing file-list readiness command leaked wrong readiness state: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_corrupt_external_tracker_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 3 ]; then
    echo "corrupt external tracker command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: playback failed: status=external-decoder-failed route_id=0 route=external-library provider=libmikmod' "$err" >/dev/null; then
    echo "corrupt external tracker command did not report libmodplug decoder failure: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq '^(Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out"; then
    echo "corrupt external tracker command leaked raw diagnostic output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_project_owned_decoder_unavailable() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 3 ]; then
    echo "project-owned decoder command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: playback failed: status=project-decoder-unavailable route_id=1 route=project-owned provider=native' "$err" >/dev/null; then
    echo "project-owned decoder command did not report unavailable native decoder: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq '^(Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out"; then
    echo "project-owned decoder command leaked raw diagnostic output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_unsupported_probe_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 3 ]; then
    echo "unsupported probe command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: playback failed: status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod' "$err" >/dev/null; then
    echo "unsupported probe command did not report probe-by-content unsupported format: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq '^(Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out"; then
    echo "unsupported probe command leaked raw diagnostic output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_check_playback_decoder_failure() {
  expected_status=$1
  shift
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "decoder readiness failure command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$out" ]; then
    echo "decoder readiness failure command wrote stdout: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F "iplay.sh: playback readiness check failed: $expected_status" "$err" >/dev/null; then
    echo "decoder readiness failure command did not report expected status: $expected_status: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: playback readiness check failed with status 3' "$err" >/dev/null; then
    echo "decoder readiness failure command did not report wrapper status 3: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'iplay\.sh: playback-ready:|Module:|Terminal render:|Playback pump:' "$err"; then
    echo "decoder readiness failure command leaked playback-ready or raw diagnostics: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_sdl_audio_open_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=not-a-real-driver "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "SDL audio open failure command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'could not open SDL2 SB16 stereo audio sink requested freq=44100 format=0x8010 channels=2 samples=1024:' "$err" >/dev/null; then
    echo "SDL audio open failure command did not report SB16 stereo sink failure: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq '^(Playback pump|status=):|SDL audio sink: requested=1 opened=1' "$out"; then
    echo "SDL audio open failure command leaked successful playback output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_check_playback_sdl_audio_open_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=not-a-real-driver "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "SDL audio readiness failure command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$out" ]; then
    echo "SDL audio readiness failure command wrote stdout: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'could not open SDL2 SB16 stereo audio sink requested freq=44100 format=0x8010 channels=2 samples=1024:' "$err" >/dev/null; then
    echo "SDL audio readiness failure command did not report SB16 stereo sink failure: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: playback readiness check failed with status 2' "$err" >/dev/null; then
    echo "SDL audio readiness failure command did not report readiness failure status: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Module not found\.|iplay\.sh: playback-ready:|Playback pump:|status=' "$err"; then
    echo "SDL audio readiness failure command leaked wrong readiness state: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_diagnostics_playback_evidence() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit "$rc"
  fi
  for required in \
    'Module: aryx.s3m' \
    'Selected text mode: 80x50 cols=80 rows=50' \
    'Terminal render: requested=1 cols=80 rows=50 bytes=8000' \
    'SDL audio sink: requested=1 opened=1' \
    'Terminal live summary: requested=1' \
    'Playback output: SDL-compatible SB16 16-bit stereo native.' \
    'Decoder route: id=0 name=external-library' \
    'PCM source: external_module seed=1 truncated=0 input=file-path renderer=e route=0 provider=libmikmod hook_provider=libmikmod stream_start=0' \
    'status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1'
  do
    if ! grep -F "$required" "$out" >/dev/null; then
      echo "diagnostics playback missing evidence: $required" >&2
      cat "$out"
      cat "$err" >&2
      rm -f "$out" "$err"
      exit 1
    fi
  done
  for arg in "$@"; do
    case "$arg" in
      @*)
        if ! grep -E '^File list: @.*[[:alnum:]_-]+\.LST selected=.*aryx\.s3m$' "$out" >/dev/null; then
          echo "diagnostics playback missing file-list selection evidence: $arg" >&2
          cat "$out"
          cat "$err" >&2
          rm -f "$out" "$err"
          exit 1
        fi
        ;;
    esac
  done
  if [ -s "$err" ]; then
    echo "diagnostics playback wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_auto_size_diagnostics_evidence() {
  columns=$1
  lines=$2
  expected_mode=$3
  expected_render=$4
  shift 4
  out=$(mktemp)
  err=$(mktemp)
  set +e
  printf 'q\n' | SDL_AUDIODRIVER=dummy COLUMNS=$columns LINES=$lines "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit "$rc"
  fi
  for required in \
    "Selected text mode: $expected_mode" \
    "$expected_render" \
    'SDL audio sink: requested=1 opened=1' \
    'status=keyboard route_id=0 route=external-library provider=libmikmod'
  do
    if ! grep -F "$required" "$out" >/dev/null; then
      echo "auto-size diagnostics missing evidence: $required" >&2
      cat "$out"
      cat "$err" >&2
      rm -f "$out" "$err"
      exit 1
    fi
  done
  if [ -s "$err" ]; then
    echo "auto-size diagnostics wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

check_external_tracker_extensions() {
  out=$("$@" --list-extensions)
  IPLAY_EXTENSION_LIST=$out python3 - <<'PY'
import os
import re
import sys

text = os.environ["IPLAY_EXTENSION_LIST"].strip()
if not text.startswith("extensions="):
    print("external tracker extension list must start with extensions=", file=sys.stderr)
    sys.exit(1)
extensions = [extension.strip() for extension in text[len("extensions="):].split(",") if extension.strip()]
if not extensions:
    print("external tracker extension list is empty", file=sys.stderr)
    sys.exit(1)
bad = [extension for extension in extensions if not re.fullmatch(r"\.[0-9a-z]+", extension)]
if bad:
    print("external tracker extension list contains malformed entries: " + ", ".join(bad), file=sys.stderr)
    sys.exit(1)
duplicates = sorted({extension for extension in extensions if extensions.count(extension) > 1})
if duplicates:
    print("external tracker extension list contains duplicates: " + ", ".join(duplicates), file=sys.stderr)
    sys.exit(1)
PY
  for extension in .mod .nst .s3m .stm .669 .mtm .psm .far .ult .wow .okt .oct .xm .it .ptm .ams .dbm .dmf .mdl .dsm .med .imf .j2b; do
    if ! printf '%s\n' "$out" | grep -F "$extension" >/dev/null; then
      echo "external tracker extension missing from --list-extensions: $extension" >&2
      printf '%s\n' "$out" >&2
      exit 1
    fi
  done
  if printf '%s\n' "$out" | grep -F '.inr' >/dev/null; then
    echo "project-owned .inr must not be advertised as an external tracker extension" >&2
    printf '%s\n' "$out" >&2
    exit 1
  fi
}

check_external_tracker_classification() {
  launcher=$1
  for extension in mod nst s3m stm 669 mtm psm far ult wow okt oct xm it ptm ams dbm dmf mdl dsm med imf j2b; do
    "$launcher" --classify "example.$extension" | grep 'route_id=0 route=external-library library=1' >/dev/null
  done
}

check_decoder_route_classification() {
  launcher=$1
  path=$2
  expected=$3
  out=$("$launcher" --classify "$path" | tr -d '\r')
  if [ "$out" != "$expected" ]; then
    echo "decoder route classification mismatch for $path" >&2
    echo "expected: $expected" >&2
    echo "actual:   $out" >&2
    exit 1
  fi
}

run_check_playback_ready() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "playback readiness command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -E 'iplay\.sh: playback-ready: exe=rewrite/\.build/iplay video=(40x25|80x25|80x50) audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod' "$out" >/dev/null; then
    echo "playback readiness command did not report ready state: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq '^(File list|Module|Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out"; then
    echo "playback readiness command leaked raw playback diagnostics: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "playback readiness command wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_check_playback_ready_modes() {
  launcher=$1
  module=$2
  for mode in 40x25 40x25bw 40x25color 80x25 80x25bw 80x25color 80x50 80x50project auto terminal; do
    run_check_playback_ready "$launcher" --check-playback "$module" --video-mode="$mode"
  done
}

playback_check_tmp=
cleanup_playback_check_tmp() {
  if [ -n "$playback_check_tmp" ]; then
    rm -rf "$playback_check_tmp"
  fi
}
trap cleanup_playback_check_tmp EXIT

./rewrite/build_rewrite.sh

test -s rewrite/.build/IPLAYC.EXE
test -s rewrite/.build/IPLAYC.map
test -s rewrite/.build/IPLAYTRY.EXE
test -s rewrite/.build/IPLAYTRY.map
test -s rewrite/.build/IPLAYCONT.EXE
test -s rewrite/.build/IPLAYCONT.map
test -s rewrite/.build/IPLAYDIAG.EXE
test -s rewrite/.build/IPLAYDIAG.map
test -s rewrite/.build/IPLAYHW.EXE
test -s rewrite/.build/IPLAYHW.map
test -s rewrite/.build/IRUN.EXE
test -s rewrite/.build/IRUN.map
test -s rewrite/.build/IUIRUN.EXE
test -s rewrite/.build/IUIRUN.map
test -s rewrite/.build/IARUN.EXE
test -s rewrite/.build/IARUN.map
test -s rewrite/.build/IPHWRUN.EXE
test -s rewrite/.build/IPHWRUN.map
test -x rewrite/.build/iplay_modern_host
test -x rewrite/.build/iplay_native
test -x rewrite/.build/iplay
test -x rewrite/iplay.sh
test -x iplay.sh
./iplay.sh --check >/dev/null
run_invalid_video_mode_failure ./iplay.sh --video-mode=bad samples/aryx.s3m
run_invalid_video_mode_failure ./iplay.sh samples/aryx.s3m --video-mode=bad
run_check_playback_invalid_video_mode_failure ./iplay.sh --check-playback --video-mode=bad samples/aryx.s3m
run_check_playback_invalid_video_mode_failure ./iplay.sh --check-playback samples/aryx.s3m --video-mode=bad
run_clean_normal_playback ./iplay.sh samples/aryx.s3m
run_clean_normal_playback ./iplay.sh --video-mode=40x25 samples/aryx.s3m
run_clean_normal_playback ./iplay.sh --video-mode=40x25bw samples/aryx.s3m
run_clean_normal_playback ./iplay.sh --video-mode=40x25color samples/aryx.s3m
run_clean_normal_playback ./iplay.sh --video-mode=80x25 samples/aryx.s3m
run_clean_normal_playback ./iplay.sh --video-mode=80x25bw samples/aryx.s3m
run_clean_normal_playback ./iplay.sh --video-mode=80x25color samples/aryx.s3m
run_clean_normal_playback ./iplay.sh --video-mode=80x50 samples/aryx.s3m
run_clean_normal_playback ./iplay.sh samples/aryx.s3m --video-mode=80x50
run_clean_normal_playback ./iplay.sh --video-mode=80x50project samples/aryx.s3m
run_clean_normal_playback ./iplay.sh --video-mode=auto samples/aryx.s3m
run_clean_normal_playback ./iplay.sh --video-mode=terminal samples/aryx.s3m
run_diagnostics_playback_evidence ./iplay.sh --diagnostics --video-mode=80x50 samples/aryx.s3m
run_auto_size_diagnostics_evidence 40 25 'auto cols=40 rows=25' 'Terminal render: requested=1 cols=40 rows=25 bytes=2000' ./iplay.sh --diagnostics samples/aryx.s3m
run_auto_size_diagnostics_evidence 80 25 'auto cols=80 rows=25' 'Terminal render: requested=1 cols=80 rows=25 bytes=4000' ./iplay.sh --diagnostics samples/aryx.s3m
run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics samples/aryx.s3m
run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics --video-mode=auto samples/aryx.s3m
run_auto_size_diagnostics_evidence 80 50 'terminal cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics --video-mode=terminal samples/aryx.s3m
./iplay.sh --check-playback samples/aryx.s3m | grep 'iplay\.sh: playback-ready: exe=rewrite/\.build/iplay video=40x25 audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod' >/dev/null
run_check_playback_ready_modes ./iplay.sh samples/aryx.s3m
playback_check_tmp=$(mktemp -d)
cp samples/aryx.s3m "$playback_check_tmp/aryx.s3m"
printf 'not a tracker module\n' > "$playback_check_tmp/BAD.S3M"
printf 'INR placeholder\n' > "$playback_check_tmp/SONG.INR"
printf 'not a tracker module\n' > "$playback_check_tmp/BAD.BIN"
printf '\n\t aryx.s3m \r\nignored.s3m\n' > "$playback_check_tmp/PLAYLIST.LST"
printf 'aryx.s3m\n' > "$playback_check_tmp/caseplay.lst"
printf '\n\t \r\n' > "$playback_check_tmp/EMPTY.LST"
run_missing_module_failure ./iplay.sh "$playback_check_tmp/MISSING.S3M"
run_missing_filelist_failure ./iplay.sh "@$playback_check_tmp/MISSING.LST"
run_missing_filelist_failure ./iplay.sh "@$playback_check_tmp/EMPTY.LST"
run_check_playback_missing_filelist_failure ./iplay.sh --check-playback "@$playback_check_tmp/MISSING.LST"
run_check_playback_missing_filelist_failure ./iplay.sh --check-playback "@$playback_check_tmp/EMPTY.LST"
run_corrupt_external_tracker_failure ./iplay.sh "$playback_check_tmp/BAD.S3M"
run_project_owned_decoder_unavailable ./iplay.sh "$playback_check_tmp/SONG.INR"
run_unsupported_probe_failure ./iplay.sh "$playback_check_tmp/BAD.BIN"
run_check_playback_decoder_failure 'status=external-decoder-failed route_id=0 route=external-library provider=libmikmod' ./iplay.sh --check-playback "$playback_check_tmp/BAD.S3M"
run_check_playback_decoder_failure 'status=project-decoder-unavailable route_id=1 route=project-owned provider=native' ./iplay.sh --check-playback "$playback_check_tmp/SONG.INR"
run_check_playback_decoder_failure 'status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod' ./iplay.sh --check-playback "$playback_check_tmp/BAD.BIN"
run_sdl_audio_open_failure ./iplay.sh "$playback_check_tmp/aryx.s3m"
run_check_playback_sdl_audio_open_failure ./iplay.sh --check-playback "$playback_check_tmp/aryx.s3m"
run_check_playback_sdl_audio_open_failure ./iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST"
run_check_playback_sdl_audio_open_failure ./iplay.sh --check-playback "@$playback_check_tmp/CASEPLAY.LST"
run_invalid_video_mode_failure ./iplay.sh "@$playback_check_tmp/PLAYLIST.LST" --video-mode=bad
run_check_playback_invalid_video_mode_failure ./iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST" --video-mode=bad
run_clean_normal_playback ./iplay.sh "@$playback_check_tmp/PLAYLIST.LST"
run_clean_normal_playback ./iplay.sh "@$playback_check_tmp/CASEPLAY.LST"
run_clean_normal_playback ./iplay.sh --video-mode=auto "@$playback_check_tmp/PLAYLIST.LST"
run_clean_normal_playback ./iplay.sh --video-mode=terminal "@$playback_check_tmp/PLAYLIST.LST"
run_clean_normal_playback ./iplay.sh --video-mode=auto "@$playback_check_tmp/CASEPLAY.LST"
run_clean_normal_playback ./iplay.sh --video-mode=terminal "@$playback_check_tmp/CASEPLAY.LST"
run_diagnostics_playback_evidence ./iplay.sh --diagnostics --video-mode=80x50 "@$playback_check_tmp/PLAYLIST.LST"
run_diagnostics_playback_evidence ./iplay.sh --diagnostics --video-mode=80x50 "@$playback_check_tmp/CASEPLAY.LST"
run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics --video-mode=auto "@$playback_check_tmp/PLAYLIST.LST"
run_auto_size_diagnostics_evidence 80 50 'terminal cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics --video-mode=terminal "@$playback_check_tmp/PLAYLIST.LST"
run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics --video-mode=auto "@$playback_check_tmp/CASEPLAY.LST"
run_auto_size_diagnostics_evidence 80 50 'terminal cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics --video-mode=terminal "@$playback_check_tmp/CASEPLAY.LST"
./iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST" | grep 'iplay\.sh: playback-ready: exe=rewrite/\.build/iplay video=40x25 audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod' >/dev/null
./iplay.sh --check-playback "@$playback_check_tmp/CASEPLAY.LST" | grep 'iplay\.sh: playback-ready: exe=rewrite/\.build/iplay video=40x25 audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod' >/dev/null
run_check_playback_ready_modes ./iplay.sh "@$playback_check_tmp/PLAYLIST.LST"
run_check_playback_ready_modes ./iplay.sh "@$playback_check_tmp/CASEPLAY.LST"
check_external_tracker_extensions ./iplay.sh
check_external_tracker_classification ./iplay.sh
check_decoder_route_classification ./iplay.sh example.s3m 'external=1 project=0 route_id=0 route=external-library library=1 backend="SDL-compatible SB16 16-bit stereo"'
check_decoder_route_classification ./iplay.sh example.inr 'external=0 project=1 route_id=1 route=project-owned library=0 backend="SDL-compatible SB16 16-bit stereo"'
check_decoder_route_classification ./iplay.sh example.bin 'external=0 project=0 route_id=2 route=probe-by-content library=1 backend="SDL-compatible SB16 16-bit stereo"'
./iplay.sh --classify example.inr | grep 'route_id=1 route=project-owned library=0' >/dev/null
./iplay.sh --classify example.bin | grep 'route_id=2 route=probe-by-content library=1' >/dev/null
./rewrite/iplay.sh --check >/dev/null
run_invalid_video_mode_failure ./rewrite/iplay.sh --video-mode=bad samples/aryx.s3m
run_invalid_video_mode_failure ./rewrite/iplay.sh samples/aryx.s3m --video-mode=bad
run_check_playback_invalid_video_mode_failure ./rewrite/iplay.sh --check-playback --video-mode=bad samples/aryx.s3m
run_check_playback_invalid_video_mode_failure ./rewrite/iplay.sh --check-playback samples/aryx.s3m --video-mode=bad
run_clean_normal_playback ./rewrite/iplay.sh samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=40x25 samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=40x25bw samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=40x25color samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x25 samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x25bw samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x25color samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x50 samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh samples/aryx.s3m --video-mode=80x50
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x50project samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=auto samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=terminal samples/aryx.s3m
run_diagnostics_playback_evidence ./rewrite/iplay.sh --diagnostics --video-mode=80x50 samples/aryx.s3m
run_auto_size_diagnostics_evidence 40 25 'auto cols=40 rows=25' 'Terminal render: requested=1 cols=40 rows=25 bytes=2000' ./rewrite/iplay.sh --diagnostics samples/aryx.s3m
run_auto_size_diagnostics_evidence 80 25 'auto cols=80 rows=25' 'Terminal render: requested=1 cols=80 rows=25 bytes=4000' ./rewrite/iplay.sh --diagnostics samples/aryx.s3m
run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics samples/aryx.s3m
run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics --video-mode=auto samples/aryx.s3m
run_auto_size_diagnostics_evidence 80 50 'terminal cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics --video-mode=terminal samples/aryx.s3m
run_missing_module_failure ./rewrite/iplay.sh "$playback_check_tmp/MISSING.S3M"
run_missing_filelist_failure ./rewrite/iplay.sh "@$playback_check_tmp/MISSING.LST"
run_missing_filelist_failure ./rewrite/iplay.sh "@$playback_check_tmp/EMPTY.LST"
run_check_playback_missing_filelist_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/MISSING.LST"
run_check_playback_missing_filelist_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/EMPTY.LST"
run_corrupt_external_tracker_failure ./rewrite/iplay.sh "$playback_check_tmp/BAD.S3M"
run_project_owned_decoder_unavailable ./rewrite/iplay.sh "$playback_check_tmp/SONG.INR"
run_unsupported_probe_failure ./rewrite/iplay.sh "$playback_check_tmp/BAD.BIN"
run_check_playback_decoder_failure 'status=external-decoder-failed route_id=0 route=external-library provider=libmikmod' ./rewrite/iplay.sh --check-playback "$playback_check_tmp/BAD.S3M"
run_check_playback_decoder_failure 'status=project-decoder-unavailable route_id=1 route=project-owned provider=native' ./rewrite/iplay.sh --check-playback "$playback_check_tmp/SONG.INR"
run_check_playback_decoder_failure 'status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod' ./rewrite/iplay.sh --check-playback "$playback_check_tmp/BAD.BIN"
run_sdl_audio_open_failure ./rewrite/iplay.sh "$playback_check_tmp/aryx.s3m"
run_check_playback_sdl_audio_open_failure ./rewrite/iplay.sh --check-playback "$playback_check_tmp/aryx.s3m"
run_check_playback_sdl_audio_open_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST"
run_check_playback_sdl_audio_open_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/CASEPLAY.LST"
run_invalid_video_mode_failure ./rewrite/iplay.sh "@$playback_check_tmp/PLAYLIST.LST" --video-mode=bad
run_check_playback_invalid_video_mode_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST" --video-mode=bad
run_clean_normal_playback ./rewrite/iplay.sh "@$playback_check_tmp/PLAYLIST.LST"
run_clean_normal_playback ./rewrite/iplay.sh "@$playback_check_tmp/CASEPLAY.LST"
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=auto "@$playback_check_tmp/PLAYLIST.LST"
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=terminal "@$playback_check_tmp/PLAYLIST.LST"
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=auto "@$playback_check_tmp/CASEPLAY.LST"
run_clean_normal_playback ./rewrite/iplay.sh --video-mode=terminal "@$playback_check_tmp/CASEPLAY.LST"
run_diagnostics_playback_evidence ./rewrite/iplay.sh --diagnostics --video-mode=80x50 "@$playback_check_tmp/PLAYLIST.LST"
run_diagnostics_playback_evidence ./rewrite/iplay.sh --diagnostics --video-mode=80x50 "@$playback_check_tmp/CASEPLAY.LST"
run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics --video-mode=auto "@$playback_check_tmp/PLAYLIST.LST"
run_auto_size_diagnostics_evidence 80 50 'terminal cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics --video-mode=terminal "@$playback_check_tmp/PLAYLIST.LST"
run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics --video-mode=auto "@$playback_check_tmp/CASEPLAY.LST"
run_auto_size_diagnostics_evidence 80 50 'terminal cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics --video-mode=terminal "@$playback_check_tmp/CASEPLAY.LST"
./rewrite/iplay.sh --check-playback samples/aryx.s3m | grep 'iplay\.sh: playback-ready: exe=rewrite/\.build/iplay video=40x25 audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod' >/dev/null
run_check_playback_ready_modes ./rewrite/iplay.sh samples/aryx.s3m
./rewrite/iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST" | grep 'iplay\.sh: playback-ready: exe=rewrite/\.build/iplay video=40x25 audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod' >/dev/null
./rewrite/iplay.sh --check-playback "@$playback_check_tmp/CASEPLAY.LST" | grep 'iplay\.sh: playback-ready: exe=rewrite/\.build/iplay video=40x25 audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod' >/dev/null
run_check_playback_ready_modes ./rewrite/iplay.sh "@$playback_check_tmp/PLAYLIST.LST"
run_check_playback_ready_modes ./rewrite/iplay.sh "@$playback_check_tmp/CASEPLAY.LST"
./rewrite/iplay.sh --rebuild --check | grep 'iplay\.sh: ready: exe=rewrite/\.build/iplay rebuilt=1' >/dev/null
check_external_tracker_extensions ./rewrite/iplay.sh
check_external_tracker_classification ./rewrite/iplay.sh
check_decoder_route_classification ./rewrite/iplay.sh example.s3m 'external=1 project=0 route_id=0 route=external-library library=1 backend="SDL-compatible SB16 16-bit stereo"'
check_decoder_route_classification ./rewrite/iplay.sh example.inr 'external=0 project=1 route_id=1 route=project-owned library=0 backend="SDL-compatible SB16 16-bit stereo"'
check_decoder_route_classification ./rewrite/iplay.sh example.bin 'external=0 project=0 route_id=2 route=probe-by-content library=1 backend="SDL-compatible SB16 16-bit stereo"'
./rewrite/iplay.sh --classify example.inr | grep 'route_id=1 route=project-owned library=0' >/dev/null
./rewrite/iplay.sh --classify example.bin | grep 'route_id=2 route=probe-by-content library=1' >/dev/null
test -x rewrite/.build/iplay_rewrite_dos_runner
if [ "$(dd if=rewrite/.build/IPLAYC.EXE bs=2 count=1 2>/dev/null)" != "MZ" ]; then
  echo "rewrite/.build/IPLAYC.EXE is not a DOS MZ executable" >&2
  exit 1
fi
if [ "$(dd if=rewrite/.build/IPLAYTRY.EXE bs=2 count=1 2>/dev/null)" != "MZ" ]; then
  echo "rewrite/.build/IPLAYTRY.EXE is not a DOS MZ executable" >&2
  exit 1
fi
if [ "$(dd if=rewrite/.build/IPLAYCONT.EXE bs=2 count=1 2>/dev/null)" != "MZ" ]; then
  echo "rewrite/.build/IPLAYCONT.EXE is not a DOS MZ executable" >&2
  exit 1
fi
if [ "$(dd if=rewrite/.build/IPLAYDIAG.EXE bs=2 count=1 2>/dev/null)" != "MZ" ]; then
  echo "rewrite/.build/IPLAYDIAG.EXE is not a DOS MZ executable" >&2
  exit 1
fi
if [ "$(dd if=rewrite/.build/IPLAYHW.EXE bs=2 count=1 2>/dev/null)" != "MZ" ]; then
  echo "rewrite/.build/IPLAYHW.EXE is not a DOS MZ executable" >&2
  exit 1
fi
if [ "$(dd if=rewrite/.build/IRUN.EXE bs=2 count=1 2>/dev/null)" != "MZ" ]; then
  echo "rewrite/.build/IRUN.EXE is not a DOS MZ executable" >&2
  exit 1
fi
if [ "$(dd if=rewrite/.build/IUIRUN.EXE bs=2 count=1 2>/dev/null)" != "MZ" ]; then
  echo "rewrite/.build/IUIRUN.EXE is not a DOS MZ executable" >&2
  exit 1
fi
if [ "$(dd if=rewrite/.build/IARUN.EXE bs=2 count=1 2>/dev/null)" != "MZ" ]; then
  echo "rewrite/.build/IARUN.EXE is not a DOS MZ executable" >&2
  exit 1
fi
if [ "$(dd if=rewrite/.build/IPHWRUN.EXE bs=2 count=1 2>/dev/null)" != "MZ" ]; then
  echo "rewrite/.build/IPHWRUN.EXE is not a DOS MZ executable" >&2
  exit 1
fi
python3 - <<'PY'
import pathlib
import struct
import sys

for name in ["IPLAYC.EXE", "IPLAYTRY.EXE", "IPLAYCONT.EXE", "IPLAYDIAG.EXE", "IPLAYHW.EXE", "IRUN.EXE", "IUIRUN.EXE", "IARUN.EXE", "IPHWRUN.EXE"]:
    path = pathlib.Path("rewrite/.build") / name
    data = path.read_bytes()
    if data[:2] != b"MZ":
        print(f"{path} is not an MZ executable", file=sys.stderr)
        sys.exit(1)
    if len(data) >= 0x40:
        pe_off = struct.unpack_from("<I", data, 0x3C)[0]
        if pe_off + 4 <= len(data) and data[pe_off:pe_off + 4] == b"PE\0\0":
            print(f"{path} is a PE executable, not a DOS executable", file=sys.stderr)
            sys.exit(1)
PY
if grep -Eq 'iplay_masm_|iplay_m_|iplay_abi_hex|abi_runner' rewrite/.build/IPLAYC.map rewrite/.build/IPLAYTRY.map rewrite/.build/IPLAYCONT.map rewrite/.build/IPLAYDIAG.map rewrite/.build/IPLAYHW.map; then
  echo "rewrite player maps reference generated/fallback translated sources" >&2
  exit 1
fi
if grep -Eq 'iplay_masm_|iplay_m_|iplay_abi_hex|abi_runner' rewrite/.build/IRUN.map rewrite/.build/IUIRUN.map rewrite/.build/IARUN.map rewrite/.build/IPHWRUN.map; then
  echo "rewrite helper runner maps reference generated/fallback translated sources" >&2
  exit 1
fi
if grep -Eiq 'libdosbox|dosbox' rewrite/build_rewrite.sh rewrite/.build/IPLAYC.map rewrite/.build/IPLAYTRY.map rewrite/.build/IPLAYCONT.map rewrite/.build/IPLAYDIAG.map rewrite/.build/IPLAYHW.map rewrite/.build/IRUN.map rewrite/.build/IPHWRUN.map; then
  echo "rewrite build/player artifacts must not depend on libdosbox/dosbox" >&2
  exit 1
fi
if grep -Eq 'IPLAY\.EXE|/home/xor/masm2c/examples/IPLAY\.EXE' rewrite/build_rewrite.sh rewrite/.build/IPLAYC.map rewrite/.build/IPLAYTRY.map rewrite/.build/IPLAYCONT.map rewrite/.build/IPLAYDIAG.map rewrite/.build/IPLAYHW.map rewrite/.build/IRUN.map rewrite/.build/IPHWRUN.map; then
  echo "rewrite build/player artifacts must not depend on the original IPLAY.EXE binary" >&2
  exit 1
fi
python3 - <<'PY'
import pathlib
import re
import sys

limit = 0xFF80
for exe_name in ["IPLAYC", "IPLAYTRY", "IPLAYCONT", "IPLAYDIAG", "IPLAYHW"]:
    map_path = pathlib.Path("rewrite/.build") / f"{exe_name}.map"
    map_text = map_path.read_text(errors="ignore")
    code_segments = []
    for match in re.finditer(r"^\s*(\S+)\s+CODE\s+AUTO\s+[0-9A-Fa-f:]+\s+([0-9A-Fa-f]+)\s*$", map_text, re.M):
        code_segments.append((match.group(1), int(match.group(2), 16)))
    if not code_segments:
        print(f"could not find {exe_name} AUTO code segments in {map_path}", file=sys.stderr)
        sys.exit(1)
    size = sum(segment_size for _, segment_size in code_segments)
    if size > limit:
        detail = ", ".join(f"{name}=0x{segment_size:04X}" for name, segment_size in code_segments)
        print(f"{exe_name} AUTO code is too close to the 64 KiB DOS limit: 0x{size:04X} > 0x{limit:04X} ({detail})", file=sys.stderr)
        sys.exit(1)
PY

for pure_c_source in rewrite/iplay_rewrite.c rewrite/iplay_player.c rewrite/rewrite_runner.c; do
  if grep -Eq '__WATCOMC__|_asm' "$pure_c_source"; then
    echo "$pure_c_source must remain pure C without Watcom/inline-asm ABI glue" >&2
    exit 1
  fi
done

test ! -e rewrite/.build/IABI.EXE
test ! -e rewrite/.build/IABI.map
if grep -Eq 'iplay_abi_hex\.c|abi_runner\.c|wcl .*IABI\.EXE|kvikdos IABI\.EXE' rewrite/build_rewrite.sh; then
  echo "generated monolithic ABI fallback is referenced by build_rewrite.sh" >&2
  exit 1
fi
python3 - <<'PY'
import pathlib
import sys

build = pathlib.Path("rewrite/build_rewrite.sh").read_text()
bad = []
for line in build.splitlines():
    if "/wcl " not in line or "IABI_" not in line or "-fe=rewrite/.build/IABI_" not in line:
        continue
    if "rewrite/iplay_rewrite.c" not in line:
        bad.append(line)
if bad:
    print("standalone ABI test binaries must link rewrite/iplay_rewrite.c", file=sys.stderr)
    for line in bad:
        print(line, file=sys.stderr)
    sys.exit(1)
PY
python3 - <<'PY'
import pathlib
import re
import sys

build = pathlib.Path("rewrite/build_rewrite.sh").read_text()
if not re.search(r'/home/xor/watcom/binl64/wcc\b[^\n]*-fo=rewrite/\.build/iplay_rewrite\.obj[^\n]*rewrite/iplay_rewrite\.c', build):
    print("rewrite/iplay_rewrite.c is not compiled by /home/xor/watcom/binl64/wcc into rewrite/.build/iplay_rewrite.obj", file=sys.stderr)
    sys.exit(1)
iplayc_compile_checks = [
    ("rewrite/iplay_rewrite.c", "rewrite/.build/iplay_rewrite_zm.obj"),
    ("rewrite/iplay_abi_watcom.c", "rewrite/.build/iplay_abi_watcom_zm.obj"),
    ("rewrite/iplay_player.c", "rewrite/.build/iplay_player_diag_zm.obj"),
    ("rewrite/iplay_player.c", "rewrite/.build/iplay_player_cont_zm.obj"),
    ("rewrite/iplay_player.c", "rewrite/.build/iplay_player_try_zm.obj"),
    ("rewrite/iplay_player.c", "rewrite/.build/iplay_player_contdiag_zm.obj"),
    ("rewrite/iplay_player.c", "rewrite/.build/iplay_player_hwdiag_zm.obj"),
]
for source, obj in iplayc_compile_checks:
    pattern = rf'/home/xor/watcom/binl64/wcc\b[^\n]*-bt=dos[^\n]*-3[^\n]*-ml[^\n]*-zm[^\n]*-fo={re.escape(obj)}[^\n]*{re.escape(source)}'
    if not re.search(pattern, build):
        print(f"IPLAYC.EXE source {source} is not compiled by /home/xor/watcom/binl64/wcc -bt=dos -3 -ml -zm into {obj}", file=sys.stderr)
        sys.exit(1)
prod_sb16_pattern = r'/home/xor/watcom/binl64/wcc\b[^\n]*-DIPLAY_PLAYER_ENABLE_DIAGNOSTICS=0[^\n]*-DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1[^\n]*-fo=rewrite/\.build/iplay_player_cont_zm\.obj[^\n]*rewrite/iplay_player\.c'
if not re.search(prod_sb16_pattern, build):
    print("IPLAYC.EXE player object must be compiled with real SB16 hardware I/O", file=sys.stderr)
    sys.exit(1)
diag_real_sb16_pattern = r'/home/xor/watcom/binl64/wcc\b[^\n]*-DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1[^\n]*-fo=rewrite/\.build/iplay_player_(diag_zm|contdiag_zm)\.obj'
if re.search(diag_real_sb16_pattern, build):
    print("diagnostic player objects must keep wrapper SB16 I/O for kvikdos tests", file=sys.stderr)
    sys.exit(1)
try_real_sb16_pattern = r'/home/xor/watcom/binl64/wcc\b[^\n]*-DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1[^\n]*-fo=rewrite/\.build/iplay_player_try_zm\.obj[^\n]*rewrite/iplay_player\.c'
if re.search(try_real_sb16_pattern, build):
    print("IPLAYTRY.EXE player object must keep wrapper SB16 I/O for kvikdos trial playback", file=sys.stderr)
    sys.exit(1)
hwdiag_sb16_pattern = r'/home/xor/watcom/binl64/wcc\b[^\n]*-DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1[^\n]*-fo=rewrite/\.build/iplay_player_hwdiag_zm\.obj[^\n]*rewrite/iplay_player\.c'
if not re.search(hwdiag_sb16_pattern, build):
    print("IPLAYHW.EXE player object must keep diagnostics enabled while using real SB16 hardware I/O", file=sys.stderr)
    sys.exit(1)
stable_abi_pattern = r'/home/xor/watcom/binl64/wcc\b[^\n]*-DIPLAY_PLAYER_OMIT_RISKY_UI_ABI[^\n]*-fo=rewrite/\.build/iplay_abi_watcom_zm\.obj[^\n]*rewrite/iplay_abi_watcom\.c'
if not re.search(stable_abi_pattern, build):
    print("IPLAYC.EXE stable ABI object must be compiled with -DIPLAY_PLAYER_OMIT_RISKY_UI_ABI", file=sys.stderr)
    sys.exit(1)
full_abi_pattern = r'/home/xor/watcom/binl64/wcc\b(?![^\n]*-DIPLAY_PLAYER_OMIT_RISKY_UI_ABI)[^\n]*-fo=rewrite/\.build/iplay_abi_watcom_full\.obj[^\n]*rewrite/iplay_abi_watcom\.c'
if not re.search(full_abi_pattern, build):
    print("full production ABI source must be compiled without -DIPLAY_PLAYER_OMIT_RISKY_UI_ABI into rewrite/.build/iplay_abi_watcom_full.obj", file=sys.stderr)
    sys.exit(1)
if "/home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYC.lnk" not in build:
    print("IPLAYC.EXE is not linked by /home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYC.lnk", file=sys.stderr)
    sys.exit(1)
if "/home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYTRY.lnk" not in build:
    print("IPLAYTRY.EXE is not linked by /home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYTRY.lnk", file=sys.stderr)
    sys.exit(1)
if "/home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYHW.lnk" not in build:
    print("IPLAYHW.EXE is not linked by /home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYHW.lnk", file=sys.stderr)
    sys.exit(1)
iplayc_link = pathlib.Path("rewrite/.build/IPLAYC.lnk").read_text()
iplaytry_link = pathlib.Path("rewrite/.build/IPLAYTRY.lnk").read_text()
iplayhw_link = pathlib.Path("rewrite/.build/IPLAYHW.lnk").read_text()
required_link_lines = [
    "system dos",
    "option map=rewrite/.build/IPLAYC.map",
    "option eliminate",
    "name rewrite/.build/IPLAYC.EXE",
    "file rewrite/.build/iplay_rewrite_zm.obj",
    "file rewrite/.build/iplay_abi_watcom_zm.obj",
    "file rewrite/.build/iplay_player_cont_zm.obj",
]
for required in required_link_lines:
    if required not in iplayc_link:
        print(f"rewrite/.build/IPLAYC.lnk is missing required directive: {required}", file=sys.stderr)
        sys.exit(1)
required_try_link_lines = [
    "system dos",
    "option map=rewrite/.build/IPLAYTRY.map",
    "option eliminate",
    "name rewrite/.build/IPLAYTRY.EXE",
    "file rewrite/.build/iplay_rewrite_zm.obj",
    "file rewrite/.build/iplay_abi_watcom_zm.obj",
    "file rewrite/.build/iplay_player_try_zm.obj",
]
for required in required_try_link_lines:
    if required not in iplaytry_link:
        print(f"rewrite/.build/IPLAYTRY.lnk is missing required directive: {required}", file=sys.stderr)
        sys.exit(1)
required_hw_link_lines = [
    "system dos",
    "option map=rewrite/.build/IPLAYHW.map",
    "option eliminate",
    "name rewrite/.build/IPLAYHW.EXE",
    "file rewrite/.build/iplay_rewrite_zm.obj",
    "file rewrite/.build/iplay_abi_watcom_zm.obj",
    "file rewrite/.build/iplay_player_hwdiag_zm.obj",
]
for required in required_hw_link_lines:
    if required not in iplayhw_link:
        print(f"rewrite/.build/IPLAYHW.lnk is missing required directive: {required}", file=sys.stderr)
        sys.exit(1)
irun_line = next((line for line in build.splitlines() if "-fe=rewrite/.build/IRUN.EXE" in line), "")
iui_line = next((line for line in build.splitlines() if "-fe=rewrite/.build/IUIRUN.EXE" in line), "")
iarun_line = next((line for line in build.splitlines() if "-fe=rewrite/.build/IARUN.EXE" in line), "")
iphwrun_line = next((line for line in build.splitlines() if "-fe=rewrite/.build/IPHWRUN.EXE" in line), "")
for label, line in [("IRUN.EXE", irun_line), ("IUIRUN.EXE", iui_line), ("IARUN.EXE", iarun_line), ("IPHWRUN.EXE", iphwrun_line)]:
    for option in ["-bt=dos", "-3"]:
        if option not in line:
            print(f"{label} link command is missing OpenWatcom DOS option {option}", file=sys.stderr)
            sys.exit(1)
    if "-ms" not in line and "-ml" not in line:
        print(f"{label} link command is missing an OpenWatcom DOS memory model option", file=sys.stderr)
        sys.exit(1)
for forbidden in ["iplay_masm_.cpp", "iplay_m_.cpp", "iplay_abi_hex.c", "abi_runner.c", "rewrite/abi_"]:
    if forbidden in iplayc_link:
        print(f"IPLAYC.EXE link command uses forbidden generated/fallback source {forbidden}", file=sys.stderr)
        sys.exit(1)
    if forbidden in iplaytry_link:
        print(f"IPLAYTRY.EXE link command uses forbidden generated/fallback source {forbidden}", file=sys.stderr)
        sys.exit(1)
    if forbidden in iplayhw_link:
        print(f"IPLAYHW.EXE link command uses forbidden generated/fallback source {forbidden}", file=sys.stderr)
        sys.exit(1)
if ".cpp" in iplayc_link:
    print("IPLAYC.EXE link command must not use C++ sources", file=sys.stderr)
    sys.exit(1)
if ".cpp" in iplaytry_link:
    print("IPLAYTRY.EXE link command must not use C++ sources", file=sys.stderr)
    sys.exit(1)
if ".cpp" in iplayhw_link:
    print("IPLAYHW.EXE link command must not use C++ sources", file=sys.stderr)
    sys.exit(1)
if re.search(r'\.(asm|o)\b', iplayc_link, re.I):
    print("IPLAYC.EXE link command must not use assembly or non-Watcom object sources", file=sys.stderr)
    sys.exit(1)
if re.search(r'\.(asm|o)\b', iplaytry_link, re.I):
    print("IPLAYTRY.EXE link command must not use assembly or non-Watcom object sources", file=sys.stderr)
    sys.exit(1)
if re.search(r'\.(asm|o)\b', iplayhw_link, re.I):
    print("IPLAYHW.EXE link command must not use assembly or non-Watcom object sources", file=sys.stderr)
    sys.exit(1)
if iplayc_link.count("rewrite/.build/iplay_abi_watcom_zm.obj") != 1:
    print("IPLAYC.EXE link command must include exactly one ABI glue object: rewrite/.build/iplay_abi_watcom_zm.obj", file=sys.stderr)
    sys.exit(1)
if iplaytry_link.count("rewrite/.build/iplay_abi_watcom_zm.obj") != 1:
    print("IPLAYTRY.EXE link command must include exactly one ABI glue object: rewrite/.build/iplay_abi_watcom_zm.obj", file=sys.stderr)
    sys.exit(1)
if iplayhw_link.count("rewrite/.build/iplay_abi_watcom_zm.obj") != 1:
    print("IPLAYHW.EXE link command must include exactly one ABI glue object: rewrite/.build/iplay_abi_watcom_zm.obj", file=sys.stderr)
    sys.exit(1)
required_runner_sources = [
    "rewrite/iplay_rewrite.c",
    "rewrite/rewrite_runner.c",
]
irun_sources = [token for token in irun_line.split() if token.endswith(".c")]
if irun_sources != required_runner_sources:
    print(f"IRUN.EXE link command must use exactly {required_runner_sources}, got {irun_sources}", file=sys.stderr)
    sys.exit(1)
for source in required_runner_sources:
    if source not in irun_line:
        print(f"IRUN.EXE link command is missing {source}", file=sys.stderr)
        sys.exit(1)
required_text_runner_sources = [
    "rewrite/iplay_rewrite.c",
    "rewrite/text_wrapper_runner.c",
]
iui_sources = [token for token in iui_line.split() if token.endswith(".c")]
if iui_sources != required_text_runner_sources:
    print(f"IUIRUN.EXE link command must use exactly {required_text_runner_sources}, got {iui_sources}", file=sys.stderr)
    sys.exit(1)
for source in required_text_runner_sources:
    if source not in iui_line:
        print(f"IUIRUN.EXE link command is missing {source}", file=sys.stderr)
        sys.exit(1)
required_audio_runner_sources = [
    "rewrite/iplay_rewrite.c",
    "rewrite/audio_wrapper_runner.c",
]
iarun_sources = [token for token in iarun_line.split() if token.endswith(".c")]
if iarun_sources != required_audio_runner_sources:
    print(f"IARUN.EXE link command must use exactly {required_audio_runner_sources}, got {iarun_sources}", file=sys.stderr)
    sys.exit(1)
for source in required_audio_runner_sources:
    if source not in iarun_line:
        print(f"IARUN.EXE link command is missing {source}", file=sys.stderr)
        sys.exit(1)
required_player_hw_runner_sources = [
    "rewrite/player_hw_runner.c",
    "rewrite/iplay_rewrite.c",
    "rewrite/iplay_abi_watcom.c",
]
iphwrun_sources = [token for token in iphwrun_line.split() if token.endswith(".c")]
if iphwrun_sources != required_player_hw_runner_sources:
    print(f"IPHWRUN.EXE link command must use exactly {required_player_hw_runner_sources}, got {iphwrun_sources}", file=sys.stderr)
    sys.exit(1)
for source in required_player_hw_runner_sources:
    if source not in iphwrun_line:
        print(f"IPHWRUN.EXE link command is missing {source}", file=sys.stderr)
        sys.exit(1)
for required_flag in ["-DIPLAY_PLAYER_OMIT_RISKY_UI_ABI", "-DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28", "-DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS"]:
    if required_flag not in iphwrun_line:
        print(f"IPHWRUN.EXE link command is missing required stable ABI flag {required_flag}", file=sys.stderr)
        sys.exit(1)
for forbidden in ["iplay_masm_.cpp", "iplay_m_.cpp", "iplay_abi_hex.c", "abi_runner.c"]:
    if forbidden in irun_line or forbidden in iui_line or forbidden in iarun_line or forbidden in iphwrun_line:
        print(f"helper runner link command uses forbidden generated/fallback source {forbidden}", file=sys.stderr)
        sys.exit(1)
if ".cpp" in irun_line or ".cpp" in iui_line or ".cpp" in iarun_line or ".cpp" in iphwrun_line:
    print("helper runner link commands must not use C++ sources", file=sys.stderr)
    sys.exit(1)
if re.search(r'\.(asm|obj|o)\b', irun_line, re.I) or re.search(r'\.(asm|obj|o)\b', iui_line, re.I) or re.search(r'\.(asm|obj|o)\b', iarun_line, re.I) or re.search(r'\.(asm|obj|o)\b', iphwrun_line, re.I):
    print("helper runner link commands must not use assembly or prebuilt object sources", file=sys.stderr)
    sys.exit(1)
if "rewrite/iplay_abi_watcom.c" in irun_line or "rewrite/iplay_abi_watcom.c" in iui_line or "rewrite/iplay_abi_watcom.c" in iarun_line:
    print("helper parity runners must not link production ABI glue", file=sys.stderr)
    sys.exit(1)
tests = pathlib.Path("tests/test_function_parity.py").read_text()
if 'ORIGINAL_EXE = Path("original/IPLAY.EXE")' not in tests:
    print("function parity must keep original/IPLAY.EXE as the original reference", file=sys.stderr)
    sys.exit(1)
inventory = pathlib.Path("tests/test_unit_coverage_inventory.py").read_text()
abi = pathlib.Path("rewrite/iplay_abi_watcom.c").read_text()
header = pathlib.Path("rewrite/iplay_rewrite.h").read_text()
rewrite_source = pathlib.Path("rewrite/iplay_rewrite.c").read_text()
allowed_inline_abi_shims = {
    "midi_public_regs",
    "CheckSB",
}
for match in re.finditer(r'(?:static\s+)?void\s+([A-Za-z_][A-Za-z0-9_]*)\s*\([^)]*\)\s*\{', abi):
    name = match.group(1)
    depth = 1
    pos = match.end()
    while pos < len(abi) and depth:
        if abi[pos] == "{":
            depth += 1
        elif abi[pos] == "}":
            depth -= 1
        pos += 1
    body = abi[match.end():pos - 1]
    if "_asm" in body and "iplay_" not in body and name not in allowed_inline_abi_shims:
        print(f"inline Watcom ABI behavior must delegate to pure C helper or be explicitly allowlisted: {name}", file=sys.stderr)
        sys.exit(1)
watcom_guard_depth = 0
for lineno, line in enumerate(header.splitlines(), 1):
    stripped = line.strip()
    if stripped == "#ifdef __WATCOMC__":
        watcom_guard_depth += 1
    elif stripped == "#endif" and watcom_guard_depth:
        watcom_guard_depth -= 1
    if "#pragma aux" in line and watcom_guard_depth == 0:
        print(f"#pragma aux outside __WATCOMC__ guard at rewrite/iplay_rewrite.h:{lineno}", file=sys.stderr)
        sys.exit(1)
allowed_reg_write_helpers = {
    "apply_full_regs6",
    "apply_eax_reg",
    "apply_ebp_reg",
    "apply_esi_reg",
    "apply_edi_reg",
    "apply_eax_edi_regs",
    "apply_eax_esi_regs",
    "apply_ecx_esi_regs",
    "apply_eax_edx_regs",
    "apply_eax_ecx_edx_regs",
    "apply_mix_setup_regs",
    "apply_sndsettings_regs",
}
current_function = None
for lineno, line in enumerate(rewrite_source.splitlines(), 1):
    match = re.match(r'^(?:static\s+)?(?:void|int|db|dw|dd)\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', line)
    if match:
        current_function = match.group(1)
    if re.search(r'\br->(?:e?[abcd]x|e[sd]i|e[bs]p|flags)\s*(?:(?:[&|+\-*/]?=)(?!=)|--)', line):
        if current_function not in allowed_reg_write_helpers:
            print(f"direct IplayRegs write outside apply helper at rewrite/iplay_rewrite.c:{lineno}: {line.strip()}", file=sys.stderr)
            sys.exit(1)
public_abi_symbols = sorted(set(re.findall(r'^void\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', abi, re.M)))
pragma_symbols = set(re.findall(r'#pragma\s+aux\s+([A-Za-z_][A-Za-z0-9_]*)\b', header))
missing_pragma = [symbol for symbol in public_abi_symbols if symbol not in pragma_symbols]
if missing_pragma:
    for symbol in missing_pragma:
        print(f"public ABI symbol is missing #pragma aux declaration: {symbol}", file=sys.stderr)
    sys.exit(1)
header_public_abi_symbols = sorted(set(re.findall(r'^void\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(void\);\n#pragma aux', header, re.M)))
covered_original_entries = set(re.findall(r'^\s+"([^"]+)",\s*$', inventory, re.M))
missing_inventory = [symbol for symbol in header_public_abi_symbols if symbol not in covered_original_entries]
if missing_inventory:
    for symbol in missing_inventory:
        print(f"public ABI symbol is missing from TESTED_ORIGINAL_ENTRIES coverage inventory: {symbol}", file=sys.stderr)
    sys.exit(1)
commands = sorted(set(re.findall(r'translated\("(abi[^"]+)"', tests)))
missing = [command for command in commands if command not in build]
if missing:
    for command in missing:
        print(f"missing explicit ABI route: {command}", file=sys.stderr)
    sys.exit(1)
gate = pathlib.Path("rewrite/check_rewrite.sh").read_text()
if 'pytest -q tests/test_player_behavior.py' not in gate:
    print("player behavior must run tests/test_player_behavior.py", file=sys.stderr)
    sys.exit(1)
if 'pytest -q tests/test_modplug_renderer.py' not in gate:
    print("libmodplug renderer proof must run tests/test_modplug_renderer.py", file=sys.stderr)
    sys.exit(1)
if 'pytest -q tests/test_function_parity.py' not in gate:
    print("function parity must run tests/test_function_parity.py", file=sys.stderr)
    sys.exit(1)
modplug_tests = pathlib.Path("tests/test_modplug_renderer.py").read_text()
native_probe = pathlib.Path("rewrite/modplug_audio_probe.cpp").read_text()
if 'usage: %s [--modern] [--video-mode=MODE] <module-file|@file-list> [--modern|max-blocks|--blocks=N|--source-end|--keyboard-after-one]' not in native_probe:
    print("native probe help must advertise --blocks=N bounded playback", file=sys.stderr)
    sys.exit(1)
if 'static int native_parse_max_blocks_arg(const char *arg)' not in native_probe or 'std::strncmp(arg, "--blocks=", 9) == 0' not in native_probe:
    print("native probe must parse --blocks=N directly, not only wrapper-side", file=sys.stderr)
    sys.exit(1)
if '#include <SDL.h>' not in native_probe or 'SDL_OpenAudioDevice(0, 0, &want, &have, SDL_AUDIO_ALLOW_SAMPLES_CHANGE)' not in native_probe or 'SDL_QueueAudio(sink->device, pcm, byte_count)' not in native_probe:
    print("native probe must provide an opt-in real SDL2 queued-audio sink", file=sys.stderr)
    sys.exit(1)
if 'static void native_terminal_render_playback(const IplayTextMode *mode, const char *path, const IplayModernPlaybackResult *result)' not in native_probe or 'native_terminal_render_cells(cells, mode);' not in native_probe:
    print("native probe must provide an opt-in terminal renderer fed by notcurses-style cells", file=sys.stderr)
    sys.exit(1)
if 'static bool native_playback_progress(void *user, const IplayModplugAudioBridgeStats *stats)' not in native_probe or 'Terminal live: block=%lu frames=%lu accepted=%lu levels=%u/%u L[' not in native_probe:
    print("native probe must provide an opt-in live terminal audio-level meter", file=sys.stderr)
    sys.exit(1)
if 'static int native_stdin_keyboard_pressed(void)' not in native_probe or 'select(STDIN_FILENO + 1, &fds, 0, 0, &tv)' not in native_probe or 'Stdin keyboard: requested=1 stopped=%d' not in native_probe:
    print("native probe must provide an opt-in stdin keyboard stop path", file=sys.stderr)
    sys.exit(1)
if 'struct NativeKeyboardMode' not in native_probe or 'static void native_keyboard_mode_enable(NativeKeyboardMode *mode, int requested)' not in native_probe or 'raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);' not in native_probe or 'native_keyboard_mode_restore(&keyboard_mode);' not in native_probe:
    print("native probe must put interactive stdin keyboard mode into raw TTY mode and restore it", file=sys.stderr)
    sys.exit(1)
if "test_native_binary_blocks_option_reports_block_limit" not in modplug_tests:
    print("native --blocks=N direct playback path must have a block-limit test", file=sys.stderr)
    sys.exit(1)
if "test_native_binary_sdl_audio_option_queues_exact_sb16_sink" not in modplug_tests:
    print("native --sdl-audio direct playback path must have an SDL sink queueing test", file=sys.stderr)
    sys.exit(1)
if "test_native_binary_terminal_render_option_paints_selected_text_mode" not in modplug_tests:
    print("native --terminal-render direct playback path must have a terminal render test", file=sys.stderr)
    sys.exit(1)
if "test_native_binary_terminal_live_option_updates_audio_levels_per_block" not in modplug_tests:
    print("native --terminal-live direct playback path must have a live level test", file=sys.stderr)
    sys.exit(1)
if "test_native_binary_stdin_keyboard_option_stops_on_q" not in modplug_tests:
    print("native --stdin-keyboard direct playback path must have a stdin keyboard-stop test", file=sys.stderr)
    sys.exit(1)
native_build = pathlib.Path("rewrite/build_native_player.sh").read_text()
if "pkg-config --cflags libmodplug sdl2" not in native_build or "pkg-config --libs libmodplug sdl2" not in native_build:
    print("native player build must link SDL2 for the playable native audio sink", file=sys.stderr)
    sys.exit(1)
if 'timeout = int(os.environ.get("IPLAY_HOST_TEST_TIMEOUT", "30"))' not in modplug_tests:
    print("modern host probe tests must have a bounded subprocess timeout", file=sys.stderr)
    sys.exit(1)
if "PROBE_COMPILED = False" not in modplug_tests or "AUDIO_PROBE_COMPILED = False" not in modplug_tests:
    print("modern host probe tests must compile probes once per pytest process", file=sys.stderr)
    sys.exit(1)
if 'subprocess.run(cmd,' not in modplug_tests or "timeout=timeout" not in modplug_tests:
    print("modern host probe run helper must pass timeout to subprocess.run", file=sys.stderr)
    sys.exit(1)
smoke_tests = pathlib.Path("tests/test_player_smoke.py").read_text()
if 'IPLAY_SMOKE_TEST_TIMEOUT' not in smoke_tests or "timeout=90" in smoke_tests:
    print("DOS smoke pytest wrapper must use a short configurable timeout", file=sys.stderr)
    sys.exit(1)
player_behavior = pathlib.Path("tests/test_player_behavior.py").read_text()
if "xfail" in player_behavior:
    print("player behavior tests must be passing assertions, not expected failures", file=sys.stderr)
    sys.exit(1)
if "timeout: Optional[int] = None" not in player_behavior or 'timeout = 3 if exe == ORIGINAL_EXE else int(os.environ.get("IPLAY_KVIKDOS_TEST_TIMEOUT", "3"))' not in player_behavior:
    print("player behavior run_dos must keep unsupported original-binary probes short by default", file=sys.stderr)
    sys.exit(1)
if 'IPLAY_ORIGINAL_KVIKDOS_MAX_TIMEOUT' not in player_behavior or 'exe.name.upper() == "IPLAY.EXE" and exe.parent != BUILD_DIR' not in player_behavior:
    print("player behavior run_dos must cap unsupported original IPLAY.EXE probes even when a test requests a longer timeout", file=sys.stderr)
    sys.exit(1)
if '["timeout", "-k", "1", str(timeout), str(KVIKDOS), str(exe), *dos_args]' not in player_behavior:
    print("player behavior run_dos must hard-kill stuck kvikdos children after a short grace period", file=sys.stderr)
    sys.exit(1)
if "proc.communicate(timeout=timeout + 2)" not in player_behavior or "python timeout after {timeout + 2}s" not in player_behavior:
    print("player behavior run_dos must keep Python process-group timeout close to the external kvikdos timeout", file=sys.stderr)
    sys.exit(1)
if '["timeout", "-k", "1", "3", str(KVIKDOS)' not in player_behavior:
    print("player behavior direct kvikdos probes must use short hard-kill timeout wrappers", file=sys.stderr)
    sys.exit(1)
smoke = pathlib.Path("rewrite/smoke_player.sh").read_text()
if "from player_behavior_fixtures import write_endcont_module, write_smoke_modules" not in smoke:
    print("rewrite/smoke_player.sh must use shared tests/player_behavior_fixtures.py module fixtures", file=sys.stderr)
    sys.exit(1)
if "./rewrite/build_player.sh" not in smoke or "./rewrite/build_rewrite.sh" in smoke:
    print("rewrite/smoke_player.sh must use the player-only rebuild, not the full ABI/test-runner rebuild", file=sys.stderr)
    sys.exit(1)
if "run_iplaytry ENDCONT.S3M" not in smoke or "IPLAYTRY.EXE" not in smoke:
    print("rewrite/smoke_player.sh must smoke the quiet IPLAYTRY.EXE trial binary under kvikdos with a source-end fixture", file=sys.stderr)
    sys.exit(1)
if "run_iplaycont ENDCONT.S3M" not in smoke or "IPLAYCONT.EXE" not in smoke:
    print("rewrite/smoke_player.sh must smoke the visible continuous IPLAYCONT.EXE diagnostic binary under kvikdos with a source-end fixture", file=sys.stderr)
    sys.exit(1)
if "Playback loop: mode=playback policy=timer-keyboard cadence=timer max_blocks=0 frames/block=1024" not in smoke:
    print("rewrite/smoke_player.sh must require continuous timer-keyboard playback loop evidence", file=sys.stderr)
    sys.exit(1)
if "limit=0 source_end=1 stop=source-end" not in smoke:
    print("rewrite/smoke_player.sh must require source-end continuous playback pump evidence", file=sys.stderr)
    sys.exit(1)
for mode_probe in [
    "run_video_mode_smoke SMOKE.S3M 40x25bw 40 25 2000",
    "run_video_mode_smoke SMOKE.S3M 40x25color 40 25 2000",
    "run_video_mode_smoke SMOKE.S3M 80x25bw 80 25 4000",
    "run_video_mode_smoke SMOKE.S3M 80x25color 80 25 4000",
    "run_video_mode_smoke SMOKE.S3M 80x50 80 50 8000",
]:
    if mode_probe not in smoke:
        print(f"rewrite/smoke_player.sh must smoke selectable text mode: {mode_probe}", file=sys.stderr)
        sys.exit(1)
if "IPLAYDIAG video-mode smoke failed:" not in smoke or '"--video-mode=$mode"' not in smoke:
    print("rewrite/smoke_player.sh must run bounded diagnostic smoke probes with explicit --video-mode", file=sys.stderr)
    sys.exit(1)
if "unexpectedly used real SB16 unavailable path" not in smoke:
    print("rewrite/smoke_player.sh must reject IPLAYTRY.EXE falling back to the real-SB16 unavailable path", file=sys.stderr)
    sys.exit(1)
if "IPLAYTRY/IPLAYCONT/IPLAYHW/IPLAYDIAG/TEXTMODE smoke ok" not in smoke or "IPLAYTRY/IPLAYCONT/IPLAYHW/IPLAYDIAG/TEXTMODE smoke ok" not in smoke_tests:
    print("DOS smoke success marker must include IPLAYTRY, IPLAYCONT, IPLAYHW, IPLAYDIAG, and text-mode probes", file=sys.stderr)
    sys.exit(1)
if 'IPLAY_SMOKE_KVIKDOS_SECONDS:-3' not in smoke or "timeout 160" in smoke:
    print("rewrite/smoke_player.sh must keep kvikdos smoke timeout short and configurable", file=sys.stderr)
    sys.exit(1)
if "timeout -k 1" not in smoke:
    print("rewrite/smoke_player.sh must hard-kill stuck kvikdos children after a short grace period", file=sys.stderr)
    sys.exit(1)
if "bytearray(" in smoke or "def write(name, data)" in smoke:
    print("rewrite/smoke_player.sh must not duplicate tracker fixture byte construction", file=sys.stderr)
    sys.exit(1)
trial = pathlib.Path("rewrite/try_player.sh").read_text()
player_build = pathlib.Path("rewrite/build_player.sh").read_text()
player_source = pathlib.Path("rewrite/iplay_player.c").read_text()
native_probe = pathlib.Path("rewrite/modplug_audio_probe.cpp").read_text()
for marker in [
    "--video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50",
    "static int player_streq_ci(const char *left, const char *right)",
    "static int player_arg_is_video_mode_override(const char *arg)",
    "static db player_parse_cli_video_mode(int argc, char **argv)",
    "static int player_parse_cli_video_mode_valid(int argc, char **argv)",
    "static int player_video_mode_value_supported(const char *value)",
    "static int player_report_invalid_video_mode(void)",
    "Unsupported video mode. Use --video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50",
    "if (!player_module_request_video_mode_valid(&request)) return player_report_invalid_video_mode();",
    "player_set_text_video_mode_id(player_module_request_video_mode(request));",
    "IPLAY_VIDEO_MODE_40X25_BW",
    "IPLAY_VIDEO_MODE_40X25_COLOR",
    "IPLAY_VIDEO_MODE_80X25_BW",
    "IPLAY_VIDEO_MODE_80X25_COLOR",
    "IPLAY_VIDEO_MODE_80X50_PROJECT",
    'player_streq_ci(value, "80x50")',
]:
    if marker not in player_source:
        print(f"rewrite/iplay_player.c must support DOS text video-mode option marker: {marker}", file=sys.stderr)
        sys.exit(1)
if "needs_rebuild()" not in player_build or "compile_obj()" not in player_build or "link_exe()" not in player_build:
    print("rewrite/build_player.sh must keep player-only rebuilds incremental", file=sys.stderr)
    sys.exit(1)
if 'cmp -s "$tmp" "$lnk"' not in player_build:
    print("rewrite/build_player.sh must avoid touching unchanged link files or every EXE relinks", file=sys.stderr)
    sys.exit(1)
if "./rewrite/build_player.sh" not in trial or "./rewrite/build_rewrite.sh" in trial:
    print("rewrite/try_player.sh must use the player-only rebuild, not the full ABI/test-runner rebuild", file=sys.stderr)
    sys.exit(1)
if "IPLAY_TRIAL_LOG=${IPLAY_TRIAL_LOG:-RES.TXT}" not in trial:
    print("rewrite/try_player.sh must write a bounded trial result log for headless kvikdos runs", file=sys.stderr)
    sys.exit(1)
if "KVIKDOS_SECONDS=${KVIKDOS_SECONDS:-3}" not in trial:
    print("rewrite/try_player.sh must default to a short bounded kvikdos trial timeout", file=sys.stderr)
    sys.exit(1)
if "usage: ./rewrite/try_player.sh [--rebuild] [--modern|--native|--native-interactive|--native-source-end|--native-keyboard-after-one|--native-stdin-keyboard|--native-audio|--native-terminal|--native-live|--quiet|--diagnostics|--continuous-diagnostics|--hardware-diagnostics|--production] [--blocks=N] [--video-mode=MODE] <module-file|@file-list>" not in trial:
    print("rewrite/try_player.sh usage must avoid advertising unsupported arbitrary player args", file=sys.stderr)
    sys.exit(1)
if "--native-interactive enables native source-end playback, SDL2 audio, terminal render, live meters, and raw stdin keyboard stop" not in trial:
    print("rewrite/try_player.sh must expose one-shot native interactive mode", file=sys.stderr)
    sys.exit(1)
if "--native-stdin-keyboard stops native playback when q, Q, or Escape is read from stdin" not in trial:
    print("rewrite/try_player.sh must expose the opt-in native stdin keyboard stop path", file=sys.stderr)
    sys.exit(1)
if "--native-terminal renders the final notcurses-style text cells to the host terminal with ANSI 16-color output" not in trial:
    print("rewrite/try_player.sh must expose the opt-in native terminal renderer", file=sys.stderr)
    sys.exit(1)
if "--native-live updates ANSI audio level meters from the native playback callback while blocks are submitted" not in trial:
    print("rewrite/try_player.sh must expose the opt-in native live level renderer", file=sys.stderr)
    sys.exit(1)
for marker in [
    "try_player: file list not found:",
    "try_player: file list has no module entries:",
    "resolve_case_insensitive_file()",
    "try_player: resolved DOS-style case-insensitive file-list path:",
    "trial_filelist_arg=%s",
    "trial_filelist_path=%s",
    "trial_filelist_selected=%s",
    "trial_filelist_selected_host=%s",
    "native_module_arg=@$trial_filelist_abs",
    'set -- "$native_module_arg" "$native_play_arg" "$trial_video_mode"',
    '"rewrite/.build/$IPLAY_NATIVE_EXE" "$@"',
]:
    if marker not in trial:
        print(f"rewrite/try_player.sh missing @file-list wrapper evidence: {marker}", file=sys.stderr)
        sys.exit(1)
for marker in [
    "native_resolve_case_insensitive_path",
    "native_streq_ci",
    "resolved_list_path",
    'std::fprintf(stderr, "Module not found.\\n");',
    'std::fprintf(stderr, "%s: unsupported text mode: %s\\n", native_program_name(argv[0]), video_mode_arg);',
    "print_color_probe_evidence()",
    "Color probe: target_fg=16 target_bg=8 cells=24 fg_matches=%u bg_matches=%u blink_matches=%u fg_mask=%04x bg_mask=%02x blink_mask=%02x",
    "iplay_ncplane_putc_yx(plane, 0u, (dw)i, (db)('A' + i), iplay_text_attr((IplayTextColor)i, (IplayTextColor)0, 0));",
    "module filenames are resolved with DOS-style case-insensitive matching in their host directory",
    'native_streq_ci(arg, "80x50project")',
]:
    if marker not in native_probe:
        print(f"rewrite/modplug_audio_probe.cpp missing DOS-style case-insensitive native path marker: {marker}", file=sys.stderr)
        sys.exit(1)
if 'case "$1" in' not in trial or trial.count('-h|--help)') < 3:
    print("rewrite/try_player.sh must support help before or after trial options without rebuilding", file=sys.stderr)
    sys.exit(1)
if "--diagnostics runs IPLAYDIAG.EXE for visible bounded diagnostic stdout" not in trial:
    print("rewrite/try_player.sh must expose command-line diagnostic mode", file=sys.stderr)
    sys.exit(1)
if "--continuous-diagnostics runs IPLAYCONT.EXE for visible continuous-loop diagnostic stdout" not in trial:
    print("rewrite/try_player.sh must keep continuous diagnostics explicit instead of making them the default kvikdos path", file=sys.stderr)
    sys.exit(1)
if "--quiet runs IPLAYTRY.EXE continuous quiet playback; in headless kvikdos this can end by timeout" not in trial:
    print("rewrite/try_player.sh must keep quiet continuous trial available without making it the slow default", file=sys.stderr)
    sys.exit(1)
if "--hardware-diagnostics runs IPLAYHW.EXE for real-SB16 probe/unavailable diagnostics" not in trial or "trial_hardware_diagnostics=${IPLAY_TRIAL_HARDWARE_DIAGNOSTICS:-0}" not in trial:
    print("rewrite/try_player.sh must expose command-line real-SB16 diagnostic mode", file=sys.stderr)
    sys.exit(1)
if "--rebuild forces a player-only rebuild before launching kvikdos" not in trial or "trial_rebuild=${IPLAY_TRIAL_REBUILD:-auto}" not in trial:
    print("rewrite/try_player.sh must make rebuilds explicit and skip redundant player rebuilds by default", file=sys.stderr)
    sys.exit(1)
if 'player_rebuild_deps="rewrite/iplay_player.c rewrite/iplay_rewrite.c rewrite/iplay_rewrite.h rewrite/iplay_abi_watcom.c rewrite/build_player.sh"' not in trial:
    print("rewrite/try_player.sh must keep selected-player EXE freshness deps explicit", file=sys.stderr)
    sys.exit(1)
if "try_player: selected trial executable is stale after player build:" not in trial:
    print("rewrite/try_player.sh must refuse to run stale selected trial binaries after rebuild", file=sys.stderr)
    sys.exit(1)
if "trial_binary_fresh=%s" not in trial or "trial_exe_path=%s" not in trial:
    print("rewrite/try_player.sh must write selected trial binary freshness evidence to RES.TXT", file=sys.stderr)
    sys.exit(1)
if "--blocks=N is consumed by IPLAYDIAG.EXE when bounded diagnostics are enabled" not in trial:
    print("rewrite/try_player.sh must explain that --blocks=N is a diagnostic-mode option", file=sys.stderr)
    sys.exit(1)
if "--video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50|terminal|auto selects the text mode for the trial" not in trial:
    print("rewrite/try_player.sh must expose trial video-mode selection", file=sys.stderr)
    sys.exit(1)
if "try_player: unsupported video mode:" not in trial:
    print("rewrite/try_player.sh must reject unsupported explicit trial video modes before launching kvikdos", file=sys.stderr)
    sys.exit(1)
if 'try_player: module file not found:' not in trial:
    print("rewrite/try_player.sh must fail clearly before rebuilding when the host module path is missing", file=sys.stderr)
    sys.exit(1)
if 'try_player: missing module file after trial options' not in trial:
    print("rewrite/try_player.sh must reject option-only invocations before rebuilding", file=sys.stderr)
    sys.exit(1)
if 'if [ "$src_dir/$name" != "$dst_dir/$name" ]; then' not in trial or 'cp "$src_dir/$name" "$dst_dir/$name"' not in trial:
    print("rewrite/try_player.sh must avoid copying a build-dir module onto itself", file=sys.stderr)
    sys.exit(1)
if trial.count('--diagnostics)') < 2 or "trial_diagnostics=1" not in trial:
    print("rewrite/try_player.sh must accept --diagnostics before or after the host module path", file=sys.stderr)
    sys.exit(1)
if trial.count('--continuous-diagnostics)') < 2 or "IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYCONT.EXE}" not in trial:
    print("rewrite/try_player.sh must accept explicit continuous diagnostics before or after the host module path", file=sys.stderr)
    sys.exit(1)
if trial.count('--hardware-diagnostics)') < 2 or "trial_hardware_diagnostics=1" not in trial:
    print("rewrite/try_player.sh must accept --hardware-diagnostics before or after the host module path", file=sys.stderr)
    sys.exit(1)
if trial.count('--rebuild)') < 2 or "needs_rebuild=1" not in trial or 'if [ "$needs_rebuild" = "1" ]; then' not in trial:
    print("rewrite/try_player.sh must accept --rebuild before or after the host module path and otherwise avoid redundant rebuilds", file=sys.stderr)
    sys.exit(1)
if "rewrite/iplay_rewrite.h" not in trial:
    print("rewrite/try_player.sh must rebuild when the shared rewrite/player header is newer than the selected trial EXE", file=sys.stderr)
    sys.exit(1)
if trial.count('--blocks=*)') < 2 or 'set -- $player_args "$name" "$@"' not in trial:
    print("rewrite/try_player.sh must accept --blocks=N before or after the host module path and pass it before the DOS module name", file=sys.stderr)
    sys.exit(1)
if trial.count('--video-mode=*)') < 2 or 'player_args="${player_args}${player_args:+ }$1"' not in trial:
    print("rewrite/try_player.sh must accept --video-mode=MODE before or after the host module path and pass it before the DOS module name", file=sys.stderr)
    sys.exit(1)
if "trial_video_mode_arg=" not in trial or "trial_video_mode_default=80x25color" not in trial or "trial_video_mode_key=$(printf '%s' \"${trial_video_mode_arg:-$trial_video_mode_default}\" | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz')" not in trial:
    print("rewrite/try_player.sh must normalize requested trial video mode for RES.TXT evidence", file=sys.stderr)
    sys.exit(1)
if 'if [ -n "${trial_video_mode_arg:-}" ]; then' not in trial:
    print("rewrite/try_player.sh must only default video mode when the user did not explicitly request one", file=sys.stderr)
    sys.exit(1)
for mode_marker in ["trial_video_mode=40x25bw", "trial_video_mode=40x25color", "trial_video_mode=80x25bw", "trial_video_mode=80x50", "trial_video_mode=80x25color"]:
    if mode_marker not in trial:
        print(f"rewrite/try_player.sh must report supported trial video mode marker {mode_marker}", file=sys.stderr)
        sys.exit(1)
if "trial_blocks_set=0" not in trial or 'IPLAY_TRIAL_DIAGNOSTIC_BLOCKS:-32' not in trial:
    print("rewrite/try_player.sh diagnostic mode must default to a bounded --blocks value when the user does not supply one", file=sys.stderr)
    sys.exit(1)
if '"$name" "$@"' not in trial:
    print("rewrite/try_player.sh must copy the first host module path even when extra player args are present", file=sys.stderr)
    sys.exit(1)
if "trial_diagnostics=${IPLAY_TRIAL_DIAGNOSTICS:-1}" not in trial:
    print("rewrite/try_player.sh must default to bounded diagnostics so normal kvikdos trials do not wait for the quiet continuous timeout", file=sys.stderr)
    sys.exit(1)
if trial.count('--quiet)') < 2 or "trial_diagnostics=0" not in trial:
    print("rewrite/try_player.sh must accept --quiet before or after the host module path for continuous quiet trial playback", file=sys.stderr)
    sys.exit(1)
if "IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYTRY.EXE}" not in trial:
    print("rewrite/try_player.sh must still offer IPLAYTRY.EXE for explicit quiet kvikdos trial playback", file=sys.stderr)
    sys.exit(1)
if "try_player: trial executable not found after player build:" not in trial:
    print("rewrite/try_player.sh must fail clearly before kvikdos when IPLAY_TRIAL_EXE is not produced by the player build", file=sys.stderr)
    sys.exit(1)
if "IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYDIAG.EXE}" not in trial or 'if [ "$trial_diagnostics" = "1" ]; then' not in trial:
    print("rewrite/try_player.sh must default to IPLAYDIAG.EXE bounded diagnostic mode for headless kvikdos evidence", file=sys.stderr)
    sys.exit(1)
if "IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYHW.EXE}" not in trial or 'if [ "$trial_hardware_diagnostics" = "1" ]; then' not in trial:
    print("rewrite/try_player.sh must offer IPLAYHW.EXE diagnostic mode for real-SB16 unavailable evidence", file=sys.stderr)
    sys.exit(1)
if "trial_audio_mode=real-sb16-hardware" not in trial or "trial_audio_mode=wrapper-sb16-kvikdos-not-audible" not in trial:
    print("rewrite/try_player.sh must identify whether the selected trial EXE uses real SB16 hardware or the kvikdos wrapper seam", file=sys.stderr)
    sys.exit(1)
for policy in [
    "trial_loop_policy=bounded-diagnostics",
    "trial_loop_policy=continuous-diagnostics",
    "trial_loop_policy=continuous-quiet-wrapper",
    "trial_loop_policy=continuous-real-sb16",
    "trial_loop_policy=continuous-real-sb16-diagnostics",
    "trial_loop_policy=custom",
]:
    if policy not in trial:
        print(f"rewrite/try_player.sh must report trial loop policy {policy}", file=sys.stderr)
        sys.exit(1)
for scope in [
    "trial_proof_scope=playable-wrapper-diagnostic",
    "trial_proof_scope=playable-wrapper-continuous",
    "trial_proof_scope=production-real-sb16",
    "trial_proof_scope=hardware-unavailable-probe",
    "trial_proof_scope=custom",
]:
    if scope not in trial:
        print(f"rewrite/try_player.sh must report trial proof scope {scope}", file=sys.stderr)
        sys.exit(1)
if 'IPLAYC.EXE|IPLAYHW.EXE)' not in trial:
    print("rewrite/try_player.sh must reserve audio_mode=real-sb16-hardware for explicit real-SB16 EXE overrides", file=sys.stderr)
    sys.exit(1)
if 'printf \'trial_exe=%s diagnostics=%s hardware_diagnostics=%s production=%s rebuild=%s needs_rebuild=%s\\n\' "$IPLAY_TRIAL_EXE" "$trial_diagnostics" "$trial_hardware_diagnostics" "$trial_production" "$trial_rebuild" "$needs_rebuild"' not in trial:
    print("rewrite/try_player.sh must write a trial header before kvikdos output", file=sys.stderr)
    sys.exit(1)
if "host_module_size=$(wc -c < \"$host_module\" | tr -d ' ')" not in trial:
    print("rewrite/try_player.sh must measure host module size before copying/running it", file=sys.stderr)
    sys.exit(1)
# Guarded shell marker: host_module_size=$(wc -c < "$host_module" | tr -d ' ')
if "printf 'host_module_size=%s\\n' \"$host_module_size\"" not in trial:
    print("rewrite/try_player.sh must write host module size to RES.TXT", file=sys.stderr)
    sys.exit(1)
# Guarded shell marker: printf 'host_module_size=%s\n' "$host_module_size"
if "printf 'audio_mode=%s\\n' \"$trial_audio_mode\"" not in trial:
    print("rewrite/try_player.sh must write audio mode into RES.TXT", file=sys.stderr)
    sys.exit(1)
if "printf 'dos_args='" not in trial or "sep=\n  for arg in \"$@\"; do" not in trial:
    print("rewrite/try_player.sh must record the effective DOS player argument list in RES.TXT", file=sys.stderr)
    sys.exit(1)
if "trial_mode_note=quiet-player-no-diagnostic-stdout" not in trial:
    print("rewrite/try_player.sh must explain quiet trial mode when RES.TXT has no player diagnostics", file=sys.stderr)
    sys.exit(1)
if "quiet_trial_timeout=yes meaning=headless-run-ended-by-timeout-not-by-player-exit" not in trial:
    print("rewrite/try_player.sh must label quiet kvikdos timeout kills explicitly", file=sys.stderr)
    sys.exit(1)
if "quiet_trial_completed=yes meaning=player-exited-without-diagnostic-stdout" not in trial:
    print("rewrite/try_player.sh must label successful quiet exits explicitly", file=sys.stderr)
    sys.exit(1)
if "trial_result=quiet-completed-no-diagnostics" not in trial:
    print("rewrite/try_player.sh must classify clean quiet wrapper exits without pretending they have diagnostic playback proof", file=sys.stderr)
    sys.exit(1)
for marker in [
    "trial_module_loaded=yes",
    "trial_module_loaded=no",
    "trial_loaded_module_name=%s",
    "trial_loaded_module_name=none",
    "trial_loaded_module_key=%s",
    "trial_requested_module_key=%s",
    "trial_requested_module_loaded=yes",
    "trial_requested_module_loaded=no",
    "trial_module_size=%s",
    "trial_module_size=none",
    "trial_module_size_matches_host=yes",
    "trial_module_size_matches_host=no",
    "trial_module_loader_line=%s",
    "trial_module_loader_line=none",
    "trial_module_loader=%s",
    "trial_module_loader=none",
    "trial_module_type_tag=%s",
    "trial_module_type_tag=none",
    "trial_module_title=%s",
    "trial_module_title=none",
    "trial_ok_loader_metadata=yes",
    "trial_ok_loader_metadata=no",
    "trial_playback_pump=yes",
    "trial_playback_pump=no",
    "trial_playback_valid=yes",
    "trial_playback_valid=no",
    "trial_screen_present=yes",
    "trial_screen_present=no",
    "trial_playback_position_present=yes",
    "trial_playback_position_present=no",
    "trial_playback_position_valid=yes",
    "trial_playback_position_valid=no",
    "trial_playback_position_geometry_valid=yes",
    "trial_playback_position_geometry_valid=no",
    "trial_color_probe_valid=yes",
    "trial_color_probe_valid=no",
    "trial_post_playback_status_present=yes",
    "trial_post_playback_status_present=no",
    "trial_post_playback_status_valid=yes",
    "trial_post_playback_status_valid=no",
    "trial_post_playback_status_geometry_valid=yes",
    "trial_post_playback_status_geometry_valid=no",
    "trial_audio_unavailable=yes",
    "trial_audio_unavailable=no",
    "trial_audio_unavailable_source=screen",
    "trial_audio_unavailable_source=exit-code",
    "trial_audio_unavailable_source=none",
    "trial_playback_line=%s",
    "trial_screen_reasons=%s",
    "trial_pcm_source_line=%s",
    "trial_pcm_source_line=none",
    "trial_pcm_provider=%s",
    "trial_pcm_renderer=%s",
    "trial_pcm_route=%s",
    "trial_pcm_input=%s",
    "trial_pcm_truncated=%s",
    "trial_pcm_hook_provider=%s",
    "trial_pcm_stream_start=%s",
    "trial_decoder_route_line=%s",
    "trial_decoder_route_line=none",
    "trial_decoder_route_id=%s",
    "trial_decoder_route_name=%s",
    "trial_decoder_handoff_line=%s",
    "trial_decoder_handoff_line=none",
    "trial_decoder_handoff=%s",
    "trial_decoder_handoff=none",
    "trial_pcm_provider=none",
    "trial_pcm_renderer=none",
    "trial_pcm_route=none",
    "trial_pcm_input=none",
    "trial_pcm_truncated=none",
    "trial_pcm_hook_provider=missing",
    "trial_pcm_stream_start=none",
    "trial_decoder_route_id=none",
    "trial_decoder_route_name=none",
]:
    if marker not in trial:
        print(f"rewrite/try_player.sh must write trial summary marker {marker}", file=sys.stderr)
        sys.exit(1)
for marker in [
    "trial_result=bounded-ui-playback-ok",
    "trial_result=source-ended-ui-ok",
    "trial_result=module-size-mismatch",
    "trial_result=loader-metadata-invalid",
    "trial_result=decoder-route-missing",
    "trial_result=decoder-handoff-missing",
    "trial_result=pcm-source-missing",
    "trial_result=playback-pump-invalid",
    "trial_result=screen-evidence-invalid",
    "trial_result=post-screen-evidence-invalid",
    "trial_result=requested-module-not-loaded",
    "trial_result=playback-without-screen",
    "trial_result=audio-unavailable",
    "trial_result=quiet-completed-no-diagnostics",
    "trial_result=kvikdos-timeout",
    "trial_result=exited-without-playback-pump",
    "trial_result=failed",
]:
    if marker not in trial:
        print(f"rewrite/try_player.sh must classify trial result {marker}", file=sys.stderr)
        sys.exit(1)
TRIAL_RESULT_STATUS_MARKERS = """
trial_result=loader-metadata-invalid
  trial_script_exit_status=4
trial_module_size=$(grep '^Size: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | sed 's/^Size: //; s/ bytes$//')
[ "$trial_module_size" = "$host_module_size" ]
grep '^trial_module_size_matches_host=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1
trial_result=module-size-mismatch
  trial_script_exit_status=4
grep '^trial_module_size_matches_host=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1
trial_result=decoder-route-missing
  trial_script_exit_status=4
grep '^trial_decoder_route_line=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_decoder_route_id=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_decoder_route_name=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1
trial_result=decoder-handoff-missing
  trial_script_exit_status=4
trial_result=pcm-source-missing
  trial_script_exit_status=4
grep '^trial_pcm_source_line=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_pcm_provider=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_pcm_input=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_pcm_hook_provider=missing$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_pcm_stream_start=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1
trial_result=playback-pump-invalid
  trial_script_exit_status=4
grep '^trial_playback_pump=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^trial_playback_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1
trial_result=screen-evidence-invalid
  trial_script_exit_status=4
grep '^trial_playback_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && { grep '^trial_playback_position_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_playback_position_geometry_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; }
trial_result=post-screen-evidence-invalid
  trial_script_exit_status=4
trial_result=requested-module-not-loaded
  trial_script_exit_status=4
grep '^trial_requested_module_loaded=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1
grep '^trial_playback_valid=yes$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && grep '^Playback pump: .* stop=block-limit' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && { grep '^trial_post_playback_status_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 || grep '^trial_post_playback_status_geometry_valid=no$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; }
trial_module_loader_line=$(grep '^Loader: ' "$IPLAY_TRIAL_LOG" | tail -n 1)
trial_module_loader=$(printf '%s\n' "$trial_module_loader_line" | sed 's/^Loader: //')
trial_module_type_tag=$(grep '^Module type tag: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | sed 's/^Module type tag: //')
trial_module_title=$(grep '^Title: ' "$IPLAY_TRIAL_LOG" | tail -n 1 | sed 's/^Title: //')
grep '^trial_module_type_tag=[0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F]$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1
! grep '^trial_module_type_tag=00000000$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1
grep '^Size: ' "$IPLAY_TRIAL_LOG"
grep '^Loader: ' "$IPLAY_TRIAL_LOG"
grep '^Module type tag: ' "$IPLAY_TRIAL_LOG"
grep '^Title: ' "$IPLAY_TRIAL_LOG"
grep '^PCM source: ' "$IPLAY_TRIAL_LOG"
grep '^Decoder route: ' "$IPLAY_TRIAL_LOG"
grep '^Decoder handoff: ' "$IPLAY_TRIAL_LOG"
grep '^trial_decoder_handoff=[^ ]' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1 && ! grep '^trial_decoder_handoff=none$' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1
trial_decoder_handoff_line=$(grep '^Decoder handoff: ' "$IPLAY_TRIAL_LOG" | tail -n 1)
trial_decoder_handoff=$(printf '%s\n' "$trial_decoder_handoff_line" | sed 's/^Decoder handoff: //')
grep '^Playback pump: blocks=[1-9][0-9]* frames=[1-9][0-9]* accepted=[1-9][0-9]* checksum=[1-9][0-9]* .* stop=' "$IPLAY_TRIAL_LOG"
grep '^Screen present: reason=playback-position ' "$IPLAY_TRIAL_LOG"
grep '^Screen present: reason=playback-position .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' "$IPLAY_TRIAL_LOG"
grep '^Screen present: reason=post-playback-status .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' "$IPLAY_TRIAL_LOG"
"""
for reason in [
    "trial_failure_reason=none",
    "trial_failure_reason=audio-pump-without-valid-screen-present",
    "trial_failure_reason=module-size-mismatch",
    "trial_failure_reason=loader-metadata-invalid",
    "trial_failure_reason=decoder-route-missing",
    "trial_failure_reason=decoder-handoff-missing",
    "trial_failure_reason=pcm-source-missing",
    "trial_failure_reason=playback-pump-invalid",
    "trial_failure_reason=screen-evidence-invalid",
    "trial_failure_reason=post-screen-evidence-invalid",
    "trial_failure_reason=requested-module-not-loaded",
    "trial_failure_reason=sb16-audio-unavailable",
    "trial_failure_reason=emulator-timeout",
    "trial_failure_reason=no-playback-pump-evidence",
    "trial_failure_reason=player-process-failed",
    "trial_failure_reason=unknown",
]:
    if reason not in trial:
        print(f"rewrite/try_player.sh must classify trial failure reason {reason}", file=sys.stderr)
        sys.exit(1)
for probe in [
    "grep '^Module: ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Size: ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Loader: ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Module type tag: ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Title: ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Playback pump: .* stop=' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Playback pump: blocks=[1-9][0-9]* frames=[1-9][0-9]* accepted=[1-9][0-9]* checksum=[1-9][0-9]* .* stop=' \"$IPLAY_TRIAL_LOG\"",
    "grep '^PCM source: ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Decoder route: ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Decoder handoff: ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Screen present: ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Screen present: reason=playback-position ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Screen present: reason=playback-position .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Screen present: reason=post-playback-status .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Screen present: reason=audio-unavailable ' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Playback pump: .* stop=block-limit' \"$IPLAY_TRIAL_LOG\"",
    "grep '^Playback pump: .* stop=source-end' \"$IPLAY_TRIAL_LOG\"",
]:
    if probe not in trial:
        print(f"rewrite/try_player.sh must derive trial summary from DOS output with {probe}", file=sys.stderr)
        sys.exit(1)
if "grep '^trial_requested_module_loaded=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_playback_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh OK results must require loaded-module evidence before playback/UI evidence", file=sys.stderr)
    sys.exit(1)
if "trial_loaded_module_name=$(grep '^Module: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | sed 's/^Module: //')" not in trial:
    print("rewrite/try_player.sh must record the loaded module name from DOS output", file=sys.stderr)
    sys.exit(1)
if "trial_loaded_module_key=$(printf '%s' \"$trial_loaded_module_name\" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" not in trial:
    print("rewrite/try_player.sh must normalize loaded module name for DOS-style comparison", file=sys.stderr)
    sys.exit(1)
if "trial_requested_module_key=$(printf '%s' \"$name\" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" not in trial:
    print("rewrite/try_player.sh must normalize requested module name for DOS-style comparison", file=sys.stderr)
    sys.exit(1)
if '[ "$trial_loaded_module_key" = "$trial_requested_module_key" ]' not in trial:
    print("rewrite/try_player.sh OK results must require normalized requested module name to match loaded module", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_requested_module_loaded=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh OK classifications must use requested-module match marker", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_module_size_matches_host=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh OK classifications must require DOS module size to match host file size", file=sys.stderr)
    sys.exit(1)
if "trial_module_size=$(grep '^Size: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | sed 's/^Size: //; s/ bytes$//')" not in trial:
    print("rewrite/try_player.sh must record the final DOS module size without its prefix/suffix", file=sys.stderr)
    sys.exit(1)
if "[ \"$trial_module_size\" = \"$host_module_size\" ]" not in trial:
    print("rewrite/try_player.sh must compare DOS-reported module size against the host file size", file=sys.stderr)
    sys.exit(1)
if "elif [ \"$rc\" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_module_size_matches_host=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1; then\n  trial_result=module-size-mismatch\n  trial_script_exit_status=4" not in trial:
    print("rewrite/try_player.sh must fail the command when a loaded module size differs from the host file", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_module_size_matches_host=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh module-size-mismatch must require explicit host/DOS size mismatch evidence", file=sys.stderr)
    sys.exit(1)
if "elif [ \"$rc\" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_ok_loader_metadata=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1; then\n  trial_result=loader-metadata-invalid\n  trial_script_exit_status=4" not in trial:
    print("rewrite/try_player.sh must fail the command when a loaded module has invalid loader metadata", file=sys.stderr)
    sys.exit(1)
if "trial_result=decoder-route-missing\n  trial_script_exit_status=4" not in trial:
    print("rewrite/try_player.sh must fail the command when a loaded module lacks decoder route evidence", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_decoder_route_line=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_decoder_route_id=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_decoder_route_name=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh decoder-route-missing must require missing route line/id/name evidence", file=sys.stderr)
    sys.exit(1)
if "elif [ \"$rc\" -eq 0 ] && grep '^trial_requested_module_loaded=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_decoder_handoff=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1; then\n  trial_result=decoder-handoff-missing\n  trial_script_exit_status=4" not in trial:
    print("rewrite/try_player.sh must fail the command when a loaded module lacks decoder handoff evidence", file=sys.stderr)
    sys.exit(1)
if "trial_result=pcm-source-missing\n  trial_script_exit_status=4" not in trial:
    print("rewrite/try_player.sh must fail the command when a loaded module lacks PCM source evidence", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_pcm_source_line=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_pcm_provider=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_pcm_input=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_pcm_hook_provider=missing$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_pcm_stream_start=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh pcm-source-missing must require missing PCM source/provider/input/hook-provider/stream-start evidence", file=sys.stderr)
    sys.exit(1)
if "trial_result=playback-pump-invalid\n  trial_script_exit_status=4" not in trial:
    print("rewrite/try_player.sh must fail the command when a loaded module has invalid playback-pump evidence", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_playback_pump=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_playback_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh playback-pump-invalid must require a present but invalid playback-pump summary", file=sys.stderr)
    sys.exit(1)
if "trial_result=screen-evidence-invalid\n  trial_script_exit_status=4" not in trial:
    print("rewrite/try_player.sh must fail the command when a loaded module has invalid playback screen evidence", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_playback_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && { grep '^trial_playback_position_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_playback_position_geometry_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1; }" not in trial:
    print("rewrite/try_player.sh screen-evidence-invalid must require valid playback with invalid playback-position screen evidence", file=sys.stderr)
    sys.exit(1)
if "trial_result=post-screen-evidence-invalid\n  trial_script_exit_status=4" not in trial:
    print("rewrite/try_player.sh must fail the command when bounded playback has invalid post-playback screen evidence", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_playback_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^Playback pump: .* stop=block-limit' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && { grep '^trial_post_playback_status_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_post_playback_status_geometry_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1; }" not in trial:
    print("rewrite/try_player.sh post-screen-evidence-invalid must require bounded playback with invalid post-playback screen evidence", file=sys.stderr)
    sys.exit(1)
if "trial_result=requested-module-not-loaded\n  trial_script_exit_status=4" not in trial:
    print("rewrite/try_player.sh must fail the command when the requested module was not loaded", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_requested_module_loaded=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh requested-module-not-loaded must require requested-module mismatch evidence", file=sys.stderr)
    sys.exit(1)
if "trial_module_loader_line=$(grep '^Loader: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1)" not in trial:
    print("rewrite/try_player.sh must record the final DOS loader line from trial output", file=sys.stderr)
    sys.exit(1)
if "trial_module_loader=$(printf '%s\\n' \"$trial_module_loader_line\" | sed 's/^Loader: //')" not in trial:
    print("rewrite/try_player.sh must strip the Loader prefix before writing trial_module_loader", file=sys.stderr)
    sys.exit(1)
if "trial_module_type_tag=$(grep '^Module type tag: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | sed 's/^Module type tag: //')" not in trial:
    print("rewrite/try_player.sh must record the final DOS module type tag without its prefix", file=sys.stderr)
    sys.exit(1)
if "trial_module_title=$(grep '^Title: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | sed 's/^Title: //')" not in trial:
    print("rewrite/try_player.sh must record the final DOS module title without its prefix when present", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_module_loader=[^ ]' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_module_loader=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_module_type_tag=[0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F]$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_module_type_tag=00000000$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh must accept loader metadata only when loader is present and module type tag is nonzero 8-digit hex", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_playback_position_geometry_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh OK classifications must require selected video geometry on playback-position screen", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_post_playback_status_geometry_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh bounded OK classification must require selected video geometry on post-playback-status screen", file=sys.stderr)
    sys.exit(1)
if 'grep "^Screen present: reason=playback-position .* cols=$trial_video_cols rows=$trial_video_rows " "$IPLAY_TRIAL_LOG" >/dev/null 2>&1' not in trial:
    print("rewrite/try_player.sh must derive playback-position geometry validity from selected trial video mode", file=sys.stderr)
    sys.exit(1)
if 'grep "^Screen present: reason=post-playback-status .* cols=$trial_video_cols rows=$trial_video_rows " "$IPLAY_TRIAL_LOG" >/dev/null 2>&1' not in trial:
    print("rewrite/try_player.sh must derive post-playback-status geometry validity from selected trial video mode", file=sys.stderr)
    sys.exit(1)
if "! grep '^trial_pcm_provider=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_pcm_input=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_pcm_hook_provider=missing$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_pcm_stream_start=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh OK results must require PCM source provider/route/input/stream-start evidence", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_decoder_route_id=[0-9][0-9]*$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_decoder_route_name=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh OK results must require decoder route id/name evidence", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_decoder_handoff=[^ ]' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_decoder_handoff=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh OK results must require decoder handoff evidence", file=sys.stderr)
    sys.exit(1)
if "trial_decoder_handoff_line=$(grep '^Decoder handoff: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1)" not in trial:
    print("rewrite/try_player.sh must record the final DOS decoder handoff line", file=sys.stderr)
    sys.exit(1)
if "trial_decoder_handoff=$(printf '%s\\n' \"$trial_decoder_handoff_line\" | sed 's/^Decoder handoff: //')" not in trial:
    print("rewrite/try_player.sh must strip the Decoder handoff prefix before writing trial_decoder_handoff", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_playback_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_playback_position_geometry_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial or "! grep '^trial_pcm_provider=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_pcm_input=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_pcm_hook_provider=missing$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_pcm_stream_start=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^Playback pump: .* stop=source-end' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh source-ended OK result must require valid playback-position screen evidence after audio", file=sys.stderr)
    sys.exit(1)
if "grep '^trial_post_playback_status_geometry_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_decoder_route_id=[0-9][0-9]*$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh source-ended OK result must require valid post-playback status screen evidence after audio", file=sys.stderr)
    sys.exit(1)
if "! grep '^trial_pcm_input=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh OK results must reject missing PCM input evidence", file=sys.stderr)
    sys.exit(1)
if "! grep '^trial_pcm_provider=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" not in trial:
    print("rewrite/try_player.sh OK results must reject missing PCM provider evidence", file=sys.stderr)
    sys.exit(1)
if '"$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" "$IPLAY_TRIAL_EXE" "$@" >> "$IPLAY_TRIAL_LOG" 2>&1' not in trial:
    print("rewrite/try_player.sh must hard-kill stuck kvikdos trial playback", file=sys.stderr)
    sys.exit(1)
if "host_module=%s dos_module=%s" not in trial:
    print("rewrite/try_player.sh must append host and DOS module names to RES.TXT", file=sys.stderr)
    sys.exit(1)
if "kvikdos_timeout_seconds=%s" not in trial or "kvikdos_timeout=yes seconds=%s" not in trial:
    print("rewrite/try_player.sh must make kvikdos timeout kills explicit in RES.TXT", file=sys.stderr)
    sys.exit(1)
if "trial_exe=%s exit_status=%s" not in trial:
    print("rewrite/try_player.sh must append the trial executable and exit status to RES.TXT", file=sys.stderr)
    sys.exit(1)
if "elif [ \"$rc\" -eq 0 ] && [ \"$trial_loop_policy\" = \"continuous-quiet-wrapper\" ] && grep '^quiet_trial_completed=yes ' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1; then\n  trial_result=quiet-completed-no-diagnostics" not in trial:
    print("rewrite/try_player.sh must treat clean quiet wrapper completion as a successful quiet run", file=sys.stderr)
    sys.exit(1)
if "trial_failure_reason=%s" not in trial:
    print("rewrite/try_player.sh must append a concise trial failure reason to RES.TXT", file=sys.stderr)
    sys.exit(1)
if "trial_loop_policy=%s" not in trial:
    print("rewrite/try_player.sh must write the selected playback loop policy to RES.TXT", file=sys.stderr)
    sys.exit(1)
if "trial_proof_scope=%s" not in trial:
    print("rewrite/try_player.sh must write the selected trial proof scope to RES.TXT", file=sys.stderr)
    sys.exit(1)
if "trial_video_mode=%s cols=%s rows=%s" not in trial:
    print("rewrite/try_player.sh must write expected trial video geometry to RES.TXT", file=sys.stderr)
    sys.exit(1)
if "trial_script_exit_status=%s" not in trial:
    print("rewrite/try_player.sh must append the trial script proof exit status to RES.TXT", file=sys.stderr)
    sys.exit(1)
if "trial_script_exit_status=4" not in trial or 'exit "$trial_script_exit_status"' not in trial:
    print("rewrite/try_player.sh must fail the command when playback/UI evidence is incomplete despite player exit 0", file=sys.stderr)
    sys.exit(1)
if 'elif grep \'^trial_audio_unavailable=yes$\' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then\n  trial_result=audio-unavailable\n  if [ "$rc" -eq 0 ]; then\n    trial_script_exit_status=4' not in trial:
    print("rewrite/try_player.sh must not report audio-unavailable with player exit 0 as a successful playable trial", file=sys.stderr)
    sys.exit(1)
for field in ["provider", "renderer", "route", "input", "truncated", "hook_provider", "stream_start"]:
    if f'case "$trial_pcm_source_line" in *" {field}="*)' not in trial:
        print(f"rewrite/try_player.sh must mark missing PCM {field} evidence as none", file=sys.stderr)
        sys.exit(1)
for field in ["id", "name"]:
    if f'case "$trial_decoder_route_line" in *" {field}="*)' not in trial:
        print(f"rewrite/try_player.sh must mark missing decoder route {field} evidence as none", file=sys.stderr)
        sys.exit(1)
build = pathlib.Path("rewrite/build_rewrite.sh").read_text()
if 'KVIKDOS_SECONDS=${KVIKDOS_SECONDS:-3}' not in build:
    print("generated dos runner must default to a short kvikdos timeout", file=sys.stderr)
    sys.exit(1)
if 'exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS"' not in build:
    print("generated dos runner must hard-kill stuck kvikdos children after a short grace period", file=sys.stderr)
    sys.exit(1)
if 'exec "$KVIKDOS_TIMEOUT" "$KVIKDOS_SECONDS" "$KVIKDOS"' in build:
    print("generated dos runner contains a kvikdos route without hard-kill grace period", file=sys.stderr)
    sys.exit(1)
runner = pathlib.Path("rewrite/.build/iplay_rewrite_dos_runner").read_text()
if 'KVIKDOS_SECONDS=${KVIKDOS_SECONDS:-3}' not in runner:
    print("current dos runner must default to a short kvikdos timeout", file=sys.stderr)
    sys.exit(1)
if 'exec "$KVIKDOS_TIMEOUT" "$KVIKDOS_SECONDS" "$KVIKDOS"' in runner:
    print("current dos runner contains a kvikdos route without hard-kill grace period", file=sys.stderr)
    sys.exit(1)
PY

run_pytest_no_skip env IPLAY_REQUIRE_FULL_UNIT_COVERAGE=1 pytest -q tests/test_unit_coverage_inventory.py

run_pytest_no_skip pytest -q tests/test_player_smoke.py

run_pytest_no_skip pytest -q tests/test_modplug_renderer.py

run_pytest_no_skip pytest -q tests/test_player_behavior.py

run_pytest_no_skip pytest -q tests/test_function_parity.py

rm -rf tests/__pycache__ .pytest_cache

# Preferred launcher must preserve DOS-style case-insensitive module lookup on a
# case-sensitive host filesystem. This keeps original-shaped uppercase module
# names usable while still proving the SDL/notcurses/libmodplug path.
run_clean_normal_playback ./iplay.sh samples/ARYX.S3M
run_clean_normal_playback ./rewrite/iplay.sh samples/ARYX.S3M
run_check_playback_ready ./iplay.sh --check-playback samples/ARYX.S3M
run_check_playback_ready ./rewrite/iplay.sh --check-playback samples/ARYX.S3M

run_clean_extension_inventory_output() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  "$@" --list-extensions >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "extension inventory command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "extension inventory command wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ "$(wc -l <"$out")" -ne 1 ]; then
    echo "extension inventory command did not write exactly one line: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! tr -d '\r' <"$out" | grep -E '^extensions=(\.[a-z0-9]+)(,\.[a-z0-9]+)*$' >/dev/null; then
    echo "extension inventory command wrote malformed output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -F '.inr' "$out" >/dev/null; then
    echo "extension inventory command incorrectly exposed deferred INR format: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_clean_classify_output() {
  launcher=$1
  path=$2
  expected=$3
  out=$(mktemp)
  err=$(mktemp)
  set +e
  "$launcher" --classify "$path" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "classify command returned unexpected status $rc: $launcher --classify $path" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "classify command wrote stderr: $launcher --classify $path" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ "$(wc -l <"$out")" -ne 1 ]; then
    echo "classify command did not write exactly one line: $launcher --classify $path" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if [ "$(tr -d '\r' <"$out")" != "$expected" ]; then
    echo "classify command output mismatch: $launcher --classify $path" >&2
    echo "expected: $expected" >&2
    echo "actual:" >&2
    cat "$out" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_clean_extension_inventory_output ./iplay.sh
run_clean_extension_inventory_output ./rewrite/iplay.sh
run_clean_classify_output ./iplay.sh samples/aryx.s3m 'external=1 project=0 route_id=0 route=external-library library=1 backend="SDL-compatible SB16 16-bit stereo"'
run_clean_classify_output ./rewrite/iplay.sh samples/aryx.s3m 'external=1 project=0 route_id=0 route=external-library library=1 backend="SDL-compatible SB16 16-bit stereo"'

# Decoder-route control commands must stay stable for DOS-shaped uppercase names,
# deferred project-owned formats, and probe-by-content files. These are control
# boundaries for the future C/C++ rewrite, not playback implementations.
run_clean_classify_output ./iplay.sh samples/ARYX.S3M 'external=1 project=0 route_id=0 route=external-library library=1 backend="SDL-compatible SB16 16-bit stereo"'
run_clean_classify_output ./rewrite/iplay.sh samples/ARYX.S3M 'external=1 project=0 route_id=0 route=external-library library=1 backend="SDL-compatible SB16 16-bit stereo"'
run_clean_classify_output ./iplay.sh /tmp/DEFERRED.INR 'external=0 project=1 route_id=1 route=project-owned library=0 backend="SDL-compatible SB16 16-bit stereo"'
run_clean_classify_output ./rewrite/iplay.sh /tmp/DEFERRED.INR 'external=0 project=1 route_id=1 route=project-owned library=0 backend="SDL-compatible SB16 16-bit stereo"'
run_clean_classify_output ./iplay.sh /tmp/UNKNOWN.BIN 'external=0 project=0 route_id=2 route=probe-by-content library=1 backend="SDL-compatible SB16 16-bit stereo"'
run_clean_classify_output ./rewrite/iplay.sh /tmp/UNKNOWN.BIN 'external=0 project=0 route_id=2 route=probe-by-content library=1 backend="SDL-compatible SB16 16-bit stereo"'

run_clean_launcher_ready_output() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  "$@" --check >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "launcher readiness command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "launcher readiness command wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ "$(wc -l <"$out")" -ne 1 ]; then
    echo "launcher readiness command did not write exactly one line: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -E '^iplay\.sh: ready: exe=rewrite/\.build/iplay rebuilt=[01]$' "$out" >/dev/null; then
    echo "launcher readiness command wrote malformed output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Inertia Player|Terminal render|SDL audio sink|Playback pump|status=|Module:' "$out"; then
    echo "launcher readiness command leaked playback/diagnostic output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_clean_launcher_ready_output ./iplay.sh
run_clean_launcher_ready_output ./rewrite/iplay.sh

run_clean_default_terminal_size_playback() {
  columns=$1
  lines=$2
  shift 2
  out=$(mktemp)
  err=$(mktemp)
  set +e
  printf 'q\n' | SDL_AUDIODRIVER=dummy COLUMNS=$columns LINES=$lines "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "default terminal-size playback returned unexpected status $rc for ${columns}x${lines}: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "default terminal-size playback wrote stderr for ${columns}x${lines}: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! python3 - "$out" <<'PY'
import pathlib
import sys

data = pathlib.Path(sys.argv[1]).read_bytes()
required = [
    b"\x1b[?25l",
    b"Inertia Player V1.22",
    b"Filename      : ",
    b"aryx.s3m",
    b"Playing in Stereo",
    b"Playing in Stereo",
    b"Output Levels : ",
    b"Module Type   : ",
    b"S3M",
]
missing = [item for item in required if item not in data]
if missing:
    print("missing terminal default UI bytes: " + ", ".join(repr(item) for item in missing), file=sys.stderr)
    sys.exit(1)
PY
  then
    echo "default terminal-size playback did not emit required UI for ${columns}x${lines}: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Terminal render end|^(File list|Module|Size|Loader|Module type tag|Title|Terminal render|Selected text mode|SDL audio sink|Stdin keyboard|status=|Playback pump):' "$out"; then
    echo "default terminal-size playback leaked diagnostic output for ${columns}x${lines}: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_clean_default_terminal_size_playback 40 25 "$launcher" samples/aryx.s3m
  run_clean_default_terminal_size_playback 80 25 "$launcher" samples/aryx.s3m
  run_clean_default_terminal_size_playback 80 50 "$launcher" samples/aryx.s3m
done

run_filelist_default_terminal_size_playback_checks() {
  fixture_dir=$(mktemp -d)
  cp samples/aryx.s3m "$fixture_dir/aryx.s3m"
  printf '  ARYX.S3M  \n' >"$fixture_dir/PLAYLIST.LST"
  for launcher in ./iplay.sh ./rewrite/iplay.sh; do
    run_clean_default_terminal_size_playback 40 25 "$launcher" "@$fixture_dir/PLAYLIST.LST"
    run_clean_default_terminal_size_playback 80 25 "$launcher" "@$fixture_dir/PLAYLIST.LST"
    run_clean_default_terminal_size_playback 80 50 "$launcher" "@$fixture_dir/PLAYLIST.LST"
    run_check_playback_ready "$launcher" --check-playback "@$fixture_dir/PLAYLIST.LST"
  done
  rm -rf "$fixture_dir"
}

run_filelist_default_terminal_size_playback_checks

run_clean_keyboard_stop_playback() {
  stop_input=$1
  stop_name=$2
  shift 2
  out=$(mktemp)
  err=$(mktemp)
  set +e
  printf '%b' "$stop_input" | SDL_AUDIODRIVER=dummy COLUMNS=80 LINES=50 "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "keyboard-stop playback returned unexpected status $rc for $stop_name: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "keyboard-stop playback wrote stderr for $stop_name: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! python3 - "$out" <<'PY'
import pathlib
import sys

data = pathlib.Path(sys.argv[1]).read_bytes()
required = [
    b"\x1b[?25l",
    b"Inertia Player V1.22",
    b"Filename      : ",
    b"aryx.s3m",
    b"Playing in Stereo",
    b"Playing in Stereo",
    b"Output Levels : ",
    b"L[",
    b"] R[",
    b"Module Type   : ",
    b"S3M",
]
missing = [item for item in required if item not in data]
if missing:
    print("missing keyboard-stop UI bytes: " + ", ".join(repr(item) for item in missing), file=sys.stderr)
    sys.exit(1)
PY
  then
    echo "keyboard-stop playback did not emit required UI for $stop_name: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Terminal render end|^(File list|Module|Size|Loader|Module type tag|Title|Terminal render|Selected text mode|SDL audio sink|Stdin keyboard|status=|Playback pump):' "$out"; then
    echo "keyboard-stop playback leaked diagnostic output for $stop_name: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_clean_keyboard_stop_playback 'Q\n' uppercase-Q "$launcher" samples/aryx.s3m
  run_clean_keyboard_stop_playback '\033' escape "$launcher" samples/aryx.s3m
done

# Real terminals are rarely exactly the DOS text geometry. Normal playback must
# still map common non-exact sizes to a supported text mode without leaking
# diagnostics or requiring an explicit --video-mode.
for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_clean_default_terminal_size_playback 50 30 "$launcher" samples/aryx.s3m
  run_clean_default_terminal_size_playback 120 40 "$launcher" samples/aryx.s3m
  run_clean_default_terminal_size_playback 132 60 "$launcher" samples/aryx.s3m
done

# Undersized terminals should still fall back to a supported DOS text geometry
# and render cleanly. This protects the notcurses-style terminal wrapper from
# depending on the host terminal already being at least 40x25.
for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_clean_default_terminal_size_playback 20 10 "$launcher" samples/aryx.s3m
  run_clean_default_terminal_size_playback 39 24 "$launcher" samples/aryx.s3m
  run_clean_default_terminal_size_playback 79 24 "$launcher" samples/aryx.s3m
done

run_diagnostics_color_probe_evidence() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  printf 'q\n' | SDL_AUDIODRIVER=dummy COLUMNS=80 LINES=50 "$@" --diagnostics --video-mode=80x50 samples/aryx.s3m >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "diagnostic color-probe command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "diagnostic color-probe command wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'Color probe: target_fg=16 target_bg=8 cells=24 fg_matches=16 bg_matches=8 blink_matches=8 fg_mask=ffff bg_mask=ff blink_mask=aa present_calls=1 bytes=4000 cols=80 rows=25' "$out" >/dev/null; then
    echo "diagnostic color-probe command did not prove 16-color terminal cells: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'screen80x50_present=calls:1 bytes:8000 cols:80 rows:50' "$out" >/dev/null; then
    echo "diagnostic color-probe command did not preserve 80x50 screen evidence: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_diagnostics_color_probe_evidence ./iplay.sh
run_diagnostics_color_probe_evidence ./rewrite/iplay.sh

run_diagnostics_subwindow_resize_evidence() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  printf 'q\n' | SDL_AUDIODRIVER=dummy COLUMNS=80 LINES=50 "$@" --diagnostics --video-mode=80x50 samples/aryx.s3m >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "diagnostic subwindow command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "diagnostic subwindow command wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -E '^Resize present: phase=before bytes=4000 .* cols=80 rows=25 resize_ok=1 audio_frames=512 levels=[1-9][0-9]*/[1-9][0-9]*$' "$out" >/dev/null; then
    echo "diagnostic subwindow command did not prove 80x25 resize-before present: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -E '^Resize present: phase=after bytes=8000 .* cols=80 rows=50 resize_ok=1 audio_frames=512 levels=[1-9][0-9]*/[1-9][0-9]*$' "$out" >/dev/null; then
    echo "diagnostic subwindow command did not prove 80x50 resize-after present: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -E '^Subwindow present: origin=3,5 rows=5 cols=34 .* present_cols=80 present_rows=25 audio_frames=512 levels=[1-9][0-9]*/[1-9][0-9]*$' "$out" >/dev/null; then
    echo "diagnostic subwindow command did not prove subwindow present with live audio levels: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'subwindow_title="     SUBWINDOW' "$out" >/dev/null; then
    echo "diagnostic subwindow command did not expose subwindow title text: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'subwindow_audio="     Audio: SDL-compatible SB16 16-bit' "$out" >/dev/null; then
    echo "diagnostic subwindow command did not expose SDL/SB16 subwindow audio text: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_diagnostics_subwindow_resize_evidence ./iplay.sh
run_diagnostics_subwindow_resize_evidence ./rewrite/iplay.sh

# Readiness preflight must accept the same DOS-style case-insensitive text-mode
# aliases as the player, instead of rejecting them before playback can start.
for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_check_playback_ready "$launcher" --check-playback --video-mode=80X50 samples/aryx.s3m
  run_check_playback_ready "$launcher" --check-playback --video-mode=40X25MONO samples/aryx.s3m
  run_check_playback_ready "$launcher" --check-playback --video-mode=80x25mono samples/aryx.s3m
done

# Normal playback must accept the same DOS-style case-insensitive text-mode
# aliases as readiness preflight and the player parser.
for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_clean_normal_playback "$launcher" --video-mode=80X50 samples/aryx.s3m
  run_clean_normal_playback "$launcher" --video-mode=40X25MONO samples/aryx.s3m
  run_clean_normal_playback "$launcher" --video-mode=80x25mono samples/aryx.s3m
done

run_filelist_video_mode_alias_playback_checks() {
  fixture_dir=$(mktemp -d)
  cp samples/aryx.s3m "$fixture_dir/aryx.s3m"
  printf '  ARYX.S3M  \n' >"$fixture_dir/PLAYLIST.LST"
  for launcher in ./iplay.sh ./rewrite/iplay.sh; do
    run_clean_normal_playback "$launcher" --video-mode=80X50 "@$fixture_dir/PLAYLIST.LST"
    run_clean_normal_playback "$launcher" --video-mode=40X25MONO "@$fixture_dir/PLAYLIST.LST"
    run_clean_normal_playback "$launcher" --video-mode=80x25mono "@$fixture_dir/PLAYLIST.LST"
    run_check_playback_ready "$launcher" --check-playback --video-mode=80X50 "@$fixture_dir/PLAYLIST.LST"
    run_check_playback_ready "$launcher" --check-playback --video-mode=40X25MONO "@$fixture_dir/PLAYLIST.LST"
    run_check_playback_ready "$launcher" --check-playback --video-mode=80x25mono "@$fixture_dir/PLAYLIST.LST"
  done
  rm -rf "$fixture_dir"
}

run_filelist_video_mode_alias_playback_checks

run_diagnostics_audio_backend_identity() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  printf 'q\n' | SDL_AUDIODRIVER=dummy COLUMNS=80 LINES=50 "$@" --diagnostics --video-mode=80x50 samples/aryx.s3m >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "diagnostic audio-backend command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "diagnostic audio-backend command wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'Playback output: SDL-compatible SB16 16-bit stereo native.' "$out" >/dev/null; then
    echo "diagnostic audio-backend command did not report SDL-compatible SB16 16-bit stereo output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -E '^SDL audio sink: requested=1 opened=1 bytes=2048 queue_failures=0 freq=44100 format=0x8010 channels=2 samples=1024 queue_limit_bytes=16384 queue_waits=0 driver=dummy closed=1$' "$out" >/dev/null; then
    echo "diagnostic audio-backend command did not prove opened SB16 stereo SDL sink parameters: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'PCM source: external_module seed=1 truncated=0 input=file-path renderer=e route=0 provider=libmikmod hook_provider=libmikmod stream_start=0' "$out" >/dev/null; then
    echo "diagnostic audio-backend command did not preserve libmodplug PCM source evidence: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_diagnostics_audio_backend_identity ./iplay.sh
run_diagnostics_audio_backend_identity ./rewrite/iplay.sh

run_diagnostics_status_audio_levels() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  printf 'q\n' | SDL_AUDIODRIVER=dummy COLUMNS=80 LINES=50 "$@" --diagnostics --video-mode=80x50 samples/aryx.s3m >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "diagnostic status-level command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "diagnostic status-level command wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -E '^status=keyboard route_id=0 route=external-library provider=libmikmod stop=keyboard source_end=0 blocks=1 source_frames=512 .*levels=[1-9][0-9]*,[1-9][0-9]* maxlevels=[1-9][0-9]*,[1-9][0-9]*' "$out" >/dev/null; then
    echo "diagnostic status-level command did not prove live/max stereo levels in status summary: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -E '^Level sequence: target=16 samples=16 nonzero=16 changed=1 .* max=[1-9][0-9]*,[1-9][0-9]* .* status=keyboard stop=keyboard$' "$out" >/dev/null; then
    echo "diagnostic status-level command did not prove changing stereo level sequence: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_diagnostics_status_audio_levels ./iplay.sh
run_diagnostics_status_audio_levels ./rewrite/iplay.sh

# Complete the explicit DOS-style text-mode alias surface for the preferred
# SDL/notcurses launchers. These modes are accepted case-insensitively by the
# player and by readiness preflight.
for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_clean_normal_playback "$launcher" --video-mode=40X25BW samples/aryx.s3m
  run_clean_normal_playback "$launcher" --video-mode=40X25COLOR samples/aryx.s3m
  run_clean_normal_playback "$launcher" --video-mode=80X25BW samples/aryx.s3m
  run_clean_normal_playback "$launcher" --video-mode=80X25COLOR samples/aryx.s3m
  run_check_playback_ready "$launcher" --check-playback --video-mode=40X25BW samples/aryx.s3m
  run_check_playback_ready "$launcher" --check-playback --video-mode=40X25COLOR samples/aryx.s3m
  run_check_playback_ready "$launcher" --check-playback --video-mode=80X25BW samples/aryx.s3m
  run_check_playback_ready "$launcher" --check-playback --video-mode=80X25COLOR samples/aryx.s3m
done

run_clean_launcher_help_output() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  "$@" --help >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "launcher help command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$out" ]; then
    echo "launcher help command wrote stdout: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'usage:' "$err" >/dev/null; then
    echo "launcher help command did not print usage: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'supported --video-mode values: 40x25bw, 40x25color, 40x25mono, 80x25bw, 80x25color, 80x25mono, 80x50, 80x50project, auto, terminal' "$err" >/dev/null; then
    echo "launcher help command did not print supported video modes: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'a bare text-mode token may be supplied before or after <module-file|@file-list>' "$err" >/dev/null; then
    echo "launcher help command did not print bare mode-token ordering contract: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'press q, Q, or Escape to stop playback' "$err" >/dev/null; then
    echo "launcher help command did not print keyboard stop contract: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Inertia Player|Terminal render|SDL audio sink|Playback pump|status=|Module:' "$err"; then
    echo "launcher help command leaked playback/diagnostic output: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_clean_launcher_help_output ./iplay.sh
run_clean_launcher_help_output ./rewrite/iplay.sh

run_clean_launcher_no_args_usage() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "launcher no-args command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$out" ]; then
    echo "launcher no-args command wrote stdout: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'usage:' "$err" >/dev/null; then
    echo "launcher no-args command did not print usage: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'supported --video-mode values: 40x25bw, 40x25color, 40x25mono, 80x25bw, 80x25color, 80x25mono, 80x50, 80x50project, auto, terminal' "$err" >/dev/null; then
    echo "launcher no-args command did not print supported video modes: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'a bare text-mode token may be supplied before or after <module-file|@file-list>' "$err" >/dev/null; then
    echo "launcher no-args command did not print bare mode-token ordering contract: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Inertia Player|Terminal render|SDL audio sink|Playback pump|status=|Module:' "$err"; then
    echo "launcher no-args command leaked playback/diagnostic output: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_clean_launcher_no_args_usage ./iplay.sh
run_clean_launcher_no_args_usage ./rewrite/iplay.sh

run_clean_launcher_rebuild_ready_output() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  "$@" --rebuild --check >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "launcher forced-rebuild readiness command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "launcher forced-rebuild readiness command wrote stderr: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ "$(wc -l <"$out")" -ne 1 ]; then
    echo "launcher forced-rebuild readiness command did not write exactly one line: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: ready: exe=rewrite/.build/iplay rebuilt=1' "$out" >/dev/null; then
    echo "launcher forced-rebuild readiness command did not report rebuilt=1: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Inertia Player|Terminal render|SDL audio sink|Playback pump|status=|Module:' "$out"; then
    echo "launcher forced-rebuild readiness command leaked playback/diagnostic output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_clean_launcher_rebuild_ready_output ./iplay.sh
run_clean_launcher_rebuild_ready_output ./rewrite/iplay.sh

# Forced rebuild followed by actual playback must preserve the same clean
# SDL/notcurses/SB16-stereo runtime contract as the normal launcher path.
run_clean_normal_playback ./iplay.sh --rebuild samples/aryx.s3m
run_clean_normal_playback ./rewrite/iplay.sh --rebuild samples/aryx.s3m

# Forced rebuild followed by the short playback readiness probe must also be
# clean. This is the quickest refresh-and-prove command before interactive use.
run_check_playback_ready ./iplay.sh --rebuild --check-playback samples/aryx.s3m
run_check_playback_ready ./rewrite/iplay.sh --rebuild --check-playback samples/aryx.s3m

# Original-shaped bare text-mode tokens after the module path must reach clean
# SDL/notcurses playback. Readiness uses a controlled fixed mode internally, so
# bare mode-token coverage belongs to normal runtime playback.
for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_clean_normal_playback "$launcher" samples/aryx.s3m 80X50
  run_clean_normal_playback "$launcher" samples/aryx.s3m 40X25MONO
  run_clean_normal_playback "$launcher" samples/aryx.s3m 80X25BW
  run_clean_normal_playback "$launcher" samples/aryx.s3m 80X25COLOR
done

run_filelist_bare_video_mode_playback_checks() {
  fixture_dir=$(mktemp -d)
  cp samples/aryx.s3m "$fixture_dir/aryx.s3m"
  printf '  ARYX.S3M  \n' >"$fixture_dir/PLAYLIST.LST"
  for launcher in ./iplay.sh ./rewrite/iplay.sh; do
    run_clean_normal_playback "$launcher" "@$fixture_dir/PLAYLIST.LST" 80X50
    run_clean_normal_playback "$launcher" "@$fixture_dir/PLAYLIST.LST" 40X25MONO
    run_clean_normal_playback "$launcher" "@$fixture_dir/PLAYLIST.LST" 80X25BW
    run_clean_normal_playback "$launcher" "@$fixture_dir/PLAYLIST.LST" 80X25COLOR
  done
  rm -rf "$fixture_dir"
}

run_filelist_bare_video_mode_playback_checks

# A bare text-mode token before the module is an original-shaped invocation.
# Normalize it to the supported module-then-mode order before launching the
# SDL/notcurses player or the short readiness probe.
for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_clean_normal_playback "$launcher" 80X50 samples/aryx.s3m
  run_clean_normal_playback "$launcher" 40X25MONO samples/aryx.s3m
done

run_filelist_leading_bare_video_mode_playback_checks() {
  fixture_dir=$(mktemp -d)
  cp samples/aryx.s3m "$fixture_dir/aryx.s3m"
  printf '  ARYX.S3M  \n' >"$fixture_dir/PLAYLIST.LST"
  for launcher in ./iplay.sh ./rewrite/iplay.sh; do
    run_clean_normal_playback "$launcher" 80X50 "@$fixture_dir/PLAYLIST.LST"
    run_clean_normal_playback "$launcher" 40X25MONO "@$fixture_dir/PLAYLIST.LST"
  done
  rm -rf "$fixture_dir"
}

run_filelist_leading_bare_video_mode_playback_checks

# Leading bare text-mode normalization must preserve original-style early failure
# behavior: missing modules/file-lists fail before SDL audio, terminal rendering,
# decoder routing, or playback starts.
run_missing_module_failure ./iplay.sh 80X50 /tmp/IPLAY_MISSING_MODULE.S3M
run_missing_module_failure ./rewrite/iplay.sh 80X50 /tmp/IPLAY_MISSING_MODULE.S3M

run_leading_mode_missing_filelist_failure() {
  out=$(mktemp)
  err=$(mktemp)
  launcher=$1
  mode_arg=$2
  filelist_arg=$3
  set +e
  SDL_AUDIODRIVER=not-a-real-driver "$launcher" "$mode_arg" "$filelist_arg" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "leading-mode missing file-list command returned unexpected status $rc: $launcher $mode_arg $filelist_arg" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F "iplay: could not resolve file list: $filelist_arg" "$err" >/dev/null; then
    echo "leading-mode missing file-list command did not report clear file-list error: $launcher $mode_arg $filelist_arg" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: player exited with status 2' "$err" >/dev/null; then
    echo "leading-mode missing file-list command did not report wrapper status: $launcher $mode_arg $filelist_arg" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'could not open SDL2 SB16 stereo audio sink|Inertia Player|^(File list|Module|Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out" "$err"; then
    echo "leading-mode missing file-list command leaked playback state: $launcher $mode_arg $filelist_arg" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_leading_mode_missing_filelist_failure ./iplay.sh 80X50 @/tmp/IPLAY_MISSING_FILELIST.LST
run_leading_mode_missing_filelist_failure ./rewrite/iplay.sh 80X50 @/tmp/IPLAY_MISSING_FILELIST.LST

run_launcher_usage_identity_output() {
  launcher=$1
  expected_usage=$2
  for mode in help noargs; do
    out=$(mktemp)
    err=$(mktemp)
    set +e
    if [ "$mode" = "help" ]; then
      "$launcher" --help >"$out" 2>"$err"
      rc=$?
      expected_rc=0
    else
      "$launcher" >"$out" 2>"$err"
      rc=$?
      expected_rc=2
    fi
    set -e
    if [ "$rc" -ne "$expected_rc" ]; then
      echo "launcher usage identity command returned unexpected status $rc for $mode: $launcher" >&2
      cat "$out"
      cat "$err" >&2
      rm -f "$out" "$err"
      exit 1
    fi
    if [ -s "$out" ]; then
      echo "launcher usage identity command wrote stdout for $mode: $launcher" >&2
      cat "$out"
      rm -f "$out" "$err"
      exit 1
    fi
    if ! grep -F "$expected_usage" "$err" >/dev/null; then
      echo "launcher usage identity command did not print expected usage for $mode: $launcher" >&2
      echo "expected: $expected_usage" >&2
      cat "$err" >&2
      rm -f "$out" "$err"
      exit 1
    fi
    rm -f "$out" "$err"
  done
}

run_launcher_usage_identity_output ./iplay.sh 'usage: ./iplay.sh [--check] [--rebuild] [--diagnostics] [iplay-options] <module-file|@file-list>'
run_launcher_usage_identity_output ./rewrite/iplay.sh 'usage: ./rewrite/iplay.sh [--check] [--rebuild] [--diagnostics] [iplay-options] <module-file|@file-list>'

run_invalid_leading_bare_video_mode_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "invalid leading bare video-mode command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay: unsupported text mode: 80x60' "$err" >/dev/null; then
    echo "invalid leading bare video-mode command did not report the bad leading mode: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -F 'iplay: unsupported text mode: /' "$err" >/dev/null; then
    echo "invalid leading bare video-mode command reported the module path as the bad mode: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'could not open SDL2 SB16 stereo audio sink|Inertia Player|^(File list|Module|Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out" "$err"; then
    echo "invalid leading bare video-mode command leaked playback state: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_invalid_leading_bare_video_mode_failure "$launcher" 80x60 samples/aryx.s3m
done

run_filelist_invalid_leading_bare_video_mode_failure_checks() {
  fixture_dir=$(mktemp -d)
  cp samples/aryx.s3m "$fixture_dir/aryx.s3m"
  printf '  ARYX.S3M  \n' >"$fixture_dir/PLAYLIST.LST"
  for launcher in ./iplay.sh ./rewrite/iplay.sh; do
    run_invalid_leading_bare_video_mode_failure "$launcher" 80x60 "@$fixture_dir/PLAYLIST.LST"
  done
  rm -rf "$fixture_dir"
}

run_filelist_invalid_leading_bare_video_mode_failure_checks

run_invalid_trailing_bare_video_mode_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "invalid trailing bare video-mode command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay: unsupported text mode: 80x60' "$err" >/dev/null; then
    echo "invalid trailing bare video-mode command did not report the bad trailing mode: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'could not open SDL2 SB16 stereo audio sink|Inertia Player|^(File list|Module|Selected text mode|Terminal render|SDL audio sink|Playback pump|status=):' "$out" "$err"; then
    echo "invalid trailing bare video-mode command leaked playback state: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_invalid_trailing_bare_video_mode_failure "$launcher" samples/aryx.s3m 80x60
done

run_filelist_invalid_trailing_bare_video_mode_failure_checks() {
  fixture_dir=$(mktemp -d)
  cp samples/aryx.s3m "$fixture_dir/aryx.s3m"
  printf '  ARYX.S3M  \n' >"$fixture_dir/PLAYLIST.LST"
  for launcher in ./iplay.sh ./rewrite/iplay.sh; do
    run_invalid_trailing_bare_video_mode_failure "$launcher" "@$fixture_dir/PLAYLIST.LST" 80x60
  done
  rm -rf "$fixture_dir"
}

run_filelist_invalid_trailing_bare_video_mode_failure_checks

run_invalid_bare_video_mode_readiness_failure() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  SDL_AUDIODRIVER=dummy "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 2 ]; then
    echo "invalid bare video-mode readiness command returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$out" ]; then
    echo "invalid bare video-mode readiness command wrote stdout: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay: unsupported text mode: 80x60' "$err" >/dev/null; then
    echo "invalid bare video-mode readiness command did not report the bad mode: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if ! grep -F 'iplay.sh: playback readiness check failed with status 2' "$err" >/dev/null; then
    echo "invalid bare video-mode readiness command did not report readiness failure status: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'could not open SDL2 SB16 stereo audio sink|iplay\.sh: playback-ready:|Playback pump:|Terminal render:|SDL audio sink|status=' "$err"; then
    echo "invalid bare video-mode readiness command leaked playback state: $*" >&2
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

for launcher in ./iplay.sh ./rewrite/iplay.sh; do
  run_invalid_bare_video_mode_readiness_failure "$launcher" --check-playback 80x60 samples/aryx.s3m
  run_invalid_bare_video_mode_readiness_failure "$launcher" --check-playback samples/aryx.s3m 80x60
done

run_filelist_invalid_bare_video_mode_readiness_failure_checks() {
  fixture_dir=$(mktemp -d)
  cp samples/aryx.s3m "$fixture_dir/aryx.s3m"
  printf '  ARYX.S3M  \n' >"$fixture_dir/PLAYLIST.LST"
  for launcher in ./iplay.sh ./rewrite/iplay.sh; do
    run_invalid_bare_video_mode_readiness_failure "$launcher" --check-playback 80x60 "@$fixture_dir/PLAYLIST.LST"
    run_invalid_bare_video_mode_readiness_failure "$launcher" --check-playback "@$fixture_dir/PLAYLIST.LST" 80x60
  done
  rm -rf "$fixture_dir"
}

run_filelist_invalid_bare_video_mode_readiness_failure_checks
