import numpy as np
from osgeo import gdal, osr

ref_tif = "/Users/toand/git/mivp/terrain/terrain/testdata/south_west/west_1sh/west_1sh.tif"

dataset = gdal.Open(ref_tif)
#gt = dataset.GetGeoTransform()
#pr = dataset.GetProjectionRef()

#print gt, '\n'
#print pr, '\n'

srs = osr.SpatialReference()
srs.ImportFromWkt(dataset.GetProjectionRef())
#srs.ImportFromProj4("+proj=lcc +lat_1=-18 +lat_2=-36 +lat_0=0 +lon_0=134 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs ")

srsLatLong = srs.CloneGeogCS()
ct = osr.CoordinateTransformation(srsLatLong, srs)

topleft = ct.TransformPoint(141.2, -37.2)

stations = np.array([
[142.98,	-37.28],
[143.2,	-37.23],
[141.54,	-38.43],
[143.51,	-38.86],
[141.33,	-37.58],
[141.26,	-37.92],
[142.06,	-37.65],
[142.77,	-38.07],
[143.79,	-38.23],
[142.6,	-37.3],
[142.23,	-38.39],
[141.47,	-38.31],
[141.61,	-38.34],
[142.45,	-38.29],
[142.94,	-37.71]
])

print stations.shape

for i in range(stations.shape[0]):
	p = ct.TransformPoint(stations[i][0], stations[i][1])
	print p[1] - topleft[1]

"""
topleft = ct1.TransformPoint(141.2, -37.2)
print topleft

topmiddle = ct1.TransformPoint(142.525, -37.2)
print topmiddle

print topmiddle[0] - topleft[0], topmiddle[1] - topleft[1]
"""