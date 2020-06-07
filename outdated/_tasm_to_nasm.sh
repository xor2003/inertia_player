#!/bin/sh +x
cp iplay_tasm.asm iplay_tasm_.asm
sed -ie 's!far_module!faar_module!g' iplay_tasm_.asm
perl nomyso.pl  iplay_tasm_.asm  iplay_tasmn.asm 

grep far iplay_tasm.asm  |perl -pe 's![ \t]+! !g' |cut -f2 -d' ' | grep -wFf - iplay_tasm.asm | grep 'call\|jmp' | grep -v 'near\|callsubx' \
| perl -pe 's!(call|jmp)(\s+)([A-Za-z_0-9\[\]:]+)!s/$1$2\\<$3\\>/$1 far $3/!;  s!\[!\\[!g; s!\]!\\]!g' | grep -v ';' | sort | uniq > _temp.sed

echo 	 > iplay_nasm.asm

cat iplay_tasmn.asm | grep -vw "model large" | \
perl -pe "s/\r//g; s!jcxz\s+short\s+!jcxz !; s!jmp	short loc_11B28!jmp	loc_11B28!; s!\['\+:A\+'\]!']A:['!" | sed -f _temp.sed \
>> iplay_nasm.asm

cp iplay_nasm.asm iplay_nasm_or.asm
patch -p0 iplay_nasm.asm iplay_nasm_override.patch 

#/cygdrive/c/Program\ Files/DOSBox-0.74/DOSBox.exe  V.BAT
./build_nasm.bat

#signatures (character constants) grep -P "'[A-Za-z0-9 \.\[\]\\\/][A-Za-z0-9 \.\[\]\\\/]+'" iplay_tasm.asm | grep -Pv "db|segment|'\s+'" > _signatures.txt

#segs override
#grep -n "lods\|stos\|movs" iplay_tasm.asm  | grep -P "[a-z]:[a-z]"