#include "utils.h"

void Utils::drawPoly(cv::Mat &im,
                     const std::vector<cv::Point> &pt,
                     const cv::Scalar &color,
                     const int thickness,
                     const int lineType)
{
    cv::Point lastPt = pt[0];
    for (int i = 1; i < pt.size(); ++i)
    {
        cv::line(im, lastPt, pt[i], color, thickness, lineType);
        lastPt = pt[i];
    }
    line(im, lastPt, pt[0], color, thickness);
}
