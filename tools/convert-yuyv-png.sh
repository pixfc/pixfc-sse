#!/bin/bash -x

if [ $# -ne 1 -a $# -ne 2 ]; then
	echo "Usage: $0 <yuv_in_format> [filename]"
	echo "where 'yuv_in_format' is either YUYV, UYVY, YUV422p or YUV420p"
	echo "Input files must also bear that extension"
	echo "If 'filename' is specified, only this file will be converted"
	exit 1
fi

ext=$1

# Get list of files to convert
if [ $# -eq 2 ]; then
	if [ ! -f "$2" ]; then
		echo "File \'$2\' does not exist"
		exit 1
	fi
	filearray[0]="$2"
else
	readarray -t filearray < <(ls -1 *.${ext})
fi

# Reset the commands
cmd=
cmd2=

# Set commands based on the provided extension
if [ "${ext}" = "YUYV" ]; then
	cmd="mplayer -demuxer rawvideo -rawvideo format=yuy2:w=WIDTH:h=HEIGHT -vo png:z=0"
elif [ "${ext}" = "UYVY" ]; then
	cmd="mplayer -demuxer rawvideo -rawvideo format=uyvy:w=WIDTH:h=HEIGHT -vo png:z=0"
elif [ "${ext}" = "YUV422p" ]; then
	# I cant get mplayer's rawvideo module to accept the 422p format,
	# so we do the conversion in 2 steps: to yuyv first using one of our tool
	# then to png using mplayer
	
	# Ensure yuv422p_to_yuyv exists, otherwise build it
	if [ ! -x "$(dirname $0)/yuv422p_to_yuyv" ]; then
		gcc "$(dirname $0)/yuv422p_to_yuyv.c" -o "$(dirname $0)/yuv422p_to_yuyv"
	fi  
	cmd="$(dirname $0)/yuv422p_to_yuyv WIDTH HEIGHT"
	cmd2="mplayer -demuxer rawvideo -rawvideo format=yuy2:w=WIDTH:h=HEIGHT -vo png:z=0 output.YUYV"
elif [ "${ext}" = "YUV420p" ]; then
	cmd="mplayer -demuxer rawvideo -rawvideo format=i420:w=WIDTH:h=HEIGHT -vo png:z=0"
else
	echo "Unknown yuv format \'${ext}\'"
	exit 1
fi

# Perform conversion for each file
for (( i=0; i<${#filearray[@]}; i++ ))
do
	name="${filearray[$i]}"
	echo $name

	# Get width and height from filename
	width=${name%%_*}
	rest="${name#*_}"
	height=${rest%%_*}

	if [ -z "$width" -o -z "$height" ]; then
		echo "Invalid width or height in filename: " $name
		exit 1
	fi

	# replace WIDTH and HEIGHT with actual value in commands
	cmd="${cmd/WIDTH/$width}"
	cmd="${cmd/HEIGHT/$height}"

	# run first commandd
	${cmd} "${filearray[$i]}"

	# and repeat if there is a second one
	if [ -n "${cmd2}" ]; then
		cmd2="${cmd2/WIDTH/$width}"
		cmd2="${cmd2/HEIGHT/$height}"
		${cmd2}
	fi

	mv 00000001.png "${filearray[$i]}.png"
done

