echo on
set OPT=-Og -mno-ms-bitfields -Wno-multichar -I/MinGW/include/ -L/MinGW/lib -I. -L. C:\MinGW\lib\pdcurses.a  -ggdb3 -lSDL -lSDLmain
rem  -mconsole -DSDL_MAIN_HANDLED
rem   -DDEBUG=3
set CC="C:\MinGW\bin\g++.exe"
%CC% %OPT% asm.cpp -c
rem %CC% %OPT% iprintf.cpp -c
rem %CC% %OPT% iprintf.cpp -S -masm=intel
%CC% %OPT% memmgr.cpp -c
%CC% %OPT% %1.cpp -c
%CC%  %1.o asm.o memmgr.o -o %1 %OPT%
rem iprintf.o 
