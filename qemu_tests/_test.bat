del test-i386_conv.exe test-i386_conv.txt
rem g++ -O0 -ggdb3 -I.. test-i386_conv.cpp -e -fpermissive > test-i386_conv.e
g++ -O0  -I.. test-i386_conv.cpp -o test-i386_conv -fpermissive 2>&1 | grep "error:"
rem -Og -ggdb3
test-i386_conv.exe > test-i386_conv.txt
rem \winmerge\winmergeu test-i386.txt test-i386_conv.txt 