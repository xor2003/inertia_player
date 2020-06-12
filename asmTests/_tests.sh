#!/bin/sh
> _result.log
ls *.asm | \
while read name;
do 
  n=$(echo $name| perl -pe 's!\.asm!!');
  (./_singletest.sh $n)|| echo "$n failed"
done