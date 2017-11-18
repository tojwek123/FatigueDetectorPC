from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtNetwork import *
from PyQt5.QtGui import *
from connectionSettings import ConnectionSettings
import pyqtgraph as pg
import numpy as np
import cv2
from remoteDataExchangerClient import RemoteDataExchangerClient
from lxml import etree

root = etree.Element('request')
child = etree.Element('variable')
child.text = 'EAR'
root.append(child)
child = etree.Element('variable')
child.text = 'tmp'
root.append(child)

data = etree.tostring(root) + b'<'
print(data)



parser = etree.XMLParser()
parser.feed(data)
root = parser.close()

print(root.find('variable'))

#
# for i in root:
#     print(i.text)


