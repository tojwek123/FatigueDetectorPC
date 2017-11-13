from PyQt5.QtCore import pyqtSlot
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtWidgets import QStatusBar
from PyQt5.QtWidgets import QLabel
from PyQt5.QtWidgets import QAction
from PyQt5.QtWidgets import QMessageBox
from PyQt5.QtWidgets import QGridLayout
from PyQt5.QtWidgets import QSizePolicy
from PyQt5.QtWidgets import QWidget
from PyQt5.QtWidgets import QPushButton
from PyQt5.QtCore import QTimer
from PyQt5.QtNetwork import QTcpSocket, QHostAddress, QAbstractSocket
from PyQt5.QtCore import QSettings
from PyQt5.QtCore import QDir
from PyQt5.QtGui import QImage, QPixmap
from connectionsettings import ConnectionSettings
import numpy as np
import cv2

class MainWindow(QMainWindow):

    WaitingAnimation = [' |', ' /', ' -', ' \\']
    WaitingAnimationFrameDurationMs = 100
    ConnectionTimerTimeoutMs = 5000
    DataExchangeTimerTimeoutMs = 250

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

    #     self.cap = cv2.VideoCapture(0)
    #     self.dispTimer = QTimer(self)
    #     self.dispTimer.timeout.connect(self.onDispTimerTimeout)
    #     self.dispTimer.start(100)

    # @pyqtSlot()
    # def onDispTimerTimeout(self):
    #     ret, frame = self.cap.read()
    #     image = self.cvToQtIm(frame)
    #     pixmap = QPixmap.fromImage(image)
    #     self.streamDisp.setPixmap(pixmap)

    def initUI(self):
        self.setGeometry(300, 300, 300, 300)
        self.setWindowTitle('Diag Tool')

        statusBar = QStatusBar(self)
        self.targetInfo = QLabel(self)
        self.connectionStatus = QLabel(self)
        statusBar.addWidget(self.targetInfo, 0)
        statusBar.addWidget(self.connectionStatus, 1)
        self.setStatusBar(statusBar)

        self.streamDisp = QLabel(self)
        self.streamDisp.setSizePolicy(QSizePolicy.MinimumExpanding,
                                      QSizePolicy.MinimumExpanding)

        layout = QGridLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(self.streamDisp, 0, 0, 0, 4)
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
                self.streamDisp.setPixmap(pixmap)

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