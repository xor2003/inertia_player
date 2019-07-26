echo on
set OPT=-mno-ms-bitfields -Wno-multichar -I/MinGW/include/ -L/MinGW/lib -I. -L. pdcurses.a -lmingw32 -lSDL2main -lSDL2 -ggdb
rem   -DDEBUG=3
set CC="C:\MinGW\bin\gcc.exe"
%CC% %OPT% asm.c -c
%CC% %OPT% memmgr.c -c
%CC% %OPT% %1.c -c
%CC%  %1.o asm.o memmgr.o -o %1 %OPT%
rem pdcurses.lib
rem  -lgdi32 -lcomdlg32