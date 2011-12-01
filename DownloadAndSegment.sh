#!/bin/sh

cd Data/Images
wget $1
cd -
./Segment.sh Data/Images/`basename $1` $2
