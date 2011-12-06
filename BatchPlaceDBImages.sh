#!/bin/bash
# Usage: ./BatchPlaceDBImages.sh LTPM_name rows cols

LTPM_name=$1
rows=$2
cols=$3

for candidateImagePath in Data/DBImages/Images/*.jpg
do
	candidateName=`basename $candidateImagePath`
	for ((  row = 0 ;  row <= $rows;  row++  ))
	do
		for ((  col = 0 ;  col <= $cols;  col++  ))
		do
			echo Checking $candidateName at $row, $col ...
			./MatchRealSegsAndStoreScore.sh $LTPM_name $row $col $candidateName
			
		done
	done
done
