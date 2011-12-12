#!/usr/bin/env python

import urllib
import os, sys
from PIL import Image, ImageOps
import MySQLdb

if len(sys.argv) < 4:
	print "Usage: "+sys.argv[0]+" <LTPM_name> <rows> <cols>"
	exit(0)
LTPMName = sys.argv[1]
rows = int(sys.argv[2])
cols = int(sys.argv[3])

#basename = os.path.basename(sys.argv[1])
#LTPMName, ext = os.path.splitext(basename)

db=MySQLdb.connect(user="LTPM", db="LTPM")
c=db.cursor()

originalTargetImage = Image.open("Data/"+LTPMName+"/"+"Original.jpg")
width  = originalTargetImage.size[0]
height = originalTargetImage.size[1]
LTPMImage = Image.new("RGB", originalTargetImage.size)


#destPath = sys.argv[4]

cellWidth = originalTargetImage.size[0] / cols
cellHeight = originalTargetImage.size[1] / rows


for row in range(0,rows):
	for col in range(0,cols):
		#target_tile = LTPMName+'_'+str(row)+'_'+str(col)+'.realseg'
		target_tile = "Data/Images/realsegFiles/grid-adam_0_0.jpg.8.realseg"
		c.execute("""SELECT candidate_name, score FROM scores WHERE score <= 1.0 AND LTPM_name = %s AND target_row = %s AND target_col = %s ORDER BY score DESC LIMIT 1""", (LTPMName, row, col,))
		result = c.fetchone()
		if result != None:
			winningCandidateURL = result[0]
			score = result[1]
			x = col*cellWidth
			y = row*cellHeight

			# get candidate flickr image
			(filename, headers) = urllib.urlretrieve(winningCandidateURL)
			flickrImage = Image.open(filename)

			# fit candidate image to tile size
			candidateImage = ImageOps.fit(flickrImage, (cellWidth, cellHeight), Image.ANTIALIAS, 0, (0.5, 0.5))
			os.remove(filename)


			print "Pasting " + winningCandidateURL + " with score " + str(score) + " at ("+str(x)+", "+str(y)+")"
			LTPMImage.paste(candidateImage, (x, y))


c.close()
db.close()


LTPMImage.save("Data/"+LTPMName+"/"+LTPMName+"_LTPM.png")

sideBySide = Image.new("RGB", (width*2, height))
sideBySide.paste(LTPMImage, (0, 0))

blend = Image.blend(LTPMImage, originalTargetImage, 0.5)
sideBySide.paste(blend, (width, 0))




sideBySide.show()

exit(0)
