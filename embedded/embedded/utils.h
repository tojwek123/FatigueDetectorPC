#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <cmath>
#include <QVector>
#include <dlib/geometry/rectangle.h>

#ifdef _WIN32
    #include <Windows.h>
#endif

namespace Utils
{
    class MovAvgDbl;

    double euclDist(const cv::Point &a, const cv::Point &b);

    void drawPoly(cv::Mat &im,
                  const QVector<cv::Point> &pt,
                  const cv::Scalar &color,
                  const int thickness = 1,
                  const int lineType = 8);

    void drawPolyChain(cv::Mat &im,
                       const QVector<cv::Point> &pt,
                       const cv::Scalar &color,
                       const int thickness = 1,
                       const int lineType = 8);

    void beep(int freq, int duration);

    inline dlib::rectangle cvToDlibRect(const cv::Rect &rect) { return dlib::rectangle(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height); }

    template<typename T>
    QVector<T> sliceVect(const QVector<T> &src, int start, int end);
}

class Utils::MovAvgDbl
{
private:
    QVector<double> m_data;
    int m_winSize;

public:
    MovAvgDbl(int winSize = 1);

    void push(double item);
    double get();
};

template<typename T>
static QVector<T> Utils::sliceVect(const QVector<T> &src, int start, int end)
{
    return src.mid(start, end - start + 1);
}

#endif // UTILS_H
