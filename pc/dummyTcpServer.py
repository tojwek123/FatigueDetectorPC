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
        while self.client.canReadLine():
            line = self.client.readLine()[:-1]

            if 'get' == line:
                ret, frame = self.cap.read()
                data = cv2.imencode('.jpg', frame)[1]
                self.client.writeData((str(len(data)) + '\n').encode())
                self.client.writeData(data)

if __name__ == "__main__":
    app = MainApp(sys.argv)
    sys.exit(app.exec())