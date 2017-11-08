#include "facedetector.h"

FaceDetector::FaceDetector()
{
    //m_detector = dlib::get_frontal_face_detector();
    if (!m_detector.load("/home/pi/FatigueDetector/haarcascade_frontalface_alt.xml"))//"C:\\Dev\\OpenCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml"))
    {
        std::cout << "Zesralo sie!" << std::endl;
    }
}

void FaceDetector::loadPredictorData(const QString &predictorFile)
{
    dlib::deserialize(predictorFile.toStdString()) >> m_predictor;
}

QVector<QVector<Point>> FaceDetector::detect(const cv::Mat &im)
{
    QElapsedTimer perfTimer;

    perfTimer.start();
    QVector<QVector<Point>> faces;

    std::vector<cv::Rect> rects;
    cv::Mat imGray = im.clone();
    cv::cvtColor(imGray, imGray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(imGray, imGray);
    m_detector.detectMultiScale(imGray, rects, 1.1, 5, cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    dlib::cv_image<dlib::bgr_pixel> wrappedIm(im);
    //auto rects = m_detector(wrappedIm, 0);
    std::cout << "m_detector: " << perfTimer.elapsed() << "ms" << std::endl;

    for (auto rect : rects)
    {
        QVector<Point> face;
        auto dlibRect = dlib::rectangle(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
        auto objDetect = m_predictor(wrappedIm, dlibRect);

        for (auto i = 0u; i < objDetect.num_parts(); ++i)
        {
            auto part = objDetect.part(i);
            face.append(Point(part.x(), part.y()));
        }

        faces.append(face);
    }

    return faces;
}
