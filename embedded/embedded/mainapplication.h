#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include <QTimer>
#include <QThread>
#include <QTextStream>
#include <QElapsedTimer>
#include "config.h"
#include "fatiguedetector.h"

#ifdef CFG_APP_TYPE_GUI
    #include "testwindow.h"
    #include <QPushButton>
    #include <QApplication>
    #define APPLICATION_BASE_CLASS QApplication
#else
    #include <QCoreApplication>
    #define APPLICATION_BASE_CLASS QCoreApplication
#endif

class MainApplication : public APPLICATION_BASE_CLASS
{
    Q_OBJECT
private:

#ifdef CFG_APP_TYPE_GUI
    TestWindow m_testWindow;
#endif

    FatigueDetector m_fatigueDetector;
    QThread m_fatigueDetectorThread;
    QTextStream m_logger;
    QElapsedTimer m_perfTimer;

public:
    explicit MainApplication(int &argc, char **argv);
    ~MainApplication();
signals:
    void loadDetectorDataFiles();
    void openCamera(int index = 0);
    void detect();

public slots:
    void onDetectorDataFilesLoaded(bool success);
    void onCameraOpened(bool success);
    void onDetected(bool success, FatigueDetectorStat stat);
};

#endif // MAINAPPLICATION_H
