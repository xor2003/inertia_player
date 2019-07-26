#!/bin/sh
rm $1.cpp $1.c $1.exe; python2 ../masm-recover $1.asm >$1.txt && ./build_mingw.sh $1 2>&1 && ./$1.exe 2>&1
