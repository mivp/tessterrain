#/usr/bin/env python
import numpy
import csv

from math import *
from euclid import *
from omega import *
from cyclops import *
import utm

csv_filename = "testdata/south_west/south_west_stations.csv"
ptopleft = utm.from_latlon(-37.2, 141.2)
ptopmiddle = utm.from_latlon(-37.2, 142.525)
poffset = [ptopmiddle[0]-ptopleft[0], ptopmiddle[1]-ptopleft[1]]
print 'ptopleft', ptopleft
print 'ptopmiddle', ptopmiddle
print 'offset', poffset

# model
scene = getSceneManager()
#all = SceneNode.create("everything")
# ui
uim = UiModule.createAndInitialize()

station_centers = {}

# other settings
cam = getDefaultCamera()
#cam.setPosition(Vector3(73823.5, 97650, 148957))
cam.setPosition(Vector3(98493.20, 186738.52, 187947.87))
cam.setOrientation(Quaternion(-0.86, 0.50, 0.00, 0.00))
cam.getController().setSpeed(20000)
setNearFarZ(10, 1000000)

# some lights
light = Light.create()
light.setColor(Color("#a0a0a0"))
light.setAmbient(Color("#101020"))
light.setPosition(Vector3(0, 5, -5))
light.setEnabled(True)

light2 = Light.create()
light2.setColor(Color("#ffffff"))
light2.setPosition(Vector3(0, 0, 0))
light2.setEnabled(True)
cam.addChild(light2)
#light2.setAttenuation(0, 0.1, 0.1)

# arrow
mi = ModelInfo()
mi.name = "arrow"
mi.path = "testdata/arrow/arrow.obj"
scene.loadModel(mi)


def calWindDir(str):
    winddir = numpy.asarray([0, 0, 0])
    for i in range(len(str)):
        c = str[i]
        if c == 'E':
            winddir += numpy.asarray([1, 0, 0])
        elif c == 'W':
            winddir += numpy.asarray([-1, 0, 0])
        elif c == 'N':
            winddir += numpy.asarray([0, 1, 0])
        elif c == 'S':
            winddir += numpy.asarray([0, -1, 0])

    norm=numpy.linalg.norm(winddir, ord=2)
    if(norm == 0):
        return numpy.asarray([0, 0, 0])
    return winddir / norm


def drawStation(station, options):

    print station["Short"], station["Name"], station["Lat"], station["Lon"], station["Height"]
    lat = float(station["Lat"])
    lon = float(station["Lon"])
    x = float(station["x"])
    z = float(station["z"])
    y = float(station["Height"])
    label = station["Short"]
    name = station["Name"]
    
    global ptopleft
    p = utm.from_latlon(lat, lon)
    pos = Vector3(p[0]-ptopleft[0], y, -1*(p[1]-ptopleft[1]))
    print 'pos: ', pos

    #sphere
    sphere = SphereShape.create(0.5, 2)
    sphere.setScale(Vector3(1000, 1000, 1000))       
    sphere.setEffect('colored -d red') 
    sphere.setVisible(True)
    sphere.setPosition(pos)
    #all.addChild(sphere)
    station_centers[name] = sphere

    #text
    text = Text3D.create('fonts/arial.ttf', 1, label)
    text.setPosition(Vector3(0.5, -0.2, 0.5))
    #text.setPosition(pos)
    text.setFontResolution(120)
    text.setColor(Color('green'))
    #text.setFacingCamera( getDefaultCamera() )
    sphere.addChild(text)

    #bars
    xspace = 400
    yspace = 1000
    W = 200

    length = 2000
    box1 = BoxShape.create(1,1,1)
    box1.setScale(Vector3(W, length, W))                                                   
    box1.setPosition(Vector3(pos[0] - xspace, pos[1] + yspace + length/2, pos[2]))
    box1.setEffect('colored -e #ff00ff') 
    #all.addChild(box1)

    length = 3000
    box2 = BoxShape.create(1,1,1)
    box2.setScale(Vector3(W, length, W))                                                   
    box2.setPosition(Vector3(pos[0], pos[1] + yspace + length/2, pos[2]))
    box2.setEffect('colored -e #D4D80F') 
    #all.addChild(box2)

    length = 2500
    box3 = BoxShape.create(1,1,1)
    box3.setScale(Vector3(W, length, W))                                                   
    box3.setPosition(Vector3(pos[0] + xspace, pos[1] + yspace + length/2, pos[2]))
    box3.setEffect('colored -e #0C39E8') 
    #all.addChild(box3)

    #wind
    arrow = StaticObject.create("arrow")
    arrow.setScale(Vector3(1000, 100, 100))
    arrow.setPosition(Vector3(pos[0], pos[1] + yspace - 150, pos[2]))
    #arrow.yaw(radians(45))
    arrow.setEffect("colored -d #ffffff")
    
    

