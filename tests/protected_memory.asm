# protected_memory.asm - test illegal memory access

# set the protection window to be up til the 'user' code
SPMW user_code
# set the exception vector to a simple 'print err and die'
SEVA exception_vector
# jump to init, which will trampoline to user mode
JMP init

:exception_vector
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

:init
# TODO set up the stacks before leaving PMODE
# load a char in rA so we can _try_ to print it (should fail)
LOAD char_A
# change to user mode and jump to user code
JCPF user_code
# if we weren't in PMODE, that JCPF would be a NOP.
# but we should be, so you shouldn't see this 'B' printed.
LOAD char_B
STORE FF FF
HALT

:user_code
# print the contents of rA three times
STORE FF FF
STORE FF FF
STORE FF FF
# now try to jump back into protected memory; this would be an inf. loop, but it'll throw instead
JMP init

:char_A
DB 41
:char_B
DB 42
:char_E
DB 45
:char_X
DB 58
:char_newline
DB 0A
:char_bang
DB 21
