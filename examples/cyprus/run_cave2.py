# /usr/bin/env python
import numpy
import csv

from math import *
from euclid import *
from omega import *
from cyclops import *
import tessterrain

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
colormap[1] = '#00008F'
colormap[2] = '#0080FF'
colormap[3] = '#80FF80'
colormap[4] = '#FF8000'
colormap[5] = '#800000'


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
cam.setPosition(Vector3(146047.75, 17604.45, 178648.63))
cam.setOrientation(Quaternion(0.99, -0.14, -0.1, 0.01))
cam.getController().setSpeed(4000)
setNearFarZ(2, 400000)
cam.addChild(light2)


# ui
uim = UiModule.createAndInitialize()
imgplot = loadImage('terraindata/cyprus/colormap.jpg')
hplot = Container.create(ContainerLayout.LayoutVertical, uim.getUi())
hplot.setVisible(True)
hplot.setStyle('fill: #aaaaaa80')
hplot.setSize(Vector2(imgplot.getWidth(), imgplot.getHeight())) 
#hplot.setAutosize(True)
#hplot.setPosition(Vector3(10000, 200, 0))
hplot.setPosition(Vector3(10, 10, 0))
plotview = Image.create(hplot)
plotview.setData(imgplot)
#plotview.setPosition(Vector2(2, 2))
#plotview.setSize(Vector2(imgplot.getWidth(), 64)) 


def drawSite(site, options):
    print site["Tier"], site["Short"], site["Latitude"], site["Longitude"], site["Height"], site["Vessels"], site["Beads"]
    lat = float(site["Latitude"])
    lon = float(site["Longitude"])
    height = float(site["Height"]) + 200
    vessel = int(site["Vessels"]) 
    vessel_c = int(min(vessel / 5, 5))
    bead = int(site["Beads"])
    bead_c = int(min(bead / 5, 5))
    display = '(' + site["Tier"] + ') '  + site["Short"]

    # calculate position
    oneDegreeLength = 30.86666667 * 3647
    x = (lon - ref_point['lon']) * oneDegreeLength * data_xz_scale[0];
    z = -1 * (lat - ref_point['lat']) * oneDegreeLength * data_xz_scale[1];
    pos = Vector3(x, height, z)
    print 'pos: ', pos, vessel_c, bead_c

    #text
    text = Text3D.create('fonts/arial.ttf', 0.6, display)
    text.setPosition(Vector3(0.5, 0.4, 0.5))
    #text.setPosition(pos)
    text.setFontResolution(120)
    text.setColor(Color('green'))

    center = SphereShape.create(0.5, 2)
    center.setScale(Vector3(1000, 1000, 1000))
    center.setPosition(pos)
    center.setVisible(False)
    center.addChild(text)

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
        cylinder = CylinderShape.create(1, 0.5, 0.5, 1, 16)
        cylinder.pitch(radians(-90))
        cylinder.setScale(Vector3(size, size, size))
        cylinder.setEffect('colored -d ' + colormap[vessel_c])
        cylinder.setVisible(True)
        cylinder.setPosition(pos[0], pos[1], pos[2] + size/2)
        

    elif vessel == 0 and bead > 0:
        print 'bead only'
        sphere = SphereShape.create(0.5, 2)
        sphere.setScale(Vector3(size, size, size))
        sphere.setEffect('colored -d ' + colormap[bead_c])
        sphere.setVisible(True)
        sphere.setPosition(pos[0], pos[1] + size/2, pos[2])
        
    else:
        print 'both'
        cylinder = CylinderShape.create(1, 0.5, 0.5, 1, 16)
        cylinder.pitch(radians(-90))
        cylinder.setScale(Vector3(size, size, size))
        cylinder.setEffect('colored -d ' + colormap[vessel_c])
        cylinder.setVisible(True)
        cylinder.setPosition(Vector3(pos[0] - size/2-100, pos[1], pos[2]))

        sphere = SphereShape.create(0.5, 2)
        sphere.setScale(Vector3(size, size, size))
        sphere.setEffect('colored -d ' + colormap[bead_c])
        sphere.setVisible(True)
        sphere.setPosition(Vector3(pos[0] + size/2+100, pos[1] + size/2, pos[2]))


# sites
with open(csv_filename, 'rb') as csvfile:
    sites = csv.DictReader(csvfile)
    options = {}

    csvfile.seek(0)
    sites = csv.DictReader(csvfile)
    for site in sites:
        drawSite(site, options)


def setHeightScale(value):
    val = (float(value) / 100) * (hscale_max - hscale_min) + hscale_min
    hscale_label.setText('Height scale: ' + str(val))
    hscale_value = val * data_height_scale
    tt.setHeightScale(hscale_value)
    #vf.setElevationScale(hscale_value)

# menu
mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)

# menu items
menu.addButton("Go to camera 1",
               'cam.setPosition(Vector3(146047.75, 17604.45, 178648.63)), cam.setOrientation(Quaternion(0.99, -0.14, -0.1, 0.01))')

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

