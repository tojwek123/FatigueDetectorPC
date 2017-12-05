from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5.QtCore import pyqtSlot, pyqtSignal, QLocale
from PyQt5.QtWidgets import QDialog
from PyQt5.QtWidgets import QGridLayout
from PyQt5.QtWidgets import QLineEdit
from PyQt5.QtWidgets import QSpinBox, QDoubleSpinBox, QComboBox, QAbstractSpinBox
from PyQt5.QtWidgets import QLabel
from PyQt5.QtWidgets import QPushButton
import re


class VarValueChanger(QDialog):

    valueChanged = pyqtSignal(str, str)

    def __init__(self, parent=None):
        super().__init__(parent)

        self.setWindowFlags(self.windowFlags() & (~QtCore.Qt.WindowContextHelpButtonHint))
        self.setFixedSize(0, 0)
        self.setLocale(QLocale(QLocale.English))

        self.editedVarName = ''
        self.editedVarType = ''
        self.editedVarValStr = ''

        self.valueEditTypeFloat = QDoubleSpinBox(self)
        self.valueEditTypeFloat.setVisible(False)
        self.valueEditTypeFloat.setDecimals(3)
        self.valueEditTypeFloat.setRange(-1000000, 1000000)
        self.valueEditTypeInt = QSpinBox(self)
        self.valueEditTypeInt.setVisible(False)
        self.valueEditTypeInt.setRange(-1000000, 1000000)
        self.valueEditTypeBool = QComboBox(self)
        self.valueEditTypeBool.setVisible(False)
        self.valueEditTypeBool.addItem('True', True)
        self.valueEditTypeBool.addItem('False', False)
        self.valueEditTypeUnknown = QLineEdit(self)
        self.valueEditTypeUnknown.setVisible(False)

        self.okButton = QPushButton('OK', self)
        self.okButton.clicked.connect(self.onOkButtonClicked)
        self.cancelButton = QPushButton('Cancel', self)
        self.cancelButton.clicked.connect(self.onCancelButtonClicked)
        layout = QGridLayout(self)
        layout.addWidget(self.valueEditTypeFloat, 0, 0, 1, 2)
        layout.addWidget(self.valueEditTypeInt, 0, 0, 1, 2)
        layout.addWidget(self.valueEditTypeBool, 0, 0, 1, 2)
        layout.addWidget(self.valueEditTypeUnknown, 0, 0, 1, 2)
        layout.addWidget(self.okButton, 1, 0)
        layout.addWidget(self.cancelButton, 1, 1)
        self.setLayout(layout)

    def changeValue(self, varName, varValStr, varType):
        self.editedVarName = varName
        self.editedVarValStr = varValStr
        self.editedVarType = varType

        self.setWindowTitle(varName + ' value')
        if 'float' == varType:
            self.valueEditTypeFloat.setVisible(True)
            self.valueEditTypeFloat.setValue(float(varValStr))
        elif 'int' == varType:
            self.valueEditTypeInt.setVisible(True)
            self.valueEditTypeInt.setValue(int(varValStr))
        elif 'bool' == varType:
            self.valueEditTypeBool.setVisible(True)
            if 'true' == varValStr.lower():
                self.valueEditTypeBool.setCurrentIndex(0)
            else:
                self.valueEditTypeBool.setCurrentIndex(1)
        else:
            self.valueEditTypeUnknown.setVisible(True)
            self.valueEditTypeUnknown.setText(varValStr)

        self.show()

    @pyqtSlot(bool)
    def onOkButtonClicked(self, checked):
        varValStr = ''
        if 'float' == self.editedVarType:
            varValStr = str(self.valueEditTypeFloat.value())
        elif 'int' == self.editedVarType:
            varValStr = str(self.valueEditTypeInt.value())
        elif 'bool' == self.editedVarType:
            varValStr = self.valueEditTypeBool.currentText().lower()
        else:
            varValStr = self.valueEditTypeUnknown.text()

        if varValStr != self.editedVarValStr:
            self.valueChanged.emit(self.editedVarName, varValStr)
        self.hide()

    @pyqtSlot(bool)
    def onCancelButtonClicked(self, checked):
        self.hide()

    def hideEvent(self, event):
        self.valueEditTypeFloat.setVisible(False)
        self.valueEditTypeInt.setVisible(False)
        self.valueEditTypeBool.setVisible(False)
        self.valueEditTypeUnknown.setVisible(False)