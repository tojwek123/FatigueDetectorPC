from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from connectionSettings import ConnectionSettings
import numpy as np
import cv2
import utils
from remoteDataExchangerClient import RemoteDataExchangerClient
from varPlot import VarPlot
from variable import Variable
from varValueChanger import VarValueChanger
from dataLogger import DataLogger


class MainWindow(QMainWindow):

    BoundingBoxColor = (255, 0, 0)
    BoundingBoxWidth = 2
    EyesContourColor = (0, 0, 255)
    EyesContourWidth = 2
    WaitingAnimation = [' |', ' /', ' -', ' \\']
    WaitingAnimationFrameDurationMs = 100
    ConnectionTimeoutMs = 5000
    VarPlotUpdateRateMs = 100
    VarPlotReplotPeriodMs = 1000
    VarPlotReplotOnUpdate = 10
    VideoStreamResolution = QSize(480, 360)
    VarPlotSamplesNo = 100
    VariablesToRead = ['EAR', 'EARLimit', 'FatigueDetected', 'FPS']
    VarTableColumns = ['Name', 'Value', 'Type', 'Read-only', 'Plot', 'Plot color', 'Description']
    VarTableColumnsNo = {'Name': 0, 'Value': 1, 'Type': 2, 'Read-only': 3, 'Plot': 4, 'Plot color' : 5, 'Description': 6}

    def __init__(self):
        super().__init__()
        self.targetAddr = '0.0.0.0'
        self.targetPort = 0
        QSettings.setPath(QSettings.IniFormat, QSettings.UserScope, QDir.currentPath())
        self.settings = QSettings(QSettings.IniFormat, QSettings.UserScope, 'config')
        self.dataLogger = DataLogger()
        value = self.settings.value('target/addr')
        if value:
            self.targetAddr = str(value)
        value = self.settings.value('target/port')
        if value:
            self.targetPort = int(value)
        self.targetDisconnectClicked = False
        self.connectionSettings = ConnectionSettings(self.targetAddr, self.targetPort, self)
        self.connectionSettings.settingsChanged.connect(self.onConnectionSettingsSettingsChanged)
        self.target = RemoteDataExchangerClient(self)
        self.target.stateChanged.connect(self.onTargetStateChanged)
        self.target.varInfoRead.connect(self.onTargetVarInfoRead)
        self.target.varValuesRead.connect(self.onTargetVarValuesRead)
        self.target.camFrameRead.connect(self.onTargetCamFrameRead)
        self.lastTargetState = RemoteDataExchangerClient.StateDisconnected
        self.animationTimer = QTimer(self)
        self.animationTimer.timeout.connect(self.onAnimationTimerTimeout)
        self.animationCounter = 0
        self.variables = {}
        self.initUI()
        self.onConnectionSettingsSettingsChanged(self.targetAddr, self.targetPort)
        self.replotCnt = 0

    def initUI(self):
        self.setWindowTitle('Diag Tool')
        self.exportFileDialog = QFileDialog(self, 'Export', '', 'csv table (*.csv)')
        self.exportFileDialog.fileSelected.connect(self.onExportFileDialogFileSelected)
        self.statusBar = QStatusBar(self)
        self.targetInfo = QLabel(self)
        self.connectionStatus = QLabel('Offline', self)
        self.statusBar.addWidget(self.targetInfo, 0)
        self.statusBar.addWidget(self.connectionStatus, 1)
        self.setStatusBar(self.statusBar)
        self.varPlot = VarPlot(self.VarPlotSamplesNo, self.VarPlotUpdateRateMs, self)
        #self.varPlot.setFixedHeight(300)
        self.varPlot.setStyleSheet('border: 1px solid #BEBEBE; background-color: white')
        self.varTableLabel = QLabel('Variables:', self)
        self.varTable = QTableWidget(self)
        self.varTable.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.varTable.setFixedHeight(self.VideoStreamResolution.height())
        self.varTable.horizontalHeader().setStretchLastSection(True)
        self.varTable.setColumnCount(len(self.VarTableColumnsNo))
        self.varTable.setHorizontalHeaderLabels(self.VarTableColumns)
        self.varTable.setVisible(False)
        self.varTable.doubleClicked.connect(self.onVarTableDoubleClicked)
        self.varTableNotAvailableLabel = QLabel('Variables not available')
        self.varTableNotAvailableLabel.setAlignment(Qt.AlignCenter)
        self.varTableNotAvailableLabel.setMinimumWidth(300)
        self.varTableNotAvailableLabel.setStyleSheet('border: 1px solid #BEBEBE; background-color: white')
        #self.console = QTextEdit(self)
        #self.console.setStyleSheet('border: 1px solid #BEBEBE; background-color: white')
        #self.console.setTextColor(QColor('white'))
        #self.console.setStyleSheet('background-color: black;')
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
        layout.addWidget(QLabel('Plot:'), 2, 0, 1, 2)
        layout.addWidget(self.varPlot, 3, 0, 1, 2)
        #layout.addWidget(QLabel('Console:'), 4, 0, 1, 2)
        #layout.addWidget(self.console, 5, 0, 1, 2)
        self.setCentralWidget(QWidget(self))
        self.centralWidget().setLayout(layout)
        self.menuTarget = self.menuBar().addMenu('Target')
        self.menuTargetConnect = QAction('Connect', self)
        self.menuTargetConnect.setShortcut('Ctrl+R')
        self.menuTargetConnect.triggered.connect(self.onMenuTargetConnect)
        self.menuTargetSettings = QAction('Settings', self)
        self.menuTargetSettings.setShortcut('Ctrl+T')
        self.menuTargetSettings.triggered.connect(self.onMenuTargetSettings)
        self.menuTarget.addAction(self.menuTargetConnect)
        self.menuTarget.addAction(self.menuTargetSettings)
        self.menuCameraStream = self.menuBar().addMenu('Camera Stream')
        self.menuCameraStreamDrawBoundingBox = QAction('Draw Bounding Box', self)
        self.menuCameraStreamDrawBoundingBox.setShortcut('Ctrl+B')
        self.menuCameraStreamDrawBoundingBox.setCheckable(True)
        value = self.settings.value('cameraStream/drawBoundingBox')
        self.menuCameraStreamDrawBoundingBox.setChecked(value == 'true')
        self.menuCameraStreamDrawEyes = QAction('Draw Eyes', self)
        self.menuCameraStreamDrawEyes.setShortcut('Ctrl+E')
        self.menuCameraStreamDrawEyes.setCheckable(True)
        value = self.settings.value('cameraStream/drawEyes')
        self.menuCameraStreamDrawEyes.setChecked(value == 'true')
        self.menuCameraStream.addAction(self.menuCameraStreamDrawBoundingBox)
        self.menuCameraStream.addAction(self.menuCameraStreamDrawEyes)
        self.menuPlot = self.menuBar().addMenu('Plot')
        self.menuPlotExport = QAction('Export to csv', self)
        self.menuPlotExport.triggered.connect(self.onMenuPlotExport)
        self.menuPlotExport.setShortcut('Ctrl+S')
        self.menuPlotExport.setEnabled(False)
        self.menuPlotClear = QAction('Clear', self)
        self.menuPlotClear.setShortcut('Ctrl+D')
        self.menuPlotClear.triggered.connect(self.onMenuPlotClear)
        self.menuPlot.addAction(self.menuPlotExport)
        self.menuPlot.addAction(self.menuPlotClear)
        self.varValueChanger = VarValueChanger(self)
        self.varValueChanger.valueChanged.connect(self.onVarValueChangerValueChanged)
        self.resize(900, 640)

    def closeEvent(self, event):
        self.writeSettings()

    @pyqtSlot()
    def onMenuPlotExport(self):
        self.exportFileDialog.show()

    @pyqtSlot(str)
    def onExportFileDialogFileSelected(self, file):
        self.dataLogger.save(file)

    @pyqtSlot(str, str)
    def onVarValueChangerValueChanged(self, varName, varValueStr):
        self.target.setVarValue(varName, varValueStr)

    @pyqtSlot(QModelIndex)
    def onVarTableDoubleClicked(self, modelIndex):
        if modelIndex.column() == self.VarTableColumnsNo['Value']:
            varName = self.varTable.item(modelIndex.row(), self.VarTableColumnsNo['Name']).data(Qt.DisplayRole)
            varReadOnly = self.variables[varName].getRdOnly()
            if not varReadOnly:
                varValStr = self.variables[varName].getValueStr()
                varType = self.variables[varName].getVarType()
                self.varValueChanger.changeValue(varName, varValStr, varType)
            else:
                QMessageBox.warning(self, 'Warning', 'Cannot change value. Variable is read-only.')

    @pyqtSlot(list)
    def onTargetVarInfoRead(self, varInfo):
        for i in varInfo:
            var = Variable(i['name'], len(self.variables), '', self)
            var.setVarType(i['type'])
            var.setRdOnly(i['readOnly'])
            var.setValueStr(i['valStr'])
            var.plotClicked.connect(self.varPlot.setPlotVisible)
            var.plotColorChanged.connect(self.varPlot.setPlotColor)
            self.variables.update({i['name']: var})
            self.varTable.setRowCount(var.getRow() + 1)
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Name'], var.getNameTableItem())
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Value'], var.getValueTableItem())
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Type'], var.getVarTypeTableItem())
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Read-only'], var.getRdOnlyTableItem())
            self.varTable.setCellWidget(var.getRow(), self.VarTableColumnsNo['Plot'], var.getPlotTableWidget())
            self.varTable.setCellWidget(var.getRow(), self.VarTableColumnsNo['Plot color'],
                                        var.getPlotColorTableWidget())
            self.varTable.setItem(var.getRow(), self.VarTableColumnsNo['Description'], var.getDescTableItem())

            if i['type'] == 'float' or i['type'] == 'int' or i['type'] == 'bool':
                self.varPlot.addVariable(var.getName(), var.getPlotColor(), var.getPlot())

        self.target.startVarStream()

    @pyqtSlot(list)
    def onTargetVarValuesRead(self, varValues):
        for i in varValues:
            varName = i['name']
            varValStr = i['valStr']
            varType = self.variables[varName].getVarType()
            self.variables[varName].setValueStr(varValStr)
            if varType == 'float':
                try:
                    self.varPlot.appendValue(varName, float(varValStr))
                except ValueError:
                    continue
            elif varType == 'int':
                try:
                    self.varPlot.appendValue(varName, int(varValStr))
                except ValueError:
                    continue
            elif varType == 'bool':
                try:
                    self.varPlot.appendValue(varName, int('true' == varValStr))
                except ValueError:
                    continue

        self.varPlot.updateXScale()
        self.replotCnt += 1
        if self.replotCnt >= self.VarPlotReplotOnUpdate:
            self.varPlot.replot()
            self.replotCnt = 0

        self.dataLogger.appendData(varValues)

    @pyqtSlot(np.ndarray)
    def onTargetCamFrameRead(self, frame):
        if 'faceBoundingBox' in self.variables and self.menuCameraStreamDrawBoundingBox.isChecked():
            #Draw bounding box
            try:
                rectPt = eval(self.variables['faceBoundingBox'].getValueStr())
                if len(rectPt) > 3:
                    cv2.rectangle(frame, (rectPt[0], rectPt[1]), (rectPt[2], rectPt[3]), self.BoundingBoxColor, self.BoundingBoxWidth)
            except:
                pass

        #Draw eyes
        if 'leftEye' in self.variables and 'rightEye' in self.variables and self.menuCameraStreamDrawEyes.isChecked():
            try:
                leftEyePt = eval(self.variables['leftEye'].getValueStr())
                for i in range(0, len(leftEyePt), 2):
                    pt1 = (leftEyePt[i % len(leftEyePt)], leftEyePt[(i + 1) % len(leftEyePt)])
                    pt2 = (leftEyePt[(i + 2) % len(leftEyePt)], leftEyePt[(i + 3) % len(leftEyePt)])
                    cv2.line(frame, pt1, pt2, self.EyesContourColor, self.EyesContourWidth)
                rightEyePt = eval(self.variables['rightEye'].getValueStr())
                for i in range(0, len(rightEyePt), 2):
                    pt1 = (rightEyePt[i % len(rightEyePt)], rightEyePt[(i + 1) % len(rightEyePt)])
                    pt2 = (rightEyePt[(i + 2) % len(rightEyePt)], rightEyePt[(i + 3) % len(rightEyePt)])
                    cv2.line(frame, pt1, pt2, self.EyesContourColor, self.EyesContourWidth)
            except:
                pass
        frame = cv2.resize(frame, (self.VideoStreamResolution.width(), self.VideoStreamResolution.height()))
        pixmap = utils.cvToQtPixmap(frame)
        self.camFrameView.setPixmap(pixmap)

    def writeSettings(self):
        self.settings.setValue('target/addr', self.targetAddr)
        self.settings.setValue('target/port', self.targetPort)
        self.settings.setValue('cameraStream/drawBoundingBox', self.menuCameraStreamDrawBoundingBox.isChecked())
        self.settings.setValue('cameraStream/drawEyes', self.menuCameraStreamDrawEyes.isChecked())

    @pyqtSlot()
    def onMenuPlotClear(self):
        self.varPlot.clearData()
        self.dataLogger.clear()

    @pyqtSlot()
    def onMenuTargetConnect(self):
        if RemoteDataExchangerClient.StateDisconnected == self.target.state():
            self.menuTargetConnect.setEnabled(False)
            self.connectionSettings.setUserInputEnabled(False)
            self.target.connect(self.targetAddr, self.targetPort, self.ConnectionTimeoutMs)
        elif RemoteDataExchangerClient.StateConnected == self.target.state():
            self.target.stopVideoStream()
            self.target.stopVarStream()
            self.replotCnt = 0
            self.targetDisconnectClicked = True
            self.menuTargetConnect.setEnabled(False)
            self.menuPlotExport.setEnabled(False)
            self.dataLogger.stop()
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
            self.varTable.setRowCount(0)
            self.variables = {}
            self.varTableNotAvailableLabel.setVisible(True)
            for varName in self.variables:
                self.variables[varName].setValueStr('')
                self.variables[varName].setVarType('')
                self.variables[varName].setRdOnly(False)
            self.varPlot.clearData()
            if RemoteDataExchangerClient.StateConnecting == self.lastTargetState:
                QMessageBox.warning(self, 'Warning', 'Unable to connect.')
            elif not self.targetDisconnectClicked:
                QMessageBox.warning(self, 'Warning', 'Connection has been lost.')
            self.targetDisconnectClicked = False
        elif RemoteDataExchangerClient.StateConnected == state:
            self.animationTimer.stop()
            self.menuTargetConnect.setText('Disconnect')
            self.menuTargetConnect.setEnabled(True)
            self.menuPlotExport.setEnabled(True)
            self.dataLogger.start()
            self.connectionStatus.setText('Online')
            self.varTable.setVisible(True)
            self.varTableNotAvailableLabel.setVisible(False)
            self.target.startVideoStream()
            self.target.requestVarInfo()
            self.replotCnt = 0
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