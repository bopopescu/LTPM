#!/bin/sh
# Usage: <LTPM_name> <target_row> <target_col> <candidate_name>

LTPM_name=$1
target_row=$2
target_col=$3
candidate_name=$4

target_realseg_path=Data/${LTPM_name}/TileRealSegs/${LTPM_name}_${target_row}_${target_col}.jpg.32.realseg
candidate_realseg_path=Data/DBImages/RealSegs/${candidate_name}.32.realseg


score=`./PlaceImage/build/PlaceImage ${target_realseg_path} ${candidate_realseg_path}`

# insert in db
echo "insert into scores(LTPM_name, target_row, target_col, candidate_name, score) VALUES('$LTPM_name', '$target_row', '$target_col', '$candidate_name', $score)" | mysql -u LTPM LTPM
