#!/bin/bash

INFILE=$1
OUTFILE=$2

cat $INFILE | grep -v '^\s*#' | \
 sed s/LOAD/08/g | \
 sed s/STORE/09/g | \
 sed s/ADDI/0A/g | \
 sed s/NOP/00/g | \
 sed s/CMP/0B/g | \
 sed s/JE/0C/g | \
 sed s/JLT/0D/g | \
 sed s/DB//g | \
 sed s/DW//g | \
 xxd -r -p > $OUTFILE

hexdump -C $OUTFILE

# what if I could create those sed commands above directly from the sourcecode? ...
#grep -e '^const.*INSTR' main.c | awk '{ printf "%s %s\n", $3, $5 }' | sed s/INSTR_// | sed s/0x// | tr -d ';' | awk '{ printf "sed s/%s/%s/g | \\ \n", $1, $2 }'


