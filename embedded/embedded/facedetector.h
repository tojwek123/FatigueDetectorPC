#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/opencv.h>

using std::vector;
using std::string;
using cv::Point;

class FaceDetector
{
private:
    dlib::frontal_face_detector m_detector;
    dlib::shape_predictor m_predictor;

public:
    FaceDetector();
    void loadPredictorData(string predictorFile);
    vector<vector<Point>> detect(const cv::Mat &im);
};

#endif // FACEDETECTOR_H
