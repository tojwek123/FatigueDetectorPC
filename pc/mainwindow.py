from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtNetwork import *
from PyQt5.QtGui import *
from connectionSettings import ConnectionSettings
import pyqtgraph as pg
import numpy as np
import cv2
import utils
from remoteDataExchangerClient import RemoteDataExchangerClient
from varPlot import VarPlot
from variable import Variable


class MainWindow(QMainWindow):

    WaitingAnimation = [' |', ' /', ' -', ' \\']
    WaitingAnimationFrameDurationMs = 100
    ConnectionTimeoutMs = 5000
    DataReadTimerTimeoutMs = 100
    ReplotTimerTimeoutMs = 100
    VideoStreamResolution = QSize(640, 480)
    VarPlotSamplesNo = 300
    VariablesToRead = ['EAR', 'EARLimit', 'FatigueDetected', 'FPS']
    VarTableColumns = ['Name', 'Value', 'Type', 'Read-only', 'Plot', 'Plot color', 'Description']
    VarTableColumnsNo = {'Name': 0, 'Value': 1, 'Type': 2, 'Read-only': 3, 'Plot': 4, 'Plot color' : 5, 'Description': 6}

    def __init__(self):
        super().__init__()
        self.targetAddr = '0.0.0.0'
        self.targetPort = 0
        QSettings.setPath(QSettings.IniFormat, QSettings.UserScope, QDir.currentPath())
        self.settings = QSettings(QSettings.IniFormat, QSettings.UserScope, 'config')
        self.readSettings()
        self.targetDisconnectClicked = False
        self.connectionSettings = ConnectionSettings(self.targetAddr, self.targetPort, self)
        self.connectionSettings.settingsChanged.connect(self.onConnectionSettingsSettingsChanged)
        self.target = RemoteDataExchangerClient(self)
        self.target.stateChanged.connect(self.onTargetStateChanged)
        self.target.variableTypeRead.connect(self.onTargetVariableTypeRead)
        self.target.variableValueRead.connect(self.onTargetVariableValueRead)
        self.target.camFrameRead.connect(self.onTargetCamFrameRead)
        self.lastTargetState = RemoteDataExchangerClient.StateDisconnected
        self.animationTimer = QTimer(self)
        self.animationTimer.timeout.connect(self.onAnimationTimerTimeout)
        self.animationCounter = 0
        self.dataReadTimer = QTimer(self)
        self.dataReadTimer.timeout.connect(self.onDataReadTimerTimeout)
        self.variables = {}
        self.replotTimer = QTimer(self)
        self.replotTimer.timeout.connect(self.onReplotTimerTimeout)
        self.initUI()
        self.onConnectionSettingsSettingsChanged(self.targetAddr, self.targetPort)

    def initUI(self):
        self.setWindowTitle('Diag Tool')
        self.statusBar = QStatusBar(self)
        self.targetInfo = QLabel(self)
        self.connectionStatus = QLabel('Offline', self)
        self.statusBar.addWidget(self.targetInfo, 0)
        self.statusBar.addWidget(self.connectionStatus, 1)
        self.setStatusBar(self.statusBar)
        self.varPlot = VarPlot(self.VarPlotSamplesNo, self.ReplotTimerTimeoutMs, self)
        self.varTableLabel = QLabel('Variables:', self)
        self.varTable = QTableWidget(self)
        self.varTable.horizontalHeader().setStretchLastSection(True)
        self.varTable.setColumnCount(len(self.VarTableColumnsNo))
        self.varTable.setHorizontalHeaderLabels(self.VarTableColumns)
        self.varTable.setVisible(False)
        for varName in self.VariablesToRead:
            var = Variable(varName, len(self.variables), '', self)
            var.plotClicked.connect(self.varPlot.setPlotVisible)
            var.plotColorChanged.connect(self.varPlot.setPlotColor)
            self.variables.update({varName: var})
            self.varTable.setRowCount(var.getRow() + 1)
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Name'], var.getNameTableItem())
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Value'], var.getValueTableItem())
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Type'], var.getVarTypeTableItem())
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Read-only'], var.getRdOnlyTableItem())
            self.varTable.setCellWidget(var.getRow(), self.VarTableColumnsNo['Plot'], var.getPlotTableWidget())
            self.varTable.setCellWidget(var.getRow(), self.VarTableColumnsNo['Plot color'], var.getPlotColorTableWidget())
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Description'], var.getDescTableItem())
        self.varTableNotAvailableLabel = QLabel('Variables not available')
        self.varTableNotAvailableLabel.setAlignment(Qt.AlignCenter)
        self.varTableNotAvailableLabel.setMinimumWidth(300)
        self.resize(1400, 800)
        self.varTableNotAvailableLabel.setStyleSheet('border: 1px solid #BEBEBE; background-color: white')
        self.console = QTextEdit(self)
        self.console.setTextColor(QColor('white'))
        self.console.setStyleSheet('background-color: black;')
        self.tabWidget = QTabWidget(self)
        self.tabWidget.addTab(self.varPlot, 'Plot')
        self.tabWidget.addTab(self.console, 'Console')
        self.camFrameViewLabel = QLabel('Camera stream:', self)
        self.camFrameView = QLabel(self)
        self.camFrameView.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.camFrameView.setFixedSize(self.VideoStreamResolution)
        self.camFrameView.setStyleSheet('border: 1px solid #BEBEBE; background-color: white')
        self.camFrameView.setAlignment(Qt.AlignCenter)
        self.camFrameView.setText('Camera stream not available')

        layout = QGridLayout(self)
        layout.setContentsMargins(3, 3, 3, 3)
        layout.setSpacing(3)
        layout.addWidget(self.varTableLabel, 0, 0)
        layout.addWidget(self.camFrameViewLabel, 0, 1)
        layout.addWidget(self.varTable, 1, 0)
        layout.addWidget(self.varTableNotAvailableLabel, 1, 0)
        layout.addWidget(self.camFrameView, 1, 1)
        layout.addWidget(self.tabWidget, 2, 0, 1, 2)
        self.setCentralWidget(QWidget(self))
        self.centralWidget().setLayout(layout)
        self.menuTarget = self.menuBar().addMenu('Target')
        self.menuTargetConnect = QAction('Connect', self)
        self.menuTargetConnect.setShortcut('Ctrl+R')
        self.menuTargetConnect.triggered.connect(self.onMenuTargetConnect)
        self.menuTargetSettings = QAction('Settings', self)
        self.menuTargetSettings.setShortcut('Ctrl+E')
        self.menuTargetSettings.triggered.connect(self.onMenuTargetSettings)
        self.menuTarget.addAction(self.menuTargetConnect)
        self.menuTarget.addAction(self.menuTargetSettings)

    def closeEvent(self, event):
        self.writeSettings()

    @pyqtSlot()
    def onReplotTimerTimeout(self):
        for varName in self.variables:
            if varName in self.varPlot:
                self.varPlot.appendValue(varName, float(self.variables[varName].getValueStr()))
        self.varPlot.replot()

    @pyqtSlot(str, str, bool)
    def onTargetVariableTypeRead(self, varName, varTypeStr, varRdOnly):
        if varName in self.variables:
            self.variables[varName].setVarType(varTypeStr)
            self.variables[varName].setRdOnly(varRdOnly)

            if varTypeStr == 'float' or varTypeStr == 'int':
                if varName not in self.varPlot:
                    self.varPlot.addVariable(varName, self.variables[varName].getPlotColor(), self.variables[varName].getPlot())

    @pyqtSlot(str, str)
    def onTargetVariableValueRead(self, varName, varValueStr):
        if varName in self.variables:
            self.variables[varName].setValueStr(varValueStr)

    @pyqtSlot()
    def onDataReadTimerTimeout(self):
        self.target.requestCamFrame()
        for varName in self.variables:
            self.target.requestVariable(varName)

    @pyqtSlot(np.ndarray)
    def onTargetCamFrameRead(self, frame):
        pixmap = utils.cvToQtPixmap(frame)
        self.camFrameView.setPixmap(pixmap)

    def readSettings(self):
        value = self.settings.value('target/addr')
        if value:
            self.targetAddr = str(value)
        value = self.settings.value('target/port')
        if value:
            self.targetPort = int(value)

    def writeSettings(self):
        self.settings.setValue('target/addr', self.targetAddr)
        self.settings.setValue('target/port', self.targetPort)

    @pyqtSlot()
    def onMenuTargetConnect(self):
        if RemoteDataExchangerClient.StateDisconnected == self.target.state():
            self.menuTargetConnect.setEnabled(False)
            self.connectionSettings.setUserInputEnabled(False)
            self.target.connect(self.targetAddr, self.targetPort, self.ConnectionTimeoutMs)
        elif RemoteDataExchangerClient.StateConnected == self.target.state():
            self.targetDisconnectClicked = True
            self.dataReadTimer.stop()
            self.replotTimer.stop()
            self.menuTargetConnect.setEnabled(False)
            self.target.disconnect()

    @pyqtSlot()
    def onMenuTargetSettings(self):
        self.connectionSettings.show()

    @pyqtSlot(int)
    def onTargetStateChanged(self, state):
        if RemoteDataExchangerClient.StateDisconnected == state:
            self.animationTimer.stop()
            self.menuTargetConnect.setText('Connect')
            self.menuTargetConnect.setEnabled(True)
            self.connectionStatus.setText('Offline')
            self.connectionSettings.setUserInputEnabled(True)
            self.camFrameView.setPixmap(QPixmap())
            self.camFrameView.setText('Camera stream not available')
            self.varTable.setVisible(False)
            self.varTableNotAvailableLabel.setVisible(True)
            for varName in self.variables:
                self.variables[varName].setValueStr('')
                self.variables[varName].setVarType('')
                self.variables[varName].setRdOnly(False)
            if RemoteDataExchangerClient.StateConnecting == self.lastTargetState:
                QMessageBox.warning(self, 'Warning', 'Unable to connect.')
            elif not self.targetDisconnectClicked:
                QMessageBox.warning(self, 'Warning', 'Connection has been lost.')
            self.targetDisconnectClicked = False
            self.varPlot.clearData()
        elif RemoteDataExchangerClient.StateConnected == state:
            self.animationTimer.stop()
            self.menuTargetConnect.setText('Disconnect')
            self.menuTargetConnect.setEnabled(True)
            self.connectionStatus.setText('Online')
            self.dataReadTimer.timeout.emit()
            self.dataReadTimer.start(self.DataReadTimerTimeoutMs)
            self.replotTimer.start(self.ReplotTimerTimeoutMs)
            self.varTable.setVisible(True)
            self.varTableNotAvailableLabel.setVisible(False)

            for varName in self.VariablesToRead:
                self.target.requestVariableType(varName)
        elif RemoteDataExchangerClient.StateConnecting == state:
            self.animationTimer.start(self.WaitingAnimationFrameDurationMs)
            self.connectionStatus.setText('Connecting')
            self.animationCounter = 0
        elif RemoteDataExchangerClient.StateDisconnecting == state:
            self.animationTimer.start(self.WaitingAnimationFrameDurationMs)
            self.connectionStatus.setText('Disconnecting')
            self.animationCounter = 0
        self.lastTargetState = state

    @pyqtSlot(str, int)
    def onConnectionSettingsSettingsChanged(self, addr, port):
        self.targetInfo.setText('Target (' + addr + ':' + str(port) + ')')
        self.targetAddr = addr
        self.targetPort = port

    @pyqtSlot()
    def onAnimationTimerTimeout(self):
        if RemoteDataExchangerClient.StateConnecting == self.target.state():
            text = 'Connecting'
        else:
            text = 'Disconnecting'

        text += self.WaitingAnimation[self.animationCounter]
        self.animationCounter = (self.animationCounter + 1) % len(self.WaitingAnimation)
        self.connectionStatus.setText(text)