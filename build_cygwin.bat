set OPT= -mno-ms-bitfields  -Wno-multichar  -lSDLmain -lSDL -ggdb3 -O2 -I.
set CC=C:\cygwin\bin\gcc.exe
rem -DDEBUG=1

%CC% asm.c  -c %OPT%
%CC% memmgr.c -c %OPT%
rem %CC% %1.cpp  %OPT% -Og -S -masm=intel -fverbose-asm > %1_.s
%CC% %1.c -c %OPT%
%CC% %1.o asm.o memmgr.o -o %1 pdcurses.a %OPT% 
rem pdcurses.lib
