#!/bin/bash

# Assume this script is in the same dir as the convert-yuyv-png.sh script
PATH_TO_TOOLS="$(dirname $0)"
CONVERT_CMD="${PATH_TO_TOOLS}/convert-yuyv-png.sh"

if [ ! -x "${CONVERT_CMD}" ]; then
    echo "${CONVERT_CMD} is not executable"
    exit 1
fi

for ext in YUYV UYVY YUV422p YUV420p v210
do
    ${CONVERT_CMD} ${ext} -d
done 

