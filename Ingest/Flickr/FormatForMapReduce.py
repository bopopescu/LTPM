import sys

for line in sys.stdin:
   photo_label = "photo:"
   if line[0:len(photo_label)] == photo_label:
      (label, id, secret, server) = line[0:-1].split(' ')
      #unique = "candidate_flickr_" + server + "_" + id + "_" + secret + "_m.jpg"
      #url = 'http://static.flickr.com/' + server + "/" + id + "_" + secret + '_m.jpg'
      print server + ' ' + id + ' ' + secret

#   if line == '\n':
#      sys.stdout.write('\n')
#   else:
#      sys.stdout.write(line[0:-1] + ';')

       






