set OPT=-O0  -Wno-multichar   -I\MinGW\include\ -I. -lmingw32 -lSDL2main -lSDL2 -ggdb3 
set CC="C:\MinGW\bin\g++.exe"
rem -DDEBUG=1
%CC% -mno-ms-bitfields asm.c  -c %OPT%
%CC% -mno-ms-bitfields memmgr.c -c %OPT%
%CC% -mno-ms-bitfields %1.c -c %OPT%
%CC% -mno-ms-bitfields %1.o asm.o memmgr.o -o %1 %OPT% pdcurses.lib
rem  -lgdi32 -lcomdlg32