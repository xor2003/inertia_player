#!/bin/sh
set -eu
cd "$(dirname "$0")/.."

./rewrite/build_player.sh

cd rewrite/.build
rm -f SMOKE.*

PYTHONPATH=../../tests python3 - <<'PY'
from pathlib import Path
from player_behavior_fixtures import write_endcont_module, write_smoke_modules

write_smoke_modules(Path("."))
write_endcont_module(Path("."))
PY

check_loader() {
  file=$1
  loader=$2
  title=$3
  run_iplayc "$file" || return 1
  if ! printf '%s\n' "$out" | grep "Loader: $loader" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing loader %s\n%s\n' "$file" "$loader" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "$title" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing title %s\n%s\n' "$file" "$title" "$out" >&2
    return 1
  fi
}

run_iplayc() {
  file=$1
  set +e
  out=$(timeout -k 1 "${IPLAY_SMOKE_KVIKDOS_SECONDS:-3}" /home/xor/kvikdos/kvikdos IPLAYDIAG.EXE --blocks=32 "$file" 2>&1)
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    printf 'IPLAYDIAG smoke failed: %s exited with status %s\n%s\n' "$file" "$rc" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Decoder handoff:" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing playback handoff\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Decoder route: id=" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing decoder route\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Screen present: reason=status scope=full-screen bytes=.* screen_bytes=.* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=.* rows=.* mode_ok=1 audio_frames=0 levels=[0-9][0-9]*/[0-9][0-9]*" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing status screen present\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Screen present: reason=playback-position scope=full-screen bytes=.* screen_bytes=.* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=.* rows=.* mode_ok=1 audio_frames=16384 levels=[0-9][0-9]*/[0-9][0-9]*" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing playback screen present\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Screen present: reason=post-playback-status scope=status-only bytes=.* screen_bytes=.* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=.* rows=.* mode_ok=1 audio_frames=16384 levels=[0-9][0-9]*/[0-9][0-9]*" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing post-playback status screen present\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "PCM source: $loader" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing PCM source %s\n%s\n' "$file" "$loader" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "PCM source: .* seed=" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing module PCM seed\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Decoder geometry: orders=.* rows/order=64 restart=.* speed=.* tempo=.* channels=" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing decoder geometry\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Decoder event:" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing decoder event\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Decoder voice:" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing decoder voice\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Playback prime: ready=1 hw=1 backend=.* status=.* frames=16384 capacity=0 dropped=0 queued=0 levels=[0-9][0-9]*/[0-9][0-9]*" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing active SB16 playback prime\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Playback loop: mode=playback policy=bounded-trial cadence=immediate max_blocks=32 frames/block=512" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing SB16 playback loop\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Playback pump: blocks=32 frames=16384 accepted=65536 checksum=.* limit=1 source_end=0 stop=block-limit" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing SB16 playback pump\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Decoder progress: block=32/[0-9][0-9]* order=.* pattern=.* row=.* channel=.* tick=.* speed=.* tempo=.* ended=0 loop=0" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing decoder progress\n%s\n' "$file" "$out" >&2
    return 1
  fi
  return 0
}

run_video_mode_smoke() {
  file=$1
  mode=$2
  cols=$3
  rows=$4
  bytes=$5
  set +e
  out=$(timeout -k 1 "${IPLAY_SMOKE_KVIKDOS_SECONDS:-3}" /home/xor/kvikdos/kvikdos IPLAYDIAG.EXE --blocks=1 "--video-mode=$mode" "$file" 2>&1)
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    printf 'IPLAYDIAG video-mode smoke failed: %s %s exited with status %s\n%s\n' "$file" "$mode" "$rc" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Screen present: reason=playback-position scope=full-screen bytes=$bytes screen_bytes=$bytes screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=$cols rows=$rows mode_ok=1 audio_frames=[1-9][0-9]* levels=[0-9][0-9]*/[0-9][0-9]*" >/dev/null; then
    printf 'IPLAYDIAG video-mode smoke failed: %s %s missing playback-position geometry %sx%s/%s bytes\n%s\n' "$file" "$mode" "$cols" "$rows" "$bytes" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Screen present: reason=post-playback-status .* screen_bytes=$bytes .* cols=$cols rows=$rows mode_ok=1 audio_frames=[1-9][0-9]*" >/dev/null; then
    printf 'IPLAYDIAG video-mode smoke failed: %s %s missing post-playback geometry %sx%s/%s bytes\n%s\n' "$file" "$mode" "$cols" "$rows" "$bytes" "$out" >&2
    return 1
  fi
  return 0
}

run_iplaytry() {
  file=$1
  set +e
  out=$(timeout -k 1 "${IPLAY_SMOKE_KVIKDOS_SECONDS:-3}" /home/xor/kvikdos/kvikdos IPLAYTRY.EXE "$file" 2>&1)
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    printf 'IPLAYTRY smoke failed: %s exited with status %s\n%s\n' "$file" "$rc" "$out" >&2
    return 1
  fi
  if printf '%s\n' "$out" | grep "Playback disabled: SB16 not detected" >/dev/null; then
    printf 'IPLAYTRY smoke failed: %s unexpectedly used real SB16 unavailable path\n%s\n' "$file" "$out" >&2
    return 1
  fi
  return 0
}

