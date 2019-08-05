echo on
set OPT=-mno-ms-bitfields -Wno-multichar -I/MinGW/include/ -L/MinGW/lib -I. -L. C:\MinGW\lib\pdcurses.a  -lmingw32 -lSDLmain -lSDL 
rem -ggdb3
rem -DDEBUG=3
set CC="C:\MinGW\bin\gcc.exe"
%CC% %OPT% asm.cpp -c
%CC% %OPT% memmgr.c -c
%CC% %OPT% %1.cpp -c
%CC%  %1.o asm.o memmgr.o -o %1 %OPT%
rem pdcurses.lib
rem  -lgdi32 -lcomdlg32