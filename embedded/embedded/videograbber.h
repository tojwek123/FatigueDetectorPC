#ifndef VIDEOGRABBER_H
#define VIDEOGRABBER_H

#include "config.h"
#include <QObject>

#ifdef CFG_USE_RASPICAM
    #include <raspicam/raspicam_cv.h>
#else
    #include <opencv2/opencv.hpp>
#endif

class VideoGrabber : public QObject
{
    Q_OBJECT

private:
#ifdef CFG_USE_RASPICAM
    raspicam::RaspiCam_Cv m_grabber;
#else
    cv::VideoCapture m_grabber;
#endif

public:
    explicit VideoGrabber(QObject *parent = 0);
    bool open();
    bool grab();
    void retrieve(cv::Mat &image);
    double get(int propId);
    bool set(int propId, double value);
signals:

public slots:
};

#endif // VIDEOGRABBER_H
