#ifndef FACE_H
#define FACE_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>
#include "facelandmark68.h"

using std::vector;
using cv::Point;

class Face
{    
public:
    Face() = delete;
    Face(const int faceType);
    Face(const int faceType, const vector<Point> &facePt);

    void updateFaceType(const int faceType);
    void updateFacePt(const vector<Point> &facePt);

    const vector<Point> &facePt() const;
    vector<Point> jawline() const;
    vector<Point> leftEyebrow() const;
    vector<Point> rightEyebrow() const;
    vector<Point> noseVertical() const;
    vector<Point> noseHorizontal() const;
    vector<Point> leftEye() const;
    vector<Point> rightEye() const;
    vector<Point> mouth() const;

private:
    vector<Point> m_facePt;
    int m_jawlineStartIndex;
    int m_jawlineEndIndex;
    int m_leftEyebrowStartIndex;
    int m_leftEyebrowEndIndex;
    int m_rightEyebrowStartIndex;
    int m_rightEyeBrowEndIndex;
    int m_noseVerticalStartIndex;
    int m_noseVerticalEndIndex;
    int m_noseHorizontalStartIndex;
    int m_noseHorizontalEndIndex;
    int m_leftEyeStartIndex;
    int m_leftEyeEndIndex;
    int m_rightEyeStartIndex;
    int m_rightEyeEndIndex;
    int m_mouthStartIndex;
    int m_mouthEndIndex;
};

#endif // FACE_H
