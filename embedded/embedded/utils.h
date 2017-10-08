#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>

class Utils
{
public:
    Utils() = delete;

    static void drawPoly(cv::Mat &im,
                         const std::vector<cv::Point> &pt,
                         const cv::Scalar &color,
                         const int thickness = 1,
                         const int lineType = cv::LINE_8);

    template<typename T>
    static std::vector<T> sliceVect(std::vector<T> src, int start, int end);
};

template<typename T>
static std::vector<T> Utils::sliceVect(std::vector<T> src, int start, int end)
{
    if (start < 0)
    {
        start = 0;
    }
    else if (start >= src.size())
    {
        start = src.size() - 1;
    }

    if (end < 0)
    {
        end = 0;
    }
    else if (end >= src.size())
    {
        end = src.size() - 1;
    }

    return std::vector<T>(&src[start], &src[end] + 1);
}

#endif // UTILS_H
