# check gdal
import sys
import numpy as np
import scipy.ndimage

try:
    from osgeo import ogr, osr, gdal, gdalnumeric
except:
    sys.exit('ERROR: cannot find GDAL/OGR modules')

#input_filename = 'testdata/mel_small/mel_small.tif'
#input_filename = 'testdata/data/drycreek.tif'
input_filename = 'testdata/sw_lr/sw_lr.tif'
print input_filename

output_filename = input_filename + '.txt'

version_num = int(gdal.VersionInfo('VERSION_NUM'))
print 'gdal version: ' + str(version_num)
if version_num < 1100000:
    sys.exit('ERROR: Python bindings of GDAL 1.10 or later required')

# Enable GDAL/OGR exceptions
gdal.UseExceptions()

# open a dataset
try:
    src_ds = gdal.Open( input_filename )
except RuntimeError, e:
    print 'Unable to open INPUT.tif'
    print e
    sys.exit(1)


# print out metadata
print src_ds.GetMetadata()

# get number of raster bands
print "[ RASTER BAND COUNT ]: ", src_ds.RasterCount

# project ref
print src_ds.GetProjectionRef()

# driver
dri = src_ds.GetDriver()
print "[driver desc: ]: ", dri.GetDescription()

# projection
proj = src_ds.GetProjectionRef()
print "[projection]: ", proj

# get raster band
try:
    srcband = src_ds.GetRasterBand(1)
except RuntimeError, e:
    # for example, try GetRasterBand(10)
    print 'Band ( %i ) not found' % band_num
    print e
    sys.exit(1)

stats = srcband.GetStatistics( True, True )
print "[ STATS ] =  Minimum=%.3f, Maximum=%.3f, Mean=%.3f, StdDev=%.3f" % (stats[0], stats[1], stats[2], stats[3])
"""
# print out band information
print "[ NO DATA VALUE ] = ", srcband.GetNoDataValue()
print "[ MIN ] = ", srcband.GetMinimum()
print "[ MAX ] = ", srcband.GetMaximum()
print "[ SCALE ] = ", srcband.GetScale()
print "[ UNIT TYPE ] = ", srcband.GetUnitType()
ctable = srcband.GetColorTable()

if ctable is None:
    print 'No ColorTable found'
    sys.exit(1)

print "[ COLOR TABLE COUNT ] = ", ctable.GetCount()
for i in range( 0, ctable.GetCount() ):
    entry = ctable.GetColorEntry( i )
    if not entry:
        continue
    print "[ COLOR ENTRY RGB ] = ", ctable.GetColorEntryAsRGB( i, entry )
"""

geoTrans = src_ds.GetGeoTransform()
print "[GeoTransform]: ", geoTrans

# read band data
#scanline = srcband.ReadRaster( 0, 0, srcband.XSize, 1, srcband.XSize, 1, gdal.GDT_Float32 )

# Another way to load source data as a gdalnumeric array
try:
    srcArray = gdalnumeric.LoadFile(input_filename)

except RuntimeError, e:
    print 'Unable to open INPUT.tif'
    print e
    sys.exit(1)

# easy way
data = srcband.ReadAsArray()
low_values_indices = data <= 0
data[low_values_indices] = 0
data = scipy.ndimage.zoom(data, 0.5, order=1) #bilinear interpolation 0: nearest 2: cubic
print 'Size: ', data.shape
print 'Res: ', geoTrans[1]*2, ' ', geoTrans[5]*2

np.savetxt(output_filename, data)

# close dataset
src_ds = None

print 'Done!'