run_iplaycont() {
  file=$1
  set +e
  out=$(timeout -k 1 "${IPLAY_SMOKE_KVIKDOS_SECONDS:-3}" /home/xor/kvikdos/kvikdos IPLAYCONT.EXE "$file" 2>&1)
  rc=$?
  set -e
  if [ "$rc" -ne 0 ]; then
    printf 'IPLAYCONT smoke failed: %s exited with status %s\n%s\n' "$file" "$rc" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Decoder route: id=" >/dev/null; then
    printf 'IPLAYCONT smoke failed: %s missing decoder route\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Playback loop: mode=playback policy=timer-keyboard cadence=timer max_blocks=0 frames/block=1024" >/dev/null; then
    printf 'IPLAYCONT smoke failed: %s missing continuous playback loop\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Playback pump: blocks=[1-9][0-9]* frames=[1-9][0-9]* accepted=[1-9][0-9]* checksum=.* limit=0 source_end=1 stop=source-end" >/dev/null; then
    printf 'IPLAYCONT smoke failed: %s missing source-end playback pump\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Screen present: reason=playback-position scope=full-screen bytes=.* screen_bytes=.* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=.* rows=.* mode_ok=1 audio_frames=[1-9][0-9]* levels=[0-9][0-9]*/[0-9][0-9]*" >/dev/null; then
    printf 'IPLAYCONT smoke failed: %s missing playback-position screen present\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Screen present: reason=post-playback-status scope=status-only bytes=.* screen_bytes=.* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=.* rows=.* mode_ok=1 audio_frames=[1-9][0-9]* levels=[0-9][0-9]*/[0-9][0-9]*" >/dev/null; then
    printf 'IPLAYCONT smoke failed: %s missing post-playback status screen present\n%s\n' "$file" "$out" >&2
    return 1
  fi
  return 0
}

run_iplayhw_unavailable() {
  file=$1
  set +e
  out=$(timeout -k 1 "${IPLAY_SMOKE_KVIKDOS_SECONDS:-3}" /home/xor/kvikdos/kvikdos IPLAYHW.EXE "$file" 2>&1)
  rc=$?
  set -e
  if [ "$rc" -ne 3 ]; then
    printf 'IPLAYHW smoke failed: %s expected audio-unavailable status 3, got %s\n%s\n' "$file" "$rc" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Playback disabled: SB16 not detected" >/dev/null; then
    printf 'IPLAYHW smoke failed: %s missing SB16 unavailable message\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Screen present: reason=audio-unavailable scope=full-screen bytes=.* screen_bytes=.* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=.* rows=.* mode_ok=1 audio_frames=0 levels=[0-9][0-9]*/[0-9][0-9]*" >/dev/null; then
    printf 'IPLAYHW smoke failed: %s missing audio-unavailable screen present\n%s\n' "$file" "$out" >&2
    return 1
  fi
  if printf '%s\n' "$out" | grep "Playback pump:" >/dev/null; then
    printf 'IPLAYHW smoke failed: %s unexpectedly entered playback pump\n%s\n' "$file" "$out" >&2
    return 1
  fi
  return 0
}

check_loader_tag() {
  file=$1
  loader=$2
  title=$3
  tag=$4
  run_iplayc "$file" || return 1
  if ! printf '%s\n' "$out" | grep "Loader: $loader" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing loader %s\n%s\n' "$file" "$loader" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "$title" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing title %s\n%s\n' "$file" "$title" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "Module type tag: $tag" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing module type tag %s\n%s\n' "$file" "$tag" "$out" >&2
    return 1
  fi
}

check_loader_line() {
  file=$1
  loader=$2
  title=$3
  line=$4
  run_iplayc "$file" || return 1
  if ! printf '%s\n' "$out" | grep "Loader: $loader" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing loader %s\n%s\n' "$file" "$loader" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "$title" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing title %s\n%s\n' "$file" "$title" "$out" >&2
    return 1
  fi
  if ! printf '%s\n' "$out" | grep "$line" >/dev/null; then
    printf 'IPLAYDIAG smoke failed: %s missing line %s\n%s\n' "$file" "$line" "$out" >&2
    return 1
  fi
}

run_iplaytry ENDCONT.S3M
run_iplaycont ENDCONT.S3M
run_iplayhw_unavailable SMOKE.S3M
run_video_mode_smoke SMOKE.S3M 40x25bw 40 25 2000
run_video_mode_smoke SMOKE.S3M 40x25color 40 25 2000
run_video_mode_smoke SMOKE.S3M 80x25bw 80 25 4000
run_video_mode_smoke SMOKE.S3M 80x25color 80 25 4000
run_video_mode_smoke SMOKE.S3M 80x50 80 50 8000
check_loader_line SMOKE.S3M "s3m_module" "SMOKE S3M" "Order preview: 00 01 02"
check_loader_line SMOKE.MOD "mod_n_t_module" "SMOKE MOD" "Order preview: 00 01 02 04 FE"
if ! printf '%s\n' "$out" | grep "Decoder event: period=855 note=1 octave=1 instrument=1 volume=64 effect=12 param=127" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: SMOKE.MOD missing decoded MOD clamped volume cell\n%s\n' "$out" >&2
  exit 1
