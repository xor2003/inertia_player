#!/bin/sh
export OPT="-O0  -mno-ms-bitfields -Wno-multichar   -I/MinGW/include/ -lmingw32 -lSDL2main -lSDL2 -ggdb3"
export CXX="/cygdrive/c/MinGW/bin/g++"
# -DDEBUG=1
$CXX asm.cpp  -c $OPT
$CXX memmgr.cpp -c $OPT
$CXX $1.cpp -c $OPT
$CXX $1.o asm.o memmgr.o -o $1 $OPT pdcurses.lib

