#/bin/env python

import simplejson
import urllib

# rough elevation values to use with vector field

ELEVATION_BASE_URL = 'https://maps.googleapis.com/maps/api/elevation/json'

tl_point = {}
tl_point['lat'] = -36.9998611
tl_point['lon'] = 140.9998611
br_point = {}
br_point['lat'] = -38.9998611
br_point['lon'] = 143.9998611

nrows = 300
ncols = 500

dlat = (br_point['lat'] - tl_point['lat']) / nrows
dlon = (br_point['lon'] - tl_point['lon']) / ncols

print dlat, dlon

out_filename = 'vic_elevation.txt'

with open(out_filename, 'wt') as fout:
    fout.write(str(nrows) + ' ' + str(ncols) + '\n')

    for r in range(nrows):

        print r
        for chunk in range(5): # devide 500 to 5 chunks
            cstart = chunk * (ncols/5)
            cend = (chunk+1) * (ncols/5)
            print cstart, cend
            locs_str = ''
            for c in range(cstart, cend):
                #print c
                lat = tl_point['lat'] + r * dlat
                lon = tl_point['lon'] + c * dlon
                locs_str = locs_str + str(lat) + ',' + str(lon)
                if c < cend - 1:
                    locs_str = locs_str + '|'

            #print locs_str
            url = ELEVATION_BASE_URL + '?locations=' + locs_str + '&key=AIzaSyCtNmamMwHaF6QIAe2TICKUtw3Na8E2oPo'
            #print url
            response = simplejson.load(urllib.urlopen(url))

            for resultset in response['results']:
                val = resultset['elevation']
                if val < 0:
                    val = 0
                fout.write(str(val) + ' ')

        fout.write('\n')

