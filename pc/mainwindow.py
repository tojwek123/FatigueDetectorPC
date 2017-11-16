from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtNetwork import *
from PyQt5.QtGui import *
from connectionSettings import ConnectionSettings
import pyqtgraph as pg
import numpy as np
import cv2


class MainWindow(QMainWindow):

    WaitingAnimation = [' |', ' /', ' -', ' \\']
    WaitingAnimationFrameDurationMs = 100
    ConnectionTimerTimeoutMs = 5000
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
        self.socket = QTcpSocket(self)
        self.socket.stateChanged.connect(self.onSocketStateChanged)
        self.socket.readyRead.connect(self.onSocketReadyRead)
        self.frameLength = 0
        self.lastSocketState = QAbstractSocket.UnconnectedState
        self.animationTimer = QTimer(self)
        self.animationTimer.timeout.connect(self.onAnimationTimerTimeout)
        self.animationCounter = 0
        self.connectionTimer = QTimer(self)
        self.connectionTimer.setSingleShot(True)
        self.connectionTimer.timeout.connect(self.onConnectionTimerTimeout)
        self.dataExchangeTimer = QTimer(self)
        self.dataExchangeTimer.timeout.connect(self.onDataExchangeTimerTimeout)

        self.initUI()
        self.onConnectionSettingsSettingsChanged(self.targetAddr, self.targetPort)
        self.onSocketStateChanged()

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
        layout = QGridLayout(self)
        layout.setContentsMargins(3, 3, 3, 3)
        layout.setSpacing(3)
        layout.addWidget(self.varTable, 0, 0)
        layout.addWidget(self.streamView, 0, 1)
        layout.addWidget(self.tabWidget, 1, 0, 1, 2)
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

    def cvToQtIm(self, cvIm):
        cvIm = cv2.cvtColor(cvIm, cv2.COLOR_BGR2RGB)
        return QImage(cvIm.data, cvIm.shape[1], cvIm.shape[0], QImage.Format_RGB888)

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
        if QAbstractSocket.UnconnectedState == self.socket.state():
            self.menuTargetConnect.setEnabled(False)
            self.connectionSettings.setUserInputEnabled(False)
            self.socket.connectToHost(QHostAddress(self.targetAddr), self.targetPort)
            self.connectionTimer.start(self.ConnectionTimerTimeoutMs)
        elif QAbstractSocket.ConnectedState == self.socket.state():
            self.menuTargetConnect.setEnabled(False)
            self.socket.disconnectFromHost()

    @pyqtSlot()
    def onMenuTargetSettings(self):
        self.connectionSettings.show()

    @pyqtSlot()
    def onSocketStateChanged(self):
        #print(self.socket.state())
        if QAbstractSocket.UnconnectedState == self.socket.state():
            self.dataExchangeTimer.timeout.emit()
            self.dataExchangeTimer.start(self.DataExchangeTimerTimeoutMs)
            self.animationTimer.stop()
            self.menuTargetConnect.setText('Connect')
            self.menuTargetConnect.setEnabled(True)
            self.connectionStatus.setText('Offline')
            self.connectionSettings.setUserInputEnabled(True)
            if QAbstractSocket.ConnectingState == self.lastSocketState:
                QMessageBox.warning(self, 'Warning', 'Unable to connect!')
        elif QAbstractSocket.ConnectedState == self.socket.state():
            self.connectionTimer.stop()
            self.animationTimer.stop()
            self.dataExchangeTimer.start()
            self.menuTargetConnect.setText('Disconnect')
            self.menuTargetConnect.setEnabled(True)
            self.connectionStatus.setText('Online')
        elif QAbstractSocket.ConnectingState == self.socket.state():
            self.connectionStatus.setText('Connecting')
            self.animationCounter = 0
            self.animationTimer.start(self.WaitingAnimationFrameDurationMs)
        elif QAbstractSocket.ClosingState == self.socket.state():
            self.connectionStatus.setText('Disconnecting')
            self.animationCounter = 0
            self.animationTimer.start(self.WaitingAnimationFrameDurationMs)
        self.lastSocketState = self.socket.state()

    @pyqtSlot()
    def onSocketReadyRead(self):
        if 0 == self.frameLength:
            if self.socket.canReadLine():
                data = self.socket.readLine()[:-1]
                self.frameLength = int(data)

        if self.frameLength != 0:
            if self.socket.bytesAvailable() >= self.frameLength:
                data = self.socket.read(self.frameLength)
                arr = np.frombuffer(data, dtype=np.uint8)
                cvIm = cv2.imdecode(arr, cv2.IMREAD_COLOR)
                self.frameLength = 0
                image = self.cvToQtIm(cvIm)
                pixmap = QPixmap.fromImage(image)
                self.streamView.setPixmap(pixmap)

    @pyqtSlot(str, int)
    def onConnectionSettingsSettingsChanged(self, addr, port):
        self.targetInfo.setText('Target (' + addr + ':' + str(port) + ')')
        self.targetAddr = addr
        self.targetPort = port

    @pyqtSlot()
    def onAnimationTimerTimeout(self):
        if QAbstractSocket.ConnectingState == self.socket.state():
            text = 'Connecting'
        else:
            text = 'Disconnecting'

        text += self.WaitingAnimation[self.animationCounter]
        self.animationCounter = (self.animationCounter + 1) % len(self.WaitingAnimation)
        self.connectionStatus.setText(text)

    @pyqtSlot()
    def onConnectionTimerTimeout(self):
        self.socket.abort()

    @pyqtSlot()
    def onDataExchangeTimerTimeout(self):
        if QAbstractSocket.ConnectedState == self.socket.state():
            self.socket.writeData('get\n'.encode())