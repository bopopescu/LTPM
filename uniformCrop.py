#!/usr/bin/env python

import os, sys
import Image

# take in image path, number of rows, number of cols, and output path

im = Image.open(sys.argv[1])
divWidth = int(sys.argv[3])
divHeight = int(sys.argv[2])
destPath = sys.argv[4]

cellWidth = im.size[0] / divWidth
cellHeight = im.size[1] / divHeight

basename = os.path.basename(sys.argv[1])
file, ext = os.path.splitext(basename)

for row in range(0,divHeight):
	for col in range(0,divWidth):
		x = col*cellWidth
		y = row*cellHeight
		croppedIm = im.crop((x,y,x+cellWidth,y+cellHeight))
		print(file + "_" + str(row) + "_" + str(col) + ext)
		croppedIm.save(os.path.join(destPath,file + "_" + str(row) + "_" + str(col) + '.jpg'), "JPEG")

print("Cell Width: " + str(cellWidth) + " Cell Height: " + str(cellHeight))

#im.show()

print(sys.argv[1])
