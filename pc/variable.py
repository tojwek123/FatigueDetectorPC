from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

nextColor = 0


class Variable(QObject):

    Colors = [Qt.blue, Qt.red, Qt.darkGreen, Qt.darkYellow, Qt.darkCyan]
    colorChanged = pyqtSignal(QColor)

    def __init__(self, name, row, valueStr='', varType='', rdOnly=False, plot=False, color=QColor(), desc='', parent=None):
        super().__init__(parent)

        self.name = name
        self.row = row
        self.valueStr = valueStr
        self.varType = varType
        self.rdOnly = rdOnly
        self.plot = plot
        self.color = color
        self.desc = desc

    def nameTableItem(self):
        tableItem = QTableWidgetItem(self.name)
        tableItem.setTextAlignment(Qt.AlignCenter)
        return tableItem

    def valueTableItem(self):
        tableItem = QTableWidgetItem(self.value)
        tableItem.setTextAlignment(Qt.AlignCenter)
        return tableItem

    def varTypeTableItem(self):
        tableItem = QTableWidgetItem(self.varType)
        tableItem.setTextAlignment(Qt.AlignCenter)
        return tableItem

    def rdOnlyTableItem(self):
        tableItem = QTableWidgetItem(str(self.rdOnly))
        tableItem.setTextAlignment(Qt.AlignCenter)
        return tableItem

    def plotTableWidget(self):
        layout = QVBoxLayout()
        checkbox = QCheckBox()
        if self.varType != 'float' and self.varType != 'int':
            checkbox.setEnabled(False)
            checkbox.setVisible(False)
        layout.addWidget(checkbox)
        layout.setAlignment(Qt.AlignCenter)
        layout.setContentsMargins(0, 0, 0, 0)
        tableWidget = QWidget()
        tableWidget.setLayout(layout)
        return (tableWidget, checkbox.stateChanged)

    def plotColorTableWidget(self):
        global nextColor

        if self.varType == 'float' or self.varType == 'int':
            layout = QVBoxLayout()
            button = QPushButton()
            button.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
            palette = button.palette()
            palette.setColor(QPalette.Button, self.Colors[nextColor])
            button.setPalette(palette)
            nextColor = (nextColor + 1) % len(self.Colors)

            def onButtonClicked(clicked):
                color = QColorDialog.getColor()
                palette = onButtonClicked.button.palette()
                if color.isValid():
                    palette.setColor(QPalette.Button, color)
                    onButtonClicked.button.setPalette(palette)
                    onButtonClicked.signal.emit(color)
            onButtonClicked.button = button
            onButtonClicked.signal = self.colorChanged

            button.clicked.connect(onButtonClicked)
            layout.addWidget(button)
            layout.setAlignment(Qt.AlignCenter)
            layout.setContentsMargins(3, 3, 3, 3)
            tableWidget = QWidget()
            tableWidget.setLayout(layout)
        else:
            tableWidget = QWidget()
        return (tableWidget, self.colorChanged)

    def descTableItem(self):
        tableItem = QTableWidgetItem(self.desc)
        tableItem.setTextAlignment(Qt.AlignCenter)
        return tableItem