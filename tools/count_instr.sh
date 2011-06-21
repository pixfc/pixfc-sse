#!/bin/bash

if [ $# -ne 1 -a $# -ne 2 ]; then
    echo "$0 <symbol name> [instruction]"
	echo "If [instruction] is not provided, 'movdqa' is assumed'"
	echo "If [instruction] is @, then all instructions are counted"
    exit 1
fi

if [ ! -f ./unit-testing ]; then
    echo "./unit-testing not in current dir"
    exit 1
fi

instr=${2:-movdqa}

# Check OS and set the disassemble command and sed pattern
echo $OSTYPE | grep -q "linux"
if [ $? -eq 0 ]; then
    cmd='objdump -d'
    pattern="/<$1>/,/^$/ p"
else # assume Mac
    cmd='otool -tV'
    pattern="/$1:/,/^_/ p"
fi

if [ "$instr" = "@" ]; then
	${cmd} unit-testing | sed -n "${pattern}" | wc -l
else
	${cmd} unit-testing | sed -n "${pattern}" | grep -i "${instr}" | wc -l
fi

