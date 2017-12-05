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

    m_fatigueDetectorParams.avgEARLimit = m_settings.avgEARLimit();
    m_fatigueDetectorParams.movAvgSize = m_settings.movAvgSize();
    m_fatigueDetector = new FatigueDetector(m_fatigueDetectorParams);

    connect(&m_varStreamTimer, &QTimer::timeout,
            this, &MainApplication::onVarStreamTimerTimeout);

    /* Signals from FatigueDetector */
    connect(m_fatigueDetector, &FatigueDetector::predictorDataLoaded,
            this, &MainApplication::onDetectorDataFilesLoaded);
    connect(m_fatigueDetector, &FatigueDetector::cameraOpened,
            this, &MainApplication::onCameraOpened);
    connect(m_fatigueDetector, &FatigueDetector::detectionFinished,
            this, &MainApplication::onDetectionFinished);
    connect(m_fatigueDetector, &FatigueDetector::newFrame,
            this, &MainApplication::onFatigueDetectorFrame);

    /* Signals to FatigueDetector*/
    connect(this, &MainApplication::loadDetectorDataFiles,
            m_fatigueDetector, &FatigueDetector::loadDataFiles);
    connect(this, &MainApplication::openCamera,
            m_fatigueDetector, &FatigueDetector::openCamera);
    connect(this, &MainApplication::detect,
            m_fatigueDetector, &FatigueDetector::detect);
    connect(this, &MainApplication::setFatigueDetectorParams,
            m_fatigueDetector, &FatigueDetector::setParams);

    /* Signals from RemoteDataExchanger */
    connect(&m_dataExchangerServer, &RemoteDataExchangerServer::clientStateChanged,
            this, &MainApplication::onDataExchangerClientStateChanged);
    connect(&m_dataExchangerServer, &RemoteDataExchangerServer::newRequest,
            this, &MainApplication::onDataExchangerNewRequest);


    m_perfTimer.start();
    m_dataExchangerServer.listen(m_settings.tcpPort());

    m_fatigueDetector->moveToThread(&m_fatigueDetectorThread);
    m_fatigueDetectorThread.start();
    m_logger << "Loading detector data files..." << endl;
    emit loadDetectorDataFiles(m_settings.cascadeClassifierDataPath(), m_settings.predictorDataPath());

    /* Init variables */
    Variable EAR;
    EAR.name = "EAR";
    EAR.type = VarType::Float;
    EAR.readOnly = true;
    EAR.value = 0;
    m_variables.insert(EAR.name, EAR);

    Variable avgEAR;
    avgEAR.name = "avgEAR";
    avgEAR.type = VarType::Float;
    avgEAR.readOnly = true;
    avgEAR.value = 0;
    m_variables.insert(avgEAR.name, avgEAR);

    Variable avgEARLimit;
    avgEARLimit.name = "avgEARLimit";
    avgEARLimit.type = VarType::Float;
    avgEARLimit.readOnly = false;
    avgEARLimit.value = m_settings.avgEARLimit();
    m_variables.insert(avgEARLimit.name, avgEARLimit);

    Variable faceDetected;
    faceDetected.name = "faceDetected";
    faceDetected.type = VarType::Bool;
    faceDetected.readOnly = true;
    faceDetected.value = false;
    m_variables.insert(faceDetected.name, faceDetected);

    Variable fatigueDetected;
    fatigueDetected.name = "fatigueDetected";
    fatigueDetected.type = VarType::Bool;
    fatigueDetected.readOnly = true;
    fatigueDetected.value = false;
    m_variables.insert(fatigueDetected.name, fatigueDetected);

    Variable faceBoundingBox;
    faceBoundingBox.name = "faceBoundingBox";
    faceBoundingBox.type = VarType::IntList;
    faceBoundingBox.readOnly = true;
    faceBoundingBox.value = QVariant::fromValue(QList<int>());
    m_variables.insert(faceBoundingBox.name, faceBoundingBox);

    Variable leftEye;
    leftEye.name = "leftEye";
    leftEye.type = VarType::IntList;
    leftEye.readOnly = true;
    leftEye.value = QVariant::fromValue(QList<int>());
    m_variables.insert(leftEye.name, leftEye);

    Variable rightEye;
    rightEye.name = "rightEye";
    rightEye.type = VarType::IntList;
    rightEye.readOnly = true;
    rightEye.value = QVariant::fromValue(QList<int>());
    m_variables.insert(rightEye.name, rightEye);
}

MainApplication::~MainApplication()
{
    m_fatigueDetectorThread.quit();
    m_fatigueDetectorThread.wait();
    delete m_fatigueDetector;
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

void MainApplication::onDetectionFinished(FatigueDetectorStat stat)
{
    m_variables["EAR"].value = stat.EAR;
    m_variables["avgEAR"].value = stat.avgEAR;
    m_variables["faceDetected"].value = stat.faceDetected;
    m_variables["fatigueDetected"].value = stat.fatigueDetected;

    QList<int> boundingBoxList = {stat.boundingBox.x,
                                  stat.boundingBox.y,
                                  stat.boundingBox.x + stat.boundingBox.width,
                                  stat.boundingBox.y + stat.boundingBox.height};
    m_variables["faceBoundingBox"].value = QVariant::fromValue(boundingBoxList);

    QList<int> leftEyeList;
    for (auto i : stat.leftEye)
    {
        leftEyeList.append(i.x);
        leftEyeList.append(i.y);
    }
    m_variables["leftEye"].value = QVariant::fromValue(leftEyeList);

    QList<int> rightEyeList;
    for (auto i : stat.rightEye)
    {
        rightEyeList.append(i.x);
        rightEyeList.append(i.y);
    }
    m_variables["rightEye"].value = QVariant::fromValue(rightEyeList);

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
    case RemoteRequest::SetVarValue:
        if (args.length() >= 2)
        {
            QString varName = args[0].toString();
            if (m_variables.contains(varName))
            {
                m_variables[varName].value = args[1];
                //Validation needed
            }
        }
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
