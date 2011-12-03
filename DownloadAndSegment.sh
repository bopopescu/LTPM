#!/bin/sh

cd Data/Images
wget $1
cd -
./SegmentAndVisualize.sh Data/Images/`basename $1` $2
