#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include <QCoreApplication>
#include <QTimer>
#include <QThread>
#include <QTextStream>
#include <QElapsedTimer>
#include <QDebug>
#include "config.h"
#include "settings.h"
#include "fatiguedetector.h"
#include "remotedataexchangerserver.h"
#include "alarm.h"

class MainApplication : public QCoreApplication
{
    Q_OBJECT
private:
    RemoteDataExchangerServer m_dataExchangerServer;
    FatigueDetector *m_fatigueDetector;
    FatigueDetectorParams m_fatigueDetectorParams;
    QThread m_fatigueDetectorThread;
    QTextStream m_logger;
    QElapsedTimer m_perfTimer;
    cv::Mat m_rawFrame;
    bool m_videoStreamOn;
    bool m_varStreamOn;
    QTimer m_varStreamTimer;
    QMap<QString, Variable> m_variables;
    Settings m_settings;
    Alarm *m_alarm;

    static const int VarStreamTimerPeriodMs = 100;

public:
    explicit MainApplication(int &argc, char **argv);
    ~MainApplication();
signals:
    void loadDetectorDataFiles(const QString cascadeClassifierDataPath,
                               const QString predictorDataPath);
    void openCamera(int index = 0);
    void detect();
    void setFatigueDetectorParams(const FatigueDetectorParams);

private slots:
    void onVarStreamTimerTimeout();
    void onDetectorDataFilesLoaded(bool success);
    void onCameraOpened(bool success);
    void onDetectionFinished(FatigueDetectorStat stat);
    void onFatigueDetectorFrame(cv::Mat frame);
    void onDataExchangerClientStateChanged(bool connected);
    void onDataExchangerNewRequest(RemoteRequest request, QVector<QVariant> args);
};

#endif // MAINAPPLICATION_H
