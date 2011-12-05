#!/bin/sh

# $1 takes in a path to an image, 
# $2 x num of tiles, 
# $3 y num of tiles 
# $4 number of superpixels
# $5 output dest for tiles.jpg and .seg files, 
# $6 output path for realseg files

python uniformCrop.py $1 $2 $3 $5
./BatchSegment.sh $5 $4
./BatchGenerateRealSegFiles.sh $5 $4 $6
