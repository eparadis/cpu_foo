# make_caps.asm - input lower case letters and they're printed back uppercase
:top
# grab a character
LOAD FF FF
# if it's equal to a newline, we're done
CMP 0A
JE end
# otherwise, add the offset to make it capital
ADD capital_offset
# send it out
STORE FF FF
# repeat
JMP top

:end
HALT

:capital_offset
# we want to subtract 0x20. -0x20 => E0
DB E0
