#!/usr/bin/env python3

import os
import sys

if len(sys.argv) != 3:
  print("usage:", sys.argv[0], "INFILE.asm", "OUTFILE.bin")
  exit(1)

infilename = sys.argv[1]
outfilename = sys.argv[2]

def convert( word):
  if word == 'DB' or word == 'DW' or word == '':
    return (0, '')
  mnuemonics = [ 'LOAD', 'STORE', 'ADDI', 'NOP', 'CMP', 'JE', 'JLT', 'JMP', 'PUSH', 'POP', 'HALT', 'SHIFTL']
  opcodes =    [ '08'  , '09'   , '0A'  , '00' , '0B' , '0C', '0D' , '0E' , '0F'  , '10' , '11'  , '12' ]
  if word in mnuemonics:
    return (1, opcodes[mnuemonics.index(word)])
  return (1, word)

with open(infilename, 'r') as infile, open(outfilename, 'wb') as outfile:
  position = 0
  for line in infile:
    if line[0] == '#':
      continue
    for word in line.split(' '):
      (length, val) = convert(word.strip())
      if length == 0:
        continue
      print(val, end=' ')
      position += length
      outfile.write(bytes.fromhex(val))
  print()



#  data = f.read(length)
#  sys.stdout.buffer.write(bytes([length]))
#  sys.stdout.buffer.write(data)
#  f.close()
