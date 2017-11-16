import sys
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QStyleFactory
from mainWindow import MainWindow

if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setStyle('Fusion')
    mainWindow = MainWindow()
    mainWindow.show()
    sys.exit(app.exec())