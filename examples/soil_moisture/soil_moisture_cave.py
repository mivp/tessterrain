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

# paddocks
mesh_path = "terraindata/apps/soil_moisture/VRTPaddocks_mesh/"
nummesh = 55
paddocks = {}
graphs = {}
centers = {}
# selected will store the currently active object
selected = None
ray_start = Vector3(0,0,0)
ray_dir = Vector3(0,0,-1)


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


# load model
#load centers
with open(mesh_path+'centers.csv') as f_in:
	for line in f_in:
		line = line.strip()
		line = line.split(',')
		centers[line[0]] = (float(line[1]), float(line[2]))

def loadPaddock(id):
	global paddocks
	global graphs
	global centers

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
	paddock.setPosition(Vector3(centers[str(id)][0], 120, centers[str(id)][1]))
	#paddock.pitch(radians(45))
	paddock.setEffect("colored -d gray")
	paddock.setSelectable(True)
	paddock.setName(name)
	# graph
	w = 1800
	graph_filename = mesh_path + str(id) + ".png"
	img1 = PlaneShape.create(w, w*700.0/1000.0)
	img1.setPosition(Vector3(0, 800, 0))
	img1.setEffect("textured -v emissive -d " + graph_filename)
	#img1.roll(radians(180))
	#self.img1.pitch(radians(-5))
	img1.setFacingCamera(getDefaultCamera())
	img1.getMaterial().setDoubleFace(True)
	img1.getMaterial().setTransparent(True)
	img1.setVisible(False)
	paddock.addChild(img1)

	paddocks[str(id)] = paddock
	graphs[str(id)] = img1


#--------------------------------------------------------------------------------------------------
def onObjectSelected(node, distance):
	global selected
	global paddocks
	global graphs

	for name, p in paddocks.iteritems():
		p.setEffect("colored -d gray")

	for name, g in graphs.iteritems():
		g.setVisible(False)

	if node == None:
		print 'None'

	else:
		selected = node
		print node.getName()
		node.setEffect("colored -d green")
		graphs[node.getName()].setVisible(True)

# load paddocks
for i in range(nummesh):
	loadPaddock(i)

# ray
aimSet = LineSet.create()
aimSet.setEffect('colored -d white -g 2.0 -s 20')
aim = aimSet.addLine()
aim.setThickness(0.01)


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


queueCommand(":freefly")
