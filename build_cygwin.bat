set OPT= -mno-ms-bitfields  -Wno-multichar  -lSDL2main -lSDL2 -ggdb3 -O2
set CC="C:\cygwin\bin\gcc.exe"
rem -DDEBUG=1

%CC% asm.c  -c %OPT%
%CC% memmgr.c -c %OPT%
rem %CC% %1.cpp  %OPT% -Og -S -masm=intel -fverbose-asm > %1_.s
%CC% %1.c -c %OPT%
%CC% %1.o asm.o memmgr.o -o %1 %OPT% pdcurses.a 
rem pdcurses.lib
