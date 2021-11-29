# subroutines.asm - use CALL and RET a bunch to print something

CALL newline
CALL some_stars
HALT

:some_stars
# print a line of stars.
LOAD count
PUSH
:_ss_top
POP
CMP 00
JE _ss_done
ADDI FF
PUSH
CALL star
JMP _ss_top
:_ss_done
CALL newline
RET

:newline
LOAD char_newline
STORE FF FF
RET

:star
LOAD char_star
STORE FF FF
RET

:count
DB 04
:char_newline
DB 0A
:char_star
DB 2A
