#!/bin/sh
# needs path to images and number of super pixels to generate
#mkdir $1/segs

for i in $1*.jpg
do 
echo Segmenting... $i
./Libraries/BSE-1.2/segment -image $i -segfile $i.$2.seg -numsuperpixels $2
done

#
#./VisualizeSegmentation/build/LTPM $1 $1.$2.seg
