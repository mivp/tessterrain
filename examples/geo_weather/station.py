#/usr/bin/env python
import numpy
import csv

from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

class Station:
    def __init__(self, station, uim):
        self.id = station[0]
        self.short = station[1]
        self.name = station[2]
        self.lon = station[3]
        self.lat = station[4]
        self.height = station[5]
        self.position = Vector3(0, 0, 0)

        # sphere
        self.sphere = SphereShape.create(0.5, 2)
        self.sphere.setScale(Vector3(600, 600, 600))       
        self.sphere.setEffect('colored -d red') 
        self.sphere.setVisible(True)
        self.sphere.setPosition(Vector3(0, 0, 0))

        # name
        self.txtName = Text3D.create('fonts/arial.ttf', 0.5, str(self.name))
        self.txtName.setPosition(Vector3(0.7, 0, 0.3))
        #self.txtName.pitch(radians(-30))
        self.txtName.getMaterial().setDoubleFace(True)
        self.sphere.addChild(self.txtName)

        # info hub
        self.infoHud = Container.create(ContainerLayout.LayoutVertical, uim.getUi())
        self.infoHud.setStyle('fill: #00000080')
        self.infoHud.setVisible(False)
        self.infoC3d = self.infoHud.get3dSettings()
        self.infoC3d.enable3d = True
        self.infoC3d.position = Vector3(0, 0, 0)
        self.infoC3d.scale = 20

        self.infoL1 = Label.create(self.infoHud)
        self.infoL2 = Label.create(self.infoHud)
        self.infoL3 = Label.create(self.infoHud)
        self.infoL4 = Label.create(self.infoHud)

        self.infoL1.setFont('fonts/arial.ttf 20')
        self.infoL2.setFont('fonts/arial.ttf 14')
        self.infoL3.setFont('fonts/arial.ttf 14')
        self.infoL4.setFont('fonts/arial.ttf 14')

        # graph
        w = 5
        self.img1 = PlaneShape.create(w, w*430.0/767.0)
        self.img1.setPosition(Vector3(-0.2, 2.4, 0.5))
        self.img1.setEffect("textured -v emissive -d graphs/sample.png")
        #self.img1.roll(radians(90))
        #self.img1.pitch(radians(-5))
        self.img1.setFacingCamera(getDefaultCamera())
        self.img1.getMaterial().setDoubleFace(True)
        self.img1.getMaterial().setTransparent(True)
        self.img1.setVisible(False)
        self.sphere.addChild(self.img1)


    def updatePosition(self, ref_point, hscale_value):
        x = (self.lon - ref_point['lon']) * 36470 / 1.0002777999999921;
        height = hscale_value * self.height + 200
        z = -1 * (self.lat - ref_point['lat']) * 36470 / 1.0002777999999921;
        self.position = Vector3(x, height, z)
        # update
        self.sphere.setPosition(self.position)
        self.infoC3d.position = Vector3(x - 1000, height + 2500, z - 100)

    def updateData(self, data):
        self.infoL1.setText("At " + str(data[1]))
        self.infoL2.setText("Air temp: " + str(data[5]))
        self.infoL3.setText("Wind: " + str(data[8]) + " " + str(data[9]))
        self.infoL4.setText("Hum: " + str(data[7]) + "%")

    def hideAll(self):
        self.infoHud.setVisible(False)
        self.img1.setVisible(False)

    def showInfoHud(self, show = True):
        self.infoHud.setVisible(show)

    def showGraph(self, show = True):
        self.img1.setVisible(show)

    def isLineIntersection(self, lpos, ldir):
        def square(f):
            return f * f
        from math import sqrt
        sp = self.sphere.getPosition();
        r = 1000
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