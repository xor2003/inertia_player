#!/bin/sh -ex
#export OPT="-mno-ms-bitfields -Wno-multichar -I/MinGW/include/ -L/MinGW/lib -I../ -L../ ../pdcurses.a -lmingw32 -lSDL2main -lSDL2 -ggdb"
#export OPT="-Og -m32 -mno-ms-bitfields -Wno-multichar -Wno-narrowing -I../ -L../ -I/mingw32/include/pdcurses  -L/mingw32/lib -I. -L.  -ggdb3 -lSDL -lSDLmain"
#export OPT="-Og -mno-ms-bitfields -Wno-multichar -Wno-address-of-packed-member   -I. -I.. -L. -lmingw32 -mconsole   -ggdb3 -lSDL -lSDLmain -DDEBUG=3"

#export PKG_CONFIG_PATH=/c/msys64/mingw32/lib/pkgconfig
export CXX="g++ -m32"
#SDL=$(pkg-config --cflags --libs sdl)
SDL="-D_GNU_SOURCE=1 -Dmain=SDL_main -I/mingw/include/SDL -L/mingw/lib -lmingw32 -lSDLmain -lSDL -lmingw32 -lSDLmain -lSDL -lm -luser32 -lgdi32 -lwinmm -ldxguid"
CURSES="-I/mingw/include/pdcurses -L/mingw/lib "
export OPT="-Wno-narrowing -mno-ms-bitfields  -Wno-multichar $SDL $CURSES -D_GNU_SOURCE=1   -ggdb3 -O0 -I. -I.. -DDEBUG=3"
#-D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600 -DCHTYPE_16 
#echo $OPT

#pkg-config --cflags --libs ncurses
#  -mconsole -DSDL_MAIN_HANDLED
#   -DDEBUG=3
#export CC="/Mingw/bin/g++"

#cd ..
#$CC $OPT asm.cpp -c
#$CC $OPT memmgr.cpp -c
#cd asmTests
     
#$CC $OPT $1.cpp -E >$1.e
$CXX $OPT $1.cpp -c
$CXX  $1.o ../asm.o ../memmgr.o /MinGW/lib/pdcurses.a -o $1 $OPT


