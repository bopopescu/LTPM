#!/bin/sh

# $1 takes in a path to an image
# $2 rows
# $3 columns 
# $4 number of superpixels

tile_image_dir=`dirname $1`/TileImages
mkdir $tile_image_dir

berkeley_segs_dir=`dirname $1`/TileBerkeleySegs
mkdir $berkeley_segs_dir

realsegs_dir=`dirname $1`/TileRealSegs
mkdir $realsegs_dir

python uniformCrop.py $1 $2 $3 $tile_image_dir
./BatchSegment.sh $tile_image_dir $4 $berkeley_segs_dir
./BatchGenerateRealSegFiles.sh $tile_image_dir $berkeley_segs_dir $4 $realsegs_dir
