#!/bin/sh -e
#export OPT="-mno-ms-bitfields -Wno-multichar -I/MinGW/include/ -L/MinGW/lib -I../ -L../ ../pdcurses.a -lmingw32 -lSDL2main -lSDL2 -ggdb"
#export OPT="-Og -m32 -mno-ms-bitfields -Wno-multichar -Wno-narrowing -I../ -L../ -I/mingw32/include/pdcurses  -L/mingw32/lib -I. -L.  -ggdb3 -lSDL -lSDLmain"
#export OPT="-Og -mno-ms-bitfields -Wno-multichar -Wno-address-of-packed-member   -I. -I.. -L. -lmingw32 -mconsole   -ggdb3 -lSDL -lSDLmain -DDEBUG=3"
export PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig
export CXX="g++ -m32"
export OPT="-Wno-narrowing -mno-ms-bitfields  -Wno-multichar `pkg-config --cflags --libs sdl` `pkg-config --cflags --libs ncurses` -ggdb3 -O0 -I. -I.. -DCHTYPE_16 -DDEBUG=3"

#  -mconsole -DSDL_MAIN_HANDLED
#   -DDEBUG=3
#export CC="/Mingw/bin/g++"

#cd ..
#$CC $OPT asm.cpp -c
#$CC $OPT memmgr.cpp -c
#cd asmTests
     
#$CC $OPT $1.cpp -E >$1.e
$CXX $OPT $1.cpp -c
$CXX  $1.o ../asm.o ../memmgr.o  -o $1 $OPT


