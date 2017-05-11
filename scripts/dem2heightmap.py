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
print_config = False
#input_filename = 'testdata/mel_small/mel_small.tif'
#input_filename = 'testdata/data/drycreek.tif'
#input_filename = 'testdata/sw_lr/sw_lr.tif'
print input_filename

if print_config:
    print 'Print config file: YES'
else:
    print 'Print config file: NO'

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
height_max = tmp.max()
tmp = None

data[data < 0] = height_min

# generate config file

hightmap_cfg = output_dir + '/' + name + '.ini'
hightmap_img8 = output_dir + '/' + name + '_8.png'
hightmap_img16 = output_dir + '/' + name + '_16.ter'
texture_img = output_dir + '/' + name + '_sat.png'

if print_config:
    print 'save config to file ', hightmap_cfg
    with open(hightmap_cfg, 'wt') as f:
        f.write('[file]\n')
        f.write('heightmap = ' + hightmap_img8 + '\n')
        f.write('texture = ' + texture_img + '\n\n')

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
zoom_w = float(width) / float(data.shape[1])
zoom_h = float(height) / float(data.shape[0]) 

# padding
#data2 = np.ones((height, width), dtype=np.float32) * height_min
#data2[:data.shape[0],:data.shape[1]] = data

# resize
print 'new width: ', width, 'height', height
data2 = scipy.ndimage.zoom(data, zoom=(zoom_w, zoom_h), order=1)
print data2.shape

data8 = np.zeros(data2.shape, np.float32)
data16 = np.zeros(data2.shape, np.float32)
np.copyto(data8, data2)
np.copyto(data16, data2)

data8 = 255 * (data8 - height_min) / (height_max - height_min)
data8 = data8.astype(np.uint8)

data16 = data16.astype(np.uint16)

print 'save 8-bit data to image file ', hightmap_img8
f8 = open(hightmap_img8, 'wb')
w8 = png.Writer(width=data8.shape[1], height=data8.shape[0], greyscale=True, bitdepth=8)
w8.write(f8, data8)

"""
print 'save 16-bit data to txt file ', hightmap_img16
with open(hightmap_img16, 'wt') as f16:
    f16.write(str(data16.shape[0]) + ' ' + str(data16.shape[1]) + '\n')
    for r in range(data16.shape[0]):
        for c in range(data16.shape[1]):
            f16.write(str(data16[r][c]) + ' ')
        f16.write('\n')
"""

"""
f16 = open(hightmap_img16, 'wb')
w16 = png.Writer(width=data16.shape[1], height=data16.shape[0], greyscale=True, bitdepth=16)
w16.write(f16, data16)
"""

print 'Done!'
