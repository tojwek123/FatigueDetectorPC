#include "videograbber.h"

VideoGrabber::VideoGrabber(QObject *parent) : QObject(parent)
{

}

bool VideoGrabber::open()
{
#ifdef CFG_USE_RASPICAM
    return this->m_grabber.open();
#else
    return this->grabber.open(0);
#endif
}

bool VideoGrabber::grab()
{
    return this->m_grabber.grab();
}

void VideoGrabber::retrieve(cv::Mat &image)
{
    this->m_grabber.retrieve(image);
}

double VideoGrabber::get(int propId)
{
    return this->m_grabber.get(propId);
}

bool VideoGrabber::set(int propId, double value)
{
    return this->m_grabber.set(propId, value);
}
