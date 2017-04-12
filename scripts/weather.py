from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *
#from ImageView import *

# models to load
modelNames = {
		'capsule': ("capsule/capsule.obj", True),
		'terrain': ("sw_lr/sw_lr.tif.txt.obj", True)
		}
		
models = {}

light = Light.create()
light.setColor(Color("#505050"))
light.setAmbient(Color("#202020"))
light.setPosition(Vector3(0, 50, -5))
light.setEnabled(True)

light2 = Light.create()
light2.setColor(Color("#505030"))
light2.setPosition(Vector3(50, 0, 50))
light2.setEnabled(True)

scene = getSceneManager()

cam = getDefaultCamera()
cam.setPosition(Vector3(0, -50, 20))
cam.setNearFarZ(0.1, 100000)
cam.getController().setSpeed(10)

# Queue models for loading
for name,model in modelNames.iteritems():
	mi = ModelInfo()
	mi.name = name
	#mi.optimize = True
	mi.optimize = False
	mi.path = "testdata/" + model[0]
	scene.loadModelAsync(mi, "onModelLoaded('" + name + "')")

#--------------------------------------------------------------------------------------------------
# Model loaded callback: create objects
def onModelLoaded(name):
	print name, 'loaded'
	global models
	model = StaticObject.create(name)
	if(model != None):
		model.setPosition(Vector3(0, 0, 0))
		#model.setEffect("colored -g 1.0 -d " + modelNames[name][2])
		model.setVisible(modelNames[name][1])
		#model.setAlpha(0.0)
		model.setScale(Vector3(1, 1, 1))
		models[name] = model
		
#--------------------------------------------------------------------------------------------------
def toggleModel(name):
	global models
	models[name].setVisible(not models[name].isVisible())


