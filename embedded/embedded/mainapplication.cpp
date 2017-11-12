#include "mainapplication.h"

MainApplication::MainApplication(int &argc, char **argv) :
    APPLICATION_BASE_CLASS(argc, argv),
    m_logger(stdout, QIODevice::WriteOnly)
{
    qRegisterMetaType<FatigueDetectorStat>("FatigueDetectorStat");

    /* Signals from FatigueDetector */
    connect(&m_fatigueDetector, &FatigueDetector::predictorDataLoaded,
            this, &MainApplication::onDetectorDataFilesLoaded);
    connect(&m_fatigueDetector, &FatigueDetector::cameraOpened,
            this, &MainApplication::onCameraOpened);
    connect(&m_fatigueDetector, &FatigueDetector::detected,
            this, &MainApplication::onDetected);
#ifdef CFG_APP_TYPE_GUI
    connect(&m_fatigueDetector, &FatigueDetector::detected,
            &m_testWindow, &TestWindow::onDetected);
#endif

    /* Signals from MainApplication */
    connect(this, &MainApplication::loadDetectorDataFiles,
            &m_fatigueDetector, &FatigueDetector::loadDataFiles);
    connect(this, &MainApplication::openCamera,
            &m_fatigueDetector, &FatigueDetector::openCamera);
    connect(this, &MainApplication::detect,
            &m_fatigueDetector, &FatigueDetector::detect);

#ifdef CFG_APP_TYPE_GUI
    m_testWindow.show();
#endif

    m_perfTimer.start();

    m_fatigueDetector.moveToThread(&m_fatigueDetectorThread);
    m_fatigueDetectorThread.start();
    m_logger << "Loading detector data files..." << endl;
    emit loadDetectorDataFiles();
}

MainApplication::~MainApplication()
{
    m_fatigueDetectorThread.quit();
    m_fatigueDetectorThread.wait();
}

void MainApplication::onDetectorDataFilesLoaded(bool success)
{
    if (success)
    {
        m_logger << "Data files successfully loaded" << endl;
        m_logger << "Opening camera..." << endl;
        emit openCamera();
    }
    else
    {
        m_logger << "Unable to load detector data files" << endl;
    }
}

void MainApplication::onCameraOpened(bool success)
{
    if (success)
    {
        m_logger << "Camera opened" << endl;
        emit detect();
    }
    else
    {
        m_logger << "Unable to open camera" << endl;
    }
}

void MainApplication::onDetected(bool success, FatigueDetectorStat stat)
{
    m_logger << "Duration: " << m_perfTimer.restart() << "ms" << endl;

    if (success)
    {
        m_logger << "avgEAR: " << stat.avgEAR << endl;
    }
    emit detect();
}
