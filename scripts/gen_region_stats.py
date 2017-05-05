#/usr/bin/env python
import numpy
import csv
import sys
import utm

image_size = [3601, 3601]

# input: csv file containing regions
if len(sys.argv) != 2:
    sys.exit('ERROR: invalid arguments. Usage: ' + sys.argv[0] + ' csvfile')

input_filename = sys.argv[1]

ref_point = ()

with open(input_filename, 'rb') as csvfile:
    regions = csv.DictReader(csvfile)
    for region in regions:
        print region
        ref_point = utm.from_latlon(float(region["tl_lat"]), float(region["tl_lon"]))
        break

print 'ref_point', ref_point

with open(input_filename, 'rb') as csvfile:
    regions = csv.DictReader(csvfile)
    for region in regions:
        print '\n',region
        tl = utm.from_latlon(float(region["tl_lat"]), float(region["tl_lon"]))
        br = utm.from_latlon(float(region["br_lat"]), float(region["br_lon"]))
        print 'Region: ', region["name"]
        print 'TopLeft: ', tl
        print 'BottomRight: ', br
        dX = br[0]-tl[0]
        dY = br[1]-tl[1]
        print 'dX: ', dX, 'dY', dY
        resX = dX / (image_size[0] - 1)
        resY = dY / (image_size[1] - 1)
        print 'resX: ', resX, 'resY: ', resY
        posX = tl[0] - ref_point[0]
        posY = tl[1] - ref_point[1]
        print 'posX: ', posX, 'posY: ', posY