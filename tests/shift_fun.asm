# shift_fun.asm
# test out the SHIFTL instruction
# print a stored value
LOAD 00 25
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

# stored value
DB 7E


# bash script
# for i in `seq 0 7`; do  printf '%d %X\n' ${i} $(( ( ( 0xFE << ${i} ) + 0x80 ) & 0xFF  )) ; done
