#!/bin/bash

UNIT_TESTING=./tools/unit-testing

if [ $# -ne 1 -a $# -ne 2 -a $# -ne 3 ]; then
    echo "$0 <symbol name> [instruction] [path_to_object_file]"
	echo "If [instruction] is not provided, 'movdqa' is assumed'"
	echo "If [instruction] is @, then all instructions are counted"
    exit 1
fi

if [ $# -eq 3 ]; then
	UNIT_TESTING="$3"
fi

if [ ! -f ${UNIT_TESTING} ]; then
    echo "${UNIT_TESTING} not in current dir"
    exit 1
fi

instr=${2:-movdqa}

# Check OS and set the disassemble command and sed pattern
echo $OSTYPE | grep -q "linux"
if [ $? -eq 0 ]; then
    cmd='objdump -dw'
    pattern='/<'$1'>/,/^$/ {
/nop/ d
p
}'
else # assume Mac
    cmd='otool -tV'
    pattern='/^'$1':/,/^_/ {
/nop/ d
p
}'
fi

if [ "$instr" = "@" ]; then
	${cmd} ${UNIT_TESTING} | sed -n "${pattern}" | wc -l
else
	${cmd} ${UNIT_TESTING} | sed -n "${pattern}" | grep -i "${instr}" | wc -l
fi

