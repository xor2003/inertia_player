#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p rewrite/.build

WATCOM=/home/xor/watcom
export WATCOM
INCLUDE=/home/xor/watcom/h
export INCLUDE
PATH=/home/xor/watcom/binl64:$PATH
export PATH

WCC=/home/xor/watcom/binl64/wcc
WLINK=/home/xor/watcom/binl64/wlink
COMMON_CFLAGS="-q -bt=dos -3 -ml -os -s -zm -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -I/home/xor/watcom/h -Irewrite"
COMMON_HEADERS="rewrite/iplay_rewrite.h rewrite/iplay_player.c rewrite/iplay_abi_watcom.c"

needs_rebuild() {
  target=$1
  shift
  if [ ! -f "$target" ]; then
    return 0
  fi
  for dep in "$@"; do
    if [ "$dep" -nt "$target" ]; then
      return 0
    fi
  done
  return 1
}

compile_obj() {
  obj=$1
  src=$2
  flags=$3
  if needs_rebuild "$obj" "$src" rewrite/iplay_rewrite.h rewrite/build_player.sh; then
    "$WCC" $COMMON_CFLAGS $flags -fo="$obj" "$src"
  fi
}

write_link_file() {
  lnk=$1
  map=$2
  exe=$3
  shift 3
  tmp="$lnk.tmp"
  {
    echo "system dos"
    echo "libpath /home/xor/watcom/lib286/dos"
    echo "libpath /home/xor/watcom/lib286"
    echo "option quiet"
    echo "option map=$map"
    echo "option eliminate"
    echo "option stack=16384"
    echo "name $exe"
    for obj in "$@"; do
      echo "file $obj"
    done
  } > "$tmp"
  if [ ! -f "$lnk" ] || ! cmp -s "$tmp" "$lnk"; then
    mv "$tmp" "$lnk"
  else
    rm -f "$tmp"
  fi
}

link_exe() {
  exe=$1
  lnk=$2
  shift 2
  write_link_file "$lnk" "rewrite/.build/$(basename "$exe" .EXE).map" "$exe" "$@"
  if needs_rebuild "$exe" "$lnk" "$@"; then
    "$WLINK" @"$lnk"
  fi
}

compile_obj rewrite/.build/iplay_rewrite_zm.obj rewrite/iplay_rewrite.c ""
compile_obj rewrite/.build/iplay_abi_watcom_zm.obj rewrite/iplay_abi_watcom.c "-DIPLAY_PLAYER_OMIT_RISKY_UI_ABI"
compile_obj rewrite/.build/iplay_player_diag_zm.obj rewrite/iplay_player.c ""
compile_obj rewrite/.build/iplay_player_cont_zm.obj rewrite/iplay_player.c "-DIPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS -DIPLAY_PLAYER_ENABLE_DIAGNOSTICS=0 -DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1"
compile_obj rewrite/.build/iplay_player_try_zm.obj rewrite/iplay_player.c "-DIPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS -DIPLAY_PLAYER_ENABLE_DIAGNOSTICS=0"
compile_obj rewrite/.build/iplay_player_contdiag_zm.obj rewrite/iplay_player.c "-DIPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS"
compile_obj rewrite/.build/iplay_player_hwdiag_zm.obj rewrite/iplay_player.c "-DIPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS -DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1"

link_exe rewrite/.build/IPLAYC.EXE rewrite/.build/IPLAYC.lnk \
  rewrite/.build/iplay_rewrite_zm.obj \
  rewrite/.build/iplay_abi_watcom_zm.obj \
  rewrite/.build/iplay_player_cont_zm.obj

link_exe rewrite/.build/IPLAYTRY.EXE rewrite/.build/IPLAYTRY.lnk \
  rewrite/.build/iplay_rewrite_zm.obj \
  rewrite/.build/iplay_abi_watcom_zm.obj \
  rewrite/.build/iplay_player_try_zm.obj

link_exe rewrite/.build/IPLAYDIAG.EXE rewrite/.build/IPLAYDIAG.lnk \
  rewrite/.build/iplay_rewrite_zm.obj \
  rewrite/.build/iplay_abi_watcom_zm.obj \
  rewrite/.build/iplay_player_diag_zm.obj

link_exe rewrite/.build/IPLAYCONT.EXE rewrite/.build/IPLAYCONT.lnk \
  rewrite/.build/iplay_rewrite_zm.obj \
  rewrite/.build/iplay_abi_watcom_zm.obj \
  rewrite/.build/iplay_player_contdiag_zm.obj

link_exe rewrite/.build/IPLAYHW.EXE rewrite/.build/IPLAYHW.lnk \
  rewrite/.build/iplay_rewrite_zm.obj \
  rewrite/.build/iplay_abi_watcom_zm.obj \
  rewrite/.build/iplay_player_hwdiag_zm.obj
