#include "mainapplication.h"

MainApplication::MainApplication(int &argc, char **argv) :
    APPLICATION_BASE_CLASS(argc, argv),
    m_logger(stdout, QIODevice::WriteOnly)
{
    qRegisterMetaType<FatigueDetectorStat>("FatigueDetectorStat");

    /* Signals from FatigueDetector */
    connect(&m_fatigueDetector, &FatigueDetector::predictorDataLoaded,
            this, &MainApplication::onPredictorDataLoaded);
    connect(&m_fatigueDetector, &FatigueDetector::cameraOpened,
            this, &MainApplication::onCameraOpened);
    connect(&m_fatigueDetector, &FatigueDetector::detected,
            this, &MainApplication::onDetected);
#ifdef APP_TYPE_GUI
    connect(&m_fatigueDetector, &FatigueDetector::detected,
            &m_testWindow, &TestWindow::onDetected);
#endif

    /* Signals from MainApplication */
    connect(this, &MainApplication::loadPredictorData,
            &m_fatigueDetector, &FatigueDetector::loadPredictorData);
    connect(this, &MainApplication::openCamera,
            &m_fatigueDetector, &FatigueDetector::openCamera);
    connect(this, &MainApplication::detect,
            &m_fatigueDetector, &FatigueDetector::detect);

#ifdef APP_TYPE_GUI
    m_testWindow.show();
#endif

    m_perfTimer.start();

    m_fatigueDetector.moveToThread(&m_fatigueDetectorThread);
    m_fatigueDetectorThread.start();
    m_logger << "Loading predictor data..." << endl;
    emit loadPredictorData(PredictorFilename);
}

MainApplication::~MainApplication()
{
    m_fatigueDetectorThread.quit();
    m_fatigueDetectorThread.wait();
}

void MainApplication::onPredictorDataLoaded(bool success)
{
    if (success)
    {
        m_logger << "Predictor data loaded" << endl;
        m_logger << "Predictor data loaded" << endl;
        m_logger << "Opening camera..." << endl;
        emit openCamera();
    }
    else
    {
        m_logger << "Unable to load predictor data" << endl;
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
        m_logger << stat.avgEAR << endl;
    }
    emit detect();
}
