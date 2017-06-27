#/usr/bin/env python
import numpy
import csv
from dbutil import DBUtil
from station import Station

from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

ref_point = {}
ref_point['lat'] = -36.9998611
ref_point['lon'] = 140.9998611
data_height_scale = 0.4 

#INIT
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

# cameras
menu.addButton("Go to camera 1",
               'cam.setPosition(Vector3(46930.8, 7805.12, 65433.8)), cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))')
menu.addButton("Show weather info", "showWeatherInfo()")
menu.addButton("Show sample graph", "showSampleGraph()")

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