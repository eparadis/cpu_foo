
# this is how repl.it was compiling main.c
#clang-7 -pthread -lm -o main main.c
#clang -lm -o main main.c
gcc -lm -o main main.c

# compile ex1.asm and run its output, formatting the simulators output to characters
echo "print the capital alphabet"
rm -f out && bash ./super_crapsm.sh ex1.asm out && ./main out
echo

# compile ex2.asm and run its output, 
echo "type Z to end example 2. use 'stty cbreak' in your terminal to avoid requiring newlines each character"
rm -f out && bash ./super_crapsm.sh ex2.asm out && echo Z | ./main out 
echo


# compile ex3.asm (a binary loader)
echo "compiling loader"
bash ./super_crapsm.sh ex3.asm loader
# send a small program to display a single character into the loader
# first byte is length, followed by the data
echo "loading 7 byte program: LOAD 00 05  STORE FF FF  DB AA\n"
echo -ne \\x07\\x08\\x00\\x05\\x09\\xff\\xff\\xaa | ./main loader
echo
