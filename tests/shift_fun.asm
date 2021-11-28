# shift_fun.asm
# test out the SHIFTL instruction
# jump over our labels
JMP 00 04
# stored constant 
:letter
DB 7E

# print a stored value
LOAD letter
STORE FF FF
# shift, subtract 127, and print it a few times
SHIFTL
ADDI 80
STORE FF FF
SHIFTL
ADDI 80
STORE FF FF
SHIFTL
ADDI 80
STORE FF FF
SHIFTL
ADDI 80
STORE FF FF
SHIFTL
ADDI 80
STORE FF FF
HALT

# bash script
# for i in `seq 0 7`; do  printf '%d %X\n' ${i} $(( ( ( 0xFE << ${i} ) + 0x80 ) & 0xFF  )) ; done
