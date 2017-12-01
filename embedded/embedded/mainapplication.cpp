#include "mainapplication.h"

MainApplication::MainApplication(int &argc, char **argv) :
    QCoreApplication(argc, argv),
    m_logger(stdout, QIODevice::WriteOnly),
    m_videoStreamOn(false),
    m_varStreamOn(false)
{
    qRegisterMetaType<FatigueDetectorStat>("FatigueDetectorStat");
    qRegisterMetaType<cv::Mat>("cv::Mat");

    /* Load settings from file */
    m_settings.loadFromFile();

    connect(&m_varStreamTimer, &QTimer::timeout,
            this, &MainApplication::onVarStreamTimerTimeout);

    /* Signals from FatigueDetector */
    connect(&m_fatigueDetector, &FatigueDetector::predictorDataLoaded,
            this, &MainApplication::onDetectorDataFilesLoaded);
    connect(&m_fatigueDetector, &FatigueDetector::cameraOpened,
            this, &MainApplication::onCameraOpened);
    connect(&m_fatigueDetector, &FatigueDetector::detected,
            this, &MainApplication::onDetected);
    connect(&m_fatigueDetector, &FatigueDetector::newFrame,
            this, &MainApplication::onFatigueDetectorFrame);

    /* Signals to FatigueDetector*/
    connect(this, &MainApplication::loadDetectorDataFiles,
            &m_fatigueDetector, &FatigueDetector::loadDataFiles);
    connect(this, &MainApplication::openCamera,
            &m_fatigueDetector, &FatigueDetector::openCamera);
    connect(this, &MainApplication::detect,
            &m_fatigueDetector, &FatigueDetector::detect);

    /* Signals from RemoteDataExchanger */
    connect(&m_dataExchangerServer, &RemoteDataExchangerServer::clientStateChanged,
            this, &MainApplication::onDataExchangerClientStateChanged);
    connect(&m_dataExchangerServer, &RemoteDataExchangerServer::newRequest,
            this, &MainApplication::onDataExchangerNewRequest);


    m_perfTimer.start();
    m_dataExchangerServer.listen(m_settings.tcpPort());

    m_fatigueDetector.moveToThread(&m_fatigueDetectorThread);
    m_fatigueDetectorThread.start();
    m_logger << "Loading detector data files..." << endl;
    emit loadDetectorDataFiles(m_settings.cascadeClassifierDataPath(), m_settings.predictorDataPath());

    /* Init variables */
    Variable EAR;
    EAR.name = "EAR";
    EAR.type = "float";
    EAR.readOnly = true;
    EAR.value = 0;
    m_variables.insert(EAR.name, EAR);

    Variable avgEAR;
    avgEAR.name = "avgEAR";
    avgEAR.type = "float";
    avgEAR.readOnly = true;
    avgEAR.value = 0;
    m_variables.insert(avgEAR.name, avgEAR);

//    for (int i = 0; i < 10; ++i)
//    {
//        Variable v;
//        v.name = "var" + QString::number(i + 1);
//        v.type = "float";
//        v.readOnly = true;
//        v.value = QVariant(fmod(qrand(), 10));
//        m_variables.append(v);
//    }
}

MainApplication::~MainApplication()
{
    m_fatigueDetectorThread.quit();
    m_fatigueDetectorThread.wait();
}

void MainApplication::onVarStreamTimerTimeout()
{
    m_dataExchangerServer.sendVarStreamValue(m_variables);
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
    if (success)
    {
        m_variables["EAR"].value = stat.rawEAR;
        m_variables["avgEAR"].value = stat.avgEAR;
    }
    else
    {
        m_variables["EAR"].value = 0;
        m_variables["avgEAR"].value = 0;
    }
    emit detect();
}

void MainApplication::onDataExchangerClientStateChanged(bool connected)
{
    if (connected)
    {
        m_logger << "Client connected -> " << m_dataExchangerServer.getClientAddress().toString() \
                 << " : " << m_dataExchangerServer.getClientPort() << endl;
    }
    else
    {
        m_logger << "Client disconnected" << endl;
        m_videoStreamOn = false;
        m_varStreamOn = false;
        m_varStreamTimer.stop();
    }
}

void MainApplication::onDataExchangerNewRequest(RemoteRequest request, QVector<QVariant> args)
{
    (void)args;

    switch (request)
    {
    case RemoteRequest::ReqVarInfo:
        m_dataExchangerServer.respondVarInfo(m_variables);
        break;
    case RemoteRequest::StartVarStream:
        m_dataExchangerServer.sendVarStreamValue(m_variables);
        m_varStreamOn = true;
        m_varStreamTimer.start(VarStreamTimerPeriodMs);
        break;
    case RemoteRequest::StopVarStream:
        m_varStreamOn = false;
        m_varStreamTimer.stop();
        break;
    case RemoteRequest::StartVideoStream:
        m_videoStreamOn = true;
        break;
    case RemoteRequest::StopVideoStream:
        m_videoStreamOn = false;
        break;
    }
}

void MainApplication::onFatigueDetectorFrame(cv::Mat frame)
{
    m_rawFrame = frame;
    if (m_videoStreamOn)
    {
        m_dataExchangerServer.sendVideoStreamFrame(m_rawFrame, m_settings.videoStreamJpegCompression());
    }
}
