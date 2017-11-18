import sys
from PyQt5.QtCore import QCoreApplication, pyqtSlot
from PyQt5.QtNetwork import QTcpServer, QTcpSocket, QHostAddress, QAbstractSocket
import cv2
import numpy as np
import base64

class MainApp(QCoreApplication):

    def __init__(self, argv):
        super().__init__(argv)
        self.server = QTcpServer(self)
        self.server.setMaxPendingConnections(1)
        self.server.newConnection.connect(self.onNewConnection)
        self.server.listen(QHostAddress.Any, 6666)
        self.client = QTcpSocket(self)
        self.cap = cv2.VideoCapture(0)

        self.bytesToRead = 0
        self.readHeader = True
        self.headerTokens = []

        self.variables = {
            'EAR' : ('double', 0.0),
            'Name' : ('string', 'FatigueDetector'),
            'Iterations' : ('int', 2)
        }

    @pyqtSlot()
    def onNewConnection(self):
        self.client = self.server.nextPendingConnection()
        self.client.disconnected.connect(self.onClientDisconnected)
        self.client.readyRead.connect(self.onClientReadyRead)
        print('connected')

    @pyqtSlot()
    def onClientDisconnected(self):
        print('disconnected')

    @pyqtSlot()
    def onClientReadyRead(self):
        while True:
            if self.readHeader:
                if self.client.canReadLine():
                    line = self.client.readLine()
                    tokens = line.split('|')
                    print(b'header: ' + line)
                    self.bytesToRead = int(tokens[-1])
                    self.readHeader = False
                    self.headerTokens = tokens
                else:
                    break
            else:
                if self.client.bytesAvailable() >= self.bytesToRead:
                    data = self.client.read(self.bytesToRead)
                    print(b'data: ' + data)
                    self.readHeader = True
                    self.parseMessage(self.headerTokens, data)
                else:
                    break

    def parseMessage(self, headerTokens, data):
        if 'reqVarVal' == headerTokens[0]:
            self.send('varVal', 'EAR|' + str(5))
        elif 'reqCamFrame' == headerTokens[0]:
            ret, frame = self.cap.read()
            data = cv2.imencode('.jpg', frame)[1]
            self.sendDataEncoded('camFrame', data)

    def send(self, header, data=''):
        header = header + '|' + str(len(data)) + '\n'
        self.client.writeData(header.encode())
        self.client.writeData(data.encode())

    def sendDataEncoded(self, header, data=''):
        header = header + '|' + str(len(data)) + '\n'
        self.client.writeData(header.encode())
        self.client.writeData(data)

        # while self.client.bytesAvailable() >= self.bytesToRead:
        #     if 0 == self.bytesToRead:
        #         if self.client.canReadLine():
        #             line = self.client.readLine()
        #             tokens = line.split('|')
        #             print(b'header: ' + line)
        #             self.bytesToRead = int(tokens[-1])
        #         else:
        #             break
        #     else:
        #         data = self.client.read(self.bytesToRead)
        #         self.bytesToRead = 0
        #         print(b'data: ' + data)
        # while self.client.canReadLine():
        #     data = self.client.readLine()[:-1]
        #     data = str(data, 'utf-8')
        #     tokens = data.split('|')
        #
        #     if 'req' == tokens[0]:
        #         if 'var' == tokens[1]:
        #             varName = tokens[2]
        #             if varName in self.variables:
        #                 print(self.variables[varName])


if __name__ == '__main__':
    app = MainApp(sys.argv)
    sys.exit(app.exec())