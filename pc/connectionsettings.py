from PyQt5 import QtCore
from PyQt5.QtCore import pyqtSlot, pyqtSignal
from PyQt5.QtWidgets import QDialog
from PyQt5.QtWidgets import QGridLayout
from PyQt5.QtWidgets import QLineEdit
from PyQt5.QtWidgets import QSpinBox, QAbstractSpinBox
from PyQt5.QtWidgets import QLabel
from PyQt5.QtWidgets import QPushButton
import re


class ConnectionSettings(QDialog):

    settingsChanged = pyqtSignal(str, int)

    def __init__(self, initAddr, initPort, parent = None):
        super().__init__(parent)
        self.setWindowFlags(self.windowFlags() & (~QtCore.Qt.WindowContextHelpButtonHint))

        self.addr = initAddr
        self.port = initPort
        self.lastAddr = self.addr
        self.lastPort = self.port

        self.initUI()

    def initUI(self):
        self.setFixedSize(0, 0)
        self.setWindowTitle('Connection Settings')
        self.setModal(True)

        self.addrLabel = QLabel('Address: ', self)
        self.addrLabel.setAlignment(QtCore.Qt.AlignRight)
        self.addrLineEdit = QLineEdit(self)
        self.addrLineEdit.setText(self.addr)
        self.addrLineEdit.editingFinished.connect(self.onAddrLineEditEditingFinished)
        self.portLabel = QLabel('Port: ', self)
        self.portLabel.setAlignment(QtCore.Qt.AlignRight)
        self.portSpinBox = QSpinBox(self)
        self.portSpinBox.setButtonSymbols(QAbstractSpinBox.NoButtons)
        self.portSpinBox.setMaximum(65535)
        self.portSpinBox.setValue(self.port)
        self.okButton = QPushButton('OK', self)
        self.okButton.clicked.connect(self.onOkButtonClicked)
        self.cancelButton = QPushButton('Cancel', self)
        self.cancelButton.clicked.connect(self.onCancelButtonClicked)
        self.layout = QGridLayout(self)
        self.layout.addWidget(self.addrLabel, 0, 0)
        self.layout.addWidget(self.addrLineEdit, 0, 1, 1, 3)
        self.layout.addWidget(self.portLabel, 1, 0)
        self.layout.addWidget(self.portSpinBox, 1, 1, 1, 3)
        self.layout.addWidget(self.okButton, 2, 0, 1, 2)
        self.layout.addWidget(self.cancelButton, 2, 2, 1, 2)
        self.setLayout(self.layout)

    def setUserInputEnabled(self, enabled):
        self.addrLineEdit.setEnabled(enabled)
        self.portSpinBox.setEnabled(enabled)

    @pyqtSlot()
    def onAddrLineEditEditingFinished(self):
        match = re.match(r'\b(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\b',
                         self.addrLineEdit.text())
        if match:
            self.addr = self.addrLineEdit.text()
        else:
            self.addrLineEdit.setText(self.addr)

    @pyqtSlot(bool)
    def onOkButtonClicked(self, checked):
        self.port = self.portSpinBox.value()
        self.lastPort = self.port
        self.lastAddr = self.addr
        self.settingsChanged.emit(self.addr, self.port)
        self.hide()

    @pyqtSlot(bool)
    def onCancelButtonClicked(self, checked):
        self.port = self.lastPort
        self.addr = self.lastAddr
        self.portSpinBox.setValue(self.port)
        self.addrLineEdit.setText(self.addr)
        self.hide()