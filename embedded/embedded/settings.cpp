#include "settings.h"

const QString Settings::PathVideoStreamJpegCompression = "connection/videoStreamJpegCompression";
const QString Settings::PathTcpPort = "connection/tcpPort";
const QString Settings::PathCascadeClassifierDataPath = "imProc/cascadeClassifierDataPath";
const QString Settings::PathPredictorDataPath = "imProc/predictorDataPath";

#ifdef _WIN32
    const QString Settings::DefCascadeClassifierDataPath = "C:/Dev/OpenCV/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml";
    const QString Settings::DefPredictorDataPath = "C:/Dev/dlib-19.7/resources/shape_predictor_68_face_landmarks.dat";
#else
    const QString Settings::DefCascadeClassifierDataPath = "/home/pi/FatigueDetector/haarcascade_frontalface_alt.xml";
    const QString Settings::DefPredictorDataPath = "/home/pi/FatigueDetector/shape_predictor_68_face_landmarks.dat";
#endif

Settings::Settings() :
    QSettings(QDir::currentPath() + "/config.ini", QSettings::IniFormat),
    m_videoStreamJpegCompression(DefVideoStreamJpegCompression),
    m_tcpPort(DefTcpPort),
    m_cascadeClassifierDataPath(DefCascadeClassifierDataPath),
    m_predictorDataPath(DefCascadeClassifierDataPath)
{

}


void Settings::loadFromFile()
{
    m_videoStreamJpegCompression = this->value(PathVideoStreamJpegCompression, QVariant(DefVideoStreamJpegCompression)).toInt();
    m_tcpPort = this->value(PathTcpPort, QVariant(DefTcpPort)).toInt();
    m_cascadeClassifierDataPath = this->value(PathCascadeClassifierDataPath, QVariant(DefCascadeClassifierDataPath)).toString();
    m_predictorDataPath = this->value(PathPredictorDataPath, QVariant(DefPredictorDataPath)).toString();

    /* Save also */
    this->setValue(PathVideoStreamJpegCompression, QVariant(m_videoStreamJpegCompression));
    this->setValue(PathTcpPort, QVariant(m_tcpPort));
    this->setValue(PathCascadeClassifierDataPath, QVariant(m_cascadeClassifierDataPath));
    this->setValue(PathPredictorDataPath, QVariant(m_predictorDataPath));
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
