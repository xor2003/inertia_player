set CC=C:\inertia_player\BC5\BIN\BCC.EXE 
set OPT=-1- -Os -G -d -w-par -c -ml -M -IC:\inertia_player\BC5\INCLUDE -LC:\inertia_player\BC5\LIB -DDEBUG=3
%CC% %OPT% asm.cpp
%CC% %OPT% memmgr.cpp
%CC% %OPT% %1.cpp
C:\inertia_player\BC5\BIN\ulink.exe -Tde c0l.obj proc.obj asm.obj memmgr.obj,proc.exe,,gen.lib pdcurses.lib mathl.lib cl.lib