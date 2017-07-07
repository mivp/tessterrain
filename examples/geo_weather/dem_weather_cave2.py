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

csv_filename = "testdata/south_west_usgs/south_west_stations.csv"

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
vf.init(0, 0, 109379.614343, 72919.7428954, 4000, 3500);
#vf.loadElevationFromFile("vic_sw_elevation2.txt");
vf.setElevationScale(1.0 * data_height_scale);

vf.addControlPoint( 72195.6097426 , 10213.8282815 , 4.9193495505 , 9.838699101 );
vf.addControlPoint( 80216.7814611 , 8390.83470912 , 6.36396103068 , 6.36396103068 );
vf.addControlPoint( 19693.3948579 , 52142.6804464 , 10.6066017178 , 10.6066017178 );
vf.addControlPoint( 91519.3416099 , 67820.4251689 , -6.36396103068 , 6.36396103068 );
vf.addControlPoint( 12036.8218539 , 21151.7897158 , 9.19238815543 , 9.19238815543 );
vf.addControlPoint( 9484.63085255 , 33548.146008 , 8.049844719 , 4.0249223595 );
vf.addControlPoint( 38652.5280107 , 23703.9807172 , 7.6026311235 , 15.205262247 );
vf.addControlPoint( 64539.0367386 , 39017.1267252 , 4.9193495505 , 9.838699101 );
vf.addControlPoint( 101728.105615 , 44850.7061568 , 8.4970583145 , 16.994116629 );
vf.addControlPoint( 58340.8585925 , 10943.0257105 , 14.1421356237 , 14.1421356237 );
vf.addControlPoint( 44850.7061568 , 50684.2855885 , 5.8137767415 , 11.627553483 );
vf.addControlPoint( 17141.2038566 , 47767.4958726 , 5.8137767415 , 11.627553483 );
vf.addControlPoint( 22245.5858592 , 48861.2920161 , 14.1421356237 , 14.1421356237 );
vf.addControlPoint( 52871.8778753 , 47038.2984437 , 7.6026311235 , 15.205262247 );
vf.addControlPoint( 70737.2148847 , 25891.573004 , 8.4970583145 , 16.994116629 );

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
db_filename = "testdata/vic_usgs/south_west_stations.db"
db = DBUtil(db_filename)

# Stations
station_rows = db.getStations()
station_data = db.getData('20170515090000')
stations = {}
for r in station_rows:
    s = Station(r, uim)
    s.updatePosition(ref_point, data_height_scale)
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
