#!/bin/sh -e
> _result.log
ls *.asm | \
while read name;
do 
  n=$(echo $name| perl -pe 's!\.asm!!');
#  (
echo "Testing $n:"
./_singletest.sh $n
#)|| echo "$n failed"
done