# stations
with open(csv_filename, 'rb') as csvfile:
    stations = csv.DictReader(csvfile)
    """
    options = {}
    options["wind_min"] = 1000
    options["wind_max"] = -1000
    options["wind_dir"] = [0, 0, 0]
    for station in stations:
        wind = float(station["WindSpeed"])
        options["wind_min"] = min(options["wind_min"], wind)
        options["wind_max"] = max(options["wind_max"], wind)

    print options
    """
    options = {}
    csvfile.seek(0)
    stations = csv.DictReader(csvfile)
    for station in stations:
        drawStation(station, options)



#menu
mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)

# cameras
menu.addButton("Go to camera 1", 'cam.setPosition(Vector3(27494.30, 3014.47, 152926.58)), cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))')
menu.addButton("Go to default camera", 'cam.setPosition(Vector3(98493.20, 186738.52, 187947.87)), cam.setOrientation(Quaternion(-0.86, 0.50, 0.00, 0.00))')


# ray line
line = LineSet.create()
l = line.addLine()
l.setThickness(0.01)
line.setEffect('colored -e red')
line.setPosition(Vector3(0, 0, 0))
line.setVisible(False)

# ui
# to display stations' names
legend = Container.create(ContainerLayout.LayoutHorizontal, uim.getUi())
with open(csv_filename, 'rb') as csvfile:
    sts = csv.DictReader(csvfile)
    for station in sts:
        lname = Label.create(legend)
        lname.setFont('fonts/arial.ttf 14')
        lname.setText(station['Short'] + ' ' + station['Name'])
legend.setStyle('fill: #aaaaaa80')
legend.setAutosize(True)
legend.setPosition(Vector3(10, 10, 0))

# to display a plot
imgplot = loadImage('testdata/south_west/plots/testplot_low.png')
hplot = Container.create(ContainerLayout.LayoutVertical, uim.getUi())
hplot.setAutosize(True)
#legend.setSize(Vector2(legendimg.getWidth()+10, legendimg.getHeight()+10))
hplot.setPosition(Vector3(10, 60, 0))
lname = Label.create(hplot)
lname.setFont('fonts/arial.ttf 20')
lname.setText('# station name - plot name')
plotview = Image.create(hplot)
plotview.setData(imgplot)
plotview.setPosition(Vector2(2, 2))
hplot.setVisible(False)
hplot.setStyle('fill: #aaaaaa80')

# hud - to display station information
hinfo = Container.create(ContainerLayout.LayoutVertical, uim.getUi())
hinfo.setStyle('fill: #00000080')
hinfo.setHorizontalAlign(HAlign.AlignLeft)
lname = Label.create(hinfo)
ltime = Label.create(hinfo)
lwind = Label.create(hinfo)
ltemp = Label.create(hinfo)
lhum = Label.create(hinfo)
lname.setFont('fonts/arial.ttf 20')
lname.setText("# station name")
ltime.setFont('fonts/arial.ttf 14')
ltime.setText("Time: 2017-04-10 3:00pm")
lwind.setFont('fonts/arial.ttf 14')
lwind.setText("Wind: 20km/h")
ltemp.setFont('fonts/arial.ttf 14')
ltemp.setText("Temp: 19oC")
lhum.setFont('fonts/arial.ttf 14')
lhum.setText("Humi: 70%")
hinfo.setVisible(False)

def onEvent():
    e = getEvent()

    r = getRayFromEvent(e)
    global l
    #l.setStart(r[1])
    #l.setEnd(r[2])

    # On pointer and right click
    if(e.getServiceType() == ServiceType.Pointer and e.isButtonDown(EventFlags.Right) ):
       
        # hitNode on pointCloud node with ray start r[1] and end r[2] points
        #global station_centers
        for name, s in station_centers.iteritems():
            hitData = hitNode(s, r[1], r[2])

            hinfo.setVisible(False)
            if( hitData[0] ):
                print 'hit ', name
                print hitData[1] # print intersection position
             
                p = e.getPosition()
                p[1] = max(p[1] - 100, 0)
                print p
                hinfo.setPosition(p)
                hinfo.setVisible(True)
                lname.setText(name)
                break

                # display plot
                """
                imgplot = None
                imgplot = loadImage('testdata/south_west/plots/testplot_high.png')
                plotview.setData(imgplot)
                hplot.setVisible(True)
                hplot.setSize(Vector2(imgplot.getWidth(), imgplot.getHeight()))
                """             
                

setEventFunction(onEvent)
