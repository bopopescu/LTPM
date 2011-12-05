#!/bin/sh
# path to folder containing images and .seg files, which .seg resolution to use, and path of out folder

for i in $1*.jpg
do 
echo Working on... $i
./SuperPixelsToSegmentation/build/SuperPixelsToSegmentation $i $i.$2.seg 0 > $3/$(basename $i).realseg
done
