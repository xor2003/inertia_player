#!/bin/sh
ls *.asm | \
while read name;
do 
  n=$(echo $name| perl -pe 's!\.asm!!');
(/cygdrive/c/Python27/python ../masm-recover $name >${name}.txt && ./build_mingw.sh ${n} && ./${n}.exe )|| echo "$n failed";
done