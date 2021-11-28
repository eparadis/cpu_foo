# push a null as a marker
PUSH
# echo until new line
:top
LOAD FF FF
STORE FF FF
PUSH
CMP 0A
JE 00 10
JMP top
# pop off stack until a null
:next
POP
CMP 00
JE 00 1C
STORE FF FF
JMP next
HALT
