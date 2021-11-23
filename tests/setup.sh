
echo "compiling main"
pushd ../
gcc -lm -o main main.c
popd
cp ../main .

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

# send a small program to display a single character into the loader
./send.py tiny_echo.bin | ./main tiny_loader.bin

# print the alphabet
./main capital_alphabet.bin

# send some chars and then stop at Z
echo "zzzWXYZ123" | ./main echo_until_Z.bin

# reverse a line of input
echo "boxcar" | ./main reverse_line.bin
