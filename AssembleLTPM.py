#!/usr/bin/env python

import os, sys
import Image
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
		c.execute("""SELECT candidate_name FROM scores WHERE LTPM_name = %s AND target_row = %s AND target_col = %s ORDER BY score DESC LIMIT 1""", (LTPMName, row, col,))
		result = c.fetchone()
		if result != None:
			winningCandidateName = result[0]
			x = col*cellWidth
			y = row*cellHeight
			print "Pasting " + winningCandidateName + " at ("+str(x)+", "+str(y)+")"
			winningCandidateImage = Image.open("Data/DBImages/Images/"+winningCandidateName)
			LTPMImage.paste(winningCandidateImage, (x, y))

c.close()
db.close()


LTPMImage.save("Data/"+LTPMName+"/"+LTPMName+"_LTPM.png")
LTPMImage.show()

exit(0)
