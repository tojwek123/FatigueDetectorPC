#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <QVector>
#include <QString>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/opencv.h>
#include "config.h"

using cv::Point;

class FaceDetector
{
private:
    dlib::frontal_face_detector m_detector;
    dlib::shape_predictor m_predictor;

public:
    FaceDetector();
    void loadPredictorData(const QString &predictorFile);
    QVector<QVector<Point>> detect(const cv::Mat &im);
};

#endif // FACEDETECTOR_H
