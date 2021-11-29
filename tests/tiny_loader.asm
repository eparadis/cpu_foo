# copy bytes to memory and execute them
LOAD FF FF
# store the number of bytes we'll copy
STORE count 
# top of loop
:top
LOAD FF FF
#        vv--- we will modify this address
STORE 01 00
# 
LOAD 00 0B
ADDI 01
STORE 00 0B
# 
# get byte count
LOAD count
# subtract 1
ADDI FF
# store it
STORE count 
# 
CMP 00
# all bytes copied, jump to code
JE 01 00
# otherwise go back up
JMP top
:count
