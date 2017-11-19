import cv2
from PyQt5.QtWidgets import QPushButton
from PyQt5.QtGui import QPixmap, QImage, QPalette


def cvToQtIm(cvIm):
    cvIm = cv2.cvtColor(cvIm, cv2.COLOR_BGR2RGB)
    return QImage(cvIm.data, cvIm.shape[1], cvIm.shape[0], QImage.Format_RGB888)

def cvToQtPixmap(cvIm):
    qtIm = cvToQtIm(cvIm)
    return QPixmap.fromImage(qtIm)

def setButtonColor(button, color):
    palette = button.palette()
    palette.setColor(QPalette.Button, color)
    button.setPalette(palette)