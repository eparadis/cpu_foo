# echo until capital-Z
:top
LOAD FF FF
STORE FF FF
CMP 5A
JE end 
JMP top
:end
HALT
