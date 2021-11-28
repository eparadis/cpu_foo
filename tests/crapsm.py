#!/usr/bin/env python3

import os
import sys

if len(sys.argv) != 3:
  print("usage:", sys.argv[0], "INFILE.asm", "OUTFILE.bin")
  exit(1)

infilename = sys.argv[1]
outfilename = sys.argv[2]

def convert( word, labels):
  if word == 'DB' or word == 'DW' or word == '':
    return (0, '')
  if word in labels:
    return (2, "%0.2X %0.2X" % (labels[word] >> 8, labels[word] & 0xFF ) )
  mnuemonics = [ 'LOAD', 'STORE', 'ADDI', 'NOP', 'CMP', 'JE', 'JLT', 'JMP', 'PUSH', 'POP', 'HALT', 'SHIFTL']
  opcodes =    [ '08'  , '09'   , '0A'  , '00' , '0B' , '0C', '0D' , '0E' , '0F'  , '10' , '11'  , '12' ]
  if word in mnuemonics:
    return (1, opcodes[mnuemonics.index(word)])
  return (1, word)

labels = {}

with open(infilename, 'r') as infile, open(outfilename, 'wb') as outfile:
  position = 0
  for line in infile:
    if line[0] == '#':
      continue
    if line[0] == ':':
      label = line.split()[0][1:]
      print(label, end=' ')
      labels[label] = position
      continue
    for word in line.split():
      (length, val) = convert(word.strip(), labels)
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
