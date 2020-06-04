echo on
set OPT=-mno-ms-bitfields -Wno-multichar -I/MinGW/include/ -L/MinGW/lib -I. -L. C:\MinGW\lib\pdcurses.a  -lmingw32 -mconsole -DSDL_MAIN_HANDLED -ggdb3
rem -DDEBUG=3
set CC="C:\MinGW\bin\gcc.exe"
%CC% %OPT% asm.cpp -c
%CC% %OPT% iprintf.cpp -c
%CC% %OPT% memmgr.cpp -c
%CC% %OPT% -Os test-i386.cpp -c
%CC%  test-i386.o iprintf.o asm.o memmgr.o -o test-i386 %OPT%
rem pdcurses.lib
rem  -lgdi32 -lcomdlg32