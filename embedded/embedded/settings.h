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
    double m_avgEARLimit;
    int m_movAvgSize;

    static const QString PathVideoStreamJpegCompression;
    static const QString PathTcpPort;
    static const QString PathCascadeClassifierDataPath;
    static const QString PathPredictorDataPath;
    static const QString PathAvgEARLimit;
    static const QString PathMovAvgSize;

    static const int DefVideoStreamJpegCompression;
    static const int DefTcpPort;
    static const QString DefCascadeClassifierDataPath;
    static const QString DefPredictorDataPath;
    static const double DefAvgEARLimit;
    static const int DefMovAvgSize;

public:
    Settings();
    void loadFromFile();

    int videoStreamJpegCompression();
    int tcpPort();
    QString cascadeClassifierDataPath();
    QString predictorDataPath();
    double avgEARLimit();
    int movAvgSize();

    void saveVideoStreamJpegCompression(const int value);
    void saveTcpPort(const int value);
    void saveCascadeClassifierDataPath(const QString &value);
    void savePredictorDataPath(const QString &value);
    void saveAvgEARLimit(const double value);
    void saveMovAvgSize(const int value);
};

#endif // SETTINGS_H
