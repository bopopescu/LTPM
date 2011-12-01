#!/bin/sh

./Libraries/BSE-1.2/segment -image $1 -segfile $1.$2.seg -numsuperpixels $2
./VisualizeSegmentation/build/LTPM $1 $1.$2.seg
