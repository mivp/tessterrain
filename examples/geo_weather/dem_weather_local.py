# /usr/bin/env python
import numpy
import csv

from math import *
from euclid import *
from omega import *
from cyclops import *

csv_filename = "testdata/vic_usgs/south_west_stations.csv"
# 1.0002777999999921 degree ~ 36470 m ~ 3648 pixels
# ref_point (lat, long) : -36.9998611,140.9998611
ref_point = {}
ref_point['lat'] = -36.9998611
ref_point['lon'] = 140.9998611
data_height_scale = 0.4  # because x, z values were scaled smaller when preparing data

# model
scene = getSceneManager()
all = SceneNode.create("everything")
# ui
uim = UiModule.createAndInitialize()

station_centers = {}

# other settings
cam = getDefaultCamera()
cam.setPosition(Vector3(46930.8, 7805.12, 65433.8))
cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))
cam.getController().setSpeed(4000)
setNearFarZ(2, 400000)

# Create a directional light
light1 = Light.create()
light1.setLightType(LightType.Directional)
light1.setLightDirection(Vector3(-1.0, -1.0, -1.0))
light1.setColor(Color(1.0, 1.0, 1.0, 1.0))
light1.setAmbient(Color(0.2, 0.2, 0.2, 1.0))
light1.setEnabled(True)


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

    norm = numpy.linalg.norm(winddir, ord=2)
    if (norm == 0):
        return numpy.asarray([0, 0, 0])
    return winddir / norm


def drawStation(station, options):
    print station["Short"], station["Name"], station["Lat"], station["Lon"], station["Height"]
    lat = float(station["Lat"])
    lon = float(station["Lon"])
    height = float(station["Height"]) + 200
    label = station["Short"]
    name = station["Name"]

    # calculate position
    x = (lon - ref_point['lon']) * 36470 / 1.0002777999999921;
    z = -1 * (lat - ref_point['lat']) * 36470 / 1.0002777999999921;
    pos = Vector3(x, height, z)
    print 'pos: ', pos

    # sphere
    global station_centers
    sphere = SphereShape.create(0.5, 2)
    sphere.setScale(Vector3(600, 600, 600))
    sphere.setEffect('colored -e red')
    sphere.setVisible(True)
    sphere.setPosition(pos)
    # all.addChild(sphere)
    station_centers[name] = sphere

    # text
    text = Text3D.create('fonts/arial.ttf', 0.6, name)
    text.setPosition(Vector3(0.5, 0.4, 0.5))
    # text.setPosition(pos)
    text.setFontResolution(120)
    text.setColor(Color('green'))
    # text.setFacingCamera( getDefaultCamera() )
    sphere.addChild(text)

    # bars
    xspace = 300
    yspace = 500
    W = 150

    length = 1500
    box1 = BoxShape.create(1, 1, 1)
    box1.setScale(Vector3(W, length, W))
    box1.setPosition(Vector3(pos[0] - xspace, pos[1] + yspace + length / 2, pos[2]))
    box1.setEffect('colored -e #ff00ff')
    # all.addChild(box1)

    length = 2500
    box2 = BoxShape.create(1, 1, 1)
    box2.setScale(Vector3(W, length, W))
    box2.setPosition(Vector3(pos[0], pos[1] + yspace + length / 2, pos[2]))
    box2.setEffect('colored -e #D4D80F')
    # all.addChild(box2)

    length = 2000
    box3 = BoxShape.create(1, 1, 1)
    box3.setScale(Vector3(W, length, W))
    box3.setPosition(Vector3(pos[0] + xspace, pos[1] + yspace + length / 2, pos[2]))
    box3.setEffect('colored -e #0C39E8')
    # all.addChild(box3)


# stations
with open(csv_filename, 'rb') as csvfile:
    stations = csv.DictReader(csvfile)
    options = {}
    """
    options["wind_min"] = 1000
    options["wind_max"] = -1000
    options["wind_dir"] = [0, 0, 0]
    for station in stations:
        wind = float(station["WindSpeed"])
        options["wind_min"] = min(options["wind_min"], wind)
        options["wind_max"] = max(options["wind_max"], wind)

    print options
    """
    csvfile.seek(0)
    stations = csv.DictReader(csvfile)
    for station in stations:
        drawStation(station, options)

# menu
mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)

# cameras
menu.addButton("Go to camera 1",
               'cam.setPosition(Vector3(46930.8, 7805.12, 65433.8)), cam.setOrientation(Quaternion(-0.99, 0.07, 0.07, 0.01))')

