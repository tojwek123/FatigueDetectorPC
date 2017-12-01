#include "facedetector.h"

FaceDetector::FaceDetector()
{

}

bool FaceDetector::loadDataFiles(const QString &cascadeClassifierDataPath,
                                 const QString &predictorDataPath)
{
    if (!m_detector.load(cascadeClassifierDataPath.toStdString()))
    {
        return false;
    }

    try
    {
        dlib::deserialize(predictorDataPath.toStdString()) >> m_predictor;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

QVector<QVector<cv::Point>> FaceDetector::detect(const cv::Mat &im)
{
    cv::Mat procIm;
    im.copyTo(procIm);
    cv::pyrDown(procIm, procIm);
    cv::pyrDown(procIm, procIm);
    QVector<QVector<cv::Point>> faces;
    dlib::cv_image<dlib::bgr_pixel> wrappedIm(im);

    std::vector<cv::Rect> cvRects;
    std::vector<dlib::rectangle> rects;
    cv::cvtColor(procIm, procIm, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(procIm, procIm);
    m_detector.detectMultiScale(procIm, cvRects, 1.1, 5, cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    for (auto i : cvRects)
    {
        i.x *= 4;
        i.y *= 4;
        i.width *= 4;
        i.height *= 4;
        rects.push_back(Utils::cvToDlibRect(i));
    }
    for (auto rect : rects)
    {
        QVector<cv::Point> face;
        auto objDetect = m_predictor(wrappedIm, rect);

        for (auto i = 0u; i < objDetect.num_parts(); ++i)
        {
            auto part = objDetect.part(i);
            face.append(cv::Point(part.x(), part.y()));
        }

        faces.append(face);
    }
    return faces;
}
