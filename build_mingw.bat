set OPT=-O0  -Wno-multichar   -I\MinGW\include\ -lmingw32 -lSDL2main -lSDL2 -ggdb3 
rem -DDEBUG=1
"C:\MinGW\bin\g++.exe" -mno-ms-bitfields asm.cpp  -c %OPT%
"C:\MinGW\bin\g++.exe" -mno-ms-bitfields memmgr.cpp -c %OPT%
"C:\MinGW\bin\g++.exe" -mno-ms-bitfields %1.cpp -c %OPT%
"C:\MinGW\bin\g++.exe" -mno-ms-bitfields %1.o asm.o memmgr.o -o %1 %OPT% pdcurses.lib
rem  -lgdi32 -lcomdlg32