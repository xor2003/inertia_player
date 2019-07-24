set OPT= -mno-ms-bitfields  -Wno-multichar  -lSDL2main -lSDL2 -ggdb3 
set CC="C:\cygwin\bin\g++.exe"
rem -DDEBUG=1

rem %CC% asm.cpp  -c %OPT%
rem %CC% memmgr.cpp -c %OPT%
rem %CC% %1.cpp  %OPT% -Og -S -masm=intel -fverbose-asm > %1_.s
%CC% %1.cpp -c %OPT%
%CC% %1.o iprintf.o asm.o memmgr.o -o %1 %OPT% pdcurses.lib
