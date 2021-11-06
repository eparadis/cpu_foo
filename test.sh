
# this is how repl.it was compiling main.c
#clang-7 -pthread -lm -o main main.c
clang -lm -o main main.c

# compile ex1.asm and run its output, formatting the simulators output to characters
echo "print the capital alphabet"
rm out && bash ./super_crapsm.sh ex1.asm out && ./main out
echo

# compile ex2.asm and run its output, 
echo "type Z to end example 2"
rm out && bash ./super_crapsm.sh ex2.asm out && ./main out 
echo
