#!/bin/sh -e
rm $1.cpp $1.o $1.exe 2>/dev/null || true
python ../masm-recover $1.asm >$1.txt
./build_mingw.sh $1 2>&1
./$1.exe 2>&1

