# echo until capital-Z
LOAD FF FF
STORE FF FF
CMP 5A
JE 00 0E
JMP 00 00
# illegal opcode to cause halt
DB AA