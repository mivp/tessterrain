import tessterrain
from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

tt = tessterrain.initialize()
tt.initTerrain("testdata/tess/config.ini")

# model
scene = getSceneManager()

light = Light.create()
light.setColor(Color("#505050"))
light.setAmbient(Color("#202020"))
light.setPosition(Vector3(0, 50, -5))
light.setEnabled(True)

light2 = Light.create()
light2.setColor(Color("#505030"))
light2.setPosition(Vector3(50, 0, 50))
light2.setEnabled(True)

models = []
# Queue models for loading
mi = ModelInfo()
model = 'capsule'
mi.name = model
#mi.optimize = True
mi.optimize = False
mi.path = "testdata/capsule/capsule.obj"
scene.loadModelAsync(mi, "onModelLoaded('" + model + "')")

# Model loaded callback: create objects
def onModelLoaded(name):
    global models
    model = StaticObject.create(name)
    if(model != None):
		model.setPosition(Vector3(0, 0, 0))
		#model.setEffect("colored -g 1.0 -d red")
		model.setScale(Vector3(20, 20, 20))
		model.setVisible(True)
		models.append(model)

# camera
cam = getDefaultCamera()
cam.getController().setSpeed(10)


#menu
mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)
campos = [294.059, 85.6822, 559.623]
camori = [1, 0, 0, 0]
cmd = 'cam.setPosition(Vector3(' + str(campos[0]) + ',' + str(campos[1]) + ',' + str(campos[2]) + ')),' + \
		'cam.setOrientation(Quaternion(' + str(camori[0]) + ',' + str(camori[1]) + ',' + str(camori[2]) + ',' + str(camori[3]) + '))'
menu.addButton("Go to camera 1", cmd)

