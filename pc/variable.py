from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
import utils

nextColor = 0


class Variable(QObject):

    Colors = [Qt.blue, Qt.red, Qt.darkGreen, Qt.darkYellow, Qt.darkCyan]
    plotClicked = pyqtSignal(str, bool)
    plotColorChanged = pyqtSignal(str, QColor)

    def __init__(self, name, row, desc='', parent=None):
        global nextColor
        super().__init__(parent)

        self.name = name
        self.row = row
        self.valueStr = ''
        self.varType = ''
        self.rdOnly = False
        self.plot = False
        self.plotColor = self.Colors[nextColor]
        self.desc = desc

        nextColor = (nextColor + 1) % len(self.Colors)

        self.nameTableItem = QTableWidgetItem(self.name)
        self.nameTableItem.setTextAlignment(Qt.AlignCenter)
        self.valueTableItem = QTableWidgetItem(self.valueStr)
        self.valueTableItem.setTextAlignment(Qt.AlignCenter)
        self.varTypeTableItem = QTableWidgetItem(self.varType)
        self.varTypeTableItem.setTextAlignment(Qt.AlignCenter)
        self.rdOnlyTableItem = QTableWidgetItem(str(self.rdOnly))
        self.rdOnlyTableItem.setTextAlignment(Qt.AlignCenter)

        self.plotTableWidgetCheckbox = QCheckBox()
        self.plotTableWidgetCheckbox.clicked.connect(self.onPlotTableWidgetCheckboxClicked)
        self.plotTableWidgetCheckbox.setVisible(False)
        self.plotTableWidgetCheckbox.setChecked(self.plot)
        self.plotTableWidgetLayout = QVBoxLayout()
        self.plotTableWidgetLayout.addWidget(self.plotTableWidgetCheckbox)
        self.plotTableWidgetLayout.setAlignment(Qt.AlignCenter)
        self.plotTableWidgetLayout.setContentsMargins(0, 0, 0, 0)
        self.plotTableWidget = QWidget(parent)
        self.plotTableWidget.setLayout(self.plotTableWidgetLayout)

        self.plotColorTableWidgetButton = QPushButton()
        self.plotColorTableWidgetButton.clicked.connect(self.onPlotColorTableWidgetButtonClicked)
        self.plotColorTableWidgetButton.setVisible(False)
        self.plotColorTableWidgetButton.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.plotColorTableWidgetLayout = QVBoxLayout()
        self.plotColorTableWidgetLayout.addWidget(self.plotColorTableWidgetButton)
        utils.setButtonColor(self.plotColorTableWidgetButton, self.plotColor)
        self.plotColorTableWidget = QWidget(parent)
        self.plotColorTableWidget.setLayout(self.plotColorTableWidgetLayout)

        self.descTableItem = QTableWidgetItem(self.desc)
        self.descTableItem.setTextAlignment(Qt.AlignCenter)

    @pyqtSlot(bool)
    def onPlotTableWidgetCheckboxClicked(self, state):
        self.plotClicked.emit(self.name, state)

    @pyqtSlot(bool)
    def onPlotColorTableWidgetButtonClicked(self, state):
        color = QColorDialog.getColor()
        if color.isValid():
            palette = self.plotColorTableWidgetButton.palette()
            palette.setColor(QPalette.Button, color)
            self.plotColorTableWidgetButton.setPalette(palette)
            self.plotColorChanged.emit(self.name, color)

    def setName(self, name):
        self.nameTableItem.setText(name)
        self.name = name

    def setRow(self, row):
        self.row = row

    def setValueStr(self, valueStr):
        self.valueTableItem.setText(valueStr)
        self.valueStr = valueStr

    def setVarType(self, varType):
        self.varTypeTableItem.setText(varType)
        self.varType = varType
        visibility = (varType == 'float' or varType == 'int')
        self.plotTableWidgetCheckbox.setVisible(visibility)
        self.plotColorTableWidgetButton.setVisible(visibility)

    def setRdOnly(self, rdOnly):
        self.rdOnlyTableItem.setText(str(rdOnly))
        self.rdOnly = rdOnly

    def setPlot(self, plot):
        self.plotTableWidgetCheckbox.setChecked(plot)
        self.plot = plot

    def setPlotColor(self, plotColor):
        utils.setButtonColor(self.plotColorTableWidgetButton, plotColor)
        self.plotColor = plotColor

    def setDesc(self, desc):
        self.descTableItem.setText(desc)
        self.desc = desc

    def getName(self):
        return self.name

    def getRow(self):
        return self.row

    def getValueStr(self):
        return self.valueStr

    def getVarType(self):
        return self.varType

    def getRdOnly(self):
        return self.rdOnly

    def getPlot(self):
        return self.plot

    def getPlotColor(self):
        return self.plotColor

    def getDesc(self):
        return self.desc

    def getNameTableItem(self):
        return self.nameTableItem

    def getValueTableItem(self):
        return self.valueTableItem

    def getVarTypeTableItem(self):
        return self.varTypeTableItem

    def getRdOnlyTableItem(self):
        return self.rdOnlyTableItem

    def getPlotTableWidget(self):
        return self.plotTableWidget

    def getPlotColorTableWidget(self):
        return self.plotColorTableWidget

    def getDescTableItem(self):
        return self.descTableItem