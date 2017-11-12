#include "facedetector.h"

FaceDetector::FaceDetector()
{
#if CFG_FACE_DETECTOR == DLIB_FRONTAL_FACE_DETECTOR
    m_detector = dlib::get_frontal_face_detector();
#endif
}

bool FaceDetector::loadDataFiles()
{
#if CFG_FACE_DETECTOR == CV_CASCADE_CLASSIFIER
    if (!m_detector.load(CFG_CV_CASCADE_CLASSIFIER_DATA_PATH))
    {
        return false;
    }
#endif

    try
    {
        dlib::deserialize(CFG_PREDICTOR_DATA_PATH) >> m_predictor;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

QVector<QVector<Point>> FaceDetector::detect(const cv::Mat &im)
{
    QVector<QVector<Point>> faces;
    dlib::cv_image<dlib::bgr_pixel> wrappedIm(im);

#if CFG_FACE_DETECTOR == CV_CASCADE_CLASSIFIER
    std::vector<cv::Rect> cvRects;
    std::vector<dlib::rectangle> rects;
    cv::Mat imGray = im.clone();
    cv::cvtColor(imGray, imGray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(imGray, imGray);
    m_detector.detectMultiScale(imGray, cvRects, 1.1, 5, cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    for (auto i : cvRects) { rects.push_back(Utils::cvToDlibRect(i)); }
#elif CFG_FACE_DETECTOR == DLIB_FRONTAL_FACE_DETECTOR
    auto rects = m_detector(wrappedIm, 0);
#endif

    for (auto rect : rects)
    {
        QVector<Point> face;
        auto objDetect = m_predictor(wrappedIm, rect);

        for (auto i = 0u; i < objDetect.num_parts(); ++i)
        {
            auto part = objDetect.part(i);
            face.append(Point(part.x(), part.y()));
        }

        faces.append(face);
    }

    return faces;
}