fi
if ! printf '%s\n' "$out" | grep "Decoder voice: active=1 period=855 note=1 octave=1 instrument=1 volume=64 sample_len=4 sample_vol=64 loop=0/2 data=6204" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: SMOKE.MOD missing initial MOD voice state\n%s\n' "$out" >&2
  exit 1
fi
if ! printf '%s\n' "$out" | grep "Playback pump: blocks=32 frames=16384 accepted=65536 checksum=.* limit=1 source_end=0 stop=block-limit" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: SMOKE.MOD missing voice-derived PCM checksum\n%s\n' "$out" >&2
  exit 1
fi
if ! printf '%s\n' "$out" | grep "Decoder progress: block=32/7680" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: SMOKE.MOD missing Fxx speed update\n%s\n' "$out" >&2
  exit 1
fi
if ! printf '%s\n' "$out" | grep "Decoder event: period=855 note=1 octave=1 instrument=1 volume=64 effect=12 param=127" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: SMOKE.MOD missing final decoded MOD cursor cell\n%s\n' "$out" >&2
  exit 1
fi
if ! printf '%s\n' "$out" | grep "Decoder voice: active=0 period=855 note=1 octave=1 instrument=1 volume=64 sample_len=4 sample_vol=64 loop=0/2 data=6204" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: SMOKE.MOD missing ended non-loop MOD voice state\n%s\n' "$out" >&2
  exit 1
fi
check_loader_line SMOKE.NST "mod_n_t_module" "SMOKE NST" "Order preview: 00 01 02 04 FE"
check_loader BADORD.MOD "mod_n_t_module" "BADORD MOD"
if ! printf '%s\n' "$out" | grep "Orders: 129 Channels: 4" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: BADORD.MOD missing raw overlong MOD order count\n%s\n' "$out" >&2
  exit 1
fi
if ! printf '%s\n' "$out" | grep "Decoder geometry: orders=128 rows/order=64 restart=0 speed=6 tempo=125 channels=4" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: BADORD.MOD did not cap playback order count to 128\n%s\n' "$out" >&2
  exit 1
fi
if ! printf '%s\n' "$out" | grep "Decoder event: period=855 note=1 octave=1 instrument=1 volume=64 effect=12 param=127" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: BADORD.MOD did not sanitize FE order to pattern 0 event\n%s\n' "$out" >&2
  exit 1
fi
if ! printf '%s\n' "$out" | grep "Playback pump: blocks=32 frames=16384 accepted=65536 checksum=.* limit=1 source_end=0 stop=block-limit" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: BADORD.MOD did not preserve sanitized pattern 0 playback checksum\n%s\n' "$out" >&2
  exit 1
fi
if ! printf '%s\n' "$out" | grep "Decoder progress: block=32/65535" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: BADORD.MOD did not report sanitized current pattern 0\n%s\n' "$out" >&2
  exit 1
fi
check_loader FASTROW.MOD "mod_n_t_module" "FASTROW MOD"
if ! printf '%s\n' "$out" | grep "Decoder geometry: orders=3 rows/order=64 restart=0 speed=6 tempo=125 channels=4" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: FASTROW.MOD missing one-order decoder geometry\n%s\n' "$out" >&2
  exit 1
fi
case "$out" in
  *"Decoder progress: block=32/4608"*"order=0"*"row=2"*"speed=1"*) ;;
  *)
    printf 'IPLAYDIAG smoke failed: FASTROW.MOD did not apply E6x pattern loop under F01 timing\n%s\n' "$out" >&2
    exit 1
    ;;
esac
if ! printf '%s\n' "$out" | grep "Decoder voice: active=1 period=762 note=3 octave=1 instrument=1 volume=32 sample_len=4 sample_vol=64 loop=0/2 data=2108" >/dev/null; then
  printf 'IPLAYDIAG smoke failed: FASTROW.MOD missing durable row-1 voice state\n%s\n' "$out" >&2
  exit 1
fi
check_loader_line SMOKE.MTM "mtm_module" "SMOKE MTM" "Order preview: 00 01 02 03 04"
check_loader SMOKE.FAR "far_module" "SMOKE FAR"
check_loader_tag SMOKE.669 "e669_module" "SMOKE669" "39363645"
check_loader SMOKE.ULT "ult_module" "SMOKE ULT"
check_loader SMOKE.PSM "psm_module" "SMOKE PSM"
check_loader SMOKE.INR "inr_module" "SMOKE INR"
check_loader SMOKE.STM "_2stm_module" "SMOKE STM"

rm -f SMOKE.* BADORD.* FASTROW.* ENDCONT.*
echo "IPLAYTRY/IPLAYCONT/IPLAYHW/IPLAYDIAG/TEXTMODE smoke ok"
