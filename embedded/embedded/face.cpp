#include "face.h"

Face::Face(const int faceType)
{
    updateFaceType(faceType);
}

Face::Face(const int faceType, const vector<Point> &facePt)
{
    updateFaceType(faceType);
    updateFacePt(facePt);
}

void Face::updateFaceType(const int faceType)
{
    if (FACELANDMARK68 == faceType)
    {
        m_jawlineStartIndex = FL68_JAWLINE_START_INDEX;
        m_jawlineEndIndex = FL68_JAWLINE_END_INDEX;
        m_leftEyebrowStartIndex = FL68_LEFT_EYEBROW_START_INDEX;
        m_leftEyebrowEndIndex = FL68_LEFT_EYEBROW_END_INDEX;
        m_rightEyebrowStartIndex = FL68_RIGHT_EYEBROW_START_INDEX;
        m_rightEyeBrowEndIndex = FL68_RIGHT_EYEBROW_END_INDEX;
        m_noseVerticalStartIndex = FL68_NOSE_VERTICAL_START_INDEX;
        m_noseVerticalEndIndex = FL68_NOSE_VERTICAL_END_INDEX;
        m_noseHorizontalStartIndex = FL68_NOSE_HORIZONTAL_START_INDEX;
        m_noseHorizontalEndIndex = FL68_NOSE_HORIZONTAL_END_INDEX;
        m_leftEyeStartIndex = FL68_LEFT_EYE_START_INDEX;
        m_leftEyeEndIndex = FL68_LEFT_EYE_END_INDEX;
        m_rightEyeStartIndex = FL68_RIGHT_EYE_START_INDEX;
        m_rightEyeEndIndex = FL68_RIGHT_EYE_END_INDEX;
        m_mouthStartIndex = FL68_MOUTH_START_INDEX;
        m_mouthEndIndex = FL68_MOUTH_END_INDEX;
    }
}

void Face::updateFacePt(const vector<Point> &facePt)
{
    m_facePt = facePt;
}

const vector<Point> &Face::facePt() const
{
    return m_facePt;
}

vector<Point> Face::jawline() const
{
    return vector<Point>(&m_facePt[m_jawlineStartIndex], &m_facePt[m_jawlineEndIndex]);
}

//vector<Point> Face::leftEyebrow() const
//{

//}

//vector<Point> Face::rightEyebrow() const;
//vector<Point> Face::noseVertical() const;
//vector<Point> Face::noseHorizontal() const;
//vector<Point> Face::leftEye() const;
//vector<Point> Face::rightEye() const;
//vector<Point> Face::mouth() const;
