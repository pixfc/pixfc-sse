#!/bin/bash -x

if [ $# -ne 3 -a $# -ne 4 ]; then
	echo "Usage: $0 <yuv_in_format> <width> <height> [filename]"
	echo "where 'yuv_in_format' is either YUYV, UYVY or YUV422p"
	echo "Input files must also bear that extension"
	echo "If 'filename' is specified, onyl this file will be converted"
	exit 1
fi

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
	cmd="mplayer -demuxer rawvideo -rawvideo format=yuy2:w=${width}:h=${height} -vo png:z=0"
elif [ "${ext}" = "UYVY" ]; then
	cmd="mplayer -demuxer rawvideo -rawvideo format=uyvy:w=${width}:h=${height} -vo png:z=0"
elif [ "${ext}" = "YUV422p" ]; then
	# We need to convert to YUYV first, using one of our tools.
	# Ensure yuv422p_to_yuyv exists, otherwise build it
	if [ ! -x "$(dirname $0)/yuv422p_to_yuyv" ]; then
		gcc "$(dirname $0)/yuv422p_to_yuyv.c" -o "$(dirname $0)/yuv422p_to_yuyv"
	fi  
	cmd="$(dirname $0)/yuv422p_to_yuyv ${width} ${height}"
	cmd2="mplayer -demuxer rawvideo -rawvideo format=yuy2:w=${width}:h=${height} -vo png:z=0 output.YUYV"
else
	echo "Unknown yuv format \'${ext}\'"
	exit 1
fi

# Perform conversion for each file
for (( i=0; i<${#filearray[@]}; i++ ))
do
	echo ${filearray[$i]}
	${cmd} "${filearray[$i]}"
	if [ -n "${cmd2}" ]; then
		${cmd2}
	fi

	mv 00000001.png "${filearray[$i]}.png"
done

