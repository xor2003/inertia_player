#!/bin/sh
export OPT="-mno-ms-bitfields -Wno-multichar -I/MinGW/include/ -L/MinGW/lib -I../ -L../ ../pdcurses.a -lmingw32 -lSDL2main -lSDL2 -ggdb"
export CXX="/MinGW/bin/gcc"
# -DDEBUG=1
#$CXX asm.c  -c $OPT
#$CXX memmgr.c -c $OPT
$CXX $OPT $1.c -c
$CXX $1.o ../asm.o ../memmgr.o -o $1 $OPT 

