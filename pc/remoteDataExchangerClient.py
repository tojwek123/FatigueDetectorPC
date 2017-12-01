from PyQt5.QtCore import *
from PyQt5.QtNetwork import *
import numpy as np
import cv2


class RemoteDataExchangerClient(QObject):

    stateChanged = pyqtSignal(int)
    varInfoRead = pyqtSignal(list)
    varValueRead = pyqtSignal(list)
    camFrameRead = pyqtSignal(np.ndarray)

    TokenSeparator = ' '
    VarTokenSeparator = ','
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
                    self.headerTokens = line.split(self.TokenSeparator)
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
            if 'varInfo' == headerTokens[0]:
                varInfo = []
                dataTokens = data.decode().split(self.TokenSeparator)

                for token in dataTokens:
                    varTokens = token.split(self.VarTokenSeparator)

                    if len(varTokens) >= 4:
                        name = varTokens[0];
                        type = varTokens[1]
                        readOnly = (varTokens[2] == 'r')
                        valStr = varTokens[3]
                        varInfo.append({'name': name, 'type': type, 'readOnly': readOnly, 'valStr': valStr})

                self.varInfoRead.emit(varInfo)
            elif 'varStreamValue' == headerTokens[0]:
                varValue = []
                dataTokens = data.decode().split(self.TokenSeparator)

                for token in dataTokens:
                    varTokens = token.split(self.VarTokenSeparator)

                    if len(varTokens) >= 2:
                        name = varTokens[0];
                        valStr = varTokens[1];
                        varValue.append({'name': name, 'valStr': valStr})

                self.varValueRead.emit(varValue);

            elif 'videoStreamFrame' == headerTokens[0]:
                arr = np.frombuffer(data, dtype=np.uint8)
                cvIm = cv2.imdecode(arr, cv2.IMREAD_COLOR)
                self.camFrameRead.emit(cvIm)

    def send(self, header, data=''):
        header = header + self.TokenSeparator + str(len(data)) + '\n'
        self.socket.writeData(header.encode())
        self.socket.writeData(data.encode())

    def startVarStream(self):
        self.send('startVarStream')
        
    def stopVarStream(self):
        self.send('stopVarStream')

    def requestVarInfo(self):
        self.send('reqVarInfo')

    def startVideoStream(self):
        self.send('startVideoStream')

    def stopVideoStream(self):
        self.send('stopVideoStream')

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