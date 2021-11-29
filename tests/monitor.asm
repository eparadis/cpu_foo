# monitor.asm - should be loaded at 0x0100 (the exception vector)

# jump to start
JMP start
# constant: prompt 0x0103
:prompt
DB 3E
# JMP command we'll modify to use the G command 0x0104 05 06
:go_vector
JMP 00 00
# variable: cursor 0x0107 08
:cursor_hi
DB 00
:cursor_lo
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
JE g_command
# is it 'L' ? jump to handler
CMP 4C
JE l_command
# is it 'W' ?
CMP 57
JE w_command
# otherwise jump back to start to get command again
JMP start

# 'G' command - "Go"
# get four characters
:g_command
LOAD FF FF
PUSH
LOAD FF FF
PUSH
LOAD FF FF
PUSH
LOAD FF FF
PUSH
# call the hex converter
CALL hex_converter
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
CALL hex_converter
# top of stack is high byte of our cursor
POP
STORE cursor_hi
POP
STORE cursor_lo
# jump back for the next command
JMP start

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
CALL hex_converter
# discard the top byte
POP
# second byte is our data, load it where the cursor points
# modify the storying vector. we use self modifying code b/c there's no indexing yet
LOAD cursor_hi
STORE _w_store_hi 
LOAD cursor_lo
STORE _w_store_lo
# finally pop the byte that's been waiting in the stack
POP
# storing vector, this is what we modify above
STORE
:_w_store_hi
00
:_w_store_lo
00 
# all done
JMP start 

# hex location converter -- takes four ascii hexadecimal characters on the stack and pushes their 16bit value on the stack
# characters entered: WXYZ (WX top byte, YZ bottom byte)
# on stack like: (top) W X Y Z (bottom)
# result: (top) jj kk (bottom) (jj high byte, kk low byte)
:hex_converter
# convert top of stack from ascii to hex
CALL ascii_to_hex
POP
# multiply it by 16
SHIFTL
SHIFTL
SHIFTL
SHIFTL
# put it in a temp location
STORE _hc_tmp
# convert another
CALL ascii_to_hex
POP
# add in the top hex character
ADD _hc_tmp
# store back in the temp location
STORE _hc_tmp
# now convert the low byte
# convert
CALL ascii_to_hex
POP
# multiply by 16
SHIFTL
SHIFTL
SHIFTL
SHIFTL
# put in a tmp2 location
STORE _hc_tmp2
# convert last byte
CALL ascii_to_hex
POP
# add in top hex character
ADD _hc_tmp2
# push because we return the low byte last
PUSH
# fetch the high byte and put it on the stack
LOAD _hc_tmp
PUSH
# we're done, return
RET

:_hc_tmp
DB 00
:_hc_tmp2
DB 00

# hex character converter -- takes an acii hexadecimal character on the stack and pushes it's 4 bit value
# '0' -> 0x30
# 'A' -> 0x41
:ascii_to_hex
# if the value is greater than 0x39 ('9'), subtract 55 (0x37)
CMP 40
JLT _a2h_alpha 
# -55 => 0xC9 
ADDI C9
JMP _a2h_done
# otherwise subtract 48 (0x30)
:_a2h_alpha
# -48 => 0xD0
ADDI D0
# we're done
:_a2h_done
PUSH
RET

