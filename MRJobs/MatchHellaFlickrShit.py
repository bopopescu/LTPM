#!/usr/bin/env python

from mrjob.job import MRJob
from boto.s3.connection import S3Connection
from boto.s3.key import Key
from boto.s3.bucket import Bucket
import urllib
import subprocess, shlex
import os
from PIL import Image, ImageOps
import MySQLdb

number_of_superpixels = 32

class MatchHellaFlickrShit(MRJob):

    def get_target_seg_path(self, row, col):
        return self.options.LTPM_root + '/Data/' + self.options.target_image_name + '/TileRealSegs/Original_' + str(row) + '_' + str(col) + '.jpg.' + str(number_of_superpixels) + '.realseg'
    
    def get_candidate_seg_path(self, candidate_key):
        return 'Data/DBImages/RealSegs/' + candidate_key + '.' + number_of_superpixels + '.realseg'

    def configure_options(self):
        super(MatchHellaFlickrShit, self).configure_options()
        self.add_passthrough_option('--target-image-name', type='str', default='Untitled_Photomosaic', help='Name of target image to match against')
        self.add_passthrough_option('--rows', type='int')
        self.add_passthrough_option('--cols', type='int')
        self.add_passthrough_option('--tile-width', type='int')
        self.add_passthrough_option('--tile-height', type='int')
        self.add_passthrough_option('--LTPM-root', type='str') # abstract this shit

#    def job_runner_kwargs(self):
#        args = dict()
#        args['upload_files'] = ['Libraries/BSE-1.2/segment']
#        return args

    def mapper(self, _, line):

        photo_label = "photo:"
        if line[0:len(photo_label)] == photo_label:
            (label, id, secret, server) = line.split(' ')
            unique = "candidate_flickr_" + server + "_" + id + "_" + secret + "_m.jpg"
            url = 'http://static.flickr.com/' + server + "/" + id + "_" + secret + '_m.jpg'


            (filename, headers) = urllib.urlretrieve(url)

            width  = self.options.tile_width
            height = self.options.tile_height

            flickrImage = Image.open(filename)
            candidateImage = ImageOps.fit(flickrImage, (width, height), Image.ANTIALIAS, 0, (0.5, 0.5))
            os.remove(filename)
            candidateImage.save(filename, "JPEG")

            #imageName=os.path.basename(sys.argv[1])
            #dirName=`dirname $1`

            #echo Finding superpixels ...
            berkeleySegFile = '/tmp/' + unique + '.bse'
            #subprocess.call(['segment', '-image', filename, '-segfile', berkeleySegFile, '-numsuperpixels', str(number_of_superpixels)])
            subprocess.call(['/home/stolrsky/Desktop/LTPM/Libraries/BSE-1.2/segment', '-image', filename, '-segfile', berkeleySegFile, '-numsuperpixels', str(number_of_superpixels)])

            #echo Merging superpixels ...
            segmentationString = subprocess.check_output(['/home/stolrsky/Desktop/LTPM/SuperPixelsToSegmentation/build/SuperPixelsToSegmentation', filename, berkeleySegFile, '0'])
            #print segmentationString
            segmentationFilePath = '/tmp/' + unique + '.realseg'
            segmentationFile = open(segmentationFilePath, 'w')
            segmentationFile.write(segmentationString)
            segmentationFile.close()

            

            os.remove(berkeleySegFile)
            os.remove(filename)


            db = MySQLdb.connect(user="LTPM", db="LTPM")
            c = db.cursor()


            LTPM_name = self.options.target_image_name
            candidate_name = unique
            rows = self.options.rows
            cols = self.options.cols
 
            for target_row in range(rows):
                for target_col in range(cols):
                    target_realseg_path = self.get_target_seg_path(target_row, target_col)
                    
                    call = '/home/stolrsky/Desktop/LTPM/PlaceImage/build/PlaceImage ' + target_realseg_path + ' ' + segmentationFilePath
                    #print call
                    #score = subprocess.check_output(['/home/stolrsky/Desktop/LTPM/PlaceImage/build/PlaceImage', target_realseg_path, segmentationFilePath])
                    score = subprocess.check_output(shlex.split(call))
                    #print "scored " + unique + ' vs. ' + LTPM_name + ':' + str(target_row) + ':' + str(target_col)
                    #print score
                    #print " "
                    # insert in db
                    c.execute("insert into scores(LTPM_name, target_row, target_col, candidate_name, score) VALUES('"+LTPM_name+"', '"+str(target_row)+"', '"+str(target_col)+"', '"+url+"', "+str(score)+")")


            c.close()
            
            os.remove(segmentationFilePath)

            #(filename, headers) = urllib.urlretrieve(url, '/tmp/' + unique)
            #s3conn = S3Connection()
            #LTPM = Bucket(s3conn, 'ltpm')
            #image = Key(LTPM)
            #image.key = unique
            #image.set_contents_from_filename(filename)
            yield(unique, url)

    #def combiner(self, word, counts):
    #    yield (image_key, sum(counts))

    #def reducer(self, word, counts):
    #    yield (word, sum(counts))


if __name__ == '__main__':
    MatchHellaFlickrShit.run()
