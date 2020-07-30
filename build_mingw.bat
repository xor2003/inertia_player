echo on
set OPT=-O3 -m32 -mno-ms-bitfields -Wno-multichar -Wno-address-of-packed-member -I/msys64/mingw32/include/ncurses -L/msys64/mingw32/lib -I. -L. -lmingw32 -mconsole   -ggdb3 -lSDL2 -lSDL2main 
rem -DDEBUG=3
rem set OPT=-Og -mno-ms-bitfields -Wno-multichar -Wno-address-of-packed-member -I/msys64/mingw32/include/pdcurses -L/msys64/mingw32/lib -I. -L. -lmingw32 -mconsole   -ggdb3 -lSDL -lSDLmain
rem  -mconsole -DSDL_MAIN_HANDLED
rem   -DDEBUG=3
set CC="g++"
%CC% %OPT% asm.cpp -c
rem %CC% %OPT% iprintf.cpp -c
rem %CC% %OPT% iprintf.cpp -S -masm=intel
%CC% %OPT% memmgr.cpp -c
%CC% %OPT% %1.cpp -c
rem %CC%  %1.o asm.o memmgr.o -lpdcurses.dll -o %1 %OPT%
%CC%  %1.o asm.o memmgr.o /MinGW/lib/pdcurses.a -o %1 %OPT%
rem iprintf.o 
