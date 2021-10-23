



# compile ex1.asm and run its output, formatting the simulators output to characters
bash ./super_crapsm.sh ex1.asm out && ./main out | grep OUT | awk '{ printf "%s", $2 } END { printf "\n" }'


