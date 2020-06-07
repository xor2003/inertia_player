set OPT= -mno-ms-bitfields  -Wno-multichar  -lSDLmain -lSDL -ggdb3 -O2 -I. -IC:\cygwin\usr\include\ncurses -DCHTYPE_16
set CC=C:\cygwin\bin\g++.exe
rem -DDEBUG=1

%CC% asm.cpp  -c %OPT%
%CC% memmgr.cpp -c %OPT%
rem %CC% %1.cpp  %OPT% -Og -S -masm=intel -fverbose-asm > %1_.s
%CC% %1.cpp -c %OPT%
rem iprintf.o 
%CC% iprintf.o %1.o asm.o memmgr.o -o %1 -lncurses %OPT% 
rem pdcurses.lib
