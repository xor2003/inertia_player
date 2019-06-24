set OPT=-O0  -Wno-multichar   -I\MinGW\include\ -lmingw32 -lSDL2main -lSDL2 -ggdb3 
rem -DDEBUG=3
"C:\MinGW\bin\g++.exe" -mno-ms-bitfields asm.c  -c %OPT%
"C:\MinGW\bin\g++.exe" -mno-ms-bitfields memmgr.c -c %OPT%
"C:\MinGW\bin\g++.exe" -mno-ms-bitfields iplay_masm_.cpp -c %OPT%
"C:\MinGW\bin\g++.exe" -mno-ms-bitfields iplay_masm_.o asm.o memmgr.o pdcurses.lib -o iplay_masm_ %OPT%
rem  -lgdi32 -lcomdlg32