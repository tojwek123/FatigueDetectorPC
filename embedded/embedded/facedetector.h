#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <QVector>
#include <QString>
#include <QElapsedTimer>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/opencv.h>
#include "utils.h"
#include "config.h"

using cv::Point;

class FaceDetector
{
private:
#if CFG_FACE_DETECTOR == CV_CASCADE_CLASSIFIER
    cv::CascadeClassifier m_detector;
#elif CFG_FACE_DETECTOR == DLIB_FRONTAL_FACE_DETECTOR
    dlib::frontal_face_detector m_detector;
#endif

    dlib::shape_predictor m_predictor;

public:
    FaceDetector();
    bool loadDataFiles();
    QVector<QVector<Point>> detect(const cv::Mat &im);
};

#endif // FACEDETECTOR_H
