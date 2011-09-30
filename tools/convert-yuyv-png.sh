#!/bin/bash -x

#if [ $# -ne 3 -a $# -ne 4 ]; then
#	echo "Usage: $0 <yuv_in_format> <width> <height> [filename]"
#	echo "where 'yuv_in_format' is either YUYV, UYVY or YUV422p"
#	echo "Input files must also bear that extension"
#	echo "If 'filename' is specified, onyl this file will be converted"
#	exit 1
#fi

ext=$1
width=$2
height=$3

# Get list of files to convert
if [ $# -eq 4 ]; then
	filearray[0]="$4"
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
	# We need to convert to YUYV first, using one of our tools.
	# Ensure yuv422p_to_yuyv exists, otherwise build it
	if [ ! -x "$(dirname $0)/yuv422p_to_yuyv" ]; then
		gcc "$(dirname $0)/yuv422p_to_yuyv.c" -o "$(dirname $0)/yuv422p_to_yuyv"
	fi  
	cmd="$(dirname $0)/yuv422p_to_yuyv WIDTH HEIGHT"
	cmd2="mplayer -demuxer rawvideo -rawvideo format=yuy2:w=WIDTH:h=HEIGHT -vo png:z=0 output.YUYV"
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

