# reverse_line.asm - accept a line of text and then echo it in reverse
# set up the data stack
SDS 01 FF
# push a null as a marker
PUSH
# echo until new line
:top
LOAD FF FF
STORE FF FF
PUSH
CMP 0A
JE next
JMP top
# pop off stack until a null
:next
POP
CMP 00
JE end
STORE FF FF
JMP next
:end
HALT
