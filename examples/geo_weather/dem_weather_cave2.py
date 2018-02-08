#/usr/bin/env python
import numpy
import csv
from dbutil import DBUtil
from station import Station
import tessterrain
import vectorfield

from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

csv_filename = "terraindata/apps/geo_weather/south_west_stations.csv"
db_filename = "terraindata/apps/geo_weather/south_west_stations.db"

ref_point = {}
ref_point['lat'] = -36.9998611
ref_point['lon'] = 140.9998611
data_height_scale = 0.4 

hscale_min = 0.2
hscale_max = 4
hscale_value = data_height_scale

#INIT
# Terrain
tt = tessterrain.initialize()
tt.initTerrain('vic_config.ini')

tt.nextDisplayMode(-1)
tt.setHeightScale(1.0 * data_height_scale);

#vector field
vf = vectorfield.initialize()
vf.init(0, 0, 101313.660011, 67542.4400073, 4000, 3500);
#vf.loadElevationFromFile("terraindata/apps/geo_weather/vic_sw_elevation2.txt");
vf.setElevationScale(1.0 * data_height_scale);

vf.addControlPoint(66871.7064297, 9460.63242348, 4.919349551, 9.838699101)
vf.addControlPoint(74301.3748305, 7772.0714233, 6.363961031, 6.363961031)
vf.addControlPoint(18241.1496244, 48297.5354277, 10.60660172, 10.60660172)
vf.addControlPoint(84770.4530316, 62819.1600292, -6.363961031, 6.363961031)
vf.addControlPoint(11149.1934237, 19591.9984246, 9.192388155, 9.192388155)
vf.addControlPoint(8785.20802341, 31074.2132258, 8.049844719, 4.02492236)
vf.addControlPoint(35802.1840263, 21955.9838248, 7.602631124, 15.20526225)
vf.addControlPoint(59779.7502289, 36139.8962264, 4.919349551, 9.838699101)
vf.addControlPoint(94226.3946326, 41543.2914269, 8.497058315, 16.99411663)
vf.addControlPoint(54038.6428283, 10136.0568236, 14.14213562, 14.14213562)
vf.addControlPoint(41543.2914269, 46946.6866275, 5.813776742, 11.62755348)
vf.addControlPoint(15877.1642242, 44244.9890272, 5.813776742, 11.62755348)
vf.addControlPoint(20605.1350247, 45258.1256273, 14.14213562, 14.14213562)
vf.addControlPoint(48972.9598277, 43569.5646272, 7.602631124, 15.20526225)
vf.addControlPoint(65520.8576295, 23982.257025, 8.497058315, 16.99411663)

vf.setPointScale(1.5)
vf.updateVectorField()
vf.setVisible(False)


# Scene
scene = getSceneManager()
all = SceneNode.create("everything")
# Light
light1 = Light.create()
light1.setLightType(LightType.Directional)
light1.setLightDirection(Vector3(-1.0, -1.0, -1.0))
light1.setColor(Color(1.0, 1.0, 1.0, 1.0))
light1.setAmbient(Color(0.2, 0.2, 0.2, 1.0))
light1.setEnabled(True)
# Camera
cam = getDefaultCamera()
cam.setPosition(Vector3(46930.8, 7805.12, 65433.8))
cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))
cam.getController().setSpeed(2000)
setNearFarZ(2, 400000)
# UI
uim = UiModule.createAndInitialize()

# DB
db = DBUtil(db_filename)

# Stations
station_rows = db.getStations()
station_data = db.getData('20170515090000')
size_scale = [0.3, 0.3] # as in config.ini
stations = {}
for r in station_rows:
    s = Station(r, uim)
    s.updatePosition(ref_point, size_scale, data_height_scale)
    stations[s.id] = s

for d in station_data:
    stations[d[0]].updateData(d)


# MENU
def setHeightScale(value):
    val = (float(value) / 100) * (hscale_max - hscale_min) + hscale_min
    hscale_label.setText('Height scale: ' + str(val))
    hscale_value = val * data_height_scale
    tt.setHeightScale(hscale_value)
    vf.setElevationScale(hscale_value)

def showWeatherInfo():
    for name, s in stations.iteritems():
        s.hideAll()
        s.showInfoHud()

def showSampleGraph():
    for name, s in stations.iteritems():
        s.hideAll()
        s.showGraph()

mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)

# menu items
menu.addButton("Go to camera 1",
               'cam.setPosition(Vector3(46930.8, 7805.12, 65433.8)), cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))')

menu.addButton("Next terrain display mode", 'tt.nextDisplayMode(1)')
menu.addButton("Toggle fog", 'tt.toggleFog()')
menu.addButton("Toggle wind", "vf.toggleVisible()")
menu.addButton("Next particle type", "vf.nextParticleType()")

menu.addButton("Show weather info", "showWeatherInfo()")
menu.addButton("Show sample graph", "showSampleGraph()")

# height scale
hscale_label = menu.addLabel("Height scale: ")
hscale = 1
val = int( float(hscale - hscale_min) / (hscale_max-hscale_min) * 100 )
pointscale = menu.addSlider(100, "setHeightScale(%value%)")
pointscale.getSlider().setValue(val)
pointscale.getWidget().setWidth(200)
setHeightScale(val)


# event
def onEvent():
    e = getEvent()
    r = getRayFromEvent(e)
    
    # On pointer and right click
    if(e.getServiceType() == ServiceType.Pointer and e.isButtonDown(EventFlags.Right) ) or \
      (e.getServiceType() == ServiceType.Wand and e.isButtonDown(EventFlags.Button5) ) :
    
        for name, s in stations.iteritems():
            #print name, s, r[1], r[2]
            hit = s.isLineIntersection(r[1], r[2])
            if( hit ):
                print 'hit ', name
                break


setEventFunction(onEvent)

queueCommand(":freefly")
