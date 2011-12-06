#!/bin/bash
# Usage: ./BatchPlaceDBImages.sh LTPM_name rows cols number_of_superpixels

LTPM_name=$1
rows=$2
cols=$3
number_of_superpixels=$4

for candidateImagePath in Data/DBImages/Images/*.jpg
do
	candidateName=`basename $candidateImagePath`
	for ((  row = 0 ;  row <= $rows;  row++  ))
	do
		for ((  col = 0 ;  col <= $cols;  col++  ))
		do
			echo Checking $candidateName at $row, $col ...
			./MatchRealSegsAndStoreScore.sh $LTPM_name $row $col $candidateName $number_of_superpixels
			
		done
	done
done
