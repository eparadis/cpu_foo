
echo "compiling main"
pushd ../
gcc -lm -o main main.c
popd
cp ../main .

# assemble a binary loader. it takes a single byte length followed by data to place at 0100, then jumps there
echo "assembling loader"
bash ../super_crapsm.sh loader.asm loader.bin
echo "assembling tiny_echo"
bash ../super_crapsm.sh tiny_echo.asm tiny_echo.bin
echo "assembling capital_alphabet"
bash ../super_crapsm.sh capital_alphabet.asm capital_alphabet.bin
echo "assembling echo_until_Z"
bash ../super_crapsm.sh echo_until_Z.asm echo_until_Z.bin

# send a small program to display a single character into the loader
./send.py tiny_echo.bin | ./main loader.bin

# print the alphabet
./main capital_alphabet.bin

# send some chars and then stop at Z
echo "zzzWXYZ123" | ./main echo_until_Z.bin
