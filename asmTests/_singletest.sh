#!/bin/sh -ex
rm $1.cpp $1.h $1.err $1.log $1.txt $1.asm.log $1.o $1 2>/dev/null || true
##./ml /c /Fl $1.asm
#uasm /c /Fl $1.asm
python ../masm2c.py $1.asm >$1.txt
./build.sh $1 2>&1
./$1 2>&1
#python ../_masm61.py  $1.asm

