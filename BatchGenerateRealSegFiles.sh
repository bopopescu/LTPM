#!/bin/sh
# Usage: <tile images directory> <seg files directory> <number of superpixels> <realseg output dir>

for i in $1/*.jpg
do 
	imageName=`basename $i`
	echo Working on... $imageName
	./SuperPixelsToSegmentation/build/SuperPixelsToSegmentation $1/$imageName $2/$imageName.$3.seg 0 > $4/$imageName.$3.realseg
done
