# cpu_foo

## The Story
I couldn't sleep one night so I started writing this.

https://gist.github.com/eparadis/0e5fe0aac7bfe005cab65df5d9e4209d

I couldn't leave well enough alone and over the subsequent few days I wrote a little more and a little more. Now I've got this repo.

It's a simple 8bit CPU emulator. Not many opcodes are implemented. The ones that are were choosen off the top of my head. There's a simple assembler written in python3.

I think the biggest lesson of this is that I can write this kind of thing (nearly) in my sleep, so I need to move on to other problems. I've not been able to bridge the "machine to high level language" gap, even thought I've messed with many ideas around FORTH and BASIC. I feel like the authors of _The Elements of Computing Systems_ would be really disappointed in me.

What's next? Memory management? String manipulation? Terminal emulation? Who knows?

## Documentation

Instructions so far:
```
yyyy is a 16bit address
xx is an 8 bit value

LOAD yyyy - Load accumulator from address yyyy
STORE yyyy - Store accumulator at address yyyy
ADDI xx - Add Immediate value xx to accumulator
NOP - No OPeration
CMP xx - Compare accumulator to immediate value xx. Set flags register accordingly. 
JE yyyy - Jump to address yyyy if Equal flag is set
JLT yyyy - Jump to address yyyy if Less Than flag is set
JMP yyyy - Jump to address yyyy
PUSH - Push accumulator to data stack
POP - Pop top of data stack and store in accumulator
HALT - Halt the CPU. This stops the simulator.
SHIFTL - Shift accumulator Left one bit.
ADD yyyy - Add the contents at address yyyy to the accumulator
CALL yyyy - Push the address of the next instruction on to the call stack and jump to yyyy
RET - Pop the address of the call stack and jump to it
SDS yyyy - (PMODE only) Set the Data Stack register
SSP yyyy - (PMODE only) Set the Stack Pointer (call stack) register
SEVA yyyy - (PMODE only) Set the Exception Vector Address register
SPMW yyyy - (PMODE only) Set the Priviledged Memory Window Address register
JCPF yyyy - (PMODE only) Jump to address yyyy and Clear the Pmode Flag
```

Opcodes are one byte long, so instructions vary from one to three bytes.

Illegal opcodes/instructions cause an exception. The cpu will immediately jump to the address stored in the Exception Vector Address register.

If not in PMODE (Pmode flag set), then any memory access above the value of the Priviledges Memory Window register will cause an immediate exception.

