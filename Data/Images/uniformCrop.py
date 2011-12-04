#! usr/bin/python

import os, sys
import Image

# take in image path, number of cells in the x, number of cells in y



im = Image.open(sys.argv[1])
divWidth = int(sys.argv[2])
divHeight = int(sys.argv[3])

cellWidth = im.size[0] / divWidth
cellHeight = im.size[1] / divHeight

file, ext = os.path.splitext(sys.argv[1])

for row in range(0,divHeight):
	for col in range(0,divWidth):
		x = col*cellWidth
		y = row*cellHeight
		croppedIm = im.crop((x,y,x+cellWidth,y+cellHeight))
		croppedIm.save(file + "_" + str(row) + "_" + str(col) + ext, "JPEG")

print("Cell Width: " + str(cellWidth) + " Cell Height: " + str(cellHeight))

#im.show()

print(sys.argv[1])
