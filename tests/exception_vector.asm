# exception_vector.asm - set up an exception vector, then use an invalid opcode to test it

# set the Exception Vector Address
SEVA eva_vector
# print some letters
LOAD char_A
STORE FF FF
LOAD char_B
STORE FF FF
LOAD char_C
STORE FF FF
# illegal opcode
DB 44
# should never get here!
LOAD char_D
STORE FF FF
HALT



:eva_vector
# print !EX\n and halt
LOAD char_bang
STORE FF FF
LOAD char_E
STORE FF FF
LOAD char_X
STORE FF FF
LOAD char_newline
STORE FF FF
HALT

:char_A
DB 41
:char_B
DB 42
:char_C
DB 43
:char_D
DB 44
:char_E
DB 45
:char_X
DB 58
:char_newline
DB 0A
:char_bang
DB 21
