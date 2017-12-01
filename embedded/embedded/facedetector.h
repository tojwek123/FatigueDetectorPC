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

class FaceDetector
{
private:
    cv::CascadeClassifier m_detector;

    dlib::shape_predictor m_predictor;

public:
    FaceDetector();
    bool loadDataFiles(const QString &cascadeClassifierDataPath,
                       const QString &predictorDataPath);
    QVector<QVector<cv::Point>> detect(const cv::Mat &im);
};

#endif // FACEDETECTOR_H
