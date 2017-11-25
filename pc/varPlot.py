from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
import pyqtgraph as pg
import math


class VarPlot(pg.PlotWidget):

    PlotPenWidth = 0.1
    TicksNo = 12

    def __init__(self, samplesNo=1000, updateRateMs=100, parent=None):
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')
        pg.setConfigOptions(antialias=True)
        super().__init__(parent, title='')

        #Workaround - cut axis
        self.showAxis('right')
        self.getAxis('right').setTicks([])

        self.updateRateMs = updateRateMs
        self.ticksStr = []
        self.ticksVal = []
        self.samplesNo = samplesNo
        self.resetTicks()
        self.setEnabled(False)
        self.showGrid(True, True)
        self.setRange(xRange=[0, samplesNo], yRange=[0,10], padding=0)
        self.samplesNo = samplesNo
        self.data = {}
        self.dataLen = {}
        self.plotColor = {}
        self.plotVisible = {}
        self.plotPen = {}


    def __contains__(self, key):
        return key in self.data

    def resetTicks(self):
        self.ticksStr = []
        self.ticksVal = []
        tickInc = int(self.samplesNo / self.TicksNo)
        for i in range(tickInc, self.samplesNo, tickInc):
            value = i * self.updateRateMs / 1000
            tickStr = '{:.1f}s'.format(value)
            self.ticksVal.append(value)
            self.ticksStr.append((i, tickStr))
        self.getAxis('bottom').setTicks([self.ticksStr])

    def addVariable(self, varName, color, visible):
        self.data.update({varName: []})
        self.dataLen.update({varName: 0})
        self.plotColor.update({varName: color})
        self.plotVisible.update({varName: visible})
        self.plotPen.update({varName: QPen(color, self.PlotPenWidth)})

    def appendValue(self, varName, varValue):
        if varName in self.data:
            self.data[varName].append(varValue)
            self.dataLen[varName] += 1
            if len(self.data[varName]) > self.samplesNo + 1:
                self.data[varName] = self.data[varName][1:]

    def clearData(self):
        for varName in self.data:
            self.data[varName] = []
            self.dataLen[varName] = 0
        self.resetTicks()
        self.replot()

    def replot(self):
        extremumFound = False

        self.plot([], clear=True)
        for varName in self.data:
            if self.plotVisible[varName]:
                self.plot(self.data[varName], pen=self.plotPen[varName], clear=False)

                if len(self.data[varName]) > 0:
                    localMin = min(self.data[varName])
                    localMax = max(self.data[varName])

                    if not extremumFound:
                        extremumFound = True
                        minValue = localMin
                        maxValue = localMax

                    if localMin < minValue:
                        minValue = localMin
                    if localMax > maxValue:
                        maxValue = localMax

        if not extremumFound:
            minValue = 0
            maxValue = 10
        else:
            minValue = math.floor(minValue)
            if minValue % 2 != 0:
                minValue -= 1

            maxValue = math.ceil(maxValue)
            if maxValue % 2 != 0:
                maxValue += 1

        self.setYRange(minValue, maxValue, 0)

        if (max(self.dataLen.values())) > self.samplesNo:
            for i in range(len(self.ticksVal)):
               self.ticksVal[i] += self.updateRateMs / 1000
               self.ticksStr[i] = (self.ticksStr[i][0], '{:.1f}s'.format(self.ticksVal[i]))
            self.getAxis('bottom').setTicks([self.ticksStr])

    @pyqtSlot(str, bool)
    def setPlotVisible(self, varName, visible):
        self.plotVisible[varName] = visible

    @pyqtSlot(str, QColor)
    def setPlotColor(self, varName, color):
        self.plotPen[varName] = QPen(color, self.PlotPenWidth)