# Execution model

## Registers:
A - accumulator
FLAGS - status flags:
  HALT
  EQUAL
  GREATER-THAN
  P-LEVEL
IP - instruction pointer
EA - copy of A when an exception occurs
EFLAGS - copy of FLAGS when an exception occurs
EIP - copy of IP when an exception occurs

## Modes
### P-LEVEL flag
0: normal execution
1: priviledge mode

Memory map
0000 P-Level access begins
0100 Exception handler address
7FFE P-Level bottom of stack (grows downwards)
7FFF P-level console IO
8000 non P-level access begins
FFFE non P-level bottom of stack (grows downwards)
FFFF non P-level console IO

### Normal exectuion opcodes and address space
All opcodes are allowed except IN, OUT, SETF, READF, (things that read EA, EIP, EFLAGS), ???
Read and write memory access is allowed from 8000 to FFFF.

### Priviledge mode opcodes and address space
All opcodes are allowed.
Read and write access is allowed from 0000 to FFFF.

### Switching modes
Priviledge mode is enabled upon reset.

Priviledge mode is entered in the following ways:
- reset
- execution of an illegal opcode
- illegal memory access
- ??? division by zero (no div opcode yet)
- ??? stack underflow (no stack yet)
- ??? stack overflow (no stack yet)
- ??? hardware interrupt (no hardware interrupt yet)
- ??? reading or writing to a reserved port (no ports yet)

Upon causing an exception, the following is performed before entering P-level 1:
- The A, FLAGS, and IP registers are stored in EA, EFLAGS, and EIP, respectively
- IP is set to 0100
- The P-LEVEL flag is set. The rest of FLAGS is cleared.
- A is set to the opcode that caused the exception 
- Execution resumes at the address of IP.

Priviledge mode may return to normal mode using the JNORM opcode.
- A and FLAGS must be set prior to JNORM
- P-level flag is explicitly cleared
- JNORM is a jump instruction, so the next two locations in memory are used as the target address.
- IP is set to this target address and execution continues

*notes*
If A is set to the original opcode, P-mode can trap unimplemented opcodes and emulate them.
Proposed PUSH and POP opcodes could be used to detect stack over and under-flows.
LOAD or STORE accessing illegal memory would be fairly easy to handle like the above.
JMP (etc) would also be easy to handle in the above case.
The tricky part would be normal execution reaching the end of its space and "load"ing something outside it.
Software interrupts seem redundant when you could use an unimplemented opcode to do the same by convention.
[ref](https://people.cs.pitt.edu/~don/coe1502/current/Unit4a/Unit4a.html)

### P-Level and address spaces
P-LEVEL has access to the bottom half of the address space.
Non P-level access to the bottom half triggers an exception to P-level, which should allow for redirectable MMIO and virtual memory.
MMIO can exist in either mode's address space. If it is in the top half, it has no P-level filtering or management but is faster.
A sophisticated peripheral could have control ports only accessible at P-level that configure access at some normal level address for speed.

## Stack operations
### Normal mode
SP holds the top-of-stack address for the PUSH and POP instructions.
The stack grows downwards, so SP is subtracted from in a PUSH opereation, and added to in a POP operation.
CALL and RET push and pop the IP register to and from the stack, high byte first.

### P-level mode
EP holds the top-of-stack address for the PUSH and POP instructions in P-level. It points somewhere in the protected segment
Otherwise, stack operations are the same as in normal operation.




