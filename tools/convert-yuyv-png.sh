#!/bin/bash -x

if [ $# -ne 3 -a $# -ne 4 ]; then
	echo "Usage: $0 <yuv_in_format> <width> <height> [filename]"
	echo "where 'yuv_in_format' is either YUYV or UYVY"
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

# Set mplayer fileformat based on the provided extension
if [ "${ext}" = "YUYV" ]; then
	type="yuy2"
elif [ "${ext}" = "UYVY" ]; then
	type="uyvy"
elif [ "${ext}" = "YUV422p" ]; then
	type="422p"
else
	echo "Unkown yuv format \'${ext}\'"
	exit 1
fi

# Perform conversion for each file
for (( i=0; i<${#filearray[@]}; i++ ))
do
	echo ${filearray[$i]}
	mplayer -demuxer rawvideo -rawvideo format=${type}:w=${width}:h=${height} "${filearray[$i]}" -vo png:z=0
	mv 00000001.png "${filearray[$i]}.png"
done

