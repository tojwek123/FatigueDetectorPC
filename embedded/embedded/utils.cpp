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

void Utils::drawPolyChain(cv::Mat &im,
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
}

void Utils::beep(int freq, int duration)
{
#ifdef _WIN32
    Beep(freq, duration);
#else
#error Utils::beep() not implemented for this platform
#endif
}

double Utils::euclDist(const cv::Point &a, const cv::Point &b)
{
    cv::Point diff = a - b;
    return sqrt(diff.x * diff.x + diff.y * diff.y);
}

Utils::MovAvgDbl::MovAvgDbl() :
    m_winSize(0)
{

}

Utils::MovAvgDbl::MovAvgDbl(int winSize) :
    m_winSize(winSize)
{

}

void Utils::MovAvgDbl::push(double item)
{
    if (m_data.size() < m_winSize)
    {
        m_data.push_back(item);
    }
    else
    {
        int i = 0;
        for (i = 0; i < m_data.size() - 1; ++i)
        {
            m_data[i] = m_data[i + 1];
        }
        m_data[i] = item;
    }
}

double Utils::MovAvgDbl::get()
{
    double sum = 0;
    for (auto i : m_data)
    {
        sum += i;
    }
    return sum / m_data.size();
}
