from mrjob.job import MRJob
from boto.s3.connection import S3Connection
from boto.s3.key import Key
from boto.s3.bucket import Bucket
import urllib

class DownloadFlickrImages(MRJob):

    def mapper(self, _, line):

        photo_label = "photo:"
        if line[0:len(photo_label)] == photo_label:
            (label, id, secret, server) = line.split(' ')
            unique = "candidate_flickr_" + server + "_" + id + "_" + secret + "_m.jpg"
            url = 'http://static.flickr.com/' + server + "/" + id + "_" + secret + '_m.jpg'

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
    DownloadFlickrImages.run()
