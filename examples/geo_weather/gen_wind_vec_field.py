#!/usr/bin/env python

import os
import csv
import numpy as np
from math import pow
from math import sqrt
import matplotlib.pyplot as plt

test_data_filename = 'testdata/vic_usgs/wind_20170514150000.csv'

tl_point = {}
tl_point['lat'] = -36.9998611
tl_point['lon'] = 140.9998611
br_point = {}
br_point['lat'] = -38.9998611
br_point['lon'] = 143.9998611
cell_size = 4000

def calWindDir(str):

    if str == "CALM":
        return np.asarray([0, 0, 0])

    winddir = np.asarray([0, 0, 0])
    for i in range(len(str)):
        c = str[i]
        if c == 'E':
            winddir += np.asarray([1, 0, 0])
        elif c == 'W':
            winddir += np.asarray([-1, 0, 0])
        elif c == 'N':
            winddir += np.asarray([0, -1, 0])
        elif c == 'S':
            winddir += np.asarray([0, 1, 0])

    norm=np.linalg.norm(winddir, ord=2)
    if(norm == 0):
        return np.asarray([0, 0, 0])
    return winddir / norm


def pointValue(x, z, power, smoothing, data_points):
    nominator=0
    denominator=0
    for i in range(0,len(data_points)):
        xv = data_points[i][0]
        zv = data_points[i][1]
        v = data_points[i][2]
        dist = sqrt((x-xv)*(x-xv)+(z-zv)*(z-zv)+smoothing*smoothing)
        #If the point is really close to one of the data points, return the data point value to avoid singularities
        if(dist<0.0000000001):
            return v
        nominator = nominator + (v / pow(dist,power))
        denominator = denominator + (1 / pow(dist,power))
    #Return NODATA if the denominator is zero
    if denominator > 0:
        value = nominator/denominator
    else:
        value = -9999
    return value



def invDist(data_points, XV, ZV, power=2, smoothing=0):

    xsize = len(XV[0])
    zsize = len(XV)
    valuesGrid = np.zeros((zsize, xsize))
    for x in range(0,xsize):
        for z in range(0,zsize):
            valuesGrid[z][x] = pointValue(XV[z][x], ZV[z][x], power, smoothing, data_points)
            #print z, x, valuesGrid[z][x]
    return valuesGrid


# generate data points
data_points_windx = np.zeros( (15, 3) )
data_points_windz = np.zeros( (15, 3) )
with open(test_data_filename, 'rb') as csvfile:
    stations = csv.DictReader(csvfile)
    ind = 0
    for station in stations:
        print station
        lat = float(station["lat"])
        lon = float(station["lon"])
        x = (lon - tl_point['lon']) * 36470 / 1.0002777999999921
        z = -1 * (lat - tl_point['lat']) * 36470 / 1.0002777999999921
        wind = float(station["wind_spd_kmh"]) * calWindDir(station["wind_dir"])

        data_points_windx[ind][0] = x
        data_points_windx[ind][1] = z
        data_points_windx[ind][2] = wind[0]

        data_points_windz[ind][0] = x
        data_points_windz[ind][1] = z
        data_points_windz[ind][2] = wind[1]

        ind += 1

print 'data_points_windx: ', data_points_windx
print 'data_points_windz: ', data_points_windz

# generate grid
br_x = (br_point['lon'] - tl_point['lon']) * 36470 / 1.0002777999999921
br_z = -1 * (br_point['lat'] - tl_point['lat']) * 36470 / 1.0002777999999921
print 'br point: ', br_x, br_z

grid_size_x = int(br_x / cell_size)
grid_size_z = int(br_z / cell_size)
ti_x = np.linspace(0, br_x, grid_size_x, endpoint=True)
ti_z = np.linspace(0, br_z, grid_size_z, endpoint=True)
XV, ZV = np.meshgrid(ti_x, ti_z)

power = 2
valuesGridX = invDist(data_points_windx, XV, ZV, power)
#np.savetxt('testdata/vic_usgs/wind_20170514150000_x.txt', np.asarray(valuesGridX))

valuesGridZ = invDist(data_points_windz, XV, ZV, power)
#np.savetxt('testdata/vic_usgs/wind_20170514150000_z.txt', np.asarray(valuesGridZ))

# draw vectors
numvecs = grid_size_x * grid_size_z
vectors = np.zeros( (numvecs, 4) )
ind = 0
for x in range(0, grid_size_x):
    for z in range(0, grid_size_z):
        vectors[ind][0] = XV[z][x]
        vectors[ind][1] = ZV[z][x]
        vectors[ind][2] = valuesGridX[z][x]
        vectors[ind][3] = valuesGridZ[z][x]
        ind += 1

#print vectors

for i in range(15):
    print 'vectorField->addControlPoint(', data_points_windx[i, 0], ',', data_points_windx[i, 1], ',', data_points_windx[i, 2], ',', data_points_windz[i, 2], ');'

# draw
plt.figure()
ax = plt.gca()
ax.quiver(data_points_windx[:, 0], data_points_windx[:, 1], data_points_windx[:, 2], data_points_windz[:, 2])
ax.quiver(vectors[:, 0], vectors[:, 1], vectors[:, 2], vectors[:, 3])
ax.set_xlim([0, br_x])
ax.set_ylim([0, br_z])
plt.draw()
plt.show()


"""
X = vectors[:, 0]
Y = vectors[:, 1]
U = vectors[:, 2]
V = vectors[:, 3]
plt.figure()
ax = plt.gca()
ax.quiver(X, Y, U, V)
ax.set_xlim([0, br_x])
ax.set_ylim([0, br_z])
plt.draw()
plt.show()
"""

print 'Done!'
