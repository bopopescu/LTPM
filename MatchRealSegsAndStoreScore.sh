#!/bin/sh
# Usage: <LTPM_name> <target_row> <target_col> <candidate_name> <number_of_superpixels>

LTPM_name=$1
target_row=$2
target_col=$3
candidate_name=$4
number_of_superpixels=$5

target_realseg_path=Data/${LTPM_name}/TileRealSegs/Original_${target_row}_${target_col}.jpg.${number_of_superpixels}.realseg
candidate_realseg_path=Data/DBImages/RealSegs/${candidate_name}.${number_of_superpixels}.realseg


score=`./PlaceImage/build/PlaceImage ${target_realseg_path} ${candidate_realseg_path}`

# insert in db
echo "insert into scores(LTPM_name, target_row, target_col, candidate_name, score) VALUES('$LTPM_name', '$target_row', '$target_col', '$candidate_name', $score)" | mysql -u LTPM LTPM


echo "match with ($target_row, $target_col): $score"
