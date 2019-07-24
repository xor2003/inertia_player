set OPT=-target i686-pc-windows-gnu  -mno-ms-bitfields -O0  -Wno-multichar   -I\MinGW\include\ -lmingw32 -lSDL2main -lSDL2 -ggdb3  -Wno-shift-op-parentheses
set CC="C:\LLVM\bin\clang++.exe"
rem -DDEBUG=1
%CC% asm.cpp  -c %OPT%
%CC% memmgr.cpp -c %OPT%
%CC% %1.cpp -c %OPT%
%CC% %1.o asm.o memmgr.o -o %1 %OPT% pdcurses.lib
rem  -lgdi32 -lcomdlg32