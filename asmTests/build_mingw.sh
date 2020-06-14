#!/bin/sh -e
#export OPT="-mno-ms-bitfields -Wno-multichar -I/MinGW/include/ -L/MinGW/lib -I../ -L../ ../pdcurses.a -lmingw32 -lSDL2main -lSDL2 -ggdb"
export OPT="-Og -m32 -mno-ms-bitfields -Wno-multichar -Wno-narrowing -I../ -L../ -I/mingw32/include/pdcurses  -L/mingw32/lib -I. -L.  -ggdb3 -lSDL -lSDLmain"
#  -mconsole -DSDL_MAIN_HANDLED
#   -DDEBUG=3
#export CC="/Mingw/bin/g++"
export CC="g++"

#cd ..
#$CC $OPT asm.cpp -c
#$CC $OPT memmgr.cpp -c
#cd asmTests
     
#$CC $OPT $1.cpp -E >$1.e
$CC $OPT $1.cpp -c
$CC  $1.o ../asm.o ../memmgr.o /mingw32/lib/libpdcurses.dll.a -o $1 $OPT


