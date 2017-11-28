# /usr/bin/env python
import numpy
import csv

from math import *
from euclid import *
from omega import *
from cyclops import *

csv_filename = "terraindata/cyprus/data.csv"
ref_point = {}
ref_point['lat'] = 36.0001388889
ref_point['lon'] = 31.9998611111
data_xz_scale = [1, 1]
data_height_scale = 1.0 

hscale_min = 0.2
hscale_max = 4
hscale_value = data_height_scale

colormap = {}
colormap[0] = '#ffffff'
colormap[1] = '#ff0000'
colormap[2] = '#ffff00'
colormap[3] = '#ff00ff'
colormap[4] = '#00ff00'
colormap[5] = '#0000ff'


# Terrain
tt = tessterrain.initialize()
tt.initTerrain('terraindata/cyprus/cyprus_config.ini')
tt.nextDisplayMode(-1)
tt.setHeightScale(1.0 * data_height_scale);


# model
scene = getSceneManager()
all = SceneNode.create("everything")

# Create a directional light
light1 = Light.create()
light1.setLightType(LightType.Directional)
light1.setLightDirection(Vector3(-1.0, -1.0, -1.0))
light1.setColor(Color(1.0, 1.0, 1.0, 1.0))
light1.setAmbient(Color(0.2, 0.2, 0.2, 1.0))
light1.setEnabled(True)

light2 = Light.create()
light2.setColor(Color("#606040"))
light2.setPosition(Vector3(0, 0, 0))
light2.setEnabled(True)
#light2.setAttenuation(0, 0.1, 0.1)

# Camera
cam = getDefaultCamera()
cam.setPosition(Vector3(55875.2, 18618.9, 160089))
cam.setOrientation(Quaternion(0, 0, 0, 1))
cam.getController().setSpeed(4000)
setNearFarZ(2, 400000)
cam.addChild(light2)

def drawSite(site, options):
    print site["Latitude"], site["Longitude"], site["Height"], site["Vessels"], site["Beads"]
    lat = float(site["Latitude"])
    lon = float(site["Longitude"])
    height = float(site["Height"]) + 200
    vessel = int(site["Vessels"]) 
    vessel_c = int(min(vessel / 5, 5))
    bead = int(site["Beads"])
    bead_c = int(min(bead / 5, 5))

    # calculate position
    oneDegreeLength = 30.86666667 * 3647
    x = (lon - ref_point['lon']) * oneDegreeLength * data_xz_scale[0];
    z = -1 * (lat - ref_point['lat']) * oneDegreeLength * data_xz_scale[1];
    pos = Vector3(x, height, z)
    print 'pos: ', pos, vessel_c, bead_c

    size = 1000
    if vessel == 0 and bead == 0:
        print 'no data'
        sphere = SphereShape.create(0.5, 2)
        sphere.setScale(Vector3(500, 50, 500))
        sphere.setEffect('colored -d ' + colormap[0])
        sphere.setVisible(True)
        sphere.setPosition(pos)

    elif vessel > 0 and bead == 0:
        print 'vessel only'
        sphere = SphereShape.create(0.5, 2)
        sphere.setScale(Vector3(size, size, size))
        sphere.setEffect('colored -d ' + colormap[vessel_c])
        sphere.setVisible(True)
        sphere.setPosition(pos)

    elif vessel == 0 and bead > 0:
        print 'bead only'
        box = BoxShape.create(1, 1, 1)
        box.setScale(Vector3(size, size, size))
        box.setPosition(Vector3(pos[0], pos[1] + size/2, pos[2]))
        box.setEffect('colored -d ' + colormap[bead_c])

    else:
        print 'both'
        sphere = SphereShape.create(0.5, 2)
        sphere.setScale(Vector3(size, size, size))
        sphere.setEffect('colored -d ' + colormap[vessel_c])
        sphere.setVisible(True)
        sphere.setPosition(Vector3(pos[0] - size/2-100, pos[1] + size/2, pos[2]))

        box = BoxShape.create(1, 1, 1)
        box.setScale(Vector3(size, size, size))
        box.setPosition(Vector3(pos[0] + size/2+100, pos[1] + size/2, pos[2]))
        box.setEffect('colored -d ' + colormap[bead_c])


# sites
with open(csv_filename, 'rb') as csvfile:
    sites = csv.DictReader(csvfile)
    options = {}

    csvfile.seek(0)
    sites = csv.DictReader(csvfile)
    for site in sites:
        drawSite(site, options)


# menu
mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)

# menu items
menu.addButton("Go to camera 1",
               'cam.setPosition(Vector3(46930.8, 7805.12, 65433.8)), cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))')

menu.addButton("Next terrain display mode", 'tt.nextDisplayMode(1)')
menu.addButton("Toggle fog", 'tt.toggleFog()')

# height scale
hscale_label = menu.addLabel("Height scale: ")
hscale = 1
val = int( float(hscale - hscale_min) / (hscale_max-hscale_min) * 100 )
pointscale = menu.addSlider(100, "setHeightScale(%value%)")
pointscale.getSlider().setValue(val)
pointscale.getWidget().setWidth(200)
setHeightScale(val)


queueCommand(":freefly")

