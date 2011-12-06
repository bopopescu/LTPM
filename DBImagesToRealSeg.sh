#!/bin/sh
# Usage: <number of super pixels>
#mkdir $1/segs

number_of_superpixels=$1

mkdir -p Data/DBImages/BerkeleySegs
mkdir -p Data/DBImages/RealSegs

for i in Data/DBImages/Images/*.jpg
do 
	imageName=`basename $i`
	echo Berkeley segmenting $imageName ...
	berkeleySegFile=Data/DBImages/BerkeleySegs/`basename $i`.$number_of_superpixels.seg
	./Libraries/BSE-1.2/segment -image $i -segfile $berkeleySegFile -numsuperpixels $number_of_superpixels
	echo Real segmenting $imageName ...
	./SuperPixelsToSegmentation/build/SuperPixelsToSegmentation $i $berkeleySegFile 0 > Data/DBImages/RealSegs/$imageName.$number_of_superpixels.realseg
done
