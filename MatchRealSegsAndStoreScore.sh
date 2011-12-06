#!/bin/sh
# Usage: <target_image> <target_realseg> <candidate_realseg>

source DBConfig.sh

score=`./PlaceImage/build/PlaceImage $2 $3`

# insert in db
echo "insert into scores(target_image, target_tile, candidate_image, score) VALUES('$1', '$2', '$3', $score)" | mysql -u LTPM LTPM
