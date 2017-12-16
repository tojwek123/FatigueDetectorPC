import os
import shutil
import datetime


class DataLogger:

    TempFilename = '.log'

    def __init__(self):
        self.tempFile = None
        self.filename = ''
        self.loggingStarted = False
        self.headerWritten = False

    def start(self):
        self.tempFile = open(self.TempFilename, 'w')
        self.loggingStarted = True

    def stop(self):
        self.loggingStarted = False
        self.headerWritten = False
        self.tempFile.close()
        if os.path.exists(self.TempFilename):
            os.remove(self.TempFilename)

    def appendData(self, data):
        if self.loggingStarted:
            if not self.headerWritten:
                self.writeHeader(data)
                self.headerWritten = True
            self.writeDataRow(data)


    def writeHeader(self, data):
        #Need empty column for timestamp
        for row in data:
            self.tempFile.write(';')
            self.tempFile.write(row['name'])
        self.tempFile.write('\n')

    def writeDataRow(self, data):
        firstRow = True
        for row in data:
            if firstRow:
                self.tempFile.write(self.getTimestamp())
                firstRow = False

            self.tempFile.write(';')
            self.tempFile.write(row['valStr'])
        self.tempFile.write('\n')

    def getTimestamp(self):
        return datetime.datetime.now().strftime("%H:%M:%S.%f")

    def clear(self):
        if self.loggingStarted:
            self.headerWritten = False
            self.tempFile.close()
            self.tempFile = open(self.TempFilename, 'w')

    def save(self, filename):
        if self.loggingStarted:
            self.tempFile.flush()
            shutil.copyfile(self.TempFilename, filename)