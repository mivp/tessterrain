#/usr/bin/env python
import numpy
import csv

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
overlay_alpha_min = 0.4
overlay_alpha_max = 1
overlay_alpha_value = 0.5

terrains = (
    "s35_e141", "s35_e142",
    "s36_e141", "s36_e142", "s36_e143", "s36_e144", "s36_e145", "s36_e146", "s36_e147",
    "s37_e141", "s37_e142", "s37_e143", "s37_e144", "s37_e145", "s37_e146", "s37_e147", "s37_e148",
    "s38_e141", "s38_e142", "s38_e143", "s38_e144", "s38_e145", "s38_e146", "s38_e147", "s38_e148", "s38_e149",
    "s39_e141", "s39_e142", "s39_e143", "s39_e144", "s39_e145", "s39_e146", "s39_e147"
    )

print 'terrains:', terrains

tt = tessterrain.initialize()
for i in range(len(terrains)):
    filename = 'config/' + terrains[i] + '_1arc_v3.ini'
    print filename
    tt.addTerrain(filename)

# go colormap mode
tt.nextDisplayMode(3)
tt.setHeightScale(1.0 * data_height_scale);

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

#menu
mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)

# cameras
menu.addButton("Go to camera 1", 'cam.setPosition(Vector3(46930.8, 7805.12, 65433.8)), cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))')

menu.addButton("Next terrain display mode", 'tt.nextDisplayMode()')

menu.addButton("Toggle fog", 'tt.toggleFog()')

menu.addButton("Reload Overlay", 'tt.reloadOverlay()')

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
overlay_alpha = 1
val = int( float(overlay_alpha - overlay_alpha_min) / (overlay_alpha_max-overlay_alpha_min) * 100 )
overlay = menu.addSlider(100, "setOverlayAlpha(%value%)")
overlay.getSlider().setValue(val)
overlay.getWidget().setWidth(200)
setOverlayAlpha(val)


def onEvent():
    e = getEvent()        

    if (e.getServiceType() == ServiceType.Wand and e.isButtonDown(EventFlags.Button4) ) :
        tt.nextDisplayMode()
	

setEventFunction(onEvent)

queueCommand(":freefly")

