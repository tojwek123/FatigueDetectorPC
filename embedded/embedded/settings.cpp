#include "settings.h"

const QString Settings::PathVideoStreamJpegCompression = "connection/videoStreamJpegCompression";
const QString Settings::PathTcpPort = "connection/tcpPort";
const QString Settings::PathCascadeClassifierDataPath = "imProc/cascadeClassifierDataPath";
const QString Settings::PathPredictorDataPath = "imProc/predictorDataPath";
const QString Settings::PathAvgEARLimit = "fatigueDetector/avgEARLimit";
const QString Settings::PathMovAvgSize = "fatigueDetector/movAvgSize";
const QString Settings::PathAlarmPriorityLowTimeMs = "alarm/lowPriorityTimeMs";
const QString Settings::PathAlarmPriorityMediumTimeMs = "alarm/mediumPriorityTimeMs";
const QString Settings::PathAlarmPriorityHighTimeMs = "alarm/highPriorityTimeMs";

const int Settings::DefVideoStreamJpegCompression = 20;
const int Settings::DefTcpPort = 6666;
#ifdef _WIN32
    const QString Settings::DefCascadeClassifierDataPath = "C:/Dev/OpenCV/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml";
    const QString Settings::DefPredictorDataPath = "C:/Dev/dlib-19.7/resources/shape_predictor_68_face_landmarks.dat";
#else
    const QString Settings::DefCascadeClassifierDataPath = "/home/pi/FatigueDetector/haarcascade_frontalface_alt.xml";
    const QString Settings::DefPredictorDataPath = "/home/pi/FatigueDetector/shape_predictor_68_face_landmarks.dat";
#endif
const double Settings::DefAvgEARLimit = 0.25;
const int Settings::DefMovAvgSize = 5;
const int Settings::DefAlarmPriorityLowTimeMs = 1000;
const int Settings::DefAlarmPriorityMediumTimeMs = 2000;
const int Settings::DefAlarmPriorityHighTimeMs = 3000;

Settings::Settings() :
    QSettings(QDir::currentPath() + "/config.ini", QSettings::IniFormat),
    m_videoStreamJpegCompression(DefVideoStreamJpegCompression),
    m_tcpPort(DefTcpPort),
    m_cascadeClassifierDataPath(DefCascadeClassifierDataPath),
    m_predictorDataPath(DefCascadeClassifierDataPath),
    m_avgEARLimit(DefAvgEARLimit),
    m_movAvgSize(DefMovAvgSize),
    m_alarmPriorityLowTimeMs(DefAlarmPriorityLowTimeMs),
    m_alarmPriorityMediumTimeMs(DefAlarmPriorityMediumTimeMs),
    m_alarmPriorityHighTimeMs(DefAlarmPriorityHighTimeMs)
{

}


void Settings::loadFromFile()
{
    m_videoStreamJpegCompression = this->value(PathVideoStreamJpegCompression, QVariant(DefVideoStreamJpegCompression)).toInt();
    m_tcpPort = this->value(PathTcpPort, QVariant(DefTcpPort)).toInt();
    m_cascadeClassifierDataPath = this->value(PathCascadeClassifierDataPath, QVariant(DefCascadeClassifierDataPath)).toString();
    m_predictorDataPath = this->value(PathPredictorDataPath, QVariant(DefPredictorDataPath)).toString();
    m_avgEARLimit = this->value(PathAvgEARLimit, QVariant(DefAvgEARLimit)).toDouble();
    m_movAvgSize = this->value(PathMovAvgSize, QVariant(DefMovAvgSize)).toInt();
    m_alarmPriorityLowTimeMs = this->value(PathAlarmPriorityLowTimeMs, QVariant(DefAlarmPriorityLowTimeMs)).toInt();
    m_alarmPriorityMediumTimeMs = this->value(PathAlarmPriorityMediumTimeMs, QVariant(DefAlarmPriorityMediumTimeMs)).toInt();
    m_alarmPriorityHighTimeMs = this->value(PathAlarmPriorityHighTimeMs, QVariant(DefAlarmPriorityHighTimeMs)).toInt();

    /* Save also */
    this->setValue(PathVideoStreamJpegCompression, QVariant(m_videoStreamJpegCompression));
    this->setValue(PathTcpPort, QVariant(m_tcpPort));
    this->setValue(PathCascadeClassifierDataPath, QVariant(m_cascadeClassifierDataPath));
    this->setValue(PathPredictorDataPath, QVariant(m_predictorDataPath));
    this->setValue(PathAvgEARLimit, QVariant(m_avgEARLimit));
    this->setValue(PathMovAvgSize, QVariant(m_movAvgSize));
    this->setValue(PathAlarmPriorityLowTimeMs, QVariant(m_alarmPriorityLowTimeMs));
    this->setValue(PathAlarmPriorityMediumTimeMs, QVariant(m_alarmPriorityMediumTimeMs));
    this->setValue(PathAlarmPriorityHighTimeMs, QVariant(m_alarmPriorityHighTimeMs));
}

int Settings::videoStreamJpegCompression()
{
    return m_videoStreamJpegCompression;
}

int Settings::tcpPort()
{
    return m_tcpPort;
}

QString Settings::cascadeClassifierDataPath()
{
    return m_cascadeClassifierDataPath;
}

QString Settings::predictorDataPath()
{
    return m_predictorDataPath;
}

double Settings::avgEARLimit()
{
    return m_avgEARLimit;
}

int Settings::movAvgSize()
{
    return m_movAvgSize;
}

int Settings::alarmPriorityLowTimeMs()
{
    return m_alarmPriorityLowTimeMs;
}

int Settings::alarmPriorityMediumTimeMs()
{
    return m_alarmPriorityMediumTimeMs;
}

int Settings::alarmPriorityHighTimeMs()
{
    return m_alarmPriorityHighTimeMs;
}

void Settings::saveVideoStreamJpegCompression(const int value)
{
    m_videoStreamJpegCompression = value;
    this->setValue(PathVideoStreamJpegCompression, QVariant(value));
}

void Settings::saveTcpPort(const int value)
{
    m_tcpPort = value;
    this->setValue(PathTcpPort, QVariant(value));
}

void Settings::saveCascadeClassifierDataPath(const QString &value)
{
    m_cascadeClassifierDataPath = value;
    this->setValue(PathCascadeClassifierDataPath, QVariant(value));
}

void Settings::savePredictorDataPath(const QString &value)
{
    m_predictorDataPath = value;
    this->setValue(PathPredictorDataPath, QVariant(value));
}

void Settings::saveAvgEARLimit(const double value)
{
    m_avgEARLimit = value;
    this->setValue(PathAvgEARLimit, QVariant(value));
}

void Settings::saveMovAvgSize(const int value)
{
    m_movAvgSize = value;
    this->setValue(PathMovAvgSize, QVariant(value));
}

void Settings::saveAlarmPriorityMediumTimeMs(const int value)
{
    m_alarmPriorityMediumTimeMs = value;
    this->setValue(PathAlarmPriorityMediumTimeMs, QVariant(value));
}

void Settings::saveAlarmPriorityHighTimeMs(const int value)
{
    m_alarmPriorityHighTimeMs = value;
    this->setValue(PathAlarmPriorityHighTimeMs, QVariant(value));
}
