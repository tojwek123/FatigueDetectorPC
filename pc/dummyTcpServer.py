import sys
from PyQt5.QtCore import QCoreApplication, pyqtSlot
from PyQt5.QtNetwork import QTcpServer, QTcpSocket, QHostAddress, QAbstractSocket


class MainApp(QCoreApplication):

    def __init__(self, argv):
        super().__init__(argv)
        self.server = QTcpServer(self)
        self.server.setMaxPendingConnections(1)
        self.server.newConnection.connect(self.onNewConnection)
        self.server.listen(QHostAddress.Any, 6666)

    @pyqtSlot()
    def onNewConnection(self):
        client = self.server.nextPendingConnection()
        client.disconnected.connect(self.onClientDisconnected)
        print('connected')

    @pyqtSlot()
    def onClientDisconnected(self):
        print('disconnected')


if __name__ == "__main__":
    app = MainApp(sys.argv)
    sys.exit(app.exec())