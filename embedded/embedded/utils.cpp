#include "utils.h"

void Utils::drawPoly(cv::Mat &im,
                     const QVector<cv::Point> &pt,
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
                          const QVector<cv::Point> &pt,
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

#endif
}

double Utils::euclDist(const cv::Point &a, const cv::Point &b)
{
    cv::Point diff = a - b;
    return sqrt(diff.x * diff.x + diff.y * diff.y);
}

Utils::MovAvgDbl::MovAvgDbl(int winSize)
{
    if (winSize < 1)
    {
        m_winSize = 1;
    }
    else
    {
        m_winSize = winSize;
    }
}

void Utils::MovAvgDbl::push(double item)
{
    if (m_data.size() < m_winSize)
    {
        m_data.append(item);
    }
    else
    {
        for (int i = 0; i < m_data.size() - 1; ++i)
        {
            m_data[i] = m_data[i + 1];
        }
        m_data[m_data.size() - 1] = item;
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

void Utils::MovAvgDbl::setWinSize(int winSize)
{
    if (winSize < 1)
    {
        winSize = 1;
    }
    if (winSize < m_winSize)
    {
        m_data.resize(winSize);
    }
    m_winSize = winSize;
}
