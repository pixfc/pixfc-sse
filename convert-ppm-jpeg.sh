#!/bin/bash

readarray -t filearray < <(ls -1 *.ppm)

# Perform conversion for each file
for (( i=0; i<${#filearray[@]}; i++ ))
do
        pnmtojpeg "${filearray[$i]}" > "${filearray[$i]}".jpg
done

