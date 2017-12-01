#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSettings>
#include <QDir>
#include <QDebug>

class Settings : private QSettings
{
private:
    int m_videoStreamJpegCompression;
    int m_tcpPort;
    QString m_cascadeClassifierDataPath;
    QString m_predictorDataPath;

    static const QString PathVideoStreamJpegCompression;
    static const QString PathTcpPort;
    static const QString PathCascadeClassifierDataPath;
    static const QString PathPredictorDataPath;

    static const int DefVideoStreamJpegCompression = 20;
    static const int DefTcpPort = 6666;
    static const QString DefCascadeClassifierDataPath;
    static const QString DefPredictorDataPath;

public:
    Settings();
    void loadFromFile();

    int videoStreamJpegCompression();
    int tcpPort();
    QString cascadeClassifierDataPath();
    QString predictorDataPath();

    void saveVideoStreamJpegCompression(const int value);
    void saveTcpPort(const int value);
    void saveCascadeClassifierDataPath(const QString &value);
    void savePredictorDataPath(const QString &value);
};

#endif // SETTINGS_H
