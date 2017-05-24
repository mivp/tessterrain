# Tessellation terrain

Desktop/CAVE2 terrain rendering using tessellation shaders.

**Contact**: Toan Nguyen ([http://monash.edu/mivp](http://monash.edu/mivp))

## References:
- [DirectX 11 terrain tessellation](https://developer.nvidia.com/sites/default/files/akamai/gamedev/files/sdk/11/TerrainTessellation_WhitePaper.pdf)
- [Solid wireframe](http://developer.download.nvidia.com/SDK/10.5/direct3d/Source/SolidWireframe/Doc/SolidWireframe.pdf)
- [QT5 and OpenGL terrain tessellation example](http://www.kdab.com/opengl-in-qt-5-1-part-5/)

## Prerequisites

- OpenGL version >= 4.0


## Omegalib module

Tested with Omegalib 13.1

Compile:
```
cd tessterrain
mkdir build
cd build
cmake ..
make
```

Run with python script:
```
import tessterrain

tt = tessterrain.initialize()
tt.addTerrain("path/to/inifile1.ini")
tt.addTerrain("path/to/inifile2.ini")
```

Functions which can be used in python script:
```
moveTo(index, x, y, z): move a specific terrain (specified by terrain index) to a position
nextDisplayMode(): change display mode
displayInfo(): display information of terrains
toggleFog(): toggle fog in scene
```

## Standalone app

Tested with MacOS 10.12.4

```
cd tessterrain/app
mkdir build
cd build
cmake .. (or cmake .. -G Xcode to build XCode project)
make (or run build in XCode)
```

Run
```
./tessterrain [path/to/inifile.ini]
```

Keys

- N: go to next display mode
- I: print out camera information

## Configuration file (INI)

Example

```
[file]
; absolute path or relative path from running directory
heightmap = testdata/tess/heightmap-1024x1024.png	; height map 8-bit png image
texture = testdata/tess/grass.png			; (optional) texture image

[horizontalres]
; (optional) the distance (e.g. metter) between 2 pixels in horizontal and 
; vertical directions of the heightmap. Defaults to [1, 1]
wres = 20
hres = 20

[heightrange]
; (optional) min and max height of the heightmap. Defaults to [0, 1, 1]
min = 0
max = 200
scale = 1

[verticalscale]
; (optional) display range. If [verticalscale] is not specified, [heightrange] will be used instead
min = 0
max = 220

[moveto]
; (optional) move terain to position. Defautls to (0, 0, 0)
x = 0
y = 0
z = 0
```

## Heightmap

Currently support heightmaps stored in a png file (red channel will be used) or a text file.
Heightmaps can be generated from geotiff with scripts/dem2heightmap.py. The python script uses GDAL and numpy.
