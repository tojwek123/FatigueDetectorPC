#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>

#ifdef _WIN32
    #include <Windows.h>
#endif

namespace Utils
{
    class MovAvgDbl;

    double euclDist(const cv::Point &a, const cv::Point &b);

    void drawPoly(cv::Mat &im,
                  const std::vector<cv::Point> &pt,
                  const cv::Scalar &color,
                  const int thickness = 1,
                  const int lineType = cv::LINE_8);

    void drawPolyChain(cv::Mat &im,
                       const std::vector<cv::Point> &pt,
                       const cv::Scalar &color,
                       const int thickness = 1,
                       const int lineType = cv::LINE_8);

    void beep(int freq, int duration);

    template<typename T>
    std::vector<T> sliceVect(std::vector<T> src, int start, int end);
}

class Utils::MovAvgDbl
{
private:
    std::vector<double> m_data;
    int m_winSize;

public:
    MovAvgDbl();
    MovAvgDbl(int winSize);

    void push(double item);
    double get();
};

//class Utils
//{
//public:
//    Utils() = delete;

//    static double euclDist(const cv::Point &a, const cv::Point &b);

//    static void drawPoly(cv::Mat &im,
//                         const std::vector<cv::Point> &pt,
//                         const cv::Scalar &color,
//                         const int thickness = 1,
//                         const int lineType = cv::LINE_8);

//    static void drawPolyChain(cv::Mat &im,
//                              const std::vector<cv::Point> &pt,
//                              const cv::Scalar &color,
//                              const int thickness = 1,
//                              const int lineType = cv::LINE_8);

//    static void beep(int freq, int duration);

//    template<typename T>
//    static std::vector<T> sliceVect(std::vector<T> src, int start, int end);
//};

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
