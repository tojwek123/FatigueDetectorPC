import cv2
from PyQt5.QtGui import QPixmap, QImage


def cvToQtIm(cvIm):
    cvIm = cv2.cvtColor(cvIm, cv2.COLOR_BGR2RGB)
    return QImage(cvIm.data, cvIm.shape[1], cvIm.shape[0], QImage.Format_RGB888)

def cvToQtPixmap(cvIm):
    qtIm = cvToQtIm(cvIm)
    return QPixmap.fromImage(qtIm)