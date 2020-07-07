#!/bin/sh
export PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig
export CXX="g++ -m32"
export OPT="-mno-ms-bitfields  -Wno-multichar `pkg-config --cflags --libs sdl` `pkg-config --cflags --libs ncurses` -ggdb3 -O0 -I. -DCHTYPE_16 -DDEBUG=3"
# -DDEBUG=1

$CXX asm.cpp  -c $OPT
$CXX memmgr.cpp -c $OPT
# $CXX $1.cpp  $OPT -Og -S -masm=intel -fverbose-asm > $1_.s
#$CXX $1.cpp -E $OPT > $1.e
$CXX $1.cpp -c $OPT
# iprintf.o 
$CXX $1.o asm.o memmgr.o -o $1  $OPT 
# pdcurses.lib
