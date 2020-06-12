#!/bin/sh
export OPT="-m32 -mno-ms-bitfields -Wno-multichar -I/mingw32/include/pdcurses -L/mingw32/lib -I. -L. -lmingw32 -mconsole -DSDL_MAIN_HANDLED -ggdb3  -DDEBUG=3"
# -DDEBUG=3
export CC="gcc"
$CC $OPT asm.cpp -c
$CC $OPT iprintf.cpp -c
$CC $OPT memmgr.cpp -c
$CC $OPT -Os test-i386.cpp -c
$CC  test-i386.o iprintf.o asm.o memmgr.o /mingw32/lib/libpdcurses.dll.a -o test-i386 $OPT
# pdcurses.lib
#  -lgdi32 -lcomdlg32