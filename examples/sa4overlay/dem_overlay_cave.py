#/usr/bin/env python
import numpy
import csv
import subprocess
import time

from math import *
from euclid import *
from omega import *
from cyclops import *
import tessterrain

# 1.0002777999999921 degree ~ 36470 m ~ 3648 pixels
# ref_point (lat, long) : -36.9998611,140.9998611
ref_point = {}
ref_point['lat'] = -36.9998611
ref_point['lon'] = 140.9998611

# height scale
data_height_scale = 0.4          # because x, z values were scaled smaller when preparing data
                                 # this value is multiple with height scale
hscale_min = 0.2
hscale_max = 4
hscale_value = data_height_scale

# ovelay alpha
overlay_alpha_min = 0.2
overlay_alpha_max = 1
overlay_alpha_value = 0.6

terrains = (
    "s35_e141", "s35_e142",
    "s36_e141", "s36_e142", "s36_e143", "s36_e144", "s36_e145", "s36_e146", "s36_e147",
    "s37_e141", "s37_e142", "s37_e143", "s37_e144", "s37_e145", "s37_e146", "s37_e147", "s37_e148",
    "s38_e141", "s38_e142", "s38_e143", "s38_e144", "s38_e145", "s38_e146", "s38_e147", "s38_e148", "s38_e149",
    "s39_e141", "s39_e142", "s39_e143", "s39_e144", "s39_e145", "s39_e146", "s39_e147"
    )

print 'terrains:', terrains

overlay_dir = '/home/toand/git/projects/tessterrain/testdata/vic_usgs/overlay/'

# INIT TERRAIN
tt = tessterrain.initialize()
for i in range(len(terrains)):
    filename = 'config/' + terrains[i] + '_1arc_v3.ini'
    print filename
    tt.addTerrain(filename)

# go colormap mode
tt.setHeightScale(1.0 * data_height_scale);
tt.nextDisplayMode(-1);

# model
scene = getSceneManager()
all = SceneNode.create("everything")
# ui
uim = UiModule.createAndInitialize()

# other settings
cam = getDefaultCamera()
cam.setPosition(Vector3(46930.8, 7805.12, 65433.8))
cam.setOrientation(Quaternion(-0.86, 0.50, 0.00, 0.00))
cam.getController().setSpeed(2000)
setNearFarZ(2, 400000)

# Create a directional light                                                        
light1 = Light.create()
light1.setLightType(LightType.Directional)
light1.setLightDirection(Vector3(-1.0, -1.0, -1.0))
light1.setColor(Color(1.0, 1.0, 1.0, 1.0))
light1.setAmbient(Color(0.2, 0.2, 0.2, 1.0))
light1.setEnabled(True)


def setHeightScale(value):
    val = (float(value) / 100) * (hscale_max - hscale_min) + hscale_min
    hscale_label.setText('Height scale: ' + str(val))
    hscale_value = val * data_height_scale
    tt.setHeightScale(hscale_value)
    

def setOverlayAlpha(value):
    val = (float(value) / 100) * (overlay_alpha_max - overlay_alpha_min) + overlay_alpha_min
    overlay_alpha_label.setText('Overlay alpha: ' + str(val))
    tt.setOverlayAlpha(val)


def changeOverlayData(type, reload=True):  

    if isMaster():
    	cmd = 'unlink ' + str(overlay_dir) + 'current'
    	subprocess.call(cmd.split())

    b11.getButton().setChecked(False)
    b12.getButton().setChecked(False)
    b13.getButton().setChecked(False)

    if type == 'population':
        cmd = 'ln -s ' + str(overlay_dir) + 'sa4_population ' + str(overlay_dir) + 'current'
        b12.getButton().setChecked(True)

    elif type == 'job':
        cmd = 'ln -s ' + str(overlay_dir) + 'sa4_job  ' + str(overlay_dir) + 'current'
	b13.getButton().setChecked(True)

    else:
        cmd = 'ln -s ' + str(overlay_dir) + 'sa4_nodata ' + str(overlay_dir) + 'current'
	b11.getButton().setChecked(True)

    if isMaster():
    	print cmd
    	subprocess.call(cmd.split())
    else:
	time.sleep(1)

    if reload:
    	tt.reloadOverlay()


#menu
mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)

# cameras
menu.addButton("Go to camera 1", 'cam.setPosition(Vector3(46930.8, 7805.12, 65433.8)), cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))')
menu.addButton("Go to camera 2 (topview)", 'cam.setPosition(Vector3(123302.47, 77291.67, 104510.23)), cam.setOrientation(Quaternion(-0.92, 0.38, 0.05, 0.02))')


menu.addButton("Next terrain display mode", 'tt.nextDisplayMode(1)')
menu.addButton("Previous terrain display mode", 'tt.nextDisplayMode(-1)')

menu.addButton("Toggle fog", 'tt.toggleFog()')

# overlay type
l_overlay_data = menu.addLabel("Overlay data")
b11 = menu.addButton("none", "changeOverlayData('nodata')")
b12 = menu.addButton("population", "changeOverlayData('population')")
b13 = menu.addButton("job", "changeOverlayData('job')")
b11.getButton().setCheckable(True)
b12.getButton().setCheckable(True)
b13.getButton().setCheckable(True)
changeOverlayData('nodata', False)

# height scale
hscale_label = menu.addLabel("Height scale: ")
hscale = 1
val = int( float(hscale - hscale_min) / (hscale_max-hscale_min) * 100 )
pointscale = menu.addSlider(100, "setHeightScale(%value%)")
pointscale.getSlider().setValue(val)
pointscale.getWidget().setWidth(200)
setHeightScale(val)

# overlay alpha
overlay_alpha_label = menu.addLabel("Overlay alpha: ")
val = int( float(overlay_alpha_value - overlay_alpha_min) / (overlay_alpha_max-overlay_alpha_min) * 100 )
overlay = menu.addSlider(100, "setOverlayAlpha(%value%)")
overlay.getSlider().setValue(val)
overlay.getWidget().setWidth(200)
setOverlayAlpha(val)

def onEvent():
    e = getEvent()        

    if (e.getServiceType() == ServiceType.Wand and e.isButtonDown(EventFlags.Button4) ) :
        tt.nextDisplayMode(1)
	

setEventFunction(onEvent)

queueCommand(":freefly")

