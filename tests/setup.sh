#!/usr/bin/env bash -e

echo "compiling main"
pushd ../
gcc -lm -o main main.c
popd
ln -f -s ../main main

# assemble a binary loader. it takes a single byte length followed by data to place at 0100, then jumps there
echo "assembling tiny_loader"
./crapsm.py tiny_loader.asm tiny_loader.bin
echo "assembling tiny_echo"
./crapsm.py tiny_echo.asm tiny_echo.bin
echo "assembling capital_alphabet"
./crapsm.py capital_alphabet.asm capital_alphabet.bin
echo "assembling echo_until_Z"
./crapsm.py echo_until_Z.asm echo_until_Z.bin
echo "assembling reverse_line"
./crapsm.py reverse_line.asm reverse_line.bin
echo "assembling shift_fun"
./crapsm.py shift_fun.asm shift_fun.bin
echo "assembling make_caps"
./crapsm.py make_caps.asm make_caps.bin
echo "assembling subroutines"
./crapsm.py subroutines.asm subroutines.bin
echo "assembling monitor"
./crapsm.py monitor.asm monitor.bin
echo "assembling exception_vector"
./crapsm.py exception_vector.asm exception_vector.bin
echo "assembling protected_memory"
./crapsm.py protected_memory.asm protected_memory.bin

# send a small program to display a single character into the loader
./send.py tiny_echo.bin | ./main tiny_loader.bin

# print the alphabet
./main capital_alphabet.bin

# send some chars and then stop at Z
echo "zzzWXYZ123" | ./main echo_until_Z.bin

# reverse a line of input
echo "boxcar" | ./main -r 512 reverse_line.bin

# print out characters that have been SHIFTL'd
./main shift_fun.bin

# make the string capital letters
echo "boxcar" | ./main make_caps.bin

# print a string of *
./main -r 512 subroutines.bin

# load the monitor and use it to write a program that prints a '#'
echo "L0100 W23 W08 W01 W00 W09 WFF WFF W11 G0101" | ./main -r 512 monitor.bin

# print some characters and then try to execute an illegal instruction
./main exception_vector.bin

# setup a user code area and jump to it, then try and jump back
./main protected_memory.bin
