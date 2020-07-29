del test-i386_conv.exe test-i386_conv.txt
rem g++ -O0  -I.. test-i386_conv.cpp -E -fpermissive > test-i386_conv.e
g++ -O0  -I.. test-i386_conv.cpp -o test-i386_conv -fpermissive -w 2>&1 
rem -Og -ggdb3
test-i386_conv.exe > test-i386_conv.txt
\winmerge\winmergeu test-i386.txt test-i386_conv.txt 