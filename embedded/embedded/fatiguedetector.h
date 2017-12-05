#ifndef FATIGUEDETECTOR_H
#define FATIGUEDETECTOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include "config.h"
#include "facedetector.h"
#include "facelandmark68.h"
#include "videograbber.h"
#include "utils.h"

struct FatigueDetectorStat
{
    bool fatigueDetected;
    bool faceDetected;
    double EAR;
    double avgEAR;
    cv::Rect boundingBox;
    QVector<cv::Point> leftEye;
    QVector<cv::Point> rightEye;

    FatigueDetectorStat() :
        fatigueDetected(0),
        faceDetected(0),
        EAR(0),
        avgEAR(0),
        boundingBox()
    {}
};

struct FatigueDetectorParams
{
    double avgEARLimit;
    int movAvgSize;

    FatigueDetectorParams() :
        avgEARLimit(0),
        movAvgSize(0)
    {}
};

class FatigueDetector : public QObject
{
    Q_OBJECT
private:
    FaceDetector m_faceDetector;
    VideoGrabber m_grabber;
    Utils::MovAvgDbl m_movAvg;
    FatigueDetectorParams m_params;

    double calcEAR(const QVector<cv::Point> &eye);

public:
    FatigueDetector() = delete;
    explicit FatigueDetector(const FatigueDetectorParams &params, QObject *parent = 0);

signals:
    void predictorDataLoaded(bool success);
    void cameraOpened(bool success);
    void detectionFinished(FatigueDetectorStat stat);
    void newFrame(cv::Mat frame);

public slots:
    bool openCamera();
    bool loadDataFiles(const QString cascadeClassifierDataPath,
                       const QString predictorDataPath);
    void setParams(const FatigueDetectorParams params);
    void detect();
};

#endif // FATIGUEDETECTOR_H
