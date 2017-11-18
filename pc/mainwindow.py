from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtNetwork import *
from PyQt5.QtGui import *
from connectionSettings import ConnectionSettings
import pyqtgraph as pg
import numpy as np
import cv2
from remoteDataExchangerClient import RemoteDataExchangerClient

class MainWindow(QMainWindow):

    WaitingAnimation = [' |', ' /', ' -', ' \\']
    WaitingAnimationFrameDurationMs = 100
    ConnectionTimeoutMs = 5000
    DataExchangeTimerTimeoutMs = 250
    VideoStreamResolution = QSize(640, 480)

    def __init__(self):
        super().__init__()
        self.targetAddr = '0.0.0.0'
        self.targetPort = 0
        QSettings.setPath(QSettings.IniFormat, QSettings.UserScope, QDir.currentPath())
        self.settings = QSettings(QSettings.IniFormat, QSettings.UserScope, 'config')
        self.readSettings()

        self.connectionSettings = ConnectionSettings(self.targetAddr, self.targetPort, self)
        self.connectionSettings.settingsChanged.connect(self.onConnectionSettingsSettingsChanged)
        self.target = RemoteDataExchangerClient(self)
        self.target.stateChanged.connect(self.onTargetStateChanged)
        self.target.variableRead.connect(self.onTargetVariableRead)
        self.target.camFrameRead.connect(self.onTargetCamFrameRead)
        self.lastTargetState = RemoteDataExchangerClient.StateDisconnected
        self.animationTimer = QTimer(self)
        self.animationTimer.timeout.connect(self.onAnimationTimerTimeout)
        self.animationCounter = 0

        self.initUI()
        self.onConnectionSettingsSettingsChanged(self.targetAddr, self.targetPort)

    def initUI(self):

        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')
        self.setWindowTitle('Diag Tool')
        self.statusBar = QStatusBar(self)
        self.targetInfo = QLabel(self)
        self.connectionStatus = QLabel(self)
        self.statusBar.addWidget(self.targetInfo, 0)
        self.statusBar.addWidget(self.connectionStatus, 1)
        self.setStatusBar(self.statusBar)
        self.varTable = QTableWidget(self)
        self.varPlot = pg.PlotWidget(self)
        self.varPlot.setEnabled(False)
        self.console = QTextEdit(self)
        self.console.setTextColor(QColor('white'))
        self.console.setStyleSheet('background-color: black;')
        self.tabWidget = QTabWidget(self)
        self.tabWidget.addTab(self.varPlot, 'Plot')
        self.tabWidget.addTab(self.console, 'Console')
        self.streamView = QLabel(self)
        self.streamView.setSizePolicy(QSizePolicy.MinimumExpanding,
                                      QSizePolicy.MinimumExpanding)
        self.streamView.setFixedSize(self.VideoStreamResolution)
        self.streamView.setStyleSheet('border: 1px solid #BEBEBE; background-color: white')
        self.testButton = QPushButton('test', self)
        self.testButton.clicked.connect(self.onTestButtonClicked)

        layout = QGridLayout(self)
        layout.setContentsMargins(3, 3, 3, 3)
        layout.setSpacing(3)
        layout.addWidget(self.varTable, 0, 0)
        layout.addWidget(self.streamView, 0, 1)
        layout.addWidget(self.tabWidget, 1, 0, 1, 2)
        layout.addWidget(self.testButton, 2, 0)
        self.setCentralWidget(QWidget(self))
        self.centralWidget().setLayout(layout)
        self.menuTarget = self.menuBar().addMenu('Target')
        self.menuTargetConnect = QAction('Connect', self)
        self.menuTargetConnect.triggered.connect(self.onMenuTargetConnect)
        self.menuTargetSettings = QAction('Settings', self)
        self.menuTargetSettings.triggered.connect(self.onMenuTargetSettings)
        self.menuTarget.addAction(self.menuTargetConnect)
        self.menuTarget.addAction(self.menuTargetSettings)

    def closeEvent(self, event):
        self.writeSettings()

    @pyqtSlot(bool)
    def onTestButtonClicked(self, clicked):
        print('testButton')
        self.target.requestCamFrame()

    @pyqtSlot(str, str)
    def onTargetVariableRead(self, varName, varValueStr):
        print(varName + '=' + varValueStr)

    @pyqtSlot(np.ndarray)
    def onTargetCamFrameRead(self, frame):
        pixmap = self.cvToQtPixmap(frame)
        self.streamView.setPixmap(pixmap)

    def cvToQtIm(self, cvIm):
        cvIm = cv2.cvtColor(cvIm, cv2.COLOR_BGR2RGB)
        return QImage(cvIm.data, cvIm.shape[1], cvIm.shape[0], QImage.Format_RGB888)

    def cvToQtPixmap(self, cvIm):
        qtIm = self.cvToQtIm(cvIm)
        return QPixmap.fromImage(qtIm)

    def readSettings(self):
        value = self.settings.value('target/addr')
        if value: self.targetAddr = str(value)
        value = self.settings.value('target/port')
        if value: self.targetPort = int(value)

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
            if RemoteDataExchangerClient.StateConnecting == self.lastTargetState:
                QMessageBox.warning(self, 'Warning', 'Unable to connect!')
        elif RemoteDataExchangerClient.StateConnected == state:
            self.animationTimer.stop()
            self.menuTargetConnect.setText('Disconnect')
            self.menuTargetConnect.setEnabled(True)
            self.connectionStatus.setText('Online')
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