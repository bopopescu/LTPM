#!/usr/bin/env python

import os, sys
import Image
import MySQLdb

if len(sys.argv) < 4:
	print "Usage: "+sys.argv[0]+" <LTPM_name> <rows> <cols>"

db=MySQLdb.connect(username="LTPM",db="LTPM")
c=db.cursor()

originalTargetImage = Image.open(sys.argv[1])
width  = originalTargetImage.size[0]
height = originalTargetImage.size[1]
LTPMImage = Image.new("RGB", originalTargetImage.size)

rows = sys.argv[2]
cols = sys.argv[3]

destPath = sys.argv[4]

cellWidth = originalTargetImage.size[0] / cols
cellHeight = originalTargetImage.size[1] / rows

basename = os.path.basename(sys.argv[1])
file, ext = os.path.splitext(basename)

for row in range(0,rows):
	for col in range(0,cols):
		c.execute("""SELECT candidate_image FROM scores WHERE target_tile = %s ORDER BY score DESC LIMIT 1""", (target_tile,))
		x = col*cellWidth
		y = row*cellHeight
		candidateImage = blah
		LTPMImage.paste(candidateImage, (x, y))

im.save("Data/LTPMs/"+LTPMName+".png")

exit(0)
