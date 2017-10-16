from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

mesh_path = "terraindata/apps/soil_moisture/VRTPaddocks_mesh/"
nummesh = 55

# paddocks
paddocks = {}

# selected will store the currently active object
selected = None
ray_start = Vector3(0,0,0)
ray_dir = Vector3(0,0,-1)

# Scene
scene = getSceneManager()

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
cam.getController().setSpeed(8000)
setNearFarZ(2, 400000)

# load model
def loadPaddock(id):
	# Load a static model
	name = str(id)
	print name

	model = ModelInfo()
	model.name = name
	model.path = mesh_path + str(id) + ".stl"
	#model.size = 1.0
	scene.loadModel(model)

	# Create a scene object using the loaded model
	paddock = StaticObject.create(name)
	#paddock.setPosition(Vector3(0, 2, -4))
	#paddock.pitch(radians(45))
	paddock.setEffect("colored -d gray")
	paddock.setSelectable(True)
	paddock.setName(name)

	return paddock


#--------------------------------------------------------------------------------------------------
def onObjectSelected(node, distance):
	global selected
	global paddocks

	for name, p in paddocks.iteritems():
		p.setEffect("colored -d gray")

	if node == None:
		print 'None'

	else:
		selected = node
		print node.getName()
		node.setEffect("colored -d green")
	

# load paddocks
for i in range(nummesh):
	paddocks[i] = loadPaddock(i)

# ray
aimSet = LineSet.create()
aimSet.setEffect('colored -d white -g 2.0 -s 20')
aim = aimSet.addLine()
aim.setThickness(0.01)


# MENU
mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)
# menu items
menu.addButton("Go to camera 1",
               'cam.setPosition(Vector3(46930.8, 7805.12, 65433.8)), cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))')


#--------------------------------------------------------------------------------------------------
def onUpdate(frame, t, dt):
	global ray_start
	global ray_dir
	aim.setStart(ray_start)
	aim.setEnd(ray_start + ray_dir * 100)


#--------------------------------------------------------------------------------------------------
def onEvent():
	global ray_start
	global ray_dir
	global paddocks

	e = getEvent()
	#if(e.getType() != EventType.Update): print(e.getType() )
	if(e.getServiceType() == ServiceType.Pointer or e.getServiceType() == ServiceType.Wand):
		# Button mappings are different when using wand or mouse
		confirmButton = EventFlags.Button2
		if(e.getServiceType() == ServiceType.Wand): 
			confirmButton = EventFlags.Button5
		
		r = getRayFromEvent(e)
		if(r[0]):
			ray_start = r[1]
			ray_dir = r[2]

		"""
		if e.isButtonDown(confirmButton) and r[0]:
			print "press"
			for name, p in paddocks.iteritems():
				#print name, s, r[1], r[2]
				hitData = hitNode(p, r[1], r[2])
				if(hitData[0]):
					p.setEffect('colored -e green')
					break
		"""

		# When the confirm button is pressed:
		if(e.isButtonDown(confirmButton)):
			if(r[0]): 
				querySceneRay(r[1], r[2], onObjectSelected)
		
#--------------------------------------------------------------------------------------------------
setUpdateFunction(onUpdate)
setEventFunction(onEvent)


# Other settings
queueCommand(":freefly")