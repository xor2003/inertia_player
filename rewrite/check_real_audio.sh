#!/bin/sh
set -eu
cd "$(dirname "$0")/.."

run_real_audio_playback() {
  out=$(mktemp)
  err=$(mktemp)
  set +e
  printf 'q\n' | env -u SDL_AUDIODRIVER COLUMNS=80 LINES=50 "$@" >"$out" 2>"$err"
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    echo "real SDL audio playback returned unexpected status $rc: $*" >&2
    cat "$out"
    cat "$err" >&2
    rm -f "$out" "$err"
    exit 1
  fi
  if [ -s "$err" ]; then
    echo "real SDL audio playback wrote stderr: $*" >&2
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
    b"Output Levels : ",
    b"L[#",
    b"R[#",
    b"Module Type   : ",
    b"S3M",
]
missing = [item for item in required if item not in data]
if missing:
    print("missing real-audio terminal UI bytes: " + ", ".join(repr(item) for item in missing), file=sys.stderr)
    sys.exit(1)
PY
  then
    echo "real SDL audio playback did not emit required UI: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  if grep -Eq 'Terminal render end|^(File list|Module|Size|Loader|Module type tag|Title|Terminal render|Selected text mode|SDL audio sink|Stdin keyboard|status=|Playback pump):' "$out"; then
    echo "real SDL audio playback leaked diagnostic output: $*" >&2
    cat "$out"
    rm -f "$out" "$err"
    exit 1
  fi
  rm -f "$out" "$err"
}

run_real_audio_playback ./iplay.sh samples/aryx.s3m
run_real_audio_playback ./rewrite/iplay.sh samples/aryx.s3m