# ray line
line = LineSet.create()
l = line.addLine()
l.setThickness(0.02)
line.setEffect('colored -e red')
line.setPosition(Vector3(0, 0, 0))

# ui
# to display stations' names
legend = Container.create(ContainerLayout.LayoutHorizontal, uim.getUi())
with open(csv_filename, 'rb') as csvfile:
    sts = csv.DictReader(csvfile)
    for station in sts:
        lname = Label.create(legend)
        lname.setFont('fonts/arial.ttf 100')
        lname.setText(station['Short'] + ' ' + station['Name'])
legend.setStyle('fill: #aaaaaa80')
legend.setAutosize(True)
legend.setPosition(Vector3(10000, 50, 0))
legend.setVisible(False)

# to display a plot
imgplot = loadImage('testdata/south_west/plots/testplot_high.png')
hplot = Container.create(ContainerLayout.LayoutVertical, uim.getUi())
hplot.setAutosize(True)
# legend.setSize(Vector2(legendimg.getWidth()+10, legendimg.getHeight()+10))
hplot.setPosition(Vector3(10000, 200, 0))
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
hinfo.setPosition(Vector3(11600, 200, 0))
lname = Label.create(hinfo)
ltime = Label.create(hinfo)
lwind = Label.create(hinfo)
ltemp = Label.create(hinfo)
lhum = Label.create(hinfo)
lname.setFont('fonts/arial.ttf 50')
lname.setText("# station name")
ltime.setFont('fonts/arial.ttf 40')
ltime.setText("Time: 2017-04-10 3:00pm")
lwind.setFont('fonts/arial.ttf 40')
lwind.setText("Wind: 20km/h")
ltemp.setFont('fonts/arial.ttf 40')
ltemp.setText("Temp: 19oC")
lhum.setFont('fonts/arial.ttf 40')
lhum.setText("Humi: 70%")
hinfo.setVisible(False)


def isSphereLineIntersection(sp, r, lpos, ldir):
    def square(f):
        return f * f

    from math import sqrt

    a = square(ldir[0]) + square(ldir[1]) + square(ldir[2])
    b = 2.0 * ((ldir[0]) * (lpos[0] - sp[0]) +
               (ldir[1]) * (lpos[1] - sp[1]) +
               (ldir[2]) * (lpos[2] - sp[2]))

    c = (square(sp[0]) + square(sp[1]) + square(sp[2]) + square(lpos[0]) +
         square(lpos[1]) + square(lpos[2]) -
         2.0 * (sp[0] * lpos[0] + sp[1] * lpos[1] + sp[2] * lpos[2]) - square(r))

    i = b * b - 4.0 * a * c

    if i < 0.0:
        return False
    return True


def onEvent():
    e = getEvent()

    r = getRayFromEvent(e)
    global l
    l.setStart(r[1])
    l.setEnd(r[1] + 100 * r[2])

    # On pointer and right click
    if (e.getServiceType() == ServiceType.Pointer and e.isButtonDown(EventFlags.Right)) or \
            (e.getServiceType() == ServiceType.Wand and e.isButtonDown(EventFlags.Button5)):

        # hitNode on pointCloud node with ray start r[1] and end r[2] points
        global station_centers
        for name, s in station_centers.iteritems():
            # print name, s, r[1], r[2]
            hit = isSphereLineIntersection(s.getPosition(), 1000, r[1], r[2])

            # hitData = hitNode(s, r[1], r[2])
            hinfo.setVisible(False)
            hplot.setVisible(False)
            if (hit):
                print 'hit ', name
                # print hitData[1] # print intersection position
                p = e.getPosition()
                # p[1] = max(p[1] - 100, 0)
                # print p
                # hinfo.setPosition(p)
                hinfo.setVisible(True)
                lname.setText(name)
                # global c3d
                # c3d.position = s.getPosition()

                # display plot
                imgplot = None
                imgplot = loadImage('testdata/south_west/plots/testplot_high2.png')
                plotview.setData(imgplot)
                hplot.setVisible(True)
                hplot.setSize(Vector2(imgplot.getWidth(), imgplot.getHeight()))
                break

    if (e.getServiceType() == ServiceType.Wand and e.isButtonDown(EventFlags.Button4)):
        tt.nextDisplayMode()


setEventFunction(onEvent)

queueCommand(":freefly")

