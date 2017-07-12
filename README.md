# Tessellation terrain

Desktop/CAVE2 terrain rendering using tessellation shaders. 

[Screenshots](http://www.toaninfo.com/work/2017-tessterrain.html)

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
tt.initTerrain('vic_config.ini')
```

Functions which can be used in python script:
```
nextDisplayMode(n): change display mode.
displayInfo(): display information of terrains
toggleFog(): toggle fog in scene
setHeightScale(scale): set height scale for terrain
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

- 'n': go to next display mode
- 'i': print out camera information
- 't': toggle fog
- 'o': change overlay opacity

## Configuration file (INI)

Example

```
[general]
terrains = 	s38_e141, s38_e142, s38_e143, s38_e144,
	        s39_e141, s39_e142, s39_e143, s39_e144,
hres = 10  ; 1 pixel ~ 10m
wres = 10
refLat = -36.9998611    ; topleft reference point
refLon = 140.9998611
;heightRangeScale = 1
preloadAll = 0          ; preload all terrrains
maxTerrainDisplay = 35  ; max terrains can be displayed
maxTerrainInMem = 40    ; max terrains can be stored in VRAM
numLoaderThreads = 4    ; number of threads to load texture images
bboxEnlargeFactor = 1	  ; enlarged bbox size += bboxEnlargeFactor*(bbox size)/2

; elevation data
terrainDir = terraindata/terrain
terrainFile = %s_1arc_v3_8.png 			; %s = terrain name defined in [terrains]

; texture (aerial image)
texture = 1
textureDir = terraindata/texture
textureFile = %s_1arc_v3_sat_crop.png

; overlay data
overlay = 0
overlayDir = terraindata/overlay/current
overlayFile = %s.png

; details
; geo position: tl.lat, tl.lon, br.lat, br.lon
[s35_e141]
pos = -33.9998611, 140.9998611, -35.0001389, 142.0001389
heightRange = 15, 144

[s35_e142]
pos = -33.9998611, 141.9998611, -35.0001389, 143.0001389
heightRange = 3, 135

...
```

## Heightmap

Currently support heightmaps stored in a png file (red channel will be used) or a text file.
Heightmaps can be generated from geotiff with scripts/dem2heightmap.py. The python script uses GDAL and numpy.
