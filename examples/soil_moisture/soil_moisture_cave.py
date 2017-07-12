#/usr/bin/env python
import tessterrain

from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

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


# MENU
def setHeightScale(value):
    val = (float(value) / 100) * (hscale_max - hscale_min) + hscale_min
    hscale_label.setText('Height scale: ' + str(val))
    hscale_value = val * data_height_scale
    tt.setHeightScale(hscale_value)
    vf.setElevationScale(hscale_value)

mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)

# menu items
menu.addButton("Go to camera 1",
               'cam.setPosition(Vector3(46930.8, 7805.12, 65433.8)), cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))')

menu.addButton("Next terrain display mode", 'tt.nextDisplayMode(1)')
menu.addButton("Toggle fog", 'tt.toggleFog()')
menu.addButton("Next particle type", "vf.nextParticleType()")

# height scale
hscale_label = menu.addLabel("Height scale: ")
hscale = 1
val = int( float(hscale - hscale_min) / (hscale_max-hscale_min) * 100 )
pointscale = menu.addSlider(100, "setHeightScale(%value%)")
pointscale.getSlider().setValue(val)
pointscale.getWidget().setWidth(200)
setHeightScale(val)

queueCommand(":freefly")
