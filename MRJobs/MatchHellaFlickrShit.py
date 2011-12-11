#!/usr/bin/env python

from os import remove
from mrjob.job import MRJob
from boto.s3.connection import S3Connection
from boto.s3.key import Key
from boto.s3.bucket import Bucket
import urllib


number_of_superpixels = 32

class MatchHellaFlickrShit(MRJob):

    def get_target_seg_path(self, row, col):
        return 'Data/' + self.options.target_image_name + '/TileRealSegs/Original_' + row + '_' + col + '.jpg.' + number_of_superpixels + '.realseg'
    
    def get_candidate_seg_path(self, candidate_key):
        return 'Data/DBImages/RealSegs/' + candidate_key + '.' + number_of_superpixels + '.realseg'

    def configure_options(self):
        super(MatchHellaFlickrShit, self).configure_options()
        self.add_passthrough_option('--target-image-name', type='str', default='Untitled_Photomosaic', help='Name of target image to match against')

    def mapper(self, _, line):


        (server, id, secret) = line[0:-1].split(' ')
        unique = "candidate_flickr_" + server + "_" + id + "_" + secret + "_m.jpg"
        url = 'http://static.flickr.com/' + server + "/" + id + "_" + secret + '_m.jpg'



        """
        (filename, headers) = urllib.urlretrieve(url)


        #imageName=os.path.basename(sys.argv[1])
        #dirName=`dirname $1`

        #echo Finding superpixels ...
        berkeleySegFile = '/tmp/' + unique + '.bse'
        subprocess.call(['Libraries/BSE-1.2/segment', '-image', filename, '-segfile', berkeleySegFile, '-numsuperpixels', number_of_superpixels])

        #echo Merging superpixels ...
        segmentationString = subprocess.check_output(['SuperPixelsToSegmentation/build/SuperPixelsToSegmentation', filename, berkeleySegFile, '0'])

        remove(berkeleySegFile)

        TODO: save seg to local file, save seg to S3


        LTPM_name = self.options.target_image_name
        candidate_name = unique
        number_of_rows = self.options.rows
        number_of_cols = self.options.cols

        for target_row in range(rows):
            for target_col in range(cols):
                target_realseg_path = self.get_target_seg_path(target_row, target_col)
                score = subprocess.check_output(['PlaceImage/build/PlaceImage', target_realseg_path, candidate_realseg_path])

                # insert in db
                #echo "insert into scores(LTPM_name, target_row, target_col, candidate_name, score) VALUES('$LTPM_name', '$target_row', '$target_col', '$candidate_name', $score)" | mysql -u LTPM LTPM

        """

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
