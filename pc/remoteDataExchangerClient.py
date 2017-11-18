from PyQt5.QtCore import *
from PyQt5.QtNetwork import *
import numpy as np
import cv2


class RemoteDataExchangerClient(QObject):

    stateChanged = pyqtSignal(int)
    variableTypeRead = pyqtSignal(str, str, bool)
    variableValueRead = pyqtSignal(str, str)
    camFrameRead = pyqtSignal(np.ndarray)

    StateDisconnected = 0
    StateConnecting = 1
    StateConnected = 2
    StateDisconnecting = 3

    def __init__(self, parent=None):
        super().__init__(parent)

        self.connectionState = self.StateDisconnected
        self.socket = QTcpSocket(self)
        self.socket.stateChanged.connect(self.onSocketStateChanged)
        self.socket.readyRead.connect(self.onSocketReadyRead)
        self.connectingTimer = QTimer(self)
        self.connectingTimer.setSingleShot(True)
        self.connectingTimer.timeout.connect(self.onConnectingTimerTimeout)
        self.readHeader = True
        self.dataBytesToRead = 0
        self.headerTokens = []

    @pyqtSlot()
    def onSocketStateChanged(self):
        if QAbstractSocket.UnconnectedState == self.socket.state():
            self.connectionState = self.StateDisconnected
            self.stateChanged.emit(self.connectionState)
        elif QAbstractSocket.ConnectedState == self.socket.state():
            self.connectingTimer.stop()
            self.connectionState = self.StateConnected
            self.stateChanged.emit(self.connectionState)
        elif QAbstractSocket.ConnectingState == self.socket.state():
            self.connectionState = self.StateConnecting
            self.stateChanged.emit(self.connectionState)
        elif QAbstractSocket.ClosingState == self.socket.state():
            self.connectionState = self.StateDisconnecting
            self.stateChanged.emit(self.connectionState)

    @pyqtSlot()
    def onSocketReadyRead(self):
        while True:
            if self.readHeader:
                if self.socket.canReadLine():
                    line = self.socket.readLine()
                    self.headerTokens = line.split('|')
                    self.dataBytesToRead = int(self.headerTokens[-1])
                    self.readHeader = False
                else:
                    break
            else:
                if self.socket.bytesAvailable() >= self.dataBytesToRead:
                    data = self.socket.read(self.dataBytesToRead)
                    self.readHeader = True
                    self.parseMessage(self.headerTokens, data)
                else:
                    break

    @pyqtSlot()
    def onConnectingTimerTimeout(self):
        self.socket.abort()

    def parseMessage(self, headerTokens, data):
        if len(headerTokens) > 0:
            if 'varType' == headerTokens[0]:
                if len(headerTokens) > 1:
                    name = str(headerTokens[1], 'utf-8')
                    dataTokens = str(data, 'utf-8').split('|')
                    if len(dataTokens) > 1:
                        varType = dataTokens[0]
                        varAccess = dataTokens[1]
                        if 'r' == varAccess:
                            rdOnly = True
                        elif 'rw' == varAccess:
                            rdOnly = False
                        else:
                            return
                        self.variableTypeRead.emit(name, varType, rdOnly)
            elif 'varVal' == headerTokens[0]:
                if len(headerTokens) > 1:
                    name = str(headerTokens[1], 'utf-8')
                    value = str(data, 'utf-8')
                    self.variableValueRead.emit(name, value)
            elif 'camFrame' == headerTokens[0]:
                arr = np.frombuffer(data, dtype=np.uint8)
                cvIm = cv2.imdecode(arr, cv2.IMREAD_COLOR)
                self.camFrameRead.emit(cvIm)

    def send(self, header, data=''):
        header = header + '|' + str(len(data)) + '\n'
        self.socket.writeData(header.encode())
        self.socket.writeData(data.encode())

    def requestVariable(self, varName):
        self.send('reqVarVal|' + varName)

    def requestVariableType(self, varName):
        self.send('reqVarType|' + varName)

    def requestCamFrame(self):
        self.send('reqCamFrame')

    def connect(self, addr, port, timeoutMs=5000):
        if QAbstractSocket.UnconnectedState == self.socket.state():
            self.socket.connectToHost(QHostAddress(addr), port)
            self.connectingTimer.start(timeoutMs)

    def disconnect(self):
        if QAbstractSocket.ConnectedState == self.socket.state() or \
           QAbstractSocket.ConnectingState == self.socket.state():
           self.socket.disconnectFromHost()

    def state(self):
        return self.connectionState