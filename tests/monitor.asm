# monitor.asm - should be loaded at 0x0100 (the exception vector)

# jump to start
JMP 01 xx
# constant: prompt 0x0103
:prompt
DB 3E
# JMP command we'll modify to use the G command 0x0104 05 06
:go_vector
JMP 00 00
# variable: cursor 0x0107 08
:cursor_hi
DB 00
:cursor_hi
DB 00

# start
:start
# print prompt
LOAD prompt
STORE FF FF

# get command
LOAD FF FF
# is it 'G' ? jump to handler
CMP 47
JE 01 xx
# is it 'L' ? jump to handler
CMP 4C
JE 01 xx
# otherwise jump back to start to get command again
JMP start

# 'G' command - "Go"
# get four characters
:g_commmand
LOAD FF FF
PUSH
LOAD FF FF
PUSH
LOAD FF FF
PUSH
LOAD FF FF
PUSH
# call the hex converter
CALL xx xx
# top of the stack is high byte of value, modify our jump vector
POP
STORE 01 05
# low byte
POP
STORE 01 06
# JMP to the vector we just set
JMP go_vector


# 'L' command - "set cursor Location"
# get four characters
:l_command
LOAD FF FF
PUSH
LOAD FF FF
PUSH
LOAD FF FF
PUSH
LOAD FF FF
PUSH
# call the hex converter
CALL xx xx
# top of stack is high byte of our cursor
POP
STORE 01 07
POP
STORE 01 08
# jump back for the next command
JMP 01 00

# 'W' command - "Write a byte at cursor and increment cursor"
# push two garbage values we'll discard
:w_command
PUSH
PUSH
# get two characters
LOAD FF FF
PUSH
LOAD FF FF
PUSH
# call the hex converter
CALL xx xx
# discard the top byte
POP
# second byte is our data, load it where the cursor points
# modify the storying vector. we use self modifying code b/c there's no indexing yet
LOAD 01 07
STORE 01 xx
LOAD 01 08
STORE 01 xx
# finally pop the byte that's been waiting in the stack
POP
# storing vector, this is what we modify above
STORE 00 00
# all done
JMP 01 00

# hex location converter -- takes four ascii hexadecimal characters on the stack and pushes their 16bit value on the stack
# characters entered: WXYZ (WX top byte, YZ bottom byte)
# on stack like: (top) W X Y Z (bottom)
# result: (top) jj kk (bottom) (jj high byte, kk low byte)
# convert top of stack from ascii to hex
CALL xx xx
POP
# multiply it by 16
SHIFTL
SHIFTL
SHIFTL
SHIFTL
# put it in a temp location
STORE xx xx
# convert another
CALL xx xx
POP
# add in the top hex character
ADD xx xx
# store back in the temp location
STORE xx xx
# now convert the low byte
# convert
CALL xx xx
POP
# multiply by 16
SHIFTL
SHIFTL
SHIFTL
SHIFTL
# put in a tmp2 location
STORE xx xx
# convert last byte
CALL xx xx
POP
# add in top hex character
ADD xx xx
# push because we return the low byte last
PUSH
# fetch the high byte and put it on the stack
LOAD xx xx
PUSH
# we're done, return
RET

# hex character converter -- takes an acii hexadecimal character on the stack and pushes it's 4 bit value
# '0' -> 0x30
# 'A' -> 0x41
# if the value is greater than 0x39 ('9'), subtract 55 (0x37)
CMP 40
JLT xx xx
# -55 => 0xC9 
ADDI C9
JMP xx xx
# otherwise subtract 48 (0x30)
# -48 => 0xD0
ADDI D0
# we're done
PUSH
RET

