#!/usr/bin/env python

import os, sys
import Image
import MySQLdb

if len(sys.argv) < 4:
	print "Usage: "+sys.argv[0]+" <LTPM_name> <rows> <cols>"
	exit(0)
targetImagePath = sys.argv[1]
rows = int(sys.argv[2])
cols = int(sys.argv[3])

basename = os.path.basename(sys.argv[1])
LTPMName, ext = os.path.splitext(basename)

db=MySQLdb.connect(user="LTPM", db="LTPM")
c=db.cursor()

originalTargetImage = Image.open(targetImagePath)
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
		c.execute("""SELECT candidate_image FROM scores WHERE target_tile = %s ORDER BY score DESC LIMIT 1""", (target_tile,))
		winning_candidate_image = c.fetchone()[0]
		x = col*cellWidth
		y = row*cellHeight
		print "Pasting " + winning_candidate_image + " at ("+str(x)+", "+str(y)+")"
		#candidateImage = blah
		#LTPMImage.paste(candidateImage, (x, y))

c.close()
db.close()


LTPMImage.save("Data/"+LTPMName+"/"+LTPMName+"_LTPM.png")

exit(0)
