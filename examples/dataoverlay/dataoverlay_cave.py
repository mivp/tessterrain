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

# ovelay alpha
overlay_alpha_min = 0.2
overlay_alpha_max = 1
overlay_alpha_value = 0.6


#INIT
# Terrain
tt = tessterrain.initialize()
tt.initTerrain('terraindata/apps/overlayterrain/vic_config.ini')
tt.nextDisplayMode(-1)
tt.setHeightScale(1.0 * data_height_scale);

ttOverlay = tessterrain.initialize()
ttOverlay.initTerrain('terraindata/apps/overlayterrain/data_config.ini')
ttOverlay.nextDisplayMode(-1)
ttOverlay.setHeightScale(1.0 * data_height_scale);
ttOverlay.setHeight(400)
ttOverlay.setOpacity(0.7)

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

def setOverlayAlpha(value):
    val = (float(value) / 100) * (overlay_alpha_max - overlay_alpha_min) + overlay_alpha_min
    overlay_alpha_label.setText('Overlay alpha: ' + str(val))
    tt.setOverlayAlpha(val)


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

# overlay alpha
overlay_alpha_label = menu.addLabel("Overlay alpha: ")
val = int( float(overlay_alpha_value - overlay_alpha_min) / (overlay_alpha_max-overlay_alpha_min) * 100 )
overlay = menu.addSlider(100, "setOverlayAlpha(%value%)")
overlay.getSlider().setValue(val)
overlay.getWidget().setWidth(200)
setOverlayAlpha(val)

queueCommand(":freefly")
