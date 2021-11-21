
echo "compiling main"
gcc -lm -o main main.c

# compile ex3.asm (a binary loader)
echo "assembling loader"
bash ../super_crapsm.sh loader.asm loader.bin
echo "assembling test"
bash ../super_crapsm.sh tiny_echo.asm tiny_echo.bin

# send a small program to display a single character into the loader
# first byte is length, followed by the data
#echo "loading 7 byte program: LOAD 00 05  STORE FF FF  DB AA\n"
#echo -ne \\x07\\x08\\x00\\x05\\x09\\xff\\xff\\xaa | ./main loader
#echo
./send.py tiny_echo.bin | ./main loader.bin
