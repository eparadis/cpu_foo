# push a null as a marker
PUSH
# echo until new line
LOAD FF FF
STORE FF FF
PUSH
CMP 0A
JE 00 10
JMP 00 01
# pop off stack until a null
POP
CMP 00
JE 00 1C
STORE FF FF
JMP 00 10

# illegal opcode to cause halt
DB AA
