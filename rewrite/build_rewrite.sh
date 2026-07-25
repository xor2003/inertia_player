#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p rewrite/.build
rm -f rewrite/.build/IABI.EXE rewrite/.build/IABI.map
cc -std=c99 -O0 -Wall -Wextra -Irewrite rewrite/iplay_rewrite.c rewrite/rewrite_runner.c -o rewrite/.build/iplay_rewrite_runner
cc -std=c99 -O0 -Wall -Wextra -Irewrite rewrite/iplay_rewrite.c rewrite/sb16_commands_runner.c -o rewrite/.build/sb16_commands_runner
cc -std=c99 -O0 -Wall -Wextra -Irewrite -c rewrite/iplay_rewrite.c -o rewrite/.build/iplay_rewrite_for_modplug_audio.o
c++ -std=c++17 -O0 -Wall -Wextra $(pkg-config --cflags libmodplug sdl2 notcurses) -Irewrite \
  rewrite/modplug_renderer.cpp \
  rewrite/modplug_audio_bridge.cpp \
  rewrite/modern_player.cpp \
  rewrite/notcurses_presenter.cpp \
  rewrite/sdl_visualizer.cpp \
  rewrite/modplug_audio_probe.cpp \
  rewrite/.build/iplay_rewrite_for_modplug_audio.o \
  $(pkg-config --libs libmodplug sdl2 notcurses) \
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
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcc -q -3 -ms -s -I/home/xor/watcom/h -Irewrite -fo=rewrite/.build/iplay_rewrite.obj rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ml -os -s -fm=rewrite/.build/IRUN.map -Irewrite -fe=rewrite/.build/IRUN.EXE rewrite/iplay_rewrite.c rewrite/rewrite_runner.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ml -os -s -fm=rewrite/.build/IUIRUN.map -Irewrite -fe=rewrite/.build/IUIRUN.EXE rewrite/iplay_rewrite.c rewrite/text_wrapper_runner.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ml -os -s -fm=rewrite/.build/IARUN.map -Irewrite -fe=rewrite/.build/IARUN.EXE rewrite/iplay_rewrite.c rewrite/audio_wrapper_runner.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ml -os -s -DIPLAY_PLAYER_OMIT_RISKY_UI_ABI -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -fm=rewrite/.build/IPHWRUN.map -Irewrite -fe=rewrite/.build/IPHWRUN.EXE rewrite/player_hw_runner.c rewrite/iplay_rewrite.c rewrite/iplay_abi_watcom.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -fm=rewrite/.build/IABI_NULLSUB3.map -Irewrite -fe=rewrite/.build/IABI_NULLSUB3.EXE rewrite/abi_nullsub3_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_USELESS12D61.map -Irewrite -fe=rewrite/.build/IABI_USELESS12D61.EXE rewrite/abi_useless12d61_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_USELESSWRITEINR.map -Irewrite -fe=rewrite/.build/IABI_USELESSWRITEINR.EXE rewrite/abi_uselesswriteinr_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_USELESSWRITEINR118.map -Irewrite -fe=rewrite/.build/IABI_USELESSWRITEINR118.EXE rewrite/abi_uselesswriteinr118_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_MEMFILL8080.map -Irewrite -fe=rewrite/.build/IABI_MEMFILL8080.EXE rewrite/abi_memfill8080_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SNDOFFFILL.map -Irewrite -fe=rewrite/.build/IABI_SNDOFFFILL.EXE rewrite/abi_sndofffill_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_AUDIOINITFAIL.map -Irewrite -fe=rewrite/.build/IABI_AUDIOINITFAIL.EXE rewrite/abi_audioinitfail_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SBINITNODEVICE.map -Irewrite -fe=rewrite/.build/IABI_SBINITNODEVICE.EXE rewrite/abi_sbinitnodevice_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SBTESTINT.map -Irewrite -fe=rewrite/.build/IABI_SBTESTINT.EXE rewrite/abi_sbtestint_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SBONBOUNDED.map -Irewrite -fe=rewrite/.build/IABI_SBONBOUNDED.EXE rewrite/abi_sbonbounded_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SBHANDLERINT.map -Irewrite -fe=rewrite/.build/IABI_SBHANDLERINT.EXE rewrite/abi_sbhandlerint_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SNDONPARNT.map -Irewrite -fe=rewrite/.build/IABI_SNDONPARNT.EXE rewrite/abi_sndonparnt_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_TIMERBOUNDED.map -Irewrite -fe=rewrite/.build/IABI_TIMERBOUNDED.EXE rewrite/abi_timerbounded_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SNDGUARD.map -Irewrite -fe=rewrite/.build/IABI_SNDGUARD.EXE rewrite/abi_sndguard_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_TIMERINT.map -Irewrite -fe=rewrite/.build/IABI_TIMERINT.EXE rewrite/abi_timerint_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_READSNDSETTINGS.map -Irewrite -fe=rewrite/.build/IABI_READSNDSETTINGS.EXE rewrite/abi_readsndsettings_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_FILLDMAINACTIVE.map -Irewrite -fe=rewrite/.build/IABI_FILLDMAINACTIVE.EXE rewrite/abi_filldmainactive_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_FILELIST.map -Irewrite -fe=rewrite/.build/IABI_FILELIST.EXE rewrite/abi_filelist_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_FINDMODS.map -Irewrite -fe=rewrite/.build/IABI_FINDMODS.EXE rewrite/abi_findmods_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB126A9.map -Irewrite -fe=rewrite/.build/IABI_SUB126A9.EXE rewrite/abi_sub126a9_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB13623.map -Irewrite -fe=rewrite/.build/IABI_SUB13623.EXE rewrite/abi_sub13623_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_MOD102F5.map -Irewrite -fe=rewrite/.build/IABI_MOD102F5.EXE rewrite/abi_mod102f5_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB12CAD.map -Irewrite -fe=rewrite/.build/IABI_SUB12CAD.EXE rewrite/abi_sub12cad_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB1281A.map -Irewrite -fe=rewrite/.build/IABI_SUB1281A.EXE rewrite/abi_sub1281a_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_MOD1021E.map -Irewrite -fe=rewrite/.build/IABI_MOD1021E.EXE rewrite/abi_mod1021e_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_MOD1024A.map -Irewrite -fe=rewrite/.build/IABI_MOD1024A.EXE rewrite/abi_mod1024a_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -fm=rewrite/.build/IABI_MEMFREE18A28.map -Irewrite -fe=rewrite/.build/IABI_MEMFREE18A28.EXE rewrite/abi_memfree18a28_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB11C0C.map -Irewrite -fe=rewrite/.build/IABI_SUB11C0C.EXE rewrite/abi_sub11c0c_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB1415E.map -Irewrite -fe=rewrite/.build/IABI_SUB1415E.EXE rewrite/abi_sub1415e_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB12F56.map -Irewrite -fe=rewrite/.build/IABI_SUB12F56.EXE rewrite/abi_sub12f56_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB154F4.map -Irewrite -fe=rewrite/.build/IABI_SUB154F4.EXE rewrite/abi_sub154f4_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB135CA.map -Irewrite -fe=rewrite/.build/IABI_SUB135CA.EXE rewrite/abi_sub135ca_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB13813.map -Irewrite -fe=rewrite/.build/IABI_SUB13813.EXE rewrite/abi_sub13813_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB140B6.map -Irewrite -fe=rewrite/.build/IABI_SUB140B6.EXE rewrite/abi_sub140b6_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB137D5.map -Irewrite -fe=rewrite/.build/IABI_SUB137D5.EXE rewrite/abi_sub137d5_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB13429.map -Irewrite -fe=rewrite/.build/IABI_SUB13429.EXE rewrite/abi_sub13429_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB131DA.map -Irewrite -fe=rewrite/.build/IABI_SUB131DA.EXE rewrite/abi_sub131da_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB131EF.map -Irewrite -fe=rewrite/.build/IABI_SUB131EF.EXE rewrite/abi_sub131ef_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB13177.map -Irewrite -fe=rewrite/.build/IABI_SUB13177.EXE rewrite/abi_sub13177_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFFNIBBLE.map -Irewrite -fe=rewrite/.build/IABI_EFFNIBBLE.EXE rewrite/abi_effnibble_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13A43.map -Irewrite -fe=rewrite/.build/IABI_EFF13A43.EXE rewrite/abi_eff13a43_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13BB2.map -Irewrite -fe=rewrite/.build/IABI_EFF13BB2.EXE rewrite/abi_eff13bb2_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13AD7.map -Irewrite -fe=rewrite/.build/IABI_EFF13AD7.EXE rewrite/abi_eff13ad7_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13B78.map -Irewrite -fe=rewrite/.build/IABI_EFF13B78.EXE rewrite/abi_eff13b78_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13B06.map -Irewrite -fe=rewrite/.build/IABI_EFF13B06.EXE rewrite/abi_eff13b06_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13A94.map -Irewrite -fe=rewrite/.build/IABI_EFF13A94.EXE rewrite/abi_eff13a94_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13BC8.map -Irewrite -fe=rewrite/.build/IABI_EFF13BC8.EXE rewrite/abi_eff13bc8_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13C02.map -Irewrite -fe=rewrite/.build/IABI_EFF13C02.EXE rewrite/abi_eff13c02_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13C3F.map -Irewrite -fe=rewrite/.build/IABI_EFF13C3F.EXE rewrite/abi_eff13c3f_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13C64.map -Irewrite -fe=rewrite/.build/IABI_EFF13C64.EXE rewrite/abi_eff13c64_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13C88.map -Irewrite -fe=rewrite/.build/IABI_EFF13C88.EXE rewrite/abi_eff13c88_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13C95.map -Irewrite -fe=rewrite/.build/IABI_EFF13C95.EXE rewrite/abi_eff13c95_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13CA2.map -Irewrite -fe=rewrite/.build/IABI_EFF13CA2.EXE rewrite/abi_eff13ca2_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13CB3.map -Irewrite -fe=rewrite/.build/IABI_EFF13CB3.EXE rewrite/abi_eff13cb3_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13CC9.map -Irewrite -fe=rewrite/.build/IABI_EFF13CC9.EXE rewrite/abi_eff13cc9_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13CDD.map -Irewrite -fe=rewrite/.build/IABI_EFF13CDD.EXE rewrite/abi_eff13cdd_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13CE8.map -Irewrite -fe=rewrite/.build/IABI_EFF13CE8.EXE rewrite/abi_eff13ce8_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13DE.map -Irewrite -fe=rewrite/.build/IABI_EFF13DE.EXE rewrite/abi_eff13de_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13E1E.map -Irewrite -fe=rewrite/.build/IABI_EFF13E1E.EXE rewrite/abi_eff13e1e_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13E2D.map -Irewrite -fe=rewrite/.build/IABI_EFF13E2D.EXE rewrite/abi_eff13e2d_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13E32.map -Irewrite -fe=rewrite/.build/IABI_EFF13E32.EXE rewrite/abi_eff13e32_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13E7F.map -Irewrite -fe=rewrite/.build/IABI_EFF13E7F.EXE rewrite/abi_eff13e7f_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13E84.map -Irewrite -fe=rewrite/.build/IABI_EFF13E84.EXE rewrite/abi_eff13e84_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13E8C.map -Irewrite -fe=rewrite/.build/IABI_EFF13E8C.EXE rewrite/abi_eff13e8c_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13F05.map -Irewrite -fe=rewrite/.build/IABI_EFF13F05.EXE rewrite/abi_eff13f05_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13F3B.map -Irewrite -fe=rewrite/.build/IABI_EFF13F3B.EXE rewrite/abi_eff13f3b_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF13FBE.map -Irewrite -fe=rewrite/.build/IABI_EFF13FBE.EXE rewrite/abi_eff13fbe_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_AMPLIF.map -Irewrite -fe=rewrite/.build/IABI_AMPLIF.EXE rewrite/abi_amplif_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_CALC14043.map -Irewrite -fe=rewrite/.build/IABI_CALC14043.EXE rewrite/abi_calc14043_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF14030.map -Irewrite -fe=rewrite/.build/IABI_EFF14030.EXE rewrite/abi_eff14030_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFF14067.map -Irewrite -fe=rewrite/.build/IABI_EFF14067.EXE rewrite/abi_eff14067_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB13D95.map -Irewrite -fe=rewrite/.build/IABI_SUB13D95.EXE rewrite/abi_sub13d95_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB13E9B.map -Irewrite -fe=rewrite/.build/IABI_SUB13E9B.EXE rewrite/abi_sub13e9b_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_MIDI154.map -Irewrite -fe=rewrite/.build/IABI_MIDI154.EXE rewrite/abi_midi154_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_MIDI154AC.map -Irewrite -fe=rewrite/.build/IABI_MIDI154AC.EXE rewrite/abi_midi154ac_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_MIDI15413.map -Irewrite -fe=rewrite/.build/IABI_MIDI15413.EXE rewrite/abi_midi15413_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB15577.map -Irewrite -fe=rewrite/.build/IABI_SUB15577.EXE rewrite/abi_sub15577_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB1609F.map -Irewrite -fe=rewrite/.build/IABI_SUB1609F.EXE rewrite/abi_sub1609f_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SETVIDEOMODE.map -Irewrite -fe=rewrite/.build/IABI_SETVIDEOMODE.EXE rewrite/abi_setvideomode_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB1AB8C.map -Irewrite -fe=rewrite/.build/IABI_SUB1AB8C.EXE rewrite/abi_sub1ab8c_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_TXT1ABAE.map -Irewrite -fe=rewrite/.build/IABI_TXT1ABAE.EXE rewrite/abi_txt1abae_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB13826.map -Irewrite -fe=rewrite/.build/IABI_SUB13826.EXE rewrite/abi_sub13826_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_HEX.map -Irewrite -fe=rewrite/.build/IABI_HEX.EXE rewrite/abi_hex_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_DECIMAL.map -Irewrite -fe=rewrite/.build/IABI_DECIMAL.EXE rewrite/abi_decimal_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_STRLEN.map -Irewrite -fe=rewrite/.build/IABI_STRLEN.EXE rewrite/abi_strlen_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SETTINGS.map -Irewrite -fe=rewrite/.build/IABI_SETTINGS.EXE rewrite/abi_settings_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SPECTR.map -Irewrite -fe=rewrite/.build/IABI_SPECTR.EXE rewrite/abi_spectr_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_STRING.map -Irewrite -fe=rewrite/.build/IABI_STRING.EXE rewrite/abi_string_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_VIDEO.map -Irewrite -fe=rewrite/.build/IABI_VIDEO.EXE rewrite/abi_video_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB12D35.map -Irewrite -fe=rewrite/.build/IABI_SUB12D35.EXE rewrite/abi_sub12d35_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_EFFSLIDE.map -Irewrite -fe=rewrite/.build/IABI_EFFSLIDE.EXE rewrite/abi_effslide_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB13044.map -Irewrite -fe=rewrite/.build/IABI_SUB13044.EXE rewrite/abi_sub13044_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_TEXTMSG.map -Irewrite -fe=rewrite/.build/IABI_TEXTMSG.EXE rewrite/abi_textmsg_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_VISUAL.map -Irewrite -fe=rewrite/.build/IABI_VISUAL.EXE rewrite/abi_visual_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_UIEXTRA.map -Irewrite -fe=rewrite/.build/IABI_UIEXTRA.EXE rewrite/abi_uiextra_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ml -os -s -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -fm=rewrite/.build/IABI_SYSTEM.map -Irewrite -fe=rewrite/.build/IABI_SYSTEM.EXE rewrite/abi_system_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_VOLUMEPREP.map -Irewrite -fe=rewrite/.build/IABI_VOLUMEPREP.EXE rewrite/abi_volumeprep_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcl -q -bt=dos -3 -ms -os -s -fm=rewrite/.build/IABI_SUB19050.map -Irewrite -fe=rewrite/.build/IABI_SUB19050.EXE rewrite/abi_sub19050_runner.c rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcc -q -bt=dos -3 -ml -os -s -zm -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -I/home/xor/watcom/h -Irewrite -fo=rewrite/.build/iplay_rewrite_zm.obj rewrite/iplay_rewrite.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcc -q -bt=dos -3 -ml -os -s -zm -DIPLAY_PLAYER_OMIT_RISKY_UI_ABI -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -I/home/xor/watcom/h -Irewrite -fo=rewrite/.build/iplay_abi_watcom_zm.obj rewrite/iplay_abi_watcom.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcc -q -bt=dos -3 -ml -os -s -zm -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -I/home/xor/watcom/h -Irewrite -fo=rewrite/.build/iplay_abi_watcom_full.obj rewrite/iplay_abi_watcom.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcc -q -bt=dos -3 -ml -os -s -zm -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -I/home/xor/watcom/h -I/home/xor/watcom/h -Irewrite -fo=rewrite/.build/iplay_player_diag_zm.obj rewrite/iplay_player.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcc -q -bt=dos -3 -ml -os -s -zm -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -DIPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS -DIPLAY_PLAYER_ENABLE_DIAGNOSTICS=0 -DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1 -I/home/xor/watcom/h -Irewrite -fo=rewrite/.build/iplay_player_cont_zm.obj rewrite/iplay_player.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcc -q -bt=dos -3 -ml -os -s -zm -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -DIPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS -DIPLAY_PLAYER_ENABLE_DIAGNOSTICS=0 -I/home/xor/watcom/h -Irewrite -fo=rewrite/.build/iplay_player_try_zm.obj rewrite/iplay_player.c
# inventory marker: -DIPLAY_PLAYER_ENABLE_DIAGNOSTICS=0 -Irewrite -fo=rewrite/.build/iplay_player_try_zm.obj rewrite/iplay_player.c
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wcc -q -bt=dos -3 -ml -os -s -zm -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -DIPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS -I/home/xor/watcom/h -Irewrite -fo=rewrite/.build/iplay_player_contdiag_zm.obj rewrite/iplay_player.c
/home/xor/watcom/binl64/wcc -q -bt=dos -3 -ml -os -s -zm -DIPLAY_REWRITE_EXTERNAL_MEMFREE18A28 -DIPLAY_REWRITE_OMIT_PUBLIC_NOOP_STUBS -DIPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS -DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1 -I/home/xor/watcom/h -Irewrite -fo=rewrite/.build/iplay_player_hwdiag_zm.obj rewrite/iplay_player.c
# inventory marker: -DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1 -Irewrite -fo=rewrite/.build/iplay_player_hwdiag_zm.obj rewrite/iplay_player.c
cat > rewrite/.build/IPLAYC.lnk <<'LINK'
system dos
libpath /home/xor/watcom/lib286/dos
libpath /home/xor/watcom/lib286
option quiet
option map=rewrite/.build/IPLAYC.map
option eliminate
option stack=16384
name rewrite/.build/IPLAYC.EXE
file rewrite/.build/iplay_rewrite_zm.obj
file rewrite/.build/iplay_abi_watcom_zm.obj
file rewrite/.build/iplay_player_cont_zm.obj
LINK
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYC.lnk
cat > rewrite/.build/IPLAYTRY.lnk <<'LINK'
system dos
libpath /home/xor/watcom/lib286/dos
libpath /home/xor/watcom/lib286
option quiet
option map=rewrite/.build/IPLAYTRY.map
option eliminate
option stack=16384
name rewrite/.build/IPLAYTRY.EXE
file rewrite/.build/iplay_rewrite_zm.obj
file rewrite/.build/iplay_abi_watcom_zm.obj
file rewrite/.build/iplay_player_try_zm.obj
LINK
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYTRY.lnk
cat > rewrite/.build/IPLAYDIAG.lnk <<'LINK'
system dos
libpath /home/xor/watcom/lib286/dos
libpath /home/xor/watcom/lib286
option quiet
option map=rewrite/.build/IPLAYDIAG.map
option eliminate
option stack=16384
name rewrite/.build/IPLAYDIAG.EXE
file rewrite/.build/iplay_rewrite_zm.obj
file rewrite/.build/iplay_abi_watcom_zm.obj
file rewrite/.build/iplay_player_diag_zm.obj
LINK
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYDIAG.lnk
cat > rewrite/.build/IPLAYCONT.lnk <<'LINK'
system dos
libpath /home/xor/watcom/lib286/dos
libpath /home/xor/watcom/lib286
option quiet
option map=rewrite/.build/IPLAYCONT.map
option eliminate
option stack=16384
name rewrite/.build/IPLAYCONT.EXE
file rewrite/.build/iplay_rewrite_zm.obj
file rewrite/.build/iplay_abi_watcom_zm.obj
file rewrite/.build/iplay_player_contdiag_zm.obj
LINK
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYCONT.lnk
cat > rewrite/.build/IPLAYHW.lnk <<'LINK'
system dos
libpath /home/xor/watcom/lib286/dos
libpath /home/xor/watcom/lib286
option quiet
option map=rewrite/.build/IPLAYHW.map
option eliminate
option stack=16384
name rewrite/.build/IPLAYHW.EXE
file rewrite/.build/iplay_rewrite_zm.obj
file rewrite/.build/iplay_abi_watcom_zm.obj
file rewrite/.build/iplay_player_hwdiag_zm.obj
LINK
WATCOM=/home/xor/watcom \
INCLUDE=/home/xor/watcom/h \
PATH=/home/xor/watcom/binl64:$PATH \
/home/xor/watcom/binl64/wlink @rewrite/.build/IPLAYHW.lnk
cat > rewrite/.build/iplay_rewrite_dos_runner <<'RUNNER'
#!/bin/sh
set -eu
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
KVIKDOS_TIMEOUT=${KVIKDOS_TIMEOUT:-timeout}
KVIKDOS_SECONDS=${KVIKDOS_SECONDS:-3}
KVIKDOS=/home/xor/kvikdos/kvikdos
if [ "$#" -eq 1 ] && [ "$1" = "playersb16hwblock" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playersb16hwtwoblocks" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playersb16hwdma6" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playersb16hwdma7" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playersb16hwbase240" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playerplaybacktimerhw" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playercontinuousloophw" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playerkeyboardhw" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playerkeyboardstophw" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playerplaybacklevelshw" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playermodulekeyboardstophw" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playertexthwpresent" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "plhw25" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "plhw40" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "plhw8b" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "plhw50" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playerruntimehw80x50" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "playerruntimehw80x50levels" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IPHWRUN.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "mod102f5" ] && [ ${#2} -gt 120 ]; then
  printf '%s' "$2" > "$DIR/ARG.TXT"
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IRUN.EXE "$1" "@ARG.TXT"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abimod102f5" ] && [ ${#2} -gt 120 ]; then
  printf '%s' "$2" > "$DIR/ARG.TXT"
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MOD102F5.EXE "$1" "@ARG.TXT"
fi
if [ "$#" -eq 5 ] && [ "$1" = "mod1021e" ]; then
  printf '%s' "$4" > "$DIR/ARG1.TXT"
  printf '%s' "$5" > "$DIR/ARG2.TXT"
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IRUN.EXE "$1" "$2" "$3" "@ARG1.TXT" "@ARG2.TXT"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abimod1021e" ]; then
  printf '%s' "$4" > "$DIR/ARG1.TXT"
  printf '%s' "$5" > "$DIR/ARG2.TXT"
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MOD1021E.EXE "$1" "$2" "$3" "@ARG1.TXT" "@ARG2.TXT"
fi
if [ "$#" -eq 3 ] && [ "$1" = "mod1024a" ] && [ ${#3} -gt 100 ]; then
  printf '%s' "$3" > "$DIR/ARG.TXT"
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IRUN.EXE "$1" "$2" "@ARG.TXT"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abimod1024a" ] && [ ${#3} -gt 100 ]; then
  printf '%s' "$3" > "$DIR/ARG.TXT"
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MOD1024A.EXE "$1" "$2" "@ARG.TXT"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abinoop" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_NULLSUB3.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abiuseless12d61" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_USELESS12D61.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abiuselesswriteinrfail" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_USELESSWRITEINR.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abiuselesswriteinr118" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_USELESSWRITEINR118.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abimemfill8080" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MEMFILL8080.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abisndofffill" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SNDOFFFILL.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abiaudioinitfail" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_AUDIOINITFAIL.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abisbinitnodevice" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SBINITNODEVICE.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abisbtestinterruptnodevice" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SBTESTINT.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abisbonbounded" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SBONBOUNDED.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abisbhandlerintbounded" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SBHANDLERINT.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abisndonparntbounded" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SNDONPARNT.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abitimerbounded" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_TIMERBOUNDED.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abisndguard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SNDGUARD.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abitimerint" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_TIMERINT.EXE "$@"
fi
if [ "$#" -eq 12 ] && [ "$1" = "abireadsndsettings" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_READSNDSETTINGS.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abifilldmainactivemono" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_FILLDMAINACTIVE.EXE "$@"
fi
if [ "$#" -eq 7 ] && [ "$1" = "abifilelist" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_FILELIST.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abifindmodsguard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_FINDMODS.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abisub126a9" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB126A9.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abisub13623guard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB13623.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abimod102f5" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MOD102F5.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abisub12cadguard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB12CAD.EXE "$@"
fi
if { [ "$#" -eq 1 ] || [ "$#" -eq 2 ]; } && [ "$1" = "abisub1281asmallmix" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB1281A.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abimod1021e" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MOD1021E.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abimod1024a" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MOD1024A.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abimemfree18a28" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MEMFREE18A28.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abisub11c0c" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB11C0C.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abisub1415e" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB1415E.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abisub12f56" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB12F56.EXE "$@"
fi
if [ "$#" -eq 8 ] && [ "$1" = "abisub154f4" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB154F4.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abisub135ca" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB135CA.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abisub13813" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB13813.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abisub140b6guard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB140B6.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abisub137d5guard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB137D5.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abisub13429guard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB13429.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abisub131da" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB131DA.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abisub131ef" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB131EF.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abisub13177" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB13177.EXE "$@"
fi
if [ "$#" -eq 3 ] && { [ "$1" = "abieff13bc0" ] || [ "$1" = "abieff13c34" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFFNIBBLE.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abieff13a43" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13A43.EXE "$@"
fi
if [ "$#" -eq 3 ] && { [ "$1" = "abieff13bb2" ] || [ "$1" = "abieff13ba3" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13BB2.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abieff13ad7" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13AD7.EXE "$@"
fi
if { [ "$#" -eq 3 ] && [ "$1" = "abieff13b78" ]; } || { [ "$#" -eq 4 ] && [ "$1" = "abieff13b88" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13B78.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abieff13b06" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13B06.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abieff13a94" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13A94.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abieff13bc8" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13BC8.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abieff13c02" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13C02.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abieff13c3f" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13C3F.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abieff13c64" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13C64.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abieff13c88" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13C88.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abieff13c95" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13C95.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abieff13ca2" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13CA2.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abieff13cb3" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13CB3.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abieff13cc9" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13CC9.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abieff13cdd" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13CDD.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abieff13ce8" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13CE8.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abieff13de" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13DE.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abisub14087" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13DE.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abisub13044" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB13044.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abieff13e1e" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13E1E.EXE "$@"
fi
if [ "$#" -eq 7 ] && [ "$1" = "abieff13e2d" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13E2D.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abieff13e32" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13E32.EXE "$@"
fi
if [ "$#" -eq 10 ] && [ "$1" = "abieff13e7f" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13E7F.EXE "$@"
fi
if [ "$#" -eq 11 ] && [ "$1" = "abieff13e84" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13E84.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abieff13e8c" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13E8C.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abieff13f05" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13F05.EXE "$@"
fi
if [ "$#" -eq 7 ] && [ "$1" = "abieff13f3b" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13F3B.EXE "$@"
fi
if [ "$#" -eq 7 ] && [ "$1" = "abieff13fbe" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13FBE.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abiamplif" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_AMPLIF.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abicalc14043" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_CALC14043.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abieff14030" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF14030.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abieff14067" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF14067.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abisub13d95" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB13D95.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abisub13e9b" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB13E9B.EXE "$@"
fi
if [ "$#" -eq 2 ] && { [ "$1" = "abimidi154da" ] || [ "$1" = "abimidi154de" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MIDI154.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abimidi154ac" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MIDI154AC.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abimidi15413guard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_MIDI15413.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abisub15577guard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB15577.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abisub1609fdisabled" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB1609F.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abisetvideomode" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETVIDEOMODE.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abisub1ab8c" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB1AB8C.EXE "$@"
fi
if [ "$#" -ge 2 ] && [ "$1" = "abitxt1abae" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_TXT1ABAE.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abisub13826" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB13826.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abigetsetplaystate" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abiget12f7c" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abisub12d05" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 6 ] && [ "$1" = "abisomeplaymode" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abisub12b83" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abisub12b18" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 5 ] && [ "$1" = "abisub12afd" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 6 ] && [ "$1" = "abisetplaysettings" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abimemclean" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abichangevolume" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abigetplaysettings" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abivolume12a66" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 1 ] && [ "$1" = "abivlm141df" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SETTINGS.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abispectr1bce9equal" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SPECTR.EXE "$@"
fi

if [ "$#" -eq 1 ] && [ "$1" = "abispectr1bc2dequal" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SPECTR.EXE "$@"
fi

if [ "$#" -eq 1 ] && [ "$1" = "abispectr1bbc1zero" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SPECTR.EXE "$@"
fi

if [ "$#" -eq 2 ] && { [ "$1" = "abispectrsqrt" ] || [ "$1" = "abispectr1b406small" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SPECTR.EXE "$@"
fi

if { [ "$#" -eq 2 ] || [ "$#" -eq 3 ]; } && [ "$1" = "abistrcpy" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_STRING.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abicopyprint" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_STRING.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abiputmessage" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_TEXTMSG.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abitext1bf69" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_TEXTMSG.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abiwritescr" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_TEXTMSG.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abitextsetup" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_VISUAL.EXE "$@"
fi

if [ "$#" -eq 8 ] && [ "$1" = "abidrawframe" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_VISUAL.EXE "$@"
fi

if [ "$#" -eq 1 ] && [ "$1" = "abitxtdrawtoptitle" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_VISUAL.EXE "$@"
fi

if [ "$#" -eq 9 ] && [ "$1" = "abitxtdrawbottom" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_VISUAL.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abimessage1be77" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_VISUAL.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abirecolortxt" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_UIEXTRA.EXE "$@"
fi

if [ "$#" -eq 1 ] && [ "$1" = "abimousegetpos" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_UIEXTRA.EXE "$@"
fi

if [ "$#" -eq 2 ] && { [ "$1" = "abimousecursor" ] || [ "$1" = "abimousewrapper" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_UIEXTRA.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abimousedeinit" ] || [ "$1" = "abimouseinit" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_UIEXTRA.EXE "$@"
fi

if [ "$#" -eq 7 ] && [ "$1" = "abimouse1c7a9" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_UIEXTRA.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abimouse1c7cf" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_UIEXTRA.EXE "$@"
fi

if [ "$#" -eq 2 ] && { [ "$1" = "abiint24" ] || [ "$1" = "abiemsinit" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 3 ] && { [ "$1" = "abiemsrestore" ] || [ "$1" = "abiemsrealloc2limit" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abiemsguard" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abiemsmapcopy" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abimyasmsprintf" ] || [ "$1" = "abigetcomspec" ] || [ "$1" = "abigetexename" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_STRING.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abivideoprp" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_VIDEO.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abisub12d35disable" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB12D35.EXE "$@"
fi

if { [ "$#" -eq 4 ] || [ "$#" -eq 5 ]; } && [ "$1" = "abieffslide" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFFSLIDE.EXE "$@"
fi

if [ "$#" -eq 6 ] && [ "$1" = "abieff138d2" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFFSLIDE.EXE "$@"
fi

if [ "$#" -eq 7 ] && [ "$1" = "abieff1392f" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFFSLIDE.EXE "$@"
fi

if [ "$#" -eq 8 ] && [ "$1" = "abieff139ac" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFFSLIDE.EXE "$@"
fi

if [ "$#" -eq 9 ] && [ "$1" = "abieff139b2" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFFSLIDE.EXE "$@"
fi

if [ "$#" -eq 7 ] && [ "$1" = "abieff139b9" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFFSLIDE.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abisub13cf6" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_EFF13E8C.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abistrlen" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_STRLEN.EXE "$@"
fi

if [ "$#" -eq 4 ] && [ "$1" = "abifill" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_DECIMAL.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abimyu32toa" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_DECIMAL.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abimyu32toa0" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_DECIMAL.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abimyutoa10" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_DECIMAL.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abimyitoa10" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_DECIMAL.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abidecimal16" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_DECIMAL.EXE "$@"
fi

if [ "$#" -eq 2 ] && { [ "$1" = "abihex4" ] || [ "$1" = "abihex8" ] || [ "$1" = "abihex16" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_HEX.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abimyputdigit" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_HEX.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abimyhex" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_HEX.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abihex1be39" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_HEX.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abivolumeprepinactive" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_VOLUMEPREP.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abisub19050bounded" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SUB19050.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abiclean11c43" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 5 ] && [ "$1" = "abimodsubdelta" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 6 ] && [ "$1" = "abisub11ba6" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abitxtblink" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abitimerport" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abiintvect" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abirtcclock" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abihex32" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_HEX.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abiputdigit" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_HEX.EXE "$@"
fi
if [ "$#" -eq 2 ] && { [ "$1" = "abiu32toa10" ] || [ "$1" = "abii32toa10" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_DECIMAL.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abiuseless11787zero" ] || [ "$1" = "abiuselessdoswrite2" ] || [ "$1" = "abiuselessdoswrite" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 2 ] && { [ "$1" = "abiuselessunsetegaseq" ] || [ "$1" = "abiult1150b" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abireadsb" ] || [ "$1" = "abireadmixersb" ] || [ "$1" = "abiwritesb" ] || [ "$1" = "abiwritemixersb" ] || [ "$1" = "abichecksb" ] || [ "$1" = "abimidiset" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abimidiport" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abimemfree125da" ] || [ "$1" = "abimemalloc12kbounded" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abimemfree" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abimemreallocx" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 3 ] && { [ "$1" = "abimemlimit" ] || [ "$1" = "abimemstrat" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abiallocdmafail" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 1 ] && [ "$1" = "abicallsubxfail" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 2 ] && { [ "$1" = "abisetdmamask" ] || [ "$1" = "abisettimerint" ] || [ "$1" = "abigravisdma" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 1 ] && { [ "$1" = "abisbdetectirq" ] || [ "$1" = "abisub1279dma" ] || [ "$1" = "abiprogramdma" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abiadlib18389" ] || [ "$1" = "abiadlib18395" ] || [ "$1" = "abisetegasequencer" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abicleandeinit" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 1 ] && { [ "$1" = "abideinit125b9idle" ] || [ "$1" = "abiloadcfgsuccess" ] || [ "$1" = "abidosexecnocomspec" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 2 ] && [ "$1" = "abidosdir" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 1 ] && { [ "$1" = "abidosfindnext" ] || [ "$1" = "abidosfread" ] || [ "$1" = "abidosseeksuccess" ] || [ "$1" = "abiread2buffer" ] || [ "$1" = "abireadallmoulesbounded" ] || [ "$1" = "abireadmodulefail" ] || [ "$1" = "abimodulereadfail" ] || [ "$1" = "abimodulessearchbounded" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abiinitvgabounded" ] || [ "$1" = "abigraph1c070" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 2 ] && { [ "$1" = "abigraphsetup" ] || [ "$1" = "abif2drawbounded" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abiinrread119b7" ] || [ "$1" = "abiinrread118b0fail" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abimidichannelport" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 4 ] && [ "$1" = "abisndvector" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abisub12da8guard" ] || [ "$1" = "abistartbounded" ] || [ "$1" = "abikeybbounded" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 2 ] && [ "$1" = "abisub197f2" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 3 ] && [ "$1" = "abikeybsw" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "abimodread11f4eguard" ] || [ "$1" = "abimodread12247eof" ] || [ "$1" = "abimodread10311bounded" ] || [ "$1" = "abimodntbounded" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 2 ] && { [ "$1" = "abiformatloaderheader" ] || [ "$1" = "abiultreadfast" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "textcelldigest" ] || [ "$1" = "runtimetextdigest" ] || [ "$1" = "runtimepresentdigest" ] || [ "$1" = "textscreenresizebad" ] || [ "$1" = "textscreenresizecapacity" ] || [ "$1" = "textscreenresize80x25" ] || [ "$1" = "textscreenresize80x50" ] || [ "$1" = "textscreenresizecycle" ] || [ "$1" = "textsubplaneclip" ] || [ "$1" = "textsubplanezeroedge" ] || [ "$1" = "textsubplane80x50zeroedge" ] || [ "$1" = "textpresent80x50" ] || [ "$1" = "textpresent80x25bw" ] || [ "$1" = "textresize80x25present" ] || [ "$1" = "textmodecyclepresent" ] || [ "$1" = "textpresentclear" ] || [ "$1" = "textpresentreplace" ] || [ "$1" = "textbadmodepresent" ] || [ "$1" = "textresizecapacitypresent" ] || [ "$1" = "terminalresizecapacitypresent" ] || [ "$1" = "terminalresize80x25present" ] || [ "$1" = "terminalpresent80x25bw" ] || [ "$1" = "terminalpresent80x25color" ] || [ "$1" = "terminalbadmodepresent" ] || [ "$1" = "terminalpresentclear" ] || [ "$1" = "terminalpresentreplace" ] || [ "$1" = "terminalresize80x50present" ] || [ "$1" = "terminalresizecyclepresent" ] || [ "$1" = "textsubwindowpresent" ] || [ "$1" = "textsubwindowredraw" ] || [ "$1" = "textsubwindowzeroedge" ] || [ "$1" = "textsubwindow80x50clip" ] || [ "$1" = "textsubwindow80x50zeroedge" ] || [ "$1" = "textcursorresize" ] || [ "$1" = "textcursorresizezero" ] || [ "$1" = "textcursor80x50resizezero" ] || [ "$1" = "textcolorattrs16" ] || [ "$1" = "runtimepresentclear" ] || [ "$1" = "runtimebadmodepresent" ] || [ "$1" = "runtimeresizebadpresent" ] || [ "$1" = "runtimeresizecapacitypresent" ] || [ "$1" = "runtimepresent80x25bw" ] || [ "$1" = "runtimepresent80x25color" ] || [ "$1" = "runtimeresize80x50present" ] || [ "$1" = "runtimeresize80x25present" ] || [ "$1" = "runtimeresizecyclepresent" ] || [ "$1" = "runtimecursor80x50resizezero" ] || [ "$1" = "runtimesubwindow80x50clip" ] || [ "$1" = "runtimesubwindowresizecycleclip" ] || [ "$1" = "runtimesubwindowzeroedge" ] || [ "$1" = "runtimesubwindow80x50zeroedge" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IUIRUN.EXE "$@"
fi

if [ "$#" -eq 1 ] && { [ "$1" = "sdlaudioinitformat" ] || [ "$1" = "sdlaudiowriteaccepted" ] || [ "$1" = "sdlaudiowritenull" ] || [ "$1" = "sdlaudiowritesignedlevels" ] || [ "$1" = "sdlaudiocallbackaccepted" ] || [ "$1" = "sdlaudiocallbackpartial" ] || [ "$1" = "sdlaudiocallbacksignedlevels" ] || [ "$1" = "sdlaudiocallbackpaused" ] || [ "$1" = "sdlaudiocallbacknull" ] || [ "$1" = "sdlaudiocallbacknullstream" ] || [ "$1" = "sdlaudioopenrejectnonsb16" ] || [ "$1" = "sdlaudioopenpreservesactive" ] || [ "$1" = "runtimeinitformat" ] || [ "$1" = "runtimeinitcounters" ] || [ "$1" = "runtimeaudiostartclean" ] || [ "$1" = "runtimeaudiostopclean" ] || [ "$1" = "runtimeaudiopauseclean" ] || [ "$1" = "runtimeaudiopauseresumeclean" ] || [ "$1" = "runtimeaudiocapacityclean" ] || [ "$1" = "runtimeaudioaddcapacityclean" ] || [ "$1" = "runtimeaudioclearqueuedclean" ] || [ "$1" = "runtimeaudioresetcountersclean" ] || [ "$1" = "runtimewriteaccepted" ] || [ "$1" = "runtimewritenull" ] || [ "$1" = "runtimeaudioopenpreservesactive" ] || [ "$1" = "runtimesignedlevels" ] || [ "$1" = "runtimequeuepartial" ] || [ "$1" = "runtimequeuesignedlevels" ] || [ "$1" = "runtimequeuepaused" ] || [ "$1" = "runtimepausepreserveslevels" ] || [ "$1" = "runtimepauseresumelevels" ] || [ "$1" = "runtimequeuestopstart" ] || [ "$1" = "runtimestoppreserveslevels" ] || [ "$1" = "runtimestopstartlevels" ] || [ "$1" = "runtimeclearqueuedlevels" ] || [ "$1" = "runtimeresetcounterslevels" ] || [ "$1" = "runtimeresetunderrunlevels" ] || [ "$1" = "runtimelevelsdisplay80x50" ] || [ "$1" = "runtimelevelsreset80x50" ] || [ "$1" = "runtimehwinitformat" ] || [ "$1" = "runtimehwstatustext" ] || [ "$1" = "runtimehwinitcounters" ] || [ "$1" = "runtimehwaudiostartclean" ] || [ "$1" = "runtimehwaudiostopclean" ] || [ "$1" = "runtimehwaudiopauseclean" ] || [ "$1" = "runtimehwaudiopauseresumeclean" ] || [ "$1" = "runtimehwaudiocapacityclean" ] || [ "$1" = "runtimehwaudioaddcapacityclean" ] || [ "$1" = "runtimehwaudioclearqueuedclean" ] || [ "$1" = "runtimehwaudioresetcountersclean" ] || [ "$1" = "runtimehwwriteaccepted" ] || [ "$1" = "runtimehwwritenull" ] || [ "$1" = "runtimehwaudioopenpreservesactive" ] || [ "$1" = "runtimehwqueuepartial" ] || [ "$1" = "runtimehwqueuesignedlevels" ] || [ "$1" = "runtimehwqueuepaused" ] || [ "$1" = "runtimehwwritepaused" ] || [ "$1" = "runtimehwpausepreserveslevels" ] || [ "$1" = "runtimehwpauseresumelevels" ] || [ "$1" = "runtimehwwritestopped" ] || [ "$1" = "runtimehwstoppreserveslevels" ] || [ "$1" = "runtimehwstopstartlevels" ] || [ "$1" = "runtimehwwritepauseresume" ] || [ "$1" = "runtimehwqueuestopstart" ] || [ "$1" = "runtimehwshutdown" ] || [ "$1" = "runtimehwlevelsdisplay" ] || [ "$1" = "runtimehwlevelsdisplay80x50" ] || [ "$1" = "runtimehwresetlevels" ] || [ "$1" = "runtimehwlevelsreset80x50" ] || [ "$1" = "runtimehwsignedlevels" ] || [ "$1" = "runtimehwwritesilence" ] || [ "$1" = "runtimehwstoppedsilence" ] || [ "$1" = "runtimehwpausedsilence" ] || [ "$1" = "runtimehwcapacityrefill" ] || [ "$1" = "runtimehwclearqueued" ] || [ "$1" = "runtimehwclearqueuedlevels" ] || [ "$1" = "runtimehwresetcounters" ] || [ "$1" = "runtimehwresetcounterslevels" ] || [ "$1" = "runtimehwresetunderrun" ] || [ "$1" = "runtimehwresetunderrunlevels" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IARUN.EXE "$@"
fi

if [ "$#" -eq 3 ] && [ "$1" = "abidmafillbuf" ]; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 2 ] && { [ "$1" = "abisb16probe" ] || [ "$1" = "abisb16off" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi
if [ "$#" -eq 1 ] && { [ "$1" = "abisb16initfail" ] || [ "$1" = "abisb16int" ] || [ "$1" = "abisb16dmafail" ]; }; then
  cd "$DIR"
  exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" IABI_SYSTEM.EXE "$@"
fi

exec "$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" "$DIR/IRUN.EXE" "$@"
RUNNER
chmod +x rewrite/.build/iplay_rewrite_dos_runner
