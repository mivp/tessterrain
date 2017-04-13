import os
import sys
import numpy as np
import scipy.ndimage
import scipy.misc
import osgeo.gdal
import png

if len(sys.argv) != 2:
    sys.exit('ERROR: invalid arguments. Usage: ' + sys.argv[0] + ' demfile')

input_filename = sys.argv[1]
#input_filename = 'testdata/mel_small/mel_small.tif'
#input_filename = 'testdata/data/drycreek.tif'
#input_filename = 'testdata/sw_lr/sw_lr.tif'
print input_filename

input_dir = os.path.dirname(input_filename)
input_base = os.path.basename(input_filename)
name = os.path.splitext(input_base)[0]

output_dir = input_dir # + '/' + name
print 'output_dir: ', output_dir

# create output dir
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

dataset = osgeo.gdal.Open(input_filename)
gt = dataset.GetGeoTransform()
print gt

# get raster band
try:
    srcband = dataset.GetRasterBand(1)
except RuntimeError, e:
    # for example, try GetRasterBand(10)
    print 'Band ( %i ) not found' % band_num
    print e
    sys.exit(1)

data = srcband.ReadAsArray()
print 'data shape ', data.shape

tmp = np.zeros((data.shape), dtype=np.float32)
np.copyto(tmp, data)
tmp[tmp < 0] = data.max()
height_min = tmp.min()
tmp = None

data[data < 0] = height_min

# generate config file

hightmap_cfg = output_dir + '/' + name + '.ini'
hightmap_img = output_dir + '/' + name + '.png'
print 'save config to file ', hightmap_cfg
with open(hightmap_cfg, 'wt') as f:
    f.write('[file]\n')
    f.write('heightmap = ' + hightmap_img + '\n\n')

    f.write('[topleft]\n')
    f.write('left = 0\n')
    f.write('top = 0\n\n')

    f.write('[horizontalres]\n')
    f.write('wres = ' + str(gt[1]) + '\n')
    f.write('hres = ' + str(gt[5]) + '\n\n')

    f.write('[heightrange]\n')
    f.write('min = ' + str(data.min()) + '\n')
    f.write('max = ' + str(data.max()) + '\n')

# save data to image
width = data.shape[1]
height = data.shape[0]
if width % 64 != 0:
    width = (int(width / 64) + 1) * 64
if height % 64 != 0:
    height = (int(height / 64) + 1) * 64
data2 = np.ones((height, width), dtype=np.float32) * height_min
data2[:data.shape[0],:data.shape[1]] = data

data2 = 255 * (data2 - height_min) / (data.max() -height_min)
data = data2.astype(np.uint8)

print 'save data to file ', hightmap_img
#png.from_array(data, 'L').save(hightmap_img)
f = open(hightmap_img, 'wb')
w = png.Writer(width=data.shape[1], height=data.shape[0], greyscale=True, bitdepth=8)
#data2list = data.reshape(-1, data.shape[1]).tolist()
w.write(f, data)

sys.exit(0)


output_filename = input_filename + '.txt'






# print out metadata
print src_ds.GetMetadata()

# get number of raster bands
print "[ RASTER BAND COUNT ]: ", src_ds.RasterCount

# project ref
print src_ds.GetProjectionRef()

# driver
dri = src_ds.GetDriver()
print "[driver desc: ]: ", dri.GetDescription()





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
