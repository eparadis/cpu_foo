#!/usr/bin/env python3

import os
import sys

if len(sys.argv) != 2:
  print("usage:", sys.argv[0], "FILENAME")
  exit(1)

infile = sys.argv[1]

length = os.stat(infile).st_size

with open(infile, 'rb') as f:
  data = f.read(length)
  sys.stdout.buffer.write(bytes([length]))
  sys.stdout.buffer.write(data)
  f.close()
