# Tessellation terrain

Require GLSL version 4 (tested with 4.1)

## Build Omegalib module

Tested with Omegalib 13.1

```
cd tessterrain
mkdir build
cd build
cmake ..
make
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

## Configuration file (INI)

Example

```
[file]
heightmap = testdata/tess/heightmap-1024x1024.png
texture = testdata/tess/grass.png

[topleft]
left = 0
top = 0

[horizontalres]
wres = 20
hres = 20

[heightrange]
min = 0
max = 200

; translate to point
[translate]
x = 0
y = 0
z = 0
```