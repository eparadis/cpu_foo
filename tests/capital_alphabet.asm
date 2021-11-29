# print capital A thru Z
LOAD starting_char
:top
ADDI 01
STORE FF FF
CMP 5A
JLT top
HALT
:starting_char
DB 40
DB 34

# comment

# ^^ blank line
