#ifndef FATIGUEDETECTOR_H
#define FATIGUEDETECTOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <opencv2/opencv.hpp>
#include "config.h"
#include "facedetector.h"
#include "facelandmark68.h"
#include "videograbber.h"
#include "utils.h"

struct FatigueDetectorStat;

class FatigueDetector : public QObject
{
    Q_OBJECT
private:
    FaceDetector m_faceDetector;
    VideoGrabber m_grabber;
    Utils::MovAvgDbl m_movAvg;

    double calcEAR(const QVector<cv::Point> &eye);

public:
    explicit FatigueDetector(QObject *parent = 0);

signals:
    void predictorDataLoaded(bool success);
    void cameraOpened(bool success);
    void detected(bool success, FatigueDetectorStat stat);

public slots:
    bool openCamera();
    bool loadDataFiles();
    void detect();
};

struct FatigueDetectorStat
{
    double rawEAR;
    double avgEAR;
};

#endif // FATIGUEDETECTOR_H
