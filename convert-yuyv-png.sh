#!/bin/bash

if [ $# -ne 3 ]; then
	echo "Usage: $0 <yuv_in_format> <width> <height>"
	echo "where 'yuv_in_format' is either YUYV or UYVY"
	echo "Input files must also bear that extension"
	exit 1
fi

ext=$1
width=$2
height=$3

if [ "${ext}" = "YUYV" ]; then
	type="yuy2"
elif [ "${ext}" = "UYVY" ]; then
	type="uyvy"
else
	echo "Unkown yuv format \'${ext}\'"
	exit 1
fi

readarray -t filearray < <(ls -1 *.${ext})
for (( i=0; i<${#filearray[@]}; i++ ))
do
	echo ${filearray[$i]}
	mplayer -demuxer rawvideo -rawvideo format=${type}:w=${width}:h=${height} "${filearray[$i]}" -vo png:z=0
	mv 00000001.png "${filearray[$i]}.png"
done

