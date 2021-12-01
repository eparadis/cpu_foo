# monitor.asm - should be loaded at 0x0100 (the exception vector)

# commands:
# (xxxx is a four digit uppercase hex number)
# (yy is a two digit uppercase hex number)
# Gxxxx - jump to location
# Lxxxx - set the cursor to xxxx
# Wyy - write a byte at the current cursor location and increment the cursor

# jump to start
JMP start
# constant: prompt 0x0103
:prompt
DB 3E
# JMP command we'll modify to use the G command 0x0104 05 06
:go_vector
JMP
:go_vector_hi
DB 00
:go_vector_lo
DB 00
# variable: cursor 0x0107 08
:cursor_hi
DB 00
:cursor_lo
DB 00

:char_newline
DB 0A

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
STORE go_vector_hi
# low byte
POP
STORE go_vector_lo
# print a newline
LOAD char_newline
STORE FF FF
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
# print a newline
LOAD char_newline
STORE FF FF
# jump back for the next command
JMP start

:char_0
DB 30

# 'W' command - "Write a byte at cursor and increment cursor"
# push two garbage values we'll discard
:w_command
LOAD char_0
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
# now increment the cursor
# TODO we don't have 16 bit math, so we can only work inside 256 byte segments before having to set the cursor again
LOAD cursor_lo
ADDI 01
STORE cursor_lo
# print a newline
LOAD char_newline
STORE FF FF
# all done
JMP start 

# hex location converter -- takes four ascii hexadecimal characters on the stack and pushes their 16bit value on the stack
# characters entered: WXYZ (WX hi byte, YZ lo byte)
# on stack like: (top) Z Y X W (bottom)
# result: (top) jj kk (bottom) (jj high byte, kk low byte)
:hex_converter
# convert top of stack from ascii to hex
CALL ascii_to_hex
POP
# put it in a temp location
STORE _hc_tmp_lo
# convert another
CALL ascii_to_hex
POP
# multiply it by 16
SHIFTL
SHIFTL
SHIFTL
SHIFTL
# add in the top hex character
ADD _hc_tmp_lo
# store back in the temp location
STORE _hc_tmp_lo
# now convert the high byte
# convert
CALL ascii_to_hex
POP
# put in another tmp location
STORE _hc_tmp_hi
# convert last byte
CALL ascii_to_hex
POP
# multiply by 16
SHIFTL
SHIFTL
SHIFTL
SHIFTL
# add in top hex character
ADD _hc_tmp_hi
# push low byte first
LOAD _hc_tmp_lo 
PUSH
# fetch the high byte and put it on the stack
LOAD _hc_tmp_hi
PUSH
# we're done, return
RET

:_hc_tmp_hi
DB 00
:_hc_tmp_lo
DB 00

# hex character converter -- takes an acii hexadecimal character on the stack and pushes it's 4 bit value
# '0' -> 0x30
# 'A' -> 0x41
:ascii_to_hex
# if the value is greater than 0x39 ('9'), subtract 55 (0x37)
POP
CMP 40
# if it's less than 0x40, it's a number
JLT _a2h_num
# else it's a letter
# -55 => 0xC9 
ADDI C9
PUSH
RET
# otherwise subtract 48 (0x30)
:_a2h_num
# -48 => 0xD0
ADDI D0
PUSH
RET

