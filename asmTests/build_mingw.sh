#!/bin/sh
export OPT="-O0  -mno-ms-bitfields -Wno-multichar  -lSDL2main -lSDL2 -ggdb3 -DDEBUG=3"
export CXX="g++"
# -DDEBUG=1
$CXX asm.cpp  -c $OPT
$CXX memmgr.cpp -c $OPT
$CXX $1.cpp -c $OPT
$CXX $1.o asm.o memmgr.o -o $1 $OPT pdcurses.lib

