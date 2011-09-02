#!/bin/bash

if [ $# -ne 2 -a $# -ne 3 ]; then
    echo "Usage: $0 <path to unit-testing 1> <path to unit-testing 2> [ symbol ]"
	echo "Prints the difference in the number of instructions between the two"
	echo "versions of unit-testing for each conversion routine."
	echo "If 'symbol' is specified, then only the instructions in this symbol"
	echo "are counted."
    exit 1
fi

UNIT_TESTING1="$1"
UNIT_TESTING2="$2"
DISASSEMBLY_FILE1=/tmp/disassembly1
DISASSEMBLY_FILE2=/tmp/disassembly2


#
# Sanity checks
rm -f ${DISASSEMBLY_FILE1} ${DISASSEMBLY_FILE2}
if [ ! -x ${UNIT_TESTING1} ]; then
    echo "${UNIT_TESTING1} does not exist or is not executable"
    exit 1
fi
if [ ! -x ${UNIT_TESTING2} ]; then
    echo "${UNIT_TESTING2} does not exist or is not executable"
    exit 1
fi

#
# Get the symbol list for both unit testing apps
SYMBOLS_1=$(nm "${UNIT_TESTING1}" | grep 'T' | cut -f3 -d' ' | grep 'convert')
SYMBOLS_2=$(nm "${UNIT_TESTING2}" | grep 'T' | cut -f3 -d' ' | grep 'convert')


#
# List of symbols common to unit-testing 1 and 2
declare -a COMMON_SYM
# Array of difference in instruction count 
declare -a INSTR_COUNT
index=0


#
# Build a list of symbols for which we are going to compare the
# number of instructions in both apps
if [ $# -eq 3 ]; then
	# only count instructions in given symbol
	
	# ensure symbol exits in both apps
	echo $SYMBOLS_1 | grep -q "$3" || ( echo "Symbol $3 not found in unit-testing 1"; exit 1 )
	echo $SYMBOLS_2 | grep -q "$3" || ( echo "Symbol $3 not found in unit-testing 2"; exit 1 )

	COMMON_SYM[0]=$3
else
	# Check which symbols from list 2 are also in list 1
	for sym in $SYMBOLS_2
	do
		echo $SYMBOLS_1 | grep -q $sym
		if [ $? -eq 0 ]; then
			COMMON_SYM[$index]=$sym
			(( index++ ))
		else
			 echo "Symbol $sym is not in unit-testing 1" 1>&2
		fi
	done

	# Check which symbols from list 1 are not in list 2
	for sym in $SYMBOLS_1
	do
		echo $SYMBOLS_2 | grep -q $sym || ( echo "Symbol $sym in not in unit-testing 2" 1>&2 )
	done
fi
echo


#
# Check OS and disassemble both apps
echo $OSTYPE | grep -q "linux" && OS=linux
if [ -n "$OS" ]; then
	#linux
	objdump -d ${UNIT_TESTING1} > ${DISASSEMBLY_FILE1}
	objdump -d ${UNIT_TESTING2} > ${DISASSEMBLY_FILE2}
else
	#mac
	otool -tV ${UNIT_TESTING1} > ${DISASSEMBLY_FILE1}
	otool -tV ${UNIT_TESTING2} > ${DISASSEMBLY_FILE2}
fi

index=0
# Compute difference in number of instructions in both apps
# for each symbols in COMMON_SYM
for sym in ${COMMON_SYM[*]}
do
	# Check OS and set the sed pattern
	if [ -n "$OS" ]; then
		# linux
		pattern="/<$sym/,/^$/ p"
	else # assume Mac
		pattern="/$sym:/,/^_/ p"
	fi

	count1=$(sed -n "${pattern}" ${DISASSEMBLY_FILE1} | wc -l)
	count2=$(sed -n "${pattern}" ${DISASSEMBLY_FILE2} | wc -l)
	INSTR_COUNT[$index]=$(( count2 - count1 ))
	(( index++ ))
done


#
# Print out result
index=0
while [ $index -lt ${#COMMON_SYM[*]} ]
do
	if [ ${INSTR_COUNT[$index]} -ne 0 ]; then
		printf "%-60s        % 5d\n" ${COMMON_SYM[$index]} ${INSTR_COUNT[$index]}
	fi
	(( index++ ))
done

#for sym in "${!INSTR_COUNT[@]}"
#do
#	if [ ${INSTR_COUNT[$sym]} -ne 0 ]; then
#		printf "%-60s        % 5d\n" $sym ${INSTR_COUNT[$sym]}
#	fi
#done